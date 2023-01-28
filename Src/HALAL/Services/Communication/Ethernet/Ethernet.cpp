/*
 * Ethernet.cpp
 *
 *  Created on: Nov 23, 2022
 *      Author: stefa
 */

#include "Communication/Ethernet/Ethernet.hpp"
//#ifdef HAL_ETH_MODULE_ENABLED
extern uint32_t EthernetLinkTimer;
extern struct netif gnetif;
extern ip4_addr_t ipaddr, netmask, gw;

bool Ethernet::is_ready = false;
bool Ethernet::is_running = false;

string Ethernet::board_ip = "192.168.21.0";
string Ethernet::subnet_mask = "";
string Ethernet::gateway = "";

void Ethernet::mpu_start(){
	  MPU_Region_InitTypeDef MPU_InitStruct = {0};

	  HAL_MPU_Disable();

	  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
	  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
	  MPU_InitStruct.BaseAddress = ETHERNET_POOLS_BASE_ADDRESS;
	  MPU_InitStruct.Size = MPU_REGION_SIZE_32KB;
	  MPU_InitStruct.SubRegionDisable = 0x0;
	  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
	  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
	  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
	  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
	  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

	  HAL_MPU_ConfigRegion(&MPU_InitStruct);

	  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
	  MPU_InitStruct.BaseAddress = ETHERNET_POOLS_BASE_ADDRESS;
	  MPU_InitStruct.Size = MPU_REGION_SIZE_256B;
	  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;

	  HAL_MPU_ConfigRegion(&MPU_InitStruct);

	  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

void Ethernet::start() {
	start(IPV4(board_ip), IPV4(subnet_mask), IPV4(gateway));
}

void Ethernet::start(IPV4 local_ip, IPV4 subnet_mask, IPV4 gateway) {
	if (is_running) {
		ErrorHandler("Ethernet is already running");
		return;
	}

	if (not is_ready) {
		ErrorHandler("Ethernet is not ready");
		return;
	}

	ipaddr = local_ip.address;
	netmask = subnet_mask.address;
	gw = gateway.address;
	MX_LWIP_Init();
	is_running = true;
}

void Ethernet::inscribe(){
	if(is_ready){
		ErrorHandler("Ethernet is already inscribed");
		return;
	}

	Pin::inscribe(PA1, ALTERNATIVE);
	Pin::inscribe(PA2, ALTERNATIVE);
	Pin::inscribe(PA7, ALTERNATIVE);
	Pin::inscribe(PB13, ALTERNATIVE);
	Pin::inscribe(PC1, ALTERNATIVE);
	Pin::inscribe(PC4, ALTERNATIVE);
	Pin::inscribe(PC5, ALTERNATIVE);
	Pin::inscribe(PG11, ALTERNATIVE);
	Pin::inscribe(PG13, ALTERNATIVE);
	mpu_start();
	SCB_EnableICache();
	SCB_EnableDCache();
	is_ready = true;
}

void Ethernet::update(){
	if(not is_running) {
		ErrorHandler("Ethernet is not running, check if its been inscribed");
		return;
	}

	ethernetif_input(&gnetif);
	sys_check_timeouts();

	if (HAL_GetTick() - EthernetLinkTimer >= 100) {
		EthernetLinkTimer = HAL_GetTick();
		ethernet_link_check_state(&gnetif);
	}

	if(gnetif.flags == 15) {
		netif_set_up(&gnetif);
	}
}
//#endif
