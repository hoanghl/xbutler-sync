#include <curl/curl.h>
#include <iostream>
#include <string>

// Callback function to handle the incoming data stream
size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *user_data) {
  size_t total_size = size * nmemb;
  if (user_data) {
    user_data->append(static_cast<char *>(contents), total_size);
  }
  return total_size;
}

int main() {
  CURL *curl;
  CURLcode res;
  std::string response_string;

  // Initialize libcurl globally (must be called once in your application)
  curl_global_init(CURL_GLOBAL_DEFAULT);

  // Initialize a curl easy session
  curl = curl_easy_init();

  if (curl) {
    // Set the target URL
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.github.com/zen");

    // Follow HTTP 3xx redirects automatically
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // GitHub API requires a User-Agent header; good practice for all requests
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    // Set the callback function to capture the response body
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

    // Pass the string reference to the callback function
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

    // Perform the request, res will get the return code
    res = curl_easy_perform(curl);

    // Check for errors
    if (res != CURLE_OK) {
      std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
    } else {
      // Success! Print the response data
      std::cout << "--- Response Data ---" << std::endl;
      std::cout << response_string << std::endl;
    }

    // Always cleanup after your session is done
    curl_easy_cleanup(curl);
  }

  // Globally cleanup libcurl resources
  curl_global_cleanup();

  return 0;
}