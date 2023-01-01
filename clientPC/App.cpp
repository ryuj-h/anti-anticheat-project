#include "App.h"

void App::Init() {
	m_CURL = new CURLWrapper();
	dr = new hkdrv();

	for (;;){
		g_pid = dr->GetPid((char*)"TslGame.exe", 1);
		if (g_pid != 0)
			break;
		Sleep(1000);
	}
	for (;;){
		g_base = dr->GetBaseAddress(g_pid);
		if (g_pid != 0)
			break;
		Sleep(1000);
	}

	InitDecrypt();
	UpdatePtr();

	g_gnames = FindGnames();
	PrintPtr();
	CatchNames();

}

void App::Tick()
{
	json data;
	//data["status"] = std::string("Hello");
	InitDrawSection();

	MainTick();

	data["ln"] = json::array();//lines
	for (int i = 0; i < drawsection.linenum; i++) {
		data["ln"].emplace_back(json::object({
				{"x1", (int)drawsection.drawline[i].x1},
				{"y1", (int)drawsection.drawline[i].y1},
				{"x2", (int)drawsection.drawline[i].x2},
				{"y2", (int)drawsection.drawline[i].y2},
				{"th", (int)drawsection.drawline[i].thickness},
				{"r", (int)drawsection.drawline[i].r},
				{"g", (int)drawsection.drawline[i].g},
				{"b", (int)drawsection.drawline[i].b},
				{"a",(int)drawsection.drawline[i].a}
			}));
	}
	
	if (m_CURL->getReadyState()){
		auto dd = data.dump();
		m_CURL->sendData(dd);
	}
}

void App::InitDrawSection()
{
	drawsection.drawboxnum = 0;
	drawsection.linenum = 0;
	drawsection.stringnum = 0;
}

