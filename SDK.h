#pragma once

#define RVA(addr, size) ((uintptr_t)((UINT_PTR)(addr) + *(PINT)((UINT_PTR)(addr) + ((size) - sizeof(INT))) + (size)))
#define M_PI	3.14159265358979323846264338327950288419716939937510
#define _CRT_SECURE_NO_WARNINGS

#include "Includes.h"
#include "lazyimporter.h"
#include "xorstr.h"
#include <sstream>

uintptr_t UWORLD;
uintptr_t BONEMATRIX;
uintptr_t FREEFN;
uintptr_t PROJECTWORLDTOSCREEN;
uintptr_t LINEOFSIGHTSO;
uintptr_t GETNAMEBYINDEX;
uintptr_t GOBJECT;

struct FName
{
	union
	{
		struct
		{
			int32_t ComparisonIndex;
			int32_t Number;
		};

		uint64_t CompositeComparisonValue;
	};

	inline FName()
		: ComparisonIndex(0),
		Number(0)
	{
	};

	inline FName(int32_t i)
		: ComparisonIndex(i),
		Number(0)
	{
	};

	inline bool operator==(const FName& other) const
	{
		return ComparisonIndex == other.ComparisonIndex;
	};
};

class UClass {
public:
	BYTE _padding_0[0x40];
	UClass* SuperClass;
};

class UObject {
public:
	PVOID VTableObject;
	DWORD ObjectFlags;
	DWORD InternalIndex;
	UClass* Class;
	FName  Name;
	UObject* Outer;
	

	BOOL IsA(PVOID parentClass) {
		if (((uintptr_t)this))
		{
			UObject* object = (UObject*)this;
			for (auto super = object->Class; super; super = super->SuperClass) {
				if (super == parentClass) {
					return true;
				}
			}
		}
		return false;
	}
};


class UField : public UObject
{
public:
	class UField* Next;                                                     // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
};

class UStruct : public UField
{
public:
	char                                               pad_0030[0x10];                                           // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	class UStruct* SuperField;                                               // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	class UField* Children;                                                 // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	void* ChildProperties;                                          // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	int32_t                                            PropertySize;                                             // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	int32_t                                            MinAlignment;                                             // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	char                                               pad_0060[0x50];                                           // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
};

UObject* ROBOTOFONT;
UObject* DRAWLINE;
UObject* DRAWTEXT;

class FUObjectItem {
public:
	UObject* Object;
	DWORD Flags;
	DWORD ClusterIndex;
	DWORD SerialNumber;
	DWORD SerialNumber2;
};

class TUObjectArray {
public:
	FUObjectItem* Objects[9];
};

class GObjects {
public:
	TUObjectArray* ObjectArray;
	BYTE _padding_0[0xC];
	DWORD ObjectCount;
};

class FTextData {
public:
	char pad_0x0000[0x30];  //0x0000
	wchar_t* Name;          //0x0028 
	__int32 Length;         //0x0030 

};

struct FText {
	FTextData* Data;
	char UnknownData[0x10];

	wchar_t* Get() const {
		if (Data) {
			return Data->Name;
		}

		return nullptr;
	}
};


template<class T>
struct TArray
{
	friend struct FString;

public:
	inline TArray()
	{
		Data = nullptr;
		Count = Max = 0;
	};

	inline int Num() const
	{
		return Count;
	};

	inline T& operator[](int i)
	{
		return Data[i];
	};

	inline const T& operator[](int i) const
	{
		return Data[i];
	};

	inline bool IsValidIndex(int i) const
	{
		return i < Num();
	}

private:
	T* Data;
	int32_t Count;
	int32_t Max;
};

struct FString : private TArray<wchar_t>
{
	inline FString()
	{
	};

	FString(const wchar_t* other)
	{
		Max = Count = *other ? std::wcslen(other) + 1 : 0;

		if (Count)
		{
			Data = const_cast<wchar_t*>(other);
		}
	};

	inline bool IsValid() const
	{
		return Data != nullptr;
	}

	inline const wchar_t* c_str() const
	{
		return Data;
	}

	std::string ToString() const
	{
		auto length = std::wcslen(Data);

		std::string str(length, '\0');

		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

		return str;
	}
};

static GObjects* objects = nullptr;

static void FreeFN(__int64 address)
{
	auto func = reinterpret_cast<__int64(__fastcall*)(__int64 a1)>(FREEFN);

	func(address);
}

static const char* GetObjectName(uintptr_t Object)
{
	if (Object == NULL)
		return ("");

	auto fGetObjName = reinterpret_cast<FString * (__fastcall*)(int* index, FString * res)>(GETNAMEBYINDEX);

	int index = *(int*)(Object + 0x18);

	FString result;
	fGetObjName(&index, &result);

	if (result.c_str() == NULL)
		return ("");

	auto result_str = result.ToString();

	if (result.c_str() != NULL)
		FreeFN((__int64)result.c_str());

	return result_str.c_str();
}

static const char* GetUObjectNameLoop(UObject* Object) {
	std::string name("");

	for (auto i = 0; Object; Object = Object->Outer, ++i) {

		auto fGetObjName = reinterpret_cast<FString * (__fastcall*)(int* index, FString * res)>(GETNAMEBYINDEX);

		int index = *(int*)(reinterpret_cast<uint64_t>(Object) + 0x18);

		FString internalName;
		fGetObjName(&index, &internalName);

		if (internalName.c_str() == NULL) {
			break;
		}

		auto objectName = internalName.ToString();


		name = objectName.c_str() + std::string(i > 0 ? xorstr(".") : xorstr("")) + name;
		FreeFN((__int64)internalName.c_str());
	}

	return name.c_str();
}

static PVOID FindObject(const char* name) {

	for (auto array : objects->ObjectArray->Objects) {
		auto fuObject = array;
		std::cout << "";
		for (auto i = 0; i < 0x10000 && fuObject->Object; ++i, ++fuObject)
		{
			std::cout << "";
			auto object = fuObject->Object;

			if (object->ObjectFlags != 0x41) {
				continue;
			}
			std::cout << "";

			if (strstr(GetUObjectNameLoop(object), name)) {
				return object;
			}
		}
	}

	return 0;
}

static void eraseSubStr(std::string& mainStr, const std::string& toErase)
{
	size_t pos = mainStr.find(toErase);
	if (pos != std::string::npos)
	{
		mainStr.erase(pos, toErase.length());
	}
}

static PVOID FindAllObjects(const char* name, const char* name2) {
	for (auto array : objects->ObjectArray->Objects) {
		auto fuObject = array;
		std::cout << "";
		for (auto i = 0; i < 0x10000 && fuObject->Object; ++i, ++fuObject)
		{
			auto object = fuObject->Object;

			std::string objname = GetUObjectNameLoop(object);
			if (strstr(objname.c_str(), name)) {
				eraseSubStr(objname, name);
				if (strstr(objname.c_str(), name2)) {
					return object;
				}
			}
		}
	}
	return 0;
}

static PVOID FindObject2(const char* name, const char* name2) {

	for (auto array : objects->ObjectArray->Objects) {
		auto fuObject = array;
		std::cout << "";
		for (auto i = 0; i < 0x10000 && fuObject->Object; ++i, ++fuObject)
		{
			std::cout << "";
			auto object = fuObject->Object;

			if (object->ObjectFlags != 0x41) {
				continue;
			}

			if ((strstr(GetUObjectNameLoop(object), name)) and (strstr(GetUObjectNameLoop(object), name2))) {
				return object;
			}
		}
	}

	return 0;
}


