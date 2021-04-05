cmake_minimum_required(VERSION 3.0.0)
project(PLT_projet C)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
SET(CMAKE_C_COMPILER "/usr/bin/gcc")

file(GLOB 1_lex
        "1_lex/*.h"
        "1_lex/*.c")

file(GLOB 2_syn
        "2_syn/*.h"
        "2_syn/*.c")

file(GLOB 3_table
        "3_table/*.h"
        "3_table/*.c")

file(GLOB 4_execute
        "4_execute/*.h"
        "4_execute/*.c")

file(GLOB tools
        "tools/*.h"
        "tools/*.c")

add_executable(analex test/projet1.c ${1_lex})
add_executable(anasyn test/projet2.c ${2_syn} ${tools})
add_executable(table test/projet3.c ${2_syn} ${3_table} ${tools})
add_executable(execute test/projet4.c ${1_lex} ${2_syn} ${3_table} ${4_execute} ${tools})
add_executable(test1 test/test1.c)