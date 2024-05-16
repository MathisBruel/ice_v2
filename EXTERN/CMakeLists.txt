cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(../help.cmake)

project("DEPENDENCIES")

# -- Options to static LIBRARIES
message("Indicate LIBRARIES must compile as static")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# -- Flags for static runtime library in windows
# -- Do not set if compilation for Premiere pro plugin
if (MSVC)
	message("Set flags MT for Visual Studio (only windows for us)")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd" CACHE STRING "" FORCE)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT" CACHE STRING "" FORCE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MT" CACHE STRING "" FORCE)
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd" CACHE STRING "" FORCE)
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT" CACHE STRING "" FORCE)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MT" CACHE STRING "" FORCE)
endif()

option(STAGE_COMPILATION "Stage of compilation (1 to 9)" "1")

if (${STAGE_COMPILATION} STREQUAL "1")
	### ----------------------------------------------------
	### ----------------------- Zlib -----------------------
	### ----------------------------------------------------
	set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../BUILD/ZLIB" CACHE PATH "" FORCE)
	add_subdirectory("${CMAKE_SOURCE_DIR}/ZLIB")
endif()
	
if (${STAGE_COMPILATION} STREQUAL "2")
	### ----------------------------------------------------
	### ----------------------- POCO -----------------------
	### ----------------------------------------------------
	# -- POCO compile CppUnit
	message("Indicate POCO must use CppUnit")
	#set(ENABLE_TESTS ON CACHE BOOL "" FORCE)
	
	# -- Static compilation options for windows 
	if (MSVC)
		message("Force compilation of Windows Runtime library as static (Only for windows)")
		set(POCO_MT ON CACHE BOOL "" FORCE)
	endif()
	
	message("Indicate POCO must use OpenSSL static library")
	set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL-Win64" CACHE PATH "" FORCE)
	set(OPENSSL_USE_STATIC_LIBS ON CACHE BOOL "" FORCE)
	set(OPENSSL_MSVC_STATIC_RT ON CACHE BOOL "" FORCE)
	set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../BUILD/POCO" CACHE PATH "" FORCE)
	
	add_subdirectory("${CMAKE_SOURCE_DIR}/POCO")
endif()

if (${STAGE_COMPILATION} STREQUAL "3")
	### ----------------------------------------------------
	### ----------------------- JPEG-TURBO -----------------------
	### ----------------------------------------------------
	set(ENABLE_SHARED OFF CACHE BOOL "" FORCE)
	set(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OFF CACHE BOOL "" FORCE)
	set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../BUILD/LIBJPEG" CACHE PATH "" FORCE)
	add_subdirectory("${CMAKE_SOURCE_DIR}/LIBJPEG")
endif()

if (${STAGE_COMPILATION} STREQUAL "4")
	### ------------------------------------------------------
	### ----------------------- LibPng -----------------------
	### ------------------------------------------------------
	# NEED : ZLIB
	set(CMAKE_POLICY_DEFAULT_CMP0074 NEW CACHE STRING "" FORCE)

	set(ZLIB_ROOT "${CMAKE_SOURCE_DIR}/../BUILD/ZLIB" CACHE PATH "" FORCE)
	if (MSVC)
		ICE_set_library(${ZLIB_ROOT} zlibstatic ZLIB_LIBRARY)
	else ()
		ICE_set_library(${ZLIB_ROOT} libz.a ZLIB_LIBRARY)
	endif()
	
	set(PNG_SHARED OFF CACHE BOOL "Build shared lib" FORCE)
	set(PNG_STATIC ON CACHE BOOL "Build static lib" FORCE)
	set(PNG_TESTS OFF CACHE BOOL "Build libpng tests" FORCE)
	set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../BUILD/LIBPNG" CACHE PATH "" FORCE)
	add_subdirectory("${CMAKE_SOURCE_DIR}/LIBPNG")
endif()

