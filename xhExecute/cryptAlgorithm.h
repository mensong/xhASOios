#ifndef __CRYPTALGORITHM_H
#define __CRYPTALGORITHM_H
typedef unsigned long int uint32_t;
typedef signed long int int32_t;
/////////////////////////////////////////////////////////
BOOL kEncodeData(char* pSrcDesBuf,char* key);
BOOL KDecodeData(char* pSrcDesBuf,char* key);
/////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
BOOL kEncryptData(char* pSrcBuf,char* key,char* pDesBuf);
//////////////////////////////////////////////////////////

void decrypt(uint32_t *block, uint32_t len, uint32_t *key);
void crypt(uint32_t *block, uint32_t len, uint32_t *key);
#endif