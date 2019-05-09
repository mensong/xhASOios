#include "memoryshared.h"


memoryshared::memoryshared()
{
	hMemShare = NULL;
	pShareMem = NULL;
}


memoryshared::~memoryshared()
{
	Release();
}

size_t memoryshared::Open(const TCHAR* name)
{
	hMemShare = OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, name);
	if (hMemShare)
	{
		pShareMem = (char*)MapViewOfFile(hMemShare, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		return _size = *(size_t*)pShareMem;
	}
	return 0;
}

bool memoryshared::Create(const TCHAR* name, size_t size)
{
	_size = size;
	hMemShare = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, _size + 8, name);
	if (hMemShare == NULL)
	{
		return false;
	}

	pShareMem = (char*)MapViewOfFile(hMemShare, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (pShareMem == NULL)
	{
		return false;
	}

	*(size_t*)(pShareMem) = _size;
	return true;
}

void memoryshared::Release()
{
	if (pShareMem != NULL)
	{
		UnmapViewOfFile(pShareMem);
		pShareMem = NULL;
	}
	if (hMemShare != NULL)
	{
		CloseHandle(hMemShare);
		hMemShare = NULL;
	}
}

void memoryshared::Write(unsigned msg, char* buf, size_t size)
{
	if (pShareMem == NULL)return;
	*(size_t*)(pShareMem + 4) = msg;
	memcpy(pShareMem + 8, buf, min(_size, size));
}

unsigned memoryshared::Read(char* buf, size_t size)
{
	if (pShareMem == NULL)return -1;
	memcpy(buf, pShareMem + 8, min(_size, size));
	return *(size_t*)(pShareMem + 4);
}
