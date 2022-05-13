cmake_minimum_required(VERSION 3.12)
project(TicketSystem_2022)
set(CMAKE_CXX_FLAGS "-std=c++17")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG -Wall -fsanitize=undefined -fsanitize=address")
set(SRC_FILES
        
        
        )
add_executable(code ${SRC_FILES} src/main.cpp)