#include "Overlay.h"

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



void Overlay::draw(const SharedSection& drawsection)
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

		DrawString("Hello world!", 12.f, aa, 100, 1.f, 1.f, 1.f, 1.f);
		if (aa > 1920)
			aa = 0;

		DrawLine(0, 0, 100, 100, 1, 1, 0, 0, 1.f);
		for (int i = 0; i < drawsection.linenum; i++) {
			//cout << drawsection.drawline[i].a << endl;
			DrawLine(drawsection.drawline[i].x1, drawsection.drawline[i].y1, drawsection.drawline[i].x2, drawsection.drawline[i].y2, (float)drawsection.drawline[i].thickness,
				(float)drawsection.drawline[i].r, (float)drawsection.drawline[i].g, (float)drawsection.drawline[i].b, (float)drawsection.drawline[i].a);//(float)drawsection.drawline[i].a);
		}

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
