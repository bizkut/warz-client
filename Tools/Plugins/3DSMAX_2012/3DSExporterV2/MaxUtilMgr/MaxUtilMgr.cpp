#include "stdafx.h"
#include "MaxUtilMgr.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_FONT_HEIGHT		20

extern	char*	U_GetModuleVersion(char *fname);

#define _CRT_SECURE_NO_DEPRECATE

#define	D_LOG(x)	U_DLog("chk %s:%d\n", __FILE__, __LINE__);
void U_DLog(char *Str, ...)
{
	char 	buf[1024];

  va_list ap;
  va_start(ap, Str);
  vsprintf(buf, Str, ap);
  va_end(ap);

  OutputDebugString(buf);
}

	//
	//
	// DLLEntry part
	//
	//

extern  HINSTANCE	g_hInstance;


//
// 3dsmax callbacks
// 
__declspec(dllexport) const TCHAR* LibDescription()
{ 
  return _T("TSGroup 3DSMax Utility Manager"); 
}

__declspec(dllexport) int LibNumberClasses()
{ 
  return 1;					
}

__declspec(dllexport) ClassDesc* LibClassDesc(int i)
{
  extern ClassDesc2* GetPlugDesc1();
  switch(i) 
  {
    case 0:	return GetPlugDesc1();
    default:	return 0;
  }
}

__declspec(dllexport) ULONG LibVersion()
{ 
  return VERSION_3DSMAX;
}

class CMaxToolsMgrApp : public CWinApp
{
// constructors/destructor
public:
	CMaxToolsMgrApp();
// MSVC++ stuff
	//{{AFX_VIRTUAL(CMaxToolsMgrApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMaxToolsMgrApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

static	CMaxToolsMgrApp	theApp;				


BEGIN_MESSAGE_MAP(CMaxToolsMgrApp, CWinApp)
	//{{AFX_MSG_MAP(CMaxToolsMgrApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CMaxToolsMgrApp::CMaxToolsMgrApp()
{
}

BOOL CMaxToolsMgrApp::InitInstance() 
{
  //Get the DLL's HINSTANCE
  g_hInstance = AfxGetResourceHandle();

  //Init Win32 Controls
  InitCommonControls();

  return TRUE;				
}

int CMaxToolsMgrApp::ExitInstance() 
{
  return CWinApp::ExitInstance();
}


	//
	//
	// Plugin part
	//
	//


#define	TSG_UTILMGR_CLASS_ID	Class_ID(0x24f22f8d, 0x59140b2a)

class CUtilMgrDesc : public ClassDesc2
{
 public:
	int		IsPublic()			{ return 1;}
	void*		Create(BOOL loading = FALSE)	{ return &thePlugPanel; }
	const TCHAR*	ClassName ()			{ return _T("TSG Utility Manager");}
	SClass_ID	SuperClassID ()			{ return UTILITY_CLASS_ID; }
	Class_ID	ClassID()			{ return TSG_UTILMGR_CLASS_ID;}
	const TCHAR*	Category ()			{ return _T("TSGroup");}
	const TCHAR*	InternalName()			{ return _T("TSGroup_Tools"); }		// returns fixed parsable name (scripter-visible name)
};

static	CUtilMgrDesc	theUtilMgrDesc;
ClassDesc2* GetPlugDesc1() { return &theUtilMgrDesc; }

#define TSG_UTILS_INTERFACE	Interface_ID(0x8ec54b8, 0x33767fc8)

class CUtilMgrDescActions : public FPStaticInterface
{
  protected:
	DECLARE_DESCRIPTOR(CUtilMgrDescActions)

	enum {
	  fnRunCommand = 1,
	  fnSelectDir,
	};

	BEGIN_FUNCTION_MAP
	  FN_2(fnRunCommand, TYPE_INT, thePlugPanel.ExecFP_Call, TYPE_STRING, TYPE_STRING_TAB)
	  FN_2(fnSelectDir,  TYPE_INT, CUtilMgrDescActions::SelectDir, TYPE_STRING, TYPE_STRING)
	END_FUNCTION_MAP

