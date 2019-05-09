#ifndef __ENTERPOINT_H
#define __ENTERPOINT_H

void InitItunes(HMODULE hModule);

void PatchItunes(int lpAddress);
void restorePatch();
int getHookPoint1(int lpAddr);///在调用CALL DWORD PTR DS:[EAX+5C]函数后面打补丁的位置
int getHookPoint2(char* hModule);

#endif