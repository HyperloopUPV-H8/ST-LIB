#pragma once
#define PROTECTIONTYPE_LENGTH 5

enum ProtectionType : uint64_t {
    BELOW = std::numeric_limits<uint64_t>::max() - PROTECTIONTYPE_LENGTH + 1,
    ABOVE,
    OUT_OF_RANGE,
    EQUALS,
    NOT_EQUALS
};

template<class... Types> struct Boundary;

struct Boundary<>{
    static map<ProtectionType, string> protection_type_name;
    virtual bool check_bounds() = 0;
};

template<class Type>
struct Boundary<Type, BELOW> {
	using Protector = BELOW;
	Type* src;
	Type boundary;
	Boundary(Type* src, Type boundary): src(src),boundary(boundary){}
	bool check_bounds()override{
		if(*src < boundary) return false;
		return true;
	}
};

template<class Type>
struct Boundary<Type, ABOVE> {
	using Protector = ABOVE;
	Type* src;
	Type boundary;
	Boundary(Type* src, Type boundary): src(src),boundary(boundary){}
	bool check_bounds()override{
		if(*src > boundary) return false;
		return true;
	}
};

template<class Type>
struct Boundary<Type, EQUALS> {
	using Protector = EQUALS;
	Type* src;
	Type boundary;
	Boundary(Type* src, Type boundary): src(src),boundary(boundary){}
	bool check_bounds()override{
		if(*src == boundary) return false;
		return true;
	}
};

template<class Type>
struct Boundary<Type, NOT_EQUALS> {
	using Protector = NOT_EQUALS;
	Type* src;
	Type boundary;
	Boundary(Type* src, Type boundary): src(src),boundary(boundary){}
	bool check_bounds()override{
		if(*src != boundary) return false;
		return true;
	}
};

template<class Type>
struct Boundary<Type, OUT_OF_RANGE> {
	using Protector = OUT_OF_RANGE;
	Type* src;
	Type lower_boundary, upper_boundary;
	Boundary(Type* src, Type boundary): src(src),boundary(boundary){}
	bool check_bounds()override{
		if(*src < lower_boundary || *src > upper_boundary) return false;
		return true;
	}
};