	static CUtilMgrDescActions theMaxMgrDescActions;

  public:

static	int	SelectDir(char *indir, char *root);
};

CUtilMgrDescActions CUtilMgrDescActions::theMaxMgrDescActions(
  TSG_UTILS_INTERFACE, _T("UtilMgr"), IDS_UTILMGR, &theUtilMgrDesc, FP_STATIC_METHODS,
  fnRunCommand, _T("RunCommand"), IDS_RUNMODULE, TYPE_INT, 0, 2,
   _T("prm2"), IDS_PRM2, TYPE_STRING,     f_inOut, FPP_IN_PARAM,
   _T("prm3"), IDS_PRM3, TYPE_STRING_TAB, f_inOut, FPP_IN_PARAM,
  fnSelectDir, _T("SelectDir"), IDS_RUNMODULE, TYPE_INT, 0, 2,
   _T("prm1"), IDS_PRM1, TYPE_STRING,     f_inOut, FPP_IN_OUT_PARAM,
   _T("prm2"), IDS_PRM2, TYPE_STRING,     f_inOut, FPP_IN_PARAM,
  end
);

int CUtilMgrDescActions::SelectDir(char *indir, char *root)
{
  char buf[_MAX_PATH];

  extern BOOL DirectoryPicker(HWND hWnd, char *title, char *initial, char *dir, char *root);
  if(!DirectoryPicker(NULL, "Choice Directory", indir, buf, root))
    return 0;

  strcpy(indir, buf);
  return 1;
}



	//
	//
	//
	//
	//

	CMaxMgrPanel	thePlugPanel;
	HINSTANCE	g_hInstance;

#define	IDC_MYBUTTONS	0x7000



//  Callback procedure to set the initial selection of the browser.
static int CALLBACK SetSelProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
  if (uMsg==BFFM_INITIALIZED)
  {
    ::SendMessage( hWnd, BFFM_SETSELECTION, TRUE, lpData );
  }

  return 0;
}

//  Display a folder browser allowing the user to browse for a directory.
BOOL DirectoryPicker(HWND hWnd, char *title, char *initial, char *dir, char *root)
{
  LPMALLOC pMalloc;
  char buffer[MAX_PATH];
  BOOL r = FALSE;

  // Retrieve the task memory allocator.

  if(SUCCEEDED(::SHGetMalloc(&pMalloc)))
  {
    // Fill in a BROWSEINFO structure, setting the flag to indicate
    // that we are only interested in file system directories.

    BROWSEINFO bi;
    bi.hwndOwner = hWnd;
    bi.pidlRoot  = NULL;
    if(root && *root) {
      WORD rootU[512];
      MultiByteToWideChar(CP_ACP, 0, root, -1, (LPWSTR)rootU, 512);
      LPSHELLFOLDER pDesktopFolder;
      SHGetDesktopFolder(&pDesktopFolder);
      pDesktopFolder->ParseDisplayName(NULL, NULL, (LPOLESTR)rootU,
        NULL, (struct _ITEMIDLIST **) &bi.pidlRoot, NULL);
    }
    bi.pszDisplayName = buffer;
    bi.lpszTitle = (LPCSTR) title;
    bi.ulFlags   = BIF_RETURNONLYFSDIRS;
    bi.iImage    = 0;
    bi.lpfn      = SetSelProc;
    bi.lParam    = (LPARAM)(LPCSTR)initial;

    // Display the browser.
                
    PIDLIST_ABSOLUTE item_list =  ::SHBrowseForFolder(&bi);

    // If the user selected a folder...
    if (item_list)
    {
      // Convert the item ID to a pathname, and copy it to the
      // out parameter.
      if (::SHGetPathFromIDList(item_list, buffer))
      {
        strcpy(dir, buffer);
        r   = TRUE;
      }   
                        
      // Free the PIDL allocated by SHBrowseForFolder
      pMalloc->Free(item_list);
    }

    // Release the shell's allocator 
    pMalloc->Release();
  }
        
  return r;
}



