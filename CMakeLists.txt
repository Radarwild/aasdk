cmake_minimum_required(VERSION 3.5.1)
project(aasdk LANGUAGES CXX)

option(AASDK_TEST "Build Unit Test Cases" ON)
option(AASDK_CODE_COVERAGE "Build with Code Coverage" OFF)

option(AASDK_BUILD_STATIC "Build Static Libraries" ON)
option(AASDK_BUILD_SHARED "Build Shared Libraries" ON)

set(AASDK_MAIN_PROJECT OFF CACHE BOOL "Is AASDK the main project")
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(AASDK_MAIN_PROJECT ON CACHE BOOL "Is AASDK the main project")
endif ()

set(base_directory ${CMAKE_CURRENT_SOURCE_DIR})
set(sources_directory ${base_directory}/src)

set(include_directory ${base_directory}/include)
set(include_ut_directory ${base_directory}/unit_test)

SET(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake_modules/")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} -Wall -pedantic")

#set(Boost_USE_STATIC_LIBS ON)
#set(Boost_USE_MULTITHREADED ON)
#set(Boost_USE_STATIC_RUNTIME OFF)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

add_subdirectory(aasdk_proto)

find_package(asio REQUIRED)
find_package(libusb-1.0 REQUIRED)
find_package(Protobuf REQUIRED)
find_package(OpenSSL REQUIRED)

set(AASDK_PROTO_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR})

include_directories(${AASDK_PROTO_INCLUDE_DIRS}
        ${LIBUSB_1_INCLUDE_DIRS}
        ${PROTOBUF_INCLUDE_DIR}
        ${OPENSSL_INCLUDE_DIR}
        ${GTEST_INCLUDE_DIRS}
        ${GMOCK_INCLUDE_DIRS}
        ${include_directory}
        ${include_ut_directory})

file(GLOB_RECURSE source_files ${sources_directory}/*.cpp)
file(GLOB_RECURSE include_files ${include_directory}/*.hpp)
file(GLOB_RECURSE tests_source_files ${sources_directory}/*.ut.cpp)
file(GLOB_RECURSE tests_include_files ${include_ut_directory}/*.hpp)

list(REMOVE_ITEM source_files ${tests_source_files})

add_library(aasdk OBJECT
        ${source_files}
        ${include_files})

add_dependencies(aasdk aasdk_proto)
target_compile_definitions(aasdk PUBLIC HEADER_ONLY ASIO_STANDALONE GOOGLE_PROTOBUF_NO_RTTI)
target_include_directories(aasdk PUBLIC ${include_directory})


if (AASDK_BUILD_STATIC)
    message("Building Static Library")
    add_library(aasdk_static STATIC $<TARGET_OBJECTS:aasdk>)
    target_include_directories(aasdk_static PUBLIC ${include_directory})
    target_include_directories(aasdk_static PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
endif ()
if (AASDK_BUILD_SHARED)
    message("Building Shared Library")
    add_library(aasdk_shared SHARED $<TARGET_OBJECTS:aasdk>)
    target_link_libraries(aasdk_shared
            aasdk_proto_shared
            protobuf::libprotobuf
            OpenSSL::SSL
            OpenSSL::Crypto
            libusb)
    target_include_directories(aasdk_shared PUBLIC ${include_directory})
    target_include_directories(aasdk_shared PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
endif ()


if (AASDK_MAIN_PROJECT)
    if (AASDK_BUILD_STATIC)
        install(TARGETS aasdk_static DESTINATION lib)
    endif ()
    if (AASDK_BUILD_SHARED)
        install(TARGETS aasdk_shared DESTINATION lib)
    endif ()
    install(TARGETS aasdk DESTINATION lib)
    install(DIRECTORY include/aasdk DESTINATION include)
endif ()

if (AASDK_TEST)
    enable_testing()
    ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)
    set(Boost_USE_STATIC_LIBS OFF)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_RUNTIME OFF)
    find_package(Boost REQUIRED COMPONENTS unit_test_framework)

    include(ExternalGtest)
    add_executable(aasdk_ut
            ${tests_source_files}
            ${tests_include_files})

    add_dependencies(aasdk_ut aasdk_shared googletest)

    target_include_directories(aasdk_ut PUBLIC ${EXT_STAGING_DIR}/include)
    target_link_directories(aasdk_ut PUBLIC ${EXT_STAGING_DIR}/lib)
    target_link_libraries(aasdk_ut
            aasdk_shared
            Boost::unit_test_framework
            gmock${GTEST_DEBUG_POSTFIX}
#            gmock_main${GTEST_DEBUG_POSTFIX}
            gtest${GTEST_DEBUG_POSTFIX}
#            gtest_main${GTEST_DEBUG_POSTFIX}
            Threads::Threads)

    if (AASDK_CODE_COVERAGE)
        include(CodeCoverage)
        append_coverage_compiler_flags()
        setup_target_for_coverage(NAME aasdk_coverage EXECUTABLE aasdk_ut DEPENDENCIES aasdk_ut)
    endif (AASDK_CODE_COVERAGE)
    add_test(NAME aasdk_test COMMAND aasdk_ut)

#    install(TARGETS aasdk_ut LIBRARY DESTINATION share/aasdk)

endif (AASDK_TEST)
