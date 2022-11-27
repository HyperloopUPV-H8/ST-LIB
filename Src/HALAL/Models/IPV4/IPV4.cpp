
#include "IPV4/IPV4.hpp"

IPV4::IPV4(string ip_address):string_ip(ip_address){
	stringstream sstream(ip_address);
	int ip_bytes[4];
	for(int& byte : ip_bytes){
		string temp;
		getline(sstream, temp, '.');
		byte = stoi(temp);
	}
	IP_ADDR4(&(this->ip_address),ip_bytes[0],ip_bytes[1],ip_bytes[2],ip_bytes[3]);
}

IPV4::IPV4() = default;
