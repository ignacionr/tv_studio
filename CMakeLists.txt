cmake_minimum_required(VERSION 3.5)
project(tv_studio VERSION 0.1.0)

include(CTest)
enable_testing()

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)

if(MSVC)
    add_compile_options("/std:c++latest")
endif()

add_executable(tv_studio src/main.cpp)

if(NOT MSVC)
    target_compile_features(tv_studio PRIVATE cxx_std_17)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

if(MSVC)
    target_link_libraries(tv_studio PRIVATE SDL2::SDL2main SDL2::SDL2-static)
else()
    target_link_libraries(tv_studio PRIVATE SDL2::SDL2)
endif()
target_link_libraries(tv_studio PRIVATE SDL2_image::SDL2_image)
target_link_libraries(tv_studio PRIVATE SDL2_ttf::SDL2_ttf)
