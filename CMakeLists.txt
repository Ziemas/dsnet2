cmake_minimum_required(VERSION 3.26)

project(dsnet2
  LANGUAGES C)


find_package(PkgConfig REQUIRED)
pkg_check_modules(readline REQUIRED readline)

add_subdirectory(dep/libuv EXCLUDE_FROM_ALL)

add_subdirectory(src)
