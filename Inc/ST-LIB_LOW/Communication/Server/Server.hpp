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
	ServerSocket *open_connection;
	vector<ServerSocket*> running_connections;
	IPV4 local_ip;
	uint32_t local_port;

	static vector<Server*> running_servers;

	Server(IPV4 local_ip, uint32_t local_port);
	~Server();
	void update();
	uint32_t connections_count();

	static void update_servers();
};
