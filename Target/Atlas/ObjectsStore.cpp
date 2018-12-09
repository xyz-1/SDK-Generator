#include "ObjectsStore.hpp"
#include "PatternFinder.hpp"
#include "EngineClasses.hpp"

class FUObjectArray
{
public:
	int32_t ObjFirstGCIndex;
	int32_t ObjLastNonGCIndex;
	int32_t OpenForDisregardForGC;
	TArray<UObject*> ObjObjects;
	TArray<int32_t> ObjAvailable;
};

FUObjectArray* GlobalObjects = nullptr;

bool ObjectsStore::Initialize()
{
	const auto address = FindPattern(GetModuleHandleW(nullptr), reinterpret_cast<const unsigned char*>("\x48\x8D\x05\x00\x00\x00\x00\x8B\x40\x18"), "xxx????xxx");
	GlobalObjects = reinterpret_cast<decltype(GlobalObjects)>(address + 7 + *reinterpret_cast<uint32_t*>(address + 3));

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
	return GlobalObjects->ObjObjects[id];
}
