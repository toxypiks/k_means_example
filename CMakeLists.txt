cmake_minimum_required(VERSION 3.16)
project(byte_pair_encoding)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

find_package(raylib)

set(MAIN_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
  )

add_executable(main)
target_sources(main
 PRIVATE
  ${MAIN_SRC}
)

target_link_libraries(main
  ${raylib_LIBRARIES}
  m
  ui_stuff
)

set(MAIN_WITH_WIDGETS_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_with_widgets.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/data.c
  ${CMAKE_CURRENT_SOURCE_DIR}/include/data.h
  )

add_executable(main_with_widgets)
target_sources(main_with_widgets
 PRIVATE
  ${MAIN_WITH_WIDGETS_SRC}
)

target_link_libraries(main_with_widgets
  ${raylib_LIBRARIES}
  m
  ui_stuff
)

set(UI_STUFF_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_stuff.c
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ui_stuff.h
  )

add_library(ui_stuff)
target_sources(ui_stuff
 PRIVATE
  ${UI_STUFF_SOURCES}
  )

target_include_directories(ui_stuff
 PRIVATE
 ${INCLUDE}
 )
