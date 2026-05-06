release: minor
summary: Small refactor of some spi and timerwrapper functionality

timerwrapper:
 - Add `set_callback(void (*callback)(void*), void* callback_data)` to set the callback and its data instead of needing to call `configurexxbit()` and set the period.
 - `set_limit_value(uint32_t arr)` to set the arr, this will likely be changed to use a `uint32_t` type only when using a 32 bit timer, for now it is just an alias to `instance->tim->ARR = arr;`. Was not added in this pr but also wasn't mentioned before.

spi:
 - Add `transceive` with ptr + data explicitly instead of using a span since it's sometimes a pain in the ass to use.
