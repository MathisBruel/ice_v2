cmake_minimum_required(VERSION 3.22...3.29 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CommonModules"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules"
)

include(TinyHelpers)
include(TinyInitDefaultVariables)

tiny_init_tiny_variables_pre()

tiny_read_version(TINY_VERSION
    TINY_VERSION_MAJOR TINY_VERSION_MINOR TINY_VERSION_PATCH TINY_VERSION_TWEAK
    VERSION_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/orm/version.hpp"
    PREFIX TINYORM
    HEADER_FOR "${TinyOrm_ns}"
)

project(${TinyOrm_ns}
    DESCRIPTION "Modern C++ ORM library for Qt framework"
    HOMEPAGE_URL "https://www.tinyorm.org"
    LANGUAGES CXX
    VERSION ${TINY_VERSION}
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC OFF)

set(minReqMsvcVersion    19.29)
set(minRecGCCVersion     10.2)
set(minRecClangVersion   12)
set(minReqClangClVersion 14.0.3)
set(minQtVersion       5.15.2)

tiny_toolchain_requirement(
    MSVC     ${minReqMsvcVersion}
    CLANG_CL ${minReqClangClVersion}
    GCC      ${minRecGCCVersion}
    CLANG    ${minRecClangVersion}
)

include(FeatureSummary)
include(TinyFeatureOptions)

tiny_init_cmake_variables_pre()

feature_option(BUILD_SHARED_LIBS
    "Build using shared libraries" ON
)
feature_option(BUILD_TESTS
    "Build TinyORM unit tests" OFF
)
feature_option(BUILD_TREE_DEPLOY
    "Copy TinyDrivers and TinyMySql libraries to the root of the build tree" ON
)

feature_option(BUILD_DRIVERS
    "Build TinyDrivers SQL database drivers (core/common code; replaces QtSql module)" OFF
)
feature_string_option_dependent(DRIVERS_TYPE
    "Shared;Loadable;Static"
    "How to build and link against TinyDrivers SQL database drivers (supported values: \
Shared, Loadable, and Static)" Shared
    "BUILD_DRIVERS AND BUILD_SHARED_LIBS" ${TINY_DRIVERS_TYPE_FORCE}
)
feature_option_dependent(BUILD_MYSQL_DRIVER
    "Build TinyDrivers MySQL database driver" ON
    "BUILD_DRIVERS" OFF
)

feature_option_dependent(MATCH_EQUAL_EXPORTED_BUILDTREE
    "Exported package configuration from the build tree is considered to match only \
when the build type is equal" OFF
    "CMAKE_EXPORT_PACKAGE_REGISTRY AND NOT TINY_IS_MULTI_CONFIG" OFF
)

feature_option_dependent(MSVC_RUNTIME_DYNAMIC
    "Use MSVC dynamic runtime library (-MD) instead of static (-MT), also considers \
a Debug configuration (-MTd, -MDd)" ON
    "MSVC AND NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY" MSVC_RUNTIME_DYNAMIC-NOTFOUND
)

feature_option_environment(STRICT_MODE
    "Propagate strict compiler/linker options and Qt definitions using \
the ${TinyOrm_ns}::${CommonConfig_target} interface library to the user code \
(highly recommended; can also be set with the TINYORM_STRICT_MODE environment variable)"
    TINYORM_STRICT_MODE OFF
)

feature_option(VERBOSE_CONFIGURE
    "Show information about PACKAGES_FOUND and PACKAGES_NOT_FOUND in the configure \
output" OFF
)

if(NOT (MINGW AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND BUILD_SHARED_LIBS))
    feature_option_dependent(INLINE_CONSTANTS
        "Use inline constants instead of extern constants in the shared build. \
OFF is highly recommended for the shared build; is always ON for the static build" OFF
        "BUILD_SHARED_LIBS AND NOT (MSVC AND CMAKE_CXX_COMPILER_ID STREQUAL \"Clang\")" ON
    )
endif()

mark_as_advanced(MATCH_EQUAL_EXPORTED_BUILDTREE)

include(TinyMsvcParallel)
tiny_msvc_parallel("\
Enables /MP flag for parallel builds using MSVC. Specify an \
integer value to control the number of threads used (Only \
works on some older versions of Visual Studio). Setting to \
ON lets the toolchain decide how many threads to use. Set to \
OFF to disable /MP completely.")