void NumChunks(TUObjectArray* ObjectArray, int* start, int* end)
{
	int cStart = 0, cEnd = 0;

	if (!cEnd)
	{
		while (true)
		{
			if (ObjectArray->Objects[cStart] == nullptr)
			{
				cStart++;
			}
			else
			{
				break;
			}
		}

		cEnd = cStart;
		while (true)
		{
			if (ObjectArray->Objects[cEnd] == nullptr)
			{
				break;
			}
			cEnd++;
		}
	}

	*start = cStart;
	*end = cEnd;
}

inline UObject* GetObjByIndex(TUObjectArray* ObjectArray, int32_t index)
{
	int cStart = 0, cEnd = 0;
	int chunkIndex, chunkSize = 0xFFFF, chunkPos;
	FUObjectItem* Object;

	NumChunks(ObjectArray, &cStart, &cEnd);

	chunkIndex = index / chunkSize;
	if (chunkSize * chunkIndex != 0 &&
		chunkSize * chunkIndex == index)
	{
		chunkIndex--;
	}

	chunkPos = cStart + chunkIndex;
	if (chunkPos < cEnd)
	{
		Object = ObjectArray->Objects[chunkPos] + (index - chunkSize * chunkIndex);
		if (!Object) { return nullptr; }

		return Object->Object;
	}

	return nullptr;
}


inline const wchar_t* fix_objectname(const wchar_t* objname)
{
	wchar_t* _4 = (wchar_t*)objname;

	for (int i = 0; i < wcslen(_4); i++)
	{
		if (_4[i] == L'_')
		{
			if (_4[i + 1] == L'0' ||
				_4[i + 1] == L'1' ||
				_4[i + 1] == L'2' ||
				_4[i + 1] == L'3' ||
				_4[i + 1] == L'4' ||
				_4[i + 1] == L'5' ||
				_4[i + 1] == L'6' ||
				_4[i + 1] == L'7' ||
				_4[i + 1] == L'8' ||
				_4[i + 1] == L'9')
				_4[i] = L'$';

			for (int idx = 1; idx < 7; idx++) {

				if (_4[i + idx] == L'.') break;

				if (_4[i + idx] == L'0' ||
					_4[i + idx] == L'1' ||
					_4[i + idx] == L'2' ||
					_4[i + idx] == L'3' ||
					_4[i + idx] == L'4' ||
					_4[i + idx] == L'5' ||
					_4[i + idx] == L'6' ||
					_4[i + idx] == L'7' ||
					_4[i + idx] == L'8' ||
					_4[i + idx] == L'9') {
					_4[i + idx] = L'$';
				}
			}

		}
	}

	std::wstring str = _4;
	str.erase(std::remove(str.begin(), str.end(), '$'), str.end());
	return str.c_str();
}

inline const wchar_t* GetFullObjectName(UObject* object)
{
	std::wstring name = (L"");

	for (auto i = 0; object; object = object->Outer, i++)
	{
		if (!object) return (L"");

		auto function = reinterpret_cast<FString * (__fastcall*)(int* index, FString * res)>(GETNAMEBYINDEX);

		int index = *(int*)(reinterpret_cast<uint64_t>(object) + 0x18);

		FString fObjectName;
		function(&index, &fObjectName);

		if (!fObjectName.IsValid()) break;

		auto objectName = fObjectName.c_str();

		name = objectName + std::wstring(i > 0 ? (L".") : (L"")) + name;
		FreeFN((uintptr_t)fObjectName.c_str());
	}

	return name.c_str();
}

inline PVOID FindObjectAAAAAAAAAAAAAAAAA(const wchar_t* objectname)
{
	for (auto index = 0x0; index < objects->ObjectCount; index++)
	{
		auto object = GetObjByIndex(objects->ObjectArray, index);
		const wchar_t* tmp_name = GetFullObjectName(object);
		const wchar_t* name = fix_objectname(tmp_name);


		if ((wcsstr(name, objectname)))
		{
			return object;
		}
	}

	return 0;
}









class Vector3
{
public:
	double x, y, z;
	Vector3()
	{
		x = y = z = 0.f;
	}

