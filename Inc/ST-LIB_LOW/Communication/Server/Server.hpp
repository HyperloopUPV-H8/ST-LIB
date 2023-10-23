/*
 * Server.hpp
 *
 * Created on: Oct 12, 2023
 * 		Author: Ricardo
 */

#pragma once
#include "Communication/Ethernet/TCP/ServerSocket.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

class Server{
public:

	enum ServerState{
		RUNNING,
		CLOSING,
		CLOSED
	};

	ServerSocket *open_connection;
	vector<ServerSocket*> running_connections;
	IPV4 local_ip;
	uint32_t local_port;
	ServerState status;

	static vector<Server*> running_servers;

	Server(IPV4 local_ip, uint32_t local_port);
	~Server();
	void update();
	void broadcast_order(Order& order);
	void close_all();
	uint32_t connections_count();

	static void update_servers();
};
