/*
 * Copyright 2009-2011 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */

#ifndef SUTIL_H
#define SUTIL_H

#include "PsShare.h"

namespace physx
{
	namespace general_string_parsing2
	{

char *         stristr(const char *str,const char *key);       // case insensitive str str
bool           isstristr(const char *str,const char *key);     // bool true/false based on case insenstive strstr
PxU32   GetHEX(const char *foo,const char **next=0);
PxU8  GetHEX1(const char *foo,const char **next=0);
PxU16 GetHEX2(const char *foo,const char **next=0);
PxU32   GetHEX4(const char *foo,const char **next=0);
PxF32          GetFloatValue(const char *str,const char **next=0);
PxI32            GetIntValue(const char *str,const char **next=0);
const char *   SkipWhitespace(const char *str);
bool           IsWhitespace(char c);
const char *   FloatString(PxF32 v,bool binary=false);
const char *   GetTrueFalse(PxU32 state);
bool           CharToWide(const char *source,wchar_t *dest,PxI32 maxlen);
bool           WideToChar(const wchar_t *source,char *dest,PxI32 maxlen);
const char **  GetArgs(char *str,PxI32 &count); // destructable parser, stomps EOS markers on the input string!
PxI32            GetUserArgs(const char *us,const char *key,const char **args);
bool           GetUserSetting(const char *us,const char *key,PxI32 &v);
bool           GetUserSetting(const char *us,const char *key,const char * &v);
const char *   GetRootName(const char *fname); // strip off everything but the 'root' file name.
bool           IsTrueFalse(const char *c);
bool           IsDirectory(const char *fname,char *path,char *basename,char *postfix);
bool           hasSpace(const char *str); // true if the string contains a space
const char *   lastDot(const char *src);
const char *   lastSlash(const char *src); // last forward or backward slash character, null if none found.
const char *   lastChar(const char *src,char c);
const char  	*fstring(PxF32 v);
const char *   formatNumber(PxI32 number); // JWR  format this integer into a fancy comma delimited string
bool           fqnMatch(const char *n1,const char *n2); // returns true if two fully specified file names are 'the same' but ignores case sensitivty and treats either a forward or backslash as the same character.
bool           getBool(const char *str);
bool           needsQuote(const char *str); // if this string needs quotes around it (spaces, commas, #, etc)

#define MAX_FQN 512 // maximum size of an FQN string
void           normalizeFQN(const wchar_t *source,wchar_t *dest);
void           normalizeFQN(const char *source,char *dest);
bool           endsWith(const char *str,const char *ends,bool caseSensitive);

}; // end of namespace
using namespace general_string_parsing2;
};

#endif // SUTIL_H
