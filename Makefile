CC = gcc
CFLAGS += -c -std=gnu99 -Wall -Werror -W


CLIENT_SOURCES = $(wildcard shared/crypt_tools.c client/client.c)

SERVER_SOURCES = $(wildcard shared/crypt_tools.c server/server.c)

SERVER_OBJECTS = $(SERVER_SOURCES:.c=.o)
CLIENT_OBJECTS = $(CLIENT_SOURCES:.c=.o)

SERVER = server
CLIENT = client

$(SERVER): $(SERVER_OBJECTS)
	$(CC) $(SERVER_OBJECTS) -o $(SERVER).e $(LDFLAGS)

$(CLIENT): $(CLIENT_OBJECTS)
	$(CC) $(CLIENT_OBJECTS) -o $(CLIENT).e $(LDFLAGS)

server_run:
	./server.e # add args here

client_run:
	./client.e # add args here

clean:
	rm $(CLIENT_OBJECTS) $(SERVER_OBJECTS) server.e client.e


