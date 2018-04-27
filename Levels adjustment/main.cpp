#include <Windows.h>
#include <commctrl.h>
#include <tchar.h>
#include "LevelsAdjustment.h"
#include "resource.h"

#pragma comment(lib,"comctl32.lib")

#include "Image.h"
#include "DlgHelper.h"

//#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
//#endif

CImage* g_pImg = NULL;

void SetPicControlPic(HWND hDlg);
void OnDropFiles(WPARAM wParam, HWND hDlg);
void OnTextChanged(WPARAM wParam, HWND hDlg);
void OnButtonClick(WPARAM wParam, HWND hDlg);
void SetValueToDefault(HWND hDlg);


void CheckValue0255(int &nValue)
{
	if (!nValue) return;
	if (nValue<0)
		nValue = 0;
	if (nValue>255)
		nValue = 255;
}

HBITMAP MakeBitmap(HDC hDc, LPBYTE lpBits, long lWidth, long lHeight, WORD wBitCount)
{
	BITMAPINFO bitinfo;
	memset(&bitinfo, 0, sizeof(BITMAPINFO));
	bitinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitinfo.bmiHeader.biWidth = lWidth;
	bitinfo.bmiHeader.biHeight = lHeight;
	bitinfo.bmiHeader.biPlanes = 1;
	bitinfo.bmiHeader.biBitCount = wBitCount;
	bitinfo.bmiHeader.biCompression = BI_RGB;
	bitinfo.bmiHeader.biSizeImage = lWidth*lHeight*(wBitCount / 8);
	bitinfo.bmiHeader.biXPelsPerMeter = 96;
	bitinfo.bmiHeader.biYPelsPerMeter = 96;
	bitinfo.bmiHeader.biClrUsed = 0;
	bitinfo.bmiHeader.biClrImportant = 0;

	return CreateDIBitmap(hDc, &bitinfo.bmiHeader, CBM_INIT, lpBits, &bitinfo, DIB_RGB_COLORS);
}

BOOL OpenFile(HWND hWnd)
{
	TCHAR szFile[MAX_PATH] = {0};       // buffer for file name
	if(GetOpenFilePath(hWnd,szFile))
	{
		if (g_pImg)
		{
			delete g_pImg;
			g_pImg = NULL;
		}
		g_pImg = new CImage;
		g_pImg->Load(szFile);
		g_pImg->Backup();
		SetPicControlPic(hWnd);
		return TRUE;
	}
	return FALSE;
}

void SetPicControlPic(HWND hDlg)
{
	HBITMAP hBitmap = MakeBitmap(GetDC(hDlg),g_pImg->GetBits(),g_pImg->GetWidth(),g_pImg->GetHeight(),24);
	if (!hBitmap) return;

	HWND hPic = GetDlgItem(hDlg,IDC_PIC);
	HBITMAP dwRetHandle = (HBITMAP)SendMessage(hPic, STM_SETIMAGE, IMAGE_BITMAP, LPARAM(hBitmap)); 
	if (dwRetHandle)
	{
		if (dwRetHandle!=hBitmap)
			DeleteObject(dwRetHandle);
	}
}

void OnDropFiles(WPARAM wParam, HWND hDlg)
{
	TCHAR* pszCurrentFile = GetDragFile(wParam, hDlg);
	if (pszCurrentFile)
	{
		if (g_pImg)
		{
			delete g_pImg;
		}
		g_pImg = new CImage;
		g_pImg->Load(pszCurrentFile);
		g_pImg->Backup();
		SetPicControlPic(hDlg);
		GlobalFree(pszCurrentFile);
	}
}

