
#include "../Inc/HALAL/Services/Communication/Packet.hpp"

Packet<>::Packet(int id) : id(static_cast<uint16_t>(id)) {}

Packet<>::Packet() = default;

template<typename Type, typename... Types>
Packet<Type, Types...>::Packet() = default;

template<typename Type, typename... Types>
Packet<Type, Types...>::Packet(int id) : id(static_cast<uint16_t>(id)) {}

template<typename Type, typename... Types>
Packet<Type, Types...>::Packet(int id, PacketValue<Type> a, PacketValue<Types>... args) : Packet<Types...>(id, args...), value(a), id(id) {}

template<class Type, class ... Types> template<int I>
const auto& Packet<Type, Types...>::get() const{
	if constexpr (I == 0){
		return this->value;
	}
	else if constexpr (I > 0){
		return static_cast<const base_type&>(*this). template get<I - 1>();
	}
}

template<class Type, class ... Types> template<int I>
auto& Packet<Type, Types...>::get(){
	if constexpr (I == 0){
		return this->value;
	}
	else if constexpr (I > 0){
		return static_cast<base_type&>(*this). template get<I - 1>();
	}
}

template<class Type, class ... Types>
uint8_t* Packet<Type, Types...>::build() {
	if (!has_been_built) {
		calculate_sizes();
		bffr = (uint8_t*)malloc(bffr_size);
		memcpy(bffr, &id, sizeof(id));
	}
	fill_buffer();
	ptr_loc = sizeof(id);
	return this->bffr;
}

template<class Type, class ... Types> template <size_t I = 0>
void Packet<Type, Types...>::calculate_sizes() {
	if constexpr (I == nbr_of_values) {
		return;
	}
	else if constexpr (I < nbr_of_values) {
		auto elem = get<I>();
		bffr_size += (uint16_t)elem.size();
		calculate_sizes<I + 1>();
	}
}

template<class Type, class ... Types> template <size_t I = 0>
void Packet<Type, Types...>::fill_buffer() {
	if constexpr (I == nbr_of_values) {
		return;
	}
	else if constexpr (I < nbr_of_values) {
		auto elem = get<I>();
		auto data = elem.convert();
		memcpy(bffr + ptr_loc, &data, elem.size());
		ptr_loc += (uint16_t)elem.size();
		fill_buffer<I + 1>();
	}
}
template<class Type, class ... Types>
bool Packet<Type, Types...>::check_id(uint8_t* new_data) {
    if (id != *(uint16_t*)new_data) {
        return false;
    }
    return true;
}

template<class Type, class ... Types> template <size_t I = 0>
void Packet<Type, Types...>::load_data(uint8_t* new_data) {
    if constexpr (I == nbr_of_values) {
        return;
    }
    else if constexpr (I < nbr_of_values) {
        auto elem = get<I>();
        using cast_type = remove_reference<decltype(elem)>::type::value_type;
        cast_type* new_value = (cast_type*)(new_data+ptr_loc);
        elem.load(*new_value);
        ptr_loc += (uint16_t)elem.size();
        load_data<I + 1>(new_data);
    }
}

template<class Type, class ... Types>
void Packet<Type, Types...>::save_data(uint8_t* new_data) {
    if (!check_id(new_data)) {
        return;
    }
    load_data(new_data);
    ptr_loc = sizeof(id);
}

