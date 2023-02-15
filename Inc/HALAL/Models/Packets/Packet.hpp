#pragma once

#include "PacketValue.hpp"

struct empty_type {};

template<class... ValueTypes> class Packet;

template<>
class Packet<>
{
public:
    uint16_t id;
    using value_type = empty_type;
    static constexpr size_t number_of_values = 0;
    using base_type = empty_type;
    static map<decltype(Packet<>::id), function<void(uint8_t*)>> save_by_id;
    static map<decltype(Packet<>::id), void(*)()> on_received;

public:
    Packet(uint16_t id) : id(static_cast<decltype(this->id)>(id)) {}
    Packet() = default;

    void fill_buffer(uint8_t* buffer, size_t& ptr_loc) {}
    void calculate_sizes(bool& has_container_values , size_t& bffr_size) {}
    void load_data(uint8_t* new_data, size_t& ptr_loc) {}

    static uint16_t get_id(uint8_t* new_data) {
        return *(decltype(id)*)new_data;
    }
};

template<class Type, class... Types>
class Packet<Type, Types...> : public Packet<Types...>
{
public:
    uint16_t id = 0;
    uint8_t* buffer = nullptr;
    using value_type = Type;
    static constexpr size_t number_of_values = 1 + sizeof...(Types);
    using base_type = Packet<Types...>;

protected:
    PacketValue<value_type> value;

public:
    Packet();
    Packet(uint16_t id);
    Packet(uint16_t id, PacketValue<Type> value, PacketValue<Types>... args);
    Packet(PacketValue<Type> value, PacketValue<Types>... args);
    ~Packet();

    template<int I>
    const auto& get() const;

    template<int I>
    auto& get();

    uint8_t* build();

    void calculate_sizes();

    void calculate_sizes(bool& has_container_values, size_t& buffer_size);

    void fill_buffer();

    void fill_buffer(uint8_t* buffer, size_t& ptr_loc) requires NotContainer<Type>;

    void fill_buffer(uint8_t* buffer, size_t& ptr_loc) requires Container<Type>;

    bool check_id(uint8_t* new_data);

    decltype(Packet<Type, Types...>::id) get_id();

    void load_data(uint8_t* new_data);

    void load_data(uint8_t* new_data, size_t& ptr_loc);

    void save_data(uint8_t* new_data);

    template <class FunctionType>
    void for_each(FunctionType& callback);

    template <class FunctionType>
    void for_each(FunctionType&& callback);

public:
    size_t buffer_size = sizeof(id);
    size_t ptr_loc = sizeof(id);
    bool has_been_built = false;
    bool has_container_values = false;
};

#if __cpp_deduction_guides >= 201606
template<class Type, class... Types>
Packet(uint16_t id, PacketValue<Type> value, PacketValue<Types>... args)->Packet<Type, Types...>;
template<class Type>
Packet(uint16_t id, PacketValue<Type> value)->Packet<Type>;
Packet(uint16_t id)->Packet<>;
#endif

template<class Type, class... Types>
Packet<Type, Types...>::Packet(): value(*(PacketValue<Type>*)(0)) {}

template<class Type, class... Types>
Packet<Type, Types...>::Packet(uint16_t id) : id(id) {}

template<class Type, class... Types>
Packet<Type, Types...>::Packet(uint16_t id, PacketValue<Type> value, PacketValue<Types>... args) : Packet<Type,Types...>(value,args...) {
    this->id = id;
    Packet<>::save_by_id[this->id] = [this](uint8_t* new_data) {save_data(new_data); };
}

template<class Type, class... Types>
Packet<Type, Types...>::Packet(PacketValue<Type> value, PacketValue<Types>... args) : Packet<Types...>(args...), value(value) {}

template<class Type, class... Types>
Packet<Type, Types...>::~Packet(){
    if(buffer != nullptr){
        free(buffer);
    }
}

template<class Type, class... Types> template<int I>
const auto& Packet<Type, Types...>::get() const {
    if constexpr (I == 0) {
        return this->value;
    }
    else if constexpr (I > 0) {
        return static_cast<const base_type&>(*this). template get<I - 1>();
    }
}

template<class Type, class... Types> template<int I>
auto& Packet<Type, Types...>::get() {
    if constexpr (I == 0) {
        return this->value;
    }
    else if constexpr (I > 0) {
        return static_cast<base_type&>(*this). template get<I - 1>();
    }
}

