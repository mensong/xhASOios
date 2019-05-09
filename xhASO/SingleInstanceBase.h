#pragma once

/******************************************单实例模板基类****************************************************/
template<class T> 
class CSingleInstanceBase
{
public:
	CSingleInstanceBase(void){}
	virtual ~CSingleInstanceBase(void){}

	static T* GetInstance()
	{
		static T s_Instance;
		return &s_Instance;
	}
};

