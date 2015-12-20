// MaxUtilMgr.h : main header file for the MaxUtilMgr DLL
//

#if !defined(AFX_MaxUtilMgr_H__68E70A2D_B863_11D1_98AD_0040051EDCE7__INCLUDED_)
#define AFX_MaxUtilMgr_H__68E70A2D_B863_11D1_98AD_0040051EDCE7__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
  #error include 'stdafx.h' before including this file for PCH
#endif

#include <vector>

#include "resource.h"		// main symbols


#define	THIS_DLL_FILENAME	"TSG_MaxUtilMgr.dlu"

#include "../Common/PluginInterface.h"

/////////////////////////////////////////////////////////////////////////////
// CMaxMgrPanel, manage the Plugins' display panel
class CMaxMgrPanel : public UtilityObj 
{
 public:
	Interface*	ip;
	IUtil*		iu;
	HWND		hWnd;

	struct dll_s
	{
	  std::string	fileName;
	  std::string	fileVersion;
	  std::string	filePath;

	  HWND		limiterWnd;

	  HINSTANCE	hInst;
	  struct func_s
	  {
	    std::string	name;
	    int		type;
	    char	prm1[512];
	    char	prm2[512];

	    HWND	hWnd;
	    DWORD	id;
	  };
	  std::vector<func_s> funcs;
	  expExecFunc_fn exec;
	};

	std::string	   dllPath;
	std::vector<dll_s> mDlls;
	int		_OpenDLLs();
	int		 _RegisterButtons();
	int		_CloseDLLs();

	char		exportPath[MAX_PATH];
	int		bUseDefaultPath;
	int		bShowLog;

	void		ReadConfig();
	void		WriteConfig();

 public:
	CMaxMgrPanel();
	virtual ~CMaxMgrPanel();

	// Overides of the UtilityObj class
	void		BeginEditParams(Interface* ip, IUtil* iu);
	void		EndEditParams(Interface* ip, IUtil* iu);
	void		DeleteThis()						
	{
	}
	void		Init(HWND hWnd);
	void		Destroy(HWND hWnd);


	int		ExecFP_Call(char *cmd, Tab<char*> *params);
};

extern	CMaxMgrPanel	thePlugPanel;
extern	HINSTANCE	g_hInstance;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MaxUtilMgr_H__68E70A2D_B863_11D1_98AD_0040051EDCE7__INCLUDED_)
