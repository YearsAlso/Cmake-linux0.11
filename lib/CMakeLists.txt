cmake_minimum_required(VERSION 3.5)
project(lib_static C)

# Build a static library 'lib.a' from the sources listed in the original Makefile.
set(LIB_SOURCES
  ctype.c
  _exit.c
  open.c
  close.c
  errno.c
  write.c
  dup.c
  setsid.c
  execve.c
  wait.c
  string.c
  malloc.c
)

# Set compile options similar to original Makefile
add_library(native_lib STATIC ${LIB_SOURCES})
target_include_directories(native_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_options(native_lib PRIVATE
  -Wall -O -fstrength-reduce -fomit-frame-pointer
  -finline-functions -nostdinc
)
# Append legacy flags only if the compiler supports them
if(FCOMBINE_REGS_FLAG)
  target_compile_options(native_lib PRIVATE ${FCOMBINE_REGS_FLAG})
endif()
if(MSTRING_INSNS_FLAG)
  target_compile_options(native_lib PRIVATE ${MSTRING_INSNS_FLAG})
endif()

# Place output static lib as lib/lib.a to match original layout
set_target_properties(native_lib PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
  OUTPUT_NAME lib
)

add_custom_target(native-lib ALL DEPENDS native_lib)