void OnTextChanged(WPARAM wParam, HWND hDlg)
{
	int nMsg =  HIWORD(wParam);
	switch(nMsg)
	{
	case EN_CHANGE:
		{
			int nID = LOWORD(wParam);
			switch(nID)
			{
			case IDC_INPUT_BLACK:
			case IDC_INPUT_WHILTE:
			case IDC_OUTPUT_BLACK:
			case IDC_OUTPUT_WHLTE:
				{
					int nInBlack =0;
					int nInWhite =0;
					int nOutBlack =0;
					int nOutWhite =0;
					TCHAR buf[20]={0};
					nInBlack = GetDlgItemInt(hDlg,IDC_INPUT_BLACK,FALSE,FALSE);
					nInWhite = GetDlgItemInt(hDlg,IDC_INPUT_WHILTE,FALSE,FALSE);
					nOutBlack = GetDlgItemInt(hDlg,IDC_OUTPUT_BLACK,FALSE,FALSE);
					nOutWhite = GetDlgItemInt(hDlg,IDC_OUTPUT_WHLTE,FALSE,FALSE);
					CheckValue0255(nInBlack);
					CheckValue0255(nInWhite);
					CheckValue0255(nOutBlack);
					CheckValue0255(nOutWhite);
					if(g_pImg)
					{

						BmpInfo bmp = {0};
						bmp.nWidth = g_pImg->GetWidth();
						bmp.nHeight = g_pImg->GetHeight();
						bmp.nPitch = g_pImg->GetPitch();
						g_pImg->CopyBackDataToCurData();
						bmp.pBmpColorData = g_pImg->GetBits();
						bmp.nBitCount = 24;

						Gradation(&bmp,nInBlack,nInWhite,nOutBlack,nOutWhite);
						SetPicControlPic(hDlg);
						::InvalidateRect(hDlg,NULL,TRUE);
					}
				}
			}
		}
		break;
	}
}

void OnButtonClick(WPARAM wParam, HWND hDlg)
{
	int nID =  LOWORD(wParam);
	switch(nID)
	{
	case  IDC_BTN_OPEN:
		OpenFile(hDlg);
		break;
	case IDC_BTN_ORG:
			SetValueToDefault(hDlg);
		break;
	case IDC_BTN_SAVE:
		{
			if (g_pImg)
			{
				if (g_pImg->GetSize()>0)
				{
					TCHAR szFile[MAX_PATH] = {0};
					GetSaveFilePath(hDlg,szFile);

					if(!_tcsstr(szFile,_T(".bmp")))
						_tcscat(szFile,_T(".bmp"));
					g_pImg->Save(szFile);
				}
			}
		}
		break;
	case IDC_BTN_WINDOWS:
		{
			if (!g_pImg) return;
			if (g_pImg->GetSize()<=0)  return;
			bool bNeedChangeFlag = false;
			int nWidth = g_pImg->GetWidth();
			int nHeight = g_pImg->GetHeight();
			RECT rtWindows = {0};
			GetWindowRect(hDlg,&rtWindows);
			if (rtWindows.right - rtWindows.left < nWidth + 40)
			{
				bNeedChangeFlag = true;
			}
			if (rtWindows.bottom - rtWindows.top < nHeight)
			{
				bNeedChangeFlag = true;
			}
			if (bNeedChangeFlag)
			{
				MoveWindow(hDlg,rtWindows.left,rtWindows.top,nWidth+50,nHeight+130,TRUE);
			}
			else
			{
				MoveWindow(hDlg,rtWindows.left,rtWindows.top,508,409,TRUE);
			}
		}
		break;
	}
}

LRESULT CALLBACK Proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		{
			char szTitle[MAX_PATH+1] ={0};
			GetWindowTextA(hDlg,szTitle,MAX_PATH);
			strcat(szTitle," ±àÒëÊ±¼ä£º ");
			strcat(szTitle,__DATE__);
			strcat(szTitle," ");
			strcat(szTitle,__TIME__);
			SetWindowTextA(hDlg,szTitle);
			DragAcceptFiles(hDlg,true);
			SetClassLongPtr(hDlg, GCLP_HICON, (long)LoadIcon(0, IDI_APPLICATION));
			SetValueToDefault(hDlg);
		}

		break;
	case WM_COMMAND:
		OnTextChanged(wParam, hDlg);
		OnButtonClick(wParam, hDlg);
		break;
	case WM_DROPFILES:
		OnDropFiles(wParam, hDlg);
		break;
	case WM_CLOSE:
		EndDialog(hDlg, wParam);
		break;
	}
	return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	InitCommonControls();
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_LEVELSADJUSTMENT_DIALOG), NULL, (DLGPROC)Proc);
	return 0;
}

void SetValueToDefault(HWND hDlg)
{
	SetDlgItemInt(hDlg,IDC_INPUT_BLACK,0,FALSE);
	SetDlgItemInt(hDlg,IDC_INPUT_WHILTE,255,FALSE);
	SetDlgItemInt(hDlg,IDC_OUTPUT_BLACK,0,FALSE);
	SetDlgItemInt(hDlg,IDC_OUTPUT_WHLTE,255,FALSE);
	if (g_pImg)
	{
		g_pImg->CopyBackDataToCurData();
		SetPicControlPic(hDlg);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////