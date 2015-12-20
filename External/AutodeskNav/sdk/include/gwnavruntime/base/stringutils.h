/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_StringUtils_H
#define Navigation_StringUtils_H

#include "gwnavruntime/kernel/SF_String.h"
#include "gwnavruntime/base/types.h"
#include "gwnavruntime/containers/kyarray.h"
#include <stdarg.h> // for va_list args

namespace Kaim
{

/// Utility functions for manipulating strings
class StringUtils
{
public:
	// ------------ Parsing functions ------------
	/// returns -1 if not found
	static KyInt32 FindFirstOf(const char* str, const char* charsToSearch, KyInt32 start = 0);

	// Spit string into tokens
	static void GetTokens(const char* str, const char* delimiters, KyArray<String>& tokens, bool acceptEmptyStrings = false);

	// ------------ safe sprintf variants ------------
	// It is safe in regards to the final content of 'dest' if textFmt and the argument list are well formed and enough memory is available.
	// So 'dest' can be reallocated in this function in order to be large enough to store the complete string. 
	// However using this function is slower than calling standard functions.
	static KyInt32 SafeSvprintf(KyArrayPOD<char>& dest, const char* textFmt, va_list argList);
	static KyInt32 SafeSprintf(KyArrayPOD<char>& dest, const char* textFmt, ...);
};

} // namespace Kaim

#endif
