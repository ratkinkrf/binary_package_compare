cmake_minimum_required(VERSION 3.0)
project(BinaryCompare)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_library(jsonlib SHARED src/json.cpp src/json_builder.cpp)
add_library(myjsonlib ALIAS jsonlib)

target_include_directories(jsonlib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(jsonlib PUBLIC cxx_std_17)

add_executable(BinaryCompare src/main.cpp)
target_link_libraries(BinaryCompare PRIVATE myjsonlib)
target_compile_features(BinaryCompare PUBLIC cxx_std_17)
