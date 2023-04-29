#include "FlashStorer/FlashVariable.hpp"
#include "ErrorHandler/ErrorHandler.hpp"


template<class T>
FlashVariable::FlashVariable(T& value){
	FlashVariable::v_pointer = (void*)value;
	FlashVariable::size = sizeof(T);
}

void* FlashVariable::get_pointer(){
	return FlashVariable::v_pointer;
}

size_t FlashVariable::get_size(){
	return FlashVariable::size;
}
