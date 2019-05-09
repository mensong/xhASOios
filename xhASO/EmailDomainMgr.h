#pragma once
#include <vector>
#include "SingleInstanceBase.h"
using namespace std;
class CEmailDomainMgr : public CSingleInstanceBase<CEmailDomainMgr>
{
public:
  CEmailDomainMgr(void);
  ~CEmailDomainMgr(void);

  void Initialize();

  CString GetRandomDomain();


protected:
  vector<CString>   m_vecDomain;
};

