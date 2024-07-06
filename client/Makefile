CXX = g++
CFLAGS = -Wall -g

CLIENT_SRC = main.cpp client.cpp
CLIENT_OBJ = $(subst .cpp,.o,$(CLIENT_SRC))
CLIENT_BIN = ml_client

BIN_PATH = ../bin

all: client 

client: $(CLIENT_OBJ)
	echo $(CLIENT_OBJ)
	$(CXX) $(CFLAGS) -o $(BIN_PATH)/$(CLIENT_BIN) $(CLIENT_OBJ)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm $(CLIENT_OBJ) $(BIN_PATH)/$(CLIENT_BIN)
