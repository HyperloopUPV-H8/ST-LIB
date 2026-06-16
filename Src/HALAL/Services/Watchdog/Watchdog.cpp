#include "HALAL/Services/Watchdog/Watchdog.hpp"

#ifdef HAL_IWDG_MODULE_ENABLED

IWDG_HandleTypeDef watchdog_handle;
std::chrono::microseconds Watchdog::watchdog_time =
    std::chrono::microseconds(1000000); // 1 second by default
bool reset_by_iwdg{};

#endif // HAL_IWDG_MODULE_ENABLED
