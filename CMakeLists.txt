function(add_ca65_object out src)
  add_custom_command(
    OUTPUT ${out}
    COMMAND ${CA65_BIN} -o ${out} ${CMAKE_CURRENT_SOURCE_DIR}/${src}
    DEPENDS ${src}
    VERBATIM)
endfunction()

add_ca65_object(famistudio_ca65.o famistudio_ca65.s)
add_ca65_object(musicdata.o musicdata.s)
add_ca65_object(sfxdata.o sfxdata.s)

cmake_minimum_required(VERSION 3.18)

set(LLVM_MOS_PLATFORM nes-nrom)
find_package(llvm-mos-sdk REQUIRED)

project(test LANGUAGES C CXX ASM)

set(CMAKE_EXECUTABLE_SUFFIX .nes)

add_executable(test test.cpp chr-rom.s famistudio_ca65.o musicdata.o sfxdata.o)
target_include_directories(test PRIVATE .)
target_link_libraries(test neslib)
target_link_libraries(test nesdoug)
set_property(SOURCE chr-rom.s PROPERTY
  OBJECT_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/blocks.chr)