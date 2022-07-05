cmake_minimum_required(VERSION 3.10)

set(project_name "cis-exporter")
project(${project_name} CXX)

file(GLOB SRC_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# -- create module
add_library(${project_name} MODULE "")
ICE_link_library("${CMAKE_SOURCE_DIR}/BUILD/LIBPNG" LibPng ${project_name})
ICE_link_library("${CMAKE_SOURCE_DIR}/BUILD/ZLIB" ZLib ${project_name})

# -- set extension and folder out
set_target_properties(${project_name} PROPERTIES FOLDER "premiere-plugins")
set_target_properties(${project_name} PROPERTIES SUFFIX ".prm")

# -- definition of compilation
target_compile_definitions(${project_name} PRIVATE -D_USRDLL)
target_compile_definitions(${project_name} PRIVATE -DWIN32_LEAN_AND_MEAN -D_WINDOWS -D$<$<CONFIG:DEBUG>:_DEBUG>)
target_compile_definitions(${project_name} PRIVATE -DPRWIN_ENV)

# -- options of compilation
target_compile_options(${project_name} PRIVATE -MP -W4 -Zi)
set_target_properties(${project_name} PROPERTIES LINK_FLAGS "-DEBUG")

# -- link sources and dependencies
target_sources(${project_name} PRIVATE "${SRC_FILES}")
target_include_directories(${project_name} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../SDK/PremierePro")

install(TARGETS ${project_name} LIBRARY DESTINATION "${CMAKE_BINARY_DIR}/../BUILD/PLUGIN")