cmake_minimum_required(VERSION 3.26)

project(dsnet2
  LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/BuildDep.cmake)

add_subdirectory(dep/libuv EXCLUDE_FROM_ALL)

add_executable(dsnetm
  src/main.c
  src/cmd.c
  src/misc.c
  src/dsnetm.c
)

target_include_directories(dsnetm PRIVATE include readline_includes ncurses_includes)
target_link_libraries(dsnetm PRIVATE tinfo_a ncurses_a readline_a uv_a)

set(PROGRAMS
  dsecons
  dsedb
  dsefilesv
  dselist
  dseping
  dsestart

  dsicons
  dsidb
  dsifilesv
  dsiping
  dsistart

  dscomp
)

macro(copy_exe output_name)
add_custom_command(
    TARGET dsnetm POST_BUILD
    BYPRODUCTS ${output_name}
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_BINARY_DIR}/dsnetm
            ${CMAKE_CURRENT_BINARY_DIR}/${output_name}
  )
endmacro()

foreach(exe ${PROGRAMS})
  copy_exe(${exe})
endforeach()
