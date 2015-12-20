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
#include "NMPlatform/NMString.h"
#include <stdarg.h>

//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
uint32_t NMString::getWCharStringLength(const wchar* wstring)
{
  uint32_t len;
  const wchar* p;

  NMP_ASSERT(wstring);

  len = 0;
  p = wstring;

  while (*p)
  {
    len++;
    p++;
  }

  return len;
}

//----------------------------------------------------------------------------------------------------------------------
wchar* NMString::charToWChar(wchar* wstring, const char* string)
{
  uint32_t count;
  uint32_t len;

  NMP_ASSERT(string);
  NMP_ASSERT(wstring);

  count = 0;
  len = (uint32_t) strlen(string);

  while (count < len)
  {
    wstring[count] = string[count];
    count++;
  }

  wstring[count] = '\0';

  return wstring;
}

//----------------------------------------------------------------------------------------------------------------------
char* NMString::wCharToChar(char* string, const wchar* wstring)
{
  uint32_t count;
  uint32_t len;

  NMP_ASSERT(string);
  NMP_ASSERT(wstring);

  count = 0;
  len = getWCharStringLength(wstring);

  while (count < len)
  {
    string[count] = static_cast<char> (wstring[count]);
    count++;
  }

  string[count] = '\0';

  return string;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------

