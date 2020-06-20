cmake_minimum_required(VERSION 3.16)
project(Adder)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_PREFIX_PATH E:/systemc-2.3.3/SystemC)

include_directories(${CMAKE_PREFIX_PATH}/include)
find_package(SystemCLanguage CONFIG REQUIRED)
link_directories(${CMAKE_PREFIX_PATH}/lib)
add_executable(Adder  exercises_adder.cpp)
#add_executable(Adder  simple_adder.cpp)
target_link_libraries(Adder SystemC::systemc)
