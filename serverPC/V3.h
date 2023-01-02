#pragma once
#include <math.h>
#include <xmmintrin.h>

#define M_PI	3.14159265358979323846264338327950288419716939937510

typedef struct _DDMATRIX {
	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;

		};
		float m[4][4];
	};
} DDMATRIX;

class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f)
	{

	}

	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
	{

	}
	~Vector3()
	{

	}

	float x;
	float y;
	float z;

	inline float Dot(Vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}
	inline float Size()
	{
		return (float)sqrtf(x * x + y * y + z * z);
	}
	inline float Distance(Vector3 v)
	{
		return float(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}

	static __forceinline float InvSqrt(float F)
	{
		// Performs two passes of Newton-Raphson iteration on the hardware estimate
		//    v^-0.5 = x
		// => x^2 = v^-1
		// => 1/(x^2) = v
		// => F(x) = x^-2 - v
		//    F'(x) = -2x^-3

		//    x1 = x0 - F(x0)/F'(x0)
		// => x1 = x0 + 0.5 * (x0^-2 - Vec) * x0^3
		// => x1 = x0 + 0.5 * (x0 - Vec * x0^3)
		// => x1 = x0 + x0 * (0.5 - 0.5 * Vec * x0^2)
		//
		// This final form has one more operation than the legacy factorization (X1 = 0.5*X0*(3-(Y*X0)*X0)
		// but retains better accuracy (namely InvSqrt(1) = 1 exactly).

		const __m128 fOneHalf = _mm_set_ss(0.5f);
		__m128 Y0, X0, X1, X2, FOver2;
		float temp;

		Y0 = _mm_set_ss(F);
		X0 = _mm_rsqrt_ss(Y0);	// 1/sqrt estimate (12 bits)
		FOver2 = _mm_mul_ss(Y0, fOneHalf);

		// 1st Newton-Raphson iteration
		X1 = _mm_mul_ss(X0, X0);
		X1 = _mm_sub_ss(fOneHalf, _mm_mul_ss(FOver2, X1));
		X1 = _mm_add_ss(X0, _mm_mul_ss(X0, X1));

		// 2nd Newton-Raphson iteration
		X2 = _mm_mul_ss(X1, X1);
		X2 = _mm_sub_ss(fOneHalf, _mm_mul_ss(FOver2, X2));
		X2 = _mm_add_ss(X1, _mm_mul_ss(X1, X2));

		_mm_store_ss(&temp, X2);
		return temp;
	}
	Vector3 Normalized()
	{
		const float SquareSum = this->x * this->x + this->y * this->y + this->z * this->z;
		if (SquareSum > 1.e-8f)
		{
			const float Scale = InvSqrt(SquareSum);
			this->x *= Scale; this->y *= Scale; this->z *= Scale;
		}
		return *this;
	}
	Vector3 operator+(Vector3 v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator-(Vector3 v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 operator*(float number) const {
		return Vector3(x * number, y * number, z * number);
	}
};


struct FQuat
{
	float x;
	float y;
	float z;
	float w;
};

struct FTransform
{
	FQuat rot;
	Vector3 translation;
	char pad[4];
	Vector3 scale;
	char pad1[4];
	DDMATRIX ToMatrixWithScale()
	{
		DDMATRIX m;
		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;

		float x2 = rot.x + rot.x;
		float y2 = rot.y + rot.y;
		float z2 = rot.z + rot.z;

		float xx2 = rot.x * x2;
		float yy2 = rot.y * y2;
		float zz2 = rot.z * z2;
		m._11 = (1.0f - (yy2 + zz2)) * scale.x;
		m._22 = (1.0f - (xx2 + zz2)) * scale.y;
		m._33 = (1.0f - (xx2 + yy2)) * scale.z;

		float yz2 = rot.y * z2;
		float wx2 = rot.w * x2;
		m._32 = (yz2 - wx2) * scale.z;
		m._23 = (yz2 + wx2) * scale.y;

		float xy2 = rot.x * y2;
		float wz2 = rot.w * z2;
		m._21 = (xy2 - wz2) * scale.y;
		m._12 = (xy2 + wz2) * scale.x;

		float xz2 = rot.x * z2;
		float wy2 = rot.w * y2;
		m._31 = (xz2 + wy2) * scale.z;
		m._13 = (xz2 - wy2) * scale.x;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};

//Required PUBG classes

struct FMinimalViewInfo
{
	class Vector3                                     Location;                                                 // 0x0000(0x000C) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	class Vector3                                      Rotation;                                                 // 0x000C(0x000C) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              FOV;                                                      // 0x0018(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              OrthoWidth;                                               // 0x001C(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              OrthoNearClipPlane;                                       // 0x0020(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              OrthoFarClipPlane;                                        // 0x0024(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              AspectRatio;                                              // 0x0028(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
};

struct FCameraCacheEntry
{
	float                                              TimeStamp;                                                // 0x0000(0x0004) (CPF_ZeroConstructor, CPF_IsPlainOldData)
	unsigned char                                      UnknownData00[0xC];                                       // 0x0004(0x000C) MISSED OFFSET
	struct FMinimalViewInfo                            POV;                                                      // 0x0010(0x0530)
};

enum MaleBones : int
{
	root = 0,
	pelvis = 1,
	spine_01 = 2,
	spine_02 = 3,
	spine_03 = 4,
	neck_01 = 5,
	Head = 6,
	face_root = 7,
	eyebrows_pos_root = 8,
	eyebrows_root = 9,
	eyebrows_r = 10,
	eyebrows_l = 11,
	eyebrow_l = 12,
	eyebrow_r = 13,
	forehead_root = 14,
	forehead = 15,
	jaw_pos_root = 16,
	jaw_root = 17,
	jaw = 18,
	mouth_down_pos_root = 19,
	mouth_down_root = 20,
	lip_bm_01 = 21,
	lip_bm_02 = 22,
	lip_br = 23,
	lip_bl = 24,
	jaw_01 = 25,
	jaw_02 = 26,
	cheek_pos_root = 27,
	cheek_root = 28,
	cheek_r = 29,
	cheek_l = 30,
	nose_side_root = 31,
	nose_side_r_01 = 32,
	nose_side_r_02 = 33,
	nose_side_l_01 = 34,
	nose_side_l_02 = 35,
	eye_pos_r_root = 36,
	eye_r_root = 37,
	eye_rot_r_root = 38,
	eye_lid_u_r = 39,
	eye_r = 40,
	eye_lid_b_r = 41,
	eye_pos_l_root = 42,
	eye_l_root = 43,
	eye_rot_l_root = 44,
	eye_lid_u_l = 45,
	eye_l = 46,
	eye_lid_b_l = 47,
	nose_pos_root = 48,
	nose = 49,
	mouth_up_pos_root = 50,
	mouth_up_root = 51,
	lip_ul = 52,
	lip_um_01 = 53,
	lip_um_02 = 54,
	lip_ur = 55,
	lip_l = 56,
	lip_r = 57,
	Hair_Root = 58,
	Hair_B_01 = 59,
	Hair_B_02 = 60,
	Hair_L_01 = 61,
	Hair_L_02 = 62,
	Hair_R_01 = 63,
	Hair_R_02 = 64,
	Hair_F_02 = 65,
	Hair_F_01 = 66,
	hair_b_pt_01 = 67,
	hair_b_pt_02 = 68,
	hair_b_pt_03 = 69,
	hair_b_pt_04 = 70,
	hair_b_pt_05 = 71,
	camera_fpp = 72,
	GunReferencePoint = 73,
	GunRef = 74,
	Breast_L = 75,
	Breast_R = 76,
	clavicle_l = 77,
	upperarm_l = 78,
	lowerarm_l = 79,
	hand_l = 80,
	thumb_01_l = 81,
	thumb_02_l = 82,
	thumb_03_l = 83,
	thumb_04_l_MBONLY = 84,
	index_01_l = 85,
	index_02_l = 86,
	index_03_l = 87,
	index_04_l_MBONLY = 88,
	middle_01_l = 89,
	middle_02_l = 90,
	middle_03_l = 91,
	middle_04_l_MBONLY = 92,
	ring_01_l = 93,
	ring_02_l = 94,
	ring_03_l = 95,
	ring_04_l_MBONLY = 96,
	pinky_01_l = 97,
	pinky_02_l = 98,
	pinky_03_l = 99,
	pinky_04_l_MBONLY = 100,
	item_l = 101,
	lowerarm_twist_01_l = 102,
	upperarm_twist_01_l = 103,
	clavicle_r = 104,
	upperarm_r = 105,
	lowerarm_r = 106,
	hand_r = 107,
	thumb_01_r = 108,
	thumb_02_r = 109,
	thumb_03_r = 110,
	thumb_04_r_MBONLY = 111,
	index_01_r = 112,
	index_02_r = 113,
	index_03_r = 114,
	index_04_r_MBONLY = 115,
	middle_01_r = 116,
	middle_02_r = 117,
	middle_03_r = 118,
	middle_04_r_MBONLY = 119,
	ring_01_r = 120,
	ring_02_r = 121,
	ring_03_r = 122,
	ring_04_r_MBONLY = 123,
	pinky_01_r = 124,
	pinky_02_r = 125,
	pinky_03_r = 126,
	pinky_04_r_MBONLY = 127,
	item_r = 128,
	lowerarm_twist_01_r = 129,
	upperarm_twist_01_r = 130,
	Backpack = 131,
	backpack_01 = 132,
	backpack_02 = 133,
	slot_primary = 134,
	slot_secondary = 135,
	slot_melee = 136,
	slot_throwable = 137,
	Coat_L_01 = 138,
	Coat_L_02 = 139,
	Coat_L_03 = 140,
	Coat_L_04 = 141,
	Coat_FL_01 = 142,
	Coat_FL_02 = 143,
	Coat_FL_03 = 144,
	Coat_FL_04 = 145,
	Coat_B_01 = 146,
	Coat_B_02 = 147,
	Coat_B_03 = 148,
	Coat_B_04 = 149,
	Coat_R_01 = 150,
	Coat_R_02 = 151,
	Coat_R_03 = 152,
	Coat_R_04 = 153,
	Coat_FR_01 = 154,
	Coat_FR_02 = 155,
	Coat_FR_03 = 156,
	Coat_FR_04 = 157,
	thigh_l = 158,
	calf_l = 159,
	foot_l = 160,
	ball_l = 161,
	calf_twist_01_l = 162,
	thigh_twist_01_l = 163,
	thigh_r = 164,
	calf_r = 165,
	foot_r = 166,
	ball_r = 167,
	calf_twist_01_r = 168,
	thigh_twist_01_r = 169,
	slot_sidearm = 170,
	Skirt_L_01 = 171,
	Skirt_L_02 = 172,
	Skirt_L_03 = 173,
	Skirt_F_01 = 174,
	Skirt_F_02 = 175,
	Skirt_F_03 = 176,
	Skirt_B_01 = 177,
	Skirt_B_02 = 178,
	Skirt_B_03 = 179,
	Skirt_R_01 = 180,
	Skirt_R_02 = 181,
	Skirt_R_03 = 182,
	ik_hand_root = 183,
	ik_hand_gun = 184,
	ik_hand_r = 185,
	ik_hand_l = 186,
	ik_aim_root = 187,
	ik_aim_l = 188,
	ik_aim_r = 189,
	ik_foot_root = 190,
	ik_foot_l = 191,
	ik_foot_r = 192,
	camera_tpp = 193,
	ik_target_root = 194,
	ik_target_l = 195,
	ik_target_r = 196,
	VBspine_03_spine_03 = 197,
	VBupperarm_r_lowerarm_r = 198,
	VBGunRef_spine_03 = 199,
	VBcamera_fpp_hand_l = 200,
	VBcamera_fpp_hand_r = 201,
	VBcamera_fpp_clavicle_l = 202,
	VBcamera_fpp_clavicle_r = 203,
	VBlowerarm_l_hand_l = 204
};

enum FemaleBones : int
{
	froot = 0,
	fpelvis = 1,
	fspine_01 = 2,
	fspine_02 = 3,
	fspine_03 = 4,
	fneck_01 = 5,
	fHead = 6,
	fface_root = 7,
	feyebrows_pos_root = 8,
	feyebrows_root = 9,
	feyebrows_r = 10,
	feyebrows_l = 11,
	feyebrow_l = 12,
	feyebrow_r = 13,
	fforehead_root = 14,
	fforehead = 15,
	fjaw_pos_root = 16,
	fjaw_root = 17,
	fjaw = 18,
	fmouth_down_pos_root = 19,
	fmouth_down_root = 20,
	flip_bm_01 = 21,
	flip_bm_02 = 22,
	flip_br = 23,
	flip_bl = 24,
	fjaw_01 = 25,
	fjaw_02 = 26,
	fcheek_pos_root = 27,
	fcheek_root = 28,
	fcheek_r = 29,
	fcheek_l = 30,
	fnose_side_root = 31,
	fnose_side_r_01 = 32,
	fnose_side_r_02 = 33,
	fnose_side_l_01 = 34,
	fnose_side_l_02 = 35,
	feye_pos_r_root = 36,
	feye_r_root = 37,
	feye_rot_r_root = 38,
	feye_lid_u_r = 39,
	feye_r = 40,
	feye_lid_b_r = 41,
	feye_pos_l_root = 42,
	feye_l_root = 43,
	feye_rot_l_root = 44,
	feye_lid_u_l = 45,
	feye_l = 46,
	feye_lid_b_l = 47,
	fnose_pos_root = 48,
	fnose = 49,
	fmouth_up_pos_root = 50,
	fmouth_up_root = 51,
	flip_ul = 52,
	flip_um_01 = 53,
	flip_um_02 = 54,
	flip_ur = 55,
	flip_l = 56,
	flip_r = 57,
	fHair_Root = 58,
	fHair_B_01 = 59,
	fHair_B_02 = 60,
	fHair_L_01 = 61,
	fHair_L_02 = 62,
	fHair_R_01 = 63,
	fHair_R_02 = 64,
	fHair_F_02 = 65,
	fHair_F_01 = 66,
	fhair_b_pt_01 = 67,
	fhair_b_pt_02 = 68,
	fhair_b_pt_03 = 69,
	fhair_b_pt_04 = 70,
	fhair_b_pt_05 = 71,
	fhair_l_pt_01 = 72,
	fhair_l_pt_02 = 73,
	fhair_l_pt_03 = 74,
	fhair_l_pt_04 = 75,
	fhair_l_pt_05 = 76,
	fhair_r_pt_01 = 77,
	fhair_r_pt_02 = 78,
	fhair_r_pt_03 = 79,
	fhair_r_pt_04 = 80,
	fhair_r_pt_05 = 81,
	fcamera_fpp = 82,
	fGunReferencePoint = 83,
	fGunRef = 84,
	fBreast_L = 85,
	fBreast_R = 86,
	fclavicle_l = 87,
	fupperarm_l = 88,
	flowerarm_l = 89,
	fhand_l = 90,
	fthumb_01_l = 91,
	fthumb_02_l = 92,
	fthumb_03_l = 93,
	fthumb_04_l_MBONLY = 94,
	findex_01_l = 95,
	findex_02_l = 96,
	findex_03_l = 97,
	findex_04_l_MBONLY = 98,
	fmiddle_01_l = 99,
	fmiddle_02_l = 100,
	fmiddle_03_l = 101,
	fmiddle_04_l_MBONLY = 102,
	fring_01_l = 103,
	fring_02_l = 104,
	fring_03_l = 105,
	fring_04_l_MBONLY = 106,
	fpinky_01_l = 107,
	fpinky_02_l = 108,
	fpinky_03_l = 109,
	fpinky_04_l_MBONLY = 110,
	fitem_l = 111,
	flowerarm_twist_01_l = 112,
	fupperarm_twist_01_l = 113,
	fclavicle_r = 114,
	fupperarm_r = 115,
	flowerarm_r = 116,
	fhand_r = 117,
	fthumb_01_r = 118,
	fthumb_02_r = 119,
	fthumb_03_r = 120,
	fthumb_04_r_MBONLY = 121,
	findex_01_r = 122,
	findex_02_r = 123,
	findex_03_r = 124,
	findex_04_r_MBONLY = 125,
	fmiddle_01_r = 126,
	fmiddle_02_r = 127,
	fmiddle_03_r = 128,
	fmiddle_04_r_MBONLY = 129,
	fring_01_r = 130,
	fring_02_r = 131,
	fring_03_r = 132,
	fring_04_r_MBONLY = 133,
	fpinky_01_r = 134,
	fpinky_02_r = 135,
	fpinky_03_r = 136,
	fpinky_04_r_MBONLY = 137,
	fitem_r = 138,
	flowerarm_twist_01_r = 139,
	fupperarm_twist_01_r = 140,
	fBackpack = 141,
	fbackpack_01 = 142,
	fbackpack_02 = 143,
	fslot_primary = 144,
	fslot_secondary = 145,
	fslot_melee = 146,
	fslot_throwable = 147,
	fCoat_L_01 = 148,
	fCoat_L_02 = 149,
	fCoat_L_03 = 150,
	fCoat_L_04 = 151,
	fCoat_FL_01 = 152,
	fCoat_FL_02 = 153,
	fCoat_FL_03 = 154,
	fCoat_FL_04 = 155,
	fCoat_B_01 = 156,
	fCoat_B_02 = 157,
	fCoat_B_03 = 158,
	fCoat_B_04 = 159,
	fCoat_R_01 = 160,
	fCoat_R_02 = 161,
	fCoat_R_03 = 162,
	fCoat_R_04 = 163,
	fCoat_FR_01 = 164,
	fCoat_FR_02 = 165,
	fCoat_FR_03 = 166,
	fCoat_FR_04 = 167,
	fthigh_l = 168,
	fcalf_l = 169,
	ffoot_l = 170,
	fball_l = 171,
	fcalf_twist_01_l = 172,
	fthigh_twist_01_l = 173,
	fthigh_r = 174,
	fcalf_r = 175,
	ffoot_r = 176,
	fball_r = 177,
	fcalf_twist_01_r = 178,
	fthigh_twist_01_r = 179,
	fslot_sidearm = 180,
	fSkirt_L_01 = 181,
	fSkirt_L_02 = 182,
	fSkirt_L_03 = 183,
	fSkirt_F_01 = 184,
	fSkirt_F_02 = 185,
	fSkirt_F_03 = 186,
	fSkirt_B_01 = 187,
	fSkirt_B_02 = 188,
	fSkirt_B_03 = 189,
	fSkirt_R_01 = 190,
	fSkirt_R_02 = 191,
	fSkirt_R_03 = 192,
	fik_hand_root = 193,
	fik_hand_gun = 194,
	fik_hand_r = 195,
	fik_hand_l = 196,
	fik_aim_root = 197,
	fik_aim_l = 198,
	fik_aim_r = 199,
	fik_foot_root = 200,
	fik_foot_l = 201,
	fik_foot_r = 202,
	fcamera_tpp = 203,
	fik_target_root = 204,
	fik_target_l = 205,
	fik_target_r = 206,
	fVBspine_03_spine_03 = 207,
	fVBupperarm_r_lowerarm_r = 208,
	fVBGunRef_spine_03 = 209,
	fVBcamera_fpp_hand_l = 210,
	ffVBcamera_fpp_hand_r = 211,
	ffVBcamera_fpp_clavicle_l = 212,
	ffVBcamera_fpp_clavicle_r = 213,
	ffVBlowerarm_l_hand_l = 214,
};