#pragma once

#include <cstdint>
#include <string>

namespace server {

struct ServerConfig {
  std::string host;
  uint16_t port;
  std::string directory;
};

void start(const ServerConfig& config);

} // namespace server
