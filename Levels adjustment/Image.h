


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 预处理
#pragma once
#include <Windows.h>

// 类型定义
#ifndef DOUBLE
typedef double DOUBLE;
#endif
#ifndef PCTSTR
typedef LPCTSTR PCTSTR;
#endif

// 常用函数宏
#define _HeapAlloc(n)				HeapAlloc(GetProcessHeap(), 0, n)
#define _HeapFree(p)				HeapFree(GetProcessHeap(), 0, (PVOID) p)
#define _CreateFileForWrite(f)		CreateFile(f, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL)

// 数组元素个数
#define _NumberOf(v)				(sizeof(v) / sizeof(v[0]))

// DIB 计算
#define _DibPitch(w, i)				((((w * i) + 31) & ~31) / 8)
#define _DibSize(w, i, h)			(_DibPitch(w, i) * h)
#define _DibBits(p, w, i, x, y)		(p + _DibPitch(w, i) * y + x * 3)
#define _DibPitch24(w)				((w + w + w + 3) & 0xFFFFFFFC)
#define _DibSize24(w, h)			(_DibPitch24(w) * h)
#define _DibBits24(p, w, x, y)		(p + _DibPitch24(w) * y + x * 3)

// 限制数值范围
#define _Bound(v, vMin, vMax)		(((v) > (vMax)) ? (vMax) : ((v < vMin) ? (vMin) : (v)))

// 矩形宽度和高度
#define _RectWidth(r)				((r).right - (r).left)
#define _RectHeight(r)				((r).bottom - (r).top)

// 字符串转换拷贝
#define _WStrToAStrN(a, w, n)		WideCharToMultiByte(CP_ACP, 0, w, -1, a, n, NULL, NULL)
#define _AStrToWStrN(w, a, n)		MultiByteToWideChar(CP_ACP, 0, a, -1, w, n)
#define _StrToStrN(t1, t2, n)		lstrcpyn(t1, t2, n)
#ifdef _UNICODE
#define _StrToAStrN(a, t, n)		_WStrToAStrN(a, t, n)
#define _StrToWStrN(w, t, n)		lstrcpyn(w, t, n)
#define _AStrToStrN(t, a, n)		_AStrToWStrN(t, a, n)
#define _WStrToStrN(t, w, n)		lstrcpyn(t, w, n)
#else // _UNICODE
#define _StrToAStrN(a, t, n)		lstrcpyn(a, t, n)
#define _StrToWStrN(w, t, n)		_AStrToWStrN(w, t, n)
#define _AStrToStrN(t, a, n)		lstrcpyn(t, a, n)
#define _WStrToStrN(t, w, n)		_WStrToAStrN(t, w, n)
#endif // _UNICODE

// 字符串转换拷贝到字符数组
#define _WStrToAStr(a, w)			_WStrToAStrN(a, w, _NumberOf(a))
#define _AStrToWStr(w, a)			_AStrToWStrN(w, a, _NumberOf(w))
#define _StrToStr(t1, t2)			_StrToStrN(t1, t2, _NumberOf(t1))
#define _StrToAStr(a, t)			_StrToAStrN(a, t, _NumberOf(a))
#define _StrToWStr(w, t)			_StrToWStrN(w, t, _NumberOf(w))
#define _AStrToStr(t, a)			_AStrToStrN(t, a, _NumberOf(t))
#define _WStrToStr(t, w)			_WStrToStrN(t, w, _NumberOf(t))
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CImage 类声明
class CImage
{
protected:
	// 成员变量
	UINT m_uWidth;					// 图像宽度
	UINT m_uHeight;					// 图像高度
	PBYTE m_pbBits;					// 图像数据指针
	DOUBLE m_dScale;				// 用于绘制图像的缩放系数

	UINT m_uBackupWidth;			// 备份图像宽度
	UINT m_uBackupHeight;			// 备份图像高度
	PBYTE m_pbBackupBits;			// 备份图像数据指针

	TCHAR m_tzFileName[MAX_PATH];	// 当前图像文件名称

public:
	// 构造函数
	CImage()
	{
		m_uWidth = 0;
		m_uHeight = 0;
		m_pbBits = NULL;
		m_dScale = 1;
		m_uBackupWidth = 0;
		m_uBackupHeight = 0;
		m_pbBackupBits = NULL;
		m_tzFileName[0] = 0;
	}

	// 析构函数
	virtual inline CImage::~CImage()
	{
		Destroy();
		DestroyBackup();
	}

	// 获取图像宽度
	inline UINT GetWidth()
	{
		return m_uWidth;
	}

	// 获取图像高度
	inline UINT GetHeight()
	{
		return m_uHeight;
	}

	// 获取绘制宽度
	inline UINT GetScaleWidth()
	{
		return (UINT) (GetWidth() * GetScale());
	}

	// 获取图像高度
	inline UINT GetScaleHeight()
	{
		return (UINT) (GetHeight() * GetScale());
	}

	// 获取用于绘制图像的缩放系数
	inline DOUBLE GetScale()
	{
		return m_dScale;
	}

	// 获取用于绘制图像的缩放系数
	inline VOID SetScale(DOUBLE dScale = 1)
	{
		m_dScale = dScale;
	}

	// 获取当前图像的文件名，返回值指向文件名缓冲区指针，允许被更改
	inline PTSTR GetFileName()
	{
		return m_tzFileName;
	}

