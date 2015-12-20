// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMSeh.h"

// Disable warnings about deprecated functions (sprintf, fopen)
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning (disable : 4996)
#endif

#if defined(NM_COMPILER_LLVM) || defined(NM_COMPILER_GCC) || defined(NM_COMPILER_SNC) || defined(NM_COMPILER_GHS)
  #define _vsnprintf vsnprintf
#endif

// If NM_DEBUG_OUTPUT is not defined in NMHost.h, we default to printf here.
// Some platforms do not support flushing printf to the debug output, so they may need custom output fns.
#ifndef NM_DEBUG_OUTPUT
  #define NM_DEBUG_OUTPUT(str) printf("%s", str)
#endif

//----------------------------------------------------------------------------------------------------------------------
// Output a formatted message to stdout.
void NMMessagef(
  const char* file, // If non-zero, the name of the file in which the message was called.
  int32_t     line, // The line number of the call.
  const char* fmt,  // If non-zero, a printf-style formatted explanation of the assertion.
  ...)
{
  static char printBuf[1024];
  va_list args;
  va_start(args, fmt);

  if (file != 0)
  {
    sprintf(printBuf, "%s(%i) : ", file, line);
    NM_DEBUG_OUTPUT(printBuf);
  }

  if (fmt)
  {
    _vsnprintf(printBuf, sizeof(printBuf) - 1, fmt, args);
    NM_DEBUG_OUTPUT(printBuf);
  }

  va_end(args);
  NM_DEBUG_OUTPUT("\n");
}

#if NM_ENABLE_EXCEPTIONS

//----------------------------------------------------------------------------------------------------------------------
bool NMP::Exception::sm_alreadyDealingWithAnException = false;

void NMThrowError(const char* file, int line, const char* fmt, ...)
{
  static char printBuf[1024];
  va_list args;
  va_start(args, fmt);

  if (fmt)
  {
    _vsnprintf(printBuf, sizeof(printBuf) - 1, fmt, args);
  }
  else
  {
    printBuf[0] = '\0';
  }

  va_end(args);

  // make sure we don't throw an exception while we're already excepting
  // This can happen if people have put NMP_VERIFY() in destructors.
  if (!NMP::Exception::alreadyDealingWithAnException())
  {
    throw NMP::Exception(printBuf, file, line);
  }
  else
  {
    NMP_MSG("Following exception thrown whilst handling an exception");
    NMP_MSG(printBuf);
    NM_BREAK();
  }

}

//----------------------------------------------------------------------------------------------------------------------
#if NM_CALL_STACK_DEBUG

#include <ImageHlp.h>
#pragma comment(lib, "imagehlp.lib")

