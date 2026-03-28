Create a new ST-LIB hardware driver for: $ARGUMENTS

Use ADC as the reference implementation:
- `Inc/HALAL/Services/ADC/ADC.hpp` — service header pattern
- `Tests/adc_test.cpp` — test pattern with mocked drivers

### Step 1 — Service header `Inc/HALAL/Services/[Module]/[Module].hpp`

- Define `[Module]Domain` struct (wraps everything)
- Define `Entry` struct: compile-time config per peripheral instance (pin, mode, output pointer, etc.)
- Define `Config` struct: fully resolved config after `build()` (all AUTO fields resolved)
- Define enums for peripheral IDs, operating modes, resolutions, etc.
- Implement `consteval build<N>(std::span<const Entry, N>) -> std::array<Config, N>`
  - Resolve AUTO fields (pin → peripheral/channel lookup tables)
  - Validate constraints at compile time — use `compile_error()` for violations
- Guard hardware-specific declarations with `#ifdef HAL_[MODULE]_MODULE_ENABLED`
- Include `hal_wrapper.h` at the top

### Step 2 — Service implementation `Src/HALAL/Services/[Module]/[Module].cpp`

- HAL handle declarations (`extern [MODULE]_HandleTypeDef h[module]N`)
- `start(const Config& cfg)` function: initialize HAL, configure DMA if needed
- Runtime operations: `read()`, `write()`, `get_value()`, etc.
- Use `#ifdef SIM_ON` to call mocked functions instead of real HAL

### Step 3 — MockedDriver `Inc/MockedDrivers/mocked_hal_[module].hpp`

- In-memory state arrays replacing HAL registers
- Init function called instead of HAL init in `SIM_ON` mode
- Control functions for tests: `[module]_set_*(idx, value)`, `[module]_get_*(idx)`
- Time simulation if the peripheral has timing behavior: `[module]_advance_time_ns(ns)`
- Callback/interrupt simulation if needed

### Step 4 — Tests `Tests/[module]_test.cpp`

```cpp
#include <gtest/gtest.h>
#include "HALAL/Services/[Module]/[Module].hpp"
#include "MockedDrivers/mocked_hal_[module].hpp"

namespace ST_LIB::TestErrorHandler { void reset(); void set_fail_on_error(bool); }

namespace {
// Declare test configs at namespace scope (constexpr can't be local)
inline float test_output = 0.0f;
constexpr std::array<[Module]Domain::Entry, 1> test_entry{{ ... }};
constexpr auto test_cfg = [Module]Domain::build<1>(std::span{test_entry});
static_assert(test_cfg[0].peripheral == ...);  // verify compile-time resolution
}

TEST(ModuleTest, CompileTimeConfig) { ... }
TEST(ModuleTest, BasicReadWrite) { ... }
TEST(ModuleTest, ErrorOnInvalidConfig) {
    ST_LIB::TestErrorHandler::set_fail_on_error(true);
    // trigger invalid config...
    ST_LIB::TestErrorHandler::reset();
}
```

Add to `Tests/CMakeLists.txt`:
```cmake
add_executable([module]_test [module]_test.cpp)
target_link_libraries([module]_test PRIVATE stlib GTest::gtest_main)
add_test(NAME [Module]Test COMMAND [module]_test)
```

### Step 5 — Register in HALAL

- Add `#include "HALAL/Services/[Module]/[Module].hpp"` to `Inc/HALAL/HALAL.hpp`
- Add `[Module]Domain::start()` call in `Src/HALAL/HALAL.cpp` inside `common_start()`
  - Check initialization order: DMA must be started before services that use it

### Verification

```bash
cmake --preset simulator
cmake --build --preset simulator
ctest --preset simulator-all
```
