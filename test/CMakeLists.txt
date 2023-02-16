
cmake_minimum_required(VERSION 3.16.3)

project("generated_code")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find requirements
find_package(fastcdr REQUIRED)
find_package(fastrtps REQUIRED)


message(STATUS "Configuring HelloWorld...")
add_library(HelloWorld_lib HelloWorld.cxx)
target_link_libraries(HelloWorld_lib fastcdr fastrtps)

add_executable(HelloWorld HelloWorldPubSubTypes.cxx HelloWorldPublisher.cxx HelloWorldSubscriber.cxx HelloWorldPubSubMain.cxx)
target_link_libraries(HelloWorld fastcdr fastrtps
        HelloWorld_lib )



