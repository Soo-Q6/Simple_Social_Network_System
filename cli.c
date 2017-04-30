#include<sys/types.h>
#include<sys/socket.h>
#include<time.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<arpa/inet.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include"cli.h"

void cli_download(const char* filename, int sockfd) {

	char recvline[MAXLINE];
	int n;
	// FILE* fp = fopen(filename, "w");
	// if (fp == NULL)
	// {
	// 	printf("open file error\n");
	// 	exit(0);
	// }
again:
	while ((n = read(sockfd, recvline, MAXLINE)) == MAXLINE)
	{
		fputs(recvline, stdout);
		//fwrite(recvline, 1, n, fp);
	}
	if (n>1)
	{
		//fputs(recvline, stdout);
		if (strcmp(recvline, "error") == 0)
		{
			printf("no such file!\n");
			remove(filename);
			return;
		}
		else
			fputs(recvline, stdout);
			//fwrite(recvline, 1, n, fp);

	}
	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		printf("show error");
	else
		printf("Show Complete!\n");
	//fclose(fp);
	return;
}



void cli_upload(const char* filename, int sockfd) {
	ssize_t n;
	char buf[MAXLINE];
	const char error[6] = "error";
	printf("inputing:\n");
	n=fread(buf,1,MAXLINE,stdin);
	buf[n]='\0';
	printf("%s\n",buf);
	write(sockfd, buf, strlen(buf));	
	return;
}


void cli_ls(int sockfd) {
	char recvline[100] = { '\0' };
	int n = 100;
	for (; n == 100;)
	{
		n = read(sockfd, recvline, 100);
		if (n == 100)
		{
			printf("%s\t", recvline);
		}
		else if (n == 2)
		{
			printf("cannot open this direactory!\n");
		}
		else
		{
			printf("\n");
		}
	}
	return;
}

int cli_Iscmd(char cmd[10]) {
	if (!strcmp(cmd, "cd") || !strcmp(cmd, "mkdir") || !strcmp(cmd, "show") || !strcmp(cmd, "po"))
		return 1;
	else
		return 0;
}

void cli_cmd_Up(int sockfd, char str[10], char strname[20]) {
	if (strcmp(str, "show") == 0)
	{
		printf("%s %s\n", str, strname);
		send(sockfd, strname, 20, 0);
		cli_download(strname, sockfd);
		return;

	}
	else if (strcmp(str, "po") == 0)
	{
		printf("%s %s\n", str, strname);
		send(sockfd, strname, 20, 0);
		cli_upload(strname, sockfd);
		return;
	}
	else if (strcmp(str, "mkdir") == 0)
	{
		printf("%s %s\n", str, strname);
		send(sockfd, strname, 20, 0);
		printf("create dir %s successfuly\n", strname);
		return;
	}
	else
	{
		printf("error\n");
	}
}