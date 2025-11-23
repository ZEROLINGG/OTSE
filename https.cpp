//
// Created by zerox on 2025/11/23.
//

#include "https.h"

using json = nlohmann::json;

// 解析OCR结果
std::string parse_result(const json& words_result, int line_threshold = 5) {
    try {
        std::vector<json> items;
        for (const auto& item : words_result) {
            items.push_back(item);
        }

        // 按top排序
        std::sort(items.begin(), items.end(), [](const json& a, const json& b) {
            return a["location"]["top"].get<int>() < b["location"]["top"].get<int>();
        });

        std::vector<std::vector<json>> lines;
        std::vector<json> current_line;
        int last_top = -1;
        bool first = true;

        for (const auto& item : items) {
            int top = item["location"]["top"].get<int>();

            if (first) {
                current_line.push_back(item);
                last_top = top;
                first = false;
                continue;
            }

            // 判断是否为同一行
            if (std::abs(top - last_top) <= line_threshold) {
                current_line.push_back(item);
            } else {
                lines.push_back(current_line);
                current_line.clear();
                current_line.push_back(item);
                last_top = top;
            }
        }

        // 最后一行加入结果
        if (!current_line.empty()) {
            lines.push_back(current_line);
        }

        // 对每一行内部按left排序并拼接
        std::vector<std::string> merged_lines;
        for (auto& line : lines) {
            std::sort(line.begin(), line.end(), [](const json& a, const json& b) {
                return a["location"]["left"].get<int>() < b["location"]["left"].get<int>();
            });

            std::string merged_text;
            for (size_t i = 0; i < line.size(); i++) {
                if (i > 0) merged_text += " ";
                merged_text += line[i]["words"].get<std::string>();
            }
            merged_lines.push_back(merged_text);
        }

        // 用换行符连接所有行
        std::string result;
        for (size_t i = 0; i < merged_lines.size(); i++) {
            if (i > 0) result += "\n";
            result += merged_lines[i];
        }

        return result;
    } catch (const std::exception& e) {
        std::cerr << "parse_result 错误: " << e.what() << std::endl;
        return "";
    }
}

