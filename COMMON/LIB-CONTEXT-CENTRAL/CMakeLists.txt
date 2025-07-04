cmake_minimum_required(VERSION 3.10)

message("Build LIB-CONTEXT-CENTRAL")
set(project_name "LIB-CONTEXT-CENTRAL")
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
  "${CMAKE_CURRENT_SOURCE_DIR}/include/Data/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/ContentOpsApp/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/ContentOpsBoundary/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/ContentOpsBoundary/Interactions/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/ContentOpsDomain/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/ContentOpsInfra/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/Data/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/ContentOpsDomain/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/ContentOpsInfra/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/ContentOpsApp/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/ContentOpsBoundary/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/ContentOpsBoundary/interactions/*.cpp"
)

add_library(${project_name} STATIC ${SRC_FILES})

target_link_libraries(${project_name} PUBLIC "LIB-IMS")
target_link_libraries(${project_name} PUBLIC "LIB-REPOSITORY")
target_link_libraries(${project_name} PUBLIC "LIB-SYNCHRO")
target_link_libraries(${project_name} PUBLIC "LIB-CMD")
target_link_libraries(${project_name} PUBLIC "LIB-MYSQL")

# -- force not include macro min and max from native windows
if (WIN32)
	target_compile_definitions(${project_name} PUBLIC -DNOMINMAX -D_WINDOWS -DOIIO_STATIC_DEFINE=1)
endif()

if (UNIX)
	target_link_libraries(${project_name} PUBLIC pthread)
	target_link_libraries(${project_name} PUBLIC crypto)
	target_link_libraries(${project_name} PUBLIC ssl)
	target_link_libraries(${project_name} PUBLIC mysqlclient)
endif()

## -- add includes
target_include_directories(${project_name} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
