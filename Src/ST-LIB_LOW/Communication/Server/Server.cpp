/*
 * Server.cpp
 *
 * Created on: Oct 12, 2023
 * 		Author: Ricardo
 */

#include "Communication/Server/Server.hpp"

vector<Server*> Server::running_servers = {};

Server::Server(IPV4 local_ip, uint32_t local_port): local_ip(local_ip), local_port(local_port){
	open_connection = new ServerSocket(local_ip, local_port);
	running_servers.push_back(this);
}

Server::~Server(){
	open_connection->~ServerSocket();

	for(ServerSocket *s : running_connections){
		s->~ServerSocket();
	}

	running_servers.erase(find(running_servers.begin(), running_servers.end(), this));
}

void Server::update(){
	if(open_connection->is_connected()){
		running_connections.push_back(open_connection);
		open_connection = new ServerSocket(local_ip, local_port);
	}

	for(auto& ss : running_connections){
		if(!ss->is_connected()){
			ErrorHandler("ip disconnected, going to FAULT");
		}
	}
}

uint32_t Server::connections_count(){
	return running_connections.size();
}

void Server::update_servers(){
	for(Server *s : running_servers){
		s->update();
	}
}
