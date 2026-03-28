Write a new GTest test for ST-LIB module: $ARGUMENTS

1. Read the most relevant existing test first to understand the exact patterns:
   - `Tests/adc_test.cpp` — for ADC-like services (compile-time config + DMA)
   - `Tests/dma2_test.cpp` — for DMA-focused tests
   - `Tests/spi2_test.cpp` — for SPI/communication tests
   - `Tests/adc_sensor_test.cpp` — for higher-level sensor abstractions

2. Read the service header (`Inc/HALAL/Services/[Module]/[Module].hpp`) to understand the API

3. Read the MockedDriver header (`Inc/MockedDrivers/mocked_hal_[module].hpp`) to understand what control/injection functions are available

4. Write the test file `Tests/[module]_test.cpp`:

   **Required structure:**
   - All `constexpr` test configs at **namespace scope**, not inside TEST bodies (constexpr variables cannot be local)
   - `inline float` output variables at namespace scope
   - For DMA-based services: use `merge_dma_entries<TotalN>(base_span, extra_array...)` pattern from `adc_test.cpp` when combining multiple peripheral DMA configs

   **Recommended test cases:**
   - `CompileTimeConfig`: verify `build()` resolves AUTO fields correctly via `static_assert` + `EXPECT_EQ`
   - `BasicOperation`: inject a value via mocked driver, call the service, verify output
   - `MultiInstance`: test two simultaneous instances don't interfere
   - `ErrorOnInvalidConfig`: set `TestErrorHandler::set_fail_on_error(true)`, trigger invalid config, verify error was called

   **Error handler pattern:**
   ```cpp
   namespace ST_LIB::TestErrorHandler {
   void reset();
   void set_fail_on_error(bool enabled);
   extern int call_count;
   }
   // In test:
   ST_LIB::TestErrorHandler::set_fail_on_error(true);
   // ... code that should trigger error ...
   EXPECT_GT(ST_LIB::TestErrorHandler::call_count, 0);
   ST_LIB::TestErrorHandler::reset();
   ```

5. Register in `Tests/CMakeLists.txt`:
   ```cmake
   add_executable([module]_test [module]_test.cpp)
   target_link_libraries([module]_test PRIVATE stlib GTest::gtest_main)
   add_test(NAME [Module]Test COMMAND [module]_test)
   ```

6. Run and verify:
   ```bash
   cmake --build --preset simulator
   ctest --preset simulator-all --output-on-failure
   ```
