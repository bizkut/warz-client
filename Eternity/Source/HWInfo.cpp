#include "r3dPCH.h"
#include "r3d.h"
#include <intrin.h>
#include <IPTypes.h>
#include <iphlpapi.h>
#pragma comment(lib, "Iphlpapi.lib")

#include "HWInfo.h"

CHWInfo::CHWInfo()
{
  memset(this, 0, sizeof(*this));
}

void CHWInfo::Grab()
{
  GetMACAddress();
  GetCPUInfo();
  GetCPUFreq();
  GetMemoryInfo();
  GetDesktopResolution();
  GetD3DInfo();
  GetD3DVersion();
  GetOSInfo();
}

void CHWInfo::GetMACAddress()
{
  IP_ADAPTER_INFO AdapterInfo[16];
  DWORD dwBufLen = sizeof(AdapterInfo);
  DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
  if(dwStatus != ERROR_SUCCESS)
    return;

  memcpy(&macAddress, AdapterInfo[0].Address, sizeof(macAddress));
  
  // make unique id from mac address and crypt it
  memcpy(&uniqueId, AdapterInfo[0].Address, sizeof(uniqueId));
  ((DWORD*)&uniqueId)[0] ^= 0x41FB4481;
  ((DWORD*)&uniqueId)[1] ^= 0x0000834F;
}


void CHWInfo::GetCPUInfo()
{
  int CPUInfo[4] = {-1};

  // __cpuid with an InfoType argument of 0 returns the number of
  // valid Ids in CPUInfo[0] and the CPU identification string
  __cpuid(CPUInfo, 0);
  memset(CPUString, 0, sizeof(CPUString));
  *((DWORD*)CPUString)     = CPUInfo[1];
  *((DWORD*)(CPUString+4)) = CPUInfo[3];
  *((DWORD*)(CPUString+8)) = CPUInfo[2];

  // get CPU feature information.
  __cpuid(CPUFeatures, 1);

  // Calling __cpuid with 0x80000000 as the InfoType argument
  // gets the number of valid extended IDs.
  memset(CPUBrandString, 0, sizeof(CPUBrandString));

  __cpuid(CPUInfo, 0x80000000);
  int nExIds = CPUInfo[0];

  // Get the information associated with each extended ID.
  for(int i=0x80000000; i<=nExIds; ++i)
  {
    __cpuid(CPUInfo, i);

    if(i == 0x80000002)
      memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
    else if  (i == 0x80000003)
      memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
    else if  (i == 0x80000004)
      memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
  }
  
  // check if brandsting is invalid
  if(strlen(CPUBrandString) >= sizeof(CPUBrandString))
    CPUBrandString[0] = 0;

  return;
}

void CHWInfo::GetCPUFreq()
{
  // open the key where the proc speed is stored
  HKEY hKey;
  if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
                  0, KEY_READ, &hKey) != ERROR_SUCCESS)
  {
    return;
  }
    
  // query the key
  DWORD size = sizeof(CPUFreq);
  RegQueryValueEx(hKey, "~MHz", NULL, NULL, (LPBYTE)&CPUFreq, &size);
}

void CHWInfo::GetMemoryInfo()
{
  MEMORYSTATUSEX stat;
  stat.dwLength = sizeof(stat);
  GlobalMemoryStatusEx(&stat);
  
  TotalMemory = (DWORD)(stat.ullTotalPhys / 1024);
  return;
}

void CHWInfo::GetDesktopResolution()
{
  HDC disp_dc  = CreateIC("DISPLAY", NULL, NULL, NULL);
  DisplayW     = GetDeviceCaps(disp_dc, HORZRES);
  DisplayH     = GetDeviceCaps(disp_dc, VERTRES);
  DeleteDC(disp_dc);
}

