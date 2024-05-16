cmake_minimum_required(VERSION VERSION 3.22...3.29 FATAL_ERROR)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message("Build ICE-MIGRATION")


# Initialize variables
# ---

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
set(CMAKE_INSTALL_PREFIX "BUILD/ICE-MIGRATION" CACHE PATH "" FORCE)

set(ICE_MIGRATION_ns ICE-MIGRATION)
set(ICE_MIGRATION_target ICE-MIGRATION)

set(TinyOrmSourceDir "EXTERN/TINYORM")
set(TinyOrmBuildDir "BUILD/TINYORM")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/EXTERN/VCPKG/installed/x64-linux/share/")
set(TINYORM_CMAKE_COMMON_MODULES_DIR "${CMAKE_SOURCE_DIR}/${TinyOrmSourceDir}/cmake/CommonModules")

# TinyORM CMake modules (needed to set the executable version and RC file on Windows)
list(APPEND CMAKE_MODULE_PATH "${TinyOrmSourceDir}/cmake/CommonModules")

# build tree
list(APPEND CMAKE_PREFIX_PATH "${TinyOrmBuildDir}")

# Basic project
# ---

project(${ICE_MIGRATION_ns}
    DESCRIPTION "ICE_MIGRATION console application for TinyORM C++ library"
    HOMEPAGE_URL "https://www.tinyorm.org"
    LANGUAGES CXX
)

# ICE_MIGRATION command-line application
# ---

add_executable(${ICE_MIGRATION_target}
    main.cpp
)
add_executable(${ICE_MIGRATION_ns}::${ICE_MIGRATION_target} ALIAS ${ICE_MIGRATION_target})

# ICE_MIGRATION command-line application specific configuration
# ---

set_target_properties(${ICE_MIGRATION_target}
    PROPERTIES
        C_VISIBILITY_PRESET "hidden"
        CXX_VISIBILITY_PRESET "hidden"
        VISIBILITY_INLINES_HIDDEN YES
)

target_include_directories(${ICE_MIGRATION_target}
    PRIVATE "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/database>"
)

# ICE_MIGRATION command-line application defines
# ---

target_compile_definitions(${ICE_MIGRATION_target}
    PRIVATE
        PROJECT_ICE_MIGRATION
)

# Windows resource and manifest files
# ---

# Find icons, ICE_MIGRATION/version.hpp, and Windows manifest file for MinGW
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    tiny_set_rc_flags("-I \"${TinyOrmSourceDir}/tom/resources\"")
endif()

include("${TINYORM_CMAKE_COMMON_MODULES_DIR}/TinyResourceAndManifest.cmake")
tiny_resource_and_manifest(${ICE_MIGRATION_target}
    OUTPUT_DIR "${TINY_BUILD_GENDIR}/tmp/"
    RESOURCES_DIR "${TinyOrmSourceDir}/tom/resources"
)

# Resolve and link dependencies
# ---

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
find_package(TinyOrm 0.37.1 CONFIG REQUIRED)

# Unconditional dependencies
target_link_libraries(${ICE_MIGRATION_target}
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        TinyOrm::TinyOrm
)
message("Set OUTPUT PATH of ICE-MIGRATION in ${CMAKE_BINARY_DIR}/../BUILD")
install(TARGETS ICE-MIGRATION RUNTIME DESTINATION "${CMAKE_BINARY_DIR}/../BUILD/BIN")