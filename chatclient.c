#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h> 

#define MAXMSGSIZE 500

/*
    Input:          a string
    Output:         NA
    Description:    Used to remove the newline from the end of a string
                    Replaces newline with null terminator
*/
void removeNewLine(char* string) {
    
    // get the end of the string
    int len = strlen(string) - 1;

    // check if last character is a newline
    if (string[len] == '\n')
        string[len] = '\0';

}

/*
    Input:          NA
    Output:         NA
    Description:    Requests user input for their handle and returns
                    character array of their input
*/
char* getHandle() {

    int length = 50;
    char *handle = malloc(length * sizeof(char));
    memset(handle, '\0', length);           // clear the array

    do {
        printf("What is your handle (max 10 characters): ");
        fgets(handle, length, stdin);       // read the user input
    } while (strlen(handle) > 11);

    removeNewLine(handle);                  // remove the newline

    //add "> " to the handle
    int len = strlen(handle);
    handle[len] = '>';
    handle[len+1] = ' ';

    return handle;

}

/*
    Input:          input arguments (address and port number)
                    hints struct
                    response struct
    Output:         socket id number
    Description:    handles the creation of the TCP connection
*/
int setup(char** argv, struct addrinfo* hints, struct addrinfo* res) {

    int status;

    //empty struct
    memset(hints, 0, sizeof(*hints));

    // set up hints struct
    hints->ai_family = AF_UNSPEC;        // don't care IPv4 or IPv6
    hints->ai_socktype = SOCK_STREAM;    // TCP stream sockets

    // attempt to find the host
    if ((status = getaddrinfo(argv[1], argv[2], hints, &res)) != 0) {
        fprintf(stderr, "GetAddrInfo error: %s\n", gai_strerror(status));
    }

    // get socket descriptor
    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    // connect to host
    connect(sockfd, res->ai_addr, res->ai_addrlen);

    printf("\nStarting Chatroom ...\n\n");

    // free space allocated in the response object
    freeaddrinfo(res);

    return sockfd;

}

/*
    Input:          input message
    Output:         0 if quitting, 1 if not quiting
    Description:    Check if the client or server would like to quit
*/
int checkQuit(char* msg) {

    // specifies quit keyword
    char* quit = "\\quit";

    // check if message contains "\quit"
    if(strstr(msg, quit) != NULL) {
        return 1;
    }
    return 0;
}

/*
    Input:          socket identification number
                    users hande
    Output:         int specifying quit status
    Description:    gets user message and sends it over TCP socket
*/
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

/*
    Input:          Socket identification number
    Output:         unt specifying quit status
    Description:    reads server message from the socket and prints to client
*/
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
    char* handle = getHandle();

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
