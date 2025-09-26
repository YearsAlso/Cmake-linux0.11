cmake_minimum_required(VERSION 3.5)
project(blk_drv C)

set(BLK_SOURCES
  ll_rw_blk.c
  floppy.c
  hd.c
  ramdisk.c
)

add_library(native_blk_drv STATIC ${BLK_SOURCES})
target_include_directories(native_blk_drv PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_options(native_blk_drv PRIVATE
  -Wall -O -fstrength-reduce -fomit-frame-pointer -fcombine-regs -finline-functions -mstring-insns -nostdinc -I${CMAKE_SOURCE_DIR}/kernel/blk_drv
)

set_target_properties(native_blk_drv PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
  OUTPUT_NAME blk_drv
)

add_custom_target(native-blk-drivers DEPENDS native_blk_drv)
