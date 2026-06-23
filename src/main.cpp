#include "client/client.h"
#include "cli/cli.h"
#include "server/server.h"

#include <CLI/CLI.hpp>
#include <cstdlib>
#include <spdlog/spdlog.h>
#include <string>

int main(int argc, char *argv[]) {
  CLI::App app{"xbutler-sync - Cross-platform file synchronization"};

  std::string role;
  app.add_option("--role", role, "Run as 'server' or 'client'")
      ->required()
      ->transform(CLI::CheckedTransformer(
          std::map<std::string, std::string>{
              {"server", "server"},
              {"client", "client"},
          },
          CLI::ignore_case));

  std::string directory;
  app.add_option("--directory", directory, "Path to the directory to sync")->required();

  std::string host = "0.0.0.0";
  app.add_option("--host", host, "Host address to bind (default: 0.0.0.0)");

  uint16_t port = 8080;
  app.add_option("--port", port, "Port number (default: 8080)");

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
    return app.exit(e);
  }

  if (role == "server") {
    server::ServerConfig config{host, port, directory};
    server::start(config);
  } else {
    std::string serverUrl = std::format("http://{}:{}", host, port);
    client::ClientConfig config{directory, serverUrl};

    auto localFiles = client::scan(config.directory);
    spdlog::info("Scanned {} files in {}", localFiles.size(), config.directory);

    auto result = client::syncFile(config, localFiles);
    if (!result) {
      spdlog::error("Sync failed: {}", result.error());
      return 1;
    }

    auto manifest = *result;
    spdlog::info("Sync manifest: {} to upload, {} to download",
                 manifest.upload.size(), manifest.download.size());

    client::upload(config, manifest.upload);
    client::fetch(config, manifest.download);

    spdlog::info("Sync completed");
  }

  return 0;
}