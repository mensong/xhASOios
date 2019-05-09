#include "stdafx.h"
#include "mem_lock.h"

CMemLock::CMemLock()
{
    char szMutexName[] = "56992E93-3828-415E-AB04-33107B63107D";
    m_hMutex = CreateMutexA(NULL, FALSE, szMutexName);
}

CMemLock::~CMemLock()
{
    CloseHandle(m_hMutex);
    m_hMutex = NULL;

}

void CMemLock::Lock()
{
    ::WaitForSingleObject(m_hMutex, INFINITE);
}

void CMemLock::Unlock()
{
    ReleaseMutex(m_hMutex);
}