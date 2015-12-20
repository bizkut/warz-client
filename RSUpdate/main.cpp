#include "r3dPCH.h"
#include "r3d.h"
#include "shellapi.h"
#include "shobjidl.h"

#ifndef __ITaskbarList3_FWD_DEFINED__
  #include "ITaskbarList3.h"
#endif
const IID r3dIID_ITaskbarList3 = { 0xEA1AFB91, 0x9E28, 0x4B86, { 0x90, 0xE9, 0x9E, 0x9F, 0x8A, 0x5E, 0xEF, 0xAF } };

#include "main.h"
#include "resource.h"

#include "UPDATER_CONFIG.h"
#include "Updater.h"
#include "GenerateUpdaterXML.h"
#include "SelfUpdateEXE.h"
#include "FirewallUtil.h"
#include "TexImage2D.h"
#include "EULAWnd.h"
#include "HWInfoPost.h"
#include "Win32Input.h"
#include "SteamHelper.h"

#ifdef USE_VMPROTECT
  #error "DO NOT compile updater with USE_VMPROTECT"
#endif

#pragma comment(lib, "Wbemuuid.lib")

extern	HANDLE		r3d_CurrentProcess;
extern	bool		g_bExit;
extern	int		_r3d_bTerminateOnZ;
extern	void		r3dWinStyleModify(HWND hWnd, int add, DWORD style);

	bool		g_isConsoleUpdater = false;
	extern void	cmdLine_MainLoop(CUpdater& updater);
	extern void	cmdLine_Init();
	
	ITaskbarList3*  g_taskbar = NULL;
	HANDLE		g_updaterEvt = NULL;

	r3dTexImage2D*	g_bkgLogin  = NULL;
	r3dTexImage2D*	g_bkgUpdate = NULL;
	r3dTexImage2D*	g_bkgSerialCheck = NULL;
	r3dTexImage2D*	g_bkgRegister = NULL;
	r3dTexImage2D*	g_bkgStarting = NULL;
	r3dTexImage2D*	g_bkgExpired = NULL;
	r3dTexImage2D*	g_imPB[3] = {0};	// progress bar images
	r3dTexImage2D*	g_imBtnPg[3] = {0};
	r3dTexImage2D*	g_imBtnClose = {0};
	r3dTexImage2D*	g_imBtnLogin = {0};
	r3dTexImage2D*	g_imBtnTopMenu[8] = {0};
	r3dTexImage2D*	g_imBtnRegister = {0};
	r3dTexImage2D*	g_imBtnRegisterNew = {0};
	r3dTexImage2D*	g_imBtnValidate = {0};
	r3dTexImage2D*	g_imBtnGetSerial = {0};
	r3dTexImage2D*	g_imBtnGetNewKey = {0};
	r3dTexImage2D*	g_imBtnApplyNewKey = {0};
	
	CD3DFont*	g_font1   = NULL;
	CD3DFont*	g_font2   = NULL;
	CD3DFont*	g_font3   = NULL;
	CD3DFont*	g_font4   = NULL;
	bool		g_bStartGame = false;
	
	int		g_mx = 0;
	int		g_my = 0;
	int		g_mb = 0;

	HCURSOR		g_hCursor = NULL;
	HCURSOR		gCursorArrow;
	HCURSOR		gCursorHand;
	
	wchar_t		g_RegionLockCIS[521] = L"";


// some crap to make eternity link
DWORD DriverUpdater(HWND hParentWnd, DWORD VendorId, DWORD v1, DWORD v2, DWORD v3, DWORD v4, DWORD hash) {return hash;}
r3dMesh *	r3dGOBAddMesh(const char* fname, bool addToLibrary=true, bool use_default_material=false, bool allow_async = false, bool player_mesh = false, bool use_thumbnails = false ) { return NULL; };
void writeGameOptionsFile() {}
void r3dScaleformBeginFrame() {}
void r3dScaleformEndFrame() {}
void SetNewSimpleFogParams() {}
void SetVolumeFogParams() {}

class r3dScreenBuffer * DepthBuffer;
class r3dCamera gCam;
class r3dSun * Sun;
#include "../EclipseStudio/Sources/GameLevel.h"
#include "../EclipseStudio/Sources/GameLevel.cpp"
r3dLightSystem WorldLightSystem;
void r3dAtmosphere::Reset() {}
bool g_bEditMode = false;

struct r3dRECT {
  float left, top, width, height;
};

static bool insideRect(const r3dRECT& r, int x, int y)
{
  return x >= r.left && x <= (r.left+r.width) && y >= r.top && y <= (r.top+r.height);
}

static bool checkIfAlreadyRunning()
{
  static const char* updaterName = "Global\\WarZ_Updater_001";
  static const char* gameName = "Global\\WarZ_Game_001";
  
  HANDLE h;
  if((h = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, gameName)) != NULL) {
    MessageBox(NULL, "Game is already running", "Warning", MB_OK | MB_ICONEXCLAMATION);
    CloseHandle(h);
    return true;
  }

  if((h = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, updaterName)) != NULL) {
    OutputDebugStringA("Updater is already running");
    SetEvent(h);
    CloseHandle(h);
    return true;
  }
  
  // create named event to signalize that updater is already running.
  // handle will be automatically closed on program termination
  g_updaterEvt = CreateEvent(NULL, FALSE, FALSE, updaterName);
  return false;
}

static bool testForCurrentDir()
{
  char exeDir[MAX_PATH];
  char curDir[MAX_PATH];

  char exeMain[MAX_PATH];
  GetModuleFileName(NULL, exeMain, sizeof(exeMain));
  
  char* fname;
  ::GetFullPathName(exeMain, sizeof(exeDir), exeDir, &fname);
  if(fname) *(fname - 1) = 0; // remove '\' at the end
  
  ::GetCurrentDirectory(sizeof(curDir), curDir);
  if(stricmp(exeDir, curDir) != 0) {
    char msg[2048];
    sprintf(msg, "Updater current directory (%s) is different from working directory (%s)", curDir, exeDir);
    MessageBox(NULL, msg, "Warning", MB_OK | MB_ICONEXCLAMATION);
  }
  //::SetCurrentDirectory(g_workDir);
  
  return true;
}

static bool testForWriteToDir()
{
  char buf[MAX_PATH];
  sprintf(buf, "%s", "test.bin");
  FILE* f  = fopen(buf, "wb");
  if(f == NULL) {
    return false;
  }
  fclose(f);
  _unlink(buf);
  
  return true;
}

static bool detectTerminalSession()
{
  if(strstr(__r3dCmdLine, "-cmdLine") != NULL)
    return true;
  
/*    
  int rs = GetSystemMetrics(SM_REMOTESESSION);
  if(rs > 0)
    return true;
*/    
    
  return false;
}

