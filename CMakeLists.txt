cmake_minimum_required(VERSION 3.10)
project(CaseFile VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(casefile
    src/main.cpp
    src/Evidence.cpp
    src/Investigator.cpp
    src/Case.cpp
    src/Report.cpp
)