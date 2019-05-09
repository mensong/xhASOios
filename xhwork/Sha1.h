#pragma once

#ifdef __cplusplus
extern"C"
{
#endif

#define SHA1_SIZE_BYTE 20  


typedef struct SHAstate_st
{
	unsigned long h[SHA1_SIZE_BYTE / 4]; /* 存放摘要结果(32*5=160 bits)*/
	unsigned long Nl;
	unsigned long Nh;       /*存放信息总位数，Nh：高32位，Nl：低32位*/
	unsigned long data[16]; /*数据从第0个的高8位开始依次放置*/
	int FlagInWord;     /*标识一个data元素中占用的字节数（从高->低），取值0,1,2,3*/
	int msgIndex;       /*当前已填充满的data数组元素数。*/
	int isTooMang;      /*正常为0，当处理的信息超过2^64 bits时为1；*/
} SHA1_Context;

int SHA1_Init(SHA1_Context *c);
int SHA1_Update(SHA1_Context *c, const unsigned char *chBuff, unsigned int n);
int SHA1_Final(SHA1_Context *c, unsigned char * md);
int SHA1_Clear_data(SHA1_Context *c);
#ifdef __cplusplus
}
#endif