void game::PreInit(void)  
{ 
  // set icon
  win::hWinIcon = ::LoadIcon(win::hInstance, MAKEINTRESOURCE(IDI_WARINC));

  #ifndef _DEBUG
  // do not close on debug terminate key
  _r3d_bTerminateOnZ = 0;
  #endif

  if(strstr(__r3dCmdLine, "-generate") != NULL) {
    GenerateUpdaterXML();
    TerminateProcess(GetCurrentProcess(), 0);
    return;
  }
  
  if(checkIfAlreadyRunning()) {
    TerminateProcess(GetCurrentProcess(), 0);
    return;
  }

  // see if we running from correct directory (just in case)
  testForCurrentDir();

  if(!testForWriteToDir()) {
    MessageBox(NULL, "Updater must be run from administrator", "UAC problem", MB_OK | MB_ICONINFORMATION);
    TerminateProcess(GetCurrentProcess(), 0);
    return;
  }
  
  selfUpd_TestIfUpdated();
  
  // win7 taskbar
  CoInitialize(NULL);
  if(FAILED(CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_ALL, r3dIID_ITaskbarList3, (void**)&g_taskbar))) {
    g_taskbar = NULL;
  }

  if(detectTerminalSession())
  {
    g_isConsoleUpdater = true;
  }

  gCursorArrow = LoadCursor(NULL, IDC_ARROW);
  gCursorHand  = LoadCursor(NULL, IDC_HAND);
  
  LoadStringW(NULL, IDS_STRING_REGIONLOCK_CIS, g_RegionLockCIS, sizeof(g_RegionLockCIS) / sizeof(g_RegionLockCIS[0]));
  
  return;
}

static int fake_SetMode(int XRes, int YRes)
{
  r3dRenderLayer& r = *r3dRenderer;

  HRESULT res;
  int DepthMode = D3DFMT_D24S8;
  r.DeviceType = D3DDEVTYPE_HAL;
  r.CurrentBPP = 32;

  D3DDISPLAYMODE mode;
  // get windowed display mode
  r.pd3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);
  mode.Width  = XRes;
  mode.Height = YRes;
  r3d_assert(mode.Width && mode.Height);

  // adjust our window size
  RECT rc;
  SetRect(&rc, 0, 0, mode.Width, mode.Height);        
  AdjustWindowRect(&rc, GetWindowLong(r.HLibWin, GWL_STYLE), false);
  SetWindowPos(r.HLibWin, HWND_TOP, 0, 0, (rc.right - rc.left), (rc.bottom - rc.top), SWP_NOMOVE);

  extern float __r3dGlobalAspect;
  __r3dGlobalAspect = float(mode.Width) / float(mode.Height);

  // Set up the structure used to create the D3DDevice. Since we are now
  D3DPRESENT_PARAMETERS d3dpp;
  ZeroMemory(&d3dpp, sizeof(d3dpp) );
  d3dpp.Windowed               = TRUE;
  d3dpp.hDeviceWindow          = r.HLibWin;
  d3dpp.BackBufferWidth        = mode.Width;
  d3dpp.BackBufferHeight       = mode.Height;
  d3dpp.BackBufferFormat       = D3DFMT_A8R8G8B8;
  d3dpp.BackBufferCount        = 1;
  d3dpp.MultiSampleType        = D3DMULTISAMPLE_NONE;
  d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;//D3DSWAPEFFECT_FLIP ;
  d3dpp.Flags                  = 0 ;//D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
  d3dpp.EnableAutoDepthStencil = TRUE;
  d3dpp.AutoDepthStencilFormat = (D3DFORMAT)DepthMode;
  r.d3dpp = d3dpp;
  
  IDirect3DDevice9 *mDev = NULL;
  res = r.pd3d->CreateDevice(
    D3DADAPTER_DEFAULT,
    r.DeviceType,
    r.HLibWin,
    D3DCREATE_HARDWARE_VERTEXPROCESSING,
    &r.d3dpp,
    &mDev);
  if(FAILED(res)) 
  {
    MessageBox(NULL, "Our apologies, but you need video card with hardware vertex processing to run this game.", "Apologies", MB_OK);
    TerminateProcess(GetCurrentProcess(), 0);
  }
  r.SetD3DDev(mDev);

  if(r.Reset() == false)
  {
    TerminateProcess(GetCurrentProcess(), 0);
  }
  
  r.ResetStats();

  r3dIntegrityGuardian ig ;
  _r3dSystemFont = new CD3DFont(ig, "Verdana", 10, D3DFONT_BOLD | D3DFONT_FILTERED | D3DFONT_SKIPGLYPH);
  _r3dSystemFont->CreateSystemFont();
  
  extern int r3d_CD3DFont_Quality;
  r3d_CD3DFont_Quality = 5; // set to CLEARTYPE_QUALITY

  g_font1 = new CD3DFont(ig, "System", 12, D3DFONT_FILTERED | D3DFONT_SKIPGLYPH);
  g_font1->CreateSystemFont();
  g_font2 = new CD3DFont(ig, "System", 10, D3DFONT_FILTERED | D3DFONT_SKIPGLYPH);
  g_font2->CreateSystemFont();
  g_font3 = new CD3DFont(ig, "System",  8, D3DFONT_FILTERED | D3DFONT_SKIPGLYPH);
  g_font3->CreateSystemFont();
  g_font4 = new CD3DFont(ig, "System", 11, D3DFONT_FILTERED | D3DFONT_SKIPGLYPH);
  g_font4->CreateSystemFont();

  r3dMaterialLibrary::Reset();
  
  // enable alpha blending for texture
  r3dRenderer->pd3ddev->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
  r3dRenderer->pd3ddev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
  r3dRenderer->pd3ddev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

  return TRUE;
}

