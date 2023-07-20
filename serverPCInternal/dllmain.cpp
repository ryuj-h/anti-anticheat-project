#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <cpprest/http_listener.h>

#include "ImRender.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/win32/imgui_impl_win32.h"
#include "imgui/dx9/imgui_impl_dx9.h"

#include "Utils.h"
#include "Global.h"
#include "DirectX9.h"
#include "SectionDef.h"
#include "V3.h"

using namespace std;

bool g_menu_opened = true;
WNDPROC g_wndproc = nullptr;
IMGUI_IMPL_API LRESULT  ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef HRESULT(APIENTRY* EndScene) (IDirect3DDevice9*);
EndScene oEndScene = NULL;

typedef HRESULT(APIENTRY* Present) (IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
Present oPresent = NULL;

using namespace std;
using namespace web::http;
using namespace web::http::experimental::listener;
SharedSection drawsection;

int s_width = GetSystemMetrics(SM_CXSCREEN);
int s_height = GetSystemMetrics(SM_CYSCREEN);

void handle_get(http_request request)
{
    request.reply(status_codes::OK, U("Overlay Server Online"));
}

void handle_post(http_request request)
{
    // std::cout << "handle_post request" << std::endl;
    request.extract_json().then([request](web::json::value v) {
        //wcout << body.serialize() << endl;
        try
        {
            // std::cout << "parse json data";
            utility::string_t jsonval = v.serialize();
            //wcout << jsonval;
            //auto status = v.at(U("status")).as_string();

            auto aimbot = v.at(U("ab")).as_integer();
            drawsection.aimbot = aimbot;

            auto aimspeed = v.at(U("aimspeed")).as_integer();
            drawsection.aimspeed = aimspeed;

            auto latency = v.at(U("lat")).as_integer();
            drawsection.latency = latency;

            auto predx = v.at(U("predx")).as_integer();
            drawsection.predx = predx;

            auto predy = v.at(U("predy")).as_integer();
            drawsection.predy = predy;


            auto lines = v.at(U("ln")).as_array();

            for (int i = 0; i < lines.size(); i++) {
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

                drawsection.player[i].hp = players[i].at(U("hp")).as_integer();
                //vis
                drawsection.player[i].visible = players[i].at(U("vis")).as_integer();
                drawsection.player[i]._minimapx = players[i].at(U("minix")).as_integer();
                drawsection.player[i]._minimapy = players[i].at(U("miniy")).as_integer();



                drawsection.player[i].distance = players[i].at(U("dt")).as_integer();
                drawsection.player[i]._basex = players[i].at(U("bx")).as_integer();
                drawsection.player[i]._basey = players[i].at(U("by")).as_integer();


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


            auto abplayer = v.at(U("abPlayer")).as_object();

            if (true) {
                drawsection.abplayer.visible = abplayer.at(U("15x")).as_integer();
                drawsection.abplayer._15x = abplayer.at(U("15x")).as_integer();
                drawsection.abplayer._15y = abplayer.at(U("15y")).as_integer();
                drawsection.abplayer._6x = abplayer.at(U("6x")).as_integer();
                drawsection.abplayer._6y = abplayer.at(U("6y")).as_integer();
                drawsection.abplayer._5x = abplayer.at(U("5x")).as_integer();
                drawsection.abplayer._5y = abplayer.at(U("5y")).as_integer();
                drawsection.abplayer._115x = abplayer.at(U("115x")).as_integer();
                drawsection.abplayer._115y = abplayer.at(U("115y")).as_integer();
                drawsection.abplayer._116x = abplayer.at(U("116x")).as_integer();
                drawsection.abplayer._116y = abplayer.at(U("116y")).as_integer();
                drawsection.abplayer._117x = abplayer.at(U("117x")).as_integer();
                drawsection.abplayer._117y = abplayer.at(U("117y")).as_integer();
                drawsection.abplayer._88x = abplayer.at(U("88x")).as_integer();
                drawsection.abplayer._88y = abplayer.at(U("88y")).as_integer();
                drawsection.abplayer._89x = abplayer.at(U("89x")).as_integer();
                drawsection.abplayer._89y = abplayer.at(U("89y")).as_integer();
                drawsection.abplayer._90x = abplayer.at(U("90x")).as_integer();
                drawsection.abplayer._90y = abplayer.at(U("90y")).as_integer();
                drawsection.abplayer._1x = abplayer.at(U("1x")).as_integer();
                drawsection.abplayer._1y = abplayer.at(U("1y")).as_integer();
                drawsection.abplayer._3x = abplayer.at(U("3x")).as_integer();
                drawsection.abplayer._3y = abplayer.at(U("3y")).as_integer();
                drawsection.abplayer._2x = abplayer.at(U("2x")).as_integer();
                drawsection.abplayer._2y = abplayer.at(U("2y")).as_integer();
                drawsection.abplayer._174x = abplayer.at(U("174x")).as_integer();
                drawsection.abplayer._174y = abplayer.at(U("174y")).as_integer();
                drawsection.abplayer._175x = abplayer.at(U("175x")).as_integer();
                drawsection.abplayer._175y = abplayer.at(U("175y")).as_integer();
                drawsection.abplayer._176x = abplayer.at(U("176x")).as_integer();
                drawsection.abplayer._176y = abplayer.at(U("176y")).as_integer();
                drawsection.abplayer._168x = abplayer.at(U("168x")).as_integer();
                drawsection.abplayer._168y = abplayer.at(U("168y")).as_integer();
                drawsection.abplayer._169x = abplayer.at(U("169x")).as_integer();
                drawsection.abplayer._169y = abplayer.at(U("169y")).as_integer();
                drawsection.abplayer._170x = abplayer.at(U("170x")).as_integer();
                drawsection.abplayer._170y = abplayer.at(U("170y")).as_integer();
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




LRESULT WINAPI WndProc(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param)
{
	switch (u_msg)
	{
	case WM_KEYDOWN:
		if (w_param == VK_END)
			g_menu_opened = !g_menu_opened;
		break;
	default:
		break;
	}

	if (g_menu_opened && ImGui_ImplWin32_WndProcHandler(hwnd, u_msg, w_param, l_param))
		return true;

	return CallWindowProcA(g_wndproc, hwnd, u_msg, w_param, l_param);
}


HRESULT APIENTRY MJPresent(IDirect3DDevice9* pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion) {
	return oPresent(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

void DrawJsonData(SharedSection& drawsection) {

    s_width = GetSystemMetrics(SM_CXSCREEN);
    s_height = GetSystemMetrics(SM_CYSCREEN);

    for (int i = 0; i < drawsection.linenum; i++) {

        render.draw_line(drawsection.drawline[i].x1, drawsection.drawline[i].y1, drawsection.drawline[i].x2, drawsection.drawline[i].y2,
            ImColor((float)drawsection.drawline[i].r, (float)drawsection.drawline[i].g, (float)drawsection.drawline[i].b, (float)drawsection.drawline[i].a),
            (float)drawsection.drawline[i].thickness);
    }

    for (int i = 0; i < drawsection.playernum; i++) {
        float distance = drawsection.player[i].distance;
        Vector3 basepos = { (float)drawsection.player[i]._basex,	(float)drawsection.player[i]._basey,0 };

        Vector3 fforepos = { (float)drawsection.player[i]._15x , (float)drawsection.player[i]._15y,0 };// GetBoneWithRotation(mesh, fforehead);
        Vector3 fHeadpos = { (float)drawsection.player[i]._6x , (float)drawsection.player[i]._6y,0 };//GetBoneWithRotation(mesh, fHead);
        Vector3 fneck_01pos = { (float)drawsection.player[i]._5x , (float)drawsection.player[i]._5y,0 };//GetBoneWithRotation(mesh, fneck_01);

        Vector3 fupperarm_r_pos = { (float)drawsection.player[i]._115x , (float)drawsection.player[i]._115y,0 };//GetBoneWithRotation(mesh, fupperarm_r);
        Vector3 flowerarm_r_pos = { (float)drawsection.player[i]._116x , (float)drawsection.player[i]._116y,0 };// GetBoneWithRotation(mesh, flowerarm_r);
        Vector3 fhand_r_pos = { (float)drawsection.player[i]._117x , (float)drawsection.player[i]._117y,0 };//GetBoneWithRotation(mesh, fhand_r);

        Vector3 fupperarm_l_pos = { (float)drawsection.player[i]._88x , (float)drawsection.player[i]._88y,0 };//GetBoneWithRotation(mesh, fupperarm_l);
        Vector3 flowerarm_l_pos = { (float)drawsection.player[i]._89x , (float)drawsection.player[i]._89y,0 };//GetBoneWithRotation(mesh, flowerarm_l);
        Vector3 fhand_l_pos = { (float)drawsection.player[i]._90x , (float)drawsection.player[i]._90y,0 };//GetBoneWithRotation(mesh, fhand_l);

        Vector3 fpelvis_pos = { (float)drawsection.player[i]._1x , (float)drawsection.player[i]._1y,0 };//GetBoneWithRotation(mesh, fpelvis);
        Vector3 fspine_02_pos = { (float)drawsection.player[i]._3x,  (float)drawsection.player[i]._3y, 0 };// GetBoneWithRotation(mesh, fspine_02);
        Vector3 fspine_01_pos = { (float)drawsection.player[i]._2x,  (float)drawsection.player[i]._2y, 0 };//GetBoneWithRotation(mesh, fspine_01);

        Vector3 fthigh_r_pos = { (float)drawsection.player[i]._174x,  (float)drawsection.player[i]._174y, 0 };//GetBoneWithRotation(mesh, fthigh_r);
        Vector3 fcalf_r_pos = { (float)drawsection.player[i]._175x,  (float)drawsection.player[i]._175y, 0 };//GetBoneWithRotation(mesh, fcalf_r);
        Vector3 ffoot_r_pos = { (float)drawsection.player[i]._176x,  (float)drawsection.player[i]._176y, 0 };//GetBoneWithRotation(mesh, ffoot_r);

        Vector3 fthigh_l_pos = { (float)drawsection.player[i]._168x,  (float)drawsection.player[i]._168y, 0 };//GetBoneWithRotation(mesh, fthigh_l);
        Vector3 fcalf_l_pos = { (float)drawsection.player[i]._169x,  (float)drawsection.player[i]._169y, 0 };//GetBoneWithRotation(mesh, fcalf_l);
        Vector3 ffoot_l_pos = { (float)drawsection.player[i]._170x,  (float)drawsection.player[i]._170y, 0 };//GetBoneWithRotation(mesh, ffoot_l);




        static char distancebuffer[1024];
        sprintf(distancebuffer, "[%0.0fm]\0", distance);
        render.draw_text(basepos.x, basepos.y, distancebuffer, true,true,true, ImColor(255, 255, 255, 255), c_renderer::text_normal);
        //Draw_Background_String(std::string(distancebuffer), 12.f, basepos.x, basepos.y, 1, 1, 1, 1);

        int isvisible = drawsection.player[i].visible;

        float a, r, g, b;
        if (isvisible != 0) {
            a = 0.5f;
            r = 1.f;
            g = 0.f;
            b = 0.f;
        }
        else {
            a = 0.5f;
            r = 0.f;
            g = 1.f;
            b = 0.f;
        }

        if (false)//skelleton esp
        {
            render.draw_line(fforepos.x, fforepos.y, fHeadpos.x, fHeadpos.y, ImColor(r, g, b, a), 1.0f);
            render.draw_line(fHeadpos.x, fHeadpos.y, fneck_01pos.x, fneck_01pos.y, ImColor(r, g, b, a), 1.0f);

            render.draw_line(fneck_01pos.x, fneck_01pos.y, fupperarm_r_pos.x, fupperarm_r_pos.y, ImColor(r, g, b, a), 1.0f);
            render.draw_line(fupperarm_r_pos.x, fupperarm_r_pos.y, flowerarm_r_pos.x, flowerarm_r_pos.y, ImColor(r, g, b, a), 1.0f);
            render.draw_line(flowerarm_r_pos.x, flowerarm_r_pos.y, fhand_r_pos.x, fhand_r_pos.y, 1.0f);

            render.draw_line(fneck_01pos.x, fneck_01pos.y, fupperarm_l_pos.x, fupperarm_l_pos.y, ImColor(r, g, b, a), 1.0f);
            render.draw_line(fupperarm_l_pos.x, fupperarm_l_pos.y, flowerarm_l_pos.x, flowerarm_l_pos.y, ImColor(r, g, b, a), 1.0f);
            render.draw_line(flowerarm_l_pos.x, flowerarm_l_pos.y, fhand_l_pos.x, fhand_l_pos.y, ImColor(r, g, b, a), 1.0f);

            render.draw_line(fneck_01pos.x, fneck_01pos.y, fspine_02_pos.x, fspine_02_pos.y, ImColor(r, g, b, a), 1.0f);
            render.draw_line(fspine_02_pos.x, fspine_02_pos.y, fspine_01_pos.x, fspine_01_pos.y, ImColor(r, g, b, a), 1.0f);
            render.draw_line(fspine_01_pos.x, fspine_01_pos.y, fpelvis_pos.x, fpelvis_pos.y, ImColor(r, g, b, a), 1.0f);

            render.draw_line(fpelvis_pos.x, fpelvis_pos.y, fthigh_r_pos.x, fthigh_r_pos.y, ImColor(r, g, b, a), 1.0f);
            render.draw_line(fthigh_r_pos.x, fthigh_r_pos.y, fcalf_r_pos.x, fcalf_r_pos.y, ImColor(r, g, b, a), 1.0f);
            render.draw_line(fcalf_r_pos.x, fcalf_r_pos.y, ffoot_r_pos.x, ffoot_r_pos.y, ImColor(r, g, b, a), 1.0f);

            render.draw_line(fpelvis_pos.x, fpelvis_pos.y, fthigh_l_pos.x, fthigh_l_pos.y, ImColor(r, g, b, a), 1.0f);
            render.draw_line(fthigh_l_pos.x, fthigh_l_pos.y, fcalf_l_pos.x, fcalf_l_pos.y, ImColor(r, g, b, a), 1.0f);
            render.draw_line(fcalf_l_pos.x, fcalf_l_pos.y, ffoot_l_pos.x, ffoot_l_pos.y, ImColor(r, g, b, a), 1.0f);
        }

        if (1) {
            int x, y;

            x = drawsection.player[i]._minimapx;
            y = drawsection.player[i]._minimapy;
            if (x != -1 && y != -1) {
                //1694 1049
                render.draw_rect(1437 + (1694 - 1437) * (x / 100.f) / 400.f + 1 - 2, 793 + (1049 - 793) * (y / 100.f) / 400.f + 1 - 2,
                    3, 3,
                    ImColor(255, 0, 0, 255));

                render.draw_rect(1437 + (1694 - 1437) * (x / 100.f) / 400.f - 2, 793 + (1049 - 793) * (y / 100.f) / 400.f - 2,
                    5, 5,
                    ImColor(0, 0, 0, 255));

                //
                /*render.draw_rect(1628 + 254 * (x / 100.f) / 400.f + 1 - 2, 797 + 254 * (y / 100.f) / 400.f + 1 - 2,
                    3, 3,
                    ImColor(255, 0, 0, 255));

                render.draw_rect(1628 + 254 * (x / 100.f) / 400.f - 2, 797 + 254 * (y / 100.f) / 400.f - 2,
                    5, 5,
                    ImColor(0, 0, 0, 255));
            */
            }
        }

        if (true) {
            if (distance < 400) {
                float hp = drawsection.player[i].hp;
                render.draw_rect(basepos.x - 40, basepos.y + 13,
                    80, 5,
                    ImColor(0.f, 0.f, 0.f, 0.3f));
                if (hp > 40.0) {
                    render.draw_rect(basepos.x - 40 + 1, basepos.y + 13 + 1,
                        80 * (hp / 100.f) - 2, 5 - 2,
                        ImColor(1.f, 1.f, 1.f, 0.8f));
                }
                else {
                    render.draw_rect(basepos.x - 40 + 1, basepos.y + 13 + 1,
                        80 * (hp / 100.f) - 2, 5 - 2,
                        ImColor(1.f, 0.f, 0.f, 0.8f));
                }
                render.draw_rect(basepos.x - 40, basepos.y + 13,
                    80, 5,
                    ImColor(0.f, 0.f, 0.f, 1.f));
            }
        }
    }

    /*render.draw_rect(1628, 791,
        261, 261,
        ImColor(255, 255, 255, 255));
    */
    /*render.draw_rect(1597, 792,
        1883-1597, 1050-792,
        ImColor(255, 255, 255, 255));
        */


    int latency = drawsection.latency;
    static char latencybuffer[1024];
    sprintf(latencybuffer, "latency : %d\0", latency);
    render.draw_text(s_width / 2,0, latencybuffer, true, true, true, ImColor(255, 255, 255, 255), c_renderer::text_normal);

    /*int aimspeed = drawsection.aimspeed;
    static char aimspeedbuffer[1024];
    sprintf(aimspeedbuffer, "aimspeed : %d\0", aimspeed);
    render.draw_text(s_width / 2, 13, aimspeedbuffer, true, true, true, ImColor(255, 255, 255, 255), c_renderer::text_normal);

    int aimbot = drawsection.aimbot;
    static char aimbotbuffer[1024];
    sprintf(aimbotbuffer, "aimbot : %d\0", aimbot);
    render.draw_text(s_width / 2, 16, aimbotbuffer, true, true, true, ImColor(255, 255, 255, 255), c_renderer::text_normal);
    */
    if (true) {
        int x = drawsection.predx;
        int y = drawsection.predy;

        static Vector3 center(s_width / 2, s_height / 2, 0);



        if (center.Distance(Vector3(x, y, 0)) < 125.f)
        {
            if (drawsection.abplayer.visible) {
                Vector3 basepos = { (float)drawsection.abplayer._basex,	(float)drawsection.abplayer._basey,0 };

                Vector3 fforepos = { (float)drawsection.abplayer._15x , (float)drawsection.abplayer._15y,0 };// GetBoneWithRotation(mesh, fforehead);
                Vector3 fHeadpos = { (float)drawsection.abplayer._6x , (float)drawsection.abplayer._6y,0 };//GetBoneWithRotation(mesh, fHead);
                Vector3 fneck_01pos = { (float)drawsection.abplayer._5x , (float)drawsection.abplayer._5y,0 };//GetBoneWithRotation(mesh, fneck_01);

                Vector3 fupperarm_r_pos = { (float)drawsection.abplayer._115x , (float)drawsection.abplayer._115y,0 };//GetBoneWithRotation(mesh, fupperarm_r);
                Vector3 flowerarm_r_pos = { (float)drawsection.abplayer._116x , (float)drawsection.abplayer._116y,0 };// GetBoneWithRotation(mesh, flowerarm_r);
                Vector3 fhand_r_pos = { (float)drawsection.abplayer._117x , (float)drawsection.abplayer._117y,0 };//GetBoneWithRotation(mesh, fhand_r);

                Vector3 fupperarm_l_pos = { (float)drawsection.abplayer._88x , (float)drawsection.abplayer._88y,0 };//GetBoneWithRotation(mesh, fupperarm_l);
                Vector3 flowerarm_l_pos = { (float)drawsection.abplayer._89x , (float)drawsection.abplayer._89y,0 };//GetBoneWithRotation(mesh, flowerarm_l);
                Vector3 fhand_l_pos = { (float)drawsection.abplayer._90x , (float)drawsection.abplayer._90y,0 };//GetBoneWithRotation(mesh, fhand_l);

                Vector3 fpelvis_pos = { (float)drawsection.abplayer._1x , (float)drawsection.abplayer._1y,0 };//GetBoneWithRotation(mesh, fpelvis);
                Vector3 fspine_02_pos = { (float)drawsection.abplayer._3x,  (float)drawsection.abplayer._3y, 0 };// GetBoneWithRotation(mesh, fspine_02);
                Vector3 fspine_01_pos = { (float)drawsection.abplayer._2x,  (float)drawsection.abplayer._2y, 0 };//GetBoneWithRotation(mesh, fspine_01);

                Vector3 fthigh_r_pos = { (float)drawsection.abplayer._174x,  (float)drawsection.abplayer._174y, 0 };//GetBoneWithRotation(mesh, fthigh_r);
                Vector3 fcalf_r_pos = { (float)drawsection.abplayer._175x,  (float)drawsection.abplayer._175y, 0 };//GetBoneWithRotation(mesh, fcalf_r);
                Vector3 ffoot_r_pos = { (float)drawsection.abplayer._176x,  (float)drawsection.abplayer._176y, 0 };//GetBoneWithRotation(mesh, ffoot_r);

                Vector3 fthigh_l_pos = { (float)drawsection.abplayer._168x,  (float)drawsection.abplayer._168y, 0 };//GetBoneWithRotation(mesh, fthigh_l);
                Vector3 fcalf_l_pos = { (float)drawsection.abplayer._169x,  (float)drawsection.abplayer._169y, 0 };//GetBoneWithRotation(mesh, fcalf_l);
                Vector3 ffoot_l_pos = { (float)drawsection.abplayer._170x,  (float)drawsection.abplayer._170y, 0 };//GetBoneWithRotation(mesh, ffoot_l);

                //255 * 0, 0xa3, 0xd2, (int)255 * 1
                int r = 0;
                int g = 0xa3;
                int b = 0xd2;
                int a = 200;

                render.draw_line(fforepos.x, fforepos.y, fHeadpos.x, fHeadpos.y, ImColor(r, g, b, a), 1.0f);
                render.draw_line(fHeadpos.x, fHeadpos.y, fneck_01pos.x, fneck_01pos.y, ImColor(r, g, b, a), 1.0f);

                render.draw_line(fneck_01pos.x, fneck_01pos.y, fupperarm_r_pos.x, fupperarm_r_pos.y, ImColor(r, g, b, a), 1.0f);
                render.draw_line(fupperarm_r_pos.x, fupperarm_r_pos.y, flowerarm_r_pos.x, flowerarm_r_pos.y, ImColor(r, g, b, a), 1.0f);
                render.draw_line(flowerarm_r_pos.x, flowerarm_r_pos.y, fhand_r_pos.x, fhand_r_pos.y, 1.0f);

                render.draw_line(fneck_01pos.x, fneck_01pos.y, fupperarm_l_pos.x, fupperarm_l_pos.y, ImColor(r, g, b, a), 1.0f);
                render.draw_line(fupperarm_l_pos.x, fupperarm_l_pos.y, flowerarm_l_pos.x, flowerarm_l_pos.y, ImColor(r, g, b, a), 1.0f);
                render.draw_line(flowerarm_l_pos.x, flowerarm_l_pos.y, fhand_l_pos.x, fhand_l_pos.y, ImColor(r, g, b, a), 1.0f);

                render.draw_line(fneck_01pos.x, fneck_01pos.y, fspine_02_pos.x, fspine_02_pos.y, ImColor(r, g, b, a), 1.0f);
                render.draw_line(fspine_02_pos.x, fspine_02_pos.y, fspine_01_pos.x, fspine_01_pos.y, ImColor(r, g, b, a), 1.0f);
                render.draw_line(fspine_01_pos.x, fspine_01_pos.y, fpelvis_pos.x, fpelvis_pos.y, ImColor(r, g, b, a), 1.0f);

                render.draw_line(fpelvis_pos.x, fpelvis_pos.y, fthigh_r_pos.x, fthigh_r_pos.y, ImColor(r, g, b, a), 1.0f);
                render.draw_line(fthigh_r_pos.x, fthigh_r_pos.y, fcalf_r_pos.x, fcalf_r_pos.y, ImColor(r, g, b, a), 1.0f);
                render.draw_line(fcalf_r_pos.x, fcalf_r_pos.y, ffoot_r_pos.x, ffoot_r_pos.y, ImColor(r, g, b, a), 1.0f);

                render.draw_line(fpelvis_pos.x, fpelvis_pos.y, fthigh_l_pos.x, fthigh_l_pos.y, ImColor(r, g, b, a), 1.0f);
                render.draw_line(fthigh_l_pos.x, fthigh_l_pos.y, fcalf_l_pos.x, fcalf_l_pos.y, ImColor(r, g, b, a), 1.0f);
                render.draw_line(fcalf_l_pos.x, fcalf_l_pos.y, ffoot_l_pos.x, ffoot_l_pos.y, ImColor(r, g, b, a), 1.0f);
            }



            render.draw_line(x - 2, y - 2, x - 2, y + 2,
                ImColor(255 * 0, 0xa3, 0xd2, (int)(255 * 1)),
                1.f);
            render.draw_line(x - 2, y - 2, x + 2, y - 2,
                ImColor(255 * 0, 0xa3, 0xd2, (int)(255 * 1)),
                1.f);
            render.draw_line(x + 2, y - 2, x + 2, y + 2,
                ImColor(255 * 0, 0xa3, 0xd2, (int)(255 * 1)),
                1.f);
            render.draw_line(x - 2, y + 2, x + 3, y + 2,
                ImColor(255 * 0, 0xa3, 0xd2, (int)255 * 1),
                1.f);
        }
        
    }

    if (true) {

        int fpscount = (int)ImGui::GetIO().Framerate;
        static char fpsbuffer[1024];
        sprintf(fpsbuffer, "Fps : %d\0", fpscount);
        render.draw_text(s_width - 105, 85 + 17, fpsbuffer, true, true, false, ImColor(255, 255, 255, 255), c_renderer::text_normal);

    }
}


HRESULT APIENTRY MJEndScene(IDirect3DDevice9* pDevice)
{
	static IDirect3DDevice9* pDevicest = nullptr;
	static bool bOnce = false;

	if (pDevice == nullptr || pDevicest != pDevice) {
		if (bOnce)
			render.free();
		render.init(pDevice);
		pDevicest = pDevice;
		bOnce = true;
	}

	render.begin_draw();

    DrawJsonData(drawsection);

	render.end_draw();
	return oEndScene(pDevice);
}

DWORD WINAPI drawingtrhead(LPVOID lpParam)
{
    http_listener listener(U("http://*:17892"));

    listener.open().then([&listener]() {
        std::cout << (U("\n start \n"));
    }).wait();

    listener.support(methods::GET, handle_get);
    listener.support(methods::POST, handle_post);

    while (true) {
        Sleep(10);
    }
}

void DllMainAttach(HMODULE hModule) {
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	cout << "helloworld" << endl;

    HMODULE win32u = LoadLibraryW(L"cpprest142_2_10.dll");
    HMODULE win32u2 = LoadLibraryW(L"KernelBase.dll");
    HMODULE win32u3 = LoadLibraryW(L"kernel32.dll");

    HANDLE handleDraw;
    DWORD drawID = 1;
    DWORD drawParam = 1;

    handleDraw = CreateThread(NULL,
        0,
        drawingtrhead,
        &drawParam,
        0,
        &drawID);

    

	while (true) {
		if (DirectX9::Init() == true) {
			CreateHook(42, (void**)&oEndScene, MJEndScene);
			break;
		}
		Sleep(1000);
	}
}

void DllMainDetach() {

}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID)
{
	if (hModule != nullptr)
		DisableThreadLibraryCalls(hModule);

	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		DllMainAttach(hModule);
		return TRUE;
	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH) {
		DllMainDetach();
		return TRUE;
	}

	return FALSE;
}