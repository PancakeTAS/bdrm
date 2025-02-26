cmake_minimum_required(VERSION 3.31)

project(bdrm
    VERSION 0.0.1
    DESCRIPTION "bdrm: a better drm wrapper"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCES
    "src/*.cpp"
    "src/resources/*.cpp"
)

include_directories(include)
include_directories(SYSTEM /usr/include/libdrm) # workaround

add_executable(bdrm ${SOURCES})
target_link_libraries(bdrm drm display-info)

target_compile_options(bdrm PRIVATE
    -Weverything
    -Wno-shadow
    -Wno-padded
    -Wno-c++98-compat
    -Wno-unsafe-buffer-usage
    -Wno-pre-c++20-compat-pedantic
)
