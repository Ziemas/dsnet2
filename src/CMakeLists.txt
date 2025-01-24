add_executable(dsnetm
  cmd.c
  misc.c
  dsnetm.c
  )

target_include_directories(dsnetm PRIVATE ../include)
target_link_libraries(dsnetm PRIVATE uv readline)
