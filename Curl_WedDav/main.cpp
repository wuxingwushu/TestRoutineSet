#include <iostream>
#include <curl/curl.h>
#include <fstream>
#include <sstream>

unsigned char ToHex(unsigned char x)
{
    return  x > 9 ? x + 55 : x + 48;
}

std::string UrlEncode(const std::string& str)
{
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (isalnum((unsigned char)str[i]) ||
            (str[i] == '-') ||
            (str[i] == '_') ||
            (str[i] == '.') ||
            (str[i] == '~'))
            strTemp += str[i];
        else if (str[i] == ' ')
            strTemp += "+";
        else
        {
            strTemp += '%';
            strTemp += ToHex((unsigned char)str[i] >> 4);
            strTemp += ToHex((unsigned char)str[i] % 16);
        }
    }
    return strTemp;
}

// 坚果云--设置--第三方应用管理
const std::string url = "https://dav.jianguoyun.com/dav/";
// 坚果云账号
const std::string username = "1779690036@qq.com";
// 第三方应用管理应用密码（该密码自动生成）
const std::string password = "a8wuf9wzfn7bycmh";

std::string userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/115.0.0.0 Safari/537.36 Edg/115.0.1901.203";


// 获取资源文件
void getResource(const std::string& path) {
    // 初始化libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    CURL* curl = curl_easy_init();

    if (curl) {
        // 设置请求的URL
        std::string fullUrl = url + path;
        std::cout << fullUrl << std::endl;
        curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());

        // 设置Basic认证
        //std::string auth = username + ":" + password;
        //curl_easy_setopt(curl, CURLOPT_USERPWD, auth.c_str());

        // 设置用户名和密码
        curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());

        // 设置User-Agent头字段
        curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.c_str());

        // 执行请求
        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            // 请求成功
            std::cout << "脚本获取成功" << std::endl;

            // 获取响应内容
            std::ostringstream response;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response);

            // TODO: 处理响应内容
            std::cout << response.str() << std::endl;
        }
        else {
            // 请求失败
            std::cerr << "脚本获取失败！这可能是您的网络原因造成的，建议您检查网络后再重新运行软件吧\nHTTP状态码:" << res << std::endl;
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // 清理libcurl资源
        curl_easy_cleanup(curl);
    }

    // 全局清理
    curl_global_cleanup();
}

// 删除文件
void deleteFile(const std::string& path) {
    // 初始化libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    CURL* curl = curl_easy_init();

    if (curl) {
        // 设置请求的URL
        std::string fullUrl = url + path;
        std::cout << fullUrl << std::endl;
        curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());

        // 设置请求方法为DELETE
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

        // 设置Basic认证
        std::string auth = username + ":" + password;
        curl_easy_setopt(curl, CURLOPT_USERPWD, auth.c_str());

        // 执行请求
        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            // 请求成功
            std::cout << "脚本删除成功" << std::endl;
        }
        else {
            // 请求失败
            std::cerr << "脚本删除失败！HTTP状态码:" << res << std::endl;
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // 清理libcurl资源
        curl_easy_cleanup(curl);
    }

    // 全局清理
    curl_global_cleanup();
}

// 上传文件
void uploadFile(const std::string& path, const std::string& content) {
    // 初始化libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    CURL* curl = curl_easy_init();

    if (curl) {
        // 设置请求的URL
        std::string fullUrl = url + path;
        curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());

        // 设置请求方法为PUT
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        // 设置Basic认证
        std::string auth = username + ":" + password;
        curl_easy_setopt(curl, CURLOPT_USERPWD, auth.c_str());

        // 设置用户名和密码
        //curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
        //curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());

        curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.c_str());

        // 设置请求内容
        curl_easy_setopt(curl, CURLOPT_READDATA, content.c_str());
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, static_cast<curl_off_t>(content.size()));

        // 执行请求
        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            // 请求成功
            std::cout << "文件上传成功" << std::endl;
        }
        else {
            // 请求失败
            std::cerr << "文件上传失败！HTTP状态码:" << res << std::endl;
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // 清理libcurl资源
        curl_easy_cleanup(curl);
    }

    // 全局清理
    curl_global_cleanup();
}

/* Curl 不知道 支不支持 WedDAV */

int main() {
    std::string path = "cs/Writer.txt";

    // 上传文件
    std::string content = "This is the file content.";
    uploadFile(path, content);

    // 获取资源文件
    getResource(path);

    // 删除文件
    //deleteFile(path);

    return 0;
}
