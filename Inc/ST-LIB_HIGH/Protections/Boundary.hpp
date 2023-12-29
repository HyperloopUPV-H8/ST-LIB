#pragma once
#define PROTECTIONTYPE_LENGTH 7
#include "ErrorHandler/ErrorHandler.hpp"
#include "Control/Blocks/MeanCalculator.hpp"
#include "HALAL/Models/Packets/Order.hpp"
#include "HALAL/Services/Time/RTC.hpp"

using type_id_t = void(*)();
template<typename>
void type_id() {}

namespace Protections{
enum FaultType : uint8_t{
	FAULT = 0,
	WARNING,
    OK
};
}

enum ProtectionType : uint8_t {
    BELOW = 0,
    ABOVE,
    OUT_OF_RANGE,
    EQUALS,
    NOT_EQUALS,
	ERROR_HANDLER,
	TIME_ACCUMULATION
};

struct BoundaryInterface{
public:
    virtual Protections::FaultType check_bounds() = 0;
	HeapOrder* message;

	void update_name(char* n){
		name = n;
		if(strlen(n) > NAME_MAX_LEN){
			ErrorHandler("Variable name is too long, max length is %d",NAME_MAX_LEN);
			return;
		}
		string_len = name.size();
	}
	virtual void update_error_handler_message( [[maybe_unused]] const char* err_message){
	}
	static const char* get_error_handler_string(){
		return ErrorHandlerModel::description.c_str();
	}
	uint8_t boundary_type_id{};
protected:
	static const map<type_id_t,uint8_t> format_look_up;
	static int get_error_handler_string_size(){
		return ErrorHandlerModel::description.size();
	}

	//this will store the name of the variable
	string name;
	//max variable name
	static constexpr uint8_t NAME_MAX_LEN = 40;
	uint8_t format_id;
	uint8_t string_len;
};

template<class Type, ProtectionType Protector> struct Boundary;

template<class Type>
struct Boundary<Type, BELOW> : public BoundaryInterface{
	static constexpr ProtectionType Protector = BELOW;
	bool has_warning_level{false};
	Type* src = nullptr;
	Type boundary;
	Type warning_threshold;

	Boundary(Type warning_threshold, Type boundary): has_warning_level{true}, boundary(boundary), warning_threshold(warning_threshold){};
	Boundary(Type boundary):boundary(boundary){}
	Boundary(Type* src, Boundary<Type, Protector> boundary): 
		src(src),boundary(boundary.boundary)
		{
			// we have to do this because we cannot take address of rvalue (ProtectionType::BELOW)
			boundary_type_id = Protector;
			format_id = BoundaryInterface::format_look_up.at(type_id<Type>);
			// we have to preallocate space, otherwise the might get moved around, invalidating the pointer, better safe than sorry
			name.reserve(NAME_MAX_LEN);
			message = new HeapOrder(uint16_t{111},&format_id,&boundary_type_id,&string_len,&name,&this->boundary,this->src,
				&Global_RTC::global_RTC.counter,&Global_RTC::global_RTC.second,&Global_RTC::global_RTC.minute,
				&Global_RTC::global_RTC.hour,&Global_RTC::global_RTC.day,&Global_RTC::global_RTC.month,&Global_RTC::global_RTC.year);
			}


	Boundary(Type* src, Type boundary): src(src),boundary(boundary){}
	Protections::FaultType check_bounds()override{
		if(*src < boundary) return Protections::FAULT;
		if(has_warning_level && *src < warning_threshold) return Protections::WARNING;
		return Protections::OK;
	}
};

template<class Type>
struct Boundary<Type, ABOVE> : public BoundaryInterface{
	static constexpr ProtectionType Protector = ABOVE;
	bool has_warning_level{false};
	Type* src = nullptr;
	Type boundary;
	Type warning_threshold;

	Boundary(Type warning_threshold, Type boundary): has_warning_level{true}, boundary(boundary), warning_threshold(warning_threshold){};
	Boundary(Type boundary): boundary(boundary){};
	Boundary(Type* src, Boundary<Type, Protector> boundary): 
		has_warning_level(boundary.has_warning_level),
		src(src),boundary(boundary.boundary),warning_threshold(boundary.warning_threshold)
		{
			boundary_type_id = Protector;
			format_id = BoundaryInterface::format_look_up.at(type_id<Type>);
			name.reserve(NAME_MAX_LEN);
			message = new HeapOrder(uint16_t{111},&format_id,&boundary_type_id,&string_len,&name,&this->boundary,this->src,
				&Global_RTC::global_RTC.counter,&Global_RTC::global_RTC.second,&Global_RTC::global_RTC.minute,
				&Global_RTC::global_RTC.hour,&Global_RTC::global_RTC.day,&Global_RTC::global_RTC.month,&Global_RTC::global_RTC.year);
		}
	Boundary(Type* src, Type boundary): src(src),boundary(boundary){}
	Protections::FaultType check_bounds()override{
		if(*src > boundary) return Protections::FAULT;
		if(has_warning_level && *src > warning_threshold) return Protections::WARNING;
		return Protections::OK;
	}
};

