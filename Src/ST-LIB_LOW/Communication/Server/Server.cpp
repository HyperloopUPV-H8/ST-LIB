/*
 * Server.cpp
 *
 * Created on: Oct 12, 2023
 * 		Author: Ricardo
 */

#include "Communication/Server/Server.hpp"

vector<Server*> Server::running_servers = {};

Server::Server(IPV4 local_ip, uint32_t local_port): local_ip(local_ip), local_port(local_port), status(RUNNING){
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
		if(status == RUNNING && !ss->is_connected()){
			ErrorHandler("ip %s disconnected, going to FAULT",ss->remote_ip.string_address.c_str());
			status = CLOSING;
			break;
		}
	}

	if(status == CLOSING){
		close_all();
	}
}

void Server::broadcast_order(Order& order){
	for(auto& ss : running_connections){
		if(!ss->send_order(order)){
			ErrorHandler("Couldn t put Order %d into buffer of ip's %s ServerSocket, buffer may be full or the ServerSocket may be ill formed",order.get_id(),ss->remote_ip.string_address.c_str());
		}
	}
}

void Server::close_all(){
	for(auto& ss : running_connections){
		ss->close();
		running_connections.erase(find(running_connections.begin(),running_connections.end(),ss));
	}
	status = CLOSED;
}

uint32_t Server::connections_count(){
	return running_connections.size();
}

void Server::update_servers(){
	for(Server *s : running_servers){
		s->update();
	}
}
