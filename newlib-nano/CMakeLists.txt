include(ExternalProject)

set(NEWLIB_NANODIR ${PROJECT_BINARY_DIR}/newlib-nano)

ExternalProject_Add(newlib-nano
    PREFIX newlib-nano
    GIT_REPOSITORY https://github.com/fabo-corp/newlib-nano.git
    GIT_TAG v2.1-arm-clang
    UPDATE_COMMAND ""
    DOWNLOAD_NO_PROGRESS 1
    CONFIGURE_COMMAND ../newlib-nano/newlib/configure --prefix=${NEWLIB_NANODIR}
      --build=arm-none-eabi --disable-newlib-supplied-syscalls
      CC=clang
      CC_FOR_BUILD=clang
      "CFLAGS=-target arm-none-eabi -march=armv7-m -mthumb -mcpu=cortex-m3"
    LOG_DOWNLOAD 1
    LOG_CONFIGURE 1
    LOG_BUILD 1
    LOG_INSTALL 1
)

set_property(GLOBAL APPEND PROPERTY _PHABOS_INCLUDES ${NEWLIB_NANODIR}/include)
