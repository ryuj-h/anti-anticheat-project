#include <iostream>
#include <cpprest/http_listener.h>

using namespace std;
using namespace web::http;
using namespace web::http::experimental::listener;

int main() {
	http_listener listener(U("http://localhost:7890"));
	listener.open().then([&listener]() {cout << (U("\n start \n")); }).wait();
	listener.support(methods::GET, [](http_request req) {
		req.reply(status_codes::OK, U("hello world!"));
	});
	while (true);
	listener.close();
	return 0;
}