CMaxMgrPanel::CMaxMgrPanel()
{
  hWnd = NULL;
  ip   = GetCOREInterface();

  char dir[MAX_PATH];
  sprintf(dir, "%s\\plugins", ip->GetDir(APP_MAX_SYS_ROOT_DIR));
  dllPath = dir;
  
  bUseDefaultPath = 0;
  bShowLog = 1;

  sprintf(exportPath, ip->GetDir(APP_EXPORT_DIR));
}

CMaxMgrPanel::~CMaxMgrPanel()
{
}


int CMaxMgrPanel::_OpenDLLs()
{
  if(mDlls.size())
    return 1;
  U_DLog("MaxMgrPanel::_OpenDLLs()\n");
  
  WIN32_FIND_DATA fdata;
  HANDLE	h;
  char		buf[512];
  static char	MY_DLL_EXT[] = "smax_dll";

  sprintf(buf, "%s\\*.%s", dllPath.c_str(), MY_DLL_EXT);
  if((h=FindFirstFile(buf, &fdata)) == INVALID_HANDLE_VALUE) {
    MessageBox(NULL, "NO FILES", "no2", MB_OK);
    return 0;
  }

  do
  {
    sprintf(buf, "%s\\%s", dllPath.c_str(), fdata.cFileName);
    U_DLog("  loading %s\n", buf);

    dll_s dll;
    dll.filePath    = buf;

    // try to get dll version, w/o loading all needed DLLs
    dll.hInst       = ::LoadLibraryEx(dll.filePath.c_str(), NULL, DONT_RESOLVE_DLL_REFERENCES);
    if(dll.hInst == NULL) {
      U_DLog(" Unable to Load!\n");
      continue;
    }
    expGetVersion_fn   expGetVersion   = (expGetVersion_fn)   GetProcAddress(dll.hInst, "expGetVersion");
    if(!expGetVersion) {
      U_DLog(" expGetVersion isn't defined\n");
      FreeLibrary(dll.hInst);
      continue;
    }

    DWORD dllVer;
    if((dllVer = expGetVersion()) != MAXUTIL_PLUGIN_VERSION) {
      U_DLog(" Got version %x, UtilMgr version is %x\n", dllVer, MAXUTIL_PLUGIN_VERSION);
      FreeLibrary(dll.hInst);
      continue;
    }
    FreeLibrary(dll.hInst);

    dll.hInst       = ::LoadLibrary(dll.filePath.c_str());
    if(dll.hInst == NULL) {
      U_DLog(" Unable to Load!\n");
      continue;
    }

    // make dll name without extension
    dll.fileVersion = U_GetModuleVersion(buf);
    strcpy(buf, fdata.cFileName);
    buf[strlen(buf) - sizeof(MY_DLL_EXT)] = 0;
    dll.fileName    = buf;

    expGetFuncCount_fn expGetFuncCount = (expGetFuncCount_fn) GetProcAddress(dll.hInst, "expGetFuncCount");
    expGetFuncName_fn  expGetFuncName  = (expGetFuncName_fn)  GetProcAddress(dll.hInst, "expGetFuncName");
    expGetFuncType_fn  expGetFuncType  = (expGetFuncType_fn)  GetProcAddress(dll.hInst, "expGetFuncType");
    expInit_fn         expInit         = (expInit_fn)         GetProcAddress(dll.hInst, "expInit");
    dll.exec                           = (expExecFunc_fn)     GetProcAddress(dll.hInst, "expExecFunc");
    if(!expInit || !expGetFuncType || !expGetFuncCount || !expGetFuncName || !dll.exec) {
      U_DLog(" Can't get address of needed functions\n");
      FreeLibrary(dll.hInst);
      continue;
    }

    expInit(this->ip);

    int numFuncs = expGetFuncCount();
    for(int i=0; i<numFuncs; i++) 
    {
      dll_s::func_s fnc;
      fnc.name    = expGetFuncName(i);
      fnc.prm1[0] = 0;
      fnc.prm2[0] = 0;
      fnc.type    = expGetFuncType(i, fnc.prm1, fnc.prm2, NULL);

      dll.funcs.push_back(fnc);
    }

    // FREE library - we will load it when pressing button
    ::FreeLibrary(dll.hInst);
    dll.hInst = NULL;
    dll.exec  = NULL;

    mDlls.push_back(dll);

  } while(FindNextFile(h, &fdata) != 0);

  FindClose(h);

  return 1;
}