static void CreateResources()
{
  g_bkgLogin    = new r3dTexImage2D(IDR_IMAGE_BACK_LOGIN, 785, 650);
  g_bkgUpdate   = new r3dTexImage2D(IDR_IMAGE_BACK_UPDATE, 785, 650);
  g_bkgRegister = new r3dTexImage2D(IDR_IMAGE_BACK_REGISTER, 785, 650);
  g_bkgSerialCheck = new r3dTexImage2D(IDR_IMAGE_BACK_SERIALCHECK, 785, 650);
  g_bkgStarting = new r3dTexImage2D(IDR_IMAGE_BACK_STARTING, 785, 650);
  g_bkgExpired = new r3dTexImage2D(IDR_IMAGE_BACK_EXPIRED, 785, 650);
  g_imPB[0] = new r3dTexImage2D(IDR_IMAGE_PB_LEFT, 6, 8);
  g_imPB[1] = new r3dTexImage2D(IDR_IMAGE_PB_CENTER, 4, 8);
  g_imPB[2] = new r3dTexImage2D(IDR_IMAGE_PB_RIGHT, 6, 8);
  g_imBtnClose = new r3dTexImage2D(IDR_IMAGE_BTN_CLOSE_ON, 32, 32);
  g_imBtnLogin = new r3dTexImage2D(IDR_IMAGE_BTN_LOGIN_HOVER, 455, 72);
  g_imBtnRegister = new r3dTexImage2D(IDR_IMAGE_BTN_REGISTER_HOVER, 276, 90);
  g_imBtnRegisterNew = new r3dTexImage2D(IDR_IMAGE_BTN_REGISTER_NEW_HOVER, 455, 72);
  g_imBtnValidate = new r3dTexImage2D(IDR_IMAGE_BTN_VALIDATE_HOVER, 456, 77);
  g_imBtnGetSerial = new r3dTexImage2D(IDR_IMAGE_BTN_GETSERIAL_HOVER, 456, 77);
  g_imBtnGetNewKey = new r3dTexImage2D(IDR_IMAGE_BTN_GETNEWKEY_HOVER, 455, 72);
  g_imBtnApplyNewKey = new r3dTexImage2D(IDR_IMAGE_BTN_APPLYNEWKEY_HOVER, 455, 72);
  g_imBtnTopMenu[0] = new r3dTexImage2D(IDR_IMAGE_BTN_MYACC_HOVER, 130, 52);
  g_imBtnTopMenu[1] = new r3dTexImage2D(IDR_IMAGE_BTN_FORUMS_HOVER, 89, 52);
  g_imBtnTopMenu[2] = new r3dTexImage2D(IDR_IMAGE_BTN_SUPPORT_HOVER, 99, 52);
  g_imBtnTopMenu[3] = new r3dTexImage2D(IDR_IMAGE_BTN_YOUTUBE_HOVER, 45, 52);
  g_imBtnTopMenu[4] = new r3dTexImage2D(IDR_IMAGE_BTN_FACEBOOK_HOVER, 48, 52);
  g_imBtnTopMenu[5] = new r3dTexImage2D(IDR_IMAGE_BTN_TWITTER_HOVER, 48, 52);
  
  g_imBtnPg[0] = new r3dTexImage2D(IDR_IMAGE_BTN_PG_ON,    276, 127);
  g_imBtnPg[1] = new r3dTexImage2D(IDR_IMAGE_BTN_PG_HOVER, 276, 127);

  // sync steam_api.dll from resource
  gSteam.SyncDllFromResource();

  return;
}

static bool dragActive = 0;
static POINT dragSavedPnt = {0};
static LRESULT CALLBACK updApp_WndFunc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch(uMsg) 
  {
    case WM_CLOSE:
      ClipCursor(NULL);
      ReleaseCapture();
      g_bExit = true;
      return 0;
    
    case WM_LBUTTONDOWN:
    {
      int mx = LOWORD(lParam);
      int my = HIWORD(lParam);
      if(my < 20 && mx < 747) {
        dragActive = true;
        GetCursorPos(&dragSavedPnt);

        RECT wndRect;
        GetWindowRect(hWnd, &wndRect);
        dragSavedPnt.x -= wndRect.left;
        dragSavedPnt.y -= wndRect.top;

        SetCapture(hWnd);
        break;
      }

      break;
    }

    case WM_LBUTTONUP:
      if(dragActive) {
        dragActive = false;
        ReleaseCapture();
        break;
      }

      g_mb |= 1;
      break;

    case WM_MOUSEMOVE:
      g_mx = LOWORD(lParam);
      g_my = HIWORD(lParam);
      
      if(dragActive) {
        POINT pnt;
        GetCursorPos(&pnt);

        SetWindowPos(hWnd, NULL, pnt.x - dragSavedPnt.x, pnt.y - dragSavedPnt.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        break;
      }
      break;
      
    case WM_CHAR:
      win32_OnWMChar(wParam);
      break;
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


void game::Init(void)
{
  if(g_isConsoleUpdater)
  {
    cmdLine_Init();
    return;
  }
  
  gHwInfoPoster.hw.Grab();

  // set small icon
  ::SendMessage(win::hWnd, WM_SETICON, FALSE, (LPARAM)::LoadIcon(win::hInstance, MAKEINTRESOURCE(IDI_WARINC)));
  
  // make borderless window and subclass wndProc
  r3dWinStyleModify(win::hWnd, 0, WS_BORDER);
  r3dWinStyleModify(win::hWnd, 0, WS_CAPTION);
  SetWindowLong(win::hWnd, GWL_WNDPROC, (DWORD)&updApp_WndFunc);

  // ok, now r3drendered started using vars inside itself...
  RegisterAllVars();

  const int Width  = 785;
  const int Height = 650;

  HDC disp_dc  = CreateIC("DISPLAY", NULL, NULL, NULL);
  int curDispWidth  = GetDeviceCaps(disp_dc, HORZRES);
  int curDispHeight = GetDeviceCaps(disp_dc, VERTRES);
  int StartXPos = (curDispWidth - Width) / 2;
  int StartYPos = (curDispHeight - Height) / 2;
  SetWindowPos(win::hWnd, NULL, StartXPos, StartYPos, (int)Width, (int)Height, 0);

  r3dRenderer = new r3dRenderLayer;
  r3dRenderer->Init(win::hWnd, NULL);
  
  fake_SetMode(Width, Height);
  
  char title[512];
  sprintf(title, "Kongsi Infestation Launcher %s (%s)", UPDATER_VERSION, UPDATER_BUILD);
  if(!UPDATER_UPDATER_ENABLED) strcat(title, "!!!!SELF_UPDATE_DISABLED!!!");
  ::SetWindowText(win::hWnd, title);

  CreateResources();
  
  ShowWindow(win::hWnd, SW_SHOW);
  
  ClipCursor(NULL);
  
  // release dinput classes becase we won't be using them
  Keyboard->ReleaseCapture();
  Mouse->ReleaseCapture();
}

void game::Shutdown(void)
{
}

void tempDoMsgLoop()
{
  MSG msg;
  while(PeekMessage(&msg, NULL,0,0,PM_NOREMOVE)) {
    if(!GetMessage (&msg, NULL, 0, 0)) 
      return;

    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return;
}

static SIZE GetTextExtent(CD3DFont* fnt, const char* text)
{
  RECT r;
  r.left   = 0;
  r.top    = 0;
  r.right  = 99999;
  r.bottom = 99999;

  fnt->GetD3DFont()->DrawTextA(
    NULL,
    text,
    strlen(text), 
    &r,
    DT_CALCRECT,
    0xFFFFFFFF);
    
  SIZE sz = {r.right  - r.left, r.bottom - r.top};
  return sz;
}

// YAY! STL WONDERS!
#include <algorithm>  
#include <functional>  
#include <cctype> 
#include <locale> 
// trim from start 
static inline std::string &ltrim(std::string &s) { 
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace)))); 
        return s; 
} 
// trim from end 
static inline std::string &rtrim(std::string &s) { 
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end()); 
        return s; 
} 
// trim from both ends 
static inline std::string &trim(std::string &s) { 
        return ltrim(rtrim(s)); 
} 


