#pragma once

#include "C++Utilities/CppUtils.hpp"
#include "ErrorHandler/ErrorHandler.hpp"
#include "Flash/Flash.hpp"
#include "FlashVariable.hpp"

#define FLASH_STORER_MAX_SIZE		((uint32_t)(1028*4))
#define FLASH_STORER_START_ADDRESS	(FLASH_STORER_MAX_ADDRESS - FLASH_STORER_MAX_SIZE)
#define FLASH_STORER_MAX_ADDRESS	((uint32_t)0x080DFFF0U)


class FlashStorer{
public:
	static vector<FlashVariable> variable_list;
	static uint32_t total_size;

	template<class... Type>
	static bool add_variables(Type&... var);
	static void read(void);
	static void store_all(void);
	template<class Type>
	static void store(Type& var);
	template<class... Type>
	static void store(Type&... var);

private:
	template<class Type>
	static void add_size(Type& var);
};
