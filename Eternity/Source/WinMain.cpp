#include "r3dPCH.h"
#include "r3d.h"
#include "r3dDebug.h"


#include "../SF/Console/EngineConsole.h"

char __r3dCmdLine[1024];

typedef bool (*Win32MsgProc_fn)(UINT uMsg, WPARAM wParam, LPARAM lParam);

//hack: add temp handler for external message processing

namespace
{
	const int		NUM_MSG_PROC_MAX = 8;
	Win32MsgProc_fn	r3dApp_MsgProc3[NUM_MSG_PROC_MAX] = {0};
}

static	int		StartWinHeight = 300;
static	int		StartWinWidth  = 300;
static	int		curDispWidth   = 0;
static	int		curDispHeight  = 0;

namespace win
{
	HINSTANCE	hInstance  = NULL;
	HWND		hWnd       = NULL;
	const char 	*szWinName = "$o, yeah!$";
	HICON		hWinIcon   = 0;
	int		bSuspended	= 0;
	int		bNeedRender	= 1;
	
	void		HandleActivate();
	void		HandleDeactivate();
	void		HandleMinimize();
	void		HandleRestore();

	int		Init();
}

void RegisterMsgProc (Win32MsgProc_fn proc)
{
	for (int i = 0; i<NUM_MSG_PROC_MAX; i++ )
	{
		if (!r3dApp_MsgProc3[i])
		{
			r3dApp_MsgProc3[i] = proc;
			return;
		}
	}
	r3d_assert ( 0 && "RegisterMsgProc error; register more than NUM_MSG_PROC_MAX MsgProc." );
}

void UnregisterMsgProc (Win32MsgProc_fn proc)
{
	for (int i = 0; i<NUM_MSG_PROC_MAX; i++ )
	{
		if (r3dApp_MsgProc3[i] == proc)
		{
			r3dApp_MsgProc3[i] = NULL;
			return;
		}
	}
	r3d_assert ( 0 && "UnregisterMsgProc error." );
}

#define INPUT_KBD_STACK	32
volatile static  int   input_ScanStack[INPUT_KBD_STACK + 2];
volatile static  int   * volatile input_StackHead = input_ScanStack;
volatile static  int   * volatile input_StackTail = input_ScanStack;


bool g_bExit = false;

bool IsNeedExit()
{
	return g_bExit;
}

int win::input_Flush()
{
  input_StackHead = input_ScanStack;
  input_StackTail = input_ScanStack;

  return 1;
}

int win::kbhit()
{
  if(input_StackHead == input_StackTail)
    return 0;
  else
    return 1;
}

int win::getch( bool bPop )
{
	int	ch;

  if(!win::kbhit())
    return 0;

  ch = *input_StackTail;
  if ( bPop )
	  input_StackTail++;

  if(input_StackTail >= input_ScanStack + INPUT_KBD_STACK)
    input_StackTail = input_ScanStack;

  return ch;
}


static 
void wnd__DrawLogo()
{
#if TSG_BITMAP
	HDC		hDC;
	HANDLE		hBmp;

  // load background image and center our window
  hBmp = LoadImage(
    GetModuleHandle(NULL), 
    MAKEINTRESOURCE(TSG_BITMAP),
    IMAGE_BITMAP,
    win__Width,
    win__Height,
    LR_DEFAULTCOLOR
    );
  if(hBmp == NULL) return;

  if(hDC = GetDC(win::hWnd))
  {
	HDC	dc;

    dc = CreateCompatibleDC(hDC);
    SelectObject(dc, hBmp);
    BitBlt(hDC, 0, 0, win__Width, win__Height, dc, 0, 0, SRCCOPY);
    DeleteDC(dc);

    ReleaseDC(win::hWnd, hDC);
  }

  DeleteObject(hBmp);

#endif

  return;
}

bool winMouseWasVisible = true;
extern bool r3dMouse_visible;
void win::HandleActivate()
{
	//  if(!win::bSuspended)
	//		return;

	win::bSuspended = 0;
	win::bNeedRender = 1;

	if(Mouse)    Mouse->SetCapture();
	if(Keyboard) Keyboard->ClearPressed();

	if( winMouseWasVisible || g_cursor_mode->GetInt() )
		r3dMouse::Show(true);
	else 
		r3dMouse::Hide(true);
}

