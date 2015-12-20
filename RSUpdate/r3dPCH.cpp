// stdafx.cpp : source file that includes just the standard includes
// RSUpdate.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "r3dPCH.h"

/*
void r3dOutToLog(const char* Str, ...)
{
  // buffer with indented spaces
  char buf[4096];

  va_list ap;
  va_start(ap, Str);
  StringCbVPrintfA(buf, sizeof(buf), Str, ap);
  va_end(ap);

  OutputDebugString(buf);
}

const char* r3dError(const char* fmt, ...)
{
  static char buf[4096];

  va_list ap;
  va_start(ap, fmt);
  StringCbVPrintfA(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  
  r3dOutToLog("ERROR: %s\n", buf);
  r3dOutToLog("... exiting\n");

  if(IsDebuggerPresent()) {  
    __asm int 3;
  }
  else
	  MessageBox(0, buf, "Error!", MB_OK);
  
  throw buf;
}
*/
