//
// Created by zerox on 2025/11/22.
//

#ifndef STA_OCR_MD5_H
#define STA_OCR_MD5_H
#include "main.h"

std::string md5(const std::string& str);
std::string base64_encode(const std::vector<uint8_t>& data);
std::string url_encode(const std::string& str);
int generate_random_salt();
std::wstring StringToWString(const std::string& str);
std::string WStringToString(const std::wstring& wstr);

bool file_exists(const std::string& filename);
bool GetClipboardPNG(std::vector<uint8_t>& pngData);
std::string GetDesktopPath();
bool CreateDirectoryIfNotExists(const std::string& path);
bool SavePNGToFile(const std::vector<uint8_t>& png, const std::string& filepath);
bool SaveTextToFile(const std::string& text, const std::string& filepath, bool append = false);

#endif //STA_OCR_MD5_H