	Vector3(double _x, double _y, double _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3 operator+(const Vector3& v) {
		return Vector3{ x + v.x, y + v.y, z + v.z };
	}

	Vector3 operator-(const Vector3& v) {
		return Vector3{ x - v.x, y - v.y, z - v.z };
	}

	Vector3 operator*(const double v) {
		return Vector3{ x * v, y * v, z * v };
	}

	Vector3 operator/(const double fl) const {
		return Vector3(x / fl, y / fl, z / fl);
	}

	inline double DistanceFrom(const Vector3 Other) {
		double dx = (this->x - Other.x);
		double dy = (this->y - Other.y);
		double dz = (this->z - Other.z);

		return sqrt((dx * dx) + (dy * dy) + (dz * dz));
	}
	inline double Distance(Vector3 v)
	{
		return double(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}
	inline double Length() {
		return sqrt(x * x + y * y + z * z);
	}
};

struct Vector2
{
	double                                              X;                                                        // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	double                                              Y;                                                        // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)

	inline Vector2 operator/(float v) const
	{
		return Vector2(X / v, Y / v);
	}

	inline Vector2 operator+(const Vector2& V) {
		return Vector2(X + V.X, Y + V.Y);
	}

	inline Vector2 operator-(const Vector2& V) {
		return Vector2(X - V.X, Y - V.Y);
	}

	inline Vector2()
		: X(0), Y(0)
	{ }

	inline Vector2(double x, double y)
		: X(x),
		Y(y)
	{ }

};

struct FLinearColor
{
	float                                              R;                                                        // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	float                                              G;                                                        // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	float                                              B;                                                        // 0x0008(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	float                                              A;                                                        // 0x000C(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)

	inline FLinearColor()
		: R(0), G(0), B(0), A(0)
	{ }

	inline FLinearColor(float r, float g, float b, float a)
		: R(r),
		G(g),
		B(b),
		A(a)
	{ }

};



BOOL valid_pointer(DWORD64 address)
{
	if (!IsBadWritePtr((LPVOID)address, (UINT_PTR)8)) return TRUE;
	else return FALSE;
}

template<typename ReadT>
ReadT read(DWORD_PTR address, const ReadT& def = ReadT())
{
	if (valid_pointer(address)) {
		return *(ReadT*)(address);
	}
}

template<typename WriteT>
bool write(DWORD_PTR address, WriteT value, const WriteT& def = WriteT())
{
	if (valid_pointer(address)) {
		*(WriteT*)(address) = value;
		return true;
	}
	return false;
}


static BOOL ProcessEvent(uintptr_t address, void* fnobject, void* parms)
{
	if (!valid_pointer(address)) return FALSE;
	auto index = *reinterpret_cast<void***>(address); if (!index) return FALSE;
	auto fProcessEvent = static_cast<void(*)(void* address, void* fnobject, void* parms)>(index[0x4C]); if (!fProcessEvent) return FALSE;
	fProcessEvent((void*)address, (void*)fnobject, (void*)parms);
	return TRUE;
}


class UCanvas
{
public:

	struct UCanvas_K2_DrawText_Params
	{
		UObject* RenderFont;                                               // (Parm, ZeroConstructor, IsPlainOldData)
		FString                                     RenderText;                                               // (Parm, ZeroConstructor)
		Vector2                                   ScreenPosition;                                           // (Parm, ZeroConstructor, IsPlainOldData)
		Vector2                                   Scale;                                                    // (Parm, ZeroConstructor, IsPlainOldData)
		FLinearColor                                RenderColor;                                              // (Parm, ZeroConstructor, IsPlainOldData)
		float                                       Kerning;                                                  // (Parm, ZeroConstructor, IsPlainOldData)
		FLinearColor                                ShadowColor;                                              // (Parm, ZeroConstructor, IsPlainOldData)
		Vector2                                   ShadowOffset;                                             // (Parm, ZeroConstructor, IsPlainOldData)
		bool                                        bCentreX;                                                 // (Parm, ZeroConstructor, IsPlainOldData)
		bool                                        bCentreY;                                                 // (Parm, ZeroConstructor, IsPlainOldData)
		bool                                        bOutlined;                                                // (Parm, ZeroConstructor, IsPlainOldData)
		FLinearColor                                OutlineColor;                                             // (Parm, ZeroConstructor, IsPlainOldData)
	};

	struct UCanvas_K2_DrawLine_Params
	{
		Vector2                            ScreenPositionA;                                                // (Parm, ZeroConstructor, IsPlainOldData)
		Vector2                            ScreenPositionB;                                                // (Parm, ZeroConstructor, IsPlainOldData)
		float							   Thickness;
		FLinearColor                       RenderColor;                                             // (Parm, ZeroConstructor, IsPlainOldData)
	};

	VOID K2_DrawLine(Vector3 ScreenPositionA, Vector3 ScreenPositionB, FLinearColor RenderColor, float Thickness)
	{
		UCanvas_K2_DrawLine_Params params;
		params.ScreenPositionA = Vector2(ScreenPositionA.x, ScreenPositionA.y);
		params.ScreenPositionB = Vector2(ScreenPositionB.x, ScreenPositionB.y);
		params.Thickness = Thickness;
		params.RenderColor = RenderColor;

		ProcessEvent(uintptr_t(this), DRAWLINE, &params);
	}

	VOID K2_DrawText(Vector3 ScreenPosition, FString RenderText, FLinearColor RenderColor, bool centered, bool bOutlined)
	{
		UCanvas_K2_DrawText_Params params;
		params.RenderFont = ROBOTOFONT;
		params.RenderText = RenderText;
		params.ScreenPosition = Vector2(ScreenPosition.x, ScreenPosition.y);
		params.Scale = Vector2{ 0.5f, 0.5f };
		params.RenderColor = RenderColor;
		params.Kerning = false;
		params.ShadowColor = { 0.0f, 0.0f, 0.0f, 1.f };
		params.ShadowOffset = Vector2{ -1, -1 };
		if (centered) {
			params.bCentreX = true;
			params.bCentreY = true;
		}
		else {
			params.bCentreX = false;
			params.bCentreY = false;
		}
		params.bOutlined = bOutlined;
		params.OutlineColor = { 0.0f, 0.0f, 0.0f, 1.f };
		ProcessEvent((uintptr_t)this, DRAWTEXT, &params);
	}

	VOID K2_DrawTextAMK(Vector3 ScreenPosition, FString RenderText, FLinearColor RenderColor, bool centered, bool bOutlined)
	{
		UCanvas_K2_DrawText_Params params;
		params.RenderFont = ROBOTOFONT;
		params.RenderText = RenderText;
		params.ScreenPosition = Vector2(ScreenPosition.x, ScreenPosition.y);
		params.Scale = Vector2{ 0.9f, 0.9f };
		params.RenderColor = RenderColor;
		params.Kerning = false;
		params.ShadowColor = { 0.0f, 0.0f, 0.0f, 1.f };
		params.ShadowOffset = Vector2{ -1, -1 };
		if (centered) {
			params.bCentreX = true;
			params.bCentreY = true;
		}
		else {
			params.bCentreX = false;
			params.bCentreY = false;
		}
		params.bOutlined = bOutlined;
		params.OutlineColor = { 0.0f, 0.0f, 0.0f, 1.f };
		ProcessEvent((uintptr_t)this, DRAWTEXT, &params);
	}

	Vector2 K2_StrLen(FString RenderText)
	{
		static PVOID fn = 0; if (!fn) fn = FindObject("K2_StrLen");

		struct
		{
			class UObject* RenderFont;
			struct FString                 RenderText;
			struct Vector2               ReturnValue;
		} params;

		params.RenderFont = ROBOTOFONT;
		params.RenderText = RenderText;

		ProcessEvent((uintptr_t)this, fn, &params);

		return params.ReturnValue;
	}

	Vector2 K2_TextSize(FString RenderText)
	{
		static PVOID fn = 0; if (!fn) fn = FindObject("K2_StrLen");

		struct
		{
			class UObject* RenderFont;
			struct FString                 RenderText;
			Vector2                                   Scale;                                                    // (Parm, ZeroConstructor, IsPlainOldData)
			Vector2                                   ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
		} params;

		params.RenderFont = ROBOTOFONT;
		params.RenderText = RenderText;
		params.Scale = { 0.5f, 0.5f };

		ProcessEvent(uintptr_t(this), fn, &params);

		return { params.ReturnValue.X, params.ReturnValue.Y };
	}

	void Draw2DBoundingBox(Vector3 StartBoxLoc, float flWidth, float Height, FLinearColor color)
	{
		StartBoxLoc.x = StartBoxLoc.x - (flWidth / 2);
		this->K2_DrawLine(Vector3(StartBoxLoc.x, StartBoxLoc.y, 0), Vector3(StartBoxLoc.x + flWidth, StartBoxLoc.y, 0), color, 2); //bottom
		this->K2_DrawLine(Vector3(StartBoxLoc.x, StartBoxLoc.y, 0), Vector3(StartBoxLoc.x, StartBoxLoc.y + Height, 0), color, 2); //left
		this->K2_DrawLine(Vector3(StartBoxLoc.x + flWidth, StartBoxLoc.y, 0), Vector3(StartBoxLoc.x + flWidth, StartBoxLoc.y + Height, 0), color, 2); //right
		this->K2_DrawLine(Vector3(StartBoxLoc.x, StartBoxLoc.y + Height, 0), Vector3(StartBoxLoc.x + flWidth, StartBoxLoc.y + Height, 0), color, 2); //up
	}

	void DrawCorneredBox(int X, int Y, int W, int H, FLinearColor color, int thickness) {
		float lineW = (W / 3);
		float lineH = (H / 3);
		this->K2_DrawLine(Vector3(X, Y, 0), Vector3(X, Y + lineH, 0), color, thickness);

		this->K2_DrawLine(Vector3(X, Y, 0), Vector3(X + lineW, Y, 0), color, thickness);

		this->K2_DrawLine(Vector3(X + W - lineW, Y, 0), Vector3(X + W, Y, 0), color, thickness);

		this->K2_DrawLine(Vector3(X + W, Y, 0), Vector3(X + W, Y + lineH, 0), color, thickness);

		this->K2_DrawLine(Vector3(X, Y + H - lineH, 0), Vector3(X, Y + H, 0), color, thickness);

		this->K2_DrawLine(Vector3(X, Y + H, 0), Vector3(X + lineW, Y + H, 0), color, thickness);

		this->K2_DrawLine(Vector3(X + W - lineW, Y + H, 0), Vector3(X + W, Y + H, 0), color, thickness);

		this->K2_DrawLine(Vector3(X + W, Y + H - lineH, 0), Vector3(X + W, Y + H, 0), color, thickness);

	}
};

static bool K2_SetActorRotation(uintptr_t a, Vector3 NewRotation, bool bTeleportPhysics)
{
	static PVOID fn = nullptr;
	if (!fn) fn = FindObject("K2_SetActorRotation");

	struct AActor_K2_SetActorLocation_Params
	{
		struct Vector3                                     NewRotation;                                              // (Parm, IsPlainOldData)                                        // (Parm, OutParm, IsPlainOldData)
		bool                                               bTeleport;                                                // (Parm, ZeroConstructor, IsPlainOldData)                                          // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
	};

	AActor_K2_SetActorLocation_Params params;

	params.NewRotation = NewRotation;
	params.bTeleport = false;
	ProcessEvent(a, fn, &params);


	return true;
}

//int myceilf(float num)
//{
//	int inum = (int)num;
//	if (num == (float)inum) {
//		return inum;
//	}
//	return inum + 1;
//}
////
//}

namespace StaticOffsets {
	uintptr_t OwningGameInstance = 0x1b8;
	uintptr_t LocalPlayers = 0x38;
	uintptr_t PlayerController = 0x30;
	uintptr_t PlayerCameraManager = 0x340;
	uintptr_t AcknowledgedPawn = 0x330;
	uintptr_t PrimaryPickupItemEntry = 0x318;
	uintptr_t ItemDefinition = 0x3F0;
	uintptr_t DisplayName = 0x90;
	uintptr_t Tier = 0x74;
	uintptr_t WeaponData = 0x3F0;
	uintptr_t LastFireTime = 0xAB0;
	uintptr_t LastFireTimeVerified = 0xAB4;
	uintptr_t LastFireAbilityTime = 0xDF0;
	uintptr_t CurrentWeapon = 0x8d8;
	uintptr_t bADSWhileNotOnGround = 0x4B35;

	uintptr_t Levels = 0x170;
	uintptr_t PersistentLevel = 0x30;
	uintptr_t AActors = 0x98;
	uintptr_t ActorCount = 0xA0;

	uintptr_t Spinbot = 0x310 + 0x140;
	uintptr_t RootComponent = 0x190;
	uintptr_t FireStartLoc = 0xA20;
	uintptr_t RelativeLocation = 0x128;
	uintptr_t RelativeRotation = 0x140;
	uintptr_t PlayerState = 0x2a8;
	uintptr_t Mesh = 0x310;
	uintptr_t TeamIndex = 0x10e8;
}


struct FMatrix
{
	double M[4][4];
};
static FMatrix* myMatrix = new FMatrix();

static bool GetBoneLocation(uintptr_t CurrentActor, int id, Vector3* out)
{
	uintptr_t mesh = read<uintptr_t>(CurrentActor + StaticOffsets::Mesh);
	if (!mesh) return false;

	auto fGetBoneMatrix = ((FMatrix * (__fastcall*)(uintptr_t, FMatrix*, int))(BONEMATRIX));
	fGetBoneMatrix(mesh, myMatrix, id);

	out->x = myMatrix->M[3][0];
	out->y = myMatrix->M[3][1];
	out->z = myMatrix->M[3][2];

	return true;
}

static bool WorldToScreen(uintptr_t PlayerController, Vector3 WorldLocation, Vector3* out)
{

	auto WorldToScreen = reinterpret_cast<bool(__fastcall*)(uintptr_t pPlayerController, Vector3 vWorldPos, Vector3 * vScreenPosOut, char)>(PROJECTWORLDTOSCREEN);

	WorldToScreen((uintptr_t)PlayerController, WorldLocation, out, (char)0);

	return true;
}

bool CheckInScreen(uintptr_t PlayerController, uintptr_t CurrentActor, int Width, int Height) {
	Vector3 Pos;
	GetBoneLocation(CurrentActor, 98, &Pos);
	WorldToScreen(PlayerController, Pos, &Pos);
	if (CurrentActor)
	{
		if (((Pos.x <= 0 or Pos.x > Width) and (Pos.y <= 0 or Pos.y > Height)) or ((Pos.x <= 0 or Pos.x > Width) or (Pos.y <= 0 or Pos.y > Height))) {
			return false;
		}
		else {
			return true;
		}

	}
}

namespace MemoryHelper {
	uintptr_t PatternScanW(uintptr_t pModuleBaseAddress, const char* sSignature, size_t nSelectResultIndex = 0)
	{
		static auto patternToByte = [](const char* pattern)
		{
			auto       bytes = std::vector<int>{};
			const auto start = const_cast<char*>(pattern);
			const auto end = const_cast<char*>(pattern) + strlen(pattern);

			for (auto current = start; current < end; ++current)
			{
				if (*current == '?')
				{
					++current;
					if (*current == '?')
						++current;
					bytes.push_back(-1);
				}
				else
					bytes.push_back(strtoul(current, &current, 16));
			}
			return bytes;
		};

		const auto dosHeader = (PIMAGE_DOS_HEADER)pModuleBaseAddress;
		const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)pModuleBaseAddress + dosHeader->e_lfanew);

		const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
		auto       patternBytes = patternToByte(sSignature);
		const auto scanBytes = reinterpret_cast<std::uint8_t*>(pModuleBaseAddress);

		const auto s = patternBytes.size();
		const auto d = patternBytes.data();

		size_t nFoundResults = 0;

		for (auto i = 0ul; i < sizeOfImage - s; ++i)
		{
			bool found = true;

			for (auto j = 0ul; j < s; ++j)
			{
				if (scanBytes[i + j] != d[j] && d[j] != -1)
				{
					found = false;
					break;
				}
			}

			if (found)
			{
				if (nSelectResultIndex != 0)
				{
					if (nFoundResults < nSelectResultIndex)
					{
						nFoundResults++;
						found = false;
					}
					else
						return reinterpret_cast<uintptr_t>(&scanBytes[i]);
				}
				else
					return reinterpret_cast<uintptr_t>(&scanBytes[i]);
			}
		}

		return NULL;
	}

