#pragma once

#include <expected>
#include <format>
#include <string>
#include <vector>

using FileList = std::vector<std::string>;

namespace client {

constexpr std::string ENDPOINT_SYNC = "sync";

struct SyncResponse {
  FileList upload;
  FileList download;
};

struct ClientConfig {
  std::string directory;
  std::string serverUrl;
};

FileList scan(const std::string &path);

std::expected<SyncResponse, std::string> syncFile(const ClientConfig &config,
                                                   const FileList &files);

void upload(const ClientConfig &config, const FileList &files);

void fetch(const ClientConfig &config, const FileList &files);

} // namespace client
