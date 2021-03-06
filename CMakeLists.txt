cmake_minimum_required(VERSION 3.0)
project(soft-body-simulation)

set(CMAKE_BUILD_TYPE Debug)

set(PROJECT_NAME_LIB ${PROJECT_NAME}-lib)
set(DEPENDENCIES_DIR dependencies/)

add_subdirectory(${DEPENDENCIES_DIR}/framework)
add_subdirectory(${DEPENDENCIES_DIR}/assimp)

set(APPLICATION_RESOURCES_DIR ${PROJECT_SOURCE_DIR}/assets CACHE PATH "")

configure_file(
    ${PROJECT_SOURCE_DIR}/configuration/Config.in.hpp
    ${PROJECT_BINARY_DIR}/configuration/Config.hpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}/configuration
    ${FRAMEWORK_INCLUDE_DIRS}
    ${DEPENDENCIES_DIR}/assimp/include
)

add_library(${PROJECT_NAME_LIB}
    source/Application.cpp
    source/BezierDistortionEffect.cpp
    source/BezierPatch.cpp
    source/BezierPatchEffect.cpp
    source/ControlFrame.cpp
    source/LineSetPreview.cpp
    source/ParticleState.cpp
    source/SoftBox.cpp
    source/SoftBoxPreview.cpp
)

add_executable(${PROJECT_NAME}
    source/Main.cpp
)

target_link_libraries(${PROJECT_NAME}
    ${PROJECT_NAME_LIB}
    ${FRAMEWORK_LIBRARIES}
    assimp
)

set(PROJECT_COMPILE_FEATURES
    ${PROJECT_COMPILE_FEATURES}
    cxx_auto_type
    cxx_nullptr
    cxx_range_for
)

target_compile_features(${PROJECT_NAME} PRIVATE
    ${PROJECT_COMPILE_FEATURES}
)

target_compile_features(${PROJECT_NAME_LIB} PRIVATE
    ${PROJECT_COMPILE_FEATURES}
)

