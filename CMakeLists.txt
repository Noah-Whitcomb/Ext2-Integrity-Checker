cmake_minimum_required(VERSION 3.13)
project(Ext2_Checker C)

set(CMAKE_C_STANDARD 99)

add_executable(Ext2_Checker src/main.c src/load_file.c)