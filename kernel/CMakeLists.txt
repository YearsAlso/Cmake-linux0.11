cmake_minimum_required(VERSION 3.5)
project(kernel C ASM)

# Kernel sources (from kernel/Makefile OBJS list)
set(KERNEL_SOURCES
  sched.c
  system_call.s
  traps.c
  fork.c
  panic.c
  printk.c
  vsprintf.c
  sys.c
  exit.c
  signal.c
  mktime.c
  asm.s
)

# Create a native static library (libkernel.a) in the build directory
add_library(native_kernel STATIC ${KERNEL_SOURCES})

# Kernel needs the project include dir
target_include_directories(native_kernel PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Apply compile options similar to the original Makefile
target_compile_options(native_kernel PRIVATE
  -Wall -O -fstrength-reduce -fomit-frame-pointer
  -finline-functions -nostdinc
)

if(FCOMBINE_REGS_FLAG)
  target_compile_options(native_kernel PRIVATE ${FCOMBINE_REGS_FLAG})
endif()
if(MSTRING_INSNS_FLAG)
  target_compile_options(native_kernel PRIVATE ${MSTRING_INSNS_FLAG})
endif()

# Make sure assembler files are treated as ASM language
set_source_files_properties(${CMAKE_SOURCE_DIR}/kernel/asm.s PROPERTIES LANGUAGE ASM)

# Put the produced static library into the top-level build directory (build/lib)
set_target_properties(native_kernel PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
  OUTPUT_NAME kernel
)

## Add a small alias target name for backwards compatibility
add_custom_target(native-kernel DEPENDS native_kernel)

if(EXISTS ${CMAKE_SOURCE_DIR}/kernel/blk_drv/CMakeLists.txt)
  add_subdirectory(blk_drv EXCLUDE_FROM_ALL)
  message(STATUS "Added native blk_drv subdirectory (target: native-blk-drivers)")
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/kernel/chr_drv/CMakeLists.txt)
  add_subdirectory(chr_drv EXCLUDE_FROM_ALL)
  message(STATUS "Added native chr_drv subdirectory (target: native-chr-drivers)")
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/kernel/math/CMakeLists.txt)
  add_subdirectory(math EXCLUDE_FROM_ALL)
  message(STATUS "Added native math subdirectory (target: native-math)")
endif()
