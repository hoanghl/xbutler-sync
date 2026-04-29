#include <cstdlib>
#include <iostream>
#include <string>

int main() {
  const std::string command = "\"" APP_BINARY "\"";
  const int exit_code = std::system(command.c_str());

  if (exit_code != 0) {
    std::cerr << "Expected xbutler-sync to exit successfully\n";
    return 1;
  }

  return 0;
}
