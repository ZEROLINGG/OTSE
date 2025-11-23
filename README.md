# STA_OCR (OCR 翻译助手)

**STA_OCR** 是一个运行在 Windows 平台的 C++ 后台工具。它能够监听全局快捷键，自动读取剪贴板中的图片，调用百度 AI 接口进行高精度文字识别（OCR）和翻译，并将结果保存到文件后自动使用 Sublime Text 打开。

## ✨ 主要功能

- **后台静默运行**: 程序启动后隐藏在后台，通过快捷键触发，不干扰日常工作。
    
- **快捷键触发**: 双重确认机制防止误触（`Ctrl + Shift + 1` 激活，`Enter` 执行）。
    
- **剪贴板集成**: 直接从剪贴板读取图片数据，无需手动保存文件。
    
- **云端 AI 加持**:
    
    - **OCR**: 集成百度高精度 OCR API。
        
    - **翻译**: 集成百度通用翻译 API。
        
- **自动化工作流**: 识别 -> 翻译 -> 保存至桌面 -> 自动打开编辑器查看。
    

## 🛠️ 环境依赖

- **操作系统**: Windows 10/11
    
- **编译环境**:
    
    - CMake >= 3.18
        
    - 支持 C++20 的编译器 (推荐 MSVC / Visual Studio)
        
- **第三方库**:
    
    - [nlohmann/json](https://github.com/nlohmann/json "null") (用于解析 JSON 响应)
        
    - WinHTTP (Windows 系统自带)
        

## ⚙️ 配置说明

在编译之前，您需要修改源码中的配置文件以适配您的环境。

### 1. 配置 API 密钥 (`passwd.h`)

项目根目录下的 `passwd.h` 包含了百度 AI 的密钥。请使用 `passwd.h.eg` 作为模板，填入您自己的密钥：

```
// passwd.h
inline std::string aip_baidubce_com = "grant_type=client_credentials&client_id=【您的OCR API Key】&client_secret=【您的OCR Secret Key】";
inline std::string appid = "【您的翻译 APP ID】";
inline std::string secretKey = "【您的翻译密钥】";
```

> **提示**: 您需要前往 [百度智能云](https://cloud.baidu.com/ "null") 申请通用文字识别（高精度版）权限，以及 [百度翻译开放平台](https://www.google.com/search?q=https://api.fanyi.baidu.com/ "null") 申请通用翻译权限。

### 2. 配置编辑器路径 (`main.h`)

默认代码使用 Sublime Text 打开结果文件。如果您的安装路径不同，或想使用其他编辑器（如 VS Code, Notepad++），请修改 `main.h` 中的 `sublimePath` 变量：

```
// main.h
// 修改为您实际的编辑器路径
inline std::string sublimePath = "D:\\Sublime Text\\sublime_text.exe";

// 也可以修改 Access Token 的缓存路径
inline std::string access_token_txt = R"(C:\Users\您的用户名\Documents\me\access_token.txt)";
```

## 🚀 编译构建

使用 CMake 进行构建：

```
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## 📖 使用指南

1. **启动程序**: 运行编译生成的 `STA_OCR.exe`。控制台会显示 "程序启动成功"。
    
2. **复制图片**: 使用截图工具（如 Snipaste, Windows 截图）截图，或复制任意图片到剪贴板。
    
3. **触发识别**:
    
    - 按下组合键 **`Ctrl + Shift + 1`** (激活触发器，此时程序进入待命状态)。
        
    - 在 **10秒内** 按下 **`Enter`** 键确认执行。
        
4. **查看结果**:
    
    - 程序会在您的 **桌面** 创建 `tmp` 文件夹。
        
    - 图片保存为 `tmp.png`，识别与翻译结果保存为 `tmp.txt`。
        
    - 程序会自动调用配置的编辑器打开 `tmp.txt`。
        

### 快捷键说明

|按键组合|作用|
|---|---|
|`Ctrl` + `Shift` + `1`|**激活**: 准备触发，记录当前时间戳|
|`Enter`|**执行**: 仅在激活后 10 秒内有效，执行 OCR 和翻译|
|`Esc`|**取消**: 取消激活状态|

## 📂 项目结构

```
STA_OCR/
├── CMakeLists.txt      # CMake 构建脚本
├── main.cpp            # 主程序入口，键盘钩子与业务逻辑调度
├── main.h              # 全局头文件与配置
├── https.cpp / .h      # HTTP 请求封装，OCR 与翻译 API 调用逻辑
├── sotool.cpp / .h     # 工具类：MD5, Base64, 剪贴板操作, 文件操作
├── passwd.h            # API 密钥配置 (需自行修改)
└── passwd.h.eg         # 密钥配置模板
```

## ⚠️ 注意事项

- **API 配额**: 请留意您的百度 API 免费额度，超额可能会导致服务不可用。
    
- **网络连接**: 程序依赖网络访问百度 API，请确保网络通畅。
    
- **隐私安全**: 您的 API 密钥（AK/SK）存储在 `passwd.h` 中，请勿将包含真实密钥的代码提交到公开仓库。
    
