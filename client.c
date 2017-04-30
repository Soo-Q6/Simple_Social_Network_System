#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include "cli.h"
//#define MAXLINE 1024
#define SERV_PORT 8888
#define ACCOUNT_SIZE 20
void str_cli(FILE *, int);
int main(int argc, char **argv) {
	int sockfd, n = 1;
	char recvline[MAXLINE], sendline[MAXLINE];
	char account[ACCOUNT_SIZE];
	struct sockaddr_in servaddr;
	time_t ticks;
	char str[10], str_name[20];
	char *ipaddr = "127.0.0.1";


	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("socket error");
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	if (inet_pton(AF_INET, ipaddr, &servaddr.sin_addr) <= 0)
		printf("inet_ption error for %s", ipaddr);
	fputs("please enter your acount number:\n", stdout);
	scanf("%s", account);
	if ((n = connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0)
		printf("connect error");
	write(sockfd, account, sizeof(account));
	printf("%s login successfully!\n", account);


	while (1)
	{
		printf("%s/>", "cmd");
		char str[10];
		char strname[20];
		scanf("%s", str);
        printf("%s str\n",str);
		send(sockfd, str, 10, 0);           //传指令 done.
		if (strcmp(str, "ls") == 0)
		{
			cli_ls(sockfd);
		}
		else if (cli_Iscmd(str))
		{
			scanf("%s", strname);
			cli_cmd_Up(sockfd, str, strname);
		}
		else if (strcmp(str, "exit") == 0)
		{
			exit(0);
		}
		else if(strcmp(str,"list")==0)
		{
			list(sockfd);
		}
		else
		{
			printf("commander wrong!\n");
		}
	}
	return 0;

}