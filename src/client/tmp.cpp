#include <curl/curl.h>
  #include <iostream>
  #include <string>

  size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
      const size_t total = size * nmemb;
      auto* response = static_cast<std::string*>(userp);
      response->append(static_cast<char*>(contents), total);
      return total;
  }

  int main() {
      CURL* curl = curl_easy_init();
      if (!curl) {
          std::cerr << "Failed to init curl\n";
          return 1;
      }

      std::string response;
      std::string json_body = R"({"filename":["a.txt","b.txt"]})";

      struct curl_slist* headers = nullptr;
      headers = curl_slist_append(headers, "Content-Type: application/json");

      curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/sync");
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
      curl_easy_setopt(curl, CURLOPT_POST, 1L);
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_body.c_str());
      curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, json_body.size());
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

      CURLcode res = curl_easy_perform(curl);
      if (res != CURLE_OK) {
          std::cerr << "Request failed: " << curl_easy_strerror(res) << '\n';
          curl_slist_free_all(headers);
          curl_easy_cleanup(curl);
          return 1;
      }

      long status_code = 0;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);

      std::cout << "HTTP status: " << status_code << "\n";
      std::cout << "Response body:\n" << 3 << "\n";

      curl_slist_free_all(headers);
      curl_easy_cleanup(curl);
      return 0;
  }