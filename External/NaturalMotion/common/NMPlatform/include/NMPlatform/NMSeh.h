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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef NM_SEH_H
#define NM_SEH_H
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \file NMSeh.h
/// \class NMP::SEHException
/// \brief This header defines an optional structured exception handler which generates a call stack output.
/// \ingroup NaturalMotionPlatform
//----------------------------------------------------------------------------------------------------------------------

#include "NMPlatform/NMPlatform.h"

//----------------------------------------------------------------------------------------------------------------------
#if NM_ENABLE_EXCEPTIONS
  // NM_ENABLE_EXCEPTIONS must be defined, as in our release builds, and project compiled with /EHa
 
#if defined(_M_IX86) || defined(_M_AMD64) || defined(_M_IA64)
  // Windows only

  // Set for exception handling with Call Stack output
#define NM_CALL_STACK_DEBUG 1

  // Limit the call frame depth
#define NM_CALL_STACK_MAX_FRAMES 8

  // Keep to frames with file/line or symbol info
#define NM_CALL_STACK_WANT_INFO 1

//----------------------------------------------------------------------------------------------------------------------

#if NM_CALL_STACK_DEBUG

#include <windows.h>
#include <eh.h>
#include <stdio.h>

namespace NMP
{
  class SEHException : public NMP::Exception
  {
  public:
    SEHException(const char* what, EXCEPTION_POINTERS* pExp)
      : NMP::Exception(what, __FILE__, __LINE__), m_pExp(pExp)
    { 
      // Using printf to avoid extending buffer sizes on alternative output functions.
      printf_s(what);
    }

  private:
    EXCEPTION_POINTERS* m_pExp;
  };

  class SetSETranslator
  {
  public:
    SetSETranslator()
    {
      prev = _set_se_translator(win32ExceptionHandler);
    }

    ~SetSETranslator()
    {
      _set_se_translator(prev);
    }
    
    static void win32ExceptionHandler(unsigned, EXCEPTION_POINTERS*);

  private:
    _se_translator_function prev;
  };

} // namespace NMP

#endif // #if NM_CALL_STACK_DEBUG

//----------------------------------------------------------------------------------------------------------------------

#endif // defined(_M_IX86) || defined(_M_AMD64) || defined(_M_IA64)
#endif // NM_ENABLE_EXCEPTIONS

#ifndef NM_CALL_STACK_DEBUG
#define NM_CALL_STACK_DEBUG 0
#endif

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_SEH