template<class Type, class... Types>
void Packet<Type, Types...>::calculate_sizes() {
    calculate_sizes(has_container_values, buffer_size);
}

template<class Type, class... Types>
void Packet<Type, Types...>::calculate_sizes(bool& has_container_values, size_t& buffer_size) {
    if constexpr (number_of_values <= 0) {
        return;
    }
    else {
        using cast_type = remove_reference<decltype(this->value)>::type::value_type;
        if constexpr (is_container<cast_type>::value) {
            has_container_values = true;
        }
        buffer_size += this->value.size();
        static_cast<base_type*>(this)->calculate_sizes(has_container_values, buffer_size);
    }
}

template<class Type, class... Types>
void Packet<Type, Types...>::fill_buffer() {
    fill_buffer(buffer, ptr_loc);
}

template<class Type, class... Types>
void Packet<Type, Types...>::fill_buffer(uint8_t* buffer,size_t& ptr_loc) requires NotContainer<Type> {
    if constexpr (number_of_values <= 0) {
        return;
    }
    else {
        using cast_type = remove_reference<decltype(this->value)>::type::value_type;
        *((cast_type*)(buffer + ptr_loc)) = *(this->value).convert();
        ptr_loc += this->value.size();
        static_cast<base_type*>(this)->fill_buffer(buffer, ptr_loc);
    }
}

template<class Type, class... Types>
void Packet<Type, Types...>::fill_buffer(uint8_t* buffer, size_t& ptr_loc)  requires Container<Type>{
    if constexpr (number_of_values <= 0) {
        return;
    }
    else {
        memcpy(buffer + ptr_loc, this->value.convert(), this->value.size());
        ptr_loc += this->value.size();
        static_cast<base_type*>(this)->fill_buffer(buffer, ptr_loc);
    }
}

template<class Type, class... Types>
uint8_t* Packet<Type, Types...>::build() {
    if (!has_been_built || has_container_values) {
        buffer_size = sizeof(id);
        calculate_sizes();
        if (buffer != nullptr) {
            free(buffer);
        }
        buffer = (uint8_t*)malloc(buffer_size);
        memcpy(buffer, &id, sizeof(id));
    }
    fill_buffer();
    ptr_loc = sizeof(id);
    has_been_built = true;
    return this->buffer;
}

template<class Type, class... Types>
void Packet<Type, Types...>::load_data(uint8_t* new_data) {
    load_data(new_data, ptr_loc);
}

template<class Type, class... Types>
void Packet<Type, Types...>::load_data(uint8_t * new_data, size_t& ptr_loc) {
    if constexpr (number_of_values <= 0) {
        return;
    }
    else {
        using cast_type = remove_reference<decltype(this->value)>::type::value_type;
        this->value.load((cast_type*)(new_data + ptr_loc));
        ptr_loc += this->value.size();
        static_cast<base_type*>(this)->load_data(new_data,ptr_loc);
    }
}

template<class Type, class ... Types>
bool Packet<Type, Types...>::check_id(uint8_t* new_data) {
    if (id != *(uint16_t*)new_data) {
        return false;
    }
    return true;
}

template<class Type, class... Types>
void Packet<Type, Types...>::save_data(uint8_t * new_data) {
    if (!check_id(new_data)) {
        return;
    }
    load_data(new_data);
    ptr_loc = sizeof(id);
}


template<class Type, class... Types>
uint16_t Packet<Type, Types...>::get_id() {
    return this->id;
}

template<class Type, class... Types> template<class FunctionType>
void Packet<Type, Types...>::for_each(FunctionType& callback) {
    if constexpr (number_of_values <= 0) {
        return;
    }
    else if constexpr (number_of_values == 1) {
        callback(this->value);
        return;
    }
    else {
        callback(this->value);
        static_cast<base_type*>(this)->for_each(callback);
    }
}

template<class Type, class... Types> template<class FunctionType>
void Packet<Type, Types...>::for_each(FunctionType&& callback) {
    if constexpr (number_of_values <= 0) {
        return;
    }
    else if constexpr (number_of_values == 1) {
        invoke(callback, this->value);
        return;
    }
    else {
        invoke(callback, this->value);
        static_cast<base_type*>(this)->for_each(callback);
    }
}
