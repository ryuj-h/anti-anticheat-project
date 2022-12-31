#include "App.h"

void App::init() {
	m_CURL = new CURLWrapper();

}

void App::tick()
{
	json data;
	data["status"] = std::string("Hello");



	if (m_CURL->getReadyState()) //&& !(data["players"].empty()))
	{
		auto dd = data.dump();
		m_CURL->sendData(dd);
	}
	Sleep(100);
}
