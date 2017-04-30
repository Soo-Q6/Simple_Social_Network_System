#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include "ser.h"

#define SERV_PORT 8888
//#define MAXLINE 1024
#define LISTENQ 1024
#define ACCOUNT_SIZE 20
#define SA struct sockaddr
#define max(a,b)  ((a)>(b)?(a):(b))
struct Login_info {
	int client;
	char account[ACCOUNT_SIZE];
	char *sin_addr;
	int sin_port;
};
int main(int argc, char **argv)
{
	int					i, maxi, maxfd, listenfd, connfd, sockfd;
	int					nready;
	struct Login_info   LoginInfo[FD_SETSIZE];
	ssize_t				n;
	fd_set				rset, allset;
	char				buf[MAXLINE];
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
	char path[6] = "essay";

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

	listen(listenfd, LISTENQ);

	maxfd = listenfd;			/* initialize */
	maxi = -1;					/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++)
		LoginInfo[i].client = -1;			/* -1 indicates available entry */
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
	/* end fig01 */
    
    ser_changedir(path);
	/* include fig02 */
	for (; ; ) {
		rset = allset;		/* structure assignment */
		nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(listenfd, &rset)) {	/* new client connection */
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd, (SA *)&cliaddr, &clilen);
            if(connfd<0){
                printf("cannot accept the client's connction!\n");
                continue;
            }
#ifdef	NOTDEF
			printf("new client: %s, port %d\n",
				Inet_ntop(AF_INET, &cliaddr.sin_addr, 4, NULL),
				ntohs(cliaddr.sin_port));
#endif

			for (i = 0; i < FD_SETSIZE; i++)
				if (LoginInfo[i].client < 0) {
					LoginInfo[i].client = connfd;	/* save descriptor */
					LoginInfo[i].sin_addr = inet_ntoa(cliaddr.sin_addr);
					LoginInfo[i].sin_port = ntohs(cliaddr.sin_port);
					if (read(connfd, LoginInfo[i].account, ACCOUNT_SIZE) < 0) {
						printf("getting account number error\n");
						exit(0);
					}
					else {
						printf("%s login, its ipaddr is %s and its port is %d.\n", LoginInfo[i].account, LoginInfo[i].sin_addr, LoginInfo[i].sin_port);

					}
					break;
				}
			if (i == FD_SETSIZE) {
				printf("too many clients\n");
				exit(0);
			}


			FD_SET(connfd, &allset);	/* add new descriptor to set */
			if (connfd > maxfd)
				maxfd = connfd;			/* for select */
			if (i > maxi)
				maxi = i;				/* max index in client[] array */

			if (--nready <= 0)
				continue;				/* no more readable descriptors */
		}

		for (i = 0; i <= maxi; i++) {	/* check all clients for data */
			if ((sockfd = LoginInfo[i].client) < 0)
				continue;
			if (FD_ISSET(sockfd, &rset)) {
                printf("this%d\n",i);
					char str[10] = { '\0' };
					char strname[20] = { '\0' };
					recv(sockfd, str, 10, 0);         //获取指令 done.
                    printf("str:%s\n",str);
					if (strcmp(str, "ls") == 0)
					{
						ser_ls(path, sockfd);
					}
					else if (strcmp(str, "exit") == 0)
					{
						printf("disconnection form:%s  port:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
						close(sockfd);
                        FD_CLR(sockfd,&allset);
                        LoginInfo[i].client=-1;
						exit(0);
						break;
					}
					else if (ser_Iscmd(str))
					{
						recv(sockfd, strname, 20, 0);
                        printf("strname:%s\n",strname);
						ser_cmd_Up(sockfd, str, strname, path);
					}
					else
					{
						printf("commander wrong!\n");
					}
				//}
			}
		}
	}
}