#pragma once

#include <thread>
#include <unordered_map>
#include <string>

#include <d3d9.h>
#include <vector>

#include "settings.hpp"
#include "driver.hpp"
#include "offsets.hpp"
#include "ida.hpp"

#define M_PI 3.14159265358979323846f
#define FortPTR reinterpret_cast< uint64_t >

class Vector2 {
	public:
		Vector2() : x(0.f), y(0.f) {}
		Vector2(double _x, double _y) : x(_x), y(_y) {}
		~Vector2() {}

		double x, y;
};

class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f)
	{

	}

	Vector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z)
	{

	}
	~Vector3()
	{

	}

	double x;
	double y;
	double z;

	inline double dot(Vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline bool isValid() const {
		return isfinite(x) && isfinite(y) && isfinite(z);
	}

	inline double distance(Vector3 v)
	{
		return double(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}

	inline double length() {
		return sqrt(x * x + y * y + z * z);
	}

	Vector3 operator+(const Vector3& other) const { return { this->x + other.x, this->y + other.y, this->z + other.z }; }
	Vector3 operator-(const Vector3& other) const { return { this->x - other.x, this->y - other.y, this->z - other.z }; }
	Vector3 operator*(float offset) const { return { this->x * offset, this->y * offset, this->z * offset }; }
	Vector3 operator/(float offset) const { return { this->x / offset, this->y / offset, this->z / offset }; } // MSCV linker is actually retarded LLVM on top

	Vector3& operator*=(const double other) { this->x *= other; this->y *= other; this->z *= other; return *this; }
	Vector3& operator/=(const double other) { this->x /= other; this->y /= other; this->z /= other; return *this; }

	Vector3& operator=(const Vector3& other) { this->x = other.x; this->y = other.y; this->z = other.z; return *this; }
	Vector3& operator+=(const Vector3& other) { this->x += other.x; this->y += other.y; this->z += other.z; return *this; }
	Vector3& operator-=(const Vector3& other) { this->x -= other.x; this->y -= other.y; this->z -= other.z; return *this; }
	Vector3& operator*=(const Vector3& other) { this->x *= other.x; this->y *= other.y; this->z *= other.z; return *this; }
	Vector3& operator/=(const Vector3& other) { this->x /= other.x; this->y /= other.y; this->z /= other.z; return *this; }

	operator bool() { return bool(this->x || this->y || this->z); }
	friend bool operator==(const Vector3& a, const Vector3& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }
	friend bool operator!=(const Vector3& a, const Vector3& b) { return !(a == b); }
};



struct FQuat { double x, y, z, w; };
struct FTransform
{
	FQuat rot;
	Vector3 translation;
	uint8_t pad[0x8];
	Vector3 scale;
	uint8_t pad1[0x8];
	D3DMATRIX to_matrix_with_scale() 
	{
		D3DMATRIX m{};
		const Vector3 Scale
		(
			(scale.x == 0.0) ? 1.0 : scale.x,
			(scale.y == 0.0) ? 1.0 : scale.y,
			(scale.z == 0.0) ? 1.0 : scale.z
		);


		const double x2 = rot.x + rot.x;
		const double y2 = rot.y + rot.y;
		const double z2 = rot.z + rot.z;
		const double xx2 = rot.x * x2;
		const double yy2 = rot.y * y2;
		const double zz2 = rot.z * z2;
		const double yz2 = rot.y * z2;
		const double wx2 = rot.w * x2;
		const double xy2 = rot.x * y2;
		const double wz2 = rot.w * z2;
		const double xz2 = rot.x * z2;
		const double wy2 = rot.w * y2;

		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;
		m._11 = (1.0f - (yy2 + zz2)) * scale.x;
		m._22 = (1.0f - (xx2 + zz2)) * scale.y;
		m._33 = (1.0f - (xx2 + yy2)) * scale.z;
		m._32 = (yz2 - wx2) * scale.z;
		m._23 = (yz2 + wx2) * scale.y;
		m._21 = (xy2 - wz2) * scale.y;
		m._12 = (xy2 + wz2) * scale.x;
		m._31 = (xz2 + wy2) * scale.z;
		m._13 = (xz2 - wy2) * scale.x;
		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;
		return m;
	}
};

template <typename T>
struct FArray {
	uintptr_t array = 0;
	uint32_t count = 0;
	uint32_t max_count = 0;

	uintptr_t get(uint32_t Index) const {
		if (Index >= count) {
			return 0;
		}
		return read<uintptr_t>(array + (Index * sizeof(T))); // Use sizeof(T)
	}

	uint32_t size() const {
		return count;
	}

	uintptr_t operator[](uint32_t Index) const {
		return get(Index);
	}

	bool is_valid() const {
		return array && count <= max_count && max_count <= 1000000;
	}

	uintptr_t get_address() const {
		return array;
	}
};


D3DMATRIX matrix_multiplication(const D3DMATRIX& pm1, const D3DMATRIX& pm2)
{
	D3DMATRIX pout{};
	pout._11 = pm1._11 * pm2._11 + pm1._12 * pm2._21 + pm1._13 * pm2._31 + pm1._14 * pm2._41;
	pout._12 = pm1._11 * pm2._12 + pm1._12 * pm2._22 + pm1._13 * pm2._32 + pm1._14 * pm2._42;
	pout._13 = pm1._11 * pm2._13 + pm1._12 * pm2._23 + pm1._13 * pm2._33 + pm1._14 * pm2._43;
	pout._14 = pm1._11 * pm2._14 + pm1._12 * pm2._24 + pm1._13 * pm2._34 + pm1._14 * pm2._44;
	pout._21 = pm1._21 * pm2._11 + pm1._22 * pm2._21 + pm1._23 * pm2._31 + pm1._24 * pm2._41;
	pout._22 = pm1._21 * pm2._12 + pm1._22 * pm2._22 + pm1._23 * pm2._32 + pm1._24 * pm2._42;
	pout._23 = pm1._21 * pm2._13 + pm1._22 * pm2._23 + pm1._23 * pm2._33 + pm1._24 * pm2._43;
	pout._24 = pm1._21 * pm2._14 + pm1._22 * pm2._24 + pm1._23 * pm2._34 + pm1._24 * pm2._44;
	pout._31 = pm1._31 * pm2._11 + pm1._32 * pm2._21 + pm1._33 * pm2._31 + pm1._34 * pm2._41;
	pout._32 = pm1._31 * pm2._12 + pm1._32 * pm2._22 + pm1._33 * pm2._32 + pm1._34 * pm2._42;
	pout._33 = pm1._31 * pm2._13 + pm1._32 * pm2._23 + pm1._33 * pm2._33 + pm1._34 * pm2._43;
	pout._34 = pm1._31 * pm2._14 + pm1._32 * pm2._24 + pm1._33 * pm2._34 + pm1._34 * pm2._44;
	pout._41 = pm1._41 * pm2._11 + pm1._42 * pm2._21 + pm1._43 * pm2._31 + pm1._44 * pm2._41;
	pout._42 = pm1._41 * pm2._12 + pm1._42 * pm2._22 + pm1._43 * pm2._32 + pm1._44 * pm2._42;
	pout._43 = pm1._41 * pm2._13 + pm1._42 * pm2._23 + pm1._43 * pm2._33 + pm1._44 * pm2._43;
	pout._44 = pm1._41 * pm2._14 + pm1._42 * pm2._24 + pm1._43 * pm2._34 + pm1._44 * pm2._44;
	return pout;
}

D3DMATRIX to_matrix(Vector3 rot, Vector3 origin = Vector3(0, 0, 0))
{
	float radpitch = (rot.x * M_PI / 180);
	float radyaw = (rot.y * M_PI / 180);
	float radroll = (rot.z * M_PI / 180);
	float sp = sinf(radpitch);
	float cp = cosf(radpitch);
	float sy = sinf(radyaw);
	float cy = cosf(radyaw);
	float sr = sinf(radroll);
	float cr = cosf(radroll);
	D3DMATRIX matrix{};
	matrix.m[0][0] = cp * cy;
	matrix.m[0][1] = cp * sy;
	matrix.m[0][2] = sp;
	matrix.m[0][3] = 0.f;
	matrix.m[1][0] = sr * sp * cy - cr * sy;
	matrix.m[1][1] = sr * sp * sy + cr * cy;
	matrix.m[1][2] = -sr * cp;
	matrix.m[1][3] = 0.f;
	matrix.m[2][0] = -(cr * sp * cy + sr * sy);
	matrix.m[2][1] = cy * sr - cr * sp * sy;
	matrix.m[2][2] = cr * cp;
	matrix.m[2][3] = 0.f;
	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;
	return matrix;
}

struct camera {
	Vector3 location;
	Vector3 rotation;
	float fov;
};

struct FNRot
{
	double a;
	char pad_0008[24];
	double b;
	char pad_0028[424];
	double c;
};

namespace cache {
	inline uintptr_t uworld;
	inline uintptr_t AcknowledgedPawn;
	inline uintptr_t game_instance;
	inline uintptr_t local_players;
	inline uintptr_t player_controller;
	inline uintptr_t local_pawn;
	inline uintptr_t root_component;
	inline uintptr_t player_state;
	inline uintptr_t game_state;
	inline uintptr_t player_array;
	inline uintptr_t closest_pawn;
	inline uintptr_t current_weapon;

	inline int my_team_id;
	inline int player_count;

	inline float closest_distance;

	inline Vector3 relative_location;

	inline camera local_camera;
}

struct FBounds
{
	float top, bottom, left, right;
};

struct FPlane : Vector3
{
	double W;

	FPlane() : W(0) { }
	FPlane(double W) : W(W) { }
};

class FMatrix
{
public:
	double m[4][4];
	FPlane XPlane, YPlane, ZPlane, WPlane;

	FMatrix() : XPlane(), YPlane(), ZPlane(), WPlane() { }
	FMatrix(FPlane XPlane, FPlane YPlane, FPlane ZPlane, FPlane WPlane)
		: XPlane(XPlane), YPlane(YPlane), ZPlane(ZPlane), WPlane(WPlane) { }
};

inline double RadiansToDegrees(double dRadians)
{
	return dRadians * (180.0 / M_PI);
}

template< typename t >
class TArray
{
public:

	TArray() : tData(), iCount(), iMaxCount() { }
	TArray(t* data, int count, int max_count) :
		tData(tData), iCount(iCount), iMaxCount(iMaxCount) { }

public:

	auto Get(int idx) -> t
	{
		return read< t >(reinterpret_cast<__int64>(this->tData) + (idx * sizeof(t)));
	}

	auto Size() -> std::uint32_t
	{
		return this->iCount;
	}

	bool IsValid()
	{
		return this->iCount != 0;
	}

	t* tData;
	int iCount;
	int iMaxCount;
};

uintptr_t GetViewState()
{
	TArray<uintptr_t> ViewState = read<TArray<uintptr_t>>(cache::local_players + 0xD0);
	return ViewState.Get(1);
}

camera get_view_point()
{
	//YOU MIGHT NEED TO UPDATE THE LOCATION AND ROTATION POINTER IN THE NEXT UPDATE SO CHECK IT OUT DONT FORGET!
	camera view_point{};
	uintptr_t location_pointer = read<uintptr_t>(cache::uworld + 0x128); //
	uintptr_t rotation_pointer = read<uintptr_t>(cache::uworld + 0x138); //
	FNRot fnrot{};
	fnrot.a = read<double>(rotation_pointer);
	fnrot.b = read<double>(rotation_pointer + 0x20);
	fnrot.c = read<double>(rotation_pointer + 0x1D0);
	view_point.location = read<Vector3>(location_pointer);
	view_point.rotation.x = asin(fnrot.c) * (180.0 / M_PI);
	view_point.rotation.y = ((atan2(fnrot.a * -1, fnrot.b) * (180.0 / M_PI)) * -1) * -1;
	auto fov_radians = read<float>(cache::player_controller + 0x3AC) * 2;
	return view_point;
}

//camera get_view_point()
//{
//	auto mProjection = read<FMatrix>(GetViewState() + 0x940);
//	cache::local_camera.rotation.x = RadiansToDegrees(std::asin(mProjection.ZPlane.W));
//	cache::local_camera.rotation.y = RadiansToDegrees(std::atan2(mProjection.YPlane.W, mProjection.XPlane.W));
//	cache::local_camera.rotation.z = 0.0;
//
//	cache::local_camera.location.x = mProjection.m[3][0];
//	cache::local_camera.location.y = mProjection.m[3][1];
//	cache::local_camera.location.z = mProjection.m[3][2];
//	float FieldOfView = atanf(1 / read<double>(GetViewState() + 0x740)) * 2;
//	cache::local_camera.fov = (FieldOfView) * (180.f / M_PI); return cache::local_camera;
//}

double get_cross_distance(double x1, double y1, double x2, double y2)
{
	return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
}

ImVec2 calc_aim(const Vector2& target2D, const ImVec2& screenCenter, int aimSpeed, float screenWidth, float screenHeight)
{
	float targetX = 0.0f;
	float targetY = 0.0f;

	if (target2D.x != 0)
	{
		targetX = (target2D.x > screenCenter.x) ? -(screenCenter.x - target2D.x) / aimSpeed : (target2D.x - screenCenter.x) / aimSpeed;
		if ((target2D.x > screenCenter.x && targetX + screenCenter.x > screenWidth) || (target2D.x < screenCenter.x && targetX + screenCenter.x < 0)) targetX = 0.0f;
	}

	if (target2D.y != 0)
	{
		targetY = (target2D.y > screenCenter.y) ? -(screenCenter.y - target2D.y) / aimSpeed : (target2D.y - screenCenter.y) / aimSpeed;
		if ((target2D.y > screenCenter.y && targetY + screenCenter.y > screenHeight) || (target2D.y < screenCenter.y && targetY + screenCenter.y < 0)) targetY = 0.0f;
	}

	return ImVec2(targetX, targetY);
}

Vector3 prediction(Vector3 TargetPosition, Vector3 ComponentVelocity, float player_distance, float projectile_speed = 239, float projectile_gravity = 3.5)
{
	float TimeToTarget = player_distance / projectile_speed;
	float bulletDrop = abs(projectile_gravity) * (TimeToTarget * TimeToTarget) * 0.5;

	return Vector3
	{
		TargetPosition.x + TimeToTarget * ComponentVelocity.x,
		TargetPosition.y + TimeToTarget * ComponentVelocity.y,
		TargetPosition.z + TimeToTarget * ComponentVelocity.z + bulletDrop
	};
}

Vector2 project_world_to_screen(Vector3 world_location)
{
	cache::local_camera = get_view_point();
	D3DMATRIX temp_matrix = to_matrix(cache::local_camera.rotation);

	Vector3 vaxisx = Vector3(temp_matrix.m[0][0], temp_matrix.m[0][1], temp_matrix.m[0][2]);
	Vector3 vaxisy = Vector3(temp_matrix.m[1][0], temp_matrix.m[1][1], temp_matrix.m[1][2]);
	Vector3 vaxisz = Vector3(temp_matrix.m[2][0], temp_matrix.m[2][1], temp_matrix.m[2][2]);
	Vector3 vdelta = world_location - cache::local_camera.location;

	float transformedX = vdelta.dot(vaxisy);
	float transformedY = vdelta.dot(vaxisz);
	float transformedZ = vdelta.dot(vaxisx);
	transformedZ = max(transformedZ, 1.0f);

	float fovFactor = settings::width / 2 / tanf(cache::local_camera.fov * M_PI / 360);
	float screenX = settings::width / 2 + transformedX * fovFactor / transformedZ;
	float screenY = settings::height / 2 - transformedY * fovFactor / transformedZ;

	return Vector2(screenX, screenY);
}

Vector3 world_to_screen(Vector3 WorldLocation)
{
	cache::local_camera = get_view_point();
	D3DMATRIX tempMatrix = to_matrix(cache::local_camera.rotation);
	Vector3 vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	Vector3 vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	Vector3 vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	Vector3 vDelta = WorldLocation - cache::local_camera.location;
	Vector3 vTransformed = Vector3(vDelta.dot(vAxisY), vDelta.dot(vAxisZ), vDelta.dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	return Vector3(
		(settings::width / 2 / 2.0f) + vTransformed.x * (((settings::width / 2 / 2.0f) / tanf(cache::local_camera.fov * (float)M_PI / 360.f))) / vTransformed.z, (settings::height / 2 / 2.0f) - vTransformed.y * (((settings::height / 2 / 2.0f) / tanf(cache::local_camera.fov * (float)M_PI / 360.f))) / vTransformed.z, 0
	);
}

Vector3 get_entity_bone(uintptr_t mesh, int bone_id)
{
	uintptr_t bone_array = read<uintptr_t>(mesh + BONE_ARRAY);
	if (bone_array == 0) bone_array = read<uintptr_t>(mesh + BONE_ARRAY_CACHE); // 0x10
	FTransform bone = read<FTransform>(bone_array + (bone_id * 0x60));
	FTransform component_to_world = read<FTransform>(mesh + COMPONENT_TO_WORLD);
	D3DMATRIX matrix = matrix_multiplication(bone.to_matrix_with_scale(), component_to_world.to_matrix_with_scale());
	return Vector3(matrix._41, matrix._42, matrix._43);
}

FBounds get_actor_bounds(Vector2 head2d, Vector2 bottom2d, float size)
{
	FBounds a_bounds;
	a_bounds.top = head2d.y;
	a_bounds.bottom = bottom2d.y;
	a_bounds.left = (head2d.x - (size / 2));
	a_bounds.right = bottom2d.x + (size / 2);

	return a_bounds;
}

typedef struct visible_cached
{
	uintptr_t mesh;
	float last_submit_time_on_screen;
};

std::vector<visible_cached> visible_vec = {};

bool is_visible(uintptr_t mesh)
{
	auto Seconds = read<double>(cache::uworld + 0x150);  //0x148
	auto LastRenderTime = read<float>(mesh + 0x30C);
	return Seconds - LastRenderTime <= 0.06f;
}

std::string get_weapon_name(uintptr_t pawn_private)
{
	std::string weapon_name;

	uint64_t current_weapon = read<uint64_t>(pawn_private + 0xa80);  //OFFSET CURRENT_WEAPON
	uint64_t weapon_data = read<uint64_t>(current_weapon + 0x6a0);  //OFFSET WEAPON_DATA
	uint64_t item_name = read<uint64_t>(weapon_data + 0x40);

	uint64_t FData = read<uint64_t>(item_name + 0x18);
	int FLength = read<int>(item_name + 0x20);

	if (FLength > 0 && FLength < 50)
	{
		wchar_t* WeaponBuffer = new wchar_t[FLength];
		mem::read_physical((void*)FData, (PVOID)WeaponBuffer, FLength * sizeof(wchar_t));
		std::wstring wstr_buf(WeaponBuffer);
		weapon_name.append(std::string(wstr_buf.begin(), wstr_buf.end()));

		delete[] WeaponBuffer;
	}

	return weapon_name;
}


std::string get_player_name(uintptr_t playerstate)
{
	__int64 FString = read<__int64>(playerstate + 0xAF8);
	int Length = read<int>(FString + 16);
	uintptr_t FText = read<__int64>(FString + 8);

	__int64 v6 = Length;
	if (!v6) return std::string("BOT");

	wchar_t* NameBuffer = new wchar_t[Length];
	mem::read_physical((PVOID)FText, NameBuffer, Length * sizeof(wchar_t));

	char v21;
	int v22;
	int i;
	int v25;
	WORD* v23;

	v21 = v6 - 1;
	if (!(DWORD)v6)
		v21 = 0;
	v22 = 0;
	v23 = (WORD*)NameBuffer;
	for (i = (v21) & 3; ; *v23++ += i & 7)
	{
		v25 = v6 - 1;
		if (!(DWORD)v6)
			v25 = 0;
		if (v22 >= v25)
			break;
		i += 3;
		++v22;
	}

	std::wstring wbuffer{ NameBuffer };

	return std::string(wbuffer.begin(), wbuffer.end());
}

std::string get_player_platform(uintptr_t player_state)
{
	std::string result;
	uintptr_t test_platform = read<uintptr_t>(player_state + PLATFORM);

	wchar_t platform[64];
	mem::read_physical((void*)test_platform, platform, sizeof(platform));

	std::wstring platform_wstr(platform);
	std::string platform_str(platform_wstr.begin(), platform_wstr.end());

	if (platform_str == "XBL")
	{
		result = "XBOX ONE";
	}
	else if (platform_str == "PSN")
	{
		result = "PS4";
	}
	else if (platform_str == "PS5")
	{
		result = "PS5";
	}
	else if (platform_str == "XSX")
	{
		result = "XBOX SERIES S/X";
	}
	else if (platform_str == "SWT")
	{
		result = "NINTENDO";
	}
	else if (platform_str == "WIN")
	{
		result = "WINDOWS";
	}
	else
	{
		result = "NPC";
	}

	return result;
}
