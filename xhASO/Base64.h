// Base64.h: CBase64 ¿‡∂®“Â.
//
#include <iostream>
#include <string>
using namespace std;

#ifndef _BASE64_H__INCLUDED
#define _BASE64_H__INCLUDED

// class CBase64
class CBase64  
{
public:
	CBase64();
	virtual ~CBase64();

	virtual int Decode( LPSTR szDecoding, LPSTR szOutput );
	virtual string Encode(LPSTR szEncoding,int nSize);

protected:
	void WriteBits( UINT nBits, LPSTR szOutput, int& lp );
	UINT ReadBits( int* pBitsRead, int& lp );

	int m_nInputSize;
	int m_nBitsRemaining;
	ULONG m_lBitStorage;
	LPSTR m_szInput;
	static string m_sBase64Alphabet;
};

#endif 
