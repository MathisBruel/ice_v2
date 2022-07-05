set(project_name "LIB-CUDA")
project(${project_name} LANGUAGES C CXX CUDA)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

file(GLOB SRC_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/include/seam.cuh"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/seam.cu"
)

add_library(${project_name} SHARED ${SRC_FILES})
target_compile_features(${project_name} PUBLIC cxx_std_11)
set_property(TARGET ${project_name} PROPERTY CUDA_ARCHITECTURES 75-real 60-real)

ICE_link_library("C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.6" LibCUDA ${project_name})
set_target_properties(${project_name} PROPERTIES CUDA_RESOLVE_DEVICE_SYMBOLS ON)
set_target_properties(${project_name} PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
set_target_properties(${project_name} PROPERTIES POSITION_INDEPENDENT_CODE ON)

## -- add includes
target_include_directories(${project_name} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
install(TARGETS ${project_name} RUNTIME DESTINATION "${CMAKE_BINARY_DIR}/../BUILD/R&D")