void NMP::SetSETranslator::win32ExceptionHandler(unsigned int, EXCEPTION_POINTERS* pExp)
{
  // Stack trace
  static char traceBuffer[1024 * NM_CALL_STACK_MAX_FRAMES];
  memset(traceBuffer, 0, sizeof(traceBuffer));
  sprintf(traceBuffer, "\nWIN32 EXCEPTION (code %08x) CALL STACK:\n\n", pExp->ExceptionRecord->ExceptionCode);

  HANDLE process = GetCurrentProcess();
  HANDLE thread  = GetCurrentThread();

  DWORD Options = SymGetOptions(); 
  Options |= SYMOPT_DEBUG; 
  Options |= SYMOPT_LOAD_LINES;
  ::SymSetOptions(Options); 

  // Initialize and load symbols for all modules of the current process 
  BOOL bRet = ::SymInitialize (process, NULL, TRUE); 
  if( !bRet ) 
  {
    NMP_STDOUT("Error: SymInitialize() failed. Error code: %u \n", ::GetLastError());
    return; 
  }

  CONTEXT context = *pExp->ContextRecord;
  DWORD machineType = IMAGE_FILE_MACHINE_I386;
  STACKFRAME64 stackframe;

  memset(&stackframe, 0, sizeof(STACKFRAME64));
  stackframe.AddrPC.Mode      = AddrModeFlat;
  stackframe.AddrStack.Mode   = AddrModeFlat;
  stackframe.AddrFrame.Mode   = AddrModeFlat;

#if defined(_M_IX86)
  stackframe.AddrPC.Offset    = context.Eip;
  stackframe.AddrStack.Offset = context.Esp;
  stackframe.AddrFrame.Offset = context.Ebp;
  machineType = IMAGE_FILE_MACHINE_I386;
#elif defined(_M_AMD64)
  stackframe.AddrPC.Offset    = context.Rip;
  stackframe.AddrStack.Offset = context.Rsp;
  stackframe.AddrFrame.Offset = context.Rbp;
  machineType = IMAGE_FILE_MACHINE_AMD64;
#elif defined(_M_IA64)
  stackframe.AddrPC.Offset     = context.StIIP;
  stackframe.AddrBStore.Offset = context.RsBSP;
  stackframe.AddrStack.Offset  = context.IntSp;
  machineType = IMAGE_FILE_MACHINE_IA64;
#endif

  char symbolBuffer[256];
  memset(symbolBuffer, 0, sizeof(symbolBuffer));
  PIMAGEHLP_SYMBOL64 symbol = (PIMAGEHLP_SYMBOL64)symbolBuffer;
  symbol->SizeOfStruct  = sizeof(IMAGEHLP_SYMBOL64);
  symbol->MaxNameLength = sizeof(symbolBuffer) - symbol->SizeOfStruct;

  IMAGEHLP_LINE64 line;
  line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

  DWORD frame = 0;
  DWORD frameOut = 0;
  while (StackWalk64(
    machineType,
    process, 
    thread, 
    &stackframe,
    &context,
    NULL,
    SymFunctionTableAccess64,
    SymGetModuleBase64,
    NULL))
  {
    char frameBuffer[1024];

    DWORD64 disp64 = 0;
    BOOL haveSymbol = SymGetSymFromAddr64(process, stackframe.AddrPC.Offset, &disp64, symbol);

    DWORD disp32 = (DWORD)disp64;
    BOOL haveLine = SymGetLineFromAddr64(process, stackframe.AddrPC.Offset, &disp32, &line);
#if NM_CALL_STACK_WANT_INFO
    if (haveLine || haveSymbol)
#endif
    {
      sprintf(frameBuffer,
        "Frame %d:\n"
        "    Symbol name:    %s\n"
        "    PC-Stack-Frame address: 0x%016llx - 0x%016llx - 0x%016llx\n"
        "    File (line): %s (%d)\n"
        "\n",
        frame,
        haveSymbol ? symbol->Name : "???",
        stackframe.AddrPC.Offset,
        stackframe.AddrStack.Offset,
        stackframe.AddrFrame.Offset,
        haveLine ? line.FileName : "???",
        haveLine ? line.LineNumber : 0
        );

      strcat(traceBuffer, frameBuffer);
      frameOut++;
    }

    if (++frame >= NM_CALL_STACK_MAX_FRAMES)
    {
      break;
    }
  }

  if(!frameOut)
  {
    strcat(traceBuffer, "- unavailable without the .pdb's\n");
  }

  // Uninitialize 
  bRet = ::SymCleanup(GetCurrentProcess()); 
  if( !bRet ) 
  {
    NMP_STDOUT("Error: SymCleanup() failed. Error code: %u \n", ::GetLastError());
    return; 
  }

  if (!NMP::Exception::alreadyDealingWithAnException())
  {
    throw NMP::SEHException(traceBuffer, pExp);
  }
  else
  {
    NMP_MSG("Exception thrown whilst handling an exception");
    NMP_MSG(traceBuffer);
    NM_BREAK();
  }
  
}

#endif // NM_CALL_STACK_DEBUG

//----------------------------------------------------------------------------------------------------------------------

#endif // NM_ENABLE_EXCEPTIONS

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
//----------------------------------------------------------------------------------------------------------------------
