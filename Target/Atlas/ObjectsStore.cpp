#include "ObjectsStore.hpp"
#include "PatternFinder.hpp"

class FUObjectItem
{
public:
	UObject* Object;
	int SerialNumber;
};

class FChunkedFixedUObjectArray
{
public:
	enum
	{
		ElementsPerChunk = 64 * 1024
	};

	FUObjectItem& GetByIndex(int Index)
	{
		return *GetObjectPtr(Index);
	}

	FUObjectItem* GetObjectPtr(int Index)
	{
		auto ChunkIndex = Index / ElementsPerChunk;
		auto WithinChunkIndex = Index % ElementsPerChunk;
		auto Chunk = Objects[ChunkIndex];
		return Chunk + WithinChunkIndex;
	}

	FUObjectItem** Objects;
	FUObjectItem* PreAllocatedObjects;
	int MaxElements;
	int NumElements;
	int MaxChunks;
	int NumChunks;
};

class FUObjectArray
{
public:
	int ObjFirstGCIndex;
	int ObjLastNonGCIndex;
	int MaxObjectsNotConsideredByGC;
	bool OpenForDisregardForGC;
	FChunkedFixedUObjectArray ObjObjects;
};

FUObjectArray* GlobalObjects;

bool ObjectsStore::Initialize()
{
	auto Address = FindPattern(GetModuleHandleW(0), (unsigned char*)"\x48\x8D\x05\x00\x00\x00\x00\x8B\x40", "xxx????xx");
	GlobalObjects = (FUObjectArray*)(Address + *(DWORD*)(Address + 0x3) + 0x7);

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
	return GlobalObjects->ObjObjects.GetByIndex(id).Object;
}
