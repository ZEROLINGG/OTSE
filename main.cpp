#include "main.h"

// 全局变量
HHOOK hKeyboardHook = nullptr;
std::atomic<bool> isTriggerReady(false);
std::atomic<bool> isProcessing(false);
std::chrono::steady_clock::time_point triggerTime;

// 线程安全的日志输出
std::mutex g_logMutex;

constexpr int TIMEOUT_SECONDS = 10;

// 线程安全的日志函数
void SafeLog(const std::string& message) {
    std::lock_guard<std::mutex> lock(g_logMutex);
    std::cout << message << std::endl;
}


// 业务代码执行
void ExecuteDesignatedCode() {
    // 防止重复触发
    bool expected = false;
    if (!isProcessing.compare_exchange_strong(expected, true)) {
        SafeLog("任务正在执行中，忽略本次触发");
        return;
    }

    // 使用智能指针管理线程
    std::thread worker([]() {
        try {
            // 获取桌面路径
            std::string desktopPath = GetDesktopPath();
            if (desktopPath.empty()) {
                SafeLog("无法获取桌面路径");
                isProcessing = false;
                return;
            }

            // 创建tmp目录
            std::string tmpDir = desktopPath + "\\tmp";
            if (!CreateDirectoryIfNotExists(tmpDir)) {
                SafeLog("无法创建tmp目录: " + tmpDir);
                isProcessing = false;
                return;
            }

            std::string pngPath = tmpDir + "\\tmp.png";
            std::string txtPath = tmpDir + "\\tmp.txt";

            // 获取剪贴板图片
            std::vector<uint8_t> png;
            png.reserve(1024 * 1024);

            if (!GetClipboardPNG(png)) {
                SafeLog("剪贴板中无图片");
                isProcessing = false;
                return;
            }

            // 检查图片大小
            if (png.size() > 10 * 1024 * 1024) {
                SafeLog("图片过大，超过10MB限制");
                isProcessing = false;
                return;
            }

            SafeLog("开始处理图片，大小: " + std::to_string(png.size() / 1024) + " KB");

            // 保存PNG图片到桌面/tmp/tmp.png
            if (!SavePNGToFile(png, pngPath)) {
                SafeLog("保存图片失败: " + pngPath);
                isProcessing = false;
                return;
            }
            SafeLog("图片已保存: " + pngPath);

            // Base64编码
            std::string img_base64 = base64_encode(png);
            png.clear();
            png.shrink_to_fit();

            // 执行OCR
            std::string result = run_ocr(img_base64);
            img_base64.clear();
            img_base64.shrink_to_fit();

            if (result.empty()) {
                SafeLog("OCR识别结果为空");
                isProcessing = false;
                return;
            }

            SafeLog("识别结果:\n" + result);

            // 保存OCR结果到txt文件（覆盖模式）
            if (!SaveTextToFile(result, txtPath, false)) {
                SafeLog("保存OCR结果失败: " + txtPath);
                isProcessing = false;
                return;
            }
            SafeLog("OCR结果已保存: " + txtPath);

            // 执行翻译
            std::string translate_str = translate(result);
            if (!translate_str.empty()) {
                SafeLog("翻译结果:\n" + translate_str);

                // 追加翻译结果到txt文件
                std::string appendContent = "\n\n\n##############################################################################\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n##############################################################################\n\n\n" + translate_str;
                if (!SaveTextToFile(appendContent, txtPath, true)) {
                    SafeLog("追加翻译结果失败: " + txtPath);
                    isProcessing = false;
                    return;
                }
                SafeLog("翻译结果已追加: " + txtPath);
            } else {
                SafeLog("翻译失败或结果为空");
            }

            // 使用Sublime Text打开txt文件
            std::string command = "\"" + sublimePath + "\" \"" + txtPath + "\"";

            STARTUPINFOA si = { sizeof(si) };
            PROCESS_INFORMATION pi;

            if (CreateProcessA(NULL, const_cast<char*>(command.c_str()),
                              NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
                SafeLog("已使用Sublime Text打开文件");
            } else {
                SafeLog("打开Sublime Text失败，错误代码: " + std::to_string(GetLastError()));
            }

        } catch (const std::exception& e) {
            SafeLog(std::string("执行出错: ") + e.what());
        } catch (...) {
            SafeLog("执行出错: 未知异常");
        }

        isProcessing = false;
    });

    worker.detach();
}

// 键盘钩子回调
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION) {
        return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
    }

    if (wParam != WM_KEYDOWN && wParam != WM_SYSKEYDOWN) {
        return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
    }

    auto* pKeyBoard = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
    DWORD vkCode = pKeyBoard->vkCode;

    if (vkCode == '1') {
        bool ctrlDown = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        bool shiftDown = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;

        if (ctrlDown && shiftDown) {
            isTriggerReady = true;
            triggerTime = std::chrono::steady_clock::now();
        }
    }
    else if (vkCode == VK_RETURN) {
        if (isTriggerReady.load()) {
            auto now = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                now - triggerTime).count();

            if (duration <= TIMEOUT_SECONDS) {
                ExecuteDesignatedCode();
            }

            isTriggerReady = false;
        }
    }
    else if (vkCode == VK_ESCAPE) {
        if (isTriggerReady.load()) {
            isTriggerReady = false;
        }
    }

    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

// 清理函数
void Cleanup() {
    if (hKeyboardHook != nullptr) {
        UnhookWindowsHookEx(hKeyboardHook);
        hKeyboardHook = nullptr;
    }
}

// Ctrl+C 信号处理
BOOL WINAPI ConsoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT) {
        SafeLog("\n正在退出程序...");
        Cleanup();

        int waitCount = 0;
        while (isProcessing.load() && waitCount < 30) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            waitCount++;
        }

        return TRUE;
    }
    return FALSE;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    SetConsoleTitleA("OCR翻译助手 - 后台运行中");

    if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
        std::cerr << "无法设置控制台处理器" << std::endl;
    }

    SafeLog("程序启动成功");
    SafeLog("使用方法:");
    SafeLog("  1. 按 Ctrl+Shift+1 激活触发器");
    SafeLog("  2. 10秒内按 Enter 执行OCR和翻译");
    SafeLog("  3. 按 ESC 取消触发器");
    SafeLog("  4. 按 Ctrl+C 退出程序");
    SafeLog("---------------------------------------");

    hKeyboardHook = SetWindowsHookEx(
        WH_KEYBOARD_LL,
        LowLevelKeyboardProc,
        GetModuleHandle(nullptr),
        0
    );

    if (hKeyboardHook == nullptr) {
        std::cerr << "安装钩子失败! 错误代码: " << GetLastError() << std::endl;
        return 1;
    }

    SafeLog("键盘钩子安装成功，程序运行中...\n");

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Cleanup();
    SafeLog("程序正常退出");

    return 0;
}