#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>

#define MAXSIZE 500

void getHandle(char* handle) {

    int maxLen = 11;
    memset(handle, '\0', maxLen);
    printf("What is your handle (max 10 characters): ");
    fgets(handle, maxLen, stdin);

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

    return sockfd;

}

void sendMsg(int sockfd) {
    
    char msg[500];
    int len, bytes_sent;

    memset(msg, '\0', sizeof(msg));

    strcpy(msg, "From C Client");

    len = strlen(msg);

    bytes_sent = send(sockfd, msg, len, 0);
}

void recvMsg(int sockfd) {

    char buffer[MAXSIZE];
    memset(buffer, '\0', MAXSIZE);

    printf("Test: %d\n", MAXSIZE);

    int bytes_recv = recv(sockfd, buffer, MAXSIZE, 0);
    buffer[bytes_recv] = '\0';

    printf("%s\n\n", buffer);

}

int main(int argc, char** argv) {

    int socketFD;                  // socket file descriptor
    struct addrinfo hints;
    struct addrinfo *res;          // will point to the results

    //get the users handle
    char handle[11];
    getHandle(handle);

    //set up the TCP connection
    socketFD = setup(argv, &hints, res);

    // run on a while loop

    // send message
    sendMsg(socketFD);

    //receive message
    recvMsg(socketFD);

    //close the socket
    close(socketFD);

    //free the space allocated for server address
    freeaddrinfo(res);

    return 0;
}