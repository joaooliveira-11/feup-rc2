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


#define MAX_SIZE 500
#define MAX_TRIES 3
#define DELAY 200000

/* Server Answers*/
#define FTP_PASS_REQUIRED_CODE       331
#define FTP_LOGIN_SUCCESS_CODE       230


/*Regex*/

#define RESPCODE_REGEX  "%d"


int loginToFTP(int socket, char *user, char *password);

int request_answer(int socket);