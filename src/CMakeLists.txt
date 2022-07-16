cmake_minimum_required(VERSION 3.9)
project(exercises)

add_subdirectory(Utils)
add_subdirectory(Memory)
add_subdirectory(DataStructures)

set(SOURCE_FILES main.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} Utils Memory DataStructures)
