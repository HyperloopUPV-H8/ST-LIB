set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(STM32_CLT_REQUIRED_VERSION "1.21.0" CACHE STRING "Required STM32CubeCLT version")
set(TOOLCHAIN_PREFIX arm-none-eabi-)

set(_stm32_clt_candidate_roots "")

if(DEFINED ENV{STM32_CLT_ROOT} AND NOT "$ENV{STM32_CLT_ROOT}" STREQUAL "")
    list(APPEND _stm32_clt_candidate_roots "$ENV{STM32_CLT_ROOT}")
endif()
if(DEFINED ENV{HYPER_STM32CLT_ROOT} AND NOT "$ENV{HYPER_STM32CLT_ROOT}" STREQUAL "")
    list(APPEND _stm32_clt_candidate_roots "$ENV{HYPER_STM32CLT_ROOT}")
endif()

if(WIN32)
    if(DEFINED ENV{ProgramFiles} AND NOT "$ENV{ProgramFiles}" STREQUAL "")
        list(APPEND _stm32_clt_candidate_roots
            "$ENV{ProgramFiles}/STMicroelectronics/STM32Cube/STM32CubeCLT_${STM32_CLT_REQUIRED_VERSION}"
            "$ENV{ProgramFiles}/STMicroelectronics/STM32CubeCLT_${STM32_CLT_REQUIRED_VERSION}"
        )
        file(GLOB _stm32_clt_globbed LIST_DIRECTORIES true
            "$ENV{ProgramFiles}/STMicroelectronics/STM32Cube/STM32CubeCLT_${STM32_CLT_REQUIRED_VERSION}*"
            "$ENV{ProgramFiles}/STMicroelectronics/STM32Cube/STM32CubeCLT-${STM32_CLT_REQUIRED_VERSION}*"
            "$ENV{ProgramFiles}/STMicroelectronics/STM32CubeCLT_${STM32_CLT_REQUIRED_VERSION}*"
            "$ENV{ProgramFiles}/STMicroelectronics/STM32CubeCLT-${STM32_CLT_REQUIRED_VERSION}*"
        )
        list(APPEND _stm32_clt_candidate_roots ${_stm32_clt_globbed})
    endif()
    list(APPEND _stm32_clt_candidate_roots
        "C:/ST/STM32CubeCLT_${STM32_CLT_REQUIRED_VERSION}"
        "C:/ST/STM32CubeCLT-${STM32_CLT_REQUIRED_VERSION}"
    )
    file(GLOB _stm32_clt_globbed_c LIST_DIRECTORIES true
        "C:/ST/STM32CubeCLT_${STM32_CLT_REQUIRED_VERSION}*"
        "C:/ST/STM32CubeCLT-${STM32_CLT_REQUIRED_VERSION}*"
    )
    list(APPEND _stm32_clt_candidate_roots ${_stm32_clt_globbed_c})
else()
    list(APPEND _stm32_clt_candidate_roots
        "/opt/ST/STM32CubeCLT_${STM32_CLT_REQUIRED_VERSION}"
        "/opt/ST/STM32CubeCLT-${STM32_CLT_REQUIRED_VERSION}"
        "$ENV{HOME}/ST/STM32CubeCLT_${STM32_CLT_REQUIRED_VERSION}"
        "$ENV{HOME}/ST/STM32CubeCLT-${STM32_CLT_REQUIRED_VERSION}"
    )
    file(GLOB _stm32_clt_globbed LIST_DIRECTORIES true
        "/opt/ST/STM32CubeCLT_${STM32_CLT_REQUIRED_VERSION}*"
        "/opt/ST/STM32CubeCLT-${STM32_CLT_REQUIRED_VERSION}*"
        "$ENV{HOME}/ST/STM32CubeCLT_${STM32_CLT_REQUIRED_VERSION}*"
        "$ENV{HOME}/ST/STM32CubeCLT-${STM32_CLT_REQUIRED_VERSION}*"
    )
    list(APPEND _stm32_clt_candidate_roots ${_stm32_clt_globbed})
