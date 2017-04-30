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

