#include "../include/download.h"


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

int main(int argc, char *argv[]) {




}