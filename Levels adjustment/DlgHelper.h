#ifndef DLG_HELPER


BOOL GetOpenFilePath(HWND hwnd,TCHAR *szFile0)
{
	if (!szFile0) return FALSE;
	OPENFILENAME ofn;       // common dialog box structure
	TCHAR szFile[MAX_PATH] = {0};       // buffer for file name
	//HWND hwnd;              // owner window

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = &szFile0[0];
	//
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	//
	ofn.lpstrFile[0] = _T('\0');
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = _T("bmp,jpg图片\0*.bmp;*.jpg\0All\0*.*\0\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 

	if (GetOpenFileName(&ofn))
		return TRUE;
	DWORD dwError = ::GetLastError();
	return FALSE;
}	

BOOL GetSaveFilePath(HWND hwnd,TCHAR *szFile0)
{
	if (!szFile0) return FALSE;
	OPENFILENAME ofn;       // common dialog box structure
	TCHAR szFile[MAX_PATH] = {0};       // buffer for file name
	//HWND hwnd;              // owner window

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = &szFile0[0];
	//
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	//
	ofn.lpstrFile[0] = _T('\0');
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = _T("bmp图片\0*.bmp\0jpg图片\0*.jpg\0\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST;

	// Display the Open dialog box. 

	if (GetSaveFileName(&ofn))
		return TRUE;
	DWORD dwError = ::GetLastError();
	return FALSE;
}	

//  GlobalFree the return
TCHAR* GetDragFile(WPARAM wParam, HWND hDlg)
{
	HDROP hDrop = (HDROP) wParam;

	TCHAR* pszCurrentFile;
	TCHAR *pszFileList = NULL;
	// Don't accept more dropped files
	::DragAcceptFiles(hDlg, FALSE);

	// Get number of files dropped
	const int iFiles = ::DragQueryFile(hDrop, ~0U, NULL, 0);

	// Free previous file list (if allocated)
	if (pszFileList)
	{
		GlobalFree(pszFileList);
	}

	// Allocate buffer to hold list of files
	pszFileList = (LPTSTR) GlobalAlloc(GMEM_ZEROINIT, iFiles * MAX_PATH);
	if (pszFileList)
	{
		LPTSTR pFile = pszFileList;
		for (int iIndex = 0; iIndex < iFiles; ++iIndex)
		{
			if (::DragQueryFile(hDrop, iIndex, pFile, MAX_PATH))// 获取拖入的文件名
			{
				//TRACE(_T("WM_DROPFILES (%d of %d) szPathName=%s\n"), iIndex, iFiles, pFile);
				// Add terminating '\n' between files
				int iLength = lstrlen(pFile);
				pFile += (iLength + 1);
			}
		}
		*pFile++ = '\0';
		pszCurrentFile = pszFileList;
	}
	DragFinish(hDrop);	
	::DragAcceptFiles(hDlg,TRUE);
	return pszCurrentFile;
}



#endif // DLG_HELPER