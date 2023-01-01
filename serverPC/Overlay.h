#pragma once

#include <Windows.h>
#include <iostream>
#include <d2d1.h>
#include <dwrite.h>
#include <dwmapi.h>

#include <atlconv.h>
#include <atlbase.h>

#include "SectionDef.h"

class Overlay
{
public:
	void init();
	void draw(const SharedSection& drawsection);

	void DrawString(std::string str, float fontSize, float x, float y, float r, float g, float b, float a);
	void DrawBox(float x, float y, float width, float height, float thickness, float r, float g, float b, float a, bool filled);
	void DrawLine(float x1, float y1, float x2, float y2, float thickness, float r, float g, float b, float a);
	void DrawCircle(float x, float y, float radius, float thickness, float r, float g, float b, float a, bool filled);
	void DrawEllipse(float x, float y, float width, float height, float thickness, float r, float g, float b, float a, bool filled);
	void Draw_Edge_String(std::string str, float fontSize, float x, float y, float r, float g, float b, float a);

	int aa = 0;

private:
	ID2D1Factory* factory;
	ID2D1HwndRenderTarget* target;
	ID2D1SolidColorBrush* solid_brush;
	IDWriteFactory* w_factory;
	IDWriteTextFormat* w_format;
	IDWriteTextLayout* w_layout;

	HWND hwnd;
};

