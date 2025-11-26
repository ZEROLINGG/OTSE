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

inline std::string GetDocumentPath() {
    PWSTR wpath = nullptr;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &wpath);
    if (FAILED(hr) || wpath == nullptr) {
        return "";
    }
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wpath, -1, nullptr, 0, nullptr, nullptr);
    if (size_needed <= 1) {
        CoTaskMemFree(wpath);
        return "";
    }
    std::string path(size_needed - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wpath, -1, path.data(), size_needed, nullptr, nullptr);
    CoTaskMemFree(wpath);
    return path;
}

inline std::string sublimePath = "D:\\Sublime Text\\sublime_text.exe";
inline std::string access_token_txt = GetDocumentPath() + "\\.staocrT";

#endif //STA_OCR_MAIN_H