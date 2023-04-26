#pragma once

#include "C++Utilities/CppUtils.hpp"
#include "ErrorHandler/ErrorHandler.hpp"
#include "Boundary.hpp"

class Protection{
private:
    char* name = nullptr;
    vector<unique_ptr<BoundaryInterface>> boundaries;
    BoundaryInterface* jumped_protection = nullptr;
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

    string serialize() {
        string ret = "Protection: { Name:" + string(name) + ", ";
        if(jumped_protection != nullptr){
            ret += "Jumped: {" + jumped_protection->serialize() + "}";
        }
        else{
            ret += "Jumped: None";
        }
        ret += " }";
        return ret;
    }
};