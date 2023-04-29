#include "FlashStorer/FlashStorer.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

vector<FlashVariable> FlashStorer::variable_list = {};
uint32_t FlashStorer::total_size = 0;

template<class Type>
void FlashStorer::add_size(Type& var){//?Mirar, si no hace falta borrar
	FlashStorer::total_size += sizeof(Type);
}

template<class... Type>
bool FlashStorer::add_variables(Type&... var){
	FlashStorer::variable_list.push_back(FlashVariable(var)...);
	FlashStorer::total_size = total_sizeof<Type...>::var;
	
	return FlashStorer::total_size >= FLASH_STORER_MAX_SIZE;
}

void FlashStorer::read(){
	uint32_t* data = (uint32_t*)malloc(FlashStorer::total_size);
	Flash::read(FLASH_STORER_START_ADDRESS, (uint32_t*)data, FlashStorer::total_size);

	uint32_t offset = 0;
	for(FlashVariable v: variable_list){
		memcpy(v.get_pointer(), (((uint8_t*)data) + offset), v.get_size());
		offset += v.get_size();
	}

	free(data);
}

void FlashStorer::store_all(){
	uint32_t* data = (uint32_t*)malloc(FlashStorer::total_size);
	uint32_t offset = 0;
	for(FlashVariable v: variable_list){
		memcpy((((uint8_t*)data) + offset), v.get_pointer(), v.get_size());
		offset += v.get_size();
	}

	Flash::write(data, FLASH_STORER_START_ADDRESS, FlashStorer::total_size);
	free(data);
}

template<class Type>
void FlashStorer::store(Type& var) {
	uint32_t* data = (uint32_t*)malloc(FlashStorer::total_size);
	Flash::read(FLASH_STORER_START_ADDRESS, (uint32_t*)data, FlashStorer::total_size);

	uint32_t offset = 0;
	for(FlashVariable v: variable_list){
		if (v.get_pointer() == &var) {
			memcpy((((uint8_t*)data) + offset), v.get_pointer(), v.get_size());
		}

		offset += v.get_size();
	}

	Flash::write(data, FLASH_STORER_START_ADDRESS, FlashStorer::total_size);
	free(data);
}

template<class... Type> requires requires{requires sizeof...(Type) != 1;}
void FlashStorer::store(Type&... var){
	(FlashStorer::store(var), ...);
}