int CMaxMgrPanel::_RegisterButtons()
{
  char		buf[512];

  // create buttons from DLLs
  RECT rect1, rect2;
  ::GetWindowRect(hWnd, &rect1);
  ::GetWindowRect(::GetDlgItem(hWnd, IDC_TEMPBUTTON), &rect2);

  int totbuttons = 0;
  int curx       = rect2.left - rect1.left;
  int cury       = rect2.top  - rect1.top;
  LRESULT myfont = SendMessage(GetDlgItem(hWnd, IDC_VERSION), WM_GETFONT, 0, 0);

  for(size_t did = 0; did < mDlls.size(); did++) 
  {
    dll_s &dll = mDlls[did];

    sprintf(buf, "-%s,%s-", dll.fileName.c_str(), dll.fileVersion.c_str());
    dll.limiterWnd = ::CreateWindow(
      _T("STATIC"),
      buf,
      WS_CHILD | WS_VISIBLE | SS_CENTER,
        curx,
        cury,
        rect2.right - rect2.left,
        MAX_FONT_HEIGHT,
      hWnd,
      NULL,
      g_hInstance,
      NULL);
    cury += MAX_FONT_HEIGHT;
    SendMessage(dll.limiterWnd, WM_SETFONT, myfont, 1);

    for(size_t j = 0; j < dll.funcs.size(); j++) 
    {
      dll_s::func_s &fnc = dll.funcs[j];

      fnc.id = IDC_MYBUTTONS + totbuttons;

      switch(fnc.type) 
      {
        case MAXUTIL_PLUGIN_TYPE_BUTTON:
        case MAXUTIL_PLUGIN_TYPE_EXPORTER:
          fnc.hWnd = ::CreateWindow(
            _T("BUTTON"),
            fnc.name.c_str(), 
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            curx, 
            cury, 
            rect2.right - rect2.left, 
            rect2.bottom - rect2.top,
            hWnd,
            (HMENU)(fnc.id),
            g_hInstance,
            NULL);
          cury += (rect2.bottom - rect2.top + 2);
          break;

        case MAXUTIL_PLUGIN_TYPE_CHECKBOX:
          fnc.hWnd = ::CreateWindow(
            _T("BUTTON"),
            fnc.name.c_str(), 
            WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
            curx, 
            cury, 
            rect2.right - rect2.left, 
            rect2.bottom - rect2.top,
            hWnd,
            (HMENU)(fnc.id),
            g_hInstance,
            NULL);
            
          cury += (rect2.bottom - rect2.top + 2);
          CheckDlgButton(hWnd, fnc.id, *fnc.prm1 - '0');
          break;

        default:
        case MAXUTIL_PLUGIN_TYPE_DELIMITER:
/*
          fnc.hWnd = ::CreateWindow(
            _T("BUTTON"),
            "", 
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            curx + (rect2.right - rect2.left - 15) / 2, 
            cury, 
            15, //rect2.right - rect2.left - 10, 
            2,
            hWnd,
            (HMENU)(fnc.id),
            g_hInstance,
            NULL);
*/            
          cury += (4 + 2);
          break;
      }
      totbuttons ++;
    }
  }

  //::SetWindowPos(hWnd, NULL, 0, 0, rect1.right - rect1.left, cury + (rect2.bottom - rect2.top + 2)*/, SWP_NOZORDER | SWP_NOMOVE | SWP_SHOWWINDOW);
  ::InvalidateRect(hWnd, NULL, TRUE);

  return 1;
}

