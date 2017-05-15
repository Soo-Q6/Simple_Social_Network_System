#include<sys/socket.h>
#include<wait.h>
#include<string.h>
#include<unistd.h>   
#include<signal.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include<stdio.h>
#include<sys/types.h>
#include<dirent.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<errno.h>
#include"ser.h"

void ser_download(const char* filename, int sockfd) {
	FILE *fp;
	ssize_t n;
	const char error[6] = "error";
	char buf[MAXLINE];
	int childsockfd;

	if ((fp = fopen(filename, "r")) == NULL) {
		printf("cannot open file!\n");
		write(sockfd, error, sizeof(error));
		//exit(0);
		return;
	}
again:
	while ((n = fread(buf, 1, MAXLINE, fp)) > 0) {
		write(sockfd, buf, n);
	}
	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		printf("str_echo:read error");
	write(sockfd, buf, 1);
	fclose(fp);
	return;
}


void ser_upload(const char* filename, int sockfd, struct Login_info logininfo) {
	char recvline[MAXLINE];
	FILE* fp = fopen(filename, "w");
	ssize_t n;
	if (fp == NULL)
	{
		printf("open file error\n");
		exit(0);
	}
	fprintf(fp,"account:%s\taddr:%s\tport:%d",logininfo.account,logininfo.sin_addr,logininfo.sin_port);
again:
	if ((n = read(sockfd, recvline, MAXLINE)) == MAXLINE)
	{
		fwrite(recvline, 1, n, fp);
	}
	if (n>1)
	{
		if (strcmp(recvline, "error") == 0)
		{
			remove(filename);
			return;
		}
		else
			fwrite(recvline, 1, n, fp);
	}
	if (n < 0 && errno == EINTR)
		goto again;
	else if (n <= 0)
		printf("read error");
	else
		printf("Upload Complete!\n");
	fclose(fp);
	return;
}



void ser_ls(char* tmp, int connfd) {
	struct dirent* ent = NULL;
	DIR *pDir;
	char sendline[100] = { '\0' };
	while ((pDir = opendir(tmp)) == NULL)
	{
		printf("cannot open direactory.");
		write(connfd, sendline, 2);
		return;
	}
	while ((ent = readdir(pDir)) != NULL)
	{
		strcpy(sendline, ent->d_name);
		if(strcmp(sendline,".")==0||strcmp(sendline,"..")==0){
			continue;
		}
		if (write(connfd, sendline, sizeof(sendline)) < 0)
		{
			printf("write error: %s (errno:%d)", strerror(errno), errno);
			exit(0);
		}
	}
	write(connfd, sendline, 1);
	closedir(pDir);
	return;
}

int ser_Iscmd(char cmd[10])
{
	if (!strcmp(cmd, "cd") || !strcmp(cmd, "mkdir") || !strcmp(cmd, "show") || !strcmp(cmd, "po"))
		return 1;
	else
		return 0;
}

void ser_cmd_Up(int connfd, char str[10], char strname[20], struct Login_info logininfo) {
	if (strcmp(str, "show") == 0)
	{
		printf("%s %s\n", str, strname);
		ser_download(strname, connfd);
		return;

	}
	else if (strcmp(str, "po") == 0)
	{
		ser_upload(strname, connfd, logininfo);
		return;
	}
	else if (strcmp(str, "mkdir") == 0)
	{
		mkdir(strname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		return;
	}
	else
	{
		printf("error:");
		return;
	}

}

void ser_sig_chid(int signo) {
	pid_t pid;
	int stat;
	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child:%d terminated\n", pid);
	return;
}

void ser_list(int sockfd,struct Login_info *logininfo){
	char sendline[100]={'\0'};
	char tmp[7]={'\0'};
	int i1;
	for (i1 = 0; i1 < FD_SETSIZE; i1++)
				if (logininfo[i1].client > 0) {
					strcat(sendline,logininfo[i1].account);
					strcat(sendline,"  ip:");
					strcat(sendline,logininfo[i1].sin_addr);
					strcat(sendline,"  port:");
					sprintf(tmp,"%d",logininfo[i1].sin_port);
					strcat(sendline,tmp);
					write(sockfd,sendline,sizeof(sendline));
					bzero(sendline,strlen(sendline));
				}
				write(sockfd,sendline,1);
}


void ser_broadcast(int sockfd, int udpfd) {
	char recvline[MAXLINE];
	ssize_t n;
	struct sockaddr_in addrto;
    bzero(&addrto,sizeof(struct sockaddr_in));
  	addrto.sin_family=AF_INET;
    addrto.sin_addr.s_addr=htonl(INADDR_BROADCAST);
    addrto.sin_port=htons(6000);
	//inet_pton(AF_INET, "127.0.0.1", &addrto.sin_addr);
    int nlen=sizeof(addrto);
	printf("this is broadcasting\n");
again:
	if ((n = read(sockfd, recvline, MAXLINE)) == MAXLINE)
	{
		//fwrite(recvline, 1, n, fp);
		printf("broadcasting context:%s\n",recvline);
		if(sendto(udpfd, recvline, MAXLINE, 0, (struct sockaddr *)&addrto, nlen)>0){
			printf("broadcast successfully\n");
		}
		return;
	}
	else if (n>1)
	{
		if (strcmp(recvline, "error") == 0)
		{
			return;
		}
		else{
			if(sendto(udpfd, recvline, MAXLINE, 0, (struct sockaddr *)&addrto, nlen)>0){
				printf("broadcast successfully\n");
			}
			printf("broadcasting context:%s\n",recvline);
		}
	}
	if (n < 0 && errno == EINTR)
		goto again;
	else if (n <= 0)
		printf("read error");
	else
		printf("That's all!\n");
	//fclose(fp);
	return;
}