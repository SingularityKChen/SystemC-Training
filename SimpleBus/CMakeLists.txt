###############################################################################
#
# Licensed to Accellera Systems Initiative Inc. (Accellera) under one or
# more contributor license agreements.  See the NOTICE file distributed
# with this work for additional information regarding copyright ownership.
# Accellera licenses this file to you under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with the
# License.  You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied.  See the License for the specific language governing
# permissions and limitations under the License.
#
###############################################################################

###############################################################################
#
# examples/sysc/simple_bus/CMakeLists.txt --
# CMake script to configure the SystemC sources and to generate native
# Makefiles and project workspaces for your compiler environment.
#
# Original Author: Torsten Maehne, Université Pierre et Marie Curie, Paris,
#                  2013-06-11
#
###############################################################################

###############################################################################
#
# MODIFICATION LOG - modifiers, enter your name, affiliation, date and
# changes you are making here.
#
#     Name, Affiliation, Date:
# Description of Modification:
#
###############################################################################
cmake_minimum_required(VERSION 3.16)
project(simple_bus)

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

add_executable (simple_bus simple_bus_main.cpp
                           simple_bus_test.h
                           simple_bus_master_blocking.h
                           simple_bus_master_blocking.cpp
                           simple_bus_master_non_blocking.h
                           simple_bus_master_non_blocking.cpp
                           simple_bus_master_direct.h
                           simple_bus_master_direct.cpp
                           simple_bus_slow_mem.h
                           simple_bus_types.h
                           simple_bus_blocking_if.h
                           simple_bus_direct_if.h
                           simple_bus_non_blocking_if.h
                           simple_bus_request.h
                           simple_bus_slave_if.h
                           simple_bus.h
                           simple_bus.cpp
                           simple_bus_fast_mem.h
                           simple_bus_arbiter.h
                           simple_bus_arbiter_if.h
                           simple_bus_arbiter.cpp
                           simple_bus_types.cpp
                           simple_bus_tools.cpp)
target_link_libraries (simple_bus SystemC::systemc)
#configure_and_add_test (simple_bus)
