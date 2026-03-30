# ST-LIB

Hardware abstraction library for STM32H723ZGT6 (ARM Cortex-M7). Used by template-project and all HyperloopUPV firmware projects. This repo is a git submodule in template-project at `deps/ST-LIB`.

## Architecture

```
Application (template-project)
    └── ST-LIB_HIGH   Board<>, FlashStorer, Protections, Control
         └── ST-LIB_LOW   ADC, PWM, UART, SPI, DMA, GPIO, Sensors, Timers...
              └── HALAL    compile-time config, resource management, init
                   └── STM32CubeH7 HAL  (hardware) / MockedDrivers (simulator)
```

- **`Inc/HALAL/Services/`**: peripheral services — ADC, PWM, UART, SPI, Communication, Flash, EXTI, Encoder, InputCapture, Watchdog...
- **`Inc/HALAL/Models/`**: lower-level models — DMA, GPIO, Timers, Pin, SPI, MPU
- **`Inc/ST-LIB_LOW/`**: sensors, digital I/O, encoders, half-bridges, state machines, math
- **`Inc/ST-LIB_HIGH/`**: `Board<>` template, `FlashStorer`, `Control`, `Protections`
- **`Inc/MockedDrivers/`**: simulator mocks replacing STM32 HAL for host PC builds
- **`Tests/`**: GTest suite using mocked drivers, compiled with `simulator` preset

## Core pattern: compile-time configuration

All peripherals use `constexpr` entries built at compile time via `Domain::build<N>()`:

```cpp
// Example: ADC on PA0
inline float my_output = 0.0f;

constexpr std::array<ADCDomain::Entry, 1> entries{{
    {.gpio_idx = 0,
     .pin = ST_LIB::PA0,
     .peripheral = ADCDomain::Peripheral::AUTO,   // resolves at compile time
     .channel    = ADCDomain::Channel::AUTO,      // resolves at compile time
     .resolution = ADCDomain::Resolution::BITS_12,
     .sample_time = ADCDomain::SampleTime::CYCLES_8_5,
     .prescaler  = ADCDomain::ClockPrescaler::DIV1,
     .sample_rate_hz = 0,
     .output     = &my_output}
}};

constexpr auto cfg = ADCDomain::build<1>(std::span{entries});
// Verify at compile time:
static_assert(cfg[0].peripheral == ADCDomain::Peripheral::ADC_1);
static_assert(cfg[0].channel    == ADCDomain::Channel::CH16);
```

`Peripheral::AUTO` + `Channel::AUTO` resolve pin → ADC peripheral/channel at compile time using `ADCDomain::pin_map` (42 entries for STM32H723). Wrong or unmappable pin = compile error.

## Adding a new driver

Follow this pattern (ADC is the reference: `Inc/HALAL/Services/ADC/ADC.hpp`, `Tests/adc_test.cpp`):

### 1. Service header `Inc/HALAL/Services/[Module]/[Module].hpp`
- Define `[Module]Domain` struct with `Entry`, `Config`, enums
- Implement `consteval build<N>(span<const Entry, N>)` returning `array<Config, N>`
- Guard hardware-only code: `#ifdef HAL_[MODULE]_MODULE_ENABLED`
- Include `hal_wrapper.h` for HAL types

### 2. Service implementation `Src/HALAL/Services/[Module]/[Module].cpp`
- HAL init/start functions called from `HALAL.cpp`
- Runtime operations (read, write, start, stop)
- Use `#ifdef SIM_ON` to swap real HAL calls for mock calls

### 3. MockedDriver `Inc/MockedDrivers/mocked_hal_[module].hpp`
- In-memory state replacing HAL registers
- Control functions for tests: `[module]_set_channel_raw()`, `[module]_advance_time_ns()`, etc.
- Mimic the HAL callback/interrupt interface if needed

### 4. Tests `Tests/[module]_test.cpp`
- Include GTest, service header, mocked driver headers
- Test compile-time config (static_assert + EXPECT_EQ)
- Test runtime behavior with mocked inputs
- Test error conditions with `TestErrorHandler::set_fail_on_error(true)`
- Add to `Tests/CMakeLists.txt`

### 5. Register in HALAL
- Add `#include "HALAL/Services/[Module]/[Module].hpp"` to `Inc/HALAL/HALAL.hpp`
- Add `[Module]Domain::start()` call in `Src/HALAL/HALAL.cpp` inside `common_start()` — initialization order matters

## Code conventions

