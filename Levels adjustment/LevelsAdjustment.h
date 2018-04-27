#pragma once
#include <windows.h>
typedef struct _BmpInfo
{
	int nWidth;
	int nHeight;
	int nBitCount;
	int nPitch;
	BYTE *pBmpColorData;
}BmpInfo,*PBmpInfo;




void Gradation(BmpInfo* pBmp,byte iB,byte iW,byte oB,byte oW);