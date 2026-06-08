#include <curl/curl.h>

#include "client.h"

namespace client {

size_t syncCallback(void *content, size_t size, size_t nmemb, std::string *data) {
  size_t totalSize = nmemb * size;
  if (data) {
    data->append(static_cast<std::string *>(content, totalSize));
  }
}

FileList scan(const std::string path) {
  // TODO: HoangLe [May-25]: Implement this
}

FileList syncFile(const HostName host, FileList files) {
  // Connect
}

void fetch(FileList files) {}

} // namespace client
