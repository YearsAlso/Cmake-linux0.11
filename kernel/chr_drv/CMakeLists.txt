cmake_minimum_required(VERSION 3.5)
project(chr_drv C)

set(CHR_SOURCES
  tty_io.c
  console.c
  keyboard.S
  serial.c
  rs_io.s
  tty_ioctl.c
)

# Preprocess keyboard.S to keyboard.s if necessary is left to the assembler/compile step
add_library(native_chr_drv STATIC ${CHR_SOURCES})
target_include_directories(native_chr_drv PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_options(native_chr_drv PRIVATE
  -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc
)

if(FCOMBINE_REGS_FLAG)
  target_compile_options(native_chr_drv PRIVATE ${FCOMBINE_REGS_FLAG})
endif()
if(MSTRING_INSNS_FLAG)
  target_compile_options(native_chr_drv PRIVATE ${MSTRING_INSNS_FLAG})
endif()

set_source_files_properties(${CMAKE_SOURCE_DIR}/kernel/chr_drv/keyboard.S PROPERTIES LANGUAGE ASM)

set_target_properties(native_chr_drv PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
  OUTPUT_NAME chr_drv
)

add_custom_target(native-chr-drivers DEPENDS native_chr_drv)