void win::HandleDeactivate()
{
	//  if(win::bSuspended)
	//    return;
	win::bSuspended = 1;

	if( !(r_render_on_deactivation && r_render_on_deactivation->GetInt()) )
		win::bNeedRender = 0;
	else
		win::bNeedRender = 1;

	winMouseWasVisible = r3dMouse_visible;

	if(Mouse)    Mouse->ReleaseCapture();
	if(Keyboard) Keyboard->ClearPressed();
}

void win::HandleMinimize()
{
	win::bNeedRender = 0;
}

void win::HandleRestore()
{
	win::bNeedRender = 1;
}


//
//
//
// main Callback function
//
//

/*
namespace 
{
  //
  // some vars to simulate move moving by ourselves.
  //
  
  static bool bOnCaption = false;	// true if we're on caption
  static bool bDragging  = true;	// true if we're dragging our window
  static int  dragX      = 0;		// offset of drag point, relative to window origin
  static int  dragY      = 0;		// offset of drag point, relative to window origin
  
  static void dragDisable()
  {
    if(!bDragging) return;
  
    ReleaseCapture();
    bDragging = false;

    return;
  }  
};
*/

void (*OnDblClick)() = 0;
void (*OnDrawClipboardCallback)(WPARAM wParam, LPARAM lParam) = 0;

HWND            hWndNextViewer = 0; // clipboard support

