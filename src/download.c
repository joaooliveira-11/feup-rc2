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
    printf("Connected to server.\n");
    return socket_fd;

}

int request(int socket, char *target){

    //write the target to the socket
    if(write(socket, target, strlen(target)) < 0){
        perror("Error writing to socket");
        return -1;
    }
    printf("Request sent to server.\n");
    return 0;
}

int request_answer(int socket, char *target){

    char byte;
    int index = 0, responseCode;
    state_t state = START;
    memset(target, 0, MAX_LENGTH);

    while (state != END) {
        if (read(socket, &byte, 1) <= 0) {
            perror("read");
            return -1;
        }

        if (index >= MAX_LENGTH - 1) {
            fprintf(stderr, "Server response too long\n");
            return -1;
        }

        switch (state) {
            case START:
                if (byte == ' ') state = SINGLE;
                else if (byte == '-') state = MULTIPLE;
                else if (byte == '\n') state = END;
                else target[index++] = byte;
                break;
            case SINGLE:
                if (byte == '\n') state = END;
                else target[index++] = byte;
                break;
            case MULTIPLE:
                if (byte == '\n') {
                    memset(target, 0, MAX_LENGTH);
                    state = START;
                    index = 0;
                }
                else target[index++] = byte;
                break;
            case END:
                break;
            default:
                break;
        }
    }

    target[index] = '\0'; // Null-terminate the string

    if (sscanf(target, RESPONSE, &responseCode) != 1) {
        fprintf(stderr, "Failed to parse server response\n");
        return -1;
    }

    return responseCode;
}

int login(int socket, char *user, char *password){
    char userRequest[strlen(user) + 6];
    char passwordRequest[strlen(password) + 6];

    strcpy(userRequest, "user ");
    strcat(userRequest, user);
    strcat(userRequest, "\n");

    write(socket, userRequest, strlen(userRequest));
    char answer[MAX_LENGTH];
    int readypass = request_answer(socket, answer);
    if(readypass != READY_PASS){
        printf("Unexpected response from the server. Expected %d but received %d.\n",READY_PASS,readypass);
        return -1;
    }

    strcpy(passwordRequest, "pass ");
    strcat(passwordRequest, password);
    strcat(passwordRequest, "\n");

    write(socket, passwordRequest, strlen(passwordRequest));
    char answer1[MAX_LENGTH];
    int login = request_answer(socket, answer1);
    if(login != LOGIN_SUCCESS){
        printf("Unexpected response from the server. Expected %d but received %d.\n", LOGIN_SUCCESS, login);
        return -1;
    }
    printf("Logged in to server.\n");
    return 0;
}

int passive_mode(int socket, char *ip, int *port){
    char pasvRequest[] = "pasv\n";
    write(socket, pasvRequest, 5);
    char answer[MAX_LENGTH];
    int passive =  request_answer(socket, answer);
    printf("Answer: %d\n", passive);
    if(passive != SERVER_PASSIVE){
        printf("Unexpected response from the server. Expected %d but received %d.\n", SERVER_PASSIVE, passive);
        return -1;
    }

    int ip1, ip2, ip3, ip4, p1, p2;
    sscanf(answer, PASSIVE, &ip1, &ip2, &ip3, &ip4, &p1, &p2);
    sprintf(ip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
    *port = p1 * 256 + p2;
    printf("IP: %s\nPort: %d\n", ip, *port);
    return 0;
}


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

    char server_ready[MAX_LENGTH];
    if(request_answer(socket_fd, server_ready) != READY_AUTH){
        printf("Error server not ready.\n");
        return -1;
    }

    //login to server
    if (login(socket_fd, url_info.user, url_info.pass) < 0){
        printf("Error logging in to FTP server.\n");
        return -1;
    }

    //passive mode
    int port;
    char ip[MAX_LENGTH];
    if (passive_mode(socket_fd, ip, &port) < 0){
        printf("Error entering passive mode.\n");
        return -1;
    }

    printf("creating data socket... \n");
    //create socket for data transfer
    int data_socket = create_socket(ip, port);
    if(data_socket < 0){
        printf("Error creating socket for data transfer.\n");
        return -1;
    }


    //request file
    if(request(data_socket, url_info.res) < 0){
        printf("Error requesting file.\n");
        return -1;
    }


   

    return 0;



}