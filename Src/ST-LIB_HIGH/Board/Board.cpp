/*
 * Board.cpp
 *
 *  Created on: 20 feb. 2023
 *      Author: aleja
 */

#include "Board/Board.hpp"

void Board::reset() {
	NVIC_SystemReset();
}

void Board::synchronize() {

}
