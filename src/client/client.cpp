#include "client.h"
#include "spdlog/spdlog.h"
#include <curl/curl.h>
#include <expected>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace client {

// size_t syncCallback(void *content, size_t size, size_t nmemb, std::string *data) {
//   size_t totalSize = nmemb * size;
//   if (data) {
//     data->append(static_cast<std::string *>(content, totalSize));
//   }
// }

/// @brief Scan for every files in given directory. Ignore recursive scanning.
/// @param path
/// @return List of found file names
FileList scan(const std::string path) {
  FileList files;

  if (std::filesystem::is_directory(path)) {
    for (const auto &entry : std::filesystem::directory_iterator(path)) {
      if (!entry.is_regular_file())
        continue;

      files.push_back(entry.path().filename().string());
    }
  }

  return files;
}

std::string _getSyncBody(const FileList &files) {
  nlohmann::json j = nlohmann::json{{"filename", files}};

  return j.dump();
}

/// @brief Connect to hostname to fetch
/// @param host
/// @param files
/// @return
std::expected<SyncResponse, std::string>
syncFile(const HostName host, FileList files,
         const std::string urlHost = "http://localhost:8080/sync") {
  // Connect
  CURL *curl = curl_easy_init();
  if (!curl) {
    spdlog::error("Cannot initialize the instance");
    return std::unexpected("Cannot initialize the instance");
  }

  curl_easy_setopt(curl, CURLOPT_URL, urlHost.c_str());
  curl_easy_setopt(curl, CURLOPT_POST, 1L);

  struct curl_slist *headers = nullptr;
  headers = curl_slist_append(headers, "Content-Type: application/json");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  std::string body = _getSyncBody(files);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.size());

  // Set callback for response handling
  curl_easy_setopt(curl, )
}

void fetch(FileList files) {}

} // namespace client
