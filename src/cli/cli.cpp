#include "cli.h"

#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>

namespace cli {

void parseArgument(int argc, char *argv[]) {
  // Parse CLI arguments

  CLI::App app{"xbutler-sync"};

  Role role;
  app.add_option("--role", role, "Run as server or client")
      ->required()
      ->transform(CLI::CheckedTransformer(
          std::map<std::string, Role>{
              {"server", Role::Server},
              {"client", Role::Client},
          },
          CLI::ignore_case));

  std::string directory;
  app.add_option("--directory", directory, "Path to the directory containing files")->required();

  std::string serverAddr;
  app.add_option("--server-addr", serverAddr, "IP address & port to server");
  // Try to parse input to IPv4 format: <octet1.octet2.octet3.octet3:port>
  // TODO: HoangLe [May-16]: Implement this:
//   auto parts = 


  app.parse(argc, argv);

  //   return Arguments{Role::Client};
}

} // namespace cli