int CMaxMgrPanel::_CloseDLLs()
{
  U_DLog("MaxMgrPanel::_CloseDLLs()\n");

  for(size_t did = 0; did < mDlls.size(); did++) 
  {
    dll_s &dll = mDlls[did];
    for(size_t fid = 0; fid < dll.funcs.size(); fid++) 
    {
      dll_s::func_s &fnc = dll.funcs[fid];
      ::DestroyWindow(fnc.hWnd);
    }

    ::DestroyWindow(dll.limiterWnd);

    if(dll.hInst)
      FreeLibrary(dll.hInst);

    dll.funcs.clear();
  }

  mDlls.clear();

  return 1;
}

char *U_GetModuleVersion(char *fname)
{
      // Get the module path
  HMODULE hModule = GetModuleHandle(fname);
  if(!hModule) 
    return "GetModuleHandle failed";

  // Get module file name
  char moduldeFileName[MAX_PATH];
  if(!GetModuleFileName(hModule, moduldeFileName, 512)) 
    return "GetModuleFileName failed";


  // Get version info size
  DWORD doomy;
  DWORD versionInfoSize = GetFileVersionInfoSize(moduldeFileName, &doomy);
  if(!versionInfoSize) 
    return "GetFileVersionInfoSize failed";

  // Alloc the buffer
  char buffer[8192];
  if(versionInfoSize > 8192)
    return "versionInfoSize > 8192";
  
  // Find the verion resource
  if(!GetFileVersionInfo(moduldeFileName, 0, versionInfoSize, buffer)) 
    return "GetFileVersionInfo failed";

  DWORD *versionTab;
  unsigned int versionSize;
  if(!VerQueryValue (buffer, "\\", (void**)&versionTab, &versionSize)) 
    return "VerQueryValue failed";

  // Get the pointer on the structure
  VS_FIXEDFILEINFO *info=(VS_FIXEDFILEINFO*)versionTab;
  if(!info) 
    return "VS_FIXEDFILEINFO * is NULL";

  // Setup version number
  static char version[512];
  sprintf(version, "%d.%d.%d.%d", 
    info->dwFileVersionMS>>16, 
    info->dwFileVersionMS&0xffff, 
    info->dwFileVersionLS>>16,  
    info->dwFileVersionLS&0xffff);

  return version;
};

// Replace some characters we don't care for.
TCHAR *U_FixName2(TCHAR* name)
{
  static TCHAR buf[MAX_PATH];
  TCHAR *pIn  = name;
  TCHAR	*pOut = buf;

  while(*pIn) {
    if(*pIn == '"')
      *pOut++ = '\'';
    else if ((*pIn > 0 && *pIn <= ' ') || *pIn > 'z' || *pIn < 0)
      ; //*pIn = _T('_');
    else 
      *pOut++ = *pIn;
    pIn++;
  }
  *pOut = 0;

  return buf;
}

