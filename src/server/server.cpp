#include "server.h"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_set>
#include <vector>

#include <httplib.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace server {

namespace {

std::vector<std::string> scanFiles(const std::string &directory) {
  std::vector<std::string> files;

  if (!std::filesystem::is_directory(directory)) {
    return files;
  }

  for (const auto &entry : std::filesystem::directory_iterator(directory)) {
    if (!entry.is_regular_file())
      continue;

    files.push_back(entry.path().filename().string());
  }

  return files;
}

void handleSync(const httplib::Request &req, httplib::Response &res,
                const std::string &directory) {
  try {
    auto body = nlohmann::json::parse(req.body);
    auto clientFiles = body["filename"].get<std::vector<std::string>>();

    auto serverFiles = scanFiles(directory);

    std::unordered_set<std::string> clientSet(clientFiles.begin(), clientFiles.end());
    std::unordered_set<std::string> serverSet(serverFiles.begin(), serverFiles.end());

    std::vector<std::string> download;
    std::vector<std::string> upload;

    for (const auto &f : serverFiles) {
      if (!clientSet.contains(f)) {
        download.push_back(f);
      }
    }
    for (const auto &f : clientFiles) {
      if (!serverSet.contains(f)) {
        upload.push_back(f);
      }
    }

    nlohmann::json j = {{"upload", upload}, {"download", download}};
    res.set_content(j.dump(), "application/json");
  } catch (const nlohmann::json::exception &e) {
    spdlog::error("JSON parse error: {}", e.what());
    res.status = 400;
    res.set_content("Invalid JSON", "text/plain");
  }
}

void handleDownload(const httplib::Request &req, httplib::Response &res,
                    const std::string &directory) {
  std::string filename = req.matches[1];
  std::filesystem::path filePath = std::filesystem::path(directory) / filename;

  if (!std::filesystem::exists(filePath)) {
    res.status = 404;
    res.set_content("File not found", "text/plain");
    return;
  }

  std::ifstream file(filePath, std::ios::binary);
  if (!file) {
    res.status = 500;
    res.set_content("Failed to read file", "text/plain");
    return;
  }

  std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  res.set_content(content, "application/octet-stream");
}

void handleUpload(const httplib::Request &req, httplib::Response &res,
                  const std::string &directory) {
  if (!req.has_header("X-File-Name")) {
    res.status = 400;
    res.set_content("Missing X-File-Name header", "text/plain");
    return;
  }

  std::string filename = req.get_header_value("X-File-Name");
  std::filesystem::path filePath = std::filesystem::path(directory) / filename;

  std::ofstream file(filePath, std::ios::binary);
  if (!file) {
    res.status = 500;
    res.set_content("Failed to write file", "text/plain");
    return;
  }

  file.write(req.body.data(), static_cast<std::streamsize>(req.body.size()));
  res.status = 201;
}

} // namespace

void start(const ServerConfig &config) {
  httplib::Server svr;

  svr.Post("/sync", [&config](const httplib::Request &req, httplib::Response &res) {
    handleSync(req, res, config.directory);
  });

  svr.Get(R"(/files/(.*))", [&config](const httplib::Request &req, httplib::Response &res) {
    handleDownload(req, res, config.directory);
  });

  svr.Post("/files", [&config](const httplib::Request &req, httplib::Response &res) {
    handleUpload(req, res, config.directory);
  });

  spdlog::info("Server starting on {}:{}", config.host, config.port);
  svr.listen(config.host, config.port);
}

} // namespace server
