#pragma once

#include "C++Utilities/CppUtils.hpp"
#include "ErrorHandler/ErrorHandler.hpp"
#include "Boundary.hpp"

enum FaultType{
	FAULT,
	WARNING
};

class Protection{
private:
	static constexpr const char* format = "\"protection\" : {\"name\":\"%s\", %s}";
    char* name = nullptr;
    vector<unique_ptr<BoundaryInterface>> boundaries;
    BoundaryInterface* jumped_protection = nullptr;
    static constexpr FaultType fault_type = FaultType::FAULT;
public:
    template<class Type, ProtectionType... Protector, template<class,ProtectionType> class Boundaries>
    Protection(Type* src, Boundaries<Type,Protector>... protectors) {
        boundaries.push_back(unique_ptr<BoundaryInterface>(new Boundary<Type,Protector>(src,protectors)...));
    }

    void set_name(char* name) {
        this->name = name;
    }

    char* get_name() {
        return name;
    }

    bool check_state() {
        for(unique_ptr<BoundaryInterface>& bound: boundaries){
            if(not bound.get()->check_bounds()){
                jumped_protection = bound.get();
                return false;
            }
        }
        return true;
    }

    char* serialize(char* dst) {
    	sprintf(dst,format,name,string(jumped_protection->serialize(dst)).c_str());
    	return dst;
    }

    int get_string_size(){
    	return jumped_protection->get_string_size() + snprintf(nullptr,0,format, name, "");
    }

    friend class ProtectionManager;
};
