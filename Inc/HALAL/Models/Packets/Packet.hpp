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
	static constexpr size_t nbr_of_values = 0;
	using base_type = empty_type;

public:
	Packet(int id);

	Packet();

	static decltype(id) get_id(uint8_t* new_data);
};

extern map<decltype(Packet<>::id), function<void(uint8_t*)>> save_packet_by_id;

template<class Type, class... Types>
class Packet<Type, Types...> : public Packet<Types...>
{
public:
	uint16_t id = 0;
	uint8_t* bffr = nullptr;
	using value_type = Type;
	static constexpr size_t nbr_of_values = 1 + sizeof...(Types);
	using base_type = Packet<Types...>;

protected:
	PacketValue<value_type> value;

public:
	Packet();

	Packet(int id);

	Packet(int id, PacketValue<Type> value, PacketValue<Types>... args);

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
	uint16_t bffr_size = sizeof(id);
	uint16_t ptr_loc = sizeof(id);
	bool has_been_built = false;
};

#if __cpp_deduction_guides >= 201606
template<class Type, class... Types>
Packet(int id, PacketValue<Type> value, PacketValue<Types>... args)->Packet<Type, Types...>;
template<class Type>
Packet(int id, PacketValue<Type> value)->Packet<Type>;
Packet(int id)->Packet<>;
#endif





