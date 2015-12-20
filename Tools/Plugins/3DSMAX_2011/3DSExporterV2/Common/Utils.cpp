// Utils.cpp : implementation file
//
#include "stdafx.h"
#include "Utils.h"
#include "Log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


	CLog*		gLog = NULL;

	//
	//
	//
	//
	//


void U_Abort2(int src_line, char *src_fname, char *msg)
{
	char	buf[512];
  sprintf(buf, "%s\n, %s, line %d\n", msg ? msg : "(null)", src_fname, src_line);
  MessageBox(NULL, buf, "Abort", MB_OK);
  ExitThread(0);
  exit(0); //throw "error";
}

int U_DLog(char *Str, ...)
{
	char 	buf[1024];

  va_list ap;
  va_start(ap, Str);
  vsprintf(buf, Str, ap);
  va_end(ap);

  OutputDebugString(buf);

  return 1;
}

void U_OpenLog()
{
  if(gLog)
    return;

  gLog = CLog::CreateWnd();
  return;
}

void U_CloseLog(int wait)
{
  if(!gLog)
    return;

  gLog->m_disableClose = 0;

  if(wait)
    gLog->WaitForOk();

  if(gLog->m_hWnd) gLog->SendMessage(WM_CLOSE, 0, 0);
  gLog->DestroyWindow();

  //NOTE:
  // by some reason deleting CLog will cause 3dsMax to crash
  // i have no idea why. So don't delete it and let it leak...
  //delete gLog;

  gLog = NULL;
}

int U_Log(const char* Str, ...)
{
	char 	buf[1024];

  va_list ap;
  va_start(ap, Str);
  vsprintf(buf, Str, ap);
  va_end(ap);

  if(gLog)
    gLog->Log(buf);

  U_DLog(buf);

  return 1;
}

void U_LogIdleTick()
{
  if(!gLog)
    return;

  MSG msg; 
  while(::PeekMessage(&msg, NULL,0,0,PM_REMOVE)) 
  { 
    ::TranslateMessage(&msg); 
    ::DispatchMessage(&msg); 
  }
    
  // and force focus back to log
  gLog->SetFocus();
}

UD3D9::UD3D9()
{
  *pClassName = 0;
  pd3d        = NULL;
  pd3ddev     = NULL;
  hWnd        = NULL;
  
  //Init();
}

UD3D9::~UD3D9()
{
  if(pd3ddev) Close();
}

void UD3D9::Close()
{
  if(pd3ddev)     pd3ddev->Release();
  if(pd3d)        pd3d->Release(); 
  if(hWnd)        DestroyWindow(hWnd);
  if(*pClassName) UnregisterClass(pClassName, GetModuleHandle(NULL));
  
  pd3ddev = 0;
}

LRESULT CALLBACK D3D9StaticWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
  return DefWindowProc(hWnd, uMsg, wParam, lParam );
}

void UD3D9::Init()
{
  HRESULT hr;
  
  sprintf(pClassName, "D3D9_%d", GetTickCount());

  // Register the windows class
  WNDCLASS wndClass;
  wndClass.style         = CS_DBLCLKS;
  wndClass.lpfnWndProc   = D3D9StaticWndProc;
  wndClass.cbClsExtra    = 0;
  wndClass.cbWndExtra    = 0;
  wndClass.hInstance     = GetModuleHandle(NULL);
  wndClass.hIcon         = NULL;
  wndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wndClass.lpszMenuName  = NULL;
  wndClass.lpszClassName = pClassName;
  if(!RegisterClass(&wndClass)) throw "RegisterClass() failed"; 

  hWnd = CreateWindowEx(
    0,
    pClassName,            			// window class name
    "temp d3d window",				// window caption
    WS_POPUP | WS_SYSMENU,			// window style
    0,						// initial x position
    0,						// initial y position
    2,						// initial x size
    2,						// initial y size
    NULL,                  			// parent window handle
    NULL,                  			// window menu handle
    GetModuleHandle(NULL), 			// program instance handle
    NULL);                 			// creation parameters
  if(!hWnd) throw "Window Creation Failed";

  pd3d = Direct3DCreate9(D3D_SDK_VERSION);
  if(!pd3d) throw "Direct3DCreate9 failed";

  // Create the D3DDevice
  D3DPRESENT_PARAMETERS d3dpp;
  ZeroMemory(&d3dpp, sizeof(d3dpp) );
  d3dpp.Windowed               = 1;
  d3dpp.hDeviceWindow          = hWnd;
  d3dpp.BackBufferWidth        = 2;
  d3dpp.BackBufferHeight       = 2;
  d3dpp.BackBufferFormat       = D3DFMT_UNKNOWN; //X8R8G8B8;
  d3dpp.BackBufferCount        = 1;
  d3dpp.MultiSampleType        = D3DMULTISAMPLE_NONE;
  d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
  d3dpp.Flags                  = 0;
  d3dpp.EnableAutoDepthStencil = FALSE;
  d3dpp.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
  d3dpp.FullScreen_RefreshRateInHz = 0;

  hr = pd3d->CreateDevice(
    D3DADAPTER_DEFAULT,
    D3DDEVTYPE_HAL,
    hWnd,
    D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
    &d3dpp,
    &pd3ddev);
  if(hr != D3D_OK) throw "D3D9 CreateDevice failed";

  return;
}

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
BOOL DirectoryPicker(HWND hWnd, const char* title, const char* initial, char* out_dir, const char* root)
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
        strcpy(out_dir, buffer);
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