	uintptr_t PatternScan(const char* sSignature, size_t nSelectResultIndex = 0)
	{
		static bool bIsSetted = false;

		static MODULEINFO info = { 0 };

		if (!bIsSetted)
		{

			LI_FN(GetModuleInformation)(LI_FN(GetCurrentProcess)(), LI_FN(GetModuleHandleA)(xorstr("FortniteClient-Win64-Shipping.exe")), &info, sizeof(info));
			bIsSetted = true;
		}
		return PatternScanW((uintptr_t)info.lpBaseOfDll, sSignature, nSelectResultIndex);
	}
}

void BoneDebugA(UCanvas* Canvas, uintptr_t CurrentActor, uintptr_t PlayerController)
{
	Vector3 TestEichel;
	for (int a = 0; a < 101; a++)
	{
		GetBoneLocation(CurrentActor, a, &TestEichel);
		std::wstringstream ss;
		ss << a;
		std::wstring str = ss.str();
		WorldToScreen(PlayerController, TestEichel, &TestEichel);
		Canvas->K2_DrawText(TestEichel, str.c_str(), FLinearColor(1.f, 0.f, 0.f, 1.f), 0, 0);
	}
}

struct FHitResult
{
	char memes[0x1000];
};

static bool K2_SetActorLocationAndRotation(uintptr_t a, Vector3 NewLocation, Vector3 NewRotation, bool bSweep, bool bTeleport, struct FHitResult* SweepHitResult)
{
	static PVOID fn = nullptr;
	if (!fn) fn = FindObject("K2_SetActorLocationAndRotation");

	// Function Engine.Actor.K2_SetActorLocation
	struct AActor_K2_SetActorLocation_Params
	{
		struct Vector3                                     NewLocation;                                              // (Parm, IsPlainOldData)
		struct Vector3                                     NewRotation;                                              // (Parm, IsPlainOldData)
		bool                                               bSweep;                                                   // (Parm, ZeroConstructor, IsPlainOldData)
		struct FHitResult                                  SweepHitResult;                                           // (Parm, OutParm, IsPlainOldData)
		bool                                               bTeleport;                                                // (Parm, ZeroConstructor, IsPlainOldData)
		bool                                               ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
	};

	AActor_K2_SetActorLocation_Params params;

	params.NewLocation = NewLocation;
	params.NewRotation = NewRotation;
	params.bSweep = false;
	params.bTeleport = false;
	ProcessEvent(a, fn, &params);

	return true;
}