endif()

list(REMOVE_DUPLICATES _stm32_clt_candidate_roots)

set(STM32_CLT_ROOT "")
foreach(_candidate IN LISTS _stm32_clt_candidate_roots)
    if(EXISTS "${_candidate}/GNU-tools-for-STM32/bin/${TOOLCHAIN_PREFIX}gcc")
        set(STM32_CLT_ROOT "${_candidate}")
        break()
    endif()
endforeach()

if(NOT STM32_CLT_ROOT)
    find_program(_stm32_arm_gcc NAMES ${TOOLCHAIN_PREFIX}gcc)
    if(NOT _stm32_arm_gcc)
        message(FATAL_ERROR
            "STM32CubeCLT ${STM32_CLT_REQUIRED_VERSION} is required, but arm-none-eabi-gcc was not found. "
            "Install the required CLT and/or set STM32_CLT_ROOT."
        )
    endif()

    get_filename_component(_stm32_arm_gcc_realpath "${_stm32_arm_gcc}" REALPATH)
    string(REGEX MATCH "STM32CubeCLT[_-]([0-9]+\\.[0-9]+\\.[0-9]+)" _stm32_path_match "${_stm32_arm_gcc_realpath}")

    if(NOT CMAKE_MATCH_1)
        message(FATAL_ERROR
            "STM32CubeCLT ${STM32_CLT_REQUIRED_VERSION} is required, but the compiler in PATH "
            "does not point to a versioned STM32CubeCLT installation:\n  ${_stm32_arm_gcc_realpath}"
        )
    endif()

    if(NOT CMAKE_MATCH_1 STREQUAL STM32_CLT_REQUIRED_VERSION)
        message(FATAL_ERROR
            "STM32CubeCLT ${STM32_CLT_REQUIRED_VERSION} is required, but found ${CMAKE_MATCH_1}:\n  ${_stm32_arm_gcc_realpath}"
        )
    endif()

    get_filename_component(_stm32_bin_dir "${_stm32_arm_gcc_realpath}" DIRECTORY)
    get_filename_component(_stm32_gnu_dir "${_stm32_bin_dir}" DIRECTORY)
    get_filename_component(STM32_CLT_ROOT "${_stm32_gnu_dir}" DIRECTORY)
endif()

string(REGEX MATCH "STM32CubeCLT[_-]([0-9]+\\.[0-9]+\\.[0-9]+)" _stm32_root_match "${STM32_CLT_ROOT}")
if(CMAKE_MATCH_1 AND NOT CMAKE_MATCH_1 STREQUAL STM32_CLT_REQUIRED_VERSION)
    message(FATAL_ERROR
        "STM32CubeCLT ${STM32_CLT_REQUIRED_VERSION} is required, but selected root is ${CMAKE_MATCH_1}:\n  ${STM32_CLT_ROOT}"
    )
endif()

set(ARM_TOOLCHAIN_DIR "${STM32_CLT_ROOT}/GNU-tools-for-STM32/bin")
set(CMAKE_C_COMPILER "${ARM_TOOLCHAIN_DIR}/${TOOLCHAIN_PREFIX}gcc")
set(CMAKE_ASM_COMPILER "${CMAKE_C_COMPILER}")
set(CMAKE_CXX_COMPILER "${ARM_TOOLCHAIN_DIR}/${TOOLCHAIN_PREFIX}g++")

set(CMAKE_OBJCOPY "${ARM_TOOLCHAIN_DIR}/${TOOLCHAIN_PREFIX}objcopy" CACHE INTERNAL "objcopy tool")
set(CMAKE_SIZE_UTIL "${ARM_TOOLCHAIN_DIR}/${TOOLCHAIN_PREFIX}size" CACHE INTERNAL "size tool")

set(CMAKE_FIND_ROOT_PATH "${STM32_CLT_ROOT}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