static int editTextField(float x, float y, char* editString, bool isFocused, int editType, unsigned int maxLength = 32)
{
  r3dColor clr(0, 0, 0);

  x += 7; // max request - need minor indention
  
  char stars[512];
  for(size_t i=0; i<strlen(editString); i++)
    stars[i] = '*';
  stars[strlen(editString)] = 0;
     
  char* msg = (editType == 1) ? stars : editString;
  g_font1->PrintF(x, y + ((editType == 1) ? 4 : 0), clr, "%s", msg); // '*' in font is way up, adjust it to center
  if(!isFocused)
    return 0;
    
  // blinking 
  static float blinkRate = (float)GetCaretBlinkTime() / 1000.0f;
  static float nextBlink = r3dGetTime();
  static int   caretOn   = 0;
  
  if(r3dGetTime() > nextBlink) {
    nextBlink += blinkRate;
    caretOn = !caretOn;
  }

  // by some weird reason GetTextExtent ignoring last spaces in string.
  char sizemsg[512];
  sprintf(sizemsg, "%s.", msg);
  SIZE ts1 = GetTextExtent(g_font1, sizemsg);
  SIZE ts2 = GetTextExtent(g_font1, ".");
  if(caretOn) {
    r3dDrawBox2D(x + (float)(ts1.cx - ts2.cx), y+2, 1, 16, r3dColor(0, 0, 0));
  }

  // some wierd Control-V logic, as we can't detect it - it's eated inside WMKEYDOWN
  static bool btnVPressed = false;
  if(::GetFocus() == win::hWnd && (GetKeyState('V') & 0x80) && !btnVPressed) {
    btnVPressed = true;
    if(GetKeyState(VK_CONTROL) & 0x80) {
      if(OpenClipboard(NULL)) 
      {
        HANDLE clip = GetClipboardData(CF_TEXT);
        const char* ptr = (LPSTR)GlobalLock(clip);
        if(ptr)
        {
          std::string text = ptr;
          text = trim(text);
          r3dscpy_s(editString, maxLength, text.c_str());
        }
        GlobalUnlock(clip);
        CloseClipboard();
      }
    }
  }
  else btnVPressed = false;
  
  // do input
  int ch = win32_getch();
  if(ch == 0)
    return 0;
    
  // backspace
  if(ch == 8 && *editString != 0)
    editString[strlen(editString)-1] = 0;

  // check for max length
  if(strlen(editString) >= maxLength)
    return 0;
    
  if(editType == 2)
  {
    // special case for serial entry
    int len = strlen(editString);
    if(len >= 19) // max length
      return 0;
    if(len == 4 || len == 9 || len == 14) { // '-' in serial
      if(ch == '-') {
        sprintf(editString + strlen(editString), "%c", ch);
      }
      return 0;
    }
    if(ch >= '0' && ch <= '9')
      sprintf(editString + strlen(editString), "%c", ch);
    else if(ch >= 'a' && ch <= 'z')
      sprintf(editString + strlen(editString), "%c", ch - 0x20);
    else if(ch >= 'A' && ch <= 'Z')
      sprintf(editString + strlen(editString), "%c", ch);
    else if(ch == 9 || ch == 13)
      return ch;

    return 0;
  }
    
  // normal key
  if(ch >= 0x20 && ch < 0x80) {
    sprintf(editString + strlen(editString), "%c", ch);
  }
  
  return ch;
}



static void drawProgressBar(CUpdater& updater)
{
  float base_y = 617.0f;
  
  // updater info & version
  g_font1->PrintF(20, base_y-26, r3dColor(226, 226, 226), "%s", updater.updMsg1_);
  g_font3->PrintF( 9, base_y+15, r3dColor(170, 170, 170), "Kongsi Infestation Launcher v%s%s", UPDATER_VERSION,UPDATER_VERSION_SUFFIX);
  
  if(!updater.showProgress_)
    return;

  r3dColor pbclr = r3dColor(255, 255, 255);
  float y    = base_y;
  float x    = 25;
  float w    = 735;
  float perc = updater.prgTotal_.getCoef();
  float c    = (w * perc);	// length of center bar
  
  g_imPB[1]->draw(x, y, c, 9, pbclr);

/*
  float mbLeft = ((float)updater.prgTotal_.total - updater.prgTotal_.cur) / 1024.0f / 1024.0f;
  if(mbLeft > 0.01f) {
     g_font1->PrintF(423, 224, r3dColor(153, 208, 237), "%.0f mb left\n", mbLeft);
  }
*/  
  
  if(g_taskbar) g_taskbar->SetProgressValue(win::hWnd, updater.prgTotal_.cur, updater.prgTotal_.total);
}

static bool drawUrl(float x, float y, const char* msg)
{
    CD3DFont* fnt = g_font4;
    // news text
    SIZE ts = GetTextExtent(fnt, msg);
    r3dRECT r = {x, y, (float)ts.cx, (float)ts.cy};
    r3dColor clr = r3dColor(0x8a, 0x4a, 0x4a);
    
    // selection and underline
    if(insideRect(r, g_mx, g_my)) {
      clr = r3dColor(0xc3, 0x9f, 0x9f);
      r3dDrawBox2D(x, y + (float)ts.cy, (float)ts.cx, 1, clr);
      if(g_mb) {
        return true;
      }
    }

    fnt->PrintF(x, y, clr, "%s", msg);
    return false;
}

static void drawCloseButton()
{
  static r3dRECT g_rCloseBtn = {759, 6, 20, 16};

  // close button
  r3dColor clr(255, 255, 255);
  if(insideRect(g_rCloseBtn, g_mx, g_my)) {
    g_imBtnClose->draw(753, 0, 32, 32, clr);
    if(g_mb) {
      g_bExit = true;
    }
  }
}

static void filterAccountName(char* acc)
{
  char filtered[128];
  char* out = filtered;

  const char* in = acc;
  
  // trim left
  while(*in == ' ') in++;
  
  for(; *in; in++) 
  {
    if(*in == ' ' || 
      (*in >= 'a' && *in <= 'z') ||
      (*in >= 'A' && *in <= 'Z') ||
      (*in >= '0' && *in <= '9')) {
      *out++ = *in;
    }
  }
  *out = 0;
  
  r3dscpy(acc, filtered);
}

static void drawRegisterButtons(CUpdater& updater)
{
  static r3dRECT g_rRegisterBtn  = {254, 531, 276, 90};
  static r3dRECT g_rEditFields[3] = {
    {182, 351, 424, 24},
    {182, 411, 424, 24},
    {182, 472, 424, 24},
  };
  // email, passwd1, passwd2, serial
  static char gEditText[3][512] = {0};
  static int gInputLen[3] = {64, 32, 32};

  r3dColor clr(255, 255, 255);

  // if login in process, draw only needed things
  bool testDraw = (GetAsyncKeyState(VK_F1) & 0x8000) && false;
  if(testDraw || updater.createAccHelper.createAccCode == CCreateAccHelper::CA_Processing) 
  {
    for(int i=0; i<3; i++) {
      r3dDrawBox2D(g_rEditFields[i].left-2, g_rEditFields[i].top-2, g_rEditFields[i].width+4, g_rEditFields[i].height+4, r3dColor(0, 0, 0, 100));
      editTextField(g_rEditFields[i].left, g_rEditFields[i].top, gEditText[i], false, i>=1 ? 1 : 0);
    }
    return;
  }
  
  // register button. enabled if we have all data
  if(gEditText[0][0] && gEditText[1][0] && gEditText[2][0] &&
     insideRect(g_rRegisterBtn, g_mx, g_my)) 
  {
    g_imBtnRegister->draw(g_rRegisterBtn.left, g_rRegisterBtn.top, g_rRegisterBtn.width, g_rRegisterBtn.height, clr);
    if(g_mb) 
    {
      strcpy(updater.createAccHelper.username, gEditText[0]);
      strcpy(updater.createAccHelper.passwd1,  gEditText[1]);
      strcpy(updater.createAccHelper.passwd2,  gEditText[2]);
      strcpy(updater.createAccHelper.serial,   updater.checkSerialHelper.serial);
      updater.DoCreateAccount();
    }
  }
  
  // input fields logic
  static int focusId = 0;
  
  for(int i=0; i<3; i++) 
  {
    if(g_mb && insideRect(g_rEditFields[i], g_mx, g_my)) {
      focusId = i;
      win32_input_Flush();
    }
    
    int key;
    key = editTextField(
      g_rEditFields[i].left, 
      g_rEditFields[i].top, 
      gEditText[i], 
      focusId == i, 
      (i == 1 || i == 2) ? 1 : 0,
      gInputLen[i]);
    if(key == 9)  focusId = (GetKeyState(VK_SHIFT)&0x8000) ? focusId-1 : focusId+1;
    if(key == 13 && gEditText[i][0]) focusId++;
    
    if(focusId >= 3) focusId = 0;
    if(focusId <  0) focusId = 2;
  }
  
  return;
}

