#include "App.h"

void App::init() {
	m_CURL = new CURLWrapper();

}

void App::tick()
{
	json data;
	data["status"] = std::string("Hello");
	Initdrawsection();

	DrawLine(100, 100, 100, 200, 1, 255, 0.f, 0.f, 255);
	DrawLine(200, 200, 200, 300, 1, 0.f, 255, 0.f, 255);
	DrawLine(300, 300, 300, 400, 1, 0.f, 0.f, 255, 255);




	data["lines"] = json::array();
	for (int i = 0; i < drawsection.linenum; i++) {
		data["lines"].emplace_back(json::object({
				{"x1", drawsection.drawline[i].x1},
				{"y1", drawsection.drawline[i].y1},
				{"x2", drawsection.drawline[i].x2},
				{"y2", drawsection.drawline[i].y2},
				{"thickness", drawsection.drawline[i].thickness},
				{"r", drawsection.drawline[i].r},
				{"g", drawsection.drawline[i].g},
				{"b", drawsection.drawline[i].b},
				{"a",drawsection.drawline[i].a}
			}));
	}
	
	if (m_CURL->getReadyState()) //&& !(data["players"].empty()))
	{
		auto dd = data.dump();
		m_CURL->sendData(dd);
	}
	//Sleep(10);
}

void App::Initdrawsection()
{
	drawsection.drawboxnum = 0;
	drawsection.linenum = 0;
	drawsection.stringnum = 0;
}

void App::DrawString(std::string str, float fontSize, float x, float y, float r, float g, float b, float a)
{
}

void App::DrawBox(float x, float y, float width, float height, float thickness, float r, float g, float b, float a, bool filled)
{
}

void App::DrawLine(double x1, double y1, double x2, double y2, double thickness, double r, double g, double b, double a)
{
	drawsection.drawline[drawsection.linenum].r = r;
	drawsection.drawline[drawsection.linenum].g = g;
	drawsection.drawline[drawsection.linenum].b = b;
	drawsection.drawline[drawsection.linenum].a = a;
	drawsection.drawline[drawsection.linenum].x1 = x1;
	drawsection.drawline[drawsection.linenum].y1 = y1;
	drawsection.drawline[drawsection.linenum].x2 = x2;
	drawsection.drawline[drawsection.linenum].y2 = y2;
	drawsection.drawline[drawsection.linenum].thickness = thickness;
	drawsection.linenum++;
}
