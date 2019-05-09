#pragma once
#include "SingleInstanceBase.h"
#include <string>
class CAutoDial : public CSingleInstanceBase<CAutoDial>
{
public:
  CAutoDial(void);
  ~CAutoDial(void);

  BOOL Connect(CString strEntryName, CString strUserName, CString strPassword);
  BOOL Disconnect();

  std::string GetLocalIpAddress();   
};

