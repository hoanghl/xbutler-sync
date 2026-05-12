#include "httplib.h"
#include <print>

using namespace std;
using namespace httplib;

void callbackHi(const Request &, Response &res) {
  res.set_content("Inside somethin'", "text/plain");
}

int main() {
  Server svr;

  svr.Get("/hi", callbackHi);

  println("Start");
  svr.listen("0.0.0.0", 8080);
}