static void drawTimeExpiredButtons(CUpdater& updater)
{
  static r3dRECT g_rGetNewKeyBtn   = {165, 343, 455, 72};
  static r3dRECT g_rApplyNewKeyBtn = {165, 550, 455, 72};
  static r3dRECT g_rEditFields[1] = {
    {182, 497, 424, 24},
  };
  // email, passwd1, passwd2, serial
  static char gEditText[1][512] = {0};
  static int gInputLen[1] = {64};

  r3dColor clr(255, 255, 255);

  // if login in process, draw only needed things
  bool testDraw = (GetAsyncKeyState(VK_F1) & 0x8000) && false;
  if(testDraw || updater.createAccHelper.createAccCode == CCreateAccHelper::CA_Processing) 
  {
    for(int i=0; i<1; i++) {
      r3dDrawBox2D(g_rEditFields[i].left-2, g_rEditFields[i].top-2, g_rEditFields[i].width+4, g_rEditFields[i].height+4, r3dColor(0, 0, 0, 100));
      editTextField(g_rEditFields[i].left, g_rEditFields[i].top, gEditText[i], false, i>=1 ? 1 : 0);
    }
    return;
  }
  
  // apply new key button. enabled if we have all data
  if(gEditText[0][0] && insideRect(g_rApplyNewKeyBtn, g_mx, g_my)) 
  {
    g_imBtnApplyNewKey->draw(g_rApplyNewKeyBtn.left, g_rApplyNewKeyBtn.top, g_rApplyNewKeyBtn.width, g_rApplyNewKeyBtn.height, clr);
    if(g_mb) 
    {
      updater.DoApplyNewKey(gEditText[0]);
      return;
    }
  }

  // get new serial button
  if(insideRect(g_rGetNewKeyBtn, g_mx, g_my)) 
  {
    g_imBtnGetNewKey->draw(g_rGetNewKeyBtn.left, g_rGetNewKeyBtn.top, g_rGetNewKeyBtn.width, g_rGetNewKeyBtn.height, clr);
    if(g_mb) 
    {
      ShellExecute(NULL, "open", "http://warz.kongsi.asia/", "", NULL, SW_SHOW);
    }
  }
  
  // input fields logic
  static int focusId = 0;
  
  for(int i=0; i<1; i++) 
  {
    if(g_mb && insideRect(g_rEditFields[i], g_mx, g_my)) {
      focusId = i;
      win32_input_Flush();
    }
    
    int key;
    key = editTextField(
      g_rEditFields[i].left, 
      g_rEditFields[i].top, 
      gEditText[i], 
      focusId == i, 
      0,
      gInputLen[i]);
  }
  
  return;
}

static void drawSerialCheckButtons(CUpdater& updater)
{
  static r3dRECT g_rValidateBtn   = {164, 402, 456, 77};
  static r3dRECT g_rGetSerialBtn  = {164, 546, 456, 77};
  static r3dRECT g_rEditFields[2] = {
    {182, 349, 424, 24},
  };
  // email, serial
  static char gEditText[1][512] = {0};
  static int gInputLen[1] = {48};
  
  r3dColor clr(255, 255, 255);
  
  // if serial check in process, draw only needed things
  bool testDraw = (GetAsyncKeyState(VK_F1) & 0x8000) && false;
  if(testDraw || updater.checkSerialHelper.processCode == CCheckSerialHelper::CA_Processing) 
  {
    for(int i=0; i<2; i++) {
      r3dDrawBox2D(g_rEditFields[i].left-2, g_rEditFields[i].top-2, g_rEditFields[i].width+4, g_rEditFields[i].height+4, r3dColor(0, 0, 0, 100));
      editTextField(g_rEditFields[i].left, g_rEditFields[i].top, gEditText[i], false, 0);
    }
    return;
  }
  
  // register button. enabled if we have all data
  if(gEditText[0][0] && insideRect(g_rValidateBtn, g_mx, g_my)) 
  {
    g_imBtnValidate->draw(g_rValidateBtn.left, g_rValidateBtn.top, g_rValidateBtn.width, g_rValidateBtn.height, clr);
    if(g_mb) 
    {
      strcpy(updater.checkSerialHelper.serial, gEditText[0]);
      updater.DoCheckSerial();
    }
  }
  
  // some temp warning
  {
    r3dColor clr(0x8a, 0x4a, 0x4a);
    g_font4->PrintF(317, 326, clr, "You can paste (Control-V) your serial");
    //g_font1->PrintF(180, 374, clr, "Make sure not to confuse O and 0 (zero), I and 1 (one)");
  }
  
  // get new serial button
  if(insideRect(g_rGetSerialBtn, g_mx, g_my)) 
  {
    g_imBtnGetSerial->draw(g_rGetSerialBtn.left, g_rGetSerialBtn.top, g_rGetSerialBtn.width, g_rGetSerialBtn.height, clr);
    if(g_mb) 
    {
      ShellExecute(NULL, "open", "http://warz.kongsi.asia/", "", NULL, SW_SHOW);
    }
  }
  
  // input fields logic
  static int focusId = 0;
  
  for(int i=0; i<1; i++) 
  {
    if(g_mb && insideRect(g_rEditFields[i], g_mx, g_my)) {
      focusId = i;
      win32_input_Flush();
    }
    
    int key;
    key = editTextField(
      g_rEditFields[i].left, 
      g_rEditFields[i].top, 
      gEditText[i], 
      focusId == i, 
      0,
      gInputLen[i]);
      
    //if(key == 9)  focusId = (GetKeyState(VK_SHIFT)&0x8000) ? focusId-1 : focusId+1;
    //if(key == 13 && gEditText[i][0]) focusId++;
    //if(focusId >= 2) focusId = 0;
    //if(focusId <  0) focusId = 1;
  }
  
  strupr(gEditText[1]);

  return;
}

