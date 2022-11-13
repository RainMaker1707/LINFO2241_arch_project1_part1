CFLAGS += -std=gnu99 -Wall -W -mno-sse2 -mno-avx -mno-avx2 -mno-avx512f -fno-unroll-loops -fno-tree-vectorize -O2 -lpthread

server: _server/server.o shared/crypt_tools.o shared/verbose.o
	@gcc -DOPTIM=0 shared/crypt_tools.o shared/verbose.o _server/server.o -o server $(CFLAGS)

server-optim: _server/server-optim.o shared/crypt_tools-optim.o shared/verbose.o
	@gcc -DOPTIM=1 shared/crypt_tools-optim.o shared/verbose.o _server/server-optim.o -o server-optim $(CFLAGS)

client: _client/client.o shared/crypt_tools.o shared/verbose.o
	@gcc -DOPTIM=0 shared/crypt_tools.o shared/verbose.o _client/client.o -o client $(CFLAGS)

_client/client.o: _client/client.c
	@gcc -DOPTIM=0 -c -o _client/client.o _client/client.c $(CFLAGS)

_server/server.o: _server/server.c
	@gcc -DOPTIM=0 -c -o _server/server.o _server/server.c $(CFLAGS)

_server/server-optim.o: _server/server.c
	@gcc -DOPTIM=1 -c -o _server/server-optim.o _server/server.c $(CFLAGS)

shared/crypt_tools.o: shared/crypt_tools.c
	@gcc -DOPTIM=0 -c -o shared/crypt_tools.o shared/crypt_tools.c $(CFLAGS)

shared/crypt_tools-optim.o: shared/crypt_tools.c
	@gcc -DOPTIM=1 -c -o shared/crypt_tools-optim.o shared/crypt_tools.c $(CFLAGS)

shared/verbose.o: shared/verbose.c
	@gcc -DOPTIM=0 -c -o shared/verbose.o shared/verbose.c $(CFLAGS)

clean:
	@rm _server/server.o _server/server-optim.o _client/client.o shared/crypt_tools.o shared/crypt_tools-optim.o shared/verbose.o



