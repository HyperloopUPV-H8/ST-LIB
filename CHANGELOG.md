# Changelog

This file tracks ST-LIB releases prepared by the semiautomated release flow.
The revived semantic-versioning baseline starts at `v5.0.0`.

Historical releases that predate this file remain available in Git tags such as
`v1.0.0`, `v3.0.0`, `v4.0.0-beta`, and `h10`.

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
