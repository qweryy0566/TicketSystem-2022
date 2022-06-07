cmake_minimum_required(VERSION 3.16)
project(TicketSystem_2022)
set(CMAKE_CXX_FLAGS "-std=c++17 -Ofast")
<<<<<<< HEAD
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG -Wall -fsanitize=address -fsanitize=undefined")
=======
set(CMAKE_CXX_FLAGS_RELEASE "")
set(CMAKE_CXX_FLAGS_DEBUG "-g  -Wall -fsanitize=address -fsanitize=undefined")
>>>>>>> rollback

add_executable(code src_rollback/main.cpp)