FString GetPlayerName(uintptr_t player)
{
	static PVOID fn = 0; if (!fn) fn = FindObject("GetPlayerName");

	struct
	{
		struct FString                 ReturnValue;
	} params;


	ProcessEvent(player, fn, &params);

	auto ret = params.ReturnValue;
	return ret;
}

void __forceinline AnsiToWide(char* inAnsi, wchar_t* outWide)
{
	int i = 0;
	for (; inAnsi[i] != '\0'; i++)
		outWide[i] = (wchar_t)(inAnsi)[i];
	outWide[i] = L'\0';
}

FText GetShortDisplayName(uintptr_t WeaponData) {
	static PVOID fn = 0; if (!fn) fn = FindObject("GetShortDisplayName");

	struct
	{
		struct FText                 ReturnValue;
	} params;


	ProcessEvent(WeaponData, fn, &params);

	auto ret = params.ReturnValue;
	return ret;
};

struct PlayerController_FOV_Params
{
	float                                              NewFOV_69;                                                // (Parm, ZeroConstructor, IsPlainOldData)
};

void FOV(uintptr_t PlayerController, float NewFOV_69)
{
	static PVOID fn = NULL;
	if (!fn)
		fn = FindObject("FOV");

	PlayerController_FOV_Params params;
	params.NewFOV_69 = NewFOV_69;

	ProcessEvent(PlayerController, fn, &params);
}

struct Controller_K2_GetPawn_Params
{
	uintptr_t ReturnValue_69;                                           // (Parm, OutParm, ZeroConstructor, ReturnParm)
};

uintptr_t K2_GetPawn(uintptr_t PlayerController)
{
	static PVOID fn = NULL;
	if (!fn)
		fn = FindObject("K2_GetPawn");

	Controller_K2_GetPawn_Params params;

	ProcessEvent(PlayerController, fn, &params);
	return params.ReturnValue_69;
}

struct Actor_32759_K2_TeleportTo_Params
{
	struct Vector3                                     DestLocation_69;                                          // (Parm, ZeroConstructor, IsPlainOldData)
	struct Vector3                                     DestRotation_69;                                          // (Parm, ZeroConstructor, IsPlainOldData)
	bool                                               ReturnValue_69;                                           // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
};

void K2_TeleportTo(uintptr_t CurA, Vector3 DestLocation_69, Vector3 DestRotation_69)
{
	static PVOID fn = NULL;
	if (!fn)
		fn = FindObject("K2_TeleportTo");

	Actor_32759_K2_TeleportTo_Params params;
	params.DestLocation_69 = DestLocation_69;
	params.DestRotation_69 = DestRotation_69;


	ProcessEvent(CurA, fn, &params);
}

enum class EFortItemTier : uint8_t
{
	No_Tier = 0,
	I = 1,
	II = 2,
	III = 3,
	IV = 4,
	V = 5,
	VI = 6,
	VII = 7,
	VIII = 8,
	IX = 9,
	X = 10,
	NumItemTierValues = 11,
	EFortItemTier_MAX = 12
};

