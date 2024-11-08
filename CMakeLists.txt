#
# CMakeLists
#
# File Name:   Makefile
# Author:      Yang Yujun
# E-mail:      <yujiantianhu@163.com>
# Created on:  2024.04.05
#
# Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
#

# minimum system require
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# compiler information
# (set after cmake_minimum_required, but project before)
set(ARCH                    arm)
set(TYPE                    armv7)
set(VENDOR                  imx6ull)
set(COMPILER_VER            gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf)
set(COMPILER_PATH           /usr/local/arm/${COMPILER_VER})
set(COMPILER_LIBC           ${COMPILER_PATH}/arm-linux-gnueabihf/libc)

set(CMAKE_SYSTEM_NAME       "Linux")
set(CMAKE_SYSTEM_PROCESSOR  "arm")
set(CMAKE_C_COMPILER        arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER      arm-linux-gnueabihf-g++)
set(CMAKE_OBJCOPY           arm-linux-gnueabihf-objcopy)
set(CMAKE_OBJDUMP           arm-linux-gnueabihf-objdump)

# name
set(PROJECT_NAME            HeavenFox)

# set output file name
set(PROJECT_LINK_FILE       cpu_ramboot.lds)
set(PROJECT_EXEC_FILE       ${PROJECT_NAME}.elf)
set(PROJECT_IMGE_FILE       ${PROJECT_NAME}.img)
set(PROJECT_NASM_FILE       ${PROJECT_NAME}.dis)

# project name
project(${PROJECT_EXEC_FILE})

# show compile information
set(CMAKE_VERBOSE_MAKEFILE ON)
enable_language(C CXX ASM)

# output version whith debug/release
if (NOT CMAKE_BUILD_TYPE STREQUAL "debug")
    set(CMAKE_BUILD_TYPE release CACHE STRING "Build Type" FORCE)
else ()
    set(CMAKE_BUILD_TYPE debug CACHE STRING "Build Type" FORCE)
endif()

# default compiler option
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3 -O0 -Wall -nostdlib")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--build-id=none")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wundef")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wstrict-prototypes")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-trigraphs")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-common")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror-implicit-function-declaration")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-tree-scev-cprop")

# link script path
set(link_script ${PROJECT_SOURCE_DIR}/arch/${ARCH}/${PROJECT_LINK_FILE})

# include path
set(include_dirs
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/arch/${ARCH}/include
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/board/${VENDOR}
    ${PROJECT_SOURCE_DIR}/lib
    ${PROJECT_SOURCE_DIR}/lib/fatfs
)

# source path
set(assembly_sources
    ${PROJECT_SOURCE_DIR}/arch/${ARCH}/lib
    ${PROJECT_SOURCE_DIR}/arch/${ARCH}/cpu/${TYPE}
    ${PROJECT_SOURCE_DIR}/arch/${ARCH}/kernel
)
set(common_sources
    ${PROJECT_SOURCE_DIR}/common/generic
    ${PROJECT_SOURCE_DIR}/common/mempool
)
set(architecture_sources
    ${PROJECT_SOURCE_DIR}/arch/${ARCH}/lib
    ${PROJECT_SOURCE_DIR}/arch/${ARCH}/cpu/${TYPE}
    ${PROJECT_SOURCE_DIR}/arch/${ARCH}/cpu/${TYPE}/${VENDOR}
)
set(boot_sources
    ${PROJECT_SOURCE_DIR}/boot
    ${PROJECT_SOURCE_DIR}/boot/dynamic
)
set(borad_sources
    ${PROJECT_SOURCE_DIR}/board/mach-${VENDOR}
)
set(platform_sources
    ${PROJECT_SOURCE_DIR}/platform
    ${PROJECT_SOURCE_DIR}/platform/mem
    ${PROJECT_SOURCE_DIR}/platform/mmc
    ${PROJECT_SOURCE_DIR}/platform/irq
    ${PROJECT_SOURCE_DIR}/platform/of
    ${PROJECT_SOURCE_DIR}/platform/bus_type
    ${PROJECT_SOURCE_DIR}/platform/chardev
    ${PROJECT_SOURCE_DIR}/platform/block_device
    ${PROJECT_SOURCE_DIR}/platform/net_device
    ${PROJECT_SOURCE_DIR}/platform/clk
    ${PROJECT_SOURCE_DIR}/platform/pinctrl
    ${PROJECT_SOURCE_DIR}/platform/gpio
    ${PROJECT_SOURCE_DIR}/platform/framebuffer
    ${PROJECT_SOURCE_DIR}/platform/i2c
    ${PROJECT_SOURCE_DIR}/platform/usb
    ${PROJECT_SOURCE_DIR}/platform/usb/host
)
set(kernel_sources
    ${PROJECT_SOURCE_DIR}/kernel
    ${PROJECT_SOURCE_DIR}/kernel/kthread
)
set(rootfs_sources
    ${PROJECT_SOURCE_DIR}/fs
    ${PROJECT_SOURCE_DIR}/lib/fatfs
    ${PROJECT_SOURCE_DIR}/lib/option
)
set(drivers_sources
    ${PROJECT_SOURCE_DIR}/drivers/clk
    ${PROJECT_SOURCE_DIR}/drivers/pinctrl
    ${PROJECT_SOURCE_DIR}/drivers/gpio
    ${PROJECT_SOURCE_DIR}/drivers/video
    ${PROJECT_SOURCE_DIR}/drivers/i2c
    ${PROJECT_SOURCE_DIR}/drivers/input
    ${PROJECT_SOURCE_DIR}/drivers/input/key
    ${PROJECT_SOURCE_DIR}/drivers/usb/gadget
)
set(application_sources
    ${PROJECT_SOURCE_DIR}/example
    ${PROJECT_SOURCE_DIR}/init
)

