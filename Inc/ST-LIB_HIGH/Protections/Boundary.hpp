#pragma once
#define PROTECTIONTYPE_LENGTH 5

enum ProtectionType : uint64_t {
    BELOW = std::numeric_limits<uint64_t>::max() - PROTECTIONTYPE_LENGTH + 1,
    ABOVE,
    OUT_OF_RANGE,
    EQUALS,
    NOT_EQUALS
};

struct BoundaryInterface{
    virtual bool check_bounds() = 0;
	virtual string serialize() = 0;
};

template<class Type, ProtectionType Protector> struct Boundary;

template<class Type>
struct Boundary<Type, BELOW> : public BoundaryInterface{
	static constexpr ProtectionType Protector = BELOW;
	Type* src = nullptr;
	Type boundary;
	Boundary(Type boundary): boundary(boundary){};
	Boundary(Type* src, Boundary<Type, Protector> boundary): src(src),boundary(boundary.boundary){}
	Boundary(Type* src, Type boundary): src(src),boundary(boundary){}
	bool check_bounds()override{
		if(*src < boundary) return false;
		return true;
	}
	string serialize()override{
		return "Type: " + (int)Protector + ",Value: " + to_string(*src) + ",Boundary: " + to_string(boundary);
	}
};

template<class Type>
struct Boundary<Type, ABOVE> : public BoundaryInterface{
	static constexpr ProtectionType Protector = ABOVE;
	Type* src = nullptr;
	Type boundary;
	Boundary(Type boundary): boundary(boundary){};
	Boundary(Type* src, Boundary<Type, Protector> boundary): src(src),boundary(boundary.boundary){}
	Boundary(Type* src, Type boundary): src(src),boundary(boundary){}
	bool check_bounds()override{
		if(*src > boundary) return false;
		return true;
	}
	string serialize()override{
		return "Type: " + (int)Protector + ",Value: " + to_string(*src) + ",Boundary: " + to_string(boundary);
	}
};

template<class Type>
struct Boundary<Type, EQUALS> : public BoundaryInterface{
	static constexpr ProtectionType Protector = EQUALS;
	Type* src = nullptr;
	Type boundary;
	Boundary(Type boundary): boundary(boundary){};
	Boundary(Type* src, Boundary<Type, Protector> boundary): src(src),boundary(boundary.boundary){}
	Boundary(Type* src, Type boundary): src(src),boundary(boundary){}
	bool check_bounds()override{
		if(*src == boundary) return false;
		return true;
	}
	string serialize()override{
		return "Type: " + (int)Protector + ",Value: " + to_string(*src) + ",Boundary: " + to_string(boundary);
	}
};

template<class Type>
struct Boundary<Type, NOT_EQUALS> : public BoundaryInterface{
	static constexpr ProtectionType Protector = NOT_EQUALS;
	Type* src = nullptr;
	Type boundary;
	Boundary(Type boundary): boundary(boundary){};
	Boundary(Type* src, Boundary<Type, Protector> boundary): src(src),boundary(boundary.boundary){}
	Boundary(Type* src, Type boundary): src(src),boundary(boundary){}
	bool check_bounds()override{
		if(*src != boundary) return false;
		return true;
	}
	string serialize()override{
		return "Type: " + (int)Protector + ",Value: " + to_string(*src) + ",Boundary: " + to_string(boundary);
	}
};

template<class Type>
struct Boundary<Type, OUT_OF_RANGE> : public BoundaryInterface{
	static constexpr ProtectionType Protector = OUT_OF_RANGE;
	Type* src = nullptr;
	Type lower_boundary, upper_boundary;
	Boundary(Type lower_boundary, Type upper_boundary): lower_boundary(lower_boundary), upper_boundary(upper_boundary){};
	Boundary(Type* src, Boundary<Type, Protector> boundary): src(src),lower_boundary(boundary.lower_boundary),upper_boundary(boundary.upper_boundary){}
	Boundary(Type* src, Type lower_boundary, Type upper_boundary): src(src), lower_boundary(lower_boundary), upper_boundary(upper_boundary){}
	bool check_bounds()override{
		if(*src < lower_boundary || *src > upper_boundary) return false;
		return true;
	}
	string serialize()override{
		return "Type: " + (int)Protector + ",Value: " + to_string(*src) + ",Boundary_1: " + to_string(lower_boundary) + ",Boundary_2: " + to_string(upper_boundary);
	}
};