static void drawLoginButtons(CUpdater& updater)
{
  static r3dRECT g_rLoginBtn  = {271, 483, 244, 51};
  static r3dRECT g_rRegisterNewBtn = {271, 550, 244, 72};
  static r3dRECT g_rLoginEdit = {181, 357, 424, 24};
  static r3dRECT g_rPwdEdit   = {181, 417, 424, 24};
  r3dColor clr(255, 255, 255);
  
  // if login or update in process, draw only needed things
  bool testDraw = (GetAsyncKeyState(VK_F1) & 0x8000) && false;
  if(testDraw || updater.gUserProfile.loginAnswerCode == CLoginHelper::ANS_Processing 
     || updater.gUserProfile.loginAnswerCode == CLoginHelper::ANS_Logged)
  {
    r3dDrawBox2D(g_rLoginEdit.left-2, g_rLoginEdit.top-2, g_rLoginEdit.width+4, g_rLoginEdit.height+4, r3dColor(0, 0, 0, 100));
    r3dDrawBox2D(g_rPwdEdit.left-2, g_rPwdEdit.top-2, g_rPwdEdit.width+4, g_rPwdEdit.height+4, r3dColor(0, 0, 0, 100));
    editTextField(g_rLoginEdit.left, g_rLoginEdit.top, updater.gUserProfile.username, false, 0);
    editTextField(g_rPwdEdit.left, g_rPwdEdit.top, updater.gUserProfile.passwd, false, 1);
    return;
  }
  
  // login button. enabled if we have all data
  if(*updater.gUserProfile.username && *updater.gUserProfile.passwd && insideRect(g_rLoginBtn, g_mx, g_my)) {
    g_imBtnLogin->draw(165, 473, 455, 72, clr);
    if(g_mb) {
      updater.DoLogin();
    }
  }

  if(drawUrl(492, 445, "Forgot Password?")) {
    ShellExecute(NULL, "open", "https://warz.kongsi.asia/account_check/", "", NULL, SW_SHOW);
  }

  // register new account button
  if(insideRect(g_rRegisterNewBtn, g_mx, g_my)) 
  {
    g_imBtnRegisterNew->draw(165, 550, 455, 72, clr);
    if(g_mb) {
      //ShellExecute(NULL, "open", "http://127.0.0.1/", "", NULL, SW_SHOW);
      //return;
		updater.status_ = CUpdater::STATUS_NeedRegister;
    }
  }
  
  // login error
  if(updater.loginErrMsg_[0]) {
    static float bx = 230;
    static float by = 290;
    r3dDrawBox2D(bx, by, 331, 25, r3dColor(0, 0, 0, 150));
    g_font1->DrawText(bx, by, 331, 25, r3dColor(255, 26, 26), updater.loginErrMsg_, D3DFONT_CENTERED);
  }
  
  // name/pwd logic
  static int focusId = updater.gUserProfile.username[0] ? 1 : 0;
  
  if(g_mb && insideRect(g_rLoginEdit, g_mx, g_my)) {
    focusId = 0;
    win32_input_Flush();
  }
  if(g_mb && insideRect(g_rPwdEdit, g_mx, g_my)) {
    focusId = 1;
    win32_input_Flush();
  }

  int key;
  key = editTextField(g_rLoginEdit.left, g_rLoginEdit.top, updater.gUserProfile.username, focusId == 0, 0, 64);
  if(key == 9)  focusId = 1;
  if(key == 13 && updater.gUserProfile.username[0]) focusId = 1;

  key = editTextField(g_rPwdEdit.left, g_rPwdEdit.top, updater.gUserProfile.passwd, focusId == 1, 1);
  if(key == 9)  focusId = 0;
  if(key == 13 && updater.gUserProfile.passwd[0] && updater.gUserProfile.username[0])
    updater.DoLogin();
    
  return;
}

static void executeTopMenuButton(const CUpdater& updater, int btnIdx)
{
  char token[512];
  updater.gUserProfile.CreateAuthToken(token);

  switch(btnIdx)
  {
    case 0: // my account
    {
      char url[1024];
      sprintf(url, "http://warz.kongsi.asia/"); // /?WzLogin=%s", token);
      ShellExecute(NULL, "open", url, "", NULL, SW_SHOW);
      break;
    }
    
    case 1: // forums
    {
      ShellExecute(NULL, "open", "http://warz.kongsi.asia/", "", NULL, SW_SHOW);
      break;
    }

    case 2: // support
    {
      ShellExecute(NULL, "open", "http://warz.kongsi.asia/", "", NULL, SW_SHOW);
      break;
    }
    
    case 3: // youtube
    {
      ShellExecute(NULL, "open", "http://warz.kongsi.asia/", "", NULL, SW_SHOW);
      break;
    }
    
    case 4: // facebook
    {
      ShellExecute(NULL, "open", "http://warz.kongsi.asia/", "", NULL, SW_SHOW);
      break;
    }
    
    case 5: // twitter
    {
      ShellExecute(NULL, "open", "http://warz.kongsi.asia/", "", NULL, SW_SHOW);
      break;
    }
  }

}

static void drawUpdateButtons(CUpdater& updater)
{
  static r3dRECT g_rPlayBtn  = {506, 521, 244, 61};
  static r3dRECT g_rTopBtn[6] = {
    {139, 5, 130, 52},
    {270, 5,  89, 52},
    {362, 5,  99, 52},
    {509, 5,  45, 52},
    {555, 5,  48, 52},
    {608, 5,  48, 52}
  };
    

  r3dColor clr = r3dColor(255, 255, 255);

  // play button. active only when updater finished updating.
  // and servers logically offline
  if(updater.result_ == CUpdater::RES_PLAY && updater.IsServerOnline())
  {
    if(insideRect(g_rPlayBtn, g_mx, g_my)) {
      g_imBtnPg[1]->draw(489, 484, 276, 127, clr);
    } else {
      g_imBtnPg[0]->draw(489, 484, 276, 127, clr);
    }

    if(insideRect(g_rPlayBtn, g_mx, g_my) && g_mb) 
    {
      if(updater.CheckForNewData())
      {
        MessageBox(NULL, "There is new build available, please log in again", "Update required", MB_OK | MB_ICONEXCLAMATION);
        g_bExit = true;
        return;
      }
      g_bStartGame = true;
    }
  }
  
  // top menu buttons
  for(int btnIdx=0; btnIdx<6; btnIdx++)
  {
    if(insideRect(g_rTopBtn[btnIdx], g_mx, g_my)) 
    {
      g_imBtnTopMenu[btnIdx]->draw(
        g_rTopBtn[btnIdx].left, 
        g_rTopBtn[btnIdx].top,
        g_rTopBtn[btnIdx].width,
        g_rTopBtn[btnIdx].height,
        clr);
        
      if(g_mb) {
        executeTopMenuButton(updater, btnIdx);
      }
    }
  }
  
  return;
}

