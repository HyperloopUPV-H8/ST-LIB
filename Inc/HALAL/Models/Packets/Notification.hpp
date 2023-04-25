#pragma once

#include "C++Utilities/CppUtils.hpp"
#include "Order.hpp"
#include "BoardID/BoardID.hpp"

class Notification : public Order , public HeapOrder{
public:
    stringstream message;

};

