cmake_minimum_required(VERSION 3.5)
project(tools C)

add_executable(native_build build.c)
target_include_directories(native_build PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_options(native_build PRIVATE -Wall -O)

set_target_properties(native_build PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
  OUTPUT_NAME build
)

add_custom_target(native-tools DEPENDS native_build)

# Create native 'tools/system' executable by linking the converted native objects and libs
add_executable(native_system)

# Always try to include boot head object if the boot subdir produced it
if(EXISTS ${CMAKE_BINARY_DIR}/boot/head.o)
  target_sources(native_system PRIVATE ${CMAKE_BINARY_DIR}/boot/head.o)
else()
  # If head.o is not yet produced at configure time, add an explicit dependency
  add_custom_command(TARGET native_system PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Warning: boot/head.o not found at configure time; ensure boot target is built before native_system"
  )
endif()

# Link init/main.o object coming from the native_init_objects OBJECT library if present
if(TARGET native_init_objects)
  target_sources(native_system PRIVATE $<TARGET_OBJECTS:native_init_objects>)
  add_dependencies(native_system native-init)
endif()

# Collect native libraries (link only if targets exist)
if(TARGET native_kernel)
  target_link_libraries(native_system PRIVATE ${CMAKE_BINARY_DIR}/lib/libkernel.a)
  add_dependencies(native_system native-kernel)
endif()
if(TARGET native-mm)
  # mm produces a relocatable mm.o in source tree
  target_link_libraries(native_system PRIVATE ${CMAKE_SOURCE_DIR}/mm.o)
  add_dependencies(native_system native-mm)
endif()
if(TARGET native_fs)
  target_link_libraries(native_system PRIVATE ${CMAKE_BINARY_DIR}/lib/libfs.a)
  add_dependencies(native_system native-fs)
endif()
if(TARGET native-blk-drivers)
  target_link_libraries(native_system PRIVATE ${CMAKE_BINARY_DIR}/lib/libblk_drv.a)
  add_dependencies(native_system native-blk-drivers)
endif()
if(TARGET native-chr-drivers)
  target_link_libraries(native_system PRIVATE ${CMAKE_BINARY_DIR}/lib/libchr_drv.a)
  add_dependencies(native_system native-chr-drivers)
endif()
if(TARGET native-math)
  target_link_libraries(native_system PRIVATE ${CMAKE_BINARY_DIR}/lib/libmath.a)
  add_dependencies(native_system native-math)
endif()
if(TARGET native_lib)
  # Original lib/lib.a is placed in source dir by native_lib target
  target_link_libraries(native_system PRIVATE ${CMAKE_SOURCE_DIR}/lib/lib.a)
  add_dependencies(native_system native-lib)
endif()

# Place the resulting executable into build/bin and give an alias target
set_target_properties(native_system PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
  OUTPUT_NAME system
)

add_custom_target(native-system DEPENDS native_system)
