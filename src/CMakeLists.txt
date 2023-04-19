cmake_minimum_required(VERSION 3.20)
project(Qi C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  set(CMAKE_EXE_LINKER_FLAGS "-lm")
endif()

add_executable(qi main.c common.h chunk.h chunk.c memory.h memory.c debug.h debug.c value.h value.c vm.h vm.c compiler.h compiler.c scanner.h scanner.c object.h object.c table.h table.c common.h chunk.h chunk.c compiler.c compiler.h core_module.c core_module.h)