namespace Colors
{
	FLinearColor AliceBlue = { 0.941176534f, 0.972549081f, 1.000000000f, 1.000000000f };
	FLinearColor AntiqueWhite = { 0.980392218f, 0.921568692f, 0.843137324f, 1.000000000f };
	FLinearColor Aqua = { 0.000000000f, 1.000000000f, 1.000000000f, 1.000000000f };
	FLinearColor Aquamarine = { 0.498039246f, 1.000000000f, 0.831372619f, 1.000000000f };
	FLinearColor Azure = { 0.941176534f, 1.000000000f, 1.000000000f, 1.000000000f };
	FLinearColor Beige = { 0.960784376f, 0.960784376f, 0.862745166f, 1.000000000f };
	FLinearColor Bisque = { 1.000000000f, 0.894117713f, 0.768627524f, 1.000000000f };
	FLinearColor Black = { 0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f };
	FLinearColor BlanchedAlmond = { 1.000000000f, 0.921568692f, 0.803921640f, 1.000000000f };
	FLinearColor Blue = { 0.000000000f, 0.000000000f, 1.000000000f, 1.000000000f };
	FLinearColor BlueViolet = { 0.541176498f, 0.168627456f, 0.886274576f, 1.000000000f };
	FLinearColor Brown = { 0.647058845f, 0.164705887f, 0.164705887f, 1.000000000f };
	FLinearColor BurlyWood = { 0.870588303f, 0.721568644f, 0.529411793f, 1.000000000f };
	FLinearColor CadetBlue = { 0.372549027f, 0.619607866f, 0.627451003f, 1.000000000f };
	FLinearColor Chartreuse = { 0.498039246f, 1.000000000f, 0.000000000f, 1.000000000f };
	FLinearColor Chocolate = { 0.823529482f, 0.411764741f, 0.117647067f, 1.000000000f };
	FLinearColor Coral = { 1.000000000f, 0.498039246f, 0.313725501f, 1.000000000f };
	FLinearColor CornflowerBlue = { 0.392156899f, 0.584313750f, 0.929411829f, 1.000000000f };
	FLinearColor Cornsilk = { 1.000000000f, 0.972549081f, 0.862745166f, 1.000000000f };
	FLinearColor Crimson = { 0.862745166f, 0.078431375f, 0.235294133f, 1.000000000f };
	FLinearColor Cyan = { 0.000000000f, 1.000000000f, 1.000000000f, 1.000000000f };
	FLinearColor DarkBlue = { 0.000000000f, 0.000000000f, 0.545098066f, 1.000000000f };
	FLinearColor DarkCyan = { 0.000000000f, 0.545098066f, 0.545098066f, 1.000000000f };
	FLinearColor DarkGoldenrod = { 0.721568644f, 0.525490224f, 0.043137256f, 1.000000000f };
	FLinearColor DarkGray = { 0.662745118f, 0.662745118f, 0.662745118f, 1.000000000f };
	FLinearColor DarkGreen = { 0.000000000f, 0.392156899f, 0.000000000f, 1.000000000f };
	FLinearColor DarkKhaki = { 0.741176486f, 0.717647076f, 0.419607878f, 1.000000000f };
	FLinearColor DarkMagenta = { 0.545098066f, 0.000000000f, 0.545098066f, 1.000000000f };
	FLinearColor DarkOliveGreen = { 0.333333343f, 0.419607878f, 0.184313729f, 1.000000000f };
	FLinearColor DarkOrange = { 1.000000000f, 0.549019635f, 0.000000000f, 1.000000000f };
	FLinearColor DarkOrchid = { 0.600000024f, 0.196078449f, 0.800000072f, 1.000000000f };
	FLinearColor DarkRed = { 0.545098066f, 0.000000000f, 0.000000000f, 1.000000000f };
	FLinearColor DarkSalmon = { 0.913725555f, 0.588235319f, 0.478431404f, 1.000000000f };
	FLinearColor DarkSeaGreen = { 0.560784340f, 0.737254918f, 0.545098066f, 1.000000000f };
	FLinearColor DarkSlateBlue = { 0.282352954f, 0.239215702f, 0.545098066f, 1.000000000f };
	FLinearColor DarkSlateGray = { 0.184313729f, 0.309803933f, 0.309803933f, 1.000000000f };
	FLinearColor DarkTurquoise = { 0.000000000f, 0.807843208f, 0.819607913f, 1.000000000f };
	FLinearColor DarkViolet = { 0.580392182f, 0.000000000f, 0.827451050f, 1.000000000f };
	FLinearColor DeepPink = { 1.000000000f, 0.078431375f, 0.576470613f, 1.000000000f };
	FLinearColor DeepSkyBlue = { 0.000000000f, 0.749019623f, 1.000000000f, 1.000000000f };
	FLinearColor DimGray = { 0.411764741f, 0.411764741f, 0.411764741f, 1.000000000f };
	FLinearColor DodgerBlue = { 0.117647067f, 0.564705908f, 1.000000000f, 1.000000000f };
	FLinearColor Firebrick = { 0.698039234f, 0.133333340f, 0.133333340f, 1.000000000f };
	FLinearColor FloralWhite = { 1.000000000f, 0.980392218f, 0.941176534f, 1.000000000f };
	FLinearColor ForestGreen = { 0.133333340f, 0.545098066f, 0.133333340f, 1.000000000f };
	FLinearColor Fuchsia = { 1.000000000f, 0.000000000f, 1.000000000f, 1.000000000f };
	FLinearColor Gainsboro = { 0.862745166f, 0.862745166f, 0.862745166f, 1.000000000f };
	FLinearColor GhostWhite = { 0.972549081f, 0.972549081f, 1.000000000f, 1.000000000f };
	FLinearColor Gold = { 1.000000000f, 0.843137324f, 0.000000000f, 1.000000000f };
	FLinearColor Goldenrod = { 0.854902029f, 0.647058845f, 0.125490203f, 1.000000000f };
	FLinearColor Gray = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
	FLinearColor Green = { 0.000000000f, 0.501960814f, 0.000000000f, 1.000000000f };
	FLinearColor GreenYellow = { 0.678431392f, 1.000000000f, 0.184313729f, 1.000000000f };
	FLinearColor Honeydew = { 0.941176534f, 1.000000000f, 0.941176534f, 1.000000000f };
	FLinearColor HotPink = { 1.000000000f, 0.411764741f, 0.705882370f, 1.000000000f };
	FLinearColor IndianRed = { 0.803921640f, 0.360784322f, 0.360784322f, 1.000000000f };
	FLinearColor Indigo = { 0.294117659f, 0.000000000f, 0.509803951f, 1.000000000f };
	FLinearColor Ivory = { 1.000000000f, 1.000000000f, 0.941176534f, 1.000000000f };
	FLinearColor Khaki = { 0.941176534f, 0.901960850f, 0.549019635f, 1.000000000f };
	FLinearColor Lavender = { 0.901960850f, 0.901960850f, 0.980392218f, 1.000000000f };
	FLinearColor LavenderBlush = { 1.000000000f, 0.941176534f, 0.960784376f, 1.000000000f };
	FLinearColor LawnGreen = { 0.486274540f, 0.988235354f, 0.000000000f, 1.000000000f };
	FLinearColor LemonChiffon = { 1.000000000f, 0.980392218f, 0.803921640f, 1.000000000f };
	FLinearColor LightBlue = { 0.678431392f, 0.847058892f, 0.901960850f, 1.000000000f };
	FLinearColor LightCoral = { 0.941176534f, 0.501960814f, 0.501960814f, 1.000000000f };
	FLinearColor LightCyan = { 0.878431439f, 1.000000000f, 1.000000000f, 1.000000000f };
	FLinearColor LightGoldenrodYellow = { 0.980392218f, 0.980392218f, 0.823529482f, 1.000000000f };
	FLinearColor LightGreen = { 0.564705908f, 0.933333397f, 0.564705908f, 1.000000000f };
	FLinearColor LightGray = { 0.827451050f, 0.827451050f, 0.827451050f, 1.000000000f };
	FLinearColor LightPink = { 1.000000000f, 0.713725507f, 0.756862819f, 1.000000000f };
	FLinearColor LightSalmon = { 1.000000000f, 0.627451003f, 0.478431404f, 1.000000000f };
	FLinearColor LightSeaGreen = { 0.125490203f, 0.698039234f, 0.666666687f, 1.000000000f };
	FLinearColor LightSkyBlue = { 0.529411793f, 0.807843208f, 0.980392218f, 1.000000000f };
	FLinearColor LightSlateGray = { 0.466666698f, 0.533333361f, 0.600000024f, 1.000000000f };
	FLinearColor LightSteelBlue = { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };
	FLinearColor LightYellow = { 1.000000000f, 1.000000000f, 0.878431439f, 1.000000000f };
	FLinearColor Lime = { 0.000000000f, 1.000000000f, 0.000000000f, 1.000000000f };
	FLinearColor LimeGreen = { 0.196078449f, 0.803921640f, 0.196078449f, 1.000000000f };
	FLinearColor Linen = { 0.980392218f, 0.941176534f, 0.901960850f, 1.000000000f };
	FLinearColor Magenta = { 1.000000000f, 0.000000000f, 1.000000000f, 1.000000000f };
	FLinearColor Maroon = { 0.501960814f, 0.000000000f, 0.000000000f, 1.000000000f };
	FLinearColor MediumAquamarine = { 0.400000036f, 0.803921640f, 0.666666687f, 1.000000000f };
	FLinearColor MediumBlue = { 0.000000000f, 0.000000000f, 0.803921640f, 1.000000000f };
	FLinearColor MediumOrchid = { 0.729411781f, 0.333333343f, 0.827451050f, 1.000000000f };
	FLinearColor MediumPurple = { 0.576470613f, 0.439215720f, 0.858823597f, 1.000000000f };
	FLinearColor MediumSeaGreen = { 0.235294133f, 0.701960802f, 0.443137288f, 1.000000000f };
	FLinearColor MediumSlateBlue = { 0.482352972f, 0.407843173f, 0.933333397f, 1.000000000f };
	FLinearColor MediumSpringGreen = { 0.000000000f, 0.980392218f, 0.603921592f, 1.000000000f };
	FLinearColor MediumTurquoise = { 0.282352954f, 0.819607913f, 0.800000072f, 1.000000000f };
	FLinearColor MediumVioletRed = { 0.780392230f, 0.082352944f, 0.521568656f, 1.000000000f };
	FLinearColor MidnightBlue = { 0.098039225f, 0.098039225f, 0.439215720f, 1.000000000f };
	FLinearColor MintCream = { 0.960784376f, 1.000000000f, 0.980392218f, 1.000000000f };
	FLinearColor MistyRose = { 1.000000000f, 0.894117713f, 0.882353008f, 1.000000000f };
	FLinearColor Moccasin = { 1.000000000f, 0.894117713f, 0.709803939f, 1.000000000f };
	FLinearColor NavajoWhite = { 1.000000000f, 0.870588303f, 0.678431392f, 1.000000000f };
	FLinearColor Navy = { 0.000000000f, 0.000000000f, 0.501960814f, 1.000000000f };
	FLinearColor OldLace = { 0.992156923f, 0.960784376f, 0.901960850f, 1.000000000f };
	FLinearColor Olive = { 0.501960814f, 0.501960814f, 0.000000000f, 1.000000000f };
	FLinearColor OliveDrab = { 0.419607878f, 0.556862772f, 0.137254909f, 1.000000000f };
	FLinearColor Orange = { 1.000000000f, 0.647058845f, 0.000000000f, 1.000000000f };
	FLinearColor OrangeRed = { 1.000000000f, 0.270588249f, 0.000000000f, 1.000000000f };
	FLinearColor Orchid = { 0.854902029f, 0.439215720f, 0.839215755f, 1.000000000f };
	FLinearColor PaleGoldenrod = { 0.933333397f, 0.909803987f, 0.666666687f, 1.000000000f };
	FLinearColor PaleGreen = { 0.596078455f, 0.984313786f, 0.596078455f, 1.000000000f };
	FLinearColor PaleTurquoise = { 0.686274529f, 0.933333397f, 0.933333397f, 1.000000000f };
	FLinearColor PaleVioletRed = { 0.858823597f, 0.439215720f, 0.576470613f, 1.000000000f };
	FLinearColor PapayaWhip = { 1.000000000f, 0.937254965f, 0.835294187f, 1.000000000f };
	FLinearColor PeachPuff = { 1.000000000f, 0.854902029f, 0.725490212f, 1.000000000f };
	FLinearColor Peru = { 0.803921640f, 0.521568656f, 0.247058839f, 1.000000000f };
	FLinearColor Pink = { 1.000000000f, 0.752941251f, 0.796078503f, 1.000000000f };
	FLinearColor Plum = { 0.866666734f, 0.627451003f, 0.866666734f, 1.000000000f };
	FLinearColor PowderBlue = { 0.690196097f, 0.878431439f, 0.901960850f, 1.000000000f };
	FLinearColor Purple = { 0.501960814f, 0.000000000f, 0.501960814f, 1.000000000f };
	FLinearColor Red = { 1.000000000f, 0.000000000f, 0.000000000f, 1.000000000f };
	FLinearColor RosyBrown = { 0.737254918f, 0.560784340f, 0.560784340f, 1.000000000f };
	FLinearColor RoyalBlue = { 0.254901975f, 0.411764741f, 0.882353008f, 1.000000000f };
	FLinearColor SaddleBrown = { 0.545098066f, 0.270588249f, 0.074509807f, 1.000000000f };
	FLinearColor Salmon = { 0.980392218f, 0.501960814f, 0.447058856f, 1.000000000f };
	FLinearColor SandyBrown = { 0.956862807f, 0.643137276f, 0.376470625f, 1.000000000f };
	FLinearColor SeaGreen = { 0.180392161f, 0.545098066f, 0.341176480f, 1.000000000f };
	FLinearColor SeaShell = { 1.000000000f, 0.960784376f, 0.933333397f, 1.000000000f };
	FLinearColor Sienna = { 0.627451003f, 0.321568638f, 0.176470593f, 1.000000000f };
	FLinearColor Silver = { 0.752941251f, 0.752941251f, 0.752941251f, 1.000000000f };
	FLinearColor SkyBlue = { 0.529411793f, 0.807843208f, 0.921568692f, 1.000000000f };
	FLinearColor SlateBlue = { 0.415686309f, 0.352941185f, 0.803921640f, 1.000000000f };
	FLinearColor SlateGray = { 0.439215720f, 0.501960814f, 0.564705908f, 1.000000000f };
	FLinearColor Snow = { 1.000000000f, 0.980392218f, 0.980392218f, 1.000000000f };
	FLinearColor SpringGreen = { 0.000000000f, 1.000000000f, 0.498039246f, 1.000000000f };
	FLinearColor SteelBlue = { 0.274509817f, 0.509803951f, 0.705882370f, 1.000000000f };
	FLinearColor Tan = { 0.823529482f, 0.705882370f, 0.549019635f, 1.000000000f };
	FLinearColor Teal = { 0.000000000f, 0.501960814f, 0.501960814f, 1.000000000f };
	FLinearColor Thistle = { 0.847058892f, 0.749019623f, 0.847058892f, 1.000000000f };
	FLinearColor Tomato = { 1.000000000f, 0.388235331f, 0.278431386f, 1.000000000f };
	FLinearColor Transparent = { 0.000000000f, 0.000000000f, 0.000000000f, 0.000000000f };
	FLinearColor Turquoise = { 0.250980407f, 0.878431439f, 0.815686345f, 1.000000000f };
	FLinearColor Violet = { 0.933333397f, 0.509803951f, 0.933333397f, 1.000000000f };
	FLinearColor Wheat = { 0.960784376f, 0.870588303f, 0.701960802f, 1.000000000f };
	FLinearColor White = { 1.000000000f, 1.000000000f, 1.0f, 1.000000000f };
	FLinearColor WhiteSmoke = { 0.960784376f, 0.960784376f, 0.960784376f, 1.000000000f };
	FLinearColor Yellow = { 1.000000000f, 1.000000000f, 0.000000000f, 1.000000000f };
	FLinearColor YellowGreen = { 0.603921592f, 0.803921640f, 0.196078449f, 1.000000000f };
};

