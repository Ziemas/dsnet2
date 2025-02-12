include(ExternalProject)

# Build bundled ncurses, needed for readline
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

ExternalProject_Get_Property(ncurses install_dir)
set(ncurses_includes ${install_dir}/include)
add_library(tinfo_a STATIC IMPORTED)
set_property(TARGET tinfo_a PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libtinfo.a)
add_dependencies(tinfo_a ncurses)

add_library(ncurses_a STATIC IMPORTED)
set_property(TARGET ncurses_a PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libncurses.a)
add_dependencies(ncurses_a ncurses)

# Build bundled readline
ExternalProject_Add(readline
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/dep/readline
    CONFIGURE_COMMAND <SOURCE_DIR>/configure
        --prefix=<INSTALL_DIR>
        --disable-shared
        --with-curses
    BUILD_BYPRODUCTS <INSTALL_DIR>/lib/libreadline.a
)

ExternalProject_Get_Property(readline install_dir)
set(readline_includes ${install_dir}/include)
add_library(readline_a STATIC IMPORTED)
set_property(TARGET readline_a PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libreadline.a)
add_dependencies(readline_a readline)
