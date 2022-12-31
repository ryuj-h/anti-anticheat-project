#pragma once
#include <Windows.h>
#include <iostream>

typedef struct
{
	float x1;
	float y1;
	float x2;
	float y2;

	float thickness;
	
	float r;
	float g;
	float b;
	float a;

}DrawLine;

typedef struct
{
	float x;
	float y;

	ULONG color;
	float fontsize;
	bool center;
	char text[100];
}DrawString;

typedef struct
{
	float x;
	float y;
	float w;
	float h;
	ULONG color;
	bool filled;
}DrawBox;

typedef struct
{
	int linenum;
	int stringnum;
	int drawboxnum;

	DrawLine drawline[10000];
	DrawString drawstring[1000];
	DrawBox drawbox[1000];
}SharedSection;