static void drawNewsSection(float x, float y, const std::vector<CUpdater::news_s>& news, size_t maxLines)
{
  float o = 624; // date text offset

  for(size_t i=0; i<news.size() && i < maxLines; i++) 
  {
    // news date
    g_font2->PrintF(x+o, y, r3dColor(243, 182, 86), news[i].date_.c_str());

    // news text
	const char* newsText = news[i].name_.c_str();
    SIZE ts = GetTextExtent(g_font2, newsText);
    r3dRECT r = {x, y, (float)ts.cx, (float)ts.cy};
    r3dColor clr = r3dColor(255, 255, 255);
    
    // selection and underline
    if(insideRect(r, g_mx, g_my)) {
      clr = r3dColor(255, 216, 0);
      r3dDrawBox2D(x, y + (float)ts.cy, (float)ts.cx, 1, clr);
      if(g_mb) {
        ShellExecute(NULL, "open", news[i].url_.c_str(), "", NULL, SW_SHOW);
      }
    }

    g_font2->PrintF(x, y, clr, "%s", newsText);
    y += 20;
  }
  
  //TODO: "View All"

  return;  
}

/*
static void drawAnnouncements(const std::vector<CUpdater::news_s>& news, size_t maxLines)
{
  float y  = 117;
  int   x1 = 465;
  int   x2 = 759;

  DWORD flags = DT_RIGHT | DT_TOP | DT_WORDBREAK;
  
  ID3DXFont* d3dfont = g_font2->GetD3DFont(); 
  for(size_t i=0; i<news.size() && i < maxLines; i++) 
  {
    // announcement title (in date_ field)
    RECT r1 = {x1, (int)y, x2, (int)y+30};
    
    d3dfont->DrawTextA(
      NULL, //fnt_sprite,
      news[i].date_.c_str(),
      news[i].date_.length(), 
      &r1,
      flags,
      r3dColor(191, 227, 255).GetPacked());

    // announcement text
    y += 20;
    RECT r2 = {x1, (int)y, x2, (int)y+50};

    d3dfont->DrawTextA(
      NULL, //fnt_sprite,
      news[i].name_.c_str(),
      news[i].name_.length(),
      &r2,
      flags,
      r3dColor(161, 161, 161).GetPacked());

    y += 50;
    if(drawUrl(718, y, "more...")) {
      ShellExecute(NULL, "open", news[i].url_.c_str(), "", NULL, SW_SHOW);
    }
    
    y += 35;
  }

}
*/

static void drawNews(CUpdater& updater)
{
  if(updater.newsStatus_ != 2)
    return;

  r3dCSHolder cs1(updater.csNews_);

  // check for survey
  static bool need_to_show_survey = true;
  if(!updater.surveyLinkIn_.empty() && need_to_show_survey) {
    need_to_show_survey = false;
    ShellExecute(NULL, "open", updater.surveyLinkIn_.c_str(), "", NULL, SW_SHOW);
  }

  drawNewsSection(43, 366, updater.newsData_, 5);

  return;
}

static void drawServerStatus(const CUpdater& updater)
{
  float y = 591;

  if(updater.newsStatus_ == 0) {
    g_font1->PrintF(595, y, r3dColor(214, 214, 214), "Retrieving Server Status");
    return;
  }

  if(updater.newsStatus_ == 1 || updater.serverStatus_.size() == 0) {
    g_font1->PrintF(607, y, r3dColor(255, 31, 31), "Server Status Unavailable");
    return;
  }
  
  g_font1->PrintF(610, y, r3dColor(153, 208, 237), "Server Status:");
  const char* status = updater.serverStatus_.c_str();
  if(stricmp(status, "ONLINE") == 0)
    g_font1->PrintF(709, y, r3dColor(31, 255, 57), "ONLINE");
  else if(stricmp(status, "OFFLINE") == 0)
    g_font1->PrintF(709, y, r3dColor(255, 31, 31), "OFFLINE");
  else
    g_font1->PrintF(709, y, r3dColor(255, 127, 39), status);
    
  return;
}

static void showErrorMessageBox(const CUpdater& updater)
{
  char buf[2048];
  sprintf(buf, "There was a problem updating the game\n'%s'\n\nPlease retry later", updater.updErr1_);
  MessageBox(win::hWnd, updater.updErr1_, "Error", MB_OK | MB_ICONERROR);

  return;
}

static bool createTokenRegKey(const char* token)
{
	// store key in registry
	HKEY hKey;
	int hr;
	hr = RegCreateKeyEx(HKEY_CURRENT_USER, 
		"Software\\Arktos Entertainment Group\\WarZ", 
		0, 
		NULL,
		REG_OPTION_NON_VOLATILE, 
		KEY_ALL_ACCESS,
		NULL,
		&hKey,
		NULL);
	if(hr != ERROR_SUCCESS)
		return false;
		
	hr = RegSetValueEx(hKey, "LoginToken", NULL, REG_SZ, (BYTE*)token, strlen(token) + 1);
	if(hr != ERROR_SUCCESS)
		return false;

	RegCloseKey(hKey);
	return true;
}

static void startGame(const CUpdater& updater)
{
/*
  const char* UPDATE_FINISH_MSG = \
    "The game has been successfully updated to the latest version.\n"\
    "Do you want to start the game now?";
    
  if(updater.numUpdatedFiles_) {
    if(IDYES != MessageBox(win::hWnd, UPDATE_FINISH_MSG, GAME_TITLE, MB_YESNO | MB_ICONQUESTION))
      return;
  }
*/

  // show EULA if we have updated something
  if(updater.numUpdatedFiles_) {
    if(eulaShowDialog(0) != IDOK) {
      return;
    }
    if(eulaShowDialog(1) != IDOK) {
      return;
    }
  }

  r3dOutToLog("Starting game\n"); CLOG_INDENT;
  
  /* Because ShellExecute can delegate execution to Shell extensions 
     (data sources, context menu handlers, verb implementations) that are 
     activated using Component Object Model (COM), COM should be initialized 
     before ShellExecute is called. 
  */
  CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
  
  extern bool r3dGetFileCrc32(const char* fname, DWORD* out_crc32, DWORD* out_size);
  DWORD crc32, size;
  if(!r3dGetFileCrc32(GAME_EXE_NAME, &crc32, &size)) {
    r3dOutToLog("failed1\n");
  } else {
    r3dOutToLog("%08x %d\n", crc32, size);
  }
  
  // pass special string to exe so it will know we updated successfully...
  char token[1024];
  updater.gUserProfile.CreateAuthToken(token);
  
  // store login token in registry. If successful, indicate it in command line (==) and make some random gibberish
  if(createTokenRegKey(token))
    sprintf(token, "==%08X%08X", u_random(-1), u_random(-1));

  char GAME_START_PARAM[2048];
  sprintf(GAME_START_PARAM, "-cmdWrz -WOUpdatedOk %s -WOLogin \"%s\"", __r3dCmdLine, token);
  if(updater.surveyLinkOut_.length() > 0) {
    sprintf(GAME_START_PARAM + strlen(GAME_START_PARAM), " -survey \"%s\"", updater.surveyLinkOut_.c_str());
  }
  if(gSteam.steamID > 0) {
    sprintf(GAME_START_PARAM + strlen(GAME_START_PARAM), " -steam");
  }
  
  // we don't need to elevate permission here
  int err = (int)ShellExecute(NULL, "open", GAME_EXE_NAME, GAME_START_PARAM, NULL, SW_SHOW);
  if(err < 32) {
    r3dOutToLog("failed: %d\n", err);
  }
  
  return;
}

