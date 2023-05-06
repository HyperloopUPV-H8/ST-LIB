#pragma once

#include "C++Utilities/CppUtils.hpp"

template <class T>
concept Container = requires(T a, const T b)
{
    requires std::regular<T>;
    requires std::swappable<T>;
    requires std::same_as<typename T::reference, typename T::value_type &>;
    requires std::same_as<typename T::const_reference, const typename T::value_type &>;
    requires std::forward_iterator<typename T::iterator>;
    requires std::forward_iterator<typename T::const_iterator>;
    requires std::signed_integral<typename T::difference_type>;
    requires std::same_as<typename T::difference_type, typename std::iterator_traits<typename T::iterator>::difference_type>;
    requires std::same_as<typename T::difference_type, typename std::iterator_traits<typename T::const_iterator>::difference_type>;
    { a.begin() } -> std::same_as<typename T::iterator>;
    { a.end() } -> std::same_as<typename T::iterator>;
    { b.begin() } -> std::same_as<typename T::const_iterator>;
    { b.end() } -> std::same_as<typename T::const_iterator>;
    { a.cbegin() } -> std::same_as<typename T::const_iterator>;
    { a.cend() } -> std::same_as<typename T::const_iterator>;
    { a.size() } -> std::same_as<typename T::size_type>;
    { a.max_size() } -> std::same_as<typename T::size_type>;
    { a.empty() } -> std::convertible_to<bool>;
};

template <class T>
concept NotContainer = !Container<T>;

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
PacketValue(Type*)->PacketValue<Type>;
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
PacketValue(Type*)->PacketValue<Type>;
#endif

template<class Type,size_t N> 
class PacketValue<Type(&)[N]>: public PacketValue<> {
public:
    using value_type = Type;
    Type(*src )[N] = nullptr;
    PacketValue() = default;
    PacketValue(Type(*src)[N]): src(src) {}
    ~PacketValue() = default;
    void* get_pointer() override {
        return src;
    }
    size_t get_size() override {
        return N*sizeof(Type);
    }
    void parse(void* data) override {
        memcpy(src, data, get_size());
    }
    void copy_to(void* data) override {
        memcpy(data, src, get_size());
    }
};

#if __cpp_deduction_guides >= 201606
template<class Type,size_t N> 
PacketValue(Type(*)[N])->PacketValue<Type(&)[N]>;
#endif


template<class Type,size_t N> 
class PacketValue<Type*(&)[N]>: public PacketValue<> {
public:
    using value_type = Type*;
    Type*(*src )[N] = nullptr;
    PacketValue() = default;
    PacketValue(Type*(*src)[N]): src(src) {}
    ~PacketValue() = default;
    void* get_pointer() override {
        return src;
    }
    size_t get_size() override {
        return N*sizeof(Type);
    }
    void parse(void* data) override {
        for(Type* i : *src) {
            *i = *(Type*)data;
            data += sizeof(Type);
        }
    }
    void copy_to(void* data) override {
        for(Type* i : *src) {
            *(Type*)data = *i;
            data += sizeof(Type);
        }
    }
};

#if __cpp_deduction_guides >= 201606 
template<class Type,size_t N>
PacketValue(Type*(*)[N])->PacketValue<Type*(&)[N]>;
#endif
