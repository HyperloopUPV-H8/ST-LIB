/*
 * Board.hpp
 *
 *  Created on: 20 feb. 2023
 *      Author: aleja
 */

#pragma once

#include "StateMachine/StateMachine.hpp"

class Board {
public:
	StateMachine general_state_machine;

	static void reset();
	static void synchronize();
};
