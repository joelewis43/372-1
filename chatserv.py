#!/usr/bin/env python
# CHAT SERVER

from socket import *

def getPort():
   port = raw_input("What port would you like to open: ")
   return int(port)

def getHandle():
   handle = "00000000000"

   while len(handle) > 10:
      handle = raw_input('Enter your handle (10 characters or less): ')

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

   # get port number
   serverPort = getPort()

   # get handle
   handle = getHandle()

   # create TCP socket
   serverSocket = socket(AF_INET, SOCK_STREAM)

   # bind the port to the socket
   serverSocket.bind(('', serverPort))

   print("Chatroom open on port", serverPort)

   # wait for tcp connection
   serverSocket.listen(1)
   connection, clientAddress = serverSocket.accept()

   while True:

      # wait for message from client
      clientMessage = connection.recv(2048)

      # client quits the chat room
      if '\quit' in clientMessage:
	 print("Other user has left the chat")
	 break

      # print the clients message
      print(clientMessage)

      # get message from user
      message = getMessage(handle)

      # server quits the chat
      if '\quit' in message:
	 connection.send(message)
	 break

      # send message to client
      connection.send(message)

   connection.close()
