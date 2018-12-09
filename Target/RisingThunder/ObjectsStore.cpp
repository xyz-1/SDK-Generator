#include <windows.h>

#include "PatternFinder.hpp"
#include "ObjectsStore.hpp"

#include "EngineClasses.hpp"

class FUObjectArray
{
public:
	__int32 ObjFirstGCIndex;
	__int32 ObjLastNonGCIndex;
	__int32 OpenForDisregardForGC;

	TStaticIndirectArrayThreadSafeRead<UObject, 8388608, 16384> ObjObjects;
};

FUObjectArray* GlobalObjects = nullptr;

using GetUObjectArray = FUObjectArray * (__fastcall *)();

bool ObjectsStore::Initialize()
{
	const auto address = FindPattern(GetModuleHandleW(nullptr), reinterpret_cast<const unsigned char*>("\x40\x53\x48\x83\xEC\x30\x48\xC7\x44\x24\x00\x00\x00\x00\x00\x8B\x05\x00\x00\x00\x00\xA8\x01\x75\x2A\x83\xC8\x01\x89\x05\x00\x00\x00\x00\x48\x8D\x1D\x00\x00\x00\x00\x48\x8B\xCB\xE8\x00\x00\x00\x00\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x90\x48\x8B\xC3\xEB\x07\x48\x8D\x05\x00\x00\x00\x00\x48\x83\xC4\x30\x5B\xC3\x4D\x85\xC0\x0F\x84\x00\x00\x00\x00\x48\x89\x5C\x24\x00\x48\x89\x74\x24\x00\x57\x48\x83\xEC\x50\x49\x8B\xD8\x48\x8B\xF2\x48\x8B\xF9\x48\x85\xD2\x0F\x84\x00\x00\x00\x00\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00"), "xxxxxxxxxx?????xx????xxxxxxxxx????xxx????xxxx????xxx????x????xxxxxxxxx????xxxxxxxxxxx????xxxx?xxxx?xxxxxxxxxxxxxxxxxxx????xxx????x????");
	if (address == -1)
	{
		return false;
	}

	// Sigscan for GetUObjectArray rather than finding the actual array.
	// Maybe not the best idea but it works well.
	GlobalObjects = GetUObjectArray(address)();

	return true;
}

void* ObjectsStore::GetAddress()
{
	return GlobalObjects;
}

size_t ObjectsStore::GetObjectsNum() const
{
	return GlobalObjects->ObjObjects.Num();
}

UEObject ObjectsStore::GetById(size_t id) const
{
	return const_cast<UObject*>(GlobalObjects->ObjObjects.GetById(static_cast<int32_t>(id)));
}