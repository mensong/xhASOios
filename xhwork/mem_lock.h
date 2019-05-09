#ifndef __MEM_LOCK_H
#define __MEM_LOCK_H

class CMemLock
{
public:
    CMemLock();
    ~CMemLock();
public:
    //共享内存加锁
    void Lock();
    //共享内存解锁
    void Unlock();
private:
    HANDLE           m_hMutex;
};
#endif // !__MEM_LOCK_H
