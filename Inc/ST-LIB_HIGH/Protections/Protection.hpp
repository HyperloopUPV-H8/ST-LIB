#pragma once

#include "C++Utilities/CppUtils.hpp"
#include "ErrorHandler/ErrorHandler.hpp"
#include "Boundary.hpp"



class Protection{
private:
    char* name = nullptr;
    vector<unique_ptr<BoundaryInterface>> boundaries;
    BoundaryInterface* fault_protection = nullptr;
    static constexpr Protections::FaultType fault_type = Protections::FaultType::FAULT;
    uint8_t triggered_protecions_idx[4]{};
public:
    vector<BoundaryInterface*> warnings_triggered;
    template<class Type, ProtectionType... Protector, template<class,ProtectionType> class Boundaries>
    Protection(Type* src, Boundaries<Type,Protector>... protectors) {
        (boundaries.push_back(unique_ptr<BoundaryInterface>(new Boundary<Type,Protector>(src,protectors))), ...);
    }

    void set_name(char* name) {
        this->name = name;
    }

    char* get_name() {
        return name;
    }

    Protections::FaultType check_state() {
        uint8_t warning_count = 0;
        //to save the index of the triggered warning
        uint8_t idx = 0;
        for(unique_ptr<BoundaryInterface>& bound: boundaries){
            auto fault_type = bound.get()->check_bounds();
            idx++;
            switch(fault_type){
                // in case a Protection has more than one boundary, give priority to fault messages
                case Protections::FAULT:
                    fault_protection = bound.get();
                    // adding the fault_protection to the vector is not desired,
                    // the fault signal should propagate as fast as possible
                    return Protections::FAULT;
                case Protections::WARNING:
                    //warnings are non fatal, but we cannot waste time, we need to check if any
                    //faults were triggered
                    triggered_protecions_idx[warning_count] = idx-1;
                    warning_count++;
                    break;
                default:
                    break;
            }
        }
        if(warning_count){
            for(uint8_t i = 0; i < warning_count; i++){
                warnings_triggered.push_back(boundaries[triggered_protecions_idx[i]].get());
            }
            return Protections::WARNING;
        }
        return Protections::OK;
    }
    friend class ProtectionManager;
};
