cmake_minimum_required(VERSION 3.16)
project(TicketSystem_2022)
set(CMAKE_CXX_FLAGS "-std=c++17 -O2")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG -Wall -fsanitize=address -fsanitize=undefined")

add_executable(code src/main.cpp)