#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "CURLWrapper.hpp"
#include "SectionDef.h"
#include "hookctrl.h"
#include "offsets.h"
#include "V3.h"
#include "def.h"

#include <vector>
#include <chrono>
#include <thread>
#include <iostream>
#include <list>

class App
{
public:
	void Init();
	void Tick();
	void InitDrawSection();
	void InitDecrypt();
	UINT64 Decryptptr(UINT64 Value);


	void UpdatePtr();
	void PrintPtr();
	std::string GetNameFromIdUsingGName(uint64_t GN, int ID);
	uint64_t FindGnames();
	bool isCatchnamedone();
	bool CatchNames();
	void snapShotPlayers();
	unsigned int decryptid(const unsigned int& v13);

	void GetCameraCache();
	DDMATRIX Matrix(Vector3 rot, Vector3 origin);
	Vector3 WorldToScreenAim(Vector3 WorldLocation, Vector3 Rot);
	Vector3 WorldToScreen(Vector3 WorldLocation);
	DDMATRIX MatrixMultiplication(DDMATRIX pM1, DDMATRIX pM2);
	FTransform GetBoneIndex(uint64_t mesh, int index);
	Vector3 GetBoneWithRotation(uint64_t mesh, int id);
	void skel_draw(Vector3 p1, Vector3 c1, bool isvisible);
	void DrawSkeleton_manual(uint64_t mesh, bool isvisible);
	void Playerloop(json& data);
	void MainTick(json& data);

	void DrawString(std::string str, float fontSize, float x, float y, float r, float g, float b, float a);
	void DrawBox(float x, float y, float width, float height, float thickness, float r, float g, float b, float a, bool filled);
	void DrawLine(double x1, double y1, double x2, double y2, double thickness, double r, double g, double b, double a);


private:
	CURLWrapper* m_CURL;
	SharedSection drawsection;
	hkdrv* dr;

	uint64_t Tmpadd;
#define DecryptData(argv)	fnDecryptFunctoin(Tmpadd, argv)
	typedef int64_t(__fastcall* DecryptFunctoin)(uintptr_t key, uintptr_t argv);
	DecryptFunctoin fnDecryptFunctoin = NULL;

	DWORD		g_pid;
	uint64_t	g_base;
	uint64_t	g_world;
	uint64_t	g_persistentlevel;
	uint64_t	g_actorArray;
	uint64_t	g_actor;
	uint64_t	g_gameinstance;
	uint64_t	g_localplayer;
	uint64_t	g_playercontroller;
	uint64_t	g_cameramanager;
	uint64_t	g_aPawn;
	uint64_t	g_SpectorPawn;
	uint64_t	g_savedaPawn;
	uint64_t	g_tslgamestate;
	uint64_t	g_gnames;

	int PlayerFemale_A_C = 0;
	int PlayerMale_A_C = 0;

	int RegistedPlayer = 0;

	int	AIPawn_Base_C = 0;
	int	AIPawn_Base_Female_C = 0;
	int	AIPawn_Base_Male_C = 0;
	int	UltAIPawn_Base_Male_C = 0;
	int	UltAIPawn_Base_Female_C = 0;

	int UltAIPawn_Base_Male = 0;
	int UltAIPawn_Base_Female = 0;

	int s_width = GetSystemMetrics(SM_CXSCREEN);
	int s_height = GetSystemMetrics(SM_CYSCREEN);
	Vector3 SCenter;

	uint64_t lastcheck = 0;

	int fps = 0;

	std::vector<uint64_t> Players;
	std::vector<uint64_t> Bots;

	FCameraCacheEntry CameraCache;
};

