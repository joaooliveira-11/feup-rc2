#include "../include/download.h"

int parse_url(char *input, struct URL *url) {

    regex_t regex;
    regcomp(&regex, SLASH, 0);
    if (regexec(&regex, input, 0, NULL, 0)) return -1;

    regcomp(&regex, AT, 0);
    if (regexec(&regex, input, 0, NULL, 0) != 0) { //ftp://<host>/<url-path>
        
        sscanf(input, HOST, url->host);
        strcpy(url->user, USER_CMD);
        strcpy(url->pass, PASS_CMD);

    } else { // ftp://[<user>:<password>@]<host>/<url-path>

        sscanf(input, HOST_AT, url->host);
        sscanf(input, USER, url->user);
        sscanf(input, PASS, url->pass);
    }

    sscanf(input, RESOURCE, url->res);
    strcpy(url->file, strrchr(input, '/') + 1);
    
    struct hostent *h;
    if (strlen(url->host) == 0) return -1;
    if ((h = gethostbyname(url->host)) == NULL) {
        printf("Invalid hostname '%s'\n", url->host);
        return -1;
    }
    
    strcpy(url->ip, inet_ntoa(*((struct in_addr *) h->h_addr)));
    
    return !(strlen(url->host) && strlen(url->user) && 
           strlen(url->pass) && strlen(url->res) && strlen(url->file));
}

int create_socket(char *ip, int port){
    int socket_fd;

    //to store server address info
    struct sockaddr_in server_addr;

    //create socket
    if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Error creating socket");
        return -1;
    }

    //set server address info
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    //connect to server
    if(connect(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        perror("Error connecting to server");
        return -1;
    }
    printf("Connected to server. LETSGO MEU MENINO\n");
    return socket_fd;

}

/*
int loginToFTP(int socket, char *user, char *password){
    char userRequest[strlen(user) + 6];
    char passwordRequest[strlen(password) + 6];

    strcpy(userRequest, "user ");
    strcat(userRequest, user);
    strcat(userRequest, "\n");

    write(socket, userRequest, strlen(userRequest));
    int answer = request_answer(socket);
    if(answer != READY_PASS){
        printf("Unexpected response from the server. Expected %d but received %d.\n", FTP_PASS_REQUIRED_CODE, answer);
        return -1;
    }

    strcpy(passwordRequest, "pass ");
    strcat(passwordRequest, password);
    strcat(passwordRequest, "\n");

    write(socket, passwordRequest, strlen(passwordRequest));
    int answer = request_answer(socket);
    if(answer != LOGIN_SUCCESS){
        printf("Unexpected response from the server. Expected %d but received %d.\n", FTP_LOGIN_SUCCESS_CODE, answer);
        return -1;
    }

    return 0;
}   
*/
int main(int argc, char *argv[]) {
    if(argc != 2){
        printf("Usage: ./download ftp://[<user>:<password>@]<host>/<url-path>\n");
        return -1;
    }

    struct URL url_info; //struct to store url info
    memset(&url_info, 0, sizeof(url_info)); //initialize struct

    if(parse_url(argv[1], &url_info) != 0){
        printf("Error parsing URL.\n");
        return -1;
    }

    printf("Host: %s\nResource: %s\nFile: %s\nUser: %s\nPassword: %s\nIP Address: %s\n", url_info.host, url_info.res, url_info.file, url_info.user, url_info.pass, url_info.ip);

    //descriptor for socket
    int socket_fd = create_socket(url_info.ip, PORT);
    if(socket_fd < 0){
        printf("Error creating socket.\n");
        return -1;
    }

    return 0;



}