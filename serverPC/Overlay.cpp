#include "Overlay.h"
#include "V3.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "dwrite.lib")

using namespace std;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uiMessage)
	{
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, uiMessage, wParam, lParam);
	}
	return 0;
}

void Overlay::init()
{
	WNDCLASS wc = { };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(0);
	wc.lpszClassName = L"OverlayWindow";
	RegisterClass(&wc);
	hwnd = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOPMOST,//WS_EX_LAYERED,
		wc.lpszClassName, L"OverlayWindow", WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, wc.hInstance, NULL);
	
	HRGN rgn;
	DWM_BLURBEHIND blur;
	rgn = CreateRectRgn(0, 0, 1, 1);
	blur.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
	blur.fEnable = true;
	blur.fTransitionOnMaximized = true;
	blur.hRgnBlur = rgn;
	
	DwmEnableBlurBehindWindow(hwnd, &blur);
	
	SetWindowPos(hwnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_NOACTIVATE);

	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &factory);
	if (!SUCCEEDED(hr)){
		cout << "[!] failed createFactory" << endl;
	}
	hr = factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_HARDWARE,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)),
		D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(200, 200),
			D2D1_PRESENT_OPTIONS_IMMEDIATELY), &target);
	if (!SUCCEEDED(hr)){
		cout << "[!] failed CreateHwndRenderTarget" << endl;
	}
	hr = target->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f), &solid_brush);
	if (!SUCCEEDED(hr)){
		cout << "[!] failed CreateSolidColorBrush" << endl;
	}
	target->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&w_factory));

	w_factory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 15.0f, L"ko-KR", &w_format);

	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
	UpdateWindow(hwnd);
	ShowWindow(hwnd, 1);
}


void Overlay::drawskell(SharedSection& drawsection) {

}

