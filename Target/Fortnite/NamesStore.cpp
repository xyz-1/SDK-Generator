#include <windows.h>

#include "PatternFinder.hpp"
#include "NamesStore.hpp"

#include "EngineClasses.hpp"
typedef unsigned __int64(__fastcall* d)(char * a1, char* a2);

typedef __int64(*c)();

class FNameEntry
{
public:
	__int32 Index;
	char pad_0x0004[0x4];
	FNameEntry* HashNext;
	union
	{
		char AnsiName[1024];
		wchar_t WideName[1024];
	};

	std::string GetName() const
	{
		const auto addy = FindPattern(GetModuleHandleW(NULL), reinterpret_cast<const unsigned char*>("\xB9\x00\x00\x00\x00\x49\x83\xC3"), "x????xxx") - 0x4F;
		c sub_110DAD0 = c(addy);

		DWORD v = sub_110DAD0();
		ULONG_PTR valueX = ((ULONG_PTR)v * 0x3e0f83e1);

		ULONG_PTR value2 = ((valueX >> 32) >> 3) * 0x21;
		UINT value1 = (v ^ 0x9C677CC5);

		static char buf[1024] = {};

		ULONG_PTR a2 = (ULONG_PTR)AnsiName;
		int seed = value1 + (v - value2);
		char result = seed ^ AnsiName[0];
		buf[0] = result;
		auto v8 = -(signed long long)AnsiName;
		if (buf[0])
		{
			auto index = 1;
			char c;
			do
			{
				seed += (v - value2) + v8 + ++a2;
				c = seed ^ AnsiName[index];
				buf[index++] = c;
			} while (c);
		}

		return buf;
	}
};

template<typename ElementType, int32_t MaxTotalElements, int32_t ElementsPerChunk>
class TStaticIndirectArrayThreadSafeRead
{
public:
	int32_t Num() const
	{
		return NumElements;
	}

	bool IsValidIndex(int32_t index) const
	{
		return index >= 0 && index < Num() && GetById(index) != nullptr;
	}
	typedef ElementType const* const&(__fastcall* q)(int32_t* index);

	ElementType const* const& GetById(int32_t index) const
	{
		return *GetItemPtr(index);
	}

public:
	ElementType const* const* GetItemPtr(int32_t Index) const
	{
		int32_t ChunkIndex = Index / ElementsPerChunk;
		int32_t WithinChunkIndex = Index % ElementsPerChunk;
		ElementType** Chunk = Chunks[ChunkIndex];
		return Chunk + WithinChunkIndex;
	}

	enum
	{
		ChunkTableSize = (MaxTotalElements + ElementsPerChunk - 1) / ElementsPerChunk
	};

	char UnknownData[0x128]; // 49 8B 84 C1 ?? ?? ?? ?? 48 8B 04 C8
	ElementType** Chunks[ChunkTableSize];
	char UnknownData2[0xE8];
	__int32 NumElements;
	__int32 NumChunks;
};

using TNameEntryArray = TStaticIndirectArrayThreadSafeRead<FNameEntry, 2 * 1024 * 1024, 0x4000>;

TNameEntryArray* GlobalNames = nullptr;

bool NamesStore::Initialize()
{
	const auto address = FindPattern(GetModuleHandleW(NULL), reinterpret_cast<const unsigned char*>("\xB9\x18\x06\x00\x00\x48\x89\x5C\x24\x20"), "xxxxxxxxxx") - 12;

	if (address == -1)
		return false;

	const auto offset = *reinterpret_cast<uint32_t*>(address + 3);
	GlobalNames = reinterpret_cast<decltype(GlobalNames)>(*reinterpret_cast<uintptr_t*>(address + 7 + offset));

	return true;
}

void* NamesStore::GetAddress()
{
	return GlobalNames;
}

size_t NamesStore::GetNamesNum() const
{
	return GlobalNames->Num();
}

bool NamesStore::IsValid(size_t id) const
{
	return GlobalNames->IsValidIndex(static_cast<int32_t>(id));
}

std::string NamesStore::GetById(size_t id) const
{
	return GlobalNames->GetById(static_cast<int32_t>(id))->GetName();
}