set(source_dirs
    ${assembly_sources}
    ${common_sources}
    ${architecture_sources}
    ${boot_sources}
    ${borad_sources}
    ${platform_sources}
    ${kernel_sources}
    ${rootfs_sources}
    ${drivers_sources}
    ${application_sources}
)

# executable file output path
set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/boot)
set(EXECUTABLE_OUTPUT       ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_EXEC_FILE})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --static -T${link_script}")

# CMAKE_SOURCE_DIR:         CMakeLists.txt directory (probably PROJECT_SOURCE_DIR too)
# PROJECT_SOURCE_DIR:       project directory
# PROJECT_BINARY_DIR:       Makefile directory (probably: "PROJECT_SOURCE_DIR/build")
# EXECUTABLE_OUTPUT_PATH:   ouput executable file path
message(STATUS "CMAKE_SYSTEM_PROCESSOR is: " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "CMAKE_SYSTEM_NAME      is: " ${CMAKE_SYSTEM_NAME})
message(STATUS "CMAKE_SOURCE_DIR       is: " ${CMAKE_SOURCE_DIR})
message(STATUS "PROJECT_SOURCE_DIR     is: " ${PROJECT_SOURCE_DIR})
message(STATUS "PROJECT_BINARY_DIR     is: " ${PROJECT_BINARY_DIR})
message(STATUS "EXECUTABLE_OUTPUT_PATH is: " ${EXECUTABLE_OUTPUT_PATH})

# ----------------------------------------------------------------
#                           CMake
# ----------------------------------------------------------------
# supply compile option
add_compile_options(-fexec-charset=GB2312)

# global macro
option(USER_DEFINE "Define User Command" on)
if (USER_DEFINE)
    add_definitions(-DCONFIG_DEBUG_JTAG)
    add_definitions(-DCONFIG_MACH_IMX6ULL_TOPPET)
endif()

# add_subdirectory()

# specify head file path
include_directories(${include_dirs})

# specify libraries file path
link_directories(${COMPILER_LIBC}/usr/lib 
                 ${COMPILER_LIBC}/lib)

# take out all source files (.c)
# aux_source_directory can only take out one path every time
foreach (item ${source_dirs})
    aux_source_directory(${item} sources)
endforeach()

# take out all source files (.S)
foreach (item ${assembly_sources})
    file(GLOB asm_item ${item}/*.S)
    set(assemblies ${assemblies} ${asm_item})
endforeach()

# print source list
# message(STATUS "sources  is: " ${sources})

# add to excutable output
add_executable(${PROJECT_EXEC_FILE} ${sources} ${assemblies})

# link library
target_link_libraries(${PROJECT_EXEC_FILE} -lc -lm -lgcc)

# .PHONY: self-clean (delete .elf, .img, .dis)
add_custom_target(
    self-clean
    COMMAND make clean
    COMMAND rm -rf ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_EXEC_FILE}
    COMMAND rm -rf ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_IMGE_FILE}
    COMMAND rm -rf ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NASM_FILE}
    COMMENT "make clean self"
)

# .PHONY: distclean (delete build/*)
add_custom_target(
    distclean
    COMMAND rm -rf ${PROJECT_BINARY_DIR}/*
    COMMENT "make clean all output files"
    DEPENDS self-clean
)

# comand: run whith compilling automatically
# POST_BUILD: run after all targets finished
add_custom_command(
    TARGET ${PROJECT_EXEC_FILE}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ${EXECUTABLE_OUTPUT} ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_IMGE_FILE}
    COMMAND ${CMAKE_OBJDUMP} -D -m ${ARCH} ${EXECUTABLE_OUTPUT} > ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NASM_FILE}
    COMMENT "Output binary file"
)


# end of file
