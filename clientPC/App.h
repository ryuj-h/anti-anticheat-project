#pragma once
#include "CURLWrapper.hpp"
#include "SectionDef.h"

class App
{
public:
	void init();
	void tick();
	void Initdrawsection();
	void DrawString(std::string str, float fontSize, float x, float y, float r, float g, float b, float a);
	void DrawBox(float x, float y, float width, float height, float thickness, float r, float g, float b, float a, bool filled);
	void DrawLine(double x1, double y1, double x2, double y2, double thickness, double r, double g, double b, double a);
private:
	CURLWrapper* m_CURL;
	SharedSection drawsection;
};

