#ifndef __HOOK_H
#define __HOOK_H

typedef struct _saveOldHookData
{
	struct _macHookData
	{
		DWORD dwOldHookPoint;
		char OldbyteValue[6];
		DWORD returnNextAddr;
	}macAddrData;

	struct _serialNumData
	{
		DWORD dwOldHookPoint;
		char OldbyteValue[6];
		DWORD returnNextAddr;
	}serialNumberData;

}OLDHOOKDATA,*POLDHOOKDATA;


int getMacAddrHookPoint(char* hModule);
int getSerialNumHookPoint(char* hModule);

void ModifyMacAddressValue(void);
void ModifySerialNumValue(void);

void RestoreMacAddressValue(void);
void RestoreSerialNumValue(void);

#endif