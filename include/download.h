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

// For the second version of read server answer
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>

#define MAX_LENGTH  1024
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
#define HOST            "%*[^/]//%[^/]"
#define HOST_AT         "%*[^/]//%*[^@]@%[^/]"
#define RESOURCE        "%*[^/]//%*[^/]/%s"
#define USER            "%*[^/]//%[^:/]"
#define PASS            "%*[^/]//%*[^:]:%[^@\n$]"
#define PASSIVE         "%*[^(](%d,%d,%d,%d,%d,%d)%*[^\n$)]"
#define RESPONSE        "%d %*[^\n$]"

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
    END_READ,
} state_t;


int parse_url(char *input, struct URL *url);

int create_socket(char *ip, int port);

int login(int socket, char *user, char *password);

int passive_mode(int socket, char *ip, int *port);

int request_answer(int socket, char *answer);

int request(int socket, char *target);

int get_request(int sockfd, int sockfd2, char *target);

int shutdown(int sockfd, int sockfd2);