cmake_minimum_required(VERSION 3.5)
project(mm C)

# Sources from the legacy Makefile
set(MM_C_SOURCES
  memory.c
)

set(MM_AS_SOURCES
  page.s
)

# Create object files for C sources
set(MM_OBJECTS "")
foreach(src ${MM_C_SOURCES})
  get_filename_component(fname ${src} NAME_WE)
  set(obj ${CMAKE_CURRENT_BINARY_DIR}/${fname}.o)
  add_custom_command(
    OUTPUT ${obj}
    COMMAND ${CMAKE_C_COMPILER} ${CMAKE_C_FLAGS} -O -Wall -fstrength-reduce -fcombine-regs -fomit-frame-pointer -finline-functions -nostdinc -I${CMAKE_SOURCE_DIR}/include -c ${CMAKE_SOURCE_DIR}/mm/${src} -o ${obj}
    DEPENDS ${CMAKE_SOURCE_DIR}/mm/${src}
    COMMENT "Compiling ${src}"
    VERBATIM
  )
  list(APPEND MM_OBJECTS ${obj})
endforeach()

# Assemble .s sources to object files
foreach(assrc ${MM_AS_SOURCES})
  get_filename_component(fname ${assrc} NAME_WE)
  set(obj ${CMAKE_CURRENT_BINARY_DIR}/${fname}.o)
  add_custom_command(
    OUTPUT ${obj}
    COMMAND ${CMAKE_ASM_COMPILER} -o ${obj} ${CMAKE_SOURCE_DIR}/mm/${assrc}
    DEPENDS ${CMAKE_SOURCE_DIR}/mm/${assrc}
    COMMENT "Assembling ${assrc}"
    VERBATIM
  )
  list(APPEND MM_OBJECTS ${obj})
endforeach()

# mm.o is a relocatable object produced by 'ld -r -o mm.o <objs>' in the original Makefile
set(MM_OUTPUT ${CMAKE_SOURCE_DIR}/mm.o)
add_custom_command(
  OUTPUT ${MM_OUTPUT}
  COMMAND ${CMAKE_LINKER} -r -o ${MM_OUTPUT} ${MM_OBJECTS}
  DEPENDS ${MM_OBJECTS}
  COMMENT "Linking relocatable object mm.o"
  VERBATIM
)

add_custom_target(native-mm ALL DEPENDS ${MM_OUTPUT})
