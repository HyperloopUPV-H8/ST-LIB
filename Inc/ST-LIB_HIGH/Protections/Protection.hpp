//#pragma once
//
//#include "C++Utilities/CppUtils.hpp"
//#include "ErrorHandler/ErrorHandler.hpp"
//#include "Boundary.hpp"
//
//template<class... Types> class Protection;
//
//template<>
//class Protection<>{
//public:
//	Protection() = default;
//    char* name = nullptr;
//
//    virtual bool check_state() = 0;
//};
//
//template<class Type>
//class Protection<Type> : public Protection<> {
//public:
//    Type* src;
//    Boundary<>* jumped_protection = nullptr;
//    vector<unique_ptr<Boundary<>>> boundaries;
//
//    Protection(Type* src);
//    template<ProtectionType Protector> requires requires {Protector != OUT_OF_RANGE;}
//    Protection(Type* src, Protector, Type boundary);
//    template<ProtectionType Protector> requires requires (Protector protector){protector == OUT_OF_RANGE;}
//    Protection(Type* src, ProtectionType protection_type, Type lower_boundary, Type upper_boundary);
//
//    void add_protection(ProtectionType protection_type, Type boundary);
//    template<class... AdditionalProtections>
//    void add_protection(Type upper_boundary, ProtectionType protection, AdditionalProtections...);
//    void add_protection(Type last_boundary);
//    template<class... AdditionalProtections>
//    void add_protection(ProtectionType protection_type, Type boundary, AdditionalProtections...);
//
//    bool check_state();
//};
//
//template<class Type>
//Protection<Type>::Protection(Type* src): src(src) {}
//
//template<class Type>
//template<ProtectionType Protector> requires requires (){is_same<typename decltype(Protector),typename ProtectionType>::value; Protector != OUT_OF_RANGE;}
//Protection<Type>::Protection(Type* src,  Protector, Type boundary) :src(src) {
//
//}
//
//template<class Type>
//template<ProtectionType Protector> requires requires (Protector protector){protector == OUT_OF_RANGE;}
//Protection<Type>::Protection(Type* src, Protector protection_type, Type lower_boundary, Type upper_boundary) :src(src) {
//    if (protection_type != OUT_OF_RANGE) {
//    	ErrorHandler("You can not use two values for this protection");
//        return;
//    }
//    protections.push_back(protection_type);
//    boundaries.push_back(lower_boundary);
//    boundaries.push_back(upper_boundary);
//}
//
//template<class Type>
//void Protection<Type>::add_protection(ProtectionType protection_type, Type boundary) {
//    protections.push_back(protection_type);
//    boundaries.push_back(boundary);
//}
//
//template<class Type>
//void Protection<Type>::add_protection(Type last_boundary) {
//    boundaries.push_back(last_boundary);
//}
//
//template<class Type>
//bool Protection<Type>::check_state() {
//	for(unique_ptr<Boundary<>> bound: boundaries){
//		if(not bound.get()->check_bounds()){
//			jumped_protection = bound.get();
//			return false;
//		}
//	}
//	return true;
//}
//
//template<class Type>
//template<class... AdditionalProtections>
//void Protection<Type>::add_protection(Type upper_boundary, ProtectionType protection, AdditionalProtections... rest) {
//
//}
//
//template<class Type>
//template<class... AdditionalProtections>
//void Protection<Type>::add_protection(ProtectionType protection_type, Type boundary,AdditionalProtections... protections) {
//
//}
