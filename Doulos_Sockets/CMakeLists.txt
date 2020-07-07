cmake_minimum_required(VERSION 3.16)
project(Doulos_Sockets)

set(CMAKE_CXX_STANDARD 14)

# change your SystemC installation directory
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # if under linux OS
    set(CMAKE_PREFIX_PATH /home/singularity/systemc-2.3.3-install)
    message(STATUS "Current development environment is Linux,")
else(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Current development environment is Windows,")
    set(CMAKE_PREFIX_PATH E:/systemc-2.3.3/SystemC)
endif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
message(STATUS "The SystemC installation directory is ${CMAKE_PREFIX_PATH}")

include_directories(${CMAKE_PREFIX_PATH}/include)
find_package(SystemCLanguage CONFIG REQUIRED)
link_directories(${CMAKE_PREFIX_PATH}/lib)

add_executable(Doulos_Sockets testbench.cpp target.h initiator.h top.h)
target_link_libraries (Doulos_Sockets SystemC::systemc)
