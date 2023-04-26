#pragma once

#include "C++Utilities/CppUtils.hpp"
#include "Order.hpp"
#include "BoardID/BoardID.hpp"
#include "Protection.hpp"
#include "Time/Time.hpp"

class Notification : public Order{
private:
    Board::ID board_id;
    Protection* protection;
    void(*callback)();
    string message;
    uint8_t* buffer = nullptr;
public:
    Notification(uint16_t packet_id, void(*callback)() ,Board::ID board_id, Protection* protection) : board_id(board_id), protection(protection){}

    void set_callback(void(*callback)()) override{
        this->callback = callback;
    }

    void process() override{
        callback();
    }

    void serialize(){
        string serialized_prot = "Protection: {" + protection->serialize() + "}";
        message = "{ BoardID: " + to_string(board_id) + ", TimeStamp: {" + Time::get_rtc_data().serialize()+ "}," + serialized_prot + " }";
    }



};

