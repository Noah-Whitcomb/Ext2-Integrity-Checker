cmake_minimum_required(VERSION 3.13)
project(Ext2_Checker C)

set(CMAKE_C_STANDARD 99)

add_executable(Ext2_Checker src/main.c src/Bodies/VDIFile.c src/Headers/VDIFile.h src/Bodies/Util.c src/Headers/Util.h src/Headers/Structs.h src/Headers/FileSystem.h src/Bodies/FileSystem.c src/Bodies/IntegrityCheckers.c src/Headers/IntegrityCheckers.h src/Bodies/LinkedList.c src/Headers/LinkedList.h)