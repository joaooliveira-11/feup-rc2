#include "../include/download.h"



int request_answer(int socket){
    char answer [MAX_SIZE];
    int answer_code = 0;
    int ntries_aux = MAX_TRIES;
    int bytesread = 0;

    while (ntries_aux) {
        int bytes = recv(socket, answer + bytesread, MAX_SIZE - bytesread, MSG_DONTWAIT);
        if(bytes <=0){
            ntries_aux--;
            usleep(100000);
        }
        else{
            ntries_aux = MAX_TRIES;
        }
        bytesread += bytes;
    }

    answer[bytesread] = '\0'; 

    sscanf(answer, RESPCODE_REGEX, &answer_code);
    return answer_code;
}

int loginToFTP(int socket, char *user, char *password){
    char userRequest[strlen(user) + 6];
    char passwordRequest[strlen(password) + 6];

    strcpy(userRequest, "user ");
    strcat(userRequest, user);
    strcat(userRequest, "\n");

    write(socket, userRequest, strlen(userRequest));
    int answer = request_answer(socket);
    if(answer != FTP_PASS_REQUIRED_CODE){
        printf("Unexpected response from the server. Expected %d but received %d.\n", FTP_PASS_REQUIRED_CODE, answer);
        return -1;
    }

    strcpy(passwordRequest, "pass ");
    strcat(passwordRequest, password);
    strcat(passwordRequest, "\n");

    write(socket, passwordRequest, strlen(passwordRequest));
    int answer = request_answer(socket);
    if(answer != FTP_LOGIN_SUCCESS_CODE){
        printf("Unexpected response from the server. Expected %d but received %d.\n", FTP_LOGIN_SUCCESS_CODE, answer);
        return -1;
    }

    return 0;
}   

int main(int argc, char *argv[]) {




}