template<class Type>
struct Boundary<Type, EQUALS> : public BoundaryInterface{
	static constexpr ProtectionType Protector = EQUALS;
	Type* src = nullptr;
	Type boundary;
	
	Boundary(Type boundary): boundary(boundary){};
	Boundary(Type* src, Boundary<Type, Protector> boundary): 
		src(src),boundary(boundary.boundary)
		{		
			boundary_type_id = Protector;	
			format_id = BoundaryInterface::format_look_up.at(type_id<Type>);
			name.reserve(NAME_MAX_LEN);
			message = new HeapOrder(uint16_t{111},&format_id,&boundary_type_id,&string_len,&name,&this->boundary,this->src,
				&Global_RTC::global_RTC.counter,&Global_RTC::global_RTC.second,&Global_RTC::global_RTC.minute,
				&Global_RTC::global_RTC.hour,&Global_RTC::global_RTC.day,&Global_RTC::global_RTC.month,&Global_RTC::global_RTC.year);
		}
	Boundary(Type* src, Type boundary): src(src),boundary(boundary){}
	Protections::FaultType check_bounds()override{
		if(*src == boundary) return Protections::FAULT;
		return Protections::OK;
	}
};

template<class Type>
struct Boundary<Type, NOT_EQUALS> : public BoundaryInterface{
	static constexpr ProtectionType Protector = NOT_EQUALS;
	Type* src = nullptr;
	Type boundary;
	Boundary(Type boundary): boundary(boundary){};
	Boundary(Type* src, Boundary<Type, Protector> boundary): 
		src(src),boundary(boundary.boundary)
		{
			boundary_type_id = Protector;
			format_id = BoundaryInterface::format_look_up.at(type_id<Type>);
			name.reserve(NAME_MAX_LEN);			
			message = new HeapOrder(uint16_t{111},&format_id,&boundary_type_id,&string_len,&name,&this->boundary,this->src,
				&Global_RTC::global_RTC.counter,&Global_RTC::global_RTC.second,&Global_RTC::global_RTC.minute,
				&Global_RTC::global_RTC.hour,&Global_RTC::global_RTC.day,&Global_RTC::global_RTC.month,&Global_RTC::global_RTC.year);
		}
	Boundary(Type* src, Type boundary): src(src),boundary(boundary){}
	Protections::FaultType check_bounds()override{
		if(*src != boundary) return Protections::FAULT;
		return Protections::OK;
	}
};

template<class Type>
struct Boundary<Type, OUT_OF_RANGE> : public BoundaryInterface{
	static constexpr ProtectionType Protector = OUT_OF_RANGE;
	Type* src = nullptr;
	Type lower_boundary, upper_boundary;
	
	
	Boundary(Type lower_boundary, Type upper_boundary): lower_boundary(lower_boundary), upper_boundary(upper_boundary){};
	Boundary(Type* src, Boundary<Type, Protector> boundary): 
	src(src),lower_boundary(boundary.lower_boundary),upper_boundary(boundary.upper_boundary)
	{
		boundary_type_id = Protector;
		format_id = BoundaryInterface::format_look_up.at(type_id<Type>);
		name.reserve(NAME_MAX_LEN);
		message = new HeapOrder(uint16_t{111},&format_id,&boundary_type_id,&string_len,&name,&this->lower_boundary,&this->upper_boundary,this->src,
			&Global_RTC::global_RTC.counter,&Global_RTC::global_RTC.second,&Global_RTC::global_RTC.minute,
			&Global_RTC::global_RTC.hour,&Global_RTC::global_RTC.day,&Global_RTC::global_RTC.month,&Global_RTC::global_RTC.year);
	}
	Boundary(Type* src, Type lower_boundary, Type upper_boundary): src(src), lower_boundary(lower_boundary), upper_boundary(upper_boundary){}
	Protections::FaultType check_bounds()override{
		if(*src < lower_boundary || *src > upper_boundary) return Protections::FAULT;
		return Protections::OK;
	}
};

