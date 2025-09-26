cmake_minimum_required(VERSION 3.5)
project(init C)

set(INIT_SOURCES
  main.c
)

# Compile main.c into an object in the build directory
add_library(native_init_objects OBJECT ${INIT_SOURCES})
target_include_directories(native_init_objects PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_options(native_init_objects PRIVATE
  -Wall -O -fstrength-reduce -fomit-frame-pointer -fcombine-regs -nostdinc
)

set_target_properties(native_init_objects PROPERTIES
  POSITION_INDEPENDENT_CODE OFF
)

add_custom_target(native-init DEPENDS native_init_objects)
