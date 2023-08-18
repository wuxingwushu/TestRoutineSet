#include <iostream>
#include <string>
#include <curl/curl.h>

// 回调函数处理服务器响应
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

int main() {
    std::string url = "http://fanyi.youdao.com/translate?&doctype=json&type=AUTO&to=ja&i=hello";
    std::string result;

    // 初始化 libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    CURL* curl = curl_easy_init();

    if (curl) {
        // 设置 URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // 设置回调函数和缓冲区
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

        // 设置User-Agent头字段
        std::string userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/115.0.0.0 Safari/537.36 Edg/115.0.1901.203";
        curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.c_str());

        // 发起请求
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        // 清理资源
        curl_easy_cleanup(curl);
    }

    // 打印翻译结果
    std::cout << result << std::endl;

    // 清理 libcurl
    curl_global_cleanup();

    return 0;
}
