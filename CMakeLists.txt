cmake_minimum_required(VERSION 3.13)
project(Ext2_Checker C)

set(CMAKE_C_STANDARD 99)

add_executable(Ext2_Checker src/main.c src/Bodies/load_file.c src/Headers/load_file.h src/Bodies/Util.c src/Headers/Util.h src/Bodies/Structs.c src/Headers/Structs.h)