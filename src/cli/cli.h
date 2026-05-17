#pragma once

#include <string>

namespace cli {

enum class Role { Server, Client };

struct Arguments {
  Role role;
  std::string directory;
  std::string serverIp;
  uint16_t port;
};

Arguments parseArgument();
} // namespace cli