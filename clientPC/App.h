#pragma once
#include "CURLWrapper.hpp"

class App
{
public:
	CURLWrapper* m_CURL;

	void init();
	void tick();
};

