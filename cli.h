#ifndef CLIENT_H
#define CLIENT_H

#define MAXLINE 1024
/**
@client
select a file and send it to the socket
*/
void cli_upload(const char*fp, int sockfd);
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

#endif // !CLIENT_H