std::string SendHttpsRequest(const std::string& host, const std::string& path, const std::string& method,
    const std::string& headers = "", const std::string& body = "") {

    std::string response_data;
    HINTERNET hSession = nullptr, hConnect = nullptr, hRequest = nullptr;

    // 1. 初始化 WinHTTP 会话
    hSession = WinHttpOpen(L"OCR_Client/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);

    if (!hSession) {
        std::cerr << "WinHttpOpen 失败" << std::endl;
        return "";
    }

    // 2. 连接服务器
    std::wstring wHost = StringToWString(host);
    hConnect = WinHttpConnect(hSession, wHost.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);

    if (!hConnect) {
        std::cerr << "WinHttpConnect 失败: " << host << std::endl;
        WinHttpCloseHandle(hSession);
        return "";
    }

    // 3. 创建请求
    std::wstring wPath = StringToWString(path);
    std::wstring wMethod = StringToWString(method);

    hRequest = WinHttpOpenRequest(hConnect, wMethod.c_str(), wPath.c_str(),
        nullptr, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);

    if (!hRequest) {
        std::cerr << "WinHttpOpenRequest 失败" << std::endl;
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    // 4. 发送请求
    std::wstring wHeaders = StringToWString(headers);
    LPCWSTR pwszHeaders = wHeaders.empty() ? WINHTTP_NO_ADDITIONAL_HEADERS : wHeaders.c_str();
    DWORD dwHeadersLen = wHeaders.empty() ? 0 : (DWORD)wHeaders.length();
    auto dwTotalSize = (DWORD)body.size();

    BOOL bResults = WinHttpSendRequest(hRequest,
        pwszHeaders, dwHeadersLen,
        (LPVOID)body.c_str(), dwTotalSize,
        dwTotalSize, 0);

    if (!bResults) {
        std::cerr << "WinHttpSendRequest 失败" << std::endl;
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    // 5. 接收响应
    bResults = WinHttpReceiveResponse(hRequest, nullptr);
    if (!bResults) {
        std::cerr << "WinHttpReceiveResponse 失败" << std::endl;
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    // 6. 读取数据
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    do {
        dwSize = 0;
        if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
            std::cerr << "WinHttpQueryDataAvailable 失败" << std::endl;
            break;
        }
        if (dwSize == 0) break;

        std::vector<char> pszOutBuffer(dwSize + 1);
        if (WinHttpReadData(hRequest, (LPVOID)pszOutBuffer.data(), dwSize, &dwDownloaded)) {
            response_data.append(pszOutBuffer.data(), dwDownloaded);
        } else {
            std::cerr << "WinHttpReadData 失败" << std::endl;
            break;
        }
    } while (dwSize > 0);

    // 清理句柄
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return response_data;
}

// 获取access token
std::string get_token(bool flag = false) {
    auto start_time = std::chrono::high_resolution_clock::now();
    std::string file_path = access_token_txt;

    try {
        if (flag || !file_exists(file_path)) {
            std::string host = "aip.baidubce.com";
            std::string path = "/oauth/2.0/token?" + aip_baidubce_com;

            std::string response_string = SendHttpsRequest(host, path, "GET");

            auto end_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end_time - start_time;
            std::cout << "get_access_token 耗时: " << elapsed.count() << " 秒" << std::endl;

            if (response_string.empty()) {
                std::cerr << "获取Token响应为空" << std::endl;
                return "";
            }

            json j = json::parse(response_string);
            if (!j.contains("access_token")) {
                std::cerr << "获取Token失败: " << response_string << std::endl;
                return "";
            }

            std::string access_token = j["access_token"].get<std::string>();

            std::ofstream outfile(file_path);
            if (!outfile) {
                std::cerr << "无法写入Token文件: " << file_path << std::endl;
                return access_token; // 即使文件写入失败，也返回token
            }
            outfile << access_token;
            outfile.close();

            return access_token;
        }

        // 从文件读取token
        std::ifstream infile(file_path);
        if (!infile) {
            std::cerr << "无法打开Token文件: " << file_path << std::endl;
            return "";
        }

        std::string token;
        std::getline(infile, token);
        infile.close();

        // 去除空白字符
        if (!token.empty()) {
            size_t last = token.find_last_not_of(" \n\r\t");
            if (last != std::string::npos) token.erase(last + 1);
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end_time - start_time;
        std::cout << "get_access_token 耗时: " << elapsed.count() << " 秒" << std::endl;

        return token;

    } catch (const json::exception& e) {
        std::cerr << "get_token JSON解析错误: " << e.what() << std::endl;
        return "";
    } catch (const std::exception& e) {
        std::cerr << "get_token 错误: " << e.what() << std::endl;
        return "";
    }
}

// 执行OCR
std::string run_ocr(const std::string& img_base64) {
    try {
        std::string access_token = get_token();
        if (access_token.empty()) {
            std::cerr << "获取access_token失败" << std::endl;
            return "";
        }

        auto start_time = std::chrono::high_resolution_clock::now();

        std::string host = "aip.baidubce.com";
        std::string path = "/rest/2.0/ocr/v1/accurate?access_token=" + access_token;
        std::string post_data = "image=" + url_encode(img_base64);
        std::string headers = "Content-Type: application/x-www-form-urlencoded";

        std::string response_string = SendHttpsRequest(host, path, "POST", headers, post_data);

        if (response_string.empty()) {
            std::cerr << "OCR响应为空" << std::endl;
            return "";
        }

        json data = json::parse(response_string);

        // 检查是否有错误 (Token过期等)
        if (data.contains("error_code") && !data["error_code"].is_null()) {
            std::cout << "OCR Error: " << data["error_code"] << ", Retrying with new token..." << std::endl;

            // 强制刷新 Token
            access_token = get_token(true);
            if (access_token.empty()) {
                std::cerr << "刷新access_token失败" << std::endl;
                return "";
            }

            // 重试请求
            path = "/rest/2.0/ocr/v1/accurate?access_token=" + access_token;
            response_string = SendHttpsRequest(host, path, "POST", headers, post_data);

            if (response_string.empty()) {
                std::cerr << "OCR重试响应为空" << std::endl;
                return "";
            }

            data = json::parse(response_string);
        }

        if (!data.contains("words_result")) {
            std::cerr << "OCR 响应无 words_result: " << response_string << std::endl;
            return "";
        }

        std::string text = parse_result(data["words_result"]);
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end_time - start_time;
        std::cout << "OCR 耗时: " << elapsed.count() << " 秒" << std::endl;
        return text;

    } catch (const json::exception& e) {
        std::cerr << "run_ocr JSON解析错误: " << e.what() << std::endl;
        return "";
    } catch (const std::exception& e) {
        std::cerr << "run_ocr 错误: " << e.what() << std::endl;
        return "";
    }
}

// 翻译函数
std::string translate(const std::string& query) {
    if (query.empty()) {
        return "";
    }

    try {
        auto start_time = std::chrono::high_resolution_clock::now();
        std::string salt = std::to_string(generate_random_salt());

        // 生成签名
        std::string sign_str = appid + query + salt + secretKey;
        std::string sign = md5(sign_str);

        // 构建URL路径参数
        std::string host = "fanyi-api.baidu.com";
        std::string path = "/api/trans/vip/translate?";
        path += "q=" + url_encode(query);
        path += "&from=auto";
        path += "&to=zh";
        path += "&appid=" + appid;
        path += "&salt=" + salt;
        path += "&sign=" + sign;

        // 发送 HTTPS GET 请求
        std::string response_string = SendHttpsRequest(host, path, "GET");

        if (response_string.empty()) {
            std::cerr << "翻译响应为空" << std::endl;
            return "";
        }

        // 解析JSON响应
        json response = json::parse(response_string);

        // 检查是否有错误
        if (response.contains("error_code")) {
            if (response["error_code"].is_string() && response["error_code"] != "52000") {
                std::cerr << "翻译错误: " << response["error_msg"] << std::endl;
                return "";
            }
            if (response["error_code"].is_number() && response["error_code"].get<int>() != 52000) {
                std::cerr << "翻译错误: " << response["error_msg"] << std::endl;
                return "";
            }
        }

        // 提取翻译结果
        if (!response.contains("trans_result")) {
            std::cerr << "翻译响应无 trans_result" << std::endl;
            return "";
        }

        std::string result;
        for (const auto& item : response["trans_result"]) {
            if (!result.empty()) {
                result += "\n";
            }
            result += item["dst"].get<std::string>();
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end_time - start_time;
        std::cout << "translate 耗时: " << elapsed.count() << " 秒" << std::endl;
        return result;

    } catch (const json::exception& e) {
        std::cerr << "translate JSON解析错误: " << e.what() << std::endl;
        return "";
    } catch (const std::exception& e) {
        std::cerr << "translate 错误: " << e.what() << std::endl;
        return "";
    }
}