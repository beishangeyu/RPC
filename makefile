build:
	g++ -pthread client/run_client.cpp -o run_client
	g++ -pthread rpc_server/run_rpc.cpp -o run_rpc
	g++ -pthread server/run_server.cpp -o run_server

server:
	g++ -pthread server/run_server.cpp -o run_server

client:
	g++ -pthread client/run_client.cpp -o run_client

rpc:
	g++ -pthread rpc_server/run_rpc.cpp -o run_rpc

clean:
	rm -f run_client run_rpc run_server