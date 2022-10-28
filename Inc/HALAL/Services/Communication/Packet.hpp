#pragma once

#include "PacketValue.hpp"

struct empty_type{};

template<typename... ElementTypes> class Packet;

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
};

template<typename Type, typename... Types>
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

	Packet(int id, PacketValue<Type> a, PacketValue<Types>... args);

	template<int I>
	const auto& get() const;

	template<int I>
	auto& get();

	uint8_t* build();

	template <size_t I = 0>
	void calculate_sizes();

	template <size_t I = 0>
	void fill_buffer();


public:
	uint16_t bffr_size = sizeof(id);
	uint16_t ptr_loc = sizeof(id);
	bool has_been_built = false;
};





