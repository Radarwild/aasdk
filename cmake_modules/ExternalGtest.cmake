find_package(Threads REQUIRED)

set(EXT_STAGING_DIR ${CMAKE_CURRENT_BINARY_DIR}/staging${CMAKE_INSTALL_PREFIX})

include(ExternalProject)
ExternalProject_Add(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG master
  GIT_SHALLOW ON
  CMAKE_ARGS
    -D CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
    -D CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    -D CMAKE_STAGING_PREFIX=${EXT_STAGING_DIR}
    -D CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -D CMAKE_VERBOSE_MAKEFILE=ON
)

set(googletest_postfix $<CONFIG:Debug>)
set(GTEST_DEBUG_POSTFIX $<${googletest_postfix}:d>)