tiny_init_cmake_variables()
tiny_init_tiny_variables()

tiny_check_unsupported_build()

add_library(${TinyOrm_target})
add_library(${TinyOrm_ns}::${TinyOrm_target} ALIAS ${TinyOrm_target})

target_optional_compile_definitions(${TinyOrm_target}
    PUBLIC
        FEATURE NAME MYSQL_PING
        DEFAULT OFF
        DESCRIPTION "Enable MySQL ping on Orm::MySqlConnection"
        ENABLED TINYORM_MYSQL_PING
)

target_optional_compile_definitions(${TinyOrm_target}
    PUBLIC
        ADVANCED FEATURE NAME DISABLE_THREAD_LOCAL
        DEFAULT OFF
        DESCRIPTION "Remove all the thread_local storage duration specifiers \
(disables multi-threading support)"
        ENABLED TINYORM_DISABLE_THREAD_LOCAL
)

target_optional_compile_definitions(${TinyOrm_target}
    PUBLIC
        ADVANCED FEATURE NAME ORM
        DEFAULT ON
        DESCRIPTION "Controls the compilation of all ORM-related source code, when this \
option is disabled, then only the query builder without ORM is compiled"
        DISABLED TINYORM_DISABLE_ORM
)

target_optional_compile_definitions(${TinyOrm_target}
    PUBLIC
        FEATURE NAME TOM
        DEFAULT ON
        DESCRIPTION "Controls the compilation of all Tom-related source code \
(command-line interface)"
        DISABLED TINYORM_DISABLE_TOM
)

feature_option_dependent(TOM_EXAMPLE
    "Build the Tom command-line application example" OFF
    "TOM" TOM_EXAMPLE-NOTFOUND
)

tiny_init_tom_database_dirs()

include(TinySources)
tinyorm_sources(${TinyOrm_target}_headers ${TinyOrm_target}_sources)

target_sources(${TinyOrm_target} PRIVATE
    ${${TinyOrm_target}_headers}
    ${${TinyOrm_target}_sources}
)

target_precompile_headers(${TinyOrm_target} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"pch.h">)

if(NOT CMAKE_DISABLE_PRECOMPILE_HEADERS)
    target_compile_definitions(${TinyOrm_target} PRIVATE TINYORM_USING_PCH)
endif()

set_target_properties(${TinyOrm_target}
    PROPERTIES
        C_VISIBILITY_PRESET "hidden"
        CXX_VISIBILITY_PRESET "hidden"
        VISIBILITY_INLINES_HIDDEN YES
        VERSION ${PROJECT_VERSION}
        SOVERSION 0
        EXPORT_NAME ${TinyOrm_ns}
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set_property(
        TARGET ${TinyOrm_target}
        PROPERTY OUTPUT_NAME "${TinyOrm_target}${PROJECT_VERSION_MAJOR}"
    )
endif()

target_include_directories(${TinyOrm_target}
    PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_compile_definitions(${TinyOrm_target}
    PUBLIC
        PROJECT_TINYORM
        $<$<NOT:$<CONFIG:Debug>>:TINYORM_NO_DEBUG>
        $<$<NOT:$<CONFIG:Debug>>:TINYORM_NO_DEBUG_SQL>
        $<$<CONFIG:Debug>:TINYORM_DEBUG>
        $<$<CONFIG:Debug>:TINYORM_DEBUG_SQL>
)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${TinyOrm_target}
        PRIVATE
            TINYORM_BUILDING_SHARED
        INTERFACE
            TINYORM_LINKING_SHARED
    )
endif()

if(TINY_EXTERN_CONSTANTS)
    target_compile_definitions(${TinyOrm_target} PUBLIC TINYORM_EXTERN_CONSTANTS)
else()
    target_compile_definitions(${TinyOrm_target} PUBLIC TINYORM_INLINE_CONSTANTS)
endif()

if(BUILD_DRIVERS)
    target_compile_definitions(${TinyOrm_target} PUBLIC TINYORM_USING_TINYDRIVERS)
else()
    target_compile_definitions(${TinyOrm_target} PUBLIC TINYORM_USING_QTSQLDRIVERS)
endif()

if(BUILD_TESTS)
    target_compile_definitions(${TinyOrm_target} PUBLIC TINYORM_TESTS_CODE)
endif()

if(TOM)
    tinytom_sources(${TomExample_target}_headers ${TomExample_target}_sources)

    target_sources(${TinyOrm_target} PRIVATE
        ${${TomExample_target}_headers}
        ${${TomExample_target}_sources}
    )
endif()

if(TOM)
    target_include_directories(${TinyOrm_target}
        PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/tom/include>"
    )
endif()

if(TOM)
    target_compile_definitions(${TinyOrm_target}
        PUBLIC
            $<$<NOT:$<CONFIG:Debug>>:TINYTOM_NO_DEBUG>
            $<$<CONFIG:Debug>:TINYTOM_DEBUG>
        PRIVATE
            TINYORM_MSVC_RUNTIME_DYNAMIC=${MSVC_RUNTIME_DYNAMIC}
            TINYTOM_CMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}
            TINYORM_STRICT_MODE=${STRICT_MODE}
    )

    if(BUILD_TESTS)
        target_compile_definitions(${TinyOrm_target} PUBLIC TINYTOM_TESTS_CODE)
    endif()
