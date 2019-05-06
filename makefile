CXX = gcc
CXXFLAGS = -g

main:
	$(CXX) $(CXXFLAGS) chatclient.c -o client
