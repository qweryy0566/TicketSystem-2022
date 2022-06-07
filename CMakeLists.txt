cmake_minimum_required(VERSION 3.12)
project(TicketSystem_2022)
set(CMAKE_CXX_FLAGS "-std=c++17 -Ofast")
set(CMAKE_CXX_FLAGS_RELEASE "")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG -Wall ")

add_executable(code src_rollback/main.cpp)