Show the ADC peripheral and channel mapping for GPIO pin: $ARGUMENTS

1. Read `Inc/HALAL/Services/ADC/ADC.hpp` and find the `pin_map` constexpr lookup table

2. Look up the requested pin and report:
   - Which ADC peripheral it maps to: `ADC_1`, `ADC_2`, or `ADC_3`
   - Which channel number (e.g. `CH16`)
   - Whether this pin is shared with another function (PWM, SPI, etc.) that could conflict

3. Check `Tests/adc_test.cpp` for any existing `static_assert` that already validates this mapping

4. Explain the implications for multi-channel use:
   - Pins on the **same ADC peripheral** → share one DMA stream, conversion is sequential (scan mode)
   - Pins on **different ADC peripherals** → separate DMA streams, conversions run in parallel
   - Maximum 16 channels per peripheral on STM32H723

5. Show the exact `ADCDomain::Entry` snippet to use this pin correctly:
   ```cpp
   inline float my_output = 0.0f;

   constexpr std::array<ADCDomain::Entry, 1> entry{{
       {.gpio_idx   = 0,
        .pin        = ST_LIB::[PIN],
        .peripheral = ADCDomain::Peripheral::AUTO,
        .channel    = ADCDomain::Channel::AUTO,
        .resolution = ADCDomain::Resolution::BITS_12,
        .sample_time = ADCDomain::SampleTime::CYCLES_8_5,
        .prescaler  = ADCDomain::ClockPrescaler::DIV1,
        .sample_rate_hz = 0,
        .output     = &my_output}
   }};
   constexpr auto cfg = ADCDomain::build<1>(std::span{entry});
   // Confirm the resolved mapping:
   static_assert(cfg[0].peripheral == ADCDomain::Peripheral::[RESOLVED]);
   static_assert(cfg[0].channel    == ADCDomain::Channel::[RESOLVED]);
   ```

6. If the pin is **not in the pin_map** (not ADC-capable on STM32H723), say so clearly and suggest the nearest ADC-capable pins.
