#include "CLI/CLI.hpp"
#include <print>
#include <ranges>
#include <string>
#include <vector>

int main() {
  // httplib::Server svr;
  CLI::App app{"xButler sync"};

  std::string host = "0.0.0.0";
  int port = 8080;

  // app.add_option("--port", port, "Port number");

  // Study about piping operation of
  std::string s = "gskfs";
  auto parts = s | std::views::split(':') | std::views::transform([]());

  for (const std::string &part : parts) {
    std::println("part: {}", part);
  }

  // svr.Get("/hi", callbackHi);

  // println("Start");
  // svr.listen("0.0.0.0", 8080);
}