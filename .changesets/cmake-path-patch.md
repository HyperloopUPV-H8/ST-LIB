release: patch
summary: Make variable _stm32_arm_gcc_realpath case insensitive

In some systems, by default, the installation directory name is in lowercase. Making it case insensitive, only the version matters.
