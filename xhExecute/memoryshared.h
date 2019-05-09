#pragma once
#include <windows.h>

class memoryshared
{
public:
	memoryshared();
	~memoryshared();
	size_t Open(const TCHAR* name);
	bool Create(const TCHAR* name, size_t size);
	void Release();
	void Write(unsigned int msg, char* buf, size_t size);
	unsigned int Read(char* buf, size_t size);
private:
	HANDLE hMemShare;
	char * pShareMem;
	size_t _size;
};

