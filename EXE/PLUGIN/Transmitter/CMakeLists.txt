cmake_minimum_required(VERSION 3.10)

set(project_name "transmitter")
project(${project_name} CXX)

file(GLOB SRC_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# -- create module
add_library(${project_name} MODULE "")

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

set(LIBS "Ole32.lib" "Kernel32.lib" "Shlwapi.lib" "Shell32.lib" "Ws2_32.lib") # Ole32: CoCreateGuid(), Kernel32: CreateProcessW(), Shlwapi: StrDupW(), Shell32: SHGetKnownFolderPath(), Ws2_32: winsock2.h
target_link_libraries(${project_name} PUBLIC ${LIBS})

# -- link sources and dependencies
target_sources(${project_name} PRIVATE "${SRC_FILES}")
target_include_directories(${project_name} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../SDK/PremierePro")

install(TARGETS ${project_name} LIBRARY DESTINATION "${CMAKE_BINARY_DIR}/../BUILD/PLUGIN")