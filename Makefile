all: bankingClient bankingServer

bankingClient:bankingClient.h bankingClient.c 
	gcc -lm -o bankingClient bankingClient.c -pthread

bankingServer:bankingServer.h bankingServer.c 
	gcc -lm -o bankingServer bankingServer.c -pthread

clean:
	rm bankingClient bankingServer
