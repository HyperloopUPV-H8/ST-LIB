#include "IPV4/IPV4.hpp"
#ifdef HAL_ETH_MODULE_ENABLED

IPV4::IPV4(string address) : string_address(address){
	stringstream sstream(address);
	int ip_bytes[4];
	for(int& byte : ip_bytes){
		string temp;
		getline(sstream, temp, '.');
		byte = stoi(temp);
	}
	IP_ADDR4(&(this->address), ip_bytes[0], ip_bytes[1], ip_bytes[2], ip_bytes[3]);
}

IPV4::IPV4(ip_addr_t address): address(address){
	for(uint8_t i = 0; i<16; i+=4){
		string_address += address.addr>>= i;
		string_address += ".";
	}
	string_address.pop_back();
}

IPV4::IPV4() = default;

#endif