#include "d3d11.h"
void CHWInfo::GetD3DVersion()
{
  gfxD3DVersion = 900;

  // dynamically load the D3D11 DLL
  // This may fail if Direct3D 11 isn't installed
  HMODULE hD3D11 = LoadLibrary("d3d11.dll");
  if(hD3D11 == NULL) {
    return;
  }
    
  PFN_D3D11_CREATE_DEVICE fnD3D11CreateDevice = (PFN_D3D11_CREATE_DEVICE)GetProcAddress(hD3D11, "D3D11CreateDevice");
  if(fnD3D11CreateDevice == NULL) {
    return;
  }

  ID3D11Device*        device   = NULL;
  ID3D11DeviceContext* context  = NULL;
  D3D_FEATURE_LEVEL    level;

  r3dOutToLog("Checking for D3D11\n");
  HRESULT hr = fnD3D11CreateDevice(NULL, 
	D3D_DRIVER_TYPE_HARDWARE, 
	NULL,
	0, 
	NULL, 
	0, 
	D3D11_SDK_VERSION, 
	&device, 
	&level, 
	&context);
  if(hr != S_OK) {
    return;
  }
  
  SAFE_RELEASE(context);
  SAFE_RELEASE(device);

  switch(level)
  {
    case D3D_FEATURE_LEVEL_9_1:  gfxD3DVersion =  901; break;
    case D3D_FEATURE_LEVEL_9_2:  gfxD3DVersion =  902; break;
    case D3D_FEATURE_LEVEL_9_3:  gfxD3DVersion =  903; break;
    case D3D_FEATURE_LEVEL_10_0: gfxD3DVersion = 1000; break;
    case D3D_FEATURE_LEVEL_10_1: gfxD3DVersion = 1001; break;
    case D3D_FEATURE_LEVEL_11_0: gfxD3DVersion = 1100; break;
  }
}

void CHWInfo::GetD3DInfo()
{
  IDirect3D9* pd3d = Direct3DCreate9(D3D_SDK_VERSION);
  if(pd3d == NULL) {
    gfxErrors |= (1<<0);
    return;
  }

  D3DADAPTER_IDENTIFIER9 ident;
  if(D3D_OK == pd3d->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &ident))
  {
    gfxDeviceId = ident.DeviceId;
    gfxVendorId = ident.VendorId;
    r3dscpy(gfxDescription, ident.Description);
  }
  
  D3DCAPS9 caps;
  if(D3D_OK == pd3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps))
  {
    CheckIfCapsOk(caps);
  }
  else
  {
    gfxErrors |= (1<<1);
  }

  // just in case, try to actually create device with parameters from game
  D3DPRESENT_PARAMETERS d3dpp;
  ZeroMemory(&d3dpp, sizeof(d3dpp));
  d3dpp.Windowed               = TRUE;
  d3dpp.hDeviceWindow          = win::hWnd;
  d3dpp.BackBufferWidth        = DisplayW;
  d3dpp.BackBufferHeight       = DisplayH;
  d3dpp.BackBufferFormat       = D3DFMT_X8R8G8B8;
  d3dpp.BackBufferCount        = 1;
  d3dpp.MultiSampleType        = D3DMULTISAMPLE_NONE;
  d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
  d3dpp.Flags                  = 0;
  d3dpp.EnableAutoDepthStencil = TRUE;
  d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
  
  IDirect3DDevice9* pd3ddev = NULL;
  HRESULT hr = pd3d->CreateDevice(
    D3DADAPTER_DEFAULT,
    D3DDEVTYPE_HAL,
    win::hWnd,
    D3DCREATE_HARDWARE_VERTEXPROCESSING,
    &d3dpp,
    &pd3ddev);
  if(FAILED(hr))
    gfxErrors |= (1<<5);

  SAFE_RELEASE(pd3ddev);
  SAFE_RELEASE(pd3d);
}

void CHWInfo::CheckIfCapsOk(const D3DCAPS9& caps)
{
  // need SM3.0
  DWORD NEEDED_PSVER = D3DPS_VERSION(3, 0);
  DWORD NEEDED_VSVER = D3DVS_VERSION(3, 0);
  if(caps.PixelShaderVersion < NEEDED_PSVER)
    gfxErrors |= (1<<2);
  
  if(caps.VertexShaderVersion < NEEDED_VSVER)
    gfxErrors |= (1<<3);
    
  // HW vertexprocessing
  if((caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0)
    gfxErrors |= (1<<4);
    
  return;
}

void CHWInfo::GetOSInfo()
{
  OSVERSIONINFO osvi = {0};
  osvi.dwOSVersionInfoSize = sizeof(osvi);
  GetVersionEx(&osvi);
  
  sprintf(OSVersion, "%d.%d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber);
}