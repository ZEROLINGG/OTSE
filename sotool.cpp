//
// Created by zerox on 2025/11/22.
//

#include "sotool.h"


class MD5 {
    uint32_t state[4] = {};  // ABCD状态
    uint32_t count[2] = {};  // 位计数
    uint8_t buffer[64] = {}; // 输入缓冲区

    // 辅助函数
    static inline uint32_t F(uint32_t x, uint32_t y, uint32_t z) {
        return (x & y) | (~x & z);
    }

    static inline uint32_t G(uint32_t x, uint32_t y, uint32_t z) {
        return (x & z) | (y & ~z);
    }

    static inline uint32_t H(uint32_t x, uint32_t y, uint32_t z) {
        return x ^ y ^ z;
    }

    static inline uint32_t I(uint32_t x, uint32_t y, uint32_t z) {
        return y ^ (x | ~z);
    }

    static inline uint32_t rotateLeft(uint32_t x, uint32_t n) {
        return (x << n) | (x >> (32 - n));
    }

    static inline void FF(uint32_t &a, uint32_t b, uint32_t c, uint32_t d,
                         uint32_t x, uint32_t s, uint32_t ac) {
        a = rotateLeft(a + F(b, c, d) + x + ac, s) + b;
    }

    static inline void GG(uint32_t &a, uint32_t b, uint32_t c, uint32_t d,
                         uint32_t x, uint32_t s, uint32_t ac) {
        a = rotateLeft(a + G(b, c, d) + x + ac, s) + b;
    }

    static inline void HH(uint32_t &a, uint32_t b, uint32_t c, uint32_t d,
                         uint32_t x, uint32_t s, uint32_t ac) {
        a = rotateLeft(a + H(b, c, d) + x + ac, s) + b;
    }

    static inline void II(uint32_t &a, uint32_t b, uint32_t c, uint32_t d,
                         uint32_t x, uint32_t s, uint32_t ac) {
        a = rotateLeft(a + I(b, c, d) + x + ac, s) + b;
    }

