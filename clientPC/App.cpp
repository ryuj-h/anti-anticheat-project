#include "App.h"

std::map <uint64_t, EnemyData*> g_EnemyInfo;
std::map <uint64_t, std::queue<EnemyDataPast*>*> g_EnemyPastInfo;

void App::Init() {
	system("pause");
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
	FindGnames();
	PrintPtr();
	CatchNames();
}


void App::Tick()
{
	json data;
	InitDrawSection();

	MainTick(data);

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
	
	ULONG64 tickcount = GetTickCount64();


	uint64_t t = GetTickCount64();
	if (t - lastcheck > 1000) {
		fps = 0;
		UpdatePtr();
		snapShotPlayers();
		lastcheck = t;
	}


	data["lat"] = connectiontimePlus;




	/*while (true) {
		ULONG64 tickcount2 = GetTickCount64();
		if (tickcount2 - tickcount >= 5)
			break;
	}*/

	key_ctrl = dr->GetKeystate(VK_LCONTROL);
	key_O = dr->GetKeystate(0x4F);
	key_P = dr->GetKeystate(0x50);
	
	if (key_ctrl && key_O) {
		connectiontimePlus--;
		if (connectiontimePlus < 0)
			connectiontimePlus = 0;
		Sleep(100);
	}
	else if (key_ctrl && key_P) {
		connectiontimePlus++;
		if (connectiontimePlus > 100)
			connectiontimePlus = 100;
		Sleep(100);
	}

	int ttime = connectiontimePlus;

	/*if (m_CURL->getReadyState()) {
		auto dd = data.dump();
		m_CURL->sendData(dd);//POST 요청
		ULONG64 tickcount3 = GetTickCount64();
	}*/

	
	jsonqueue.push(make_pair(t, data));

	ULONG64 tickcount2 = GetTickCount64();
	
	uint64_t ticktime;// = firstjson.first;//데이터가 처리될 때의 시간
	bool found = false;
	json jsondata;// = firstjson.second;//처리된 json 데이터

	while (true) {
		if (!jsonqueue.empty()) {
			auto firstjson = jsonqueue.front();
			ticktime = firstjson.first;
			if (tickcount2 - ticktime >= connectiontimePlus) {
				found = true;
				jsondata = firstjson.second;
				jsonqueue.pop();
			}
			else {
				break;
			}
		}
		else
			break;
	}
	if (found){
		if (m_CURL->getReadyState()) {
			auto dd = jsondata.dump();
			m_CURL->sendData(dd);//POST 요청
		}
	}
	/*if (!jsonqueue.empty()) {
		auto firstjson = jsonqueue.front();
		auto ticktime = firstjson.first;//데이터가 처리될 때의 시간
		auto jsondata = firstjson.second;//처리된 json 데이터

		if (tickcount2 - ticktime >=  connectiontimePlus){//일정 시간이 지났다면
			jsonqueue.pop();//큐에서 pop
			if (m_CURL->getReadyState()) {
				auto dd = jsondata.dump();
				m_CURL->sendData(dd);//POST 요청
				ULONG64 tickcount3 = GetTickCount64();
			}
		}
	}
	while (!jsonqueue.empty()) {//너무 오래 걸려 POST 요청이 지연되고 있다면 큐를 비워줌
		auto firstjson = jsonqueue.front();
		auto ticktime = firstjson.first;
		auto jsondata = firstjson.second;
		if (tickcount2 - ticktime > connectiontimePlus + 5){ 
			jsonqueue.pop();
		}
		else
			break;
	}*/
}

