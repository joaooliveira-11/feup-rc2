#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <regex.h>
#include <termios.h>
#include <unistd.h>

#define MAX_LENGTH  500
#define PORT        21

/* server responses */
#define READY_AUTH      220
#define READY_PASS      331
#define READY_TRANSF    150
#define LOGIN_SUCCESS   230
#define SERVER_PASSIVE  227
#define TRANSF_SUCCESS  226
#define END             221  


/* parse regex */
#define AT              "@"
#define SLASH           "/"
#define HOST            "%*[^/]//%[^/]"
#define HOST_AT         "%*[^/]//%*[^@]@%[^/]"
#define RESOURCE        "%*[^/]//%*[^/]/%s"
#define USER            "%*[^/]//%[^:/]"
#define PASS            "%*[^/]//%*[^:]:%[^@\n$]"
#define PASSIVE         "%*[^(](%d,%d,%d,%d,%d,%d)%*[^\n$)]"

/*Login */

#define USER_CMD        "anonymous"
#define PASS_CMD        "password"

/* parser */
struct URL {
    char user[MAX_LENGTH];
    char pass[MAX_LENGTH];
    char host[MAX_LENGTH];
    char res[MAX_LENGTH];
    char ip[MAX_LENGTH];
    char file[MAX_LENGTH];
};

typedef enum {
    START,
    SINGLE,
    MULTIPLE,
    ENDING
} state;


int parse_url(char *input, struct URL *output);

int create_socket(char *ip, int port);

int login(int sockfd, char *user, char *pass);

int passive_mode(int sockfd, char *ip, int *port);

int read_response(int sockfd, char *response);

int request(int sockfd, char *target);

int get_request(int sockfd, int sockfd2, char *target);

int shutdown(int sockfd, int sockfd2);

