project(aasdk_proto LANGUAGES CXX)

include(FindProtobuf)

find_package(Protobuf REQUIRED)

file(GLOB_RECURSE proto_files ${CMAKE_CURRENT_SOURCE_DIR}/*.proto)

protobuf_generate_cpp(proto_sources proto_headers ${proto_files})
foreach (header ${proto_headers})
    message("Adding ${header}")
endforeach ()
message("${CMAKE_CURRENT_BINARY_DIR}")


add_library(aasdk_proto OBJECT ${proto_headers} ${proto_sources})
target_include_directories(aasdk_proto PRIVATE ${PROTOBUF_INCLUDE_DIR})

if (AASDK_BUILD_STATIC)
    add_library(aasdk_proto_static STATIC $<TARGET_OBJECTS:aasdk_proto>)
    set_target_properties(aasdk_proto_static PROPERTIES PUBLIC_HEADER "${proto_headers}")
endif ()
if (AASDK_BUILD_SHARED)
    add_library(aasdk_proto_shared SHARED $<TARGET_OBJECTS:aasdk_proto>)
    target_link_libraries(aasdk_proto_shared ${Protobuf_LITE_LIBRARIES})
    set_target_properties(aasdk_proto_shared PROPERTIES PUBLIC_HEADER "${proto_headers}")
endif ()


if (AASDK_MAIN_PROJECT)
    if (AASDK_BUILD_STATIC)
        install(TARGETS aasdk_proto_shared DESTINATION lib)
    endif ()
    if (AASDK_BUILD_SHARED)
        install(TARGETS aasdk_proto_static DESTINATION lib)
    endif ()
    install(DIRECTORY . DESTINATION include/aasdk_proto
            FILES_MATCHING PATTERN *.h
            PATTERN CMakeFiles EXCLUDE)
endif ()