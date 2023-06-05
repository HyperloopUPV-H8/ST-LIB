#pragma once
#define PROTECTIONTYPE_LENGTH 7

#include "Control/Blocks/MeanCalculator.hpp"

enum ProtectionType : uint64_t {
    BELOW = std::numeric_limits<uint64_t>::max() - PROTECTIONTYPE_LENGTH + 1,
    ABOVE,
    OUT_OF_RANGE,
    EQUALS,
    NOT_EQUALS,
	ERROR_HANDLER,
	TIME_ACCUMULATION
};

struct BoundaryInterface{
public:
    virtual bool check_bounds() = 0;
	virtual char* serialize(char* dst) = 0;
	virtual int get_string_size() = 0;

protected:
	static int get_error_handler_string_size(){
		return ErrorHandlerModel::description.size();
	}
	static string get_error_handler_string(){
		return ErrorHandlerModel::description;
	}
};

template<class Type, ProtectionType Protector> struct Boundary;

template<class Type>
struct Boundary<Type, BELOW> : public BoundaryInterface{
	static constexpr ProtectionType Protector = BELOW;
	static constexpr const char* format = "\"type\": \"LOWER_BOUND\", \"data\": { \"value\": %s, \"bound\": %s }";
	Type* src = nullptr;
	Type boundary;
	Boundary(Type boundary): boundary(boundary){};
	Boundary(Type* src, Boundary<Type, Protector> boundary): src(src),boundary(boundary.boundary){}
	Boundary(Type* src, Type boundary): src(src),boundary(boundary){}
	bool check_bounds()override{
		if(*src < boundary) return false;
		return true;
	}
	int get_string_size()override{
		return snprintf(nullptr,0,format,to_string(*src).c_str(), to_string(boundary).c_str());
	}
	char* serialize(char* dst)override{
		sprintf(dst, format, to_string(*src).c_str(), to_string(boundary).c_str());
		return dst;
	}
};

template<class Type>
struct Boundary<Type, ABOVE> : public BoundaryInterface{
	static constexpr ProtectionType Protector = ABOVE;
	static constexpr const char* format = "\"type\": \"UPPER_BOUND\", \"data\": { \"value\": %s, \"bound\": %s }";
	Type* src = nullptr;
	Type boundary;
	Boundary(Type boundary): boundary(boundary){};
	Boundary(Type* src, Boundary<Type, Protector> boundary): src(src),boundary(boundary.boundary){}
	Boundary(Type* src, Type boundary): src(src),boundary(boundary){}
	bool check_bounds()override{
		if(*src > boundary) return false;
		return true;
	}
	int get_string_size()override{
		return snprintf(nullptr,0,format,to_string(*src).c_str(), to_string(boundary).c_str());
	}
	char* serialize(char* dst)override{
		sprintf(dst, format, to_string(*src).c_str(), to_string(boundary).c_str());
		return dst;
	}
};

template<class Type>
struct Boundary<Type, EQUALS> : public BoundaryInterface{
	static constexpr ProtectionType Protector = EQUALS;
	static constexpr const char* format = "\"type\": \"EQUALS\", \"data\": { \"value\": %s }";
	Type* src = nullptr;
	Type boundary;
	Boundary(Type boundary): boundary(boundary){};
	Boundary(Type* src, Boundary<Type, Protector> boundary): src(src),boundary(boundary.boundary){}
	Boundary(Type* src, Type boundary): src(src),boundary(boundary){}
	bool check_bounds()override{
		if(*src == boundary) return false;
		return true;
	}
	int get_string_size()override{
		return snprintf(nullptr,0,format,to_string(*src).c_str(), to_string(boundary).c_str());
	}
	char* serialize(char* dst)override{
		sprintf(dst, format, to_string(*src).c_str(), to_string(boundary).c_str());
		return dst;
	}
};

template<class Type>
struct Boundary<Type, NOT_EQUALS> : public BoundaryInterface{
	static constexpr ProtectionType Protector = NOT_EQUALS;
	static constexpr const char* format = "\"type\": \"NOT_EQUALS\", \"data\": { \"value\": %s, \"want\": %s }";
	Type* src = nullptr;
	Type boundary;
	Boundary(Type boundary): boundary(boundary){};
	Boundary(Type* src, Boundary<Type, Protector> boundary): src(src),boundary(boundary.boundary){}
	Boundary(Type* src, Type boundary): src(src),boundary(boundary){}
	bool check_bounds()override{
		if(*src != boundary) return false;
		return true;
	}
	int get_string_size()override{
		return snprintf(nullptr,0,format,to_string(*src).c_str(), to_string(boundary).c_str());
	}
	char* serialize(char* dst)override{
		sprintf(dst, format, to_string(*src).c_str(), to_string(boundary).c_str());
		return dst;
	}
};

template<class Type>
struct Boundary<Type, OUT_OF_RANGE> : public BoundaryInterface{
	static constexpr ProtectionType Protector = OUT_OF_RANGE;
	static constexpr const char* format = "\"type\": \"OUT_OF_BOUNDS\", \"data\": { \"value\": %s, \"bounds\": [%s, %s] }";
	Type* src = nullptr;
	Type lower_boundary, upper_boundary;
	Boundary(Type lower_boundary, Type upper_boundary): lower_boundary(lower_boundary), upper_boundary(upper_boundary){};
	Boundary(Type* src, Boundary<Type, Protector> boundary): src(src),lower_boundary(boundary.lower_boundary),upper_boundary(boundary.upper_boundary){}
	Boundary(Type* src, Type lower_boundary, Type upper_boundary): src(src), lower_boundary(lower_boundary), upper_boundary(upper_boundary){}
	bool check_bounds()override{
		if(*src < lower_boundary || *src > upper_boundary) return false;
		return true;
	}
	int get_string_size()override{
		return snprintf(nullptr,0,format,to_string(*src).c_str(), to_string(lower_boundary).c_str(), to_string(upper_boundary).c_str());
	}
	char* serialize(char* dst)override{
		sprintf(dst, format, to_string(*src).c_str(), to_string(lower_boundary).c_str(), to_string(upper_boundary).c_str());
		return dst;
	}
};

