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
	int visible;
	int distance;

	int _minimapx;
	int _minimapy;

	int _basex;
	int _basey;

	int _15x;
	int _15y;
	int _6x;
	int _6y;
	int _5x;
	int _5y;
	int _115x;
	int _115y;
	int _116x;
	int _116y;
	int _117x;
	int _117y;
	int _88x;
	int _88y;
	int _89x;
	int _89y;
	int _90x;
	int _90y;
	int _1x;
	int _1y;
	int _3x;
	int _3y;
	int _2x;
	int _2y;
	int _174x;
	int _174y;
	int _175x;
	int _175y;
	int _176x;
	int _176y;
	int _168x;
	int _168y;
	int _169x;
	int _169y;
	int _170x;
	int _170y;

}Player;

typedef struct
{
	int linenum;
	int stringnum;
	int drawboxnum;
	int playernum;

	DrawLine drawline[10000];
	DrawString drawstring[1000];
	DrawBox drawbox[1000];
	Player player[1000];
	
	int latency;
}SharedSection;