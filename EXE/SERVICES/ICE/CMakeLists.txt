cmake_minimum_required(VERSION 3.10)

message("Build ICE-SERVICE")
set(project_name "ICE-SERVICE")
project(${project_name})

if (MSVC)
	message("Set flags MT for Visual Studio (only windows for us)")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd" CACHE STRING "" FORCE)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT" CACHE STRING "" FORCE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MT" CACHE STRING "" FORCE)
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd" CACHE STRING "" FORCE)
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT" CACHE STRING "" FORCE)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MT" CACHE STRING "" FORCE)
endif()

file(GLOB SRC_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/projection/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/projection/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/extraction/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/extraction/*.cpp"
)

add_executable(${project_name} ${SRC_FILES})
target_link_libraries(${project_name} PUBLIC "LIB-CONTEXT-ICE")
set_property(TARGET ${project_name} PROPERTY CXX_STANDARD 17)
if (UNIX)
	target_link_libraries(${project_name} PUBLIC pthread)
	target_link_libraries(${project_name} PUBLIC crypto)
	target_link_libraries(${project_name} PUBLIC ssl)
endif()

## -- add includes
target_include_directories(${project_name} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

message("Set OUTPUT PATH of ICE-SERVICE in ${CMAKE_BINARY_DIR}/../BUILD")
install(TARGETS ${project_name} RUNTIME DESTINATION "${CMAKE_BINARY_DIR}/../BUILD/BIN")