#include <windows.h>

#include "PatternFinder.hpp"
#include "ObjectsStore.hpp"

#include "EngineClasses.hpp"

class FUObjectItem
{
public:
	UObject* Object; //0x0000
	__int32 Flags; //0x0008
	__int32 ClusterIndex; //0x000C
	__int32 SerialNumber; //0x0010
};

class TUObjectArray
{
public:
	FUObjectItem* Objects;
	int32_t MaxElements;
	int32_t NumElements;
};

class FUObjectArray
{
public:
	char UnknownData[0x360]; // 4C 8B 81 ? ? ? ? 48 8B F9 48 8D 04 6D ? ? ? ?
	TUObjectArray ObjObjects;
};

FUObjectArray* GlobalObjects = nullptr;

bool ObjectsStore::Initialize()
{
	const auto address = FindPattern(GetModuleHandleW(L"FortniteClient-Win64-Shipping.exe"), reinterpret_cast<const unsigned char*>("\x48\x8D\x0D\x00\x00\x00\x00\x89\x13"), "xxx????xx");
	if (address == -1)
	{
		return false;
	}

	const auto offset = *reinterpret_cast<uint32_t*>(address + 3);

	GlobalObjects = reinterpret_cast<decltype(GlobalObjects)>(GetModuleHandleA(NULL) + 0x57208B8);
		//reinterpret_cast<decltype(GlobalObjects)>(address + 7 + offset);

	return true;
}

void* ObjectsStore::GetAddress()
{
	return GlobalObjects;
}

size_t ObjectsStore::GetObjectsNum() const
{
	return GlobalObjects->ObjObjects.NumElements;
}

UEObject ObjectsStore::GetById(size_t id) const
{
	return GlobalObjects->ObjObjects.Objects[id].Object;
}
