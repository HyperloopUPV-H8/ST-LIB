// Compiled WITH STLIB_OPTIMIZED_PROTECTIONS — uses Protection_optimized.hpp
#include "ST-LIB_HIGH/Protections/Protection.hpp"

namespace TestOptimized {

#ifdef STLIB_OPTIMIZED_COMPARISON_TEST
template <typename T, std::size_t N> using Protection = Protections::Optimized::Protection<T, N>;
#else
template <typename T, std::size_t N> using Protection = Protections::Protection<T, N>;
#endif

float src_bus   = 0.0f;
float src_time  = 0.0f;
float src_cur   = 0.0f;
float src_range = 0.0f;

static Protection<float, 2>& bus_prot() {
    static const std::array<Protections::RuleDefinition<float>, 2> defs = {
        Protections::Rules::below(320.0f, 350.0f).value(),
        Protections::Rules::above(600.0f).value(),
    };
    static Protection<float, 2> p("optimized_bus", SampleSource<float>(src_bus), defs);
    return p;
}
static Protection<float, 1>& time_prot() {
    static const std::array<Protections::RuleDefinition<float>, 1> defs = {
        Protections::Rules::time_accumulation(10.0f, 0.001f).value(),
    };
    static Protection<float, 1> p("optimized_time", SampleSource<float>(src_time), defs);
    return p;
}
static Protection<float, 1>& current_prot() {
    static const std::array<Protections::RuleDefinition<float>, 1> defs = {
        Protections::Rules::above(110.0f, 95.0f).value(),
    };
    static Protection<float, 1> p("optimized_current", SampleSource<float>(src_cur), defs);
    return p;
}
static Protection<float, 1>& range_prot() {
    static const std::array<Protections::RuleDefinition<float>, 1> defs = {
        Protections::Rules::range(-40.0f, 150.0f).value(),
    };
    static Protection<float, 1> p("optimized_range", SampleSource<float>(src_range), defs);
    return p;
}

Protections::ProtectionEvaluation eval_bus(float val)     { src_bus = val;   return bus_prot().evaluate(); }
Protections::ProtectionEvaluation eval_time(float val)    { src_time = val;  return time_prot().evaluate(); }
Protections::ProtectionEvaluation eval_current(float val) { src_cur = val;   return current_prot().evaluate(); }
Protections::ProtectionEvaluation eval_range(float val)   { src_range = val; return range_prot().evaluate(); }

} // namespace TestOptimized