static LRESULT CALLBACK win__WndFunc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  static int userProcActive = 0;

  if(userProcActive) {
    // in very rare occasions we can reenter this function
    // one of cases is that when error is thrown from processing user or UI functions
    // so, in this case we'll enter to never ending loop
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
 
  userProcActive = true;
  bool bAnyCompleted = false;
  for(int i = 0; i < NUM_MSG_PROC_MAX; i++ )
  {
	  if ( r3dApp_MsgProc3 [i] )
		  bAnyCompleted |= r3dApp_MsgProc3[i](uMsg, wParam, lParam);
  }
  userProcActive = false;
  if ( bAnyCompleted ) return 0;


  //r3dOutToLog("uMsg %x\n", uMsg);
  switch(uMsg) 
  {
    case WM_CLOSE:
    {
      r3dOutToLog("alt-f4 pressed\n");
      r3dOutToLog("...terminating application\n");

      ClipCursor(NULL);
      
      //HRESULT res = TerminateProcess(r3d_CurrentProcess, 0);

	  g_bExit = true;
      return 0;
    }
    
    case WM_CONTEXTMENU:
      // disable context menu
      return 0;
      
    case WM_ENTERMENULOOP:
      r3dOutToLog("!!!warning!!! r3dApp entered to modal menu loop\n");
      break;
      
    case WM_ENTERSIZEMOVE:
      //r3d_assert(0 && "WM_ENTERSIZEMOVE");
      win::HandleDeactivate();
      return 0;
      
    case WM_EXITSIZEMOVE:
      r3dOutToLog("WM_EXITSIZEMOVE: %d\n", win::bSuspended);
      win::HandleActivate();
      break;

/*
    case WM_CAPTURECHANGED:
      // if we lose capture from our window - disable dragging
      if(bDragging && (HWND)lParam != hWnd) {
        // note: do not call dragDisable(), because it will call ReleaseCapture()
        bDragging = false;
        break;
      }
      break;

    case WM_LBUTTONUP:
    {
      if(bDragging) {
        dragDisable();
      }
      break;
    }

    case WM_LBUTTONDOWN:
    {
      if(bOnCaption) {
        // start dragging our window, calc drag anchor point, relative to window corner
        POINT pp;        
        GetCursorPos(&pp);
        RECT rr;
        GetWindowRect(hWnd, &rr);

        dragX = pp.x - rr.left;
        dragY = pp.y - rr.top;
        
        ::SetCapture(hWnd);
        bDragging = true;
      }
      
      break;
    }
    
    case WM_MOUSEMOVE:
    {
      if(bDragging) {
        POINT pp;        
        GetCursorPos(&pp);
        SetWindowPos(hWnd, 0, pp.x - dragX, pp.y - dragY, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER);
      }

      break;
    }

    case WM_NCHITTEST:
    {
      if(!r3dRenderer || !r3dRenderer->d3dpp.Windowed) 
        break;
      if(win::bSuspended)
        break;
        
      // if cursor is clipped somewhere, don't allow this window dragging at all.
      RECT clipRect;
      ::GetClipCursor(&clipRect);
      if(clipRect.left != 0 || clipRect.top != 0 || clipRect.right != curDispWidth || clipRect.bottom != curDispHeight) {
        return HTCLIENT;
      }
        
      LONG hitTest = DefWindowProc(hWnd, uMsg, wParam, lParam);
      if(hitTest == HTCLIENT && bOnCaption) {
        if(Mouse) Mouse->Hide(); 
        if(Mouse) Mouse->SetCapture();
        
        bOnCaption = false;
        return hitTest;
      }
      
      if(hitTest == HTCAPTION && bOnCaption) {
        // return that we still in client area - so windows will not start windows moving by itself
        return HTCLIENT;
      }
      
      if(hitTest == HTCAPTION && !bOnCaption) {
        if(Mouse) Mouse->Show(); 
        if(Mouse) Mouse->ReleaseCapture();

        // we will simuate dragging by ourselves, otherwise game will be in modal dragging loop!
        bOnCaption = true;
        return HTCLIENT;
      }

      return hitTest;
    }
*/    
    
    // disable menu calling when ALT pressed
    case WM_SYSCOMMAND:
    {
      // disable those system commands
      switch(wParam & 0xFFFF) {
        case 0xF093:		// system menu on caption bar
        case SC_KEYMENU:
        case SC_MOVE:
          return 0;
        case SC_MINIMIZE:
          win::HandleMinimize();
          break;
        case SC_RESTORE:
          win::HandleRestore();
          break;

      }

      r3dOutToLog("r3dapp: SysCmd: %x\n", wParam & 0xFFFF);
      break;
    }

    case WM_NCACTIVATE: 
      //dragDisable();

      if((wParam & 0xFFFF) == TRUE) 
        win::HandleActivate();
      break;

    case WM_ACTIVATE:
      //dragDisable();

      if((wParam & 0xFFFF) == WA_INACTIVE) {
        win::HandleDeactivate();
      } else {
        win::HandleActivate();
      }
      break;

	case WM_KEYDOWN:
	{
		EngineConsole::ProcessKey( wParam );
		break;
	}

	case WM_LBUTTONDBLCLK:
		if( OnDblClick )
			OnDblClick() ;
		break;

    // store char to input stream
    case WM_CHAR:
    {
		EngineConsole::ProcessChar( wParam );

        int	ch;

      ch              = (TCHAR)wParam;
      *(input_StackHead++) = ch;
      if(input_StackHead >= input_ScanStack + INPUT_KBD_STACK)
	input_StackHead = input_ScanStack;
      break;
    }

    case WM_PAINT:
    {
	HDC         hDC;
	PAINTSTRUCT ps;

      hDC = BeginPaint(hWnd,&ps);
      wnd__DrawLogo();
      EndPaint(hWnd,&ps);
      break;
    }

	case WM_DRAWCLIPBOARD:
		if(OnDrawClipboardCallback)
			OnDrawClipboardCallback(wParam, lParam);
		break;

	case WM_CHANGECBCHAIN:
		if ((HWND) wParam == hWndNextViewer) 
			hWndNextViewer = (HWND) lParam; 
		else if (hWndNextViewer != NULL)
			SendMessage(hWndNextViewer, WM_CHANGECBCHAIN, wParam, lParam); 
		break;

	case WM_CREATE:
		hWndNextViewer = SetClipboardViewer(hWnd);
		break;

    case WM_DESTROY:
		if (!hWndNextViewer)
			ChangeClipboardChain(hWnd, hWndNextViewer);
      //PostQuitMessage (0);
		g_bExit = true;
      break;
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void r3dWinStyleModify(HWND hWnd, int add, DWORD style)
{
  if(add)
    SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(win::hWnd, GWL_STYLE) | style);
  else
    SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(win::hWnd, GWL_STYLE) & ~style);
}


