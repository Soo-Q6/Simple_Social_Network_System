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
#define UDP_PORT  6000
#define ACCOUNT_SIZE 20
void str_cli(FILE *, int);
int main(int argc, char **argv) {
	setvbuf(stdout,NULL,_IONBF,0);
	int sockfd, udpfd, maxfd, n = 1, nready, len, connfd;
	char recvline[MAXLINE], sendline[MAXLINE];
	char account[ACCOUNT_SIZE];
	struct sockaddr_in servaddr;
	fd_set rset;
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
	printf("%s login successfully!\ncmd/>", account);
	//printf("%s/>", "cmd");


	/* for create UDP socket */
    udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(UDP_PORT);
    bind(udpfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	FD_ZERO(&rset);
	maxfd=max(udpfd,max(sockfd,fileno(stdin)));

	for (;;)
    {
		FD_SET(fileno(stdin),&rset);
		FD_SET(sockfd, &rset);
		FD_SET(udpfd, &rset);
		if ((nready = select(maxfd+1, &rset, NULL, NULL, NULL)) < 0)
		{
	    	if (errno == EINTR)
			continue;
	   	 	/* back to for() */
	    	else
			printf("select error");
		}
		if (FD_ISSET(sockfd, &rset))
		{
	   	 	len = sizeof(servaddr);
	    	connfd = accept(sockfd, (struct sockaddr *)&servaddr, &len);
	    	//if ((childpid = fork()) == 0)
	    	//{
			/* child process */
			//close(listenfd);
			/* close listening socket */
			str_echo(connfd);
			/* process the request */
			//exit(0);
	    	//}
	    	close(connfd);
		}
		if (FD_ISSET(udpfd, &rset))
		{
			char mesg[MAXLINE];
	    	len = sizeof(servaddr);
	    	n = recvfrom(udpfd, mesg, MAXLINE, 0, NULL, NULL);
			mesg[n]='\0';
			printf("broadcast:\n%s\n",mesg);
		}
		if(FD_ISSET(fileno(stdin),&rset)){
			//char a[10];
			//scanf("%s",a);
		//while (1)
		//{
			//printf("%s/>", "cmd");
			char str[10];
			char strname[20];
			scanf("%s", str);
        	printf("%s str\n",str);
			send(sockfd, str, 10, 0);           //传指令 done.
			if (strcmp(str, "ls") == 0)
			{
				printf("this is ls\n");
				cli_ls(sockfd);
			}
			else if(strcmp(str,"broadcast")==0)
			{
				printf("this is broadcast\n");
				cli_upload(sockfd);
			}
			else if (cli_Iscmd(str))
			{
				printf("this is multiple select.\n");
				scanf("%s", strname);
				cli_cmd_Up(sockfd, str, strname);
			}
			else if (strcmp(str, "exit") == 0)
			{
				exit(0);
			}
			else if(strcmp(str,"list")==0)
			{
				printf("this is list\n");
				cli_list(sockfd);
			}
			else
			{
				printf("commander wrong!\n");
			}
			//}
		}
		printf("%s/>", "cmd");
    }
	return 0;
}