void App::InitDrawSection()
{
	drawsection.playernum = 0;
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

	uint64_t pawnread = 0;
	dr->RPM(g_pid, &pawnread, g_playercontroller + oPawn, 8);
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
			g_gnames = gnames;
			return gnames;
			break;
		}
		gnames = tmp - (i * 0x10);
		if (//GetNameFromIdUsingGName(gnames, 1) == "ByteProperty" /*&& GetNameFromIdUsingGName(gnames, 2) == "IntProperty" */
			/*&& */
			GetNameFromIdUsingGName(gnames, 3) == std::string("BoolProperty"))
		{
			g_gnames = gnames;
			return gnames;
			break;
		}
	}
	g_gnames = gnames;
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
	for (int i = 0; i < 0xfffff; i++){
		if (isCatchnamedone()){
			if (result == false){
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

	if (!result) {
		MessageBox(NULL, L"Turn on the program after matching", L"ERROR", MB_OK);
		system("pause");
		CatchNames();
		//exit(true);
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
	decid = __ROR4__(v13 ^ 0x1A018DF6, 10) ^ (__ROR4__(v13 ^ 0x1A018DF6, 10) << 16) ^ 0xF7978B7A;
	//__ROL4__(v13 ^ 0x3D4D27BF, 5) ^ (__ROL4__(v13 ^ 0x3D4D27BF, 5) << 16) ^ 0xF21AA6A6;
	return decid;
}

void App::GetHotkey()
{
	key_ctrl = dr->GetKeystate(VK_LCONTROL);


}

void App::SetPlustime()
{
	key_ctrl = dr->GetKeystate(VK_LCONTROL);
	key_O = dr->GetKeystate(0x4F);
	key_P = dr->GetKeystate(0x50);
	if (key_ctrl && key_O) {
		connectiontimePlus--;
		Sleep(100);
	}
	else if (key_ctrl && key_O) {
		connectiontimePlus++;
		Sleep(100);
	}
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



void App::Playerloop(json& data)
{
	if (g_aPawn) {
		int id;
		dr->RPM(g_pid, &id, g_aPawn + oActor_ID, 4);
		id = decryptid(id);

		if (id == PlayerFemale_A_C ||
			id == PlayerMale_A_C ||
			id == RegistedPlayer){
			playerentity = g_aPawn;
		}
		else {
			uint64_t ViewTarget = 0;
			dr->RPM(g_pid, &ViewTarget, g_cameramanager + oViewTarget, 8);
			if (ViewTarget){
				int iid = 0;
				dr->RPM(g_pid, &iid, ViewTarget + oActor_ID, 4);
				iid = decryptid(iid);

				if (iid == PlayerFemale_A_C ||
					iid == PlayerMale_A_C ||
					iid == RegistedPlayer){
					playerentity = ViewTarget;
				}
			}
		}
	}
	//playerentity = g_aPawn;
	Vector3 sway;
	Vector3 recoil;

	if (playerentity) {
		dr->RPM(g_pid, &playerteam, playerentity + oTeam, 4);
		dr->RPM(g_pid, &playermesh, playerentity + oMesh, 8);

		uint64_t playerrootcomp;
		dr->RPM(g_pid, &playerrootcomp, playerentity + oActor_Rootcomp, 8);
		playerrootcomp = Decryptptr(playerrootcomp);
		dr->RPM(g_pid, &playerPosition, playerrootcomp + oRootcompPosition, 12);
		Vector3 playerVelocity;
		dr->RPM(g_pid, &playerVelocity, playerrootcomp + oComponentVelocity, 12);

		static uint64_t lastchecktick;
		static Vector3 lastposition100;
		static Vector3 calcvelocity;
		uint64_t checktick = GetTickCount64();
		if (checktick - lastchecktick >= 200) {
			lastchecktick = checktick;

			calcvelocity = playerPosition - lastposition100;
			calcvelocity = calcvelocity * 5.f;
			lastposition100 = playerPosition;
			//cout << "read velocity : x : " << playerVelocity.x << "	y : " << playerVelocity.y << "	z : " << playerVelocity.z << endl;
			//cout << "calc velocity : x : " << calcvelocity.x << "	y : " << calcvelocity.y << "	z : " << calcvelocity.z <<endl<< endl;
		}
		// Get Weapon data

		uint64_t AnimScript;
		dr->RPM(g_pid, &AnimScript, playermesh + oMeshAnimScriptInstance, 8);

		dr->RPM(g_pid, &sway, AnimScript + ocontrol_rotation_cp, 12);

		
		dr->RPM(g_pid, &recoil, AnimScript + orecoil_ads_rotation_cp, 12);

		float lla;
		float lra;

		dr->RPM(g_pid, &lla, AnimScript + olean_left_alpha_cp, 4);
		dr->RPM(g_pid, &lra, AnimScript + olean_right_alpha_cp, 4);
		recoil.y += (lra - lla) * recoil.x / 3.0f;

		/*if (recoil.Size() > 0.25f) {
			recoil.y -= lla / 4.0f;
			recoil.y += lra / 4.0f;
		}*/

		sway.x += recoil.x;
		sway.y += recoil.y;
		sway.z += recoil.z;

		uint64_t pWeaponProcessor;
		dr->RPM(g_pid, &pWeaponProcessor, playerentity + oWeaponProcessor, 8);

		uint64_t pEquippedWeapons;
		dr->RPM(g_pid, &pEquippedWeapons, pWeaponProcessor + oEquippedWeapons, 8);

		char currentWeaponIdxtemp = 0;
		dr->RPM(g_pid, &currentWeaponIdxtemp, pWeaponProcessor + oCurrWeapoinIdx, sizeof(char));
		int currentWeaponIdx = currentWeaponIdxtemp;

		uint64_t pNowWeapon;
		dr->RPM(g_pid, &pNowWeapon, pEquippedWeapons + 8 * currentWeaponIdx, 8);

		uint64_t trajectory_weapon_data;
		dr->RPM(g_pid, &trajectory_weapon_data, pNowWeapon + oWeaponTrajectoryData, 8);

		float initial_speed = 0.0f;
		dr->RPM(g_pid, &initial_speed, trajectory_weapon_data + oTrajectoryConfig, 4);

		if (currentWeaponIdx >= -1 && currentWeaponIdx <= 4) {
			playerCurrGunIdx = currentWeaponIdx;
			if (initial_speed >= 50.f && initial_speed <= 1500.f) {
				playerGunSpeed = initial_speed;
			}
		}

		uint64_t VehicleRiderComponent = 0;
		dr->RPM(g_pid, &VehicleRiderComponent, playerentity + oVehicleRiderComponent, 8);

		uint64_t LastVehiclePawn = 0;
		dr->RPM(g_pid, &LastVehiclePawn, VehicleRiderComponent + oLastVehiclePawn, 8);

		int Seatindex = 0;
		dr->RPM(g_pid, &Seatindex, VehicleRiderComponent + oSeatIndex, 4);

		Vector3 ReplicateMovement;
		dr->RPM(g_pid, &ReplicateMovement, LastVehiclePawn + oReplicatedMovement, 12);
		//oReplicatedMovement
		if (false){//debug
			system("cls");
			std::cout << "[+] pWeaponProcessor: " << hex << pWeaponProcessor << endl;
			std::cout << "[+] pEquippedWeapons: " << hex << pEquippedWeapons << endl;
			std::cout << "[+] currentWeaponIdx: " << hex << currentWeaponIdx << endl;
			std::cout << "[+] pNowWeapon	  : " << pNowWeapon << endl;
			std::cout << "[+] trajectory_weapon_data: " << hex << trajectory_weapon_data << endl;
			std::cout << "[+] bulltet speed         : " << hex << initial_speed << endl;

			//brute force
			for (int i = 0; i < 0xFFF; i = i + 0x4) {
				uint64_t pEquippedWeaponsbf = 0x0;
				dr->RPM(g_pid, &pEquippedWeaponsbf, pWeaponProcessor + i, 8);

				uint64_t pNowWeaponbf;
				dr->RPM(g_pid, &pNowWeaponbf, pEquippedWeaponsbf + 8 * currentWeaponIdx, 8);

				uint64_t trajectory_weapon_databf;
				dr->RPM(g_pid, &trajectory_weapon_databf, pNowWeaponbf + oWeaponTrajectoryData, 8);

				for (int j = 0; j < 0x7ff; j = j + 0x2) {
					float initial_speedbf = 0.f;

					dr->RPM(g_pid, &initial_speedbf, trajectory_weapon_databf + j, 4);
					if (initial_speedbf == 880.f) {
						cout << hex << "oEquippedWeapons offset : 0x" << i << endl;
						cout << hex << "oTrajectoryConfig offset : 0x" << j << endl;
						break;
					}
				}
			}


			if (false)
			for (int i = 0; i < 0xFFF; i++)
			{
				initial_speed = 0.0f;
				dr->RPM(g_pid, &initial_speed, trajectory_weapon_data + i, 4);
				if (initial_speed == 880.f) {
					cout << hex << i << endl;
				}
			}
			Sleep(64);
		}
	}

	data["Players"] = json::array();//players;

	int Aimsize = 100 * 70 / (CameraCache.POV.FOV);
	if (Aimsize > 200)
		Aimsize = 200;
	AimbotTartgetExist = false;
	AimbotPredicScreenPos = { 0,0,0 };
	float distancecehck = 999999;

	float aimspeed = 4.5f;
	float minaimspeed = 3.0f;

	int calx;
	int caly;

	float TargetX = 0;
	float TargetY = 0;

	for (auto entity : Players) {
		if (entity == playerentity)// || !playerentity)
			continue;

		int entityteam;
		dr->RPM(g_pid, &entityteam, entity + oTeam, 4);
		if (entityteam == playerteam)
			continue;

		float entityhealth = 0.f;
		dr->RPM(g_pid, &entityhealth, entity + oHealth, 4);
		if (entityhealth == 0.f || entityhealth < 0)
			continue;
			

		uint64_t entityrootcomp;
		dr->RPM(g_pid, &entityrootcomp, entity + oActor_Rootcomp, 8);
		entityrootcomp = Decryptptr(entityrootcomp);
		
		Vector3 rootcompbaseposition;
		dr->RPM(g_pid, &rootcompbaseposition, entityrootcomp + oRootcompPosition, 12);

		uint64_t mesh;
		dr->RPM(g_pid, &mesh, entity + oMesh, 8);
		if (mesh < 0xfffff || mesh > 0xfffffffffffffff)
			continue;


		int entityvis = 0;
		float LastSubmitTime;
		dr->RPM(g_pid, &LastSubmitTime, mesh + oLastSubmitTime, 4);
		float LastRenderTimeOnScreen;
		dr->RPM(g_pid, &LastRenderTimeOnScreen, mesh + oLastRenderTimeOnScreen, 4);

		const float VisionTick = 0.04f;
		bool bVisible = LastRenderTimeOnScreen + VisionTick >= LastSubmitTime;

		if (bVisible)
			entityvis = 1234;


		Vector3 baseposition = GetBoneWithRotation(mesh, 0);
		Vector3 wtsbaseposition = WorldToScreen(baseposition);
		bool IsOnScreen = false;
		if (wtsbaseposition.x >= -200 && wtsbaseposition.x <= s_width+200 && wtsbaseposition.y >= -200 && wtsbaseposition.y <= s_height+200)
			IsOnScreen = true;
		float distance = baseposition.Distance(CameraCache.POV.Location) / 100.0f;

		if (distance >= 700.f)
			continue;

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

		uint64_t currtick = GetTickCount64();

		if (g_EnemyInfo.find(entity) == g_EnemyInfo.end()) {
			g_EnemyInfo.emplace(entity, new EnemyData());
		}
		else {
			auto enemyinfo = g_EnemyInfo[entity];
			if (currtick - g_EnemyInfo[entity]->lastcheck >= 200) {
				g_EnemyInfo[entity]->lastcheck = currtick;
				g_EnemyInfo[entity]->VelocityBefore200ms = g_EnemyInfo[entity]->Velocity;
				g_EnemyInfo[entity]->Velocity = baseposition - g_EnemyInfo[entity]->lastposition;
				g_EnemyInfo[entity]->Velocity = g_EnemyInfo[entity]->Velocity * 5.f;
				g_EnemyInfo[entity]->lastposition = baseposition;
			}
		}

		Vector3 Aiminglocation;
		Vector3 ActorBaseLoc = GetBoneWithRotation(mesh, 0);
		Vector3 Velocity{ 0,0,0 };

		Velocity = g_EnemyInfo[entity]->Velocity;
		float SpeedAvg = Velocity.Size();
		float distancefromBase = ActorBaseLoc.Distance(CameraCache.POV.Location) / 100.0f;
		int aiming_position = FemaleBones::fforehead;
		if (distancefromBase < 50.0f) {
			aiming_position = FemaleBones::fforehead;//머리
		}
		else {
			if (dr->GetKeystate(VK_LSHIFT)) {
				aiming_position = FemaleBones::fforehead;
			}
			else {
				if (SpeedAvg > 1.0f) {//움직일 때
					aiming_position = FemaleBones::fneck_01;
				}
				else {//가만히 있을떄
					if (entityhealth > 60.f)
						aiming_position = FemaleBones::fforehead;//머리
					else
						aiming_position = FemaleBones::fspine_03;
				}
			}
		}

		if (recoil.Size() > 0.125) {
			aiming_position = FemaleBones::fneck_01;
		}


		Aiminglocation = GetBoneWithRotation(mesh, aiming_position);



		EnemyDataPast* pastenemydata = nullptr;//200ms 전의 상태 데이터
		if (g_EnemyPastInfo.find(entity) == g_EnemyPastInfo.end()) {
			g_EnemyPastInfo.emplace(entity, new std::queue<EnemyDataPast*>());
		}
		else {
			auto pastqueue = g_EnemyPastInfo[entity];

			//업데이트 한 정보를 push
			pastqueue->push(new EnemyDataPast(currtick, baseposition, Velocity, Aiminglocation));

			if (!pastqueue->empty()){
				//사용할 정보를 pop
				while (true) {
					pastenemydata = pastqueue->front();
					if (currtick - pastenemydata->Ticktime >= 200) {
						delete(pastenemydata);
						pastqueue->pop();
					}
					else {
						break;
					}
				}
			}
		}
		

		

		if (bVisible){
			/*uint64_t VehicleRiderComponent = 0;
			dr->RPM(g_pid, &VehicleRiderComponent, entity + oVehicleRiderComponent, 8);

			uint64_t LastVehiclePawn = 0;
			dr->RPM(g_pid, &LastVehiclePawn, VehicleRiderComponent + oLastVehiclePawn, 8);

			int Seatindex = 0;
			dr->RPM(g_pid, &Seatindex, VehicleRiderComponent + oSeatIndex, 4);

			Vector3 Velocity{ 0,0,0 };
			
			Velocity = g_EnemyInfo[entity]->Velocity;
			if (Seatindex == -1)
				dr->RPM(g_pid, &Velocity, entityrootcomp + oComponentVelocity, 12);
			else 
				dr->RPM(g_pid, &Velocity, LastVehiclePawn + oReplicatedMovement, 12);
				*/



			/*Vector3 Aiminglocation;
			Vector3 ActorBaseLoc = GetBoneWithRotation(mesh, 0);
			Vector3 Velocity{ 0,0,0 };

			Velocity = g_EnemyInfo[entity]->Velocity;
			float SpeedAvg = Velocity.Size();
			float distancefromBase = ActorBaseLoc.Distance(CameraCache.POV.Location) / 100.0f;
			int aiming_position = FemaleBones::fforehead;
			if (distancefromBase < 50.0f) {
				aiming_position = FemaleBones::fforehead;//머리
			}
			else {
				if (dr->GetKeystate(VK_LSHIFT)) {
					aiming_position = FemaleBones::fforehead;
				}
				else {
					if (SpeedAvg > 1.0f) {//움직일 때
						aiming_position = FemaleBones::fneck_01;
					}
					else {//가만히 있을떄
						if (entityhealth > 60.f)
							aiming_position = FemaleBones::fforehead;//머리
						else
							aiming_position = FemaleBones::fspine_03;
					}
				}
			}

			Aiminglocation = GetBoneWithRotation(mesh, aiming_position);
			*/


			if (pastenemydata) {
				float TravelTime = distance / playerGunSpeed;
				float BulletDrop = 0;

				Vector3 PredictedPos{ 0, 0, 0 };
				PredictedPos.x = pastenemydata->AimbotWorldPos.x + (Velocity.x * (TravelTime + 0.2f));
				PredictedPos.y = pastenemydata->AimbotWorldPos.y + (Velocity.y * (TravelTime + 0.2f));
				PredictedPos.z = pastenemydata->AimbotWorldPos.z;
				//PredictedPos.z = Aiminglocation.z + (Velocity.z * flTime) + fabs(BulletDrop); //+ (9.80665f * 100 / 2 * flTime * flTime);

				Vector3 headposwts = WorldToScreenAim(PredictedPos, sway);//WorldToScreen(PredictedPos);
				float distance1 = SCenter.Distance(headposwts);//Scenter.Distance(headposwts);


				if (distance1 < distancecehck) {
					AimbotPredicWorldPos = PredictedPos;
					AimbotPredicScreenPos = headposwts;
					distancecehck = distance1;
					AimbotTartgetExist = true;
				}
			}

			 //원본
			/*float TravelTime = distance / playerGunSpeed;
			float BulletDrop = 0;

			Vector3 PredictedPos{ 0, 0, 0 };
			PredictedPos.x = Aiminglocation.x + (Velocity.x * TravelTime);
			PredictedPos.y = Aiminglocation.y + (Velocity.y * TravelTime);
			PredictedPos.z = Aiminglocation.z;
			//PredictedPos.z = Aiminglocation.z + (Velocity.z * flTime) + fabs(BulletDrop); //+ (9.80665f * 100 / 2 * flTime * flTime);

			Vector3 headposwts = WorldToScreenAim(PredictedPos, sway);//WorldToScreen(PredictedPos);
			float distance1 = SCenter.Distance(headposwts);//Scenter.Distance(headposwts);
			

			if (distance1 < distancecehck){
				AimbotPredicWorldPos = PredictedPos;
				AimbotPredicScreenPos = headposwts;
				distancecehck = distance1;
				AimbotTartgetExist = true;
			}*/
		}

		if (distance >= 200.f) {
			fforepos = { -21,-30,0 };
			fHeadpos = { -220,-30,0 };
			fneck_01pos = { -240,-30,0 };

			fupperarm_r_pos = { -250,-30,0 };
			flowerarm_r_pos = { -260,-30,0 };
			fhand_r_pos = { -270,-30,0 };

			fupperarm_l_pos = { -250,-30,0 };
			flowerarm_l_pos = { -280,-30,0 };
			fhand_l_pos = { -270,-30,0 };

			fspine_02_pos = { -240,-30,0 };
			fspine_01_pos = { -250,-30,0 };
			fpelvis_pos = { -230,-30,0 };

			fthigh_r_pos = { -260,-30,0 };
			fcalf_r_pos = { -250,-30,0 };
			ffoot_r_pos = { -220,-30,0 };

			fthigh_l_pos = { -210,-30,0 };
			fcalf_l_pos = { -260,-30,0 };
			ffoot_l_pos = { -220,-30,0 };
		}



		int minix = -1;
		int miniy = -1;
		if (1) {
			if (baseposition.x > playerPosition.x - 20000.f && baseposition.x < playerPosition.x + 20000.f && baseposition.y > playerPosition.y - 20000.f && baseposition.y < playerPosition.y + 20000){
				minix = (int)fabsf(baseposition.x - (playerPosition.x - 20000.f));
				miniy = (int)fabsf(baseposition.y - (playerPosition.y - 20000.f));
				IsOnScreen = true;
			}
		}




		if (IsOnScreen) {
			drawsection.playernum++;
			data["Players"].emplace_back(json::object({
					{"minix", (int)minix},
					{"miniy", (int)miniy},
					{"vis", (int)entityvis},
					{"dt", (int)distance},
					{"bx", (int)wtsbaseposition.x},
					{"by", (int)wtsbaseposition.y},
					{"15x", (int)fforepos.x},
					{"15y", (int)fforepos.y},
					{"6x", (int)fHeadpos.x},
					{"6y", (int)fHeadpos.y},
					{"5x", (int)fneck_01pos.x},
					{"5y", (int)fneck_01pos.y},
					{"115x", (int)fupperarm_r_pos.x},
					{"115y", (int)fupperarm_r_pos.y},
					{"116x", (int)flowerarm_r_pos.x},
					{"116y", (int)flowerarm_r_pos.y},
					{"117x", (int)fhand_r_pos.x},
					{"117y", (int)fhand_r_pos.y},
					{"88x", (int)fupperarm_l_pos.x},
					{"88y", (int)fupperarm_l_pos.y},
					{"89x", (int)flowerarm_l_pos.x},
					{"89y", (int)flowerarm_l_pos.y},
					{"90x", (int)fhand_l_pos.x},
					{"90y", (int)fhand_l_pos.y},
					{"1x", (int)fpelvis_pos.x},
					{"1y", (int)fpelvis_pos.y},
					{"3x", (int)fspine_02_pos.x},
					{"3y", (int)fspine_02_pos.y},
					{"2x", (int)fspine_01_pos.x},
					{"2y", (int)fspine_01_pos.y},
					{"174x", (int)fthigh_r_pos.x},
					{"174y", (int)fthigh_r_pos.y},
					{"175x", (int)fcalf_r_pos.x},
					{"175y", (int)fcalf_r_pos.y},
					{"176x", (int)ffoot_r_pos.x},
					{"176y", (int)ffoot_r_pos.y},
					{"168x", (int)fthigh_l_pos.x},
					{"168y", (int)fthigh_l_pos.y},
					{"169x", (int)fcalf_l_pos.x},
					{"169y", (int)fcalf_l_pos.y},
					{"170x", (int)ffoot_l_pos.x},
					{"170y", (int)ffoot_l_pos.y}
				}));
		}
		


		//DrawSkeleton_manual(mesh, false);
	}


	if (AimbotTartgetExist){

		Vector3 AimpotPredwts = WorldToScreen(AimbotPredicWorldPos);
		DrawLine(AimpotPredwts.x - 1, AimpotPredwts.y - 1, AimpotPredwts.x + 1, AimpotPredwts.y + 1, 3, 1, 1, 1, 1);
		DrawLine(AimpotPredwts.x + 1, AimpotPredwts.y - 1, AimpotPredwts.x - 1, AimpotPredwts.y + 1, 3, 1, 1, 1, 1);
		

		if (playerCurrGunIdx == 0xffffffff)
			return;
		if (playerCurrGunIdx == 4)
			return;
		
		bool aimbotKeypress = false;

		bool MouseLeftClick = dr->GetKeystate(VK_LBUTTON);
		bool MouseX1Click = dr->GetKeystate(VK_XBUTTON1);
		bool MouseX2Click = dr->GetKeystate(VK_XBUTTON2);
		bool LctrlPress = dr->GetKeystate(VK_LCONTROL);
		
		if (MouseLeftClick)
			aimbotKeypress = true;
		if (MouseX1Click)
			aimbotKeypress = true;
		if (MouseX2Click)
			aimbotKeypress = true;

		if (LctrlPress)
			aimbotKeypress = false;

		if ((!MouseLeftClick && !MouseX1Click && !MouseX2Click) || LctrlPress) {
			SavedAimbottargetaddr = 0;
			aimbotstarttime = 0;
		}

		if (!aimbotKeypress)
			return;

		/*if (Aimbottargetaddr != 0) {
			uint64_t  Aimbotmesh = 0;
			dr->RPM(g_pid, &Aimbotmesh, Aimbottargetaddr + oMesh, 8);
			if (GetHealth(Aimbottargetaddr) == 0.f)
			{
				aimbotendtime = GetTickCount64();
				aimbotstarttime = aimbotendtime + 350;
				Aimbottargetaddr = 0;
			}
			if (!isvisible(Aimbotmesh))
			{
				Aimbottargetaddr = 0;
			}
		}*/





		if (distancecehck < Aimsize){
			if (AimbotPredicScreenPos.x > (s_width / 2)){
				calx = -((s_width / 2) - AimbotPredicScreenPos.x);
				TargetX = calx / aimspeed;

				if (TargetX > 0.125 && TargetX < 1)
					TargetX = 1;
			}
			if (AimbotPredicScreenPos.x < (s_width / 2)){
				calx = AimbotPredicScreenPos.x - (s_width / 2);
				TargetX = calx / aimspeed;

				if (TargetX < -0.125f && TargetX > -1)
					TargetX = -1;
			}

			if (AimbotPredicScreenPos.y > (s_height / 2)) {
				caly = -((s_height / 2) - AimbotPredicScreenPos.y);
				TargetY = caly / aimspeed;

				if (TargetY > 0.125 && TargetY < 1)//aimspeed)
					TargetY = 1;
			}
			if (AimbotPredicScreenPos.y < (s_height / 2)) {
				caly = AimbotPredicScreenPos.y - (s_height / 2);
				TargetY = caly / aimspeed;
				if (TargetY < -0.125 && TargetY > -1)//-1 * aimspeed)
					TargetY = -1;

			}
			TargetY = 0;
			dr->mouse_event(TargetX, TargetY, 0);
		}
	}
}


void App::MainTick(json &data)
{

	target_tp += time_between_frames;
	std::this_thread::sleep_until(target_tp);

	fps++;
	SCenter.x = s_width / 2;
	SCenter.y = s_height / 2;
	SCenter.z = 0;

	uint64_t t = GetTickCount64();
	if (t - lastcheck > 1000){
		fps = 0;
		UpdatePtr();
		snapShotPlayers();
		lastcheck = t;
	}

	GetCameraCache();
	Playerloop(data);
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
