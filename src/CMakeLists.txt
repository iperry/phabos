include(ExternalProject)

# Set include paths
set_property(GLOBAL APPEND PROPERTY _PHABOS_INCLUDES ${CMAKE_CURRENT_BINARY_DIR})
phabos_add_includes(.)
phabos_add_includes(include/)

set(KCONFIG_DIR ${PROJECT_BINARY_DIR}/kconfig)
set(KCONFIG_CONF_PATH ${KCONFIG_DIR}/bin/kconfig-conf)
set(KCONFIG_MCONF_PATH ${KCONFIG_DIR}/bin/kconfig-mconf)
message("KCONFIG_DIR: ${KCONFIG_DIR}")
message("KCONFIG_PATH: ${KCONFIG_PATH}")

ExternalProject_Add(kconfig-frontends
    PREFIX kconfig-frontends
    URL http://ymorin.is-a-geek.org/download/kconfig-frontends/kconfig-frontends-3.12.0.0.tar.xz
    UPDATE_COMMAND ""
    DOWNLOAD_NO_PROGRESS 1
    CONFIGURE_COMMAND ./configure
      --prefix=${KCONFIG_DIR}
      --enable-mconf
    INSTALL_COMMAND make install
    LOG_DOWNLOAD 1
    LOG_CONFIGURE 1
    LOG_BUILD 1
    LOG_INSTALL 1
    BUILD_IN_SOURCE 1
)

add_custom_target(menuconfig
  DEPENDS kconfig-frontends
  WORKING_DIRECTORY ${KCONFIG_DIR}
  COMMAND rsync -rv --include '*/'
    --include 'Kconfig'
    --exclude '*'
    --prune-empty-dirs ${CMAKE_CURRENT_SOURCE_DIR}/ ${KCONFIG_DIR}
  COMMAND ARCH=arm ${KCONFIG_MCONF_PATH} Kconfig)

# Find all defconfigs and make custom targets for them
set(CONFIGS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/arch/arm/configs)
file(GLOB DEFCONFIGS RELATIVE ${CONFIGS_DIR} "${CONFIGS_DIR}/*_defconfig")
message("CONFIGS_DIR: ${CONFIGS_DIR}")
message("DEFCONFIGS: ${DEFCONFIGS}")

foreach(config ${DEFCONFIGS})
  message("Generating target: ${config}")
  add_custom_target(${config}
    COMMAND mkdir -p ${KCONFIG_DIR} && cp ${CONFIGS_DIR}/${config} ${KCONFIG_DIR}/.config)
endforeach()

# Generate config.h and config.cmake from the Kconfig file
#add_custom_target(generate-configs
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/config.h ${CMAKE_BINARY_DIR}/config.cmake
  COMMAND ${CMAKE_COMMAND} -E env CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR} ${PROJECT_SOURCE_DIR}/cmake/generate-config.sh ${KCONFIG_DIR}/.config
  DEPENDS ${KCONFIG_DIR}/.config)

set(CMAKE_CONFIG ${CMAKE_BINARY_DIR}/config.cmake)
if(NOT EXISTS ${CMAKE_CONFIG})
  file(WRITE ${CMAKE_CONFIG} "")
endif()
include(${CMAKE_BINARY_DIR}/config.cmake)
add_subdirectory(arch/)

# Print accumulated values
get_property(PHABOS_SRCS GLOBAL PROPERTY _PHABOS_SRCS)
get_property(PHABOS_INCLUDES GLOBAL PROPERTY _PHABOS_INCLUDES)

message("PHABOS_CFLAGS: ${PHABOS_CFLAGS}")
message("PHABOS_SRCS: ${PHABOS_SRCS}")
message("PHABOS_INCLUDES: ${PHABOS_INCLUDES}")

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_EXE_LINKER_FLAGS "-nostartfiles -nostdlib -T/home/perry/src/phabos/src/arch/arm/mach-tsb/tsb-es2.ld")
SET(CMAKE_C_LINK_EXECUTABLE "clang ${CMAKE_C_FLAGS} ${CMAKE_EXE_LINKER_FLAGS} <OBJECTS> -o phabos.bin")

# Build executable
add_executable(phabos.elf ${CMAKE_CURRENT_BINARY_DIR}/config.h ${PHABOS_SRCS})
add_dependencies(phabos.elf kconfig-frontends)
include_directories(phabos.elf ${CMAKE_SOURCE_DIR} ${PHABOS_INCLUDES})

