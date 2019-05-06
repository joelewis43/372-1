#!/usr/bin/env python
# CHAT SERVER

from socket import *
import sys
import time

####################
# Input:       Users handle
#              User's message
# Output:      Message with handle prepended
# Description: Adds the handle to the provided message with "> " seperating
####################
def prependHandle(handle, msg):
   return handle + "> " + msg

####################
# Input:       User handle
# Output:      Message with handle prepended
# Description: gets message from user and prepends handle
####################
def getMessage(handle):

   # build prompt
   messagePrompt = prependHandle(handle, "")

   # request input
   message = raw_input(messagePrompt)
   
   # return formatted message
   return prependHandle(handle, message)





if __name__ == "__main__":

   while True:

      try:

         # get port number
         serverPort = int(sys.argv[1])

         # get handle
         handle = "Server"

         # create TCP socket
         serverSocket = socket(AF_INET, SOCK_STREAM)

         # bind the port to the socket
         serverSocket.bind(('', serverPort))

         portString = "\nChatroom open on port " + str(serverPort) + "\n"
         print(portString)

         # wait for tcp connection
         serverSocket.listen(1)
         connection, clientAddress = serverSocket.accept()

         while True:

            # wait for message from client
            clientMessage = connection.recv(2048)

            # client quits the chat room
            if '\quit' in clientMessage:
               print("\nOther user has left the chat")
               connection.close()
               break

            # print the clients message
            print(clientMessage)

            # get message from user
            message = getMessage(handle)

            # server quits the chat
            if '\quit' in message:
               connection.send(message)
               print("\nYou have left the chat")
               time.sleep(0.5)            # added to pause the server for half a second
                                          # program would fail without this because it
                                          # attempts to close TCP before the "\quit"
                                          # message is finished sending.
               connection.close()
               break
            
            # send message to client
            connection.send(message)

      # used to let the user know the SIGINT was received and is closing the server
      except KeyboardInterrupt:
         print("\nClosing Server\n")
         sys.exit()

