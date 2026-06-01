# Changelog

This file tracks ST-LIB releases prepared by the semiautomated release flow.
The revived semantic-versioning baseline starts at `v5.0.0`.

Historical releases that predate this file remain available in Git tags such as
`v1.0.0`, `v3.0.0`, `v4.0.0-beta`, and `h10`.

## v6.1.2 - 2026-05-29

### Fixes

- Fix faults caused in state machine start causing non-initialized state machine
- Fix some clock problems in spi 4 and 5

## v6.1.1 - 2026-05-11

### Fixes

- Allow protections to read volatile sample sources
  Protection sources now preserve the storage cv-qualification through a reference source wrapper,
  so protections can be declared directly over volatile variables while rule evaluation still uses
  plain sample values.

## v6.1.0 - 2026-05-11

### Features

- It is just a one word change, makes it possible to use `Board::instanceof()` to get a `constexpr auto& obj`. It works perfectly as long as you don't try using the reference before `Board::init()`.

### Fixes

- Fix reorder issue in PWM constructor
- Unify get_global_tick behaviour and make format_numeric_value return if value was written in DiagnosticFormatter
- The macros changes in fix/volatiles where reverted in a merge in the last release, this pr applies the changes once more to fix the undefined behaviour.

## v6.0.0 - 2026-05-06

### Breaking Changes

- Redesign fault handling, protections, and Board bootstrap around explicit fault policies
  This PR changes the public integration contract for applications built on ST-LIB.

  Breaking changes:

  - `Board` now takes the fault policy type as its first template parameter.
  - The global `FAULT` runtime is owned exclusively by `FaultController`.
  - User state machines are now nested under the global `OPERATIONAL` state through `FaultPolicy` or `FaultPolicyNoMachine`.
  - Protections are now compile-time `Board` request objects evaluated through `Board::ProtectionEngine`; the previous `ProtectionManager` and boundary split is no longer the active model.
  - Runtime reporting is unified under `PANIC(...)`, `FAULT(...)`, `WARNING(...)`, and `INFO(...)`.
  - The real bootstrap path is `Board::init()`. Legacy `STLIB::start()`, `STLIB::update()`, `STLIB_LOW::start()`, and `STLIB_HIGH::start()` must not be used as the integration path.

  Migration notes:

  - Declare the board as `Board<YourFaultPolicy, ...>`.
  - Use `FaultPolicy<app_machine, on_fault_enter>` when you want an operational state machine nested under the global runtime.
  - Use `FaultPolicyNoMachine<on_fault_enter>` when you only need a fault-entry callback.
  - Use `DefaultFaultPolicy` when you want neither an operational machine nor a fault-entry callback.
  - Declare protections with `Protections::protection<"name", source>(...)` and pass the resulting request objects to `Board`.
  - In the main loop, drive the runtime through `FaultController::check_transitions()`, `Board::evaluate_protections()`, and `Diagnostics::Hub::flush()`.

### Features

- add an ethernet connected check
- move initialization outside constructor in PWM, DualPWM, Encoder and InputCapture
- Small refactor of some spi and timerwrapper functionality
  timerwrapper:
   - Add `set_callback(void (*callback)(void*), void* callback_data)` to set the callback and its data instead of needing to call `configurexxbit()` and set the period.
   - `set_limit_value(uint32_t arr)` to set the arr, this will likely be changed to use a `uint32_t` type only when using a 32 bit timer, for now it is just an alias to `instance->tim->ARR = arr;`. Was not added in this pr but also wasn't mentioned before.

  spi:
   - Add `transceive` with ptr + data explicitly instead of using a span since it's sometimes a pain in the ass to use.

### Fixes

- Fix incorrect usage of non-volatile buffers in peripherals. This wrong usage was causing undefined behaviour, since the compiler could optimize out reads and writes that should have been going directly to memory.
  There's a minor API change: now the MPU macros for memory regions include the volatile keyword for the non-cached variants.
- MPUDomain dynamic regions were configured using the linker symbol values instead of their addresses (that is the correct way of using linker symbols). That made non-cached regions cached, and caused harware undefined behaviour.
- fix pwm channel 4 config in TimerWrapper
- fix prescaler calculation in scheduler

## v5.2.0 - 2026-04-22

### Features

- move a struct outside template class
- Create a parameter to choose the type of the variables

### Fixes

- RXER pin for Ethernet is now optional
  H11 ethernet doesn't use RXER pin. The RXER pin is now a pointer so it can hold a nullptr. Ethernet constructor now has a std::optional for this pin
- input capture hotfix for errorhandler
  fix: make dummy have invalid channel nº for rising, falling

## v5.1.0 - 2026-04-05

### Features

- Refactor the ADC stack around DMA-backed acquisition using new MPU
  Includes the DMA-backed `ADCDomain` migration, the shared ADC sensor base, and the updated ADC integration/tests in the `adc-dma` branch.
- Added module dfsdm tested
- fix remaining scheduler race conditions and add warning when tasks are not ran in time
  The mechanism for checking if tasks are not ran in time is very simple but that also means the scheduler can only know if a task has not been called when the waiting time runs out for the second time.
  This means you will know if you're too slow to execute the task in less than 2x its period but not if you're between 1 and 2x its period.
- input capture implementation
  Also changed how the timer interrupts are handled since this was required for inputcapture

### Fixes

- add a FLASH_ST in .ld to keep the flash information that is not code
- Fix incorrect GPIO alternate-function availability bitmaps
  Correct the STM32H723 pin alternate-function masks in `Pin.hpp` so GPIO
  configuration reflects the actual AF support per pin.

### Internal

- Introduce semver tooling and release automation infrastructure
  Bootstrap the semiautomated release flow for ST-LIB without publishing a new
  library version yet.
