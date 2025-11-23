cmake_minimum_required(VERSION 3.18)

project(STA_OCR LANGUAGES CXX)

# 设置 C++ 标准为 20
set(CMAKE_CXX_STANDARD 20)

add_executable(STA_OCR WIN32 main.cpp
        sotool.cpp
        sotool.h
        main.h
        https.cpp
        https.h
        passwd.h)
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(STA_OCR PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(STA_OCR PRIVATE winhttp)
