CC = gcc
CFLAGS += -c -std=gnu99 -Wall -Werror -W

# library sources linked from client or server
CLIENT_SOURCES = $(wildcard shared/crypt_tools.c _client/client.c)
SERVER_SOURCES = $(wildcard shared/crypt_tools.c _server/server.c)
# compile libraries to objects
SERVER_OBJECTS = $(SERVER_SOURCES:.c=.o)
CLIENT_OBJECTS = $(CLIENT_SOURCES:.c=.o)
# commands default name and default output
SERVER = server
CLIENT = client
# add args here

THREAD_N = 4
SIZE = 1024
PORT = 2241
SERVER_ARGS = -j $(THREAD_N) -s $(SIZE) -p $(PORT)

KEY_SIZE = 128
REQUEST_RATE = 4
TIME = 1
IP = 127.0.0.1
PORT = 2241
CLIENT_ARGS = -k $(KEY_SIZE) -r $(REQUEST_RATE) -t $(TIME) $(IP):$(PORT)

#################################
##  Here starts make commands  ##
#################################

$(SERVER): $(SERVER_OBJECTS)
	$(CC) $(SERVER_OBJECTS) -o $(SERVER) $(LDFLAGS)

$(CLIENT): $(CLIENT_OBJECTS)
	$(CC) $(CLIENT_OBJECTS) -o $(CLIENT) $(LDFLAGS)

$(SERVER)_run:
	make $(SERVER)
	./$(SERVER) $(SERVER_ARGS)

$(CLIENT)_run:
	make $(CLIENT)
	./$(CLIENT) $(CLIENT_ARGS)

clean:
	rm $(CLIENT_OBJECTS) $(SERVER_OBJECTS) $(SERVER) $(CLIENT)