PCHAR* CommandLineToArgvA(PCHAR CmdLine, int* _argc)
{
	PCHAR* argv;
	PCHAR  _argv;
	ULONG   len;
	ULONG   argc;
	CHAR   a;
	ULONG   i, j;

	BOOLEAN  in_QM;
	BOOLEAN  in_TEXT;
	BOOLEAN  in_SPACE;

	len = strlen(CmdLine);
	i = ((len+2)/2)*sizeof(PVOID) + sizeof(PVOID);

	argv = (PCHAR*)GlobalAlloc(GMEM_FIXED,
		i + (len+2)*sizeof(CHAR));

	_argv = (PCHAR)(((PUCHAR)argv)+i);

	argc = 0;
	argv[argc] = _argv;
	in_QM = FALSE;
	in_TEXT = FALSE;
	in_SPACE = TRUE;
	i = 0;
	j = 0;

	while( a = CmdLine[i] ) {
		if(in_QM) {
			if(a == '\"') {
				in_QM = FALSE;
			} else {
				_argv[j] = a;
				j++;
			}
		} else {
			switch(a) {
		case '\"':
			in_QM = TRUE;
			in_TEXT = TRUE;
			if(in_SPACE) {
				argv[argc] = _argv+j;
				argc++;
			}
			in_SPACE = FALSE;
			break;
		case ' ':
		case '\t':
		case '\n':
		case '\r':
			if(in_TEXT) {
				_argv[j] = '\0';
				j++;
			}
			in_TEXT = FALSE;
			in_SPACE = TRUE;
			break;
		default:
			in_TEXT = TRUE;
			if(in_SPACE) {
				argv[argc] = _argv+j;
				argc++;
			}
			_argv[j] = a;
			j++;
			in_SPACE = FALSE;
			break;
			}
		}
		i++;
	}
	_argv[j] = '\0';
	argv[argc] = NULL;

	(*_argc) = argc;
	return argv;
}

void game::MainLoop(void)
{
  r3dOutToLog("Starting updater, v:%s, cmd:%s\n", UPDATER_VERSION, __r3dCmdLine);

  CUpdater updater;
  
  // parse command line
  int argc = 0;
  char** argv = CommandLineToArgvA(__r3dCmdLine, &argc);
  for(int i=0; i<argc; i++) 
  {
    if(strcmp(argv[i], "-steam") == 0 && (i + 0) < argc)
    {
      r3dOutToLog("Trying to init steam\n");
      gSteam.InitSteam();
      continue;
    }
  }

  if(g_isConsoleUpdater)
  {
    cmdLine_MainLoop(updater);
    return;
  }
  
  updater.Start();

  win32_input_Flush();
  
  if(g_taskbar) g_taskbar->SetProgressState(win::hWnd, TBPF_NORMAL);
  
  r3dStartFrame();
  while(1)
  {
    r3dEndFrame();
    r3dStartFrame();

    g_mb = 0;
    g_hCursor = gCursorArrow;
    tempDoMsgLoop();
    ::Sleep(1);
    
    r3dRenderer->StartRender();
    r3dRenderer->StartFrame();
    r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ);
    r3dSetFiltering(R3D_POINT);
    
    r3dDrawBox2D(0, 0, r3dRenderer->ScreenW, r3dRenderer->ScreenH, r3dColor(64, 64, 64));
    
    #if 0
    updater.status_ = CUpdater::STATUS_TimeExpired;
    updater.showProgress_ = false;
    updater.prgTotal_.cur = 100;
    updater.prgTotal_.total = 100;
    r3dscpy(updater.updMsg1_, "Xxxxxxx");
    #endif

    switch(updater.status_) 
    {
      default: r3d_assert(0);
      
      case CUpdater::STATUS_Checking:
	g_bkgStarting->draw(0, 0, r3dRenderer->ScreenW, r3dRenderer->ScreenH, r3dColor(255, 255, 255));
	break;

      case CUpdater::STATUS_NeedLogin:
	g_bkgLogin->draw(0, 0, r3dRenderer->ScreenW, r3dRenderer->ScreenH, r3dColor(255, 255, 255));
	
	drawLoginButtons(updater);
	break;

      case CUpdater::STATUS_SerialCheck:
	g_bkgSerialCheck->draw(0, 0, r3dRenderer->ScreenW, r3dRenderer->ScreenH, r3dColor(255, 255, 255));

	drawSerialCheckButtons(updater);
	break;
    
      case CUpdater::STATUS_NeedRegister:
	g_bkgRegister->draw(0, 0, r3dRenderer->ScreenW, r3dRenderer->ScreenH, r3dColor(255, 255, 255));

	drawRegisterButtons(updater);
	break;
	
      case CUpdater::STATUS_TimeExpired:
	g_bkgExpired->draw(0, 0, r3dRenderer->ScreenW, r3dRenderer->ScreenH, r3dColor(255, 255, 255));

	drawTimeExpiredButtons(updater);
	break;

      case CUpdater::STATUS_Updating:
	g_bkgUpdate->draw(0, 0, r3dRenderer->ScreenW, r3dRenderer->ScreenH, r3dColor(255, 255, 255));

	drawUpdateButtons(updater);
        drawServerStatus(updater);
	drawNews(updater);
	break;
    }
    drawProgressBar(updater);
    drawCloseButton();

    r3dRenderer->EndFrame();
    r3dRenderer->EndRender(true);

    // if exit requested by control-f4
    if(g_bExit) {
      updater.RequestStop();
      break;
    }
    
    // activate updater window if other instance requested it
    if(WaitForSingleObject(g_updaterEvt, 0) == WAIT_OBJECT_0) {
      SwitchToThisWindow(win::hWnd, TRUE);
    }

    // try to remove mouse lock because of 
    // win::HandleActivate code that will lock mouse inside window
    ClipCursor(NULL);

    // if signalled to play game
    if(g_bStartGame)
      break;

    if(updater.result_ != CUpdater::RES_UPDATING && updater.result_ != CUpdater::RES_PLAY)
      break;
      
    // wndclass.hCursor must be NULL for this to work
    //SetCursor(g_hCursor);
  }
  
  updater.Stop();

  if(g_taskbar) g_taskbar->SetProgressState(win::hWnd, TBPF_NOPROGRESS);
  
  switch(updater.result_)
  {
    default: r3d_assert(0); 
    case CUpdater::RES_STOPPED:
      break;

    case CUpdater::RES_PLAY:
      startGame(updater);
      break;
      
    case CUpdater::RES_ERROR:
      showErrorMessageBox(updater);
      break;
  }
  
  gHwInfoPoster.Stop();
  TerminateProcess(GetCurrentProcess(), 0);
  
  return;
}
