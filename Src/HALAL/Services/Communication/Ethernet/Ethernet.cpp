/*
 * Ethernet.cpp
 *
 *  Created on: Nov 23, 2022
 *      Author: stefa
 */

#include "Communication/Ethernet/Ethernet.hpp"

extern uint32_t EthernetLinkTimer;
extern struct netif gnetif;

bool Ethernet::is_ready = false;
bool Ethernet::is_running = false;

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



void Ethernet::start(){
	if(!is_running && is_ready){
		MX_LWIP_Init();
		is_running = true;
	}else{
		//TODO: Error Handler
	}
}

void Ethernet::inscribe(){
	if(!is_ready){
		Pin::register_pin(PA1, ALTERNATIVE);
		Pin::register_pin(PA2, ALTERNATIVE);
		Pin::register_pin(PA7, ALTERNATIVE);
		Pin::register_pin(PB13, ALTERNATIVE);
		Pin::register_pin(PC1, ALTERNATIVE);
		Pin::register_pin(PC4, ALTERNATIVE);
		Pin::register_pin(PC5, ALTERNATIVE);
		Pin::register_pin(PG11, ALTERNATIVE);
		Pin::register_pin(PG13, ALTERNATIVE);
		mpu_start();
		SCB_EnableICache();
		SCB_EnableDCache();
		is_ready = true;
	}else{
		//TODO: Error Handler
	}
}

void Ethernet::update(){
	if(is_running){
		ethernetif_input(&gnetif);
		sys_check_timeouts();
		if (HAL_GetTick() - EthernetLinkTimer >= 100)
		{
		EthernetLinkTimer = HAL_GetTick();
		ethernet_link_check_state(&gnetif);
		}
		if(gnetif.flags == 15){
		 netif_set_up(&gnetif);
		}
	}else{
		//TODO: Error Handler;
	}
}