////////////////////////////////////////////////////////////////////////////////
// CMaxMgrPanel Dialog Procedure to handle User and System Interactions
static INT_PTR CALLBACK g_CMaxMgrPanelProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg) 
  {
    case WM_INITDIALOG:
    {
      thePlugPanel._OpenDLLs();

      // determine dialog size & resize it!
      RECT rect1, rect2;
      ::GetWindowRect(hWnd, &rect1);
      ::GetWindowRect(::GetDlgItem(hWnd, IDC_TEMPBUTTON), &rect2);

      int cury       = rect2.top  - rect1.top;
      for(size_t did = 0; did < thePlugPanel.mDlls.size(); did++) 
      {
        cury += MAX_FONT_HEIGHT;
        for(size_t fid = 0; fid < thePlugPanel.mDlls[did].funcs.size(); fid++) {
          cury += (rect2.bottom - rect2.top + 2);
	}
        cury += MAX_FONT_HEIGHT;
      }
      ::SetWindowPos(hWnd, NULL, 0, 0, rect1.right - rect1.left, cury, SWP_NOZORDER | SWP_NOMOVE | SWP_SHOWWINDOW);

      // init plugin window
      thePlugPanel.Init(hWnd);

      if(GetDlgItem(hWnd, IDC_VERSION)) {
        char buf[512];
	sprintf(buf, "%s %s", THIS_DLL_FILENAME, U_GetModuleVersion(THIS_DLL_FILENAME));
        SetWindowText(GetDlgItem(hWnd, IDC_VERSION), buf);
      }

      break;
    }

    case WM_DESTROY:						//Panel's extra destruction
      thePlugPanel.Destroy(hWnd);
      break;

    case WM_COMMAND:
      if(LOWORD(wParam) >= IDC_MYBUTTONS && LOWORD(wParam) <= IDC_MYBUTTONS + 64)
      {
        for(size_t did = 0; did < thePlugPanel.mDlls.size(); did++) 
        {
          CMaxMgrPanel::dll_s &dll = thePlugPanel.mDlls[did];
          for(size_t fid = 0; fid < dll.funcs.size(); fid++) 
          {
            CMaxMgrPanel::dll_s::func_s &fnc = dll.funcs[fid];
            if(fnc.id == LOWORD(wParam)) {
              if(fnc.type == MAXUTIL_PLUGIN_TYPE_DELIMITER)
                break;

              expParams prm;
	      prm.dllId      = int(did);
	      prm.ip         = thePlugPanel.ip;
	      prm.iu         = thePlugPanel.iu;
	      prm.hWnd       = thePlugPanel.hWnd;
	      prm.bShowLog   = thePlugPanel.bShowLog;
	      strcpy(prm.exportFile, thePlugPanel.exportPath);
	      prm.dw1        = 0;

              // if this is checkbox, flip value and pass it to function
              if(fnc.type == MAXUTIL_PLUGIN_TYPE_CHECKBOX)
	      {
	        prm.dw1 = IsDlgButtonChecked(hWnd, LOWORD(wParam)) ^ 1;
		CheckDlgButton(hWnd, LOWORD(wParam), prm.dw1);
	      }
   
	      // if this is a exporter button - make default path and default filename
              if(fnc.type == MAXUTIL_PLUGIN_TYPE_EXPORTER) 
	      {
	        CStr curSceneName = thePlugPanel.ip->GetCurFileName();
	        if(*curSceneName == 0) {
	          curSceneName = "test";
	        }

	        if(thePlugPanel.bUseDefaultPath) 
	        {
	          // always using default export path
	          sprintf(prm.exportFile, "%s\\%s", thePlugPanel.exportPath, U_FixName2(curSceneName));
                  if(strrchr(prm.exportFile, '.')) *strrchr(prm.exportFile, '.') = 0;
		  strcat(prm.exportFile, ".");
		  strcat(prm.exportFile, fnc.prm1);
	        } 
		else if(*fnc.prm1 == '!')
		{
		  // we do not need any picking at all
	          sprintf(prm.exportFile, "%s\\temp.temp", thePlugPanel.exportPath);
		}
		else if(*fnc.prm1)
		{
		  // have file description - select file
	          OPENFILENAME ofn;
		  char buf[MAX_PATH] = "";

		  ZeroMemory(&ofn, sizeof(ofn));
		  ofn.lStructSize     = sizeof(OPENFILENAME);
		  ofn.hwndOwner       = thePlugPanel.hWnd;
		  ofn.lpstrFilter     = fnc.prm1 + 4;
		  ofn.lpstrInitialDir = thePlugPanel.exportPath;
		  ofn.lpstrFile       = buf;
		  ofn.nMaxFile        = sizeof(buf)/sizeof(buf[0]);
		  ofn.lpstrDefExt     = fnc.prm1;
		  ofn.Flags           = OFN_EXPLORER | OFN_LONGNAMES | OFN_OVERWRITEPROMPT;
	          if(GetSaveFileName(&ofn) == 0) 
		    break;
		  //nFilterIndex
		  strcpy(prm.exportFile, buf);
	        }
                else 
                {
                  // select output directory
	          char buf[MAX_PATH];
	          if(!DirectoryPicker(hWnd, "Choice Export Directory", thePlugPanel.exportPath, buf, NULL))
	            break;

	          strcpy(thePlugPanel.exportPath, buf);
	          SetWindowText(GetDlgItem(hWnd,ID_PATH), thePlugPanel.exportPath);
		  sprintf(prm.exportFile, "%s\\temp.tmp", buf);
                }
                U_DLog("prm.exportfile: %s\n", prm.exportFile);
              } 

	      // if DLL wasn't loaded - load it!
              if(!dll.hInst) {
                dll.hInst  = ::LoadLibrary(dll.filePath.c_str());
                dll.exec   = (expExecFunc_fn) ::GetProcAddress(dll.hInst, "expExecFunc");
	      }
      
	      int modeless = 0;
	      dll.exec(int(fid), &prm, &modeless);
              
	      // redraw view - if something was changed
	      thePlugPanel.ip->RedrawViews(thePlugPanel.ip->GetTime());


	      if(!modeless) {
U_DLog("Mgr: DLL Unloaded immediatly!\n");
	        ::FreeLibrary(dll.hInst);
	        dll.hInst  = NULL;
	        dll.exec   = NULL;
	      }

	      break;
	    }

	  }
	}

        break;
      }

      switch(LOWORD(wParam)) 
      {			
        case IDC_ABOUT:
	  break;

	case IDC_CHECK1:
	  thePlugPanel.bUseDefaultPath = IsDlgButtonChecked(hWnd, IDC_CHECK1);
	  EnableWindow(GetDlgItem(hWnd, ID_PATH), thePlugPanel.bUseDefaultPath);
	  break;
	case IDC_CHECK3:
	  thePlugPanel.bShowLog = IsDlgButtonChecked(hWnd, IDC_CHECK3);
	  break;

	case IDC_SETOUTPUTDIR:
	{
	  char	buf[MAX_PATH];
	  if(DirectoryPicker(hWnd, "Choice Export Directory", thePlugPanel.exportPath, buf, NULL))
	    strcpy(thePlugPanel.exportPath, buf);
	  SetWindowText(GetDlgItem(hWnd,ID_PATH), thePlugPanel.exportPath);
	  break;
	}

	case IDC_DLL_UNLOAD:
	{
          CMaxMgrPanel::dll_s &dll = thePlugPanel.mDlls[(WORD)lParam];
U_DLog("Mgr: DLL Unloaded after message!\n");
	  if(dll.hInst) {
	    ::FreeLibrary(dll.hInst);
	    dll.hInst  = NULL;
	    dll.exec   = NULL;
	  }
	  
	  break;
	}

	case IDC_REFRESHDLL:
	{
          int isOk = 1;
          for(size_t did = 0; did < thePlugPanel.mDlls.size(); did++) 
          {
            CMaxMgrPanel::dll_s &dll = thePlugPanel.mDlls[did];
	    if(dll.hInst != NULL) {
	      char buf[512];
	      sprintf(buf, "Module %s is loaded - can't reload\n", dll.fileName.c_str());
	      MessageBox(NULL, buf, "!", MB_OK);

	      isOk = 0;
	      break;
	    }
	  }

	  if(isOk) {
	    thePlugPanel._CloseDLLs();
	    thePlugPanel._OpenDLLs();
	    thePlugPanel._RegisterButtons();
	  }
	  break;
	}

        default:
	  break;
      }
      break;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MOUSEMOVE:
      thePlugPanel.ip->RollupMouseMessage(hWnd, msg, wParam, lParam);
      break;

    default:
      return FALSE;
  }


  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Display the Plugin's Panel
