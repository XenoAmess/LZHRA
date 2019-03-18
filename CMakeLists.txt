cmake_minimum_required(VERSION 3.13)
project(LZHRA)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(LZHRA
        src/BWT.h
        src/Common.h
        src/List.h
        src/LZH.h
        src/LZHRA.cpp
        src/Map.h
        src/RE.h)
