cmake_minimum_required(VERSION 3.10)

set(project_name "movings-controler")
project(${project_name} CXX)

file(GLOB SRC_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.r"
  "${CMAKE_CURRENT_SOURCE_DIR}/../SDK/AfterEffect/Util/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/../SDK/AfterEffect/Util/*.cpp"
)

# -- create module
add_library(${project_name} MODULE "")

# -- set extension and folder out
set_target_properties(${project_name} PROPERTIES FOLDER "effects-plugins")
set_target_properties(${project_name} PROPERTIES SUFFIX ".aex")

# -- definition of compilation
target_compile_definitions(${project_name} PRIVATE -D_USRDLL)
target_compile_definitions(${project_name} PRIVATE -DWIN32_LEAN_AND_MEAN -D_WINDOWS -D$<$<CONFIG:DEBUG>:_DEBUG>)
target_compile_definitions(${project_name} PRIVATE -DPRWIN_ENV)

# -- options of compilation
target_compile_options(${project_name} PRIVATE -MP -W4 -Zi)
set_target_properties(${project_name} PROPERTIES LINK_FLAGS "-DEBUG")

# -- link sources and dependencies
target_sources(${project_name} PRIVATE "${SRC_FILES}")
source_group("After_Effects_utils" FILES "${CMAKE_CURRENT_SOURCE_DIR}/../SDK/AfterEffect/Util/")
target_include_directories(${project_name} PRIVATE 
  "${CMAKE_CURRENT_SOURCE_DIR}/../SDK/AfterEffect/Headers"
  "${CMAKE_CURRENT_SOURCE_DIR}/../SDK/AfterEffect/Headers/SP"
  "${CMAKE_CURRENT_SOURCE_DIR}/../SDK/AfterEffect/Util")

# -- compile PiPL file 
set(src_r_file "${CMAKE_CURRENT_SOURCE_DIR}/MovingControlerPiPL.r")
get_filename_component(stem_file ${src_r_file} "NAME_WE")
set(bin_rr_file  ${CMAKE_CURRENT_BINARY_DIR}/${stem_file}.rr)
set(bin_rrc_file ${CMAKE_CURRENT_BINARY_DIR}/${stem_file}.rrc)
set(bin_rc_file  ${CMAKE_CURRENT_BINARY_DIR}/${stem_file}.rc)
file(WRITE ${bin_rc_file} "")
add_custom_command(TARGET ${project_name} PRE_BUILD MAIN_DEPENDENCY ${src_r_file}
    COMMAND cl -D "PRWIN_ENV" -I "${CMAKE_CURRENT_SOURCE_DIR}/../SDK/AfterEffect/Headers" -EP ${src_r_file} > ${bin_rr_file}
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/../SDK/AfterEffect/Resources/PiPLTool.exe" ${bin_rr_file} ${bin_rrc_file}
    COMMAND cl -D MSWindows -EP ${bin_rrc_file} > ${bin_rc_file})
target_sources(${project_name} PRIVATE ${bin_rc_file})

install(TARGETS ${project_name} LIBRARY DESTINATION "${CMAKE_BINARY_DIR}/../BUILD/PLUGIN")