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
#ifndef NMP_STRING_H
#define NMP_STRING_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

typedef wchar_t wchar;

//----------------------------------------------------------------------------------------------------------------------
struct WideString
{
  wchar m_value;
};

//----------------------------------------------------------------------------------------------------------------------
struct CharString
{
  unsigned char m_value;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::NMString
/// \brief
/// \ingroup NaturalMotionPlatform
//----------------------------------------------------------------------------------------------------------------------
class NMString
{
public:

  static uint32_t getWCharStringLength(const wchar* wstring);

  static wchar* charToWChar(wchar* wstring, const char* string);

  static char* wCharToChar(char* string, const wchar* wstring);

private:

};

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NMP_STRING_H
//----------------------------------------------------------------------------------------------------------------------
