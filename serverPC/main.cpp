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
            //auto status = v.at(U("status")).as_string();

            auto lines = v.at(U("ln")).as_array();

            for (int i = 0; i < lines.size(); i++){
                float x1 = lines[i].at(U("x1")).as_integer();
                float y1 = lines[i].at(U("y1")).as_integer();
                float x2 = lines[i].at(U("x2")).as_integer();
                float y2 = lines[i].at(U("y2")).as_integer();
                float thickness = lines[i].at(U("th")).as_integer();
                float r = lines[i].at(U("r")).as_integer();
                float g = lines[i].at(U("g")).as_integer();
                float b = lines[i].at(U("b")).as_integer();
                float a = lines[i].at(U("a")).as_integer();
                
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
            
            auto players = v.at(U("Players")).as_array();
            for (int i = 0; i < players.size(); i++) {
                drawsection.player[i]._15x = players[i].at(U("15x")).as_integer();
                drawsection.player[i]._15y = players[i].at(U("15y")).as_integer();
                drawsection.player[i]._6x = players[i].at(U("6x")).as_integer();
                drawsection.player[i]._6y = players[i].at(U("6y")).as_integer();
                drawsection.player[i]._5x = players[i].at(U("5x")).as_integer();
                drawsection.player[i]._5y = players[i].at(U("5y")).as_integer();
                drawsection.player[i]._115x = players[i].at(U("115x")).as_integer();
                drawsection.player[i]._115y = players[i].at(U("115y")).as_integer();
                drawsection.player[i]._116x = players[i].at(U("116x")).as_integer();
                drawsection.player[i]._116y = players[i].at(U("116y")).as_integer();
                drawsection.player[i]._117x = players[i].at(U("117x")).as_integer();
                drawsection.player[i]._117y = players[i].at(U("117y")).as_integer();
                drawsection.player[i]._88x = players[i].at(U("88x")).as_integer();
                drawsection.player[i]._88y = players[i].at(U("88y")).as_integer();
                drawsection.player[i]._89x = players[i].at(U("89x")).as_integer();
                drawsection.player[i]._89y = players[i].at(U("89y")).as_integer();
                drawsection.player[i]._90x = players[i].at(U("90x")).as_integer();
                drawsection.player[i]._90y = players[i].at(U("90y")).as_integer();
                drawsection.player[i]._1x = players[i].at(U("1x")).as_integer();
                drawsection.player[i]._1y = players[i].at(U("1y")).as_integer();
                drawsection.player[i]._3x = players[i].at(U("3x")).as_integer();
                drawsection.player[i]._3y = players[i].at(U("3y")).as_integer();
                drawsection.player[i]._2x = players[i].at(U("2x")).as_integer();
                drawsection.player[i]._2y = players[i].at(U("2y")).as_integer();
                drawsection.player[i]._174x = players[i].at(U("174x")).as_integer();
                drawsection.player[i]._174y = players[i].at(U("174y")).as_integer();
                drawsection.player[i]._175x = players[i].at(U("175x")).as_integer();
                drawsection.player[i]._175y = players[i].at(U("175y")).as_integer();
                drawsection.player[i]._176x = players[i].at(U("176x")).as_integer();
                drawsection.player[i]._176y = players[i].at(U("176y")).as_integer();
                drawsection.player[i]._168x = players[i].at(U("168x")).as_integer();
                drawsection.player[i]._168y = players[i].at(U("168y")).as_integer();
                drawsection.player[i]._169x = players[i].at(U("169x")).as_integer();
                drawsection.player[i]._169y = players[i].at(U("169y")).as_integer();
                drawsection.player[i]._170x = players[i].at(U("170x")).as_integer();
                drawsection.player[i]._170y = players[i].at(U("170y")).as_integer();
            }
            drawsection.playernum = players.size();
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