if (${STAGE_COMPILATION} STREQUAL "5")
	### -------------------------------------------------------
	### ----------------------- LibTiff -----------------------
	### -------------------------------------------------------
	# NEED : ZLIB
	set(CMAKE_POLICY_DEFAULT_CMP0074 NEW CACHE STRING "" FORCE)
	
	set(ZLIB_ROOT "${CMAKE_SOURCE_DIR}/../BUILD/ZLIB" CACHE PATH "" FORCE)
	if (MSVC)
		ICE_set_library(${ZLIB_ROOT} zlibstatic ZLIB_LIBRARY)
	else ()
		ICE_set_library(${ZLIB_ROOT} libz.a ZLIB_LIBRARY)
	endif()
	
	set(JPEG_ROOT "${CMAKE_SOURCE_DIR}/../BUILD/LIBJPEG" CACHE PATH "" FORCE)
	ICE_set_library(${JPEG_ROOT} jpeg-static JPEG_LIBRARY)
	
	set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../BUILD/LIBTIFF" CACHE PATH "" FORCE)
	add_subdirectory("${CMAKE_SOURCE_DIR}/LIBTIFF")
endif()

if (${STAGE_COMPILATION} STREQUAL "6")
	### -------------------------------------------------------
	### ----------------------- LIB TAR -----------------------
	### -------------------------------------------------------
	# NEED : ZLIB
	set(ZLIB_ROOT "${CMAKE_SOURCE_DIR}/../BUILD/ZLIB" CACHE PATH "" FORCE)
	if (MSVC)
		ICE_set_library(${ZLIB_ROOT} zlibstatic ZLIB_LIBRARY)
	else ()
		ICE_set_library(${ZLIB_ROOT} libz.a ZLIB_LIBRARY)
	endif()
	
	set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../BUILD/LIBTAR" CACHE PATH "" FORCE)
	add_subdirectory("${CMAKE_SOURCE_DIR}/LIBTAR")
endif()

if (${STAGE_COMPILATION} STREQUAL "7")
	### -------------------------------------------------------
	### ----------------------- LIB WEBP -----------------------
	### -------------------------------------------------------	
	set(WEBP_BUILD_ANIM_UTILS OFF CACHE BOOL "" FORCE)
	set(WEBP_BUILD_CWEBP OFF CACHE BOOL "" FORCE)
	set(WEBP_BUILD_DWEBP OFF CACHE BOOL "" FORCE)
	set(WEBP_BUILD_GIF2WEBP OFF CACHE BOOL "" FORCE)
	set(WEBP_BUILD_IMG2WEBP OFF CACHE BOOL "" FORCE)
	set(WEBP_BUILD_WEBPINFO OFF CACHE BOOL "" FORCE)
	set(WEBP_BUILD_WEBPMUX OFF CACHE BOOL "" FORCE)
	set(WEBP_BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
	set(WEBP_NEAR_LOSSLESS OFF CACHE BOOL "" FORCE)

	set(ZLIB_ROOT "${CMAKE_SOURCE_DIR}/../BUILD/ZLIB" CACHE PATH "" FORCE)
	if (MSVC)
		ICE_set_library(${ZLIB_ROOT} zlibstatic ZLIB_LIBRARY)
	else ()
		ICE_set_library(${ZLIB_ROOT} libz.a ZLIB_LIBRARY)
	endif()

	set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../BUILD/WEBP" CACHE PATH "" FORCE)
	add_subdirectory("${CMAKE_SOURCE_DIR}/WEBP")
endif()

if (${STAGE_COMPILATION} STREQUAL "8")
	### -------------------------------------------------------
	### --------------------- LIB TINYORM ---------------------
	### -------------------------------------------------------	

	set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../BUILD/TINYORM" CACHE PATH "" FORCE)
	list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/VCPKG/installed/x64-linux/share")
	set(VERBOSE_CONFIGURE ON CACHE BOOL "" FORCE)
	set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(MYSQL_PING OFF CACHE BOOL "" FORCE)
	set(TOM ON CACHE BOOL "" FORCE)
	set(TOM_EXAMPLE OFF CACHE BOOL "" FORCE)
	set(MATCH_EQUAL_EXPORTED_BUILDTREE ON CACHE BOOL "" FORCE)
	set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
	add_subdirectory("${CMAKE_SOURCE_DIR}/TINYORM")
endif()