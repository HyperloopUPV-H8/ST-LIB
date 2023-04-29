#pragma once

#include "C++Utilities/CppUtils.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

class FlashVariable{
private:
	void* v_pointer;
	size_t size;
public:
	template<class T>
	FlashVariable(T& value);
	void* get_pointer();
	size_t get_size();
};