	// 获取一行图像所占的字节数
	inline UINT GetPitch()
	{
		return _DibPitch24(m_uWidth);
	}

	// 获取图像所占的字节数
	inline UINT GetSize()
	{
		return _DibSize24(m_uWidth, m_uHeight);
	}

	// 获取第 y 行的图像数据指针
	inline PBYTE GetBits()
	{
		return m_pbBits;
	}

	// 获取第 y 行的图像数据指针
	inline PBYTE GetBits(UINT y)
	{
		return m_pbBits + GetPitch() * y;
	}

	// 获取第 (x, y) 点的图像数据指针
	inline PBYTE GetBits(UINT x, UINT y)
	{
		return m_pbBits + GetPitch() * y + x * 3;
	}

	// 获取 BITMAPINFOHEADER 信息
	inline VOID GetBih(PBITMAPINFOHEADER pbihInfo)
	{
		pbihInfo->biSize = sizeof(BITMAPINFOHEADER);
		pbihInfo->biWidth = m_uWidth;
		pbihInfo->biHeight = m_uHeight;
		pbihInfo->biPlanes = 1;
		pbihInfo->biBitCount = 24;
		pbihInfo->biCompression = BI_RGB;
		pbihInfo->biSizeImage = GetSize();
		pbihInfo->biXPelsPerMeter = 0;
		pbihInfo->biYPelsPerMeter = 0;
		pbihInfo->biClrUsed = 0;
		pbihInfo->biClrImportant = 0;
	}

	// 创建图像。返回 TRUE 表示成功，将更新图像数据；返回 FALSE 表示失败，原有图像数据不会被更改
	inline BOOL Create(UINT uWidth, UINT uHeight, const PBYTE pbBits = NULL)
	{
		PBYTE pbTemp;

		pbTemp = (PBYTE) _HeapAlloc(_DibSize24(uWidth, uHeight));
		if (pbTemp)
		{
			Destroy();

			m_uWidth = uWidth;
			m_uHeight = uHeight;
			m_pbBits = pbTemp;

			if (pbBits)
			{
				CopyMemory(m_pbBits, pbBits, GetSize());
			}

			return TRUE;
		}

		return FALSE;
	}

	// 附加图像。返回 TRUE 表示成功，将更新图像数据；返回 FALSE 表示失败，原有图像数据不会被更改
	inline BOOL Attach(UINT uWidth, UINT uHeight, PBYTE pbBits)
	{
		if (pbBits)
		{
			m_uWidth = uWidth;
			m_uHeight = uHeight;

			if (m_pbBits)
			{
				_HeapFree(m_pbBits);
			}

			m_pbBits = pbBits;

			return TRUE;
		}

		return FALSE;
	}

	// 销毁图像数据
	inline VOID Destroy()
	{
		if (m_pbBits)
		{
			_HeapFree(m_pbBits);
			m_pbBits = NULL;
			m_uWidth = 0;
			m_uHeight = 0;
			m_dScale = 1;
		}
	}

	// 获取备份图像宽度
	inline UINT GetBackupWidth()
	{
		return m_uBackupWidth;
	}

	// 获取备份图像高度
	inline UINT GetBackupHeight()
	{
		return m_uBackupHeight;
	}

	// 获取备份图像数据
	inline PBYTE GetBackupBits()
	{
		return m_pbBackupBits;
	}

	// 备份图像数据
	inline VOID Backup()
	{
		DestroyBackup();
		m_uBackupWidth = m_uWidth;
		m_uBackupHeight = m_uHeight;
		m_pbBackupBits = (PBYTE) _HeapAlloc(GetSize());
		CopyMemory(m_pbBackupBits, m_pbBits, GetSize());
	}

	// 恢复图像数据
	inline VOID Restore()
	{
		Attach(m_uBackupWidth, m_uBackupHeight, m_pbBackupBits);
		m_pbBackupBits = NULL;
	}

	// 销毁备份图像数据
	inline VOID DestroyBackup()
	{
		if (m_pbBackupBits)
		{
			_HeapFree(m_pbBackupBits);
			m_pbBackupBits = NULL;
			m_uBackupWidth = 0;
			m_uBackupHeight = 0;
		}
	}

	// 用备份数据覆盖原数据，备份数据不删除
	inline void CopyBackDataToCurData()
	{
		if (!m_pbBackupBits) return;
		memcpy(m_pbBits,m_pbBackupBits,GetSize());
	}

	// 读取、保存及绘制图像
	BOOL Load(PCTSTR ptzFileName);
	BOOL Save(PCTSTR ptzFileName, LPCRECT prtRect = NULL);
	VOID Draw(HDC hDC, INT iLeft = 0, INT iTop = 0);

	// 图像变换
	VOID Stretch(UINT uWidth, UINT uHeight);
	VOID FlipHorizon();
	VOID FlipVertical();
	VOID Rotate(UINT uAngle, COLORREF crColor = 0x00FFFFFF);

	// 图像处理
	VOID Decolourize();
	VOID Expose(BYTE bValue = 128);
	VOID Emboss();
	VOID Adjust(INT iRed, INT iGreen, INT iBlue);

	// 边缘检测
	VOID Fringe(BYTE bValue = 64);
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


