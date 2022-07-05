set(project_name "CALIB-PANEL-LAB")
project(${project_name})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

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

set(Qt5_DIR "C:\\Qt5\\5.15.2\\msvc2019_64\\lib\\cmake\\Qt5" CACHE STRING "" FORCE)
set(Qt5_BIN "C:/Qt5/5.15.2/msvc2019_64/bin" CACHE STRING "" FORCE)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

file(GLOB SRC_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(${project_name} ${SRC_FILES})

# -- force not include macro min and max from native windows
if (WIN32)
	target_compile_definitions(${project_name} PUBLIC -DNOMINMAX -D_WINDOWS -DOIIO_STATIC_DEFINE=1)
endif()

target_link_libraries(${project_name} PUBLIC Qt5::Widgets)
target_link_libraries(${project_name} PUBLIC "LIB-IMAGE")
target_link_libraries(${project_name} PUBLIC "LIB-CONTEXT-ICE")
target_link_libraries(${project_name} PUBLIC "LIB-IMS")
target_link_libraries(${project_name} PUBLIC "LIB-KINET")
target_link_libraries(${project_name} PUBLIC "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0/um/x64/iphlpapi.lib")
target_link_libraries(${project_name} PUBLIC "C:/Program Files/OpenSSL-Win64/lib/VC/static/libcrypto64MT.lib")
target_link_libraries(${project_name} PUBLIC "C:/Program Files/OpenSSL-Win64/lib/VC/static/libssl64MT.lib")
target_link_libraries(${project_name} PUBLIC ws2_32)
target_link_libraries(${project_name} PUBLIC CRYPT32)

## -- add includes
target_include_directories(${project_name} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

## -- specific POCO 
target_include_directories(${project_name} PUBLIC "C:/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0/um/")


message("Set OUTPUT PATH of ICE-SERVICE in ${CMAKE_BINARY_DIR}/../BUILD")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../BUILD/BIN")
install(TARGETS ${project_name} RUNTIME DESTINATION "${CMAKE_BINARY_DIR}/../BUILD/BIN")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/images DESTINATION "${CMAKE_BINARY_DIR}/../BUILD/BIN")
install(FILES "${CMAKE_BINARY_DIR}/../BUILD/ZLIB/BIN/zlib.dll" DESTINATION "${CMAKE_BINARY_DIR}/../BUILD/BIN")
install(FILES "${Qt5_BIN}/Qt5Widgets.dll" DESTINATION "${CMAKE_BINARY_DIR}/../BUILD/BIN")
install(FILES "${Qt5_BIN}/Qt5Gui.dll" DESTINATION "${CMAKE_BINARY_DIR}/../BUILD/BIN")
install(FILES "${Qt5_BIN}/Qt5Core.dll" DESTINATION "${CMAKE_BINARY_DIR}/../BUILD/BIN")