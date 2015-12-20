// Log.cpp : implementation file
//

#include "stdafx.h"
#include "Log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

	CLog*		CLog::gLog;
	static int	BUTTON_HEIGHT = 20;

/////////////////////////////////////////////////////////////////////////////
// CLog

CLog::CLog()
{
  m_disableClose = 0;
  m_hadCRLF      = 1;
}

CLog::~CLog()
{
}


BEGIN_MESSAGE_MAP(CLog, CWnd)
	//{{AFX_MSG_MAP(CLog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLog message handlers

CLog* CLog::CreateWnd()
{
  CLog* wnd      = new CLog;

  wnd->parentWnd = AfxGetMainWnd();

  wnd->CreateEx(
    0,
    AfxRegisterWndClass(0, 0, (HBRUSH)(COLOR_WINDOW+1), 0),
    "Exporter Log", 
    WS_CAPTION | WS_THICKFRAME | WS_SYSMENU | WS_BORDER | WS_VISIBLE, // WS_OVERLAPPEDWINDOW
    CRect(100, 100, 700, 600),
    wnd->parentWnd, 
    NULL,
    NULL);

  if(wnd->parentWnd)
    wnd->parentWnd->EnableWindow(FALSE);

  HICON hIcon = ::LoadIcon(NULL, IDI_INFORMATION);
  wnd->SetIcon(hIcon, 0);

  wnd->ShowWindow(SW_SHOW);
  wnd->BringWindowToTop();
  wnd->SetActiveWindow();
  wnd->SetFocus();

  gLog = wnd;

  return wnd;
}

CRect CLog::btn1_Rect()
{
  CRect rect;
  this->GetClientRect(&rect);
  rect.top     = rect.bottom - BUTTON_HEIGHT;
  rect.top    += 1;
  rect.left   += 1;
  rect.right  -= 1;
  rect.bottom -= 1;

  return rect;
}

CRect CLog::ed_Rect()
{
  CRect rect;
  this->GetClientRect(&rect);
  rect.bottom -= BUTTON_HEIGHT + 3;
  rect.top    += 1;
  rect.left   += 1;
  rect.right  -= 1;
  rect.bottom -= 1;

  return rect;
}

int CLog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CWnd::OnCreate(lpCreateStruct) == -1)
    return -1;

  //
  // init default vars
  //

  mCurColor    = 0;
  mTextLen     = 0;
  mIndentLevel = 0;
  mOkState     = 0;

  fnt1.CreatePointFont(100, "Arial");
  ed.Create(
    WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
    ed_Rect(),
    this,
    0);
  //SetWindowLong(ed, GWL_EXSTYLE, WS_EX_STATICEDGE);

  // initialize control
  ed.SetSel(-1, -1);
  ed.ReplaceSel("", FALSE);
  ed.SetFont(&fnt1);

  //
  // Create the OK button
  //

  btn1.Create(
    "Close", 
    WS_VISIBLE, 
    btn1_Rect(),
    this, 
    CLOG_BUTTON_ID);
  btn1.SetFont(&fnt1);

  btn1.EnableWindow(FALSE);

  return 0;
}

void CLog::Log(const char* msg, ...)
{
  CHARFORMAT cf;
  ed.GetDefaultCharFormat(cf);

  char buf[1024], *pbuf = buf;

  // indent only if we finished our line.
  if(m_hadCRLF) {
    for(int i=0; i<mIndentLevel; i++) {
      *pbuf++ = ' ';
      *pbuf++ = ' ';
      *pbuf++ = ' ';
    }
    m_hadCRLF = 0;
  }

  DWORD clr = mCurColor;

  while(*msg == '!') {
    switch(*++msg) {
    // effect code
      case 'B':	clr |= Bold;			break;
      case 'I':	clr |= Italic;			break;
      case 'U':	clr |= Underline;		break;
      // color code
      case 'r': clr &= 0xFFFF0000; clr += Red;	break;
      case 'g': clr &= 0xFFFF0000; clr += Green;break;
      case 'b': clr &= 0xFFFF0000; clr += Blue;	break;
      case 'd': clr &= 0xFFFF0000; clr += Debug;break;
    }
    msg++;
  }

  va_list ap;
  va_start(ap, msg);
  vsprintf(pbuf, msg, ap);
  va_end(ap);

  if(strchr(pbuf, '\n') != 0)
    m_hadCRLF = 1;

  if(clr) {
    cf.dwMask      = 0;
    cf.dwEffects   = 0;
    if(clr & Bold)	{ cf.dwEffects |= CFE_BOLD;      cf.dwMask |= CFM_BOLD;      }
    if(clr & Italic)	{ cf.dwEffects |= CFE_ITALIC;    cf.dwMask |= CFM_ITALIC;    }
    if(clr & Underline)	{ cf.dwEffects |= CFE_UNDERLINE; cf.dwMask |= CFM_UNDERLINE; }

    if(clr & 0xFFFF)	cf.dwMask    |= CFM_COLOR;
    switch(clr & 0xFFFF) 
    {
      case Red:		cf.crTextColor= RGB(255,   0,   0); break;
      case Green:	cf.crTextColor= RGB(  0, 255,   0); break;
      case Blue:	cf.crTextColor= RGB(  0,   0, 255); break;
      case Debug:	cf.crTextColor= RGB(  0, 129,  70); break;
    }

    ed.SetSelectionCharFormat(cf);
  }

  ed.SetSel(mTextLen, mTextLen);
  ed.ReplaceSel(buf, FALSE);

  long n1, n2;
  ed.GetSel(n1, n2);
  mTextLen = n2;

  ed.SendMessage(WM_VSCROLL, SB_BOTTOM, (LPARAM)NULL);

  RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE );
}

void CLog::OnSize(UINT nType, int cx, int cy) 
{
  CWnd::OnSize(nType, cx, cy);

  CRect rect;
  rect = ed_Rect();
  ed.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
  rect = btn1_Rect();
  btn1.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
}

BOOL CLog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  switch(wParam) 
  {
    case CLOG_BUTTON_ID:
      if(mOkState) mOkState--;
      if(mOkState == 1) btn1.SetWindowText("Close");
      break;
  }	

  return CWnd::OnCommand(wParam, lParam);
}

void CLog::WaitForOk()
{
  SetFocus();
  btn1.EnableWindow(TRUE);
  mOkState = 1;

  DWORD	curtime = GetTickCount();
  DWORD	endtime	= curtime + 3000;
  MSG msg;
  while(mOkState)
  {
    while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
      if(!GetMessage (&msg, NULL, 0, 0)) {
        mOkState = 0;
        break;
      }
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    if(mOkState == 2) {
      if(GetTickCount() > curtime) {
        char buf[512];
        sprintf(buf, "Closing in %.1f sec", (float)(endtime - curtime) / 1000.0f);
        btn1.SetWindowText(buf);
        InvalidateRect(btn1_Rect(), FALSE);
        curtime += 500;
      }
      if(GetTickCount() > endtime)
        break;
    }

    // we will break when mOkState will be 0
  }

  return;
}

void CLog::OnClose() 
{
  if(m_disableClose) 
    return;

  mOkState = 0;

  if(parentWnd) {
    parentWnd->EnableWindow(TRUE);
    parentWnd->SetActiveWindow();
    parentWnd->SetFocus();
  }

  CWnd::OnClose();
}
