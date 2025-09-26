cmake_minimum_required(VERSION 3.5)
project(math C)

set(MATH_SOURCES
  math_emulate.c
)

add_library(native_math STATIC ${MATH_SOURCES})
target_include_directories(native_math PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_options(native_math PRIVATE
  -Wall -O -fstrength-reduce -fomit-frame-pointer -fcombine-regs -finline-functions -mstring-insns -nostdinc
)

set_target_properties(native_math PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
  OUTPUT_NAME math
)

add_custom_target(native-math DEPENDS native_math)
