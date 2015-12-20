// (Lib) assetCompilerPlugin_Euphoria.cpp : Defines the entry point for the DLL application.
//

#ifdef _MANAGED
  #pragma managed(push, off)
#endif

#include <Windows.h>

#include "NMPlatform/NMMemory.h"

BOOL APIENTRY DllMain(HMODULE, DWORD  dwReason, LPVOID)
{
  switch (dwReason)
  {
  case DLL_PROCESS_ATTACH:
    break;

  case DLL_PROCESS_DETACH:
    break;
  }

  return TRUE;
}

#ifdef _MANAGED
  #pragma managed(pop)
#endif

