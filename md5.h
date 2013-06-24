#pragma once
//#include <system>
//#include <windef.h>
#include <stdio.h>
#include <string>

#ifdef __alpha
typedef unsigned int UINT4;
#else
typedef unsigned long int UINT4;
typedef unsigned int UINT;
typedef unsigned char UCHAR;
typedef unsigned long ULONG;
typedef unsigned char BYTE;
typedef unsigned long DWORD;
#endif

#define MD5_INIT_STATE_0 0x67452301
#define MD5_INIT_STATE_1 0xefcdab89
#define MD5_INIT_STATE_2 0x98badcfe
#define MD5_INIT_STATE_3 0x10325476

void MD5Init(void);
void MD5Update(unsigned char *bug, unsigned int len);
void MD5Final(char* cReturnStr);
void Transform(UINT4 *buf, UINT4 *in);
void GetMD5(char* pBuf, UINT nLength,char* cReturnStr);
std::string MD5(std::string in);

