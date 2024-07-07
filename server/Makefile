CXX = g++
CFLAGS = -Wall -g

SERVER_SRC = main.cpp MYLServer.cpp
SERVER_OBJ = $(subst .cpp,.o,$(SERVER_SRC))
SERVER_BIN = ml_server

BIN_PATH = ../bin
INCLUDE_PATH = ../include

all: server 

server: $(SERVER_OBJ)
	echo $(SERVER_OBJ)
	$(CXX) $(CFLAGS) -o $(BIN_PATH)/$(SERVER_BIN) $(SERVER_OBJ)

%.o: %.cpp
	$(CXX) $(CFLAGS) -I$(INCLUDE_PATH) -c $< -o $@

clean:
	rm $(SERVER_OBJ) $(BIN_PATH)/$(SERVER_BIN)
