set(project_name "PROCESSOR")
project(${project_name} LANGUAGES C CXX CUDA)

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

file(GLOB SRC_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(${project_name} ${SRC_FILES})

# -- force not include macro min and max from native windows
if (WIN32)
	target_compile_definitions(${project_name} PUBLIC -DNOMINMAX -D_WINDOWS -DOIIO_STATIC_DEFINE=1)
endif()

target_link_libraries(${project_name} PUBLIC "LIB-IMAGE")
target_link_libraries(${project_name} PUBLIC "LIB-CUDA")
target_link_libraries(${project_name} PUBLIC "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0/um/x64/iphlpapi.lib")

ICE_link_library("${CMAKE_SOURCE_DIR}/EXTERN/FFMPEG" LibFFmpeg ${project_name})
ICE_link_library("C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.6" LibCUDA ${project_name})

## -- add includes
target_include_directories(${project_name} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_include_directories(${project_name} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../LIB-CUDA/include/")

## -- specific POCO 
target_include_directories(${project_name} PUBLIC "C:/Program Files (x86)/Windows Kits/8.1/Include/um/")


install(TARGETS ${project_name} RUNTIME DESTINATION "${CMAKE_BINARY_DIR}/../BUILD/R&D")
install(FILES "${CMAKE_BINARY_DIR}/../BUILD/ZLIB/BIN/zlib.dll" DESTINATION "${CMAKE_BINARY_DIR}/../BUILD/R&D")
