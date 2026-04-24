cmake_minimum_required(VERSION 3.10)
project(numtool C)

set(CMAKE_C_STANDARD 99)

add_executable(numtool
    main.c
    parser.c
    math_utils.c
)