#include "CLI/CLI.hpp"
#include "spdlog/spdlog.h"
#include <curl/curl.h>
#include <print>
#include <ranges>
#include <string>
#include <vector>

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
  const size_t totalSize = size * nmemb;
  auto *response = static_cast<std::string *>(userp);
  response->append(static_cast<char *>(contents), totalSize);

  return totalSize;
}

int main() {
  // httplib::Server svr;
  CLI::App app{"xButler sync"};

  std::string host = "0.0.0.0";
  int port = 8080;

  // app.add_option("--port", port, "Port number");

  // // Parse raw address string to add
  // std::string s = "192.168.0.1:64345";
  // std::vector<std::string> parts =
  //     s | std::views::split(':') |
  //     std::views::transform([](auto &&r) { return std::string(r.begin(), r.end()); }) |
  //     std::ranges::to<std::vector>();

  // if (parts.size() != 2) {
  //   spdlog::error("`parts` should have length of 2. Got length of {}. Address should obey format
  //   "
  //                 "`192.168.1.1:1234`",
  //                 parts.size());
  // }

  // spdlog::info("First component: {}", parts[0]);
  // spdlog::info("Second component: {}", parts[1]);

  // try {
  //   std::vector<uint8_t> octets =
  //       parts[0] | std::views::split('.') | std::views::transform([](auto &&r) {
  //         return static_cast<uint8_t>(std::stoi(std::string(r.begin(), r.end())));
  //       }) |
  //       std::ranges::to<std::vector>();

  //   spdlog::info("octets len: {}", octets.size());

  //   for (const auto octet : octets) {
  //     std::println("Octet: {}", octet);
  //   }

  // } catch (const std::invalid_argument &e) {
  //   spdlog::error("Error as parsing `address`: {}", e.what());
  //   return 1;
  // }

  // spdlog::info("Second component: {}", parts[1]);

  // svr.Get("/hi", callbackHi);

  // println("Start");
  // svr.listen("0.0.0.0", 8080);

  CURL *curl = curl_easy_init();
  std::string response = "";

  curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

  CURLcode res = curl_easy_perform(curl);
  if (curl_easy_perform(curl) != CURLE_OK) {
    spdlog::error("Error as fetching: {}", curl_easy_strerror(res));
    curl_easy_cleanup(curl);
    return 1;
  }

  spdlog::info("response: {}", response);
  curl_easy_cleanup(curl);
}