void CMaxMgrPanel::BeginEditParams(Interface* ip, IUtil* iu)
{
  this->ip = ip;
  this->iu = iu;

  if(hWnd) {
    SetActiveWindow(hWnd);
    return;
  }

  // Display the plugin's Rollup Dialog
  hWnd = ip->AddRollupPage(
             g_hInstance, 
	     MAKEINTRESOURCE(IDD_PANEL),
	     g_CMaxMgrPanelProc, 
	     _T("TSGroup Tools"),
	     0);

  ReadConfig();
  SetWindowText(GetDlgItem(hWnd,ID_PATH), exportPath);
  CheckDlgButton(hWnd, IDC_CHECK1, bUseDefaultPath);
  CheckDlgButton(hWnd, IDC_CHECK3, bShowLog);

  // _OpenDLLs(); - it was called in WM_INIT DIALOG - for size calculation
  _RegisterButtons();

  return;
}

int CMaxMgrPanel::ExecFP_Call(char *_cmd, Tab<char*> *_args)
{
  _OpenDLLs();

  int found = 0;
  for(size_t did = 0; did < thePlugPanel.mDlls.size(); did++) 
  {
    CMaxMgrPanel::dll_s &dll = thePlugPanel.mDlls[did];
    
    //right now command will be passed to all modules, because we have different filenames for them x32/x64/2011-2012
    //if(_stricmp(module, dll.fileName.c_str()) != NULL)
    //  continue;
    found++;

    expParams prm;
    prm.dllId      = int(did);
    prm.ip         = ip;
    prm.iu         = NULL;
    prm.hWnd       = ip->GetMAXHWnd();
    prm.bShowLog   = 0; 
    *prm.exportFile= 0;
    prm.dw1        = 0;
    prm.FP_cmd     = _cmd;
    prm.FP_args    = _args;

    // if DLL wasn't loaded - load it!
    if(!dll.hInst) {
      dll.hInst  = ::LoadLibrary(dll.filePath.c_str());
      dll.exec   = (expExecFunc_fn) ::GetProcAddress(dll.hInst, "expExecFunc");
    }
      
    int modeless = 0;
    dll.exec(FP_FUNC_ID, &prm, &modeless);
    // redraw view - if something was changed, aha.
    thePlugPanel.ip->RedrawViews(thePlugPanel.ip->GetTime());

    if(!modeless) {
      ::FreeLibrary(dll.hInst);
      dll.hInst  = NULL;
      dll.exec   = NULL;
    }
  }

  if(!found) {
    char buf[512];
    sprintf(buf, "Can't find module \"%s\"", "");
    MessageBox(NULL, buf, "TSGroup UtilMgr", MB_OK);
    return false;
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////
// Free the Plugins' Panel
void CMaxMgrPanel::EndEditParams(Interface *ip, IUtil *iu)
{
  this->ip = ip;
  this->iu = iu;
}

////////////////////////////////////////////////////////////////////////////////
// Panel Initialization callback
void CMaxMgrPanel::Init(HWND hWnd)
{
}

////////////////////////////////////////////////////////////////////////////////
// Panel Destruction callback
void CMaxMgrPanel::Destroy(HWND hWnd)
{
  WriteConfig();

  _CloseDLLs();
  // Remove the Rollup Dialog
  // Remove the Rollup Dialog
  // ** by some reason we can't do this in destroy. 
  // ** and we neeed opened windows for modeless dialog boxes & stuff
  //ip->DeleteRollupPage(hWnd);
  hWnd = NULL;
}


void CMaxMgrPanel::ReadConfig()
{
  char buf[512];
  sprintf(buf, "%s\\TSG_MaxUtilMgr.cfg", ip->GetDir(APP_PLUGCFG_DIR));
  
  FILE* f;
  if((f=fopen(buf, "rt")) == NULL)
    return;

  fgets(buf, sizeof(buf), f);
  sscanf(buf, "%d %d", &bUseDefaultPath, &bShowLog);
  fgets(buf, sizeof(buf), f);
  buf[strlen(buf)-1]=0;
  strcpy(exportPath, buf);

  fclose(f);
  return;
}

void CMaxMgrPanel::WriteConfig()
{
  char buf[512];
  sprintf(buf, "%s\\TSG_MaxUtilMgr.cfg", ip->GetDir(APP_PLUGCFG_DIR));
  FILE *f;
  if((f=fopen(buf, "wt")) == NULL) {
    U_DLog("!!! CMaxMgrPanel::WriteConfig failed to open %s, %d\n", buf, _errno);
    return;
  }

  fprintf(f, "%d %d\n", bUseDefaultPath, bShowLog);
  fprintf(f, "%s\n", exportPath);

  fclose(f);
  return;
}

// TSGroup_Tools.UtilMgr.RunModule "A"  "B"  #("buh") 
