release: patch
summary: Fix incorrect GPIO alternate-function availability bitmaps

Correct the STM32H723 pin alternate-function masks in `Pin.hpp` so GPIO
configuration reflects the actual AF support per pin.
