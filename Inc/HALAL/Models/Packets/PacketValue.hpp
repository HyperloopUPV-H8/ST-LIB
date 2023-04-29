#pragma once

#include "C++Utilities/CppUtils.hpp"
#include "Concepts/Concepts.hpp"

struct empty_type {};

template <class... Types> class PacketValue;

template<>
class PacketValue<> {
public:
    using value_type = empty_type;
    PacketValue() = default;
    ~PacketValue() = default;
    virtual void* get_pointer() = 0;
    virtual size_t get_size() = 0;
    virtual void parse(void* data) = 0;
    virtual void copy_to(void* data) = 0;
};

template<class Type> requires NotContainer<Type>
class PacketValue<Type>: public PacketValue<> {
public:
    using value_type = Type;
    Type* src = nullptr;
    PacketValue() = default;
    PacketValue(Type* src): src(src) {}
    ~PacketValue() = default;
    void* get_pointer() override {
        return src;
    }
    size_t get_size() override {
        return sizeof(Type);
    }
    void parse(void* data) override {
        *src = *((Type*)data);
    }
    void copy_to(void* data) override {
        *((Type*)data) = *src;
    }
};

#if __cpp_deduction_guides >= 201606
template<class Type> requires NotContainer<Type>
PacketValue(Type)->PacketValue<Type>;
#endif

template<class Type> requires Container<Type>
class PacketValue<Type>: public PacketValue<> {
public:
    using value_type = Type;
    Type* src = nullptr;
    PacketValue() = default;
    PacketValue(Type* src): src(src) {}
    ~PacketValue() = default;
    void* get_pointer() override {
        return src->data();
    }
    size_t get_size() override {
        return src->size()*sizeof(typename Type::value_type);
    }
    void parse(void* data) override {
        memcpy(src->data(), data, get_size());
    }
    void copy_to(void* data) override {
        memcpy(data, src->data(), get_size());
    }
};

template<>
class PacketValue<string> : public PacketValue<>{
public:
    using value_type = string;
    string* src = nullptr;
    PacketValue() = default;
    PacketValue(string* src): src(src) {}
    ~PacketValue() = default;
    void* get_pointer() override {
        return src->data();
    }
    size_t get_size() override {
        return strlen(src->c_str()) + 1;
    }
    void parse(void* data) override {
        memcpy(src->data(), data, get_size());
    }
    void copy_to(void* data) override {
        memcpy(data, src->data(), get_size());
    }
};

#if __cpp_deduction_guides >= 201606
template<class Type> requires Container<Type>
PacketValue(Type)->PacketValue<Type>;
#endif
