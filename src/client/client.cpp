#include "client.h"
#include "spdlog/spdlog.h"
#include <curl/curl.h>
#include <expected>
#include <filesystem>
#include <format>
#include <fstream>
#include <nlohmann/json.hpp>

namespace client {

namespace {

size_t writeCallback(void *content, size_t size, size_t nmemb, std::string *data) {
  size_t totalSize = size * nmemb;
  if (data) {
    data->append(static_cast<char *>(content), totalSize);
  }

  return totalSize;
}

std::string buildUrl(const std::string &base, const std::string &endpoint) {
  if (base.back() != '/') {
    return base + "/" + endpoint;
  }
  return base + endpoint;
}

} // namespace

void from_json(const nlohmann::json &j, SyncResponse &s) {
  j.at("upload").get_to(s.upload);
  j.at("download").get_to(s.download);
}

FileList scan(const std::string &path) {
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

std::expected<SyncResponse, std::string> syncFile(const ClientConfig &config,
                                                   const FileList &files) {
  CURL *curl = curl_easy_init();
  if (!curl) {
    return std::unexpected("Cannot initialize curl instance");
  }

  std::string responseRaw;
  std::string url = buildUrl(config.serverUrl, ENDPOINT_SYNC);
  std::string body = nlohmann::json{{"filename", files}}.dump();

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_POST, 1L);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.size());

  struct curl_slist *headers = nullptr;
  headers = curl_slist_append(headers, "Content-Type: application/json");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseRaw);

  CURLcode res = curl_easy_perform(curl);
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);

  if (res != CURLE_OK) {
    return std::unexpected(std::format("POST /sync failed: {}", curl_easy_strerror(res)));
  }

  try {
    nlohmann::json j = nlohmann::json::parse(responseRaw);
    return j.get<SyncResponse>();
  } catch (const nlohmann::json::exception &e) {
    return std::unexpected(std::format("Failed to parse sync response: {}", e.what()));
  }
}

void upload(const ClientConfig &config, const FileList &files) {
  for (const auto &filename : files) {
    std::filesystem::path filePath = std::filesystem::path(config.directory) / filename;

    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
      spdlog::error("Cannot open file for upload: {}", filename);
      continue;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    CURL *curl = curl_easy_init();
    if (!curl) {
      spdlog::error("Cannot initialize curl for upload");
      continue;
    }

    std::string url = buildUrl(config.serverUrl, "files");

    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, std::format("X-File-Name: {}", filename).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/octet-stream");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, content.size());

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      spdlog::error("Failed to upload {}: {}", filename, curl_easy_strerror(res));
    } else {
      spdlog::info("Uploaded: {}", filename);
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
  }
}

void fetch(const ClientConfig &config, const FileList &files) {
  for (const auto &filename : files) {
    CURL *curl = curl_easy_init();
    if (!curl) {
      spdlog::error("Cannot initialize curl for download");
      continue;
    }

    std::string responseRaw;
    std::string url = buildUrl(config.serverUrl, std::format("files/{}", filename));

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseRaw);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
      spdlog::error("Failed to download {}: {}", filename, curl_easy_strerror(res));
      continue;
    }

    std::filesystem::path filePath = std::filesystem::path(config.directory) / filename;
    std::ofstream file(filePath, std::ios::binary);
    if (!file) {
      spdlog::error("Cannot write file: {}", filename);
      continue;
    }

    file.write(responseRaw.data(), static_cast<std::streamsize>(responseRaw.size()));
    spdlog::info("Downloaded: {}", filename);
  }
}

} // namespace client
