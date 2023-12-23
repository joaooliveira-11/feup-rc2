#include "../include/download.h"

int parse_url(char *input, struct URL *url) {

    if (strchr(input, '/') == NULL) return -1;

    if (strchr(input, '@') == NULL) { //ftp://<host>/<url-path>
        
        sscanf(input, HOST, url->host);
        strcpy(url->user, USER_CMD);
        strcpy(url->pass, PASS_CMD);

    } else { // ftp://[<user>:<password>@]<host>/<url-path>

        sscanf(input, HOST_AT, url->host);
        sscanf(input, USER, url->user);
        sscanf(input, PASS, url->pass);
    }

    sscanf(input, RESOURCE, url->res);

    char *last_slash = strrchr(input, '/');
    if(last_slash != NULL){
        strcpy(url->file, last_slash + 1);
    }
    else{
        printf("Error: No slash found in the input string.\n");
        return -1;
    }
    
    struct hostent *h;
    if (strlen(url->host) == 0) return -1;
    if ((h = gethostbyname(url->host)) == NULL) {
        // printf("Invalid hostname '%s'\n", url->host);
        herror("gethostbyname()");
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

    //set server address info
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    //create socket
    if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Error creating socket");
        return -1;
    }

    //connect to server
    if(connect(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        perror("Error connecting to server");
        return -1;
    }

    printf("Connected to server.\n");
    return socket_fd;
}

int request_answer(int socket, char *answer){

    char byte;
    int index = 0, responseCode;
    state_t state = START;
    memset(answer, 0, MAX_LENGTH);

    while (state != END_READ) {
        if (read(socket, &byte, 1) <= 0) {
            perror("read");
            return -1;
        }

        if (index >= MAX_LENGTH - 1) {
            fprintf(stderr, "Server response too long\n");
            return -1;
        }

        switch (state) {
            case START:{
                if (byte == ' ') state = SINGLE;
                else if (byte == '-') state = MULTIPLE;
                else if (byte == '\n') state = END_READ;
                else answer[index++] = byte;
                break;
            }
            case SINGLE:{
                if (byte == '\n') state = END_READ;
                else answer[index++] = byte;
                break;
            }
            case MULTIPLE:{
                if (byte == '\n') {
                    memset(answer, 0, MAX_LENGTH);
                    state = START;
                    index = 0;
                }
                else answer[index++] = byte;
                break;
            }
            case END_READ:
                break;
            default:
                break;
        }
    }

    answer[index] = '\0'; // Null-terminate the strinG

    if (sscanf(answer, RESPONSE, &responseCode) != 1) {
        fprintf(stderr, "Failed to parse server response\n");
        return -1;
    }
    printf("Server Response: %s\n", answer);
    return responseCode;
}


/*
int request_answer(int socket, char *answer) {
    int index = 0, responseCode = 0;
    int bytesRead, totalBytesRead = 0;
    fd_set set;
    struct timeval timeout;

    memset(answer, 0, MAX_LENGTH);

    while (1) {
        FD_ZERO(&set); // clear the set
        FD_SET(socket, &set); // add our file descriptor to the set 

        timeout.tv_sec = 0;
        timeout.tv_usec = 100000; // 100 ms timeout

        int rv = select(socket + 1, &set, NULL, NULL, &timeout);
        if(rv == -1) {
            perror("select"); // an error accured 
            return -1;
        } else if(rv == 0) {
            break; // a timeout occured 
        } else {
            bytesRead = recv(socket, answer + totalBytesRead, MAX_LENGTH - totalBytesRead, 0); // there was data to read
            if (bytesRead <= 0) {
                break;
            } else {
                totalBytesRead += bytesRead;
            }
        }
    }

    answer[totalBytesRead] = '\0'; // Null-terminate the string

    if (sscanf(answer, RESPONSE, &responseCode) != 1) {
        fprintf(stderr, "Failed to parse server response\n");
        return -1;
    }

    printf("Server Response: %s\n", answer);
    return responseCode;
}
*/

int request(int socket, char *target){
    printf("Requesting file...\n");
    //write the target to the socket
    char request[strlen(target) + 6], answer[MAX_LENGTH];
    sprintf(request, "retr %s\n", target);
    if(write(socket, request, sizeof(request)) < 0){
        perror("Error writing to socket");
        return -1;
    }
    printf("Sending request: %s", request);

    int readytransf = request_answer(socket, answer);
    if(readytransf != READY_TRANSF){
        printf("Unexpected response from the server. Expected %d but received %d.\n", READY_TRANSF, readytransf);
        return -1;
    }
    
    printf("Request sent to server.\n");
    return 0;
}

int get_request(int sockfd, int sockfd2, char *target){
    printf("Receiving file...\n");

    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "downloads/%s", target);
    printf("Writing to file: %s\n", filepath);

    FILE *file = fopen(filepath, "w");
    if(file == NULL){
        perror("Error opening file");
        return -1;
    }

    char buffer[MAX_LENGTH];
    int bytes_read;
    while((bytes_read = read(sockfd2, buffer, MAX_LENGTH)) > 0){
        if(fwrite(buffer, 1, bytes_read, file) < bytes_read){
            perror("Error writing to file");
            return -1;
        }
    }

    if(bytes_read < 0){
        perror("Error reading from socket");
        return -1;
    }

    fclose(file);
    printf("File received.\n");
    return 0;
}

int login(int socket, char *user, char *password){
    char userRequest[strlen(user) + 6];
    char passwordRequest[strlen(password) + 6];
    char answer[MAX_LENGTH];

    strcpy(userRequest, "user ");
    strcat(userRequest, user);
    strcat(userRequest, "\n");

    write(socket, userRequest, strlen(userRequest));
    printf("Sending request: %s", userRequest);
    int readypass = request_answer(socket, answer);
    if(readypass != READY_PASS){
        printf("Unexpected response from the server. Expected %d but received %d.\n",READY_PASS,readypass);
        return -1;
    }

    strcpy(passwordRequest, "pass ");
    strcat(passwordRequest, password);
    strcat(passwordRequest, "\n");

    write(socket, passwordRequest, strlen(passwordRequest));
    printf("Sending request: %s", passwordRequest);
    int login = request_answer(socket, answer);
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
    printf("Sending request: %s", pasvRequest);
    char answer[MAX_LENGTH];
    int passive =  request_answer(socket, answer);
    //printf("Answer: %d\n", passive);
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

int shutdown(int sockfd, int sockfd2){
    return (close(sockfd) && close(sockfd2));
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
    if(request(socket_fd, url_info.res) < 0){
        printf("Error requesting file.\n");
        return -1;
    }

    //receive file
    if (get_request(socket_fd, data_socket, url_info.file) < 0){
        printf("Error receiving file.\n");
        return -1;
    }

    // close connection
    if (shutdown(socket_fd, data_socket) != 0) {
        printf("Sockets close error\n");
        exit(-1);
    }

    return 0;
}