// Function FortniteGame.FortPlayerState.GetPlatform
struct FortPlayerState_GetPlatform_Params
{
	struct FString                                     ReturnValue_69;                                           // (Parm, OutParm, ZeroConstructor, ReturnParm)
};

struct FString GetPlatform(uintptr_t PlayerState)
{
	static PVOID fn = NULL;
	if (!fn)
		fn = FindObject("GetPlatform");

	FortPlayerState_GetPlatform_Params params;

	ProcessEvent(PlayerState, fn, &params);

	return params.ReturnValue_69;
}

int GetBulletsPerClip(uintptr_t Cur)
{
	static PVOID fn = NULL;
	if (!fn)
		fn = FindObject("GetBulletsPerClip");


	struct
	{
		int                                                ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
	} params;


	ProcessEvent(uintptr_t(Cur), fn, &params);
	return params.ReturnValue;
}
int GetMagazineAmmoCount(uintptr_t Cur)
{
	static PVOID fn = NULL;
	if (!fn)
		fn = FindObject("GetMagazineAmmoCount");


	struct
	{
		int                                                ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
	} params;


	ProcessEvent(uintptr_t(Cur), fn, &params);
	return params.ReturnValue;
}


UClass* Projectile = 0;
UClass* Loot = 0;
UClass* PawnAthena = 0;

