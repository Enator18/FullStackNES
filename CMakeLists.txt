cmake_minimum_required(VERSION 3.18)

set(LLVM_MOS_PLATFORM nes-nrom)
find_package(llvm-mos-sdk REQUIRED)

project(test LANGUAGES C CXX ASM)

set(CMAKE_EXECUTABLE_SUFFIX .nes)

add_executable(test test.cpp chr-rom.s)
target_include_directories(test PRIVATE .)
target_link_libraries(test neslib)
target_link_libraries(test nesdoug)
set_property(SOURCE chr-rom.s PROPERTY
  OBJECT_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/blocks.chr)