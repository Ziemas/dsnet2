cmake_minimum_required(VERSION 3.26)

project(dsnet2
  LANGUAGES C)

include(ExternalProject)

ExternalProject_Add(ncurses
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/dep/ncurses
    CONFIGURE_COMMAND <SOURCE_DIR>/configure
        --prefix=<INSTALL_DIR>
        --with-termlib
        --with-terminfo-dirs=/lib/terminfo:/usr/share/terminfo:/etc/terminfo
        --disable-db-install
        --without-progs
        --without-manpages
    BUILD_BYPRODUCTS <INSTALL_DIR>/lib/libtinfo.a
                      <INSTALL_DIR>/lib/libncurses.a
)

ExternalProject_Add(readline
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/dep/readline
    CONFIGURE_COMMAND <SOURCE_DIR>/configure
        --prefix=<INSTALL_DIR>
        --disable-shared
        --with-curses
    BUILD_BYPRODUCTS <INSTALL_DIR>/lib/libreadline.a
)

ExternalProject_Get_Property(ncurses install_dir)
set(ncurses_includes ${install_dir}/include)
add_library(tinfo_a STATIC IMPORTED)
set_property(TARGET tinfo_a PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libtinfo.a)
add_dependencies(tinfo_a ncurses)

add_library(ncurses_a STATIC IMPORTED)
set_property(TARGET ncurses_a PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libncurses.a)
add_dependencies(ncurses_a ncurses)

ExternalProject_Get_Property(readline install_dir)
set(readline_includes ${install_dir}/include)
add_library(readline_a STATIC IMPORTED)
set_property(TARGET readline_a PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libreadline.a)
add_dependencies(readline_a readline)

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
