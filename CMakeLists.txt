cmake_minimum_required(VERSION 3.18)

project(OTSE LANGUAGES CXX)

# 设置 C++ 标准为 20
set(CMAKE_CXX_STANDARD 20)

add_executable(OTSE WIN32 main.cpp
        sotool.cpp
        sotool.h
        main.h
        https.cpp
        https.h
        passwd.h)
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(OTSE PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(OTSE PRIVATE winhttp)
