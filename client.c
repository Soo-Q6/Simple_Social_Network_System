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
#include <sys/select.h>
#include <sys/wait.h>
#include "cli.h"
//#define MAXLINE 1024
#define SERV_PORT 8888
#define CONN_PORT 9999
#define UDP_PORT  6000
#define ACCOUNT_SIZE 20
void str_cli(FILE *, int);
int main(int argc, char **argv) {
	setvbuf(stdout,NULL,_IONBF,0);
	int sockfd, udpfd, maxfd, n = 1, nready, len, connfd;
	int listenfd;
	int connectfd[MAX_CONNECT_NUM];
	char recvline[MAXLINE], sendline[MAXLINE];
	char account[ACCOUNT_SIZE];
	struct sockaddr_in servaddr,connaddr;
	fd_set rset;
	int cmax=0;
	time_t ticks;
	char str[10], str_name[20];
	int on=1;
	//char *ipaddr = "127.0.0.1";
	if(argc!=2){
		printf("open error\n");
		exit(0);
	}

	/*initial the connectfd[]*/
	int temp;
	for(temp=0;temp<MAX_CONNECT_NUM;temp++)
		connectfd[temp]=-1;

	/*a socket for connect server*/
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("socket error");
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
		printf("inet_ption error for %s", argv[1]);
	fputs("please enter your acount number:\n", stdout);
	scanf("%s", account);
	if ((n = connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0)
		printf("connect error");
	write(sockfd, account, sizeof(account));
	printf("%s login successfully!\ncmd/>", account);


	/*a socket for connent client*/
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("socket error");	
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	bzero(&connaddr, sizeof(connaddr));
	connaddr.sin_family = AF_INET;
	connaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	connaddr.sin_port = htons(CONN_PORT);

	bind(listenfd, (SA *)&connaddr, sizeof(connaddr));


	/* for create UDP socket */
    udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(UDP_PORT);
    bind(udpfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	FD_ZERO(&rset);
	maxfd=max(udpfd,max(listenfd,fileno(stdin)));

	for (;;)
    {
		FD_SET(fileno(stdin),&rset);
		FD_SET(listenfd, &rset);
		FD_SET(udpfd, &rset);
		if ((nready = select(maxfd+1, &rset, NULL, NULL, NULL)) < 0)
		{
	    	if (errno == EINTR)
			continue;
	   	 	/* back to for() */
	    	else
			printf("select error");
		}
		if (FD_ISSET(listenfd, &rset))
		{
	   	 	len = sizeof(servaddr);
	    	connfd = accept(listenfd, (struct sockaddr *)&servaddr, &len);
	    	
			str_echo(connfd);
			
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
				//printf("this is multiple select.\n");
				scanf("%s", strname);
				int connected_count;
				struct sockaddr_in tmpaddr;
				//FD_SET(fileno(stdin),&cset);
				if(!strcmp(str,"connect")){
					for(connected_count=0;connected_count<MAX_CONNECT_NUM;connected_count++)
						if(connectfd[connected_count]<0){
							if((connectfd[connected_count]=socket(AF_INET,SOCK_STREAM,0))<0){
								printf("socket error\n");
								break;
							}
							bzero(&tmpaddr,sizeof(tmpaddr));
							tmpaddr.sin_family=AF_INET;
							tmpaddr.sin_port=htons(CONN_PORT);
							if(inet_pton(AF_INET,strname,&tmpaddr.sin_addr)<=0){
								printf("inet_pton error for %s\n",strname);
							}
							if(connect(connectfd[connected_count],(SA*)&tmpaddr,sizeof(tmpaddr))<0){
								printf("connect error\n");
								break;
							}
							maxfd=max(connectfd[connected_count],maxfd);
							FD_SET(connectfd[connected_count],&rset);
						}
					write(connectfd[connected_count],sendline,sizeof(sendline));
				}else{
					cli_cmd_Up(sockfd, str, strname);
				}
				
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
		int j;
		int tmpfd;
		for(j=0;j<MAX_CONNECT_NUM;j++){
			if((tmpfd=connectfd[j])<0){
				continue;
			}
			if(FD_ISSET(tmpfd,&rset)){
				read(tmpfd,recvline,sizeof(recvline));
				printf("%s\n",recvline);
			}
		}
		printf("%s/>", "cmd");
    }
	return 0;
}