void Overlay::draw(SharedSection& drawsection)
{
	MSG message;
	message.message = WM_NULL;
	if (message.message != WM_QUIT)
	{
		if (PeekMessage(&message, hwnd, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		WINDOWINFO info;
		ZeroMemory(&info, sizeof(info));
		info.cbSize = sizeof(info);

		GetWindowInfo(hwnd, &info);

		D2D1_SIZE_U siz;
		siz.height = ((info.rcClient.bottom) - (info.rcClient.top));
		siz.width = ((info.rcClient.right) - (info.rcClient.left));

		target->Resize(&siz);
		target->BeginDraw();
		target->Clear(D2D1::ColorF(0, 0, 0, 0));

		//DrawString("Hello world!", 12.f, aa, 100, 1.f, 1.f, 1.f, 1.f);
		if (aa > 1920)
			aa = 0;

		//DrawLine(0, 0, 100, 100, 1, 1, 0, 0, 1.f);
		for (int i = 0; i < drawsection.linenum; i++) {
			//cout << drawsection.drawline[i].a << endl;
			DrawLine(drawsection.drawline[i].x1, drawsection.drawline[i].y1, drawsection.drawline[i].x2, drawsection.drawline[i].y2, (float)drawsection.drawline[i].thickness,
				(float)drawsection.drawline[i].r, (float)drawsection.drawline[i].g, (float)drawsection.drawline[i].b, (float)drawsection.drawline[i].a);//(float)drawsection.drawline[i].a);
		}

		for (int i = 0; i < drawsection.playernum; i++) {
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

			DrawLine(fforepos.x, fforepos.y, fHeadpos.x, fHeadpos.y, 3, 1, 1, 1, 1);
			DrawLine(fHeadpos.x, fHeadpos.y, fneck_01pos.x, fneck_01pos.y, 3, 1, 1, 1, 1);

			DrawLine(fneck_01pos.x, fneck_01pos.y, fupperarm_r_pos.x, fupperarm_r_pos.y, 3, 1, 1, 1, 1);
			DrawLine(fupperarm_r_pos.x, fupperarm_r_pos.y, flowerarm_r_pos.x, flowerarm_r_pos.y, 3, 1, 1, 1, 1);
			DrawLine(flowerarm_r_pos.x, flowerarm_r_pos.y, fhand_r_pos.x, fhand_r_pos.y, 3, 1, 1, 1, 1);

			DrawLine(fneck_01pos.x, fneck_01pos.y, fupperarm_l_pos.x, fupperarm_l_pos.y, 3, 1, 1, 1, 1);
			DrawLine(fupperarm_l_pos.x, fupperarm_l_pos.y, flowerarm_l_pos.x, flowerarm_l_pos.y, 3, 1, 1, 1, 1);
			DrawLine(flowerarm_l_pos.x, flowerarm_l_pos.y, fhand_l_pos.x, fhand_l_pos.y, 3, 1, 1, 1, 1);

			DrawLine(fneck_01pos.x, fneck_01pos.y, fspine_02_pos.x, fspine_02_pos.y, 3, 1, 1, 1, 1);
			DrawLine(fspine_02_pos.x, fspine_02_pos.y, fspine_01_pos.x, fspine_01_pos.y, 3, 1, 1, 1, 1);
			DrawLine(fspine_01_pos.x, fspine_01_pos.y, fpelvis_pos.x, fpelvis_pos.y, 3, 1, 1, 1, 1);

			DrawLine(fpelvis_pos.x, fpelvis_pos.y, fthigh_r_pos.x, fthigh_r_pos.y, 3, 1, 1, 1, 1);
			DrawLine(fthigh_r_pos.x, fthigh_r_pos.y, fcalf_r_pos.x, fcalf_r_pos.y, 3, 1, 1, 1, 1);
			DrawLine(fcalf_r_pos.x, fcalf_r_pos.y, ffoot_r_pos.x, ffoot_r_pos.y, 3, 1, 1, 1, 1);

			DrawLine(fpelvis_pos.x, fpelvis_pos.y, fthigh_l_pos.x, fthigh_l_pos.y, 3, 1, 1, 1, 1);
			DrawLine(fthigh_l_pos.x, fthigh_l_pos.y, fcalf_l_pos.x, fcalf_l_pos.y, 3, 1, 1, 1, 1);
			DrawLine(fcalf_l_pos.x, fcalf_l_pos.y, ffoot_l_pos.x, ffoot_l_pos.y, 3, 1, 1, 1, 1);
		}

		drawskell(drawsection);

		target->EndDraw();
	}

}

void Overlay::DrawString(std::string str, float fontSize, float x, float y, float r, float g, float b, float a)
{
	RECT re;
	GetClientRect(hwnd, &re);
	FLOAT dpix, dpiy;
	dpix = static_cast<float>(re.right - re.left);
	dpiy = static_cast<float>(re.bottom - re.top);

	USES_CONVERSION;
	wstring bb(A2W(str.c_str()));
	HRESULT res = w_factory->CreateTextLayout(bb.c_str(), bb.size(), w_format, dpix, dpiy, &w_layout);

	if (SUCCEEDED(res))
	{
		DWRITE_TEXT_RANGE range = { 0, str.length() };
		w_layout->SetFontSize(fontSize, range);
		solid_brush->SetColor(D2D1::ColorF(r, g, b, a));
		target->DrawTextLayout(D2D1::Point2F(x, y), w_layout, solid_brush);
		w_layout->Release();
		w_layout = NULL;
	}
}

void Overlay::DrawBox(float x, float y, float width, float height, float thickness, float r, float g, float b, float a, bool filled){
	solid_brush->SetColor(D2D1::ColorF(r, g, b, a));
	if (filled)  target->FillRectangle(D2D1::RectF(x, y, x + width, y + height), solid_brush);
	else target->DrawRectangle(D2D1::RectF(x, y, x + width, y + height), solid_brush, thickness);
}

void Overlay::DrawLine(float x1, float y1, float x2, float y2, float thickness, float r, float g, float b, float a){
	solid_brush->SetColor(D2D1::ColorF(r, g, b, a));
	target->DrawLine(D2D1::Point2F(x1, y1), D2D1::Point2F(x2, y2), solid_brush, thickness);
}

void Overlay::DrawCircle(float x, float y, float radius, float thickness, float r, float g, float b, float a, bool filled){
	solid_brush->SetColor(D2D1::ColorF(r, g, b, a));
	if (filled) target->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius), solid_brush);
	else target->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius), solid_brush, thickness);
}

void Overlay::DrawEllipse(float x, float y, float width, float height, float thickness, float r, float g, float b, float a, bool filled){
	solid_brush->SetColor(D2D1::ColorF(r, g, b, a));
	if (filled) target->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), width, height), solid_brush);
	else target->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), width, height), solid_brush, thickness);
}

void Overlay::Draw_Edge_String(std::string str, float fontSize, float x, float y, float r, float g, float b, float a){
	DrawString(str, fontSize, x + 1, y, 0, 0, 0, 1);
	DrawString(str, fontSize, x - 1, y, 0, 0, 0, 1);
	DrawString(str, fontSize, x, y + 1, 0, 0, 0, 1);
	DrawString(str, fontSize, x, y - 1, 0, 0, 0, 1);
	DrawString(str, fontSize, x, y, r, g, b, a);
}
