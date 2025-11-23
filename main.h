//
// Created by zerox on 2025/11/22.
//

#ifndef STA_OCR_MAIN_H
#define STA_OCR_MAIN_H

#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <algorithm>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <random>
#include <cmath>
#include <chrono>
#include <shellapi.h>
#include <thread>
#include <nlohmann/json.hpp>
#include <winhttp.h>
#include <mutex>
#include <condition_variable>
#include <shlobj.h>
#pragma comment(lib, "winhttp.lib")

#include "https.h"
#include "sotool.h"
#include "passwd.h"

inline std::string sublimePath = "D:\\Sublime Text\\sublime_text.exe";
inline std::string access_token_txt = R"(C:\Users\zerox\Documents\me\access_token.txt)";

#endif //STA_OCR_MAIN_H