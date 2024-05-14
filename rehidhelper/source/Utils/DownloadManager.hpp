#pragma once
#include <functional>
#include <vector>
#include <string>
#include <3ds.h>
#include <curl/curl.h>

namespace Utils {
class DownloadManager {
public:
    DownloadManager() = default;
    void operator=(const DownloadManager &) = delete;
    void Intialize();
    int GetUrl(const std::string &url, std::vector<uint8_t> &data, bool write_to_file = false, FILE *f = nullptr);
    void DownloadAndUnzipTo(const std::string &url, const std::string &location);
    void DownloadTo(const std::string &url, const std::string &location, const std::string &filename);
private:
    CURL *m_handle;
    std::string m_url;
    std::vector<uint8_t> m_data;
};
}