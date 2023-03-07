
#pragma once

#include "ST-LIB_LOW.hpp"
#include "ST-LIB_HIGH.hpp"

namespace Board{
	class Board{
		enum States{
			INITIAL,
			OPERATIONAL,
			FAULT,
		};

		StateMachine general_sm;
		Socket master_connection;
		Socket::SocketState& master_connection_state = master_connection.state;

		Board(Board::ID) : general_sm(INITIAL) {
			ProtectionManager::set_state_machine(state_machine, States::FAULT);
		}

		void add_fault_condition(const fucntion<bool()>& transition){
			general_sm.add_transition(OPERATIONAL, FAULT, transition);
		}

		void add_master_board(EthernetNode::Board board){
			master_connection = Socket(EthernetNode::tcp_server_nodes[board]);
			ProtectionManager_add_protection(&master_connection_state, ProtectionType::NOT_EQUALS, Socket::SocketState::CLOSING);
		}
	};
}
