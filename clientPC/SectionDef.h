#pragma once
#include <Windows.h>
#include <iostream>

typedef struct
{
	double x1;
	double y1;
	double x2;
	double y2;

	double thickness;

	double r;
	double g;
	double b;
	double a;

}DrawLine;

typedef struct
{
	double x;
	double y;

	ULONG color;
	double fontsize;
	bool center;
	char text[100];
}DrawString;

typedef struct
{
	double x;
	double y;
	double w;
	double h;
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