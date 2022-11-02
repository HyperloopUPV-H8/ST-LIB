#pragma once
#include "C++Utilities/CppUtils.hpp"
#include "App/lwip.h"
#include "lwip/ip_addr.h"

class IPV4{
public:
	ip_addr_t ip_address;

	IPV4(const string ip_address){
		stringstream sstream(ip_address);
		int ip_bytes[4];
		for(int& byte : ip_bytes){
			string temp;
			getline(sstream, temp, '.');
			byte = stoi(temp);
		}
		IP_ADDR4(&(this->ip_address),ip_bytes[0],ip_bytes[1],ip_bytes[2],ip_bytes[3]);
	}

};
