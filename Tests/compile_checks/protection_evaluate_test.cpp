#include "ST-LIB.hpp"

namespace {

// Simulates realistic board protections like PCU-H11:
// - 2 battery over-voltage thresholds (ABOVE, fault only)
// - 6 current sensor protections (ABOVE, fault + warning thresholds)
// - 1 bus voltage undervoltage (BELOW, fault + warning)
// - 1 temperature range check (RANGE, fault levels)
// - 1 RMS current time accumulation (TIME_ACCUMULATION, fault + warning)

float battery_voltage_a  = 0.0f;
float battery_voltage_b  = 0.0f;
float current_u          = 0.0f;
float current_v          = 0.0f;
float current_w          = 0.0f;
float bus_voltage        = 0.0f;
float motor_temp         = 25.0f;
float rms_current        = 0.0f;

inline constexpr auto protection_battery_a =
    Protections::protection<"battery_a_ov", battery_voltage_a>(
        Protections::Rules::above(410.0f)
    );

inline constexpr auto protection_battery_b =
    Protections::protection<"battery_b_ov", battery_voltage_b>(
        Protections::Rules::above(410.0f)
    );

inline constexpr auto protection_current_u =
    Protections::protection<"current_u_oc", current_u>(
        Protections::Rules::above(110.0f, 95.0f)
    );

inline constexpr auto protection_current_v =
    Protections::protection<"current_v_oc", current_v>(
        Protections::Rules::above(110.0f, 95.0f)
    );

inline constexpr auto protection_current_w =
    Protections::protection<"current_w_oc", current_w>(
        Protections::Rules::above(110.0f, 95.0f)
    );

inline constexpr auto protection_bus_uv =
    Protections::protection<"bus_voltage_uv", bus_voltage>(
        Protections::Rules::below(320.0f, 350.0f),
        Protections::Rules::above(600.0f)
    );

inline constexpr auto protection_motor_temp =
    Protections::protection<"motor_temp_range", motor_temp>(
        Protections::Rules::range(-40.0f, 150.0f)
    );

inline constexpr auto protection_rms_current =
    Protections::protection<"rms_current_acc", rms_current>(
        Protections::Rules::time_accumulation(120.0f, 100.0f, 0.5f)
    );

using TestBoard = ST_LIB::Board<
    ST_LIB::DefaultFaultPolicy,
    protection_battery_a,
    protection_battery_b,
    protection_current_u,
    protection_current_v,
    protection_current_w,
    protection_bus_uv,
    protection_motor_temp,
    protection_rms_current
>;

__attribute__((noinline, used))
void force_evaluate_instantiate() {
    battery_voltage_a = 400.0f;
    battery_voltage_b = 395.0f;
    current_u         = 85.0f;
    current_v         = 90.0f;
    current_w         = 87.0f;
    bus_voltage       = 500.0f;
    motor_temp        = 45.0f;
    rms_current       = 50.0f;
    TestBoard::evaluate_protections();
}

} // namespace
