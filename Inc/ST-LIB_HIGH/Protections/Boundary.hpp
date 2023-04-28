#pragma once
#define PROTECTIONTYPE_LENGTH 6

enum ProtectionType : uint64_t {
    BELOW = std::numeric_limits<uint64_t>::max() - PROTECTIONTYPE_LENGTH + 1,
    ABOVE,
    OUT_OF_RANGE,
    EQUALS,
    NOT_EQUALS,
	ERROR_HANDLER
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
	static constexpr const char* format = "\"type\": BELOW, \"data\": { \"value\": %s, \"bound\": %s }";
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
		return snprintf(nullptr,0,format,to_string(*src), to_string(boundary));
	}
	char* serialize(char* dst)override{
		sprintf(dst, format, to_string(*src), to_string(boundary));
		return dst;
	}
};

template<class Type>
struct Boundary<Type, ABOVE> : public BoundaryInterface{
	static constexpr ProtectionType Protector = ABOVE;
	static constexpr const char* format = "\"type\": ABOVE, \"data\": { \"value\": %s, \"bound\": %s }";
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
		return snprintf(nullptr,0,format,to_string(*src), to_string(boundary));
	}
	char* serialize(char* dst)override{
		sprintf(dst, format, to_string(*src), to_string(boundary));
		return dst;
	}
};

template<class Type>
struct Boundary<Type, EQUALS> : public BoundaryInterface{
	static constexpr ProtectionType Protector = EQUALS;
	static constexpr const char* format = "\"type\": EQUALS, \"data\": { \"value\": %s, \"bound\": %s }";
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
		return snprintf(nullptr,0,format,to_string(*src), to_string(boundary));
	}
	char* serialize(char* dst)override{
		sprintf(dst, format, to_string(*src), to_string(boundary));
		return dst;
	}
};

template<class Type>
struct Boundary<Type, NOT_EQUALS> : public BoundaryInterface{
	static constexpr ProtectionType Protector = NOT_EQUALS;
	static constexpr const char* format = "\"type\": NOT EQUALS, \"data\": { \"value\": %s, \"bound\": %s }";
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
		return snprintf(nullptr,0,format,to_string(*src), to_string(boundary));
	}
	char* serialize(char* dst)override{
		sprintf(dst, format, to_string(*src), to_string(boundary));
		return dst;
	}
};

template<class Type>
struct Boundary<Type, OUT_OF_RANGE> : public BoundaryInterface{
	static constexpr ProtectionType Protector = OUT_OF_RANGE;
	static constexpr const char* format = "\"type\": OUT OF RANGE, \"data\": { \"value\": %s, \"bounds\": [%s, %s] }";
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
		return snprintf(nullptr,0,format,to_string(*src), to_string(lower_boundary), to_string(upper_boundary));
	}
	char* serialize(char* dst)override{
		sprintf(dst, format, to_string(*src), to_string(lower_boundary), to_string(upper_boundary));
		return dst;
	}
};

template<>
struct Boundary<void, ERROR_HANDLER> : public BoundaryInterface{
	static constexpr ProtectionType Protector = ERROR_HANDLER;
	static constexpr const char* format = "\"type\": ERROR HANDLER, \"data\": %s ";
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

