cmake_minimum_required (VERSION 3.2)
project (phabos)

# cmake build settings
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_COLOR_MAKEFILE ON)

include(Phabos)

# phabos compile flags
set(PHABOS_CFLAGS
  -target arm-none-eabi
  -march=armv7-m
  -mtune=cortex-m3
  -nostdlib)
string(REPLACE ";" " " PHABOS_CFLAGS "${PHABOS_CFLAGS}")
set(CMAKE_C_FLAGS ${PHABOS_CFLAGS})

add_subdirectory(newlib-nano)
add_subdirectory(src)


