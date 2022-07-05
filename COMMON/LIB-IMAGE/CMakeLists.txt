cmake_minimum_required(VERSION 3.10)

message("Build LIB-IMAGE")
set(project_name "LIB-IMAGE")
project(${project_name})

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
)

add_library(${project_name} STATIC ${SRC_FILES})
target_link_libraries(${project_name} PUBLIC "LIB-BASE")

# -- force not include macro min and max from native windows
if (WIN32)
	target_compile_definitions(${project_name} PUBLIC -DNOMINMAX -D_WINDOWS -DOIIO_STATIC_DEFINE=1)
endif()

# -- add LIBTIFF
ICE_link_library("${CMAKE_SOURCE_DIR}/BUILD/LIBTIFF" LibTiff ${project_name})

if (UNIX)
	target_link_libraries(${project_name} PUBLIC "/usr/lib/x86_64-linux-gnu/liblzma.a")
	target_link_libraries(${project_name} PUBLIC "/usr/lib/x86_64-linux-gnu/libjbig.a")
endif()

# -- add LIBPNG
ICE_link_library("${CMAKE_SOURCE_DIR}/BUILD/LIBPNG" LibPng ${project_name})

# -- add LIBJPEG
ICE_link_library("${CMAKE_SOURCE_DIR}/BUILD/LIBJPEG" LibJpeg ${project_name})

# -- add POCO
ICE_link_Poco_library("${CMAKE_SOURCE_DIR}/BUILD/POCO" Poco ${project_name})

# -- add ZLIB
ICE_link_library("${CMAKE_SOURCE_DIR}/BUILD/ZLIB" ZLib ${project_name})

if (UNIX)
	target_link_libraries(${project_name} PUBLIC pthread)
	target_link_libraries(${project_name} PUBLIC crypto)
	target_link_libraries(${project_name} PUBLIC ssl)
endif()

## -- add includes
target_include_directories(${project_name} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
