release: patch
summary: Fix incorrect usage of non-volatile buffers in peripherals. This wrong usage was causing undefined behaviour, since the compiler could optimize out reads and writes that should have been going directly to memory.

There's a minor API change: now the MPU macros for memory regions include the volatile keyword for the non-cached variants.
