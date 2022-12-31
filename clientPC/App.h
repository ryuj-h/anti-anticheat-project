#pragma once
#include "CURLWrapper.hpp"

class App
{
public:
	void init();
	void tick();

private:
	CURLWrapper* m_CURL;
};