    void transform(const uint8_t block[64]) {
        uint32_t a = state[0], b = state[1], c = state[2], d = state[3];
        uint32_t x[16];

        // 字节转换为32位整数（小端序）
        for (int i = 0, j = 0; i < 16; ++i, j += 4) {
            x[i] = ((uint32_t)block[j]) | (((uint32_t)block[j+1]) << 8) |
                   (((uint32_t)block[j+2]) << 16) | (((uint32_t)block[j+3]) << 24);
        }

        // 第1轮
        FF(a, b, c, d, x[ 0],  7, 0xd76aa478); FF(d, a, b, c, x[ 1], 12, 0xe8c7b756);
        FF(c, d, a, b, x[ 2], 17, 0x242070db); FF(b, c, d, a, x[ 3], 22, 0xc1bdceee);
        FF(a, b, c, d, x[ 4],  7, 0xf57c0faf); FF(d, a, b, c, x[ 5], 12, 0x4787c62a);
        FF(c, d, a, b, x[ 6], 17, 0xa8304613); FF(b, c, d, a, x[ 7], 22, 0xfd469501);
        FF(a, b, c, d, x[ 8],  7, 0x698098d8); FF(d, a, b, c, x[ 9], 12, 0x8b44f7af);
        FF(c, d, a, b, x[10], 17, 0xffff5bb1); FF(b, c, d, a, x[11], 22, 0x895cd7be);
        FF(a, b, c, d, x[12],  7, 0x6b901122); FF(d, a, b, c, x[13], 12, 0xfd987193);
        FF(c, d, a, b, x[14], 17, 0xa679438e); FF(b, c, d, a, x[15], 22, 0x49b40821);

        // 第2轮
        GG(a, b, c, d, x[ 1],  5, 0xf61e2562); GG(d, a, b, c, x[ 6],  9, 0xc040b340);
        GG(c, d, a, b, x[11], 14, 0x265e5a51); GG(b, c, d, a, x[ 0], 20, 0xe9b6c7aa);
        GG(a, b, c, d, x[ 5],  5, 0xd62f105d); GG(d, a, b, c, x[10],  9, 0x02441453);
        GG(c, d, a, b, x[15], 14, 0xd8a1e681); GG(b, c, d, a, x[ 4], 20, 0xe7d3fbc8);
        GG(a, b, c, d, x[ 9],  5, 0x21e1cde6); GG(d, a, b, c, x[14],  9, 0xc33707d6);
        GG(c, d, a, b, x[ 3], 14, 0xf4d50d87); GG(b, c, d, a, x[ 8], 20, 0x455a14ed);
        GG(a, b, c, d, x[13],  5, 0xa9e3e905); GG(d, a, b, c, x[ 2],  9, 0xfcefa3f8);
        GG(c, d, a, b, x[ 7], 14, 0x676f02d9); GG(b, c, d, a, x[12], 20, 0x8d2a4c8a);

        // 第3轮
        HH(a, b, c, d, x[ 5],  4, 0xfffa3942); HH(d, a, b, c, x[ 8], 11, 0x8771f681);
        HH(c, d, a, b, x[11], 16, 0x6d9d6122); HH(b, c, d, a, x[14], 23, 0xfde5380c);
        HH(a, b, c, d, x[ 1],  4, 0xa4beea44); HH(d, a, b, c, x[ 4], 11, 0x4bdecfa9);
        HH(c, d, a, b, x[ 7], 16, 0xf6bb4b60); HH(b, c, d, a, x[10], 23, 0xbebfbc70);
        HH(a, b, c, d, x[13],  4, 0x289b7ec6); HH(d, a, b, c, x[ 0], 11, 0xeaa127fa);
        HH(c, d, a, b, x[ 3], 16, 0xd4ef3085); HH(b, c, d, a, x[ 6], 23, 0x04881d05);
        HH(a, b, c, d, x[ 9],  4, 0xd9d4d039); HH(d, a, b, c, x[12], 11, 0xe6db99e5);
        HH(c, d, a, b, x[15], 16, 0x1fa27cf8); HH(b, c, d, a, x[ 2], 23, 0xc4ac5665);

        // 第4轮
        II(a, b, c, d, x[ 0],  6, 0xf4292244); II(d, a, b, c, x[ 7], 10, 0x432aff97);
        II(c, d, a, b, x[14], 15, 0xab9423a7); II(b, c, d, a, x[ 5], 21, 0xfc93a039);
        II(a, b, c, d, x[12],  6, 0x655b59c3); II(d, a, b, c, x[ 3], 10, 0x8f0ccc92);
        II(c, d, a, b, x[10], 15, 0xffeff47d); II(b, c, d, a, x[ 1], 21, 0x85845dd1);
        II(a, b, c, d, x[ 8],  6, 0x6fa87e4f); II(d, a, b, c, x[15], 10, 0xfe2ce6e0);
        II(c, d, a, b, x[ 6], 15, 0xa3014314); II(b, c, d, a, x[13], 21, 0x4e0811a1);
        II(a, b, c, d, x[ 4],  6, 0xf7537e82); II(d, a, b, c, x[11], 10, 0xbd3af235);
        II(c, d, a, b, x[ 2], 15, 0x2ad7d2bb); II(b, c, d, a, x[ 9], 21, 0xeb86d391);

        state[0] += a; state[1] += b; state[2] += c; state[3] += d;
    }

    void init() {
        count[0] = count[1] = 0;
        state[0] = 0x67452301;
        state[1] = 0xefcdab89;
        state[2] = 0x98badcfe;
        state[3] = 0x10325476;
    }

    void update(const uint8_t* input, size_t length) {
        size_t index = count[0] / 8 % 64;

        if ((count[0] += (length << 3)) < (length << 3))
            count[1]++;
        count[1] += (length >> 29);

        size_t firstpart = 64 - index;
        size_t i = 0;

        if (length >= firstpart) {
            memcpy(&buffer[index], input, firstpart);
            transform(buffer);
            for (i = firstpart; i + 64 <= length; i += 64)
                transform(&input[i]);
            index = 0;
        }

        memcpy(&buffer[index], &input[i], length - i);
    }

