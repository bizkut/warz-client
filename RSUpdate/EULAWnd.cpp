#include "r3dPCH.h"
#include "r3d.h"
#include <commctrl.h>
#include <Richedit.h>

#include "EULAWnd.h"
#include "resource.h"
#include "HttpDownload.h"
#include "UPDATER_CONFIG.h"

namespace
{
	CkByteData	eulaData;
	LONG		eulaReaded = 0;
	UINT_PTR	eulaTimer = 0;
	
	int		whatToShow = 0;
};

static DWORD CALLBACK EditStreamCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
  if(cb + eulaReaded > (LONG)eulaData.getSize())
    cb = eulaData.getSize() - eulaReaded;
  
  if(cb == 0) {
    *pcb = 0;
    return 0;
  }
  
  memcpy(pbBuff, eulaData.getData() + eulaReaded, cb);
  eulaReaded += cb;
  *pcb = cb;
  return 0;
}

static void setEulaData(HWND hwnd)
{
  int type = SF_RTF;

  eulaReaded = 0;
  
  HttpDownload http;
  if(!http.Get(whatToShow ? TOS_URL : EULA_URL, eulaData)) {
    static const char* failed = "Failed to get data";
    eulaData.borrowData((BYTE*)failed, strlen(failed));
    type = SF_TEXT;
  }

  // stream data to RTF control
  EDITSTREAM es;
  es.dwCookie    = 0; 
  es.dwError     = 0; 
  es.pfnCallback = EditStreamCallback; 
  SendMessage(hwnd, EM_EXLIMITTEXT, 0, (LPARAM)0x80000); // 512K buffer
  SendMessage(hwnd, EM_STREAMIN, type, (LPARAM)&es);
  SendMessage(hwnd, EM_SETMODIFY, FALSE, 0L);
  
  return;
}


static INT_PTR CALLBACK EulaDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  static HBRUSH g_hbrBackground = CreateSolidBrush(RGB(255, 255, 255));

  switch(uMsg)
  {
    case WM_CLOSE:
      EndDialog(hwndDlg, IDCANCEL);
      return TRUE;
      
    case WM_COMMAND:
      if(wParam == IDOK || wParam == IDCANCEL) {
        EndDialog(hwndDlg, wParam);
        return TRUE;
      }
      break;
    
    case WM_CTLCOLORDLG:
      return (LONG)g_hbrBackground;
      
    case WM_ACTIVATE:
    {
      if(wParam == WA_ACTIVE) {
        // create timer that will fire after 0.5 sec
        eulaTimer = SetTimer(hwndDlg, 0x11112222, 500, NULL);
      }
      
      break;
    }
    
    // on timer, download and display EULA
    case WM_TIMER:
      if(eulaTimer == NULL)
        break;
        
      KillTimer(hwndDlg, eulaTimer);
      eulaTimer = NULL;

      setEulaData(GetDlgItem(hwndDlg, IDC_RICHEDIT_EULA));
      EnableWindow(GetDlgItem(hwndDlg, IDOK), TRUE);
      SetFocus(GetDlgItem(hwndDlg, IDCANCEL));
      break;
  }

  return FALSE;
}

int eulaShowDialog(int mode)
{
  whatToShow = mode;
  
  // this calls needed for rich text control inside dialog
  LoadLibraryA("RICHED32.DLL");
  LoadLibraryA("RICHED20.DLL");

  int r = ::DialogBox(win::hInstance, MAKEINTRESOURCE(IDD_DIALOG_EULA), win::hWnd, &EulaDialogProc);
  if(r == 0 || r == -1) {
    MessageBox(NULL, "Failed to display", "Error", MB_OK | MB_ICONEXCLAMATION);
    r = IDOK;
  }

  return r;
}