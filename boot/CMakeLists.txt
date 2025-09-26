cmake_minimum_required(VERSION 3.5)
project(boot C ASM)

find_program(AS86_EXECUTABLE as86)
find_program(LD86_EXECUTABLE ld86)

set(BOOT_OUT_DIR ${CMAKE_BINARY_DIR}/boot)
file(MAKE_DIRECTORY ${BOOT_OUT_DIR})

# Assemble head.s into an object usable by tools/system (placed in build dir)
add_custom_command(
  OUTPUT ${BOOT_OUT_DIR}/head.o
  COMMAND ${CMAKE_C_COMPILER} -x assembler -c ${CMAKE_SOURCE_DIR}/boot/head.s -o ${BOOT_OUT_DIR}/head.o
  DEPENDS ${CMAKE_SOURCE_DIR}/boot/head.s
  COMMENT "Assembling boot/head.s -> ${BOOT_OUT_DIR}/head.o"
  VERBATIM
)
add_custom_target(native-head DEPENDS ${BOOT_OUT_DIR}/head.o)

if(AS86_EXECUTABLE AND LD86_EXECUTABLE)
  # Use legacy as86/ld86 for 16-bit boot sectors and setup
  add_custom_command(
    OUTPUT ${BOOT_OUT_DIR}/setup
    COMMAND ${AS86_EXECUTABLE} -o ${BOOT_OUT_DIR}/setup.o ${CMAKE_SOURCE_DIR}/boot/setup.s
    COMMAND ${LD86_EXECUTABLE} -s -o ${BOOT_OUT_DIR}/setup ${BOOT_OUT_DIR}/setup.o
    DEPENDS ${CMAKE_SOURCE_DIR}/boot/setup.s
    COMMENT "Building boot/setup with as86/ld86 -> ${BOOT_OUT_DIR}/setup"
    VERBATIM
  )

  add_custom_command(
    OUTPUT ${BOOT_OUT_DIR}/bootsect
    COMMAND ${AS86_EXECUTABLE} -o ${BOOT_OUT_DIR}/bootsect.o ${CMAKE_SOURCE_DIR}/boot/bootsect.s
    COMMAND ${LD86_EXECUTABLE} -s -o ${BOOT_OUT_DIR}/bootsect ${BOOT_OUT_DIR}/bootsect.o
    DEPENDS ${CMAKE_SOURCE_DIR}/boot/bootsect.s
    COMMENT "Building boot/bootsect with as86/ld86 -> ${BOOT_OUT_DIR}/bootsect"
    VERBATIM
  )

  add_custom_target(native-boot DEPENDS ${BOOT_OUT_DIR}/head.o ${BOOT_OUT_DIR}/setup ${BOOT_OUT_DIR}/bootsect)
else()
  message(WARNING "as86/ld86 not found: native building of boot/setup and boot/bootsect is disabled. Install as86/ld86 or use legacy Makefile to produce boot image.")
  add_custom_target(native-boot DEPENDS ${BOOT_OUT_DIR}/head.o)
endif()