    void finalize(uint8_t digest[16]) {
        static uint8_t padding[64] = { 0x80 };

        uint8_t bits[8];
        for (int i = 0; i < 8; ++i)
            bits[i] = (uint8_t)(count[i >> 2] >> ((i % 4) * 8));

        size_t index = count[0] / 8 % 64;
        size_t padLen = (index < 56) ? (56 - index) : (120 - index);
        update(padding, padLen);
        update(bits, 8);

        for (int i = 0; i < 16; ++i)
            digest[i] = (uint8_t)(state[i >> 2] >> ((i % 4) * 8));
    }

public:
    std::string compute(const std::string& str) {
        init();
        update((const uint8_t*)str.c_str(), str.length());

        uint8_t digest[16];
        finalize(digest);

        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for (const unsigned char i : digest)
            oss << std::setw(2) << static_cast<int>(i);
        return oss.str();
    }
};

std::string md5(const std::string& str) {
    MD5 md5_obj;
    return md5_obj.compute(str);
}


// Base64编码函数
std::string base64_encode(const std::vector<uint8_t>& data) {
    static auto base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    std::string ret;
    int i = 0;
    int j = 0;
    uint8_t char_array_3[3];
    uint8_t char_array_4[4];
    size_t in_len = data.size();
    const uint8_t* bytes_to_encode = data.data();

    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for(i = 0; i < 4; i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i) {
        for(j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

        for (j = 0; j < i + 1; j++)
            ret += base64_chars[char_array_4[j]];

        while(i++ < 3)
            ret += '=';
    }

    return ret;
}


// URL编码
std::string url_encode(const std::string& str) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (char c : str) {
        // 保留未编码的字符: A-Z a-z 0-9 - _ . ~
        if (std::isalnum(static_cast<unsigned char>(c)) ||
            c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            }
        // 空格编码为 +
        else if (c == ' ') {
            escaped << '+';
        }
        // 其他字符进行百分号编码
        else {
            escaped << std::uppercase;
            escaped << '%' << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
            escaped << std::nouppercase;
        }
    }

    return escaped.str();
}
// 生成随机数（32768到65536之间）
int generate_random_salt() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(32768, 65536);
    return dis(gen);
}

// UTF-8 string 转 wstring
std::wstring StringToWString(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

// wstring 转 UTF-8 string
std::string WStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// 检查文件是否存在
bool file_exists(const std::string& filename) {
    std::ifstream f(filename);
    return f.good();
}

// 获取剪贴板图片
bool GetClipboardPNG(std::vector<uint8_t>& pngData)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    static UINT cfPNG = RegisterClipboardFormatW(L"image/png");

    if (!IsClipboardFormatAvailable(cfPNG))
        return false;

    if (!OpenClipboard(nullptr))
        return false;

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    HANDLE hData = GetClipboardData(cfPNG);
    if (!hData) {
        CloseClipboard();
        return false;
    }

    const void* p = GlobalLock(hData);
    if (!p) {
        CloseClipboard();
        return false;
    }

    const SIZE_T size = GlobalSize(hData);
    pngData.resize(size);
    memcpy(pngData.data(), p, size);

    GlobalUnlock(hData);

    EmptyClipboard();
    CloseClipboard();
    return true;
}

// 获取桌面路径
std::string GetDesktopPath() {
    char path[MAX_PATH];
    if (SHGetFolderPathA(nullptr, CSIDL_DESKTOPDIRECTORY, nullptr, 0, path) == S_OK) {
        return std::string(path);
    }
    return "";
}

// 创建目录（如果不存在）
bool CreateDirectoryIfNotExists(const std::string& path) {
    DWORD attr = GetFileAttributesA(path.c_str());
    if (attr == INVALID_FILE_ATTRIBUTES) {
        return CreateDirectoryA(path.c_str(), nullptr) != 0;
    }
    return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

// 保存PNG图片到文件
bool SavePNGToFile(const std::vector<uint8_t>& png, const std::string& filepath) {
    try {
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        file.write(reinterpret_cast<const char*>(png.data()), png.size());
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

// 保存文本到文件（覆盖模式）
bool SaveTextToFile(const std::string& text, const std::string& filepath, bool append) {
    try {
        std::ofstream file;
        if (append) {
            file.open(filepath, std::ios::app);
        } else {
            file.open(filepath, std::ios::trunc);
        }

        if (!file.is_open()) {
            return false;
        }
        file << text;
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}