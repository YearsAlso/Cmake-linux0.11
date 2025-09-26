cmake_minimum_required(VERSION 3.5)
project(fs C)

# Sources taken from fs/Makefile OBJS list
set(FS_SOURCES
  open.c
  read_write.c
  inode.c
  file_table.c
  buffer.c
  super.c
  block_dev.c
  char_dev.c
  file_dev.c
  stat.c
  exec.c
  pipe.c
  namei.c
  bitmap.c
  fcntl.c
  ioctl.c
  truncate.c
)

# Create native static library libfs.a in build directory
add_library(native_fs STATIC ${FS_SOURCES})
target_include_directories(native_fs PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_options(native_fs PRIVATE
  -Wall -O -fstrength-reduce -fcombine-regs -fomit-frame-pointer -mstring-insns -nostdinc
)

set_target_properties(native_fs PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
  OUTPUT_NAME fs
)

add_custom_target(native-fs DEPENDS native_fs)
