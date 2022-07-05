cmake_minimum_required(VERSION 3.10)

message("Build Base64Decoder")
set(project_name "Base64Decoder")
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
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(${project_name} ${SRC_FILES})
target_link_libraries(${project_name} PRIVATE "LIB-BASE")

if (MSVC)
	target_link_libraries(${project_name} PUBLIC "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0/um/x64/iphlpapi.lib")
	target_include_directories(${project_name} PUBLIC "C:/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0/um/")
endif()

message("Set OUTPUT PATH of EXE in ${CMAKE_BINARY_DIR}/../BUILD")
install(TARGETS ${project_name} RUNTIME DESTINATION "${CMAKE_BINARY_DIR}/../BUILD/BIN")