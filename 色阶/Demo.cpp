#include <tchar.h>
#include <stdio.h>      /* printf */
#include <math.h>
#include "Image.h"

template <typename Type>
Type round(Type val)
{
	return (val> 0.0) ? floor(val+ 0.5) : ceil(val- 0.5);
}

typedef struct _BmpInfo
{
	int nWidth;
	int nHeight;
	int nBitCount;
	int nPitch;
	byte *pBmpColorData;
}BmpInfo,*PBmpInfo;


void ApplyMapping(BmpInfo* pBmp,byte** Map)
{
	byte* p = pBmp->pBmpColorData;
	int offset = pBmp->nPitch - pBmp->nWidth*pBmp->nBitCount/8;
	for (int i=0; i< pBmp->nHeight;i++)
	{
		for (int j =0;j< pBmp->nWidth;j++)
		{
			p[0] = (*Map)[p[0]];
			p[1] = (*Map)[p[1]];
			p[2] = (*Map)[p[2]];
			p +=3;
		}
		p+=offset;
	}
	return;
}


/// <summary>
/// 色阶调整
/// </summary>
/// <param name="b">位图对象，24位彩色</param>
/// <param name="iB">输入色阶，黑场</param>
/// <param name="iW">输入色阶，白场</param>
/// <param name="oB">输出色阶，黑场</param>
/// <param name="oW">输出色阶，白场</param>
/// <returns>位图对象，24位彩色</returns>
void Gradation(BmpInfo* pBmp,byte iB,byte iW,byte oB,byte oW)
{
	byte* Map = new byte[256]; //亮度映射表
	for (int i = 0; i <= iB; i++) //将小于输入色阶黑色滑块值的像素映射为输出色阶黑色滑块值
		Map[i] = oB;
	for (int i = iW; i <= 255; i++) //将大于输入色阶白色滑块值的像素映射为输出色阶白色滑块值
		Map[i] = oW;
	double detX = iW - iB;//x变化量，即输入色阶范围
	double detY = oW - oB;//y变化量，即输出色阶范围
	double k = detY / detX;//映射曲线斜率
	double Sum = oB;//色彩映射累加器，相当于因变量y
	for (int i = iB + 1; i < iW; i++)
	{
		Sum += k;
		Map[i] = (byte)round(Sum);
	}
	ApplyMapping(pBmp,&Map);
	delete [] Map;
	Map = NULL;
}

int main(int agrc,char** agrv)
{
	CImage * pImge = new CImage;
	pImge->Load(_T("bmp.bmp"));
	BmpInfo bmpInfo;
	bmpInfo.nWidth = pImge->GetWidth();
	bmpInfo.nHeight = pImge->GetHeight();
	bmpInfo.nBitCount = 24;
	bmpInfo.pBmpColorData = pImge->GetBits();
	bmpInfo.nPitch = pImge->GetPitch();
	Gradation(&bmpInfo,0,180,0,255);

	pImge->Save(_T("bmp_deal.bmp"));


	return 0;
}