
cmake_minimum_required(VERSION 3.16.3)

project("generated_code")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find requirements
find_package(fastcdr REQUIRED)
find_package(fastrtps REQUIRED)


message(STATUS "Configuring HelloWorld...")
file(GLOB SRC_FILE "*.cxx")

add_executable(test  ${SRC_FILE})

target_link_libraries(test 
        fastcdr fastrtps)



