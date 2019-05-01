#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h> 

#define MAXMSGSIZE 500

void removeNewLine(char* string) {

    int len = strlen(string);    // remove newline at the end
    if (string[len-1] == '\n')
        string[len-1] = '\0';

}

void getHandle(char* handle) {

    int maxLen = 11;                // 10 characters and 1 newline
    memset(handle, '\0', maxLen);   // clear the array
    printf("What is your handle (max 10 characters): ");
    fgets(handle, maxLen, stdin);   // read the user input
    removeNewLine(handle);          // remove the newline

    //add "> " to the handle
    int len = strlen(handle);
    handle[len] = '>';
    handle[len+1] = ' ';

}

int setup(char** argv, struct addrinfo* hints, struct addrinfo* res) {

    int status;

    //empty struct
    memset(hints, 0, sizeof(*hints));

    // set up hints struct
    hints->ai_family = AF_UNSPEC;        // don't care IPv4 or IPv6
    hints->ai_socktype = SOCK_STREAM;    // TCP stream sockets

    if ((status = getaddrinfo(argv[1], argv[2], hints, &res)) != 0) {
        fprintf(stderr, "GetAddrInfo error: %s\n", gai_strerror(status));
        //exit 1;
    }

    //get socket descriptor
    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    // connect to host
    connect(sockfd, res->ai_addr, res->ai_addrlen);

    printf("\nStarting Chatroom ...\n\n");

    freeaddrinfo(res);

    return sockfd;

}

int checkQuit(char* msg) {

    // specifies quit keyword
    char* quit = "\\quit";

    // check if message contains "\quit"
    if(strstr(msg, quit) != NULL) {
        return 1;
    }
    return 0;
}

int sendMsg(int sockfd, char* handle) {

    //allocate buffer
    char* buffer = malloc( sizeof(char) * ( MAXMSGSIZE + 1 ));
    char* temp = buffer;

    // clear the message's memory
    memset(buffer, '\0', sizeof(buffer));

    //copy the handle into the buffer and advance buffer pointer past handle
    strcpy(buffer, handle);
    buffer += strlen(handle);

    // get the user's message
    int msgLen = MAXMSGSIZE - strlen(handle) + 1;
    printf("%s", handle);
    fgets(buffer, msgLen, stdin);           // write just beyond the handle
    removeNewLine(buffer);                  // remove the newline placed by fgets
    buffer = temp;                          // return buffer to front of array

    //send the users message
    int bytes_sent = send(sockfd, buffer, strlen(buffer), 0);

    //check if user wants to quit
    if (checkQuit(buffer)) {
        printf("\nYou have left the chatroom.\n\n");
        free(buffer);
        return 0;
    }

    // free buffer
    free(buffer);

    // return OK
    return 1;
}

int recvMsg(int sockfd) {
    
    char buffer[MAXMSGSIZE];

    // clear the buffer
    memset(buffer, '\0', sizeof(buffer));

    // receive message into buffer
    int bytes_recv = recv(sockfd, buffer, sizeof(buffer), 0);

    // check if other user is quitting
    if(checkQuit(buffer)) {
        printf("\nThe server has left that chatroom.\n\n");
        return 0;
    }

    // print the message
    printf("%s\n", buffer);

    // return OK
    return 1;
}

int main(int argc, char** argv) {

    int socketFD;                   // socket file descriptor
    struct addrinfo hints;          // used for connecting to server
    struct addrinfo *res;

    //get the users handle (10 chars for handle, 2 for "> ")
    char handle[12];
    getHandle(handle);

    //set up the TCP connection
    socketFD = setup(argv, &hints, res);

    // run on a while loop
    int sendFlag = 1;
    int recvFlag = 1;
    while (recvFlag) {

        // send message
        sendFlag = sendMsg(socketFD, handle);

        // check if user quit
        if (!sendFlag)
            break;

        //receive message
        recvFlag = recvMsg(socketFD);
    }

    //close the socket
    close(socketFD);

    return 0;
}