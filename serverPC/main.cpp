#include <iostream>
#include <cpprest/http_listener.h>

#include "Overlay.h"

using namespace std;
using namespace web::http;
using namespace web::http::experimental::listener;

Overlay* overlay;
SharedSection drawsection;

/*
* this program runs 2nd PC
*/

void handle_get(http_request request)
{
    request.reply(status_codes::OK, U("Overlay Server Online"));
}

void handle_post(http_request request)
{
   // std::cout << "handle_post request" << std::endl;
    overlay->aa++;
    request.extract_json().then([request](web::json::value v) {
        //wcout << body.serialize() << endl;



        try
        {
           // std::cout << "parse json data";
            utility::string_t jsonval = v.serialize();
            //wcout << jsonval;
            auto status = v.at(U("status")).as_string();

            auto lines = v.at(U("lines")).as_array();

            for (int i = 0; i < lines.size(); i++){
                float x1 = lines[i].at(U("x1")).as_double();
                float y1 = lines[i].at(U("y1")).as_double();
                float x2 = lines[i].at(U("x2")).as_double();
                float y2 = lines[i].at(U("y2")).as_double();
                float thickness = lines[i].at(U("thickness")).as_double();
                float r = lines[i].at(U("r")).as_double();
                float g = lines[i].at(U("g")).as_double();
                float b = lines[i].at(U("b")).as_double();
                float a = lines[i].at(U("a")).as_double();
                
                drawsection.drawline[i].x1 = x1;
                drawsection.drawline[i].y1 = y1;
                drawsection.drawline[i].x2 = x2;
                drawsection.drawline[i].y2 = y2;
                drawsection.drawline[i].thickness = thickness;
                drawsection.drawline[i].r = r;
                drawsection.drawline[i].g = g;
                drawsection.drawline[i].b = b;
                drawsection.drawline[i].a = a;
            }
            drawsection.linenum = lines.size();
            
            
            //std::wcout << "\n" << status;

            /*auto array = v.at(U("rows")).as_array();
            for (int i = 0; i < array.size(); ++i)
            {
                auto id = array[i].at(U("id")).as_string();
                std::wcout << "\n" << id;
                auto key = array[i].at(U("key")).as_string();
                std::wcout << "\n" << key;
                auto array2 = array[i].at(U("value")).as_array();

                std::wcout << array2[0] << array2[1];
            }*/
        }
        catch (const http_exception& e)
        {
            wostringstream ss;
            ss << e.what() << endl;
            wcout << ss.str();
        }


        request.reply(status_codes::OK, v);
    });
    
}


int main() {
    overlay = new Overlay();
    overlay->init();
    overlay->draw(drawsection);

	http_listener listener(U("http://*:17891"));

	listener.open().then([&listener]() {
        std::cout << (U("\n start \n")); 
    }).wait();
	
    listener.support(methods::GET, handle_get);
    listener.support(methods::POST, handle_post);
    
    while (true) {
        if (overlay)
            overlay->draw(drawsection);
    }
    listener.close();
	return 0;
}