#pragma once

#include "PacketValue.hpp"

struct empty_type{};

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
	Packet(uint16_t id): id(static_cast<decltype(this->id)>(id)){}
	Packet() = default;

	static decltype(id) get_id(uint8_t* new_data){
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

	template<int I>
	const auto& get() const;

	template<int I>
	auto& get();

	uint8_t* build();

	template <size_t I = 0>
	void calculate_sizes();

	template <size_t I = 0>
	void fill_buffer();

	bool check_id(uint8_t* new_data);

    decltype(Packet<Type,Types...>::id) get_id();

    template <size_t I = 0>
    void load_data(uint8_t* new_data);

    void save_data(uint8_t* new_data);

public:
	uint16_t buffer_size = sizeof(id);
	uint16_t ptr_loc = sizeof(id);
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
Packet<Type, Types...>::Packet() = default;

template<class Type, class... Types>
Packet<Type, Types...>::Packet(uint16_t id) : id(id) {}

template<class Type, class... Types>
Packet<Type, Types...>::Packet(uint16_t id, PacketValue<Type> value, PacketValue<Types>... args) : Packet<Types...>(id, args...), id(id), value(value) {
	Packet<>::save_by_id[this->id] = [this](uint8_t* new_data) {save_data(new_data); };
}

template<class Type, class... Types> template<int I>
const auto& Packet<Type, Types...>::get() const{
	if constexpr (I == 0){
		return this->value;
	}
	else if constexpr (I > 0){
		return static_cast<const base_type&>(*this). template get<I - 1>();
	}
}

template<class Type, class... Types> template<int I>
auto& Packet<Type, Types...>::get(){
	if constexpr (I == 0){
		return this->value;
	}
	else if constexpr (I > 0){
		return static_cast<base_type&>(*this). template get<I - 1>();
	}
}

template<class Type, class... Types> template <size_t I = 0>
void Packet<Type, Types...>::calculate_sizes() {
	if constexpr (I == number_of_values) {
		return;
	}
	else if constexpr (I < number_of_values) {
		auto elem = get<I>();
		using cast_type = remove_reference<decltype(elem)>::type::value_type;
		if constexpr (is_container<cast_type>::value) {
			has_container_values = true;
		}
		buffer_size += (uint16_t)elem.size();
		calculate_sizes<I + 1>();
	}
}

template<class Type, class... Types> template <size_t I = 0>
void Packet<Type, Types...>::fill_buffer() {
	if constexpr (I == number_of_values) {
		return;
	}
	else if constexpr (I < number_of_values) {
		auto elem = get<I>();
		using cast_type = remove_reference<decltype(elem)>::type::value_type;
		if constexpr (is_container<cast_type>::value) {
			memcpy(buffer + ptr_loc, elem.convert(), elem.size());
		}
		else {
			auto data = elem.convert();
			memcpy(buffer + ptr_loc, &data, elem.size());
		}
		ptr_loc += (uint16_t)elem.size();
		fill_buffer<I + 1>();
	}
}

template<class Type, class... Types>
uint8_t* Packet<Type, Types...>::build() {
	if (!has_been_built || has_container_values) {
		buffer_size = sizeof(id);
		calculate_sizes();
		if(buffer != nullptr){
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

template<class Type, class... Types> template <size_t I = 0>
void Packet<Type, Types...>::load_data(uint8_t* new_data) {
    if constexpr (I == number_of_values) {
        return;
    }
    else if constexpr (I < number_of_values) {
        auto elem = get<I>();
        using cast_type = remove_reference<decltype(elem)>::type::value_type;
        cast_type* new_value = (cast_type*)(new_data+ptr_loc);
        if constexpr (is_container<cast_type>::value) {
            elem.load(new_value);
        }
		else {
            elem.load(*new_value);
        }
        ptr_loc += (uint16_t)elem.size();
        load_data<I + 1>(new_data);
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
void Packet<Type, Types...>::save_data(uint8_t* new_data) {
    if (!check_id(new_data)) {
        return;
    }
    load_data(new_data);
    ptr_loc = sizeof(id);
}


template<class Type, class... Types>
decltype(Packet<Type,Types...>::id) Packet<Type, Types...>::get_id() {
    return this->id;
}

