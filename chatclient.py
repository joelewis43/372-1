#!/usr/bin/env python
# CHAT CLIENT

from socket import *

def getHandle():
   handle = "00000000000"

   while len(handle) > 10:
      handle = raw_input('Enter your handle (10 characters or less): ')

   handleString = "Welcome " + handle
   print(handleString)

   return handle

def prependHandle(handle, msg):
   return handle + "> " + msg

def getMessage(handle):

   # build prompt
   messagePrompt = prependHandle(handle, "")

   # request input
   message = raw_input(messagePrompt)
   
   # return formatted message
   return prependHandle(handle, message)





if __name__ == "__main__":

   # identify server address and port
   serverName = 'access.engr.oregonstate.edu'
   serverPort = 54545

   # get user handle
   handle = getHandle()

   # create client TCP socket
   clientSocket = socket(AF_INET, SOCK_STREAM)
   clientSocket.connect((serverName, serverPort))

   message = ""
   while True:

      # get message from user
      message = getMessage(handle)

      if '\quit' in message:
	 clientSocket.send(message)
	 break

      # send message to server
      clientSocket.send(message)

      # receive message from server
      serverMessage = clientSocket.recv(2048)

      if '\quit' in serverMessage:
	 print("Other user has left the chat")
	 break

      print(serverMessage)
      

   # clost the connection when user specified to quit
   clientSocket.close()