BOOL win::Init()
{
  static const char* szWinClassName = "r3dWin";

  r3d_assert(hInstance != NULL);

  WNDCLASS  wndclass;
  wndclass.style         = CS_DBLCLKS | CS_GLOBALCLASS;
  wndclass.lpfnWndProc   = win__WndFunc;		// window function
  wndclass.cbClsExtra    = 0;				// no extra count of bytes
  wndclass.cbWndExtra    = 0;				// no extra count of bytes
  wndclass.hInstance     = GetModuleHandle(NULL);	// this instance
  wndclass.hIcon         = (hWinIcon) ? hWinIcon : LoadIcon(NULL, IDI_APPLICATION);
  wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wndclass.lpszMenuName  = NULL;
  wndclass.lpszClassName = szWinClassName;
  RegisterClass(&wndclass);

	HDC		disp_dc;

  disp_dc  = CreateIC("DISPLAY", NULL, NULL, NULL);
  curDispWidth  = GetDeviceCaps(disp_dc, HORZRES);
  curDispHeight = GetDeviceCaps(disp_dc, VERTRES);
  //disp_bpp = GetDeviceCaps(disp_dc, BITSPIXEL);
  DeleteDC(disp_dc);

  hWnd = CreateWindow(
    /* window class name */       szWinClassName,            			
    /* window caption*/           szWinName,					
    /* window style */            WS_VISIBLE | WS_OVERLAPPEDWINDOW,
    /* initial x position */      (curDispWidth - StartWinWidth) / 2,		
    /* initial y position */      (curDispHeight - StartWinHeight) / 2,		
    /* initial x size */          StartWinWidth,				
    /* initial y size */          StartWinHeight,				
    /* parent window handle */    NULL,                 			
    /* window menu handle*/       NULL,                 			
    /* program instance handle */ GetModuleHandle(NULL), 			
    /* creation parameters */     NULL);                 			

  if(!hWnd) {
    MessageBox(GetActiveWindow(), "Window Creation Failed", "Error", MB_OK);
    return FALSE;
  }

/*
  //WinStyleModify(0, WS_SYSMENU);
  WinStyleModify(0, WS_BORDER);
  WinStyleModify(0, WS_CAPTION);
  WinStyleModify(1, WS_DLGFRAME);
*/

  // set icon
  ::SendMessage(hWnd, WM_SETICON, TRUE,  (LPARAM)wndclass.hIcon);
  ::SendMessage(hWnd, WM_SETICON, FALSE, (LPARAM)wndclass.hIcon);
  
  r3dWinStyleModify(hWnd, 0, WS_THICKFRAME);	// prevent resize
  r3dWinStyleModify(hWnd, 0, WS_MAXIMIZEBOX);	// prevent maximize
  //r3dWinStyleModify(hWnd, 0, WS_MINIMIZEBOX);	// prevent minimize
  

  ShowWindow(win::hWnd, FALSE);
  wnd__DrawLogo();
  InvalidateRect(win::hWnd, NULL, FALSE);
  UpdateWindow(win::hWnd);
  SetFocus(win::hWnd);

  return TRUE;
}



int win::ProcessSuspended()
{
  if(!bSuspended)
    return FALSE;

  MSG msg;
  while(PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE))
  {
    if(!GetMessageW(&msg, NULL, 0, 0)) 
      return 1;

    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return TRUE;
}


static void startupFunc(DWORD in)
{
//  in = in;
  
  game::PreInit();  

  win::Init();

  game::Init();

  game::MainLoop();

  game::Shutdown();
}

//
//
// the One And Only - WinMain!
//
//
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
#ifdef ENABLE_MEMORY_DEBUG
	//_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
	//_CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );

	_CrtMemState _ms;
	_CrtMemCheckpoint(&_ms);
#endif
#endif

#if 0
	char* __internal_gnrt_lkey(const char* name, int exp_year, int exp_month, int exp_day);
	void r3dLibraryInit(char* license);
	void checkLicenseKey();

	char* lic = __internal_gnrt_lkey("testing", 2010, 7, 10);
	r3dLibraryInit(lic);
	checkLicenseKey();
#endif

	// set our game to run only one processor (we're not using multithreading)
	// that will help with timer
	//DWORD_PTR oldmask = ::SetThreadAffinityMask(::GetCurrentThread(), 0);

	// Set xml custom memory management functions
#ifdef USE_R3D_MEMORY_ALLOCATOR
	pugi::set_memory_management_functions(&r3dPugiAllocateMemory, &r3dPugiDeallocateMemory);
#endif

  r3dscpy(__r3dCmdLine, lpCmdLine);
  
#ifdef _DEBUG
  //DWORD NewMask = ~(_EM_ZERODIVIDE | _EM_OVERFLOW | _EM_INVALID);
  //_controlfp(NewMask, _MCW_EM);
#endif 

  win::hInstance = hInstance;

  r3dThreadEntryHelper(startupFunc, 0);

  PostQuitMessage(0);

#ifdef _DEBUG
#ifdef ENABLE_MEMORY_DEBUG
   _CrtMemDumpAllObjectsSince(&_ms);
#endif
#endif
  return 0;
}
