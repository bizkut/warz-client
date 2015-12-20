#if !defined(AFX_UTILS_H__D770CB86_632F_41D5_94C5_7F338DB92C21__INCLUDED_)
#define AFX_UTILS_H__D770CB86_632F_41D5_94C5_7F338DB92C21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "d3d9.h"
#include "d3dx9.h"

#pragma comment(lib, "d3D9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")


// Utils.h : header file
//
class   CLog;
extern	CLog*		gLog;
extern	int		U_Log(const char* Str, ...);
extern	void		U_OpenLog();
extern	void		U_LogIdleTick();
extern	void		U_CloseLog(int wait);

	//
	// Utility functions
	//

#define	D_LOG(x)	U_DLog("chk %s:%d\n", __FILE__, __LINE__);
extern	int		U_DLog(char *Str, ...);

#define	Abort(x)	U_Abort2(__LINE__, __FILE__, x)
extern	void		U_Abort2(int src_line, char *src_fname, char *msg);

class UD3D9
{
  public:
	char		pClassName[128];
	HWND		hWnd;
	IDirect3D9     	*pd3d;
	IDirect3DDevice9 *pd3ddev;
	
  public:
	UD3D9();
	~UD3D9();
	void	Init();
	void	Close();
};

extern	BOOL		DirectoryPicker(HWND hWnd, const char* title, const char* initial, char* out_dir, const char* root);

#endif // !defined(AFX_UTILS_H__D770CB86_632F_41D5_94C5_7F338DB92C21__INCLUDED_)