endif()

if(TOM_EXAMPLE)
    target_compile_definitions(${TinyOrm_target}
        PUBLIC
            TINYTOM_MIGRATIONS_DIR=${TOM_MIGRATIONS_DIR}
            TINYTOM_MODELS_DIR=${TOM_MODELS_DIR}
            TINYTOM_SEEDERS_DIR=${TOM_SEEDERS_DIR}
        PRIVATE
            TINYORM_TOM_EXAMPLE
    )
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    tiny_set_rc_flags("-I \"${PROJECT_SOURCE_DIR}/resources\"")
endif()

include(TinyResourceAndManifest)
tiny_resource_and_manifest(${TinyOrm_target}
    OUTPUT_DIR "${TINY_BUILD_GENDIR}/tmp/"
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS ${TinyQtComponentsRequired})
tiny_find_package(Qt${QT_VERSION_MAJOR} ${minQtVersion} CONFIG
    REQUIRED COMPONENTS ${TinyQtComponentsRequired}
)
tiny_find_package(range-v3 CONFIG REQUIRED)

include(TinyCommon)
tiny_common(${CommonConfig_target}
    NAMESPACE ${TinyOrm_ns}
    EXPORT NAME ${CommonConfig_target}
)

target_link_libraries(${TinyOrm_target}
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        range-v3::range-v3
)

if(NOT BUILD_DRIVERS)
    target_link_libraries(${TinyOrm_target} PUBLIC Qt${QT_VERSION_MAJOR}::Sql)
endif()

if(STRICT_MODE)
    target_link_libraries(${TinyOrm_target}
        PUBLIC ${TinyOrm_ns}::${CommonConfig_target}
    )
else()
    target_link_libraries(${TinyOrm_target}
        PRIVATE ${TinyOrm_ns}::${CommonConfig_target}
    )
endif()

if(MYSQL_PING)
    tiny_find_and_link_mysql(${TinyOrm_target})
endif()

if(TOM)
    tiny_find_package(tabulate CONFIG REQUIRED)
    target_link_libraries(${TinyOrm_target} PUBLIC tabulate::tabulate)
endif()

if(BUILD_DRIVERS)
    add_subdirectory(drivers)

    target_link_libraries(${TinyOrm_target}
        PUBLIC ${TinyDrivers_ns}::${TinyDrivers_target}
    )
endif()

if(BUILD_TESTS)
    enable_testing()
    find_package(Qt${QT_VERSION_MAJOR} ${minQtVersion} REQUIRED COMPONENTS Test)

    add_subdirectory(tests)
endif()

if(TOM_EXAMPLE)
    add_subdirectory(examples)
endif()

include(TinyDeployment)

tiny_generate_find_dependency_calls(tiny_find_dependency_calls)

tiny_install_tinyorm()

tiny_export_build_tree()

if(BUILD_TREE_DEPLOY)
    tiny_build_tree_deployment()
endif()

set_packages_properties()

if(VERBOSE_CONFIGURE)
    if(NOT TINY_IS_MULTI_CONFIG AND NOT TINY_BUILD_TYPE_LOWER STREQUAL "debug")
        message(STATUS "Disabled debug output and asserts")
    endif()

    feature_summary(WHAT ALL)
else()
    feature_summary(WHAT ENABLED_FEATURES DISABLED_FEATURES)
endif()