template<>
struct Boundary<void, ERROR_HANDLER> : public BoundaryInterface{
	static constexpr ProtectionType Protector = ERROR_HANDLER;
	static constexpr const char* format = "\"type\": \"ERROR_HANDLER\", \"data\": %s ";
	Boundary(void*){}
	Boundary(void*, Boundary<void,ERROR_HANDLER>){}
	Boundary() = default;
	bool check_bounds() override{
		return ErrorHandlerModel::error_triggered;
	}
	int get_string_size()override{
		return BoundaryInterface::get_error_handler_string_size();
	}
	char* serialize(char* dst)override{
		sprintf(dst,format,BoundaryInterface::get_error_handler_string().c_str());
		return dst;
	}
};

template<>
struct Boundary<double, TIME_ACCUMULATION> : public BoundaryInterface{
	static constexpr ProtectionType Protector = TIME_ACCUMULATION;
	static constexpr const char* format = "\"type\": \"TIME_ACCUMULATION\", \"data\": { \"value\": %s, \"bound\": %s,\"timelimit\": %s }";
	double* src = nullptr;
	double bound;
	float time_limit;
	float frequency;
	Boundary<double,Protector>** external_pointer;
	Boundary(double bound, float time_limit, float frequency, Boundary<double, Protector>*& external_pointer): bound(bound),time_limit(time_limit),frequency(frequency), moving_order(frequency*time_limit/100),
			external_pointer(&external_pointer){
		external_pointer = this;
	};
	Boundary(double* src, Boundary<double, Protector> boundary): src(src),bound(boundary.bound),time_limit(boundary.time_limit),frequency(boundary.frequency),moving_order(frequency*time_limit/100), external_pointer(boundary.external_pointer){
		*external_pointer = this;
	}
	Boundary(double* src, double bound ,float time_limit, float frequency): src(src),bound(bound) ,time_limit(time_limit), frequency(frequency),moving_order(frequency*time_limit/100), external_pointer(nullptr){}

private:
	MeanCalculator<100> mean_calculator;
	vector<double> mean_moving_average;
	uint16_t moving_order = 0;
	uint16_t moving_last = -1;
	uint16_t moving_first = 0;
	uint16_t moving_counter = 0;
	double accumulator = 0;
	bool still_good = true;
public:
	bool check_accumulation(double value){
		if(!still_good) return false;
		mean_calculator.input(abs(value));
		if(mean_calculator.output_value == 0){
			return true;
		}
		mean_calculator.reset();
		if(moving_counter < moving_order) {
			moving_last++;
			mean_moving_average[moving_last] = mean_calculator.output_value;
			accumulator += mean_calculator.output_value;
			moving_counter++;
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

	bool check_bounds() override{
		return still_good;
	}

	int get_string_size()override{
		return snprintf(nullptr,0,format,to_string(*src).c_str(), to_string(time_limit).c_str());
	}
	char* serialize(char* dst)override{
		sprintf(dst, format, to_string(*src).c_str(), to_string(time_limit).c_str());
		return dst;
	}
};

template<>
struct Boundary<float, TIME_ACCUMULATION> : public BoundaryInterface{
	static constexpr ProtectionType Protector = TIME_ACCUMULATION;
	static constexpr const char* format = "\"type\": \"TIME_ACCUMULATION\", \"data\": { \"value\": %s, \"bound\": %s,\"timelimit\": %s }";
	float* src = nullptr;
	float bound;
	float time_limit;
	float frequency;
	bool still_good = true;
	Boundary<float,Protector>** external_pointer;
	Boundary(float bound, float time_limit, float frequency, Boundary<float, Protector>*& external_pointer): bound(bound),time_limit(time_limit) ,frequency(frequency), moving_order(frequency*time_limit/100),
			external_pointer(&external_pointer){
		external_pointer = this;
	};
	Boundary(float* src, Boundary<float, Protector> boundary): src(src),bound(boundary.bound),time_limit(boundary.time_limit),frequency(boundary.frequency),moving_order(frequency*time_limit/100), external_pointer(boundary.external_pointer){
		*external_pointer = this;
	}
	Boundary(float* src, float bound ,float time_limit, float frequency): src(src),bound(bound) ,time_limit(time_limit), frequency(frequency),moving_order(frequency*time_limit/100), external_pointer(nullptr){}

public:
	MeanCalculator<100> mean_calculator;
	vector<float> mean_moving_average;
	uint16_t moving_order = 0;
	uint16_t moving_last = -1;
	uint16_t moving_first = 0;
	uint16_t moving_counter = 0;
	float accumulator = 0;
public:
	bool check_accumulation(float value){
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

	bool check_bounds() override{
		return still_good;
	}

	int get_string_size()override{
		return snprintf(nullptr,0,format,to_string(*src).c_str(), to_string(bound).c_str() ,to_string(time_limit).c_str());
	}
	char* serialize(char* dst)override{
		sprintf(dst, format, to_string(*src).c_str(),to_string(bound).c_str() ,to_string(time_limit).c_str());
		return dst;
	}
};

