cmake_minimum_required(VERSION 3.9)
project(Memory)

file(GLOB SOURCE_FILES Private/* Public/${PROJECT_NAME}/*)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE Public/${PROJECT_NAME}/)
target_include_directories(${PROJECT_NAME} PUBLIC Public/)

target_link_libraries(${PROJECT_NAME} Utils)

set_target_properties( ${PROJECT_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
