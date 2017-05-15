#ifndef CLIENT_H
#define CLIENT_H

#define MAXLINE 1024
#define MAX_CONNECT_NUM 10
#define ACCOUNT_SIZE 20
#define SA struct sockaddr
#define max(a,b)  ((a)>(b)?(a):(b))

struct Login_info {
	int client;
	char account[ACCOUNT_SIZE];
	char *sin_addr;
	int sin_port;
};

/**
@client
select a file and send it to the socket
*/
void cli_upload(int sockfd);
/**
@client
download a file from server
*/
void cli_download(const char* filename, int sockfd);

/**
@client
get the content of the server's current path
*/
void cli_ls(int sockfd);
/**
@client
to find whether cmd is a right commander or not
include cd, download, upload, mkdir
*/
int cli_Iscmd(char cmd[10]);
/**
@client
if a commander need a object to handle
include cd, download, upload, mkdir
*/
void cli_cmd_Up(int sockfd, char str[10], char strname[20]);
/**
@client
show the on-line user
*/
void cli_list(int sockfd);
void str_echo(int sockfd);
void cli_connect(int confd);
void cli_connected(int confd);
#endif // !CLIENT_H

