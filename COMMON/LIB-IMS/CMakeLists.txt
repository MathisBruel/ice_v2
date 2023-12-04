cmake_minimum_required(VERSION 3.10)

message("Build LIB-IMS")
set(project_name "LIB-IMS")
project(${project_name})
set (CMAKE_CXX_STANDARD 17)

# -- force compilation as MT (windows only)
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
  "${CMAKE_CURRENT_SOURCE_DIR}/include/device/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/device/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/xml/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/xml/*.cpp"
)

add_library(${project_name} STATIC ${SRC_FILES})
set_property(TARGET ${project_name} PROPERTY CXX_STANDARD 17)

target_link_libraries(${project_name} PUBLIC "LIB-CMD")
target_link_libraries(${project_name} PUBLIC "LIB-SYNCHRO")
target_link_libraries(${project_name} PUBLIC "LIB-BASE")

# -- force not include macro min and max from native windows
if (WIN32)
	target_compile_definitions(${project_name} PUBLIC -DNOMINMAX -D_WINDOWS -DOIIO_STATIC_DEFINE=1)
	ICE_link_library("C:/Program Files/OpenSSL-Win64" "OPENSSL" ${project_name})
endif()

if (UNIX)
	target_link_libraries(${project_name} PUBLIC pthread)
	target_link_libraries(${project_name} PUBLIC crypto)
	target_link_libraries(${project_name} PUBLIC ssl)
endif()

## -- add includes
target_include_directories(${project_name} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
