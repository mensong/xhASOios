#include "stdafx.h"
#include "cryptAlgorithm.h"

BOOL kEncodeData(char* pSrcDesBuf,char* key)
{
	if (!pSrcDesBuf && !key)
	{
		return FALSE;
	}

	int dataLen = strlen((const char*)pSrcDesBuf);
	int keyLen = strlen((const char*)key);

	for (int i=0; i<dataLen; i++)
	{
		pSrcDesBuf[i] = ( pSrcDesBuf[i] + key[i%keyLen] ) % 0x100;
	}

	return TRUE;
}

BOOL KDecodeData(char* pSrcDesBuf,char* key)
{
	if (!pSrcDesBuf && !key)
	{
		return FALSE;
	}
		
	int dataLen = strlen((const char*)pSrcDesBuf);
	int keyLen = strlen((const char*)key);

	for (int i=0; i<dataLen; i++)
	{
		pSrcDesBuf[i] = (0x100 + pSrcDesBuf[i] - key[i%keyLen]) % 0x100;//加0x100,考虑模为0的情况下。
	}

	return TRUE;
}

BOOL kEncryptData(char* pSrcBuf,char* key,char* pDesBuf)
{
	if (!pSrcBuf && !key)
	{
		return FALSE;
	}
	int dataLen = strlen(pSrcBuf);
	int keyLen = strlen(key);
	for (int i=0; i<dataLen; i++)
	{
		pDesBuf[i] = pSrcBuf[i]^key[i%keyLen];
	}

	return TRUE;
}

void decrypt(uint32_t *block, uint32_t len, uint32_t *key)
{
    uint32_t z=block[len-1], y=block[0], sum=0, e, DELTA=0x9e3779b9;
    int32_t p, q;
    
    q = 6 + 52/len;
    sum = q*DELTA ;
    while (sum != 0) 
	{
        e = (sum >> 2) & 3;
        for (p=len-1; p>0; p--)
        {
            z = block[p-1];
            block[p] -= (z>>5^y<<2) + (y>>3^z<<4)^(sum^y) + (key[p&3^e]^z);
            y = block[p];
        }
        z = block[len-1];
        block[0] -= (z>>5^y<<2) + (y>>3^z<<4)^(sum^y) + (key[p&3^e]^z);
        y =  block[0];
        sum -= DELTA;
    }
}

/*
 * Crypt block by XXTEA.
 * Params:
 *   block - block of input data
 *   len   - length of block
 *   key   - 128b key
 */
void crypt(uint32_t *block, uint32_t len, uint32_t *key)
{
    uint32_t z=block[len-1], y=block[0], sum=0, e, DELTA=0x9e3779b9;
    int32_t p, q;
    
    q = 6 + 52/len;
    while (q-- > 0)
	{
        sum += DELTA;
        e = (sum >> 2) & 3;
        for (p=0; p<len-1; p++)
        {
            y = block[p+1];
            block[p] += (z>>5^y<<2) + (y>>3^z<<4)^(sum^y) + (key[p&3^e]^z);
            z = block[p];
        }
        y = block[0];
        block[len-1] += (z>>5^y<<2) + (y>>3^z<<4)^(sum^y) + (key[p&3^e]^z);
        z = block[len-1];
    }
}
