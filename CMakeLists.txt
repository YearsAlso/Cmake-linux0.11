cmake_minimum_required(VERSION 3.5)
project(linux0_11 C)

# This top-level CMakeLists provides a lightweight wrapper around the
# existing Makefile so you can use CMake to drive the legacy build while
# we prepare a full native conversion. It intentionally does NOT try to
# reimplement all low-level assembler/linker steps from the historic
# build; instead it delegates to the original Makefile targets.

include(ProcessorCount)
ProcessorCount(N)
if(N EQUAL 0)
  set(N 1)
endif()

set(MAKE_PROG ${CMAKE_MAKE_PROGRAM})
if(NOT MAKE_PROG)
  find_program(MAKE_PROG make)
endif()

message(STATUS "Using Make program: ${MAKE_PROG}")
# Ensure the C language is enabled before running compiler-flag checks
enable_language(C)

# Detect support for a couple of legacy GCC flags used by the original project.
# Modern toolchains may reject these options; detect them at configure time and
# expose variables for subdirectories to use when available.
include(CheckCCompilerFlag)
check_c_compiler_flag("-fcombine-regs" COMPILER_SUPPORTS_FCOMBINE_REGS)
check_c_compiler_flag("-mstring-insns" COMPILER_SUPPORTS_MSTRING_INSNS)
if(COMPILER_SUPPORTS_FCOMBINE_REGS)
  set(FCOMBINE_REGS_FLAG "-fcombine-regs")
else()
  set(FCOMBINE_REGS_FLAG "")
endif()
if(COMPILER_SUPPORTS_MSTRING_INSNS)
  set(MSTRING_INSNS_FLAG "-mstring-insns")
else()
  set(MSTRING_INSNS_FLAG "")
endif()
message(STATUS "Compiler supports -fcombine-regs: ${COMPILER_SUPPORTS_FCOMBINE_REGS}")
message(STATUS "Compiler supports -mstring-insns: ${COMPILER_SUPPORTS_MSTRING_INSNS}")

# Wrapper target: run the original Makefile default target (Image)
add_custom_target(legacy-all
  COMMAND ${MAKE_PROG} -j${N}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Building project via legacy Makefile"
)

add_custom_target(legacy-image
  COMMAND ${MAKE_PROG} Image -j${N}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Creating Image via legacy Makefile"
)

add_custom_target(legacy-clean
  COMMAND ${MAKE_PROG} clean
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Cleaning via legacy Makefile"
)

add_custom_target(legacy-tools
  COMMAND ${MAKE_PROG} tools/build tools/system -j${N}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Build tools targets via legacy Makefile"
)

message(STATUS "Legacy Makefile wrapper targets: legacy-all, legacy-image, legacy-clean, legacy-tools")

## Add native subdirectories (incremental migration)
if(EXISTS ${CMAKE_SOURCE_DIR}/lib/CMakeLists.txt)
  add_subdirectory(lib EXCLUDE_FROM_ALL)
  message(STATUS "Added native lib subdirectory (target: native-lib)")
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/mm/CMakeLists.txt)
  add_subdirectory(mm EXCLUDE_FROM_ALL)
  message(STATUS "Added native mm subdirectory (target: native-mm)")
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/kernel/CMakeLists.txt)
  add_subdirectory(kernel EXCLUDE_FROM_ALL)
  message(STATUS "Added native kernel subdirectory (target: native-kernel)")
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/fs/CMakeLists.txt)
  add_subdirectory(fs EXCLUDE_FROM_ALL)
  message(STATUS "Added native fs subdirectory (target: native-fs)")
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/init/CMakeLists.txt)
  add_subdirectory(init EXCLUDE_FROM_ALL)
  message(STATUS "Added native init subdirectory (target: native-init)")
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/tools/CMakeLists.txt)
  add_subdirectory(tools EXCLUDE_FROM_ALL)
  message(STATUS "Added native tools subdirectory (target: native-tools)")
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/boot/CMakeLists.txt)
  add_subdirectory(boot EXCLUDE_FROM_ALL)
  message(STATUS "Added native boot subdirectory (target: native-boot)")
endif()

# Helpful instructions as a CMake target
add_custom_target(info ALL
  COMMENT "Run 'cmake --build . --target legacy-image' to build Image, or use legacy-all/legacy-clean. Native targets (native-lib) are available when built by CMake."
)

# Aggregate native target: depend only on native targets that exist
add_custom_target(native-all)
if(TARGET native-lib)
  add_dependencies(native-all native-lib)
endif()
if(TARGET native-mm)
  add_dependencies(native-all native-mm)
endif()
if(TARGET native-kernel)
  add_dependencies(native-all native-kernel)
endif()
if(TARGET native-fs)
  add_dependencies(native-all native-fs)
endif()
if(TARGET native-init)
  add_dependencies(native-all native-init)
endif()
if(TARGET native-tools)
  add_dependencies(native-all native-tools)
endif()
if(TARGET native-boot)
  add_dependencies(native-all native-boot)
endif()

