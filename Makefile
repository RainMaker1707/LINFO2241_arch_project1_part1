CFLAGS += -std=gnu99 -Wall -W -O2 -mavx -march=native

# Client build
client: _client/client.o shared/crypt_tools.o shared/verbose.o
	@gcc -DOPTIM=0 shared/verbose.o _client/client.o -o client $(CFLAGS)
_client/client.o: _client/client.c
	@gcc -DOPTIM=0 -c -o _client/client.o _client/client.c $(CFLAGS)

client-queue: _client/client-queue.o shared/crypt_tools.o shared/verbose.o
	@gcc -DOPTIM=0 -DQ=1 shared/verbose.o _client/client.o -o client-queue $(CFLAGS)
_client/client-queue.o: _client/client.c
	@gcc -DOPTIM=0 -DQ=1 -c -o _client/client.o _client/client.c $(CFLAGS)

# Server without optimisation build
server: _server/server.o shared/crypt_tools.o shared/verbose.o
	@gcc -DOPTIM=0 shared/crypt_tools.o shared/verbose.o _server/server.o -o server $(CFLAGS)
_server/server.o: _server/server.c
	@gcc -DOPTIM=0 -c -o _server/server.o _server/server.c $(CFLAGS)
shared/crypt_tools.o: shared/crypt_tools.c
	@gcc -DOPTIM=0 -c -o shared/crypt_tools.o shared/crypt_tools.c $(CFLAGS)

# Optimised uint32 server
server-optim: _server/server-optim.o shared/crypt_tools-optim.o shared/verbose.o
	@gcc -DOPTIM=1 shared/crypt_tools-optim.o shared/verbose.o _server/server-optim.o -o server-optim $(CFLAGS)
_server/server-optim.o: _server/server.c
	@gcc -DOPTIM=1 -c -o _server/server-optim.o _server/server.c $(CFLAGS)
shared/crypt_tools-optim.o: shared/crypt_tools.c
	@gcc -DOPTIM=1 -c -o shared/crypt_tools-optim.o shared/crypt_tools.c $(CFLAGS)

# Tools to verbose the programmes
shared/verbose.o: shared/verbose.c
	@gcc -DOPTIM=0 -c -o shared/verbose.o shared/verbose.c $(CFLAGS)

# Float version without optimisation
server-float: shared/float-encrypt server/server-float shared/verbose.o
	@gcc -DOPTIM=2 shared/float_crypt_tools.o shared/verbose.o _server/server-float.o -o server-float $(CFLAGS)
shared/float-encrypt:
	@gcc -DOPTIM=2 -c -o shared/float_crypt_tools.o shared/float_crypt_tools.c $(CFLAGS)
server/server-float:
	@gcc -DOPTIM=2 -c -o _server/server-float.o _server/server.c $(CFLAGS)

# Optimised float version
server-float-avx: shared/float-encrypt-optim server/server-float-optim shared/verbose.o
	@gcc -DOPTIM=3 shared/float_crypt_tools_optim.o shared/verbose.o _server/server-float-optim.o -o server-float-avx $(CFLAGS)
shared/float-encrypt-optim:
	@gcc -DOPTIM=3 -c -o shared/float_crypt_tools_optim.o shared/float_crypt_tools.c $(CFLAGS)
server/server-float-optim:
	@gcc -DOPTIM=3 -c -o _server/server-float-optim.o _server/server.c $(CFLAGS)


# Optimised float version
server-queue: shared/float-encrypt-queue server/server-float-queue shared/verbose.o
	@gcc -DOPTIM=3 -DQ=1 shared/float_crypt_tools_optim.o shared/verbose.o _server/server-float-optim.o -o server-queue $(CFLAGS)
shared/float-encrypt-queue:
	@gcc -DOPTIM=3 -DQ=1 -c -o shared/float_crypt_tools_optim.o shared/float_crypt_tools.c $(CFLAGS)
server/server-float-queue:
	@gcc -DOPTIM=3 -DQ=1 -c -o _server/server-float-optim.o _server/server.c $(CFLAGS)

# Cleaner
clean:
	@rm -f _server/server.o _server/server-optim.o shared/crypt_tools.o shared/crypt_tools-optim.o shared/verbose.o
	@rm -f _server/server-float.o  _server/server-float-optim.o shared/float_crypt_tools.o shared/float_crypt_tools_optim.o
	@rm -f server server-optim server-float server-float-avx
	@rm -f _client/client.o client client-queue server-queue

# Compressor
NAME=archive.tar.gz
tar:
	@tar cfz $(NAME) Makefile _server/*.c _server/*.h shared/*.c shared/*.h _client/*.c _client/*.h requirements.txt get_data.sh make_graph.py

graph:
	@./get_data.sh && python3 make_graph.py