void App::InitDecrypt(){
	int64_t DecryptPtr;
	dr->RPM(g_pid, &DecryptPtr, g_base + ft_decrypt, 8);
	while (!DecryptPtr){
		dr->RPM(g_pid, &DecryptPtr, g_base + ft_decrypt, 8);
		Sleep(1000);
	}
	int32_t Tmp1Add;
	dr->RPM(g_pid, &Tmp1Add, DecryptPtr + 3, 4);

	Tmpadd = Tmp1Add + DecryptPtr + 7;
	unsigned char ShellcodeBuff[1024] = { NULL };
	ShellcodeBuff[0] = 0x90;
	ShellcodeBuff[1] = 0x90;

	dr->RPM(g_pid, &ShellcodeBuff[2], DecryptPtr, sizeof(ShellcodeBuff) - 2);

	ShellcodeBuff[2] = 0x48;
	ShellcodeBuff[3] = 0x8B;
	ShellcodeBuff[4] = 0xC1;
	ShellcodeBuff[5] = 0x90;
	ShellcodeBuff[6] = 0x90;
	ShellcodeBuff[7] = 0x90;
	ShellcodeBuff[8] = 0x90;
	fnDecryptFunctoin = (DecryptFunctoin)VirtualAlloc(NULL, sizeof(ShellcodeBuff) + 4, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	RtlCopyMemory((LPVOID)fnDecryptFunctoin, (LPVOID)ShellcodeBuff, sizeof(ShellcodeBuff));

}

UINT64 App::Decryptptr(UINT64 Value){
	return DecryptData(Value);
}

void App::UpdatePtr()
{
	dr->RPM(g_pid, &g_world, g_base + o_UWorld, 8);
	g_world = Decryptptr(g_world);

	dr->RPM(g_pid, &g_persistentlevel, g_world + o_Level, 8);
	g_persistentlevel = Decryptptr(g_persistentlevel);

	dr->RPM(g_pid, &g_actorArray, g_persistentlevel + o_Actor, 8);
	g_actorArray = Decryptptr(g_actorArray);

	dr->RPM(g_pid, &g_actor, g_actorArray, 8);

	dr->RPM(g_pid, &g_gameinstance, g_world + o_GameInstance, 8);
	g_gameinstance = Decryptptr(g_gameinstance);

	dr->RPM(g_pid, &g_localplayer, g_gameinstance + o_LocalPlayerArray, 8);
	dr->RPM(g_pid, &g_localplayer, g_localplayer, 8);
	g_localplayer = Decryptptr(g_localplayer);

	dr->RPM(g_pid, &g_playercontroller, g_localplayer + o_PlayerController, 8);
	g_playercontroller = Decryptptr(g_playercontroller);

	uint64_t pawnread;
	dr->RPM(g_pid, &pawnread, g_playercontroller + oPawn, 8);
	if (pawnread)
		g_aPawn = Decryptptr(pawnread);

	dr->RPM(g_pid, &g_cameramanager, g_playercontroller + o_Cameramanager, 8);



}

void App::PrintPtr()
{
	std::cout << "[+] pid                " << dec << g_pid << endl;
	std::cout << "[+] base               0x" << hex << g_base << endl;
	std::cout << "[+] world              0x" << hex << g_world << endl;
	std::cout << "[+] level              0x" << hex << g_persistentlevel << endl;
	std::cout << "[+] actor              0x" << hex << g_actor << endl;
	std::cout << "[+] gameinstance       0x" << hex << g_gameinstance << endl;
	std::cout << "[+] localplayer        0x" << hex << g_localplayer << endl;
	std::cout << "[+] playercontroller   0x" << hex << g_playercontroller << endl;
	std::cout << "[+] g_aPawn			 0x" << hex << g_aPawn << endl;
	std::cout << "[+] cameramanager      0x" << hex << g_cameramanager << endl;
	std::cout << "[+] gnames			 0x" << hex << g_gnames << endl;
}


std::string App::GetNameFromIdUsingGName(uint64_t GN, int ID)
{
	uint64_t fNamePtr;
	dr->RPM(g_pid, &fNamePtr, GN + ((int(ID / o_GNamesChunkSize)) * 8), 8);
	uint64_t fName;
	dr->RPM(g_pid, &fName, fNamePtr + ((int(ID % o_GNamesChunkSize)) * 8), 8);

	char buffer[64];
	dr->RPM(g_pid, buffer, fName + 0x10, 64);

	std::string str(buffer);
	return str;
}


uint64_t App::FindGnames()
{
	uint64_t gnames;
	dr->RPM(g_pid, &gnames, g_base + o_Gnames, 8);
	gnames = Decryptptr(gnames);

	uint64_t v11;
	dr->RPM(g_pid, &v11, gnames, 8);
	v11 = Decryptptr(v11);

	uint64_t v12;
	dr->RPM(g_pid, &v12, v11, 8);
	v12 = Decryptptr(v12);

	uint64_t v13;
	dr->RPM(g_pid, &v13, v12, 8);
	v13 = Decryptptr(v13);

	uint64_t tmp = v13;
	for (int i = 0; i < 0xFFFFFF; i++)
	{
		if (i == 0xFFFFFE)
		{
			std::cout << "load failed" << endl;
			system("pause");
		}
		gnames = tmp + (i * 0x10);
		if (//GetNameFromIdUsingGName(gnames, 1) == "ByteProperty" /*&& GetNameFromIdUsingGName(gnames, 2) == "IntProperty" */
			/*&& */
			GetNameFromIdUsingGName(gnames, 3) == std::string("BoolProperty"))
		{
			return gnames;
			break;
		}
		gnames = tmp - (i * 0x10);
		if (//GetNameFromIdUsingGName(gnames, 1) == "ByteProperty" /*&& GetNameFromIdUsingGName(gnames, 2) == "IntProperty" */
			/*&& */
			GetNameFromIdUsingGName(gnames, 3) == std::string("BoolProperty"))
		{
			return gnames;
			break;
		}
	}
	return gnames;
}

bool App::isCatchnamedone()
{
	bool result = true;

	if (!PlayerFemale_A_C)
		result = false;
	if (!PlayerMale_A_C)
		result = false;
	if (!RegistedPlayer)
		result = false;
	if (!UltAIPawn_Base_Male)
		result = false;
	if (!UltAIPawn_Base_Female)
		result = false;

	return result;
}
bool App::CatchNames()
{
	bool debug = false;
	bool result = false;
	string name = "";
	for (int i = 0; i < 0xfffff; i++)
	{
		if (isCatchnamedone())
		{
			if (result == false)
			{
				std::cout << "CatchName Almost Done" << endl;
			}
			result = true;

			//break;
		}
		name = GetNameFromIdUsingGName(g_gnames, i);
		if (name == "PlayerFemale_A_C")
		{
			if (debug)
				std::cout << "[catched]" << name << endl;
			PlayerFemale_A_C = i;
		}
		else if (name == "PlayerMale_A_C")
		{
			if (debug)
				std::cout << "[catched]" << name << endl;
			PlayerMale_A_C = i;
		}
		else if (name == "RegistedPlayer")
		{
			if (debug)
				std::cout << "[catched]" << name << endl;
			RegistedPlayer = i;
		}
		else if (name == "UltAIPawn_Base_Male")
		{
			if (debug)
				std::cout << "[catched]" << name << endl;
			UltAIPawn_Base_Male = i;
		}
		else if (name == "UltAIPawn_Base_Female")
		{
			if (debug)
				std::cout << "[catched]" << name << endl;
			UltAIPawn_Base_Female = i;
		}
		else if (name == "AIPawn_Base_C")
			//if (AIPawn_Base_C == 0)
			AIPawn_Base_C = i;
		else if (name == "AIPawn_Base_Female_C")
			//if (AIPawn_Base_Female_C == 0)
			AIPawn_Base_Female_C = i;
		else if (name == "AIPawn_Base_Male_C")
			//if (AIPawn_Base_Male_C == 0)
			AIPawn_Base_Male_C = i;
		else if (name == "UltAIPawn_Base_Male_C")
			UltAIPawn_Base_Male_C = i;
		else if (name == "UltAIPawn_Base_Female_C")
			UltAIPawn_Base_Female_C = i;

	}

	if (!result)
	{
		MessageBox(NULL, L"Turn on the program after matching", L"ERROR", MB_OK);
		exit(true);
	}
	std::cout << "CatchName Ended" << endl;
	return result;
}

void App::snapShotPlayers()
{
	Players.clear();
	Bots.clear();

	int Actorcount;
	dr->RPM(g_pid, &Actorcount, g_actorArray + 0x8, 4);

	if (Actorcount > 20000)
		Actorcount = 20000;

	//g_playeraddress = g_savedaPawn;
	//g_pawnmesh = Getlocalmesh();
	for (int i = 0; i < Actorcount; i++)
	{
		uint64_t entity;
		dr->RPM(g_pid, &entity, g_actor + (0x8 * i), 8);
		//if (entity == g_savedaPawn)
		//	continue;
		if (entity == 0)
			continue;


		int id;
		dr->RPM(g_pid, &id, entity + oActor_ID, 4);
		id = decryptid(id);

		if (id == PlayerFemale_A_C ||
			id == PlayerMale_A_C ||
			id == RegistedPlayer)
		{
			Players.push_back(entity);
		}

		//bots
		if ((AIPawn_Base_C != 0 && id == AIPawn_Base_C) ||
			(AIPawn_Base_Female_C != 0 && id == AIPawn_Base_Female_C) ||
			(AIPawn_Base_Male_C != 0 && id == AIPawn_Base_Male_C) ||
			(UltAIPawn_Base_Male_C != 0 && id == UltAIPawn_Base_Male_C) ||
			(UltAIPawn_Base_Female_C != 0 && id == UltAIPawn_Base_Female_C) ||
			(UltAIPawn_Base_Male != 0 && id == UltAIPawn_Base_Male) ||
			(UltAIPawn_Base_Female != 0 && id == UltAIPawn_Base_Female)
			)
		{
			Players.push_back(entity);
			//Bots.push_back(entity);
		}
	}
}

unsigned int App::decryptid(const unsigned int& v13)
{
	unsigned int decid = 0;
	decid = __ROR4__(v13 ^ 0xE80BFFDD, 1) ^ (__ROR4__(v13 ^ 0xE80BFFDD, 1) << 16) ^ 0xD617DAC3;
	//__ROL4__(v13 ^ 0x3D4D27BF, 5) ^ (__ROL4__(v13 ^ 0x3D4D27BF, 5) << 16) ^ 0xF21AA6A6;
	return decid;
}

void App::GetCameraCache()
{
	dr->RPM(g_pid, &CameraCache.POV.FOV, g_cameramanager + o_camera_fov, 4);
	dr->RPM(g_pid, &CameraCache.POV.Location, g_cameramanager + o_camera_location, 12);
	dr->RPM(g_pid, &CameraCache.POV.Rotation, g_cameramanager + o_camera_ratation, 12);
}

DDMATRIX App::Matrix(Vector3 rot, Vector3 origin = Vector3(0, 0, 0))
{
	float radPitch = (rot.x * float(M_PI) / 180.f);
	float radYaw = (rot.y * float(M_PI) / 180.f);
	float radRoll = (rot.z * float(M_PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	DDMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;

	return matrix;
}
Vector3 App::WorldToScreenAim(Vector3 WorldLocation, Vector3 Rot)
{
	Vector3 Screenlocation = Vector3(0, 0, 0);

	auto POV = CameraCache.POV;
	Vector3 Rotation = Rot; // FRotator

	DDMATRIX tempMatrix = Matrix(Rotation); // Matrix

	Vector3 vAxisX, vAxisY, vAxisZ;

	vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	Vector3 vDelta = WorldLocation - POV.Location;
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	float FovAngle = POV.FOV;
	float ScreenCenterX = s_width / 2.0f;
	float ScreenCenterY = s_height / 2.0f;

	Screenlocation.x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;
	Screenlocation.y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;

	return Screenlocation;
}

Vector3 App::WorldToScreen(Vector3 WorldLocation)
{
	Vector3 Screenlocation = Vector3(0, 0, 0);

	auto POV = CameraCache.POV;
	Vector3 Rotation = POV.Rotation; // FRotator

	DDMATRIX tempMatrix = Matrix(Rotation); // Matrix

	Vector3 vAxisX, vAxisY, vAxisZ;

	vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	Vector3 vDelta = WorldLocation - POV.Location;
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	float FovAngle = POV.FOV;
	float ScreenCenterX = s_width / 2.0f;
	float ScreenCenterY = s_height / 2.0f;

	Screenlocation.x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;
	Screenlocation.y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;

	return Screenlocation;
}

DDMATRIX App::MatrixMultiplication(DDMATRIX pM1, DDMATRIX pM2)
{
	DDMATRIX pOut;
	pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
	pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
	pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
	pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
	pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
	pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
	pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
	pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
	pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
	pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
	pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
	pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
	pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
	pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
	pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
	pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

	return pOut;
}

FTransform App::GetBoneIndex(uint64_t mesh, int index)
{
	uint64_t bonearray;
	dr->RPM(g_pid, &bonearray, mesh + oBoneIndex, 8);
	FTransform ftransform;
	dr->RPM(g_pid, &ftransform, bonearray + (index * 0x30), sizeof(FTransform));
	return  ftransform;
}

Vector3 App::GetBoneWithRotation(uint64_t mesh, int id)
{
	FTransform bone = GetBoneIndex(mesh, id);

	FTransform ComponentToWorld;
	dr->RPM(g_pid, &ComponentToWorld, mesh + oComponentToWorld, sizeof(FTransform));
	DDMATRIX Matrix;
	Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());
	return Vector3(Matrix._41, Matrix._42, Matrix._43);
}


list<int> fupper_part = { FemaleBones::fneck_01, FemaleBones::fHead, FemaleBones::fforehead };
list<int> fright_arm = { FemaleBones::fneck_01, FemaleBones::fupperarm_r, FemaleBones::flowerarm_r, FemaleBones::fhand_r };
list<int> fleft_arm = { FemaleBones::fneck_01, FemaleBones::fupperarm_l, FemaleBones::flowerarm_l, FemaleBones::fhand_l };
list<int> fspine = { FemaleBones::fneck_01, FemaleBones::fspine_02,FemaleBones::fspine_01, FemaleBones::fpelvis };
list<int> flower_right = { FemaleBones::fpelvis, FemaleBones::fthigh_r, FemaleBones::fcalf_r, FemaleBones::ffoot_r };
list<int> flower_left = { FemaleBones::fpelvis, FemaleBones::fthigh_l, FemaleBones::fcalf_l, FemaleBones::ffoot_l };

list<list<int>> fskeleton = { fupper_part, fright_arm, fleft_arm, fspine, flower_right, flower_left };

void App::skel_draw(Vector3 p1, Vector3 c1, bool isvisible)
{
}


void App::DrawSkeleton_manual(uint64_t mesh, bool isvisible)
{
}



void App::Playerloop()
{
	for (auto entity : Players){
		if (entity == g_savedaPawn)
		{
			//continue;
		}

		uint64_t rootcomp;
		dr->RPM(g_pid, &rootcomp, entity + oActor_Rootcomp, 8);
		rootcomp = Decryptptr(rootcomp);


		Vector3 position;
		dr->RPM(g_pid, &position, rootcomp + oRootcompPosition, 12);

		Vector3 position2 = position;
		position2.z = position2.z + 180.f;

		Vector3 wts = WorldToScreen(position);
		Vector3 wts2 = WorldToScreen(position2);

		//DrawLine(wts.x, wts.y, wts2.x, wts2.y, 1, 1, 1, 1, 1);
		//cout << wts.x << " : " << wts.y << endl;
		uint64_t mesh;
		dr->RPM(g_pid, &mesh, entity + oMesh, 8);
		if (mesh < 0xfffff || mesh > 0xfffffffffffffff)
			continue;


		float distance = position.Distance(CameraCache.POV.Location) / 100.0f;

		if (distance <= 100.f)
		{
			Vector3 fforepos = GetBoneWithRotation(mesh, fforehead);
			Vector3 fHeadpos = GetBoneWithRotation(mesh, fHead);
			Vector3 fneck_01pos = GetBoneWithRotation(mesh, fneck_01);
			

			Vector3 fupperarm_r_pos = GetBoneWithRotation(mesh, fupperarm_r);
			Vector3 flowerarm_r_pos = GetBoneWithRotation(mesh, flowerarm_r);
			Vector3 fhand_r_pos = GetBoneWithRotation(mesh, fhand_r);

			Vector3 fupperarm_l_pos = GetBoneWithRotation(mesh, fupperarm_l);
			Vector3 flowerarm_l_pos = GetBoneWithRotation(mesh, flowerarm_l);
			Vector3 fhand_l_pos = GetBoneWithRotation(mesh, fhand_l);
			
			
			Vector3 fpelvis_pos = GetBoneWithRotation(mesh, fpelvis);
			Vector3 fspine_02_pos = GetBoneWithRotation(mesh, fspine_02);
			Vector3 fspine_01_pos = GetBoneWithRotation(mesh, fspine_01);


			Vector3 fthigh_r_pos = GetBoneWithRotation(mesh, fthigh_r);
			Vector3 fcalf_r_pos = GetBoneWithRotation(mesh, fcalf_r);
			Vector3 ffoot_r_pos = GetBoneWithRotation(mesh, ffoot_r);


			Vector3 fthigh_l_pos = GetBoneWithRotation(mesh, fthigh_l);
			Vector3 fcalf_l_pos = GetBoneWithRotation(mesh, fcalf_l);
			Vector3 ffoot_l_pos = GetBoneWithRotation(mesh, ffoot_l);

			fforepos = WorldToScreen(fforepos);
			fHeadpos = WorldToScreen(fHeadpos);
			fneck_01pos = WorldToScreen(fneck_01pos);

			fupperarm_r_pos = WorldToScreen(fupperarm_r_pos);
			flowerarm_r_pos = WorldToScreen(flowerarm_r_pos);
			fhand_r_pos = WorldToScreen(fhand_r_pos);

			fupperarm_l_pos = WorldToScreen(fupperarm_l_pos);
			flowerarm_l_pos = WorldToScreen(flowerarm_l_pos);
			fhand_l_pos = WorldToScreen(fhand_l_pos);

			fspine_02_pos = WorldToScreen(fspine_02_pos);
			fspine_01_pos = WorldToScreen(fspine_01_pos);
			fpelvis_pos = WorldToScreen(fpelvis_pos);

			fthigh_r_pos = WorldToScreen(fthigh_r_pos);
			fcalf_r_pos = WorldToScreen(fcalf_r_pos);
			ffoot_r_pos = WorldToScreen(ffoot_r_pos);

			fthigh_l_pos = WorldToScreen(fthigh_l_pos);
			fcalf_l_pos = WorldToScreen(fcalf_l_pos);
			ffoot_l_pos = WorldToScreen(ffoot_l_pos);


			DrawLine(fforepos.x, fforepos.y, wts.x, wts.y, 3, 1, 1, 1, 1);

			//continue;

			DrawLine(fforepos.x, fforepos.y, fHeadpos.x, fHeadpos.y, 3, 1, 1, 1, 1);
			DrawLine(fHeadpos.x, fHeadpos.y, fneck_01pos.x, fneck_01pos.y, 3, 1, 1, 1, 1);

			DrawLine(fneck_01pos.x, fneck_01pos.y, fupperarm_r_pos.x, fupperarm_r_pos.y, 3, 1, 1, 1, 1);
			DrawLine(fupperarm_r_pos.x, fupperarm_r_pos.y, flowerarm_r_pos.x, flowerarm_r_pos.y, 3, 1, 1, 1, 1);
			DrawLine(flowerarm_r_pos.x, flowerarm_r_pos.y, fhand_r_pos.x, fhand_r_pos.y, 3, 1, 1, 1, 1);

			DrawLine(fneck_01pos.x, fneck_01pos.y, fupperarm_l_pos.x, fupperarm_l_pos.y, 3, 1, 1, 1, 1);
			DrawLine(fupperarm_l_pos.x, fupperarm_l_pos.y, flowerarm_l_pos.x, flowerarm_l_pos.y, 3, 1, 1, 1, 1);
			DrawLine(flowerarm_l_pos.x, flowerarm_l_pos.y, fhand_l_pos.x, fhand_l_pos.y, 3, 1, 1, 1, 1);

			DrawLine(fneck_01pos.x, fneck_01pos.y, fspine_02_pos.x, fspine_02_pos.y, 3, 1, 1, 1, 1);
			DrawLine(fspine_02_pos.x, fspine_02_pos.y, fspine_01_pos.x, fspine_01_pos.y, 3, 1, 1, 1, 1);
			DrawLine(fspine_01_pos.x, fspine_01_pos.y, fpelvis_pos.x, fpelvis_pos.y, 3, 1, 1, 1, 1);

			DrawLine(fpelvis_pos.x, fpelvis_pos.y, fthigh_r_pos.x, fthigh_r_pos.y, 3, 1, 1, 1, 1);
			DrawLine(fthigh_r_pos.x, fthigh_r_pos.y, fcalf_r_pos.x, fcalf_r_pos.y, 3, 1, 1, 1, 1);
			DrawLine(fcalf_r_pos.x, fcalf_r_pos.y, ffoot_r_pos.x, ffoot_r_pos.y, 3, 1, 1, 1, 1);

			DrawLine(fpelvis_pos.x, fpelvis_pos.y, fthigh_l_pos.x, fthigh_l_pos.y, 3, 1, 1, 1, 1);
			DrawLine(fthigh_l_pos.x, fthigh_l_pos.y, fcalf_l_pos.x, fcalf_l_pos.y, 3, 1, 1, 1, 1);
			DrawLine(fcalf_l_pos.x, fcalf_l_pos.y, ffoot_l_pos.x, ffoot_l_pos.y, 3, 1, 1, 1, 1);

		}



		//DrawSkeleton_manual(mesh, false);
	}
}


void App::MainTick()
{
	//int FPS = 240;
	//auto time_between_frames = std::chrono::microseconds(std::chrono::seconds(1)) / FPS;
	//auto target_tp = std::chrono::steady_clock::now();


	//target_tp += time_between_frames;
	//std::this_thread::sleep_until(target_tp);
	fps++;
	SCenter.x = s_width / 2;
	SCenter.y = s_height / 2;
	SCenter.z = 0;


	uint64_t t = GetTickCount64();
	if (t - lastcheck > 1000)
	{
		cout <<dec << fps << endl;
		fps = 0;
		UpdatePtr();
		snapShotPlayers();
		lastcheck = t;
	}

	GetCameraCache();
	Playerloop();
}



void App::DrawString(std::string str, float fontSize, float x, float y, float r, float g, float b, float a)
{
}

void App::DrawBox(float x, float y, float width, float height, float thickness, float r, float g, float b, float a, bool filled)
{
}

void App::DrawLine(double x1, double y1, double x2, double y2, double thickness, double r, double g, double b, double a)
{

	if (x1 < 0 && x1 > s_width && y1 > 0 && y1 <= s_height)
		return;
	if (x2 < 0 && x2 > s_width && y2 > 0 && y2 <= s_height)
		return;
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
