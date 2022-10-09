CC = gcc
CFLAGS += -c -std=gnu99 -Wall -Werror -W

# library sources linked from client or server
CLIENT_SOURCES = $(wildcard shared/crypt_tools.c client/client.c)
SERVER_SOURCES = $(wildcard shared/crypt_tools.c server/server.c)
# compile libraries to objects
SERVER_OBJECTS = $(SERVER_SOURCES:.c=.o)
CLIENT_OBJECTS = $(CLIENT_SOURCES:.c=.o)
# commands default name and default output.e
SERVER = server
CLIENT = client
# add args here
SERVER_ARGS =
CLIENT_ARGS =

#################################
##  Here starts make commands  ##
#################################

$(SERVER): $(SERVER_OBJECTS)
	$(CC) $(SERVER_OBJECTS) -o $(SERVER).e $(LDFLAGS)

$(CLIENT): $(CLIENT_OBJECTS)
	$(CC) $(CLIENT_OBJECTS) -o $(CLIENT).e $(LDFLAGS)

$(SERVER)_run:
	make $(SERVER)
	./$(SERVER).e $(SERVER_ARGS)

$(CLIENT)_run:
	make $(CLIENT)
	./$(CLIENT).e $(CLIENT_ARGS)

clean:
	rm $(CLIENT_OBJECTS) $(SERVER_OBJECTS) $(SERVER).e $(CLIENT).e


