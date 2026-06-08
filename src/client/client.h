#pragma once

#include <string>
#include <vector>

using FileList = std::vector<std::string>;
using HostName = std::string;

namespace client {

struct SyncResponse {
  FileList upload;
  FileList download;
};

FileList scan(const std::string path);

SyncResponse syncFile(const HostName host, FileList files);

void fetch(FileList files);

} // namespace client
