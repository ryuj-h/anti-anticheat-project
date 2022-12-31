#include <iostream>
#include <cpprest/http_listener.h>

#include "Overlay.h"

using namespace std;
using namespace web::http;
using namespace web::http::experimental::listener;

/*
* this program runs 2nd PC
*/


Overlay* overlay;

int main() {
    overlay = new Overlay();
    overlay->init();
    while(true)
        overlay->draw();

	http_listener listener(U("http://*:7890"));


	listener.open().then([&listener]() {
        std::cout << (U("\n start \n")); 
    }).wait();
	
    listener.support(methods::GET, [](http_request req) {
		req.reply(status_codes::OK, U("Overlay Server Online"));
	});
	
    listener.support(methods::POST, [](http_request req) {
        std::cout << "handle_post request" << std::endl;

        req.extract_json().then([req](web::json::value body){
            wcout << body.serialize() << endl;
            
            req.reply(status_codes::OK, body);
        });



    });
    
    while (true);
	listener.close();
	return 0;
}

