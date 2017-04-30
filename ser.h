#ifndef SERVER_H
#define SERVER_H

#define LISTENQ 1024
#define MAXLINE 1024
#define PATH_LENGTH 100
/**
@server
get the file and send it to connected client
*/
void ser_download(const char* filename, int sockfd);
/**
@server
read the file from the socket and fwrite to the file
*/
void ser_upload(const char* filename, int sockdf);
/**
@server
signal handling
*/
void ser_sig_chid(int signo);
/**
@server
display the files of the current path from server,
send the contents to client.
*/
void ser_ls(char* path, int connfd);
/**
@server
change the current path
*/
char* ser_changedir(const char* path);
/**
@server
to find whether cmd is a right commander or not
include cd, download, upload, mkdir
*/
int ser_Iscmd(char cmd[10]);
/**
@server
if a commander need a object to handle
include cd, download, upload, mkdir
*/
void ser_cmd_Up(int connfd, char str[10], char strname[20], char* path);
#endif // !SERVER_H

