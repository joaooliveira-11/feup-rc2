#include <stdio.h>
#include <stdlib.h>
#include<arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <netinet/in.h>
#include <string.h>
#include <regex.h>
#include <termios.h>


/* Server Answers*/
#define FTP_PASS_REQUIRED_CODE       331
#define FTP_LOGIN_SUCCESS_CODE       230


int loginToFTP(int socket, char *user, char *password);