- C++23, **no exceptions**, **no RTTI**
- `constexpr` / `consteval` for all compile-time config — never runtime-only init for hardware configs
- `#ifdef HAL_[MODULE]_MODULE_ENABLED` for all hardware-dependent code
- `#ifdef SIM_ON` for simulator-specific code paths
- All tests use GTest + mocked HAL (no real hardware in CI)
- Formatting: clang-format v17

## Building and testing

From template-project root (ST-LIB has no standalone build):

```bash
cmake --preset simulator
cmake --build --preset simulator
ctest --preset simulator-all          # all tests
ctest --preset simulator-adc          # ADC tests only
ctest --preset simulator-all-asan     # AddressSanitizer + UBSan
```

Or via hyper CLI:
```bash
./hyper stlib build --preset simulator --run-tests
```

## MockedDriver usage in tests

```cpp
// Inject values
adc_set_channel_raw(channel_idx, raw_value);    // set raw ADC reading
adc_advance_time_ns(1000);                      // advance simulated time

// Verify internal state
adc_get_dma_length();                           // check DMA buffer size
adc_get_rank_count();                           // check conversion rank count

// Error handling
ST_LIB::TestErrorHandler::set_fail_on_error(true);  // turn errors into test failures
ST_LIB::TestErrorHandler::reset();                  // reset between tests
```

## DMA entry merging (for multi-peripheral tests)

When combining DMA configs from multiple peripherals in a single test, use the `merge_dma_entries` pattern from `adc_test.cpp`:

```cpp
constexpr auto merged_dma = merge_dma_entries<TotalN>(
    std::span{base_entries}, extra_entries_1, extra_entries_2
);
```

## Pin mapping (ADC, STM32H723)

`ADCDomain::pin_map` has 42 entries. `Peripheral::AUTO` resolves the GPIO pin to the correct ADC peripheral and channel at compile time. Multiple pins on the same ADC peripheral share a single DMA stream (sequential scan). Pins on different peripherals run on separate DMA streams (parallel). Use `static_assert` to verify the resolved mapping in tests.

## Reference documents

Downloaded via `./hyper doc`. Stored locally at `manuals/` (gitignored).

### STM32H723 / STM32H7 (STMicroelectronics)
| Document | ID | URL |
|----------|----|-----|
| Reference Manual | RM0468 | https://www.st.com/resource/en/reference_manual/rm0468-stm32h723733-stm32h725735-and-stm32h730-value-line-advanced-armbased-32bit-mcus-stmicroelectronics.pdf |
| Datasheet | DS13313 | https://www.st.com/resource/en/datasheet/stm32h723zg.pdf |
| Programming Manual (Cortex-M7) | PM0253 | https://www.st.com/resource/en/programming_manual/pm0253-stm32f7-series-and-stm32h7-series-cortexm7-processor-programming-manual-stmicroelectronics.pdf |
| HAL/LL Drivers User Manual | UM2217 | https://www.st.com/resource/en/user_manual/um2217-description-of-stm32h7-hal-and-lowlayer-drivers-stmicroelectronics.pdf |
| Nucleo-H723ZG Board Manual | UM2407 | https://www.st.com/resource/en/user_manual/um2407-stm32h7-nucleo144-boards-mb1364-stmicroelectronics.pdf |

### ARM Cortex-M7
| Document | ID | URL |
|----------|----|-----|
| Technical Reference Manual | DDI0489F | https://developer.arm.com/documentation/ddi0489/f/DDI0489F_cortex_m7_trm.pdf |
| Devices Generic User Guide | DUI0646C | https://developer.arm.com/documentation/dui0646/c/DUI0646C_cortex_m7_dgug.pdf |

### Ethernet PHY chips (Microchip)
| Document | ID | URL |
|----------|----|-----|
| KSZ8041 Datasheet | DS00002245B | https://ww1.microchip.com/downloads/en/DeviceDoc/00002245B.pdf |
| LAN8700 Datasheet | DS00001927A | https://ww1.microchip.com/downloads/en/DeviceDoc/00001927A.pdf |
| LAN8742A Datasheet | — | https://ww1.microchip.com/downloads/en/DeviceDoc/8742a.pdf |

### Other ICs
| Document | ID | URL |
|----------|----|-----|
| LTC6810-1/2 Battery Cell Monitor | — | https://www.analog.com/media/en/technical-documentation/data-sheets/LTC6810-1-6810-2.pdf |
| LTC6820 isoSPI Interface | — | https://www.analog.com/media/en/technical-documentation/data-sheets/ltc6820.pdf |
| Bender IR155-32xx Isolation Monitor | D00376 | https://www.bender.de/fileadmin/content/Products/d/e/IR155-3210-V004_D00376_D_XXEN.pdf |