template<>
struct Boundary<void, ERROR_HANDLER> : public BoundaryInterface{
	static constexpr ProtectionType Protector = ERROR_HANDLER;
	Boundary(void*){}
	Boundary(void*, Boundary<void,ERROR_HANDLER>)
	{
		boundary_type_id = Protector;
		error_handler_string.reserve(ERROR_HANDLER_MSG_MAX_LEN);
		message = new HeapOrder(uint16_t{111},&boundary_type_id,&string_len,&name,&error_handler_string_len,&error_handler_string,
			&Global_RTC::global_RTC.counter,&Global_RTC::global_RTC.second,&Global_RTC::global_RTC.minute,
			&Global_RTC::global_RTC.hour,&Global_RTC::global_RTC.day,&Global_RTC::global_RTC.month,&Global_RTC::global_RTC.year);
	}
	Boundary() = default;
	Protections::FaultType check_bounds() override{
		return not ErrorHandlerModel::error_triggered ? Protections::OK : Protections::FAULT;
	}
	void update_error_handler_message(const char* err_message)override{
		error_handler_string = err_message;
		if(strlen(err_message) > ERROR_HANDLER_MSG_MAX_LEN){
			ErrorHandler("Error Handler message is too long, max length is %d",ERROR_HANDLER_MSG_MAX_LEN);
			return;
		}
		error_handler_string_len = error_handler_string.size();
	}
	private:
		
		string error_handler_string;
		uint16_t error_handler_string_len;
		static constexpr uint16_t ERROR_HANDLER_MSG_MAX_LEN = 255;
};



template<typename Type>
requires(std::is_floating_point_v<Type>)
struct Boundary<Type, TIME_ACCUMULATION> : public BoundaryInterface {
	static constexpr ProtectionType Protector = TIME_ACCUMULATION;	
	Boundary(Type bound, float time_limit, float frequency, Boundary<Type, Protector>*& external_pointer): bound(bound),time_limit(time_limit) ,frequency(frequency), moving_order(frequency*time_limit/100),
			external_pointer(&external_pointer){
		external_pointer = this;
	};
	Boundary(Type* src, Boundary<Type, Protector> boundary): src(src),bound(boundary.bound),time_limit(boundary.time_limit),frequency(boundary.frequency),moving_order(frequency*time_limit/100), external_pointer(boundary.external_pointer){
		*external_pointer = this;
		boundary_type_id = Protector;
		format_id = BoundaryInterface::format_look_up.at(type_id<Type>);
		name.reserve(NAME_MAX_LEN);
		message = new HeapOrder(uint16_t{111},&format_id,&boundary_type_id,&string_len,&name,&this->bound,this->src,&this->time_limit,&this->frequency,
			&Global_RTC::global_RTC.counter,&Global_RTC::global_RTC.second,&Global_RTC::global_RTC.minute,
			&Global_RTC::global_RTC.hour,&Global_RTC::global_RTC.day,&Global_RTC::global_RTC.month,&Global_RTC::global_RTC.year);
	}
	Boundary(Type* src, Type bound ,float time_limit, float frequency): src(src),bound(bound) ,time_limit(time_limit), frequency(frequency),moving_order(frequency*time_limit/100), external_pointer(nullptr){}
	
	uint8_t format_id{};
	Type* src = nullptr;
	Type bound;
	float time_limit;
	float frequency;
	bool still_good = true;
	Boundary<Type,Protector>** external_pointer;

	MeanCalculator<100> mean_calculator;
	vector<Type> mean_moving_average;
	uint16_t moving_order = 0;
	uint16_t moving_last = -1;
	uint16_t moving_first = 0;
	uint16_t moving_counter = 0;
	Type accumulator{};

	bool check_accumulation(Type value){
		if(!still_good) return false;
		mean_calculator.input(abs(value));
		mean_calculator.execute();
		if(mean_calculator.output_value == 0){
			return true;
		}
		mean_calculator.reset();
		if(moving_counter < moving_order) {
			moving_last++;
			mean_moving_average[moving_last] = mean_calculator.output_value;
			accumulator += mean_calculator.output_value/moving_order;
			moving_counter++;
			return true;
		}
		accumulator -= mean_moving_average[moving_first] / moving_order;
		moving_first = (moving_first + 1) % moving_order;
		moving_last = (moving_last + 1) % moving_counter;
		mean_moving_average[moving_last] = mean_calculator.output_value;
		accumulator += mean_moving_average[moving_last] / moving_order;
		if(accumulator > bound){
			still_good = false;
			return false;
		}
		return true;
	}

	Protections::FaultType check_bounds() override{
		return still_good ? Protections::OK : Protections::FAULT;
	}
};