uintptr_t WeaponStatHandle = 0;


static bool K2_SetActorLocation(uintptr_t a, Vector3& NewLocation, bool bSweep, bool bTeleport, struct FHitResult* SweepHitResult)
{
	static PVOID fn = nullptr;
	if (!fn) fn = FindObject("K2_SetActorLocation");

	// Function Engine.Actor.K2_SetActorLocation
	struct AActor_K2_SetActorLocation_Params
	{
		struct Vector3                                     NewLocation;                                              // (Parm, IsPlainOldData)
		bool                                               bSweep;                                                   // (Parm, ZeroConstructor, IsPlainOldData)
		struct FHitResult                                  SweepHitResult;                                           // (Parm, OutParm, IsPlainOldData)
		bool                                               bTeleport;                                                // (Parm, ZeroConstructor, IsPlainOldData)
		bool                                               ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
	};

	AActor_K2_SetActorLocation_Params params;

	params.NewLocation = NewLocation;
	params.bSweep = bSweep;
	params.bTeleport = bTeleport;
	ProcessEvent(a, fn, &params);

	if (SweepHitResult != nullptr)
		*SweepHitResult = params.SweepHitResult;

	return params.ReturnValue;
}

FLinearColor getColorFromTier(BYTE Tier) {
	FLinearColor color = { 255.f, 255.f, 255.f, 1 };

	if (Tier == 1)
		color = { 255.f, 255.f, 255.f, 1 };
	else if (Tier == 2)
		color = { Colors::LightGreen };
	else if (Tier == 3)
		color = { 0.f, 1.f, 1.f, 1 };
	else if (Tier == 4)
		color = { Colors::DarkCyan };
	else if (Tier == 5)
		color = { 1.f, 1.f, 0.f, 1 };

	return color;
}

BOOL IsA(UObject* A, UClass* parentClass) {
	if (!A) return false;
	UObject* object = (UObject*)A;
	for (auto super = object->Class; super; super = super->SuperClass) {
		if (super == parentClass) {
			return true;
		}
	}
	return false;
}

enum class EFortWeaponTriggerType
{
	OnPress = 0,
	Automatic = 1,
	OnRelease = 2,
	OnPressAndRelease = 3,
	EFortWeaponTriggerType_MAX = 4,
};

struct Actor_32759_K2_GetActorLocation_Params
{
	struct Vector3                                     ReturnValue_69;                                           // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
};

Vector3 K2_GetActorLocation(uintptr_t boat)
{
	static PVOID fn = NULL;
	if (!fn)
		fn = FindObject("Actor.K2_GetActorLocation");

	Actor_32759_K2_GetActorLocation_Params params;

	ProcessEvent(boat, fn, &params);

	return params.ReturnValue_69;
}

bool k2_set_actor_location_and_rotation(uintptr_t boat, Vector3 new_location,Vector3 new_rotation, bool b_sweep, bool b_teleport) {
	static PVOID function;
	if (!function)
		function = FindObject("Actor.K2_SetActorLocationAndRotation");

	struct aactor_k2_set_actor_location_and_rotation_params
	{
		Vector3 new_location; // (Parm, ZeroConstructor, IsPlainOldData)
		Vector3 new_rotation; // (Parm, ZeroConstructor, IsPlainOldData)
		bool b_sweep; // (Parm, ZeroConstructor, IsPlainOldData)
		int sweep_hit_result; // (Parm, OutParm, IsPlainOldData)
		bool b_teleport; // (Parm, ZeroConstructor, IsPlainOldData)
		bool return_value; // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
	};

	aactor_k2_set_actor_location_and_rotation_params params{};
	params.new_location = new_location;
	params.new_rotation = new_rotation;
	params.b_sweep = b_sweep;
	params.b_teleport = b_teleport;

	ProcessEvent(boat, function, &params);
	return params.return_value;
}

// Function FortniteGame.FortPlayerPawn.IsInVehicle
struct FortPlayerPawn_IsInVehicle_Params
{
	bool                                               ReturnValue_69;                                           // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
};

bool IsInVehicle(uintptr_t pawn)
{
	static PVOID fn = NULL;
	if (!fn)
		fn = FindObject("IsInVehicle");

	FortPlayerPawn_IsInVehicle_Params params;

	ProcessEvent(pawn, fn, &params);

	return params.ReturnValue_69;
}

UObject* gayVehicle;

uintptr_t GetVehicle(uintptr_t pawn) {
	uintptr_t result;

	ProcessEvent((uintptr_t)pawn, gayVehicle, &result); //FortPlayerPawn::CurrentVehicle 0x2348

	return result;
}

Vector3 calcangle(Vector3& zaz, Vector3& daz) {

	Vector3 dalte = zaz - daz;
	Vector3 ongle;
	float hpm = sqrtf(dalte.x * dalte.x + dalte.y * dalte.y);
	ongle.y = atan(dalte.y / dalte.x) * 57.295779513082f;
	ongle.x = (atan(dalte.z / hpm) * 57.295779513082f) * -1.f;
	if (dalte.x >= 0.f) ongle.y += 180.f;
	return ongle;
}

static void SetControlRotation(uintptr_t PlayerController, Vector3 NewRotation, bool bResetCamera = false)
{
	auto SetControlRotation_ = (*(void(__fastcall**)(uintptr_t Controller, Vector3 NewRotation, bool bResetCamera))(*(uintptr_t*)PlayerController + 0x720));
	SetControlRotation_(PlayerController, NewRotation, bResetCamera);
}
struct APlayerCameraManager_GetCameraLocation_Params
{
	Vector3                                     ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
};
Vector3 GetCameraLocation(uintptr_t PlayerCameraManager)
{
	static PVOID AAA = 0;
	if (!AAA)
		AAA = FindObject("GetCameraLocation");

	APlayerCameraManager_GetCameraLocation_Params params;

	ProcessEvent(PlayerCameraManager, AAA, &params);

	return params.ReturnValue;
}

struct APlayerCameraManager_GetCameraRotation_Params
{
	Vector3                                     ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
};
Vector3 GetCameraRotation(uintptr_t PlayerCameraManager)
{
	static PVOID AAA = 0;
	if (!AAA)
		AAA = FindObject("GetCameraRotation");

	APlayerCameraManager_GetCameraLocation_Params params;

	ProcessEvent(PlayerCameraManager, AAA, &params);

	return params.ReturnValue;
}

struct FortPawn_SetPawnVisibility_Params
{
	bool                                               bNewHidden_69;                                            // (ConstParm, Parm, ZeroConstructor, IsPlainOldData)
	bool                                               bPropagateToWeapon_69;                                    // (ConstParm, Parm, ZeroConstructor, IsPlainOldData)
};


void SetVisibilty(uintptr_t Pawn, bool bNewHidden_69, bool bPropagateToWeapon_69)
{
	static PVOID AAA = 0;
	if (!AAA)
		AAA = FindObject("SetPawnVisibility");

	FortPawn_SetPawnVisibility_Params params;
	params.bNewHidden_69 = bNewHidden_69;
	params.bPropagateToWeapon_69 = bPropagateToWeapon_69;

	ProcessEvent(Pawn, AAA, &params);
}
