#pragma once

#include "C++Utilities/CppUtils.hpp"

template<typename T>
struct has_const_iterator
{
private:
    typedef char                      yes;
    typedef struct { char array[2]; } no;

    template<typename C> static yes test(typename C::const_iterator*);
    template<typename C> static no  test(...);
public:
    static const bool value = sizeof(test<T>(0)) == sizeof(yes);
    typedef T type;
};

template <typename T>
struct has_begin_end
{
    template<typename C> static char(&f(typename std::enable_if<
        std::is_same<decltype(static_cast<typename C::const_iterator(C::*)() const>(&C::begin)),
        typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

    template<typename C> static char(&f(...))[2];

    template<typename C> static char(&g(typename std::enable_if<
        std::is_same<decltype(static_cast<typename C::const_iterator(C::*)() const>(&C::end)),
        typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

    template<typename C> static char(&g(...))[2];

    static bool const beg_value = sizeof(f<T>(0)) == 1;
    static bool const end_value = sizeof(g<T>(0)) == 1;
};

template<typename T>
struct is_container : integral_constant<bool, has_const_iterator<T>::value&& has_begin_end<T>::beg_value&& has_begin_end<T>::end_value>{};

template<class Type>
concept Container = is_container<Type>::value;

template<class Type>
concept NotContainer = !is_container<Type>::value;

template<class BaseType>
concept CustomButNotContainer = NotContainer<BaseType> && NotIntegral<BaseType>;

template<class... Types> class PacketValue;

template<class ConversionType>
class PacketValue<ConversionType>{
public:
    using value_type = ConversionType;
    double* src;
    double factor;

    PacketValue() = default;
    PacketValue(double* src, double factor):src(src),factor(factor !=0 ? factor : 1){}

    ConversionType convert(){
	    return static_cast<ConversionType>((*src) * factor);
    }

    void load(ConversionType new_data){
        *src = static_cast<double>(new_data / factor);
    }

    size_t size(){
	    return sizeof(ConversionType);
    }
};

template<class BaseType> requires CustomButNotContainer<BaseType>
class PacketValue<BaseType>{
public:
    using value_type = BaseType;
    BaseType* src;

    PacketValue() = default;
    PacketValue(BaseType* src) : src(src){}

    BaseType convert() {
        return *src;
    }

    void load(BaseType new_data) {
        *src = new_data;
    }

    size_t size() {
        return sizeof(BaseType);
    }
};

template<class BaseType> requires Container<BaseType>
class PacketValue<BaseType> {
public:
    using value_type = BaseType;
    BaseType* src;

    PacketValue() = default;
    PacketValue(BaseType* src) : src(src) {
        if constexpr (is_same<string,BaseType>::value) {
            is_string = true;
        }
    }

    auto* convert() {
        return src->data();
    }

    void load(BaseType* new_data) {
        memcpy(src->data(), new_data, size());
    }

    size_t size() {
        return src->size() * sizeof(typename remove_reference<decltype(*src)>::type::value_type) + is_string;
    }
private:
    bool is_string = false;
};

#if __cpp_deduction_guides >= 201606
template<Container BaseType>
PacketValue(BaseType* src)->PacketValue<BaseType>;
template<CustomButNotContainer BaseType>
PacketValue(BaseType* src)->PacketValue<BaseType>;
#endif

