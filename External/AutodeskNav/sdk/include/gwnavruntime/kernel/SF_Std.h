/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

/**************************************************************************

PublicHeader:   Kernel
Filename    :   KY_Std.h
Content     :   Standard C function interface
Created     :   2/25/2007
Authors     :   Ankur Mohan, Michael Antonov, Artem Bolgar, Maxim Shemanarev

**************************************************************************/

#ifndef INC_KY_Kernel_Std_H
#define INC_KY_Kernel_Std_H

#include "gwnavruntime/kernel/SF_Types.h"
#include <stdarg.h> // for va_list args
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#if !defined(KY_OS_WINCE) && defined(KY_CC_MSVC) && (KY_CC_MSVC >= 1400)
#define KY_MSVC_SAFESTRING
#include <errno.h>
#endif

// Wide-char funcs
#if !defined(KY_OS_SYMBIAN) && !defined(KY_CC_RENESAS) && !defined(KY_OS_PS2)
#include <wchar.h>
#endif

#if !defined(KY_OS_SYMBIAN) && !defined(KY_CC_RENESAS) && !defined(KY_OS_PS2) && !defined(KY_CC_SNC)
#include <wctype.h>
#endif

namespace Kaim {

#if defined(KY_OS_WIN32)
inline char* KY_CDECL SFitoa(int val, char *dest, UPInt destsize, int radix)
{
#if defined(KY_MSVC_SAFESTRING)
    _itoa_s(val, dest, destsize, radix);
    return dest;
#else
    KY_UNUSED(destsize);
    return itoa(val, dest, radix);
#endif
}
#else // KY_OS_WIN32
inline char* SFitoa(int val, char* dest, unsigned int len, int radix)
{
    if (val == 0)
    {
        if (len > 1)
        {
            dest[0] = '0';
            dest[1] = '\0';  
        }
        return dest;
    }

    int cur = val;
    unsigned int i    = 0; 
    unsigned int sign = 0;

    if (val < 0)
    {
        val = -val;
        sign = 1;
    }

    while ((val != 0) && (i < (len - 1 - sign)))        
    {
        cur    = val % radix;
        val   /= radix;

        if (radix == 16)
        {
            switch(cur)
            {
            case 10:
                dest[i] = 'a';
                break;
            case 11:
                dest[i] = 'b';
                break;
            case 12:
                dest[i] = 'c';
                break;
            case 13:
                dest[i] = 'd';
                break;
            case 14:
                dest[i] = 'e';
                break;
            case 15:
                dest[i] = 'f';
                break;
            default:
                dest[i] = (char)('0' + cur);
                break;
            }
        } 
        else
        {
            dest[i] = (char)('0' + cur);
        }
        ++i;
    }

    if (sign)
    {
        dest[i++] = '-';
    }

    for (unsigned int j = 0; j < i / 2; ++j)
    {
        char tmp        = dest[j];
        dest[j]         = dest[i - 1 - j];
        dest[i - 1 - j] = tmp;
    }
    dest[i] = '\0';

    return dest;
}

#endif


// String functions

inline UPInt KY_CDECL SFstrlen(const char* str)
{
    return strlen(str);
}

inline char* KY_CDECL SFstrcpy(char* dest, UPInt destsize, const char* src)
{
#if defined(KY_MSVC_SAFESTRING)
    strcpy_s(dest, destsize, src);
    return dest;
#else
    KY_UNUSED(destsize);
    return strcpy(dest, src);
#endif
}

// Note that when using strncpy no null-character is implicitly appended at the end of destination if source is longer than num 
// (thus, in this case, destination may not be a null terminated C string).
inline char* KY_CDECL SFstrncpy(char* dest, UPInt destsize, const char* src, UPInt count)
{
#if defined(KY_MSVC_SAFESTRING)
    strncpy_s(dest, destsize, src, count);
    return dest;
#else
    KY_UNUSED(destsize);
    return strncpy(dest, src, count);
#endif
}

inline char * KY_CDECL SFstrcat(char* dest, UPInt destsize, const char* src)
{
#if defined(KY_MSVC_SAFESTRING)
    strcat_s(dest, destsize, src);
    return dest;
#else
    KY_UNUSED(destsize);
    return strcat(dest, src);
#endif
}

inline int KY_CDECL SFstrcmp(const char* dest, const char* src)
{
    return strcmp(dest, src);
}

inline const char* KY_CDECL SFstrchr(const char* str, char c)
{
    return strchr(str, c);
}

inline char* KY_CDECL SFstrchr(char* str, char c)
{
    return strchr(str, c);
}

inline const char* SFstrrchr(const char* str, char c)
{
    UPInt len = SFstrlen(str);
    for (UPInt i=len; i>0; i--)     
        if (str[i]==c) 
            return str+i;
    return 0;
}

inline const UByte* KY_CDECL SFmemrchr(const UByte* str, UPInt size, UByte c)
{
    for (SPInt i = (SPInt)size - 1; i >= 0; i--)     
    {
        if (str[i] == c) 
            return str + i;
    }
    return 0;
}

inline char* KY_CDECL SFstrrchr(char* str, char c)
{
    UPInt len = SFstrlen(str);
    for (UPInt i=len; i>0; i--)     
        if (str[i]==c) 
            return str+i;
    return 0;
}


double KY_CDECL SFstrtod(const char* string, char** tailptr);

inline long KY_CDECL SFstrtol(const char* string, char** tailptr, int radix)
{
    return strtol(string, tailptr, radix);
}

inline long KY_CDECL SFstrtoul(const char* string, char** tailptr, int radix)
{
    return strtoul(string, tailptr, radix);
}

inline int KY_CDECL SFstrncmp(const char* ws1, const char* ws2, UPInt size)
{
    return strncmp(ws1, ws2, size);
}

inline UInt64 KY_CDECL SFstrtouq(const char *nptr, char **endptr, int base)
{
#if defined(KY_CC_MSVC) && !defined(KY_OS_WINCE)
    return _strtoui64(nptr, endptr, base);
#else
    return strtoull(nptr, endptr, base);
#endif
}

inline SInt64 KY_CDECL SFstrtoq(const char *nptr, char **endptr, int base)
{
#if defined(KY_CC_MSVC) && !defined(KY_OS_WINCE)
    return _strtoi64(nptr, endptr, base);
#else
    return strtoll(nptr, endptr, base);
#endif
}


inline SInt64 KY_CDECL SFatoq(const char* string)
{
#if defined(KY_CC_MSVC) && !defined(KY_OS_WINCE)
    return _atoi64(string);
#else
    return atoll(string);
#endif
}

inline UInt64 KY_CDECL SFatouq(const char* string)
{
  return SFstrtouq(string, NULL, 10);
}


// Implemented in GStd.cpp in platform-specific manner.
int KY_CDECL SFstricmp(const char* dest, const char* src);
int KY_CDECL SFstrnicmp(const char* dest, const char* src, UPInt count);

inline UPInt KY_CDECL SFsprintf(char *dest, UPInt destsize, const char* format, ...)
{
    va_list argList;
    va_start(argList,format);
    UPInt ret;
#if defined(KY_CC_MSVC)
    #if defined(KY_MSVC_SAFESTRING)
        ret = _vsnprintf_s(dest, destsize, _TRUNCATE, format, argList);
        KY_ASSERT(ret != -1);
    #else
        KY_UNUSED(destsize);
        ret = _vsnprintf(dest, destsize - 1, format, argList); // -1 for space for the null character
        KY_ASSERT(ret != -1);
        dest[destsize-1] = 0;
    #endif
#else
    KY_UNUSED(destsize);
    ret = vsprintf(dest, format, argList);
    KY_ASSERT(ret < destsize);
#endif
    va_end(argList);
    return ret;
}

inline UPInt KY_CDECL SFvsprintf(char *dest, UPInt destsize, const char * format, va_list argList)
{
    UPInt ret;
#if defined(KY_CC_MSVC)
    #if defined(KY_MSVC_SAFESTRING)
        dest[0] = '\0';
        int rv = vsnprintf_s(dest, destsize, _TRUNCATE, format, argList);
        if (rv == -1)
        {
            dest[destsize - 1] = '\0';
            ret = destsize - 1;
        }
        else
            ret = (UPInt)rv;
    #else
        KY_UNUSED(destsize);
        int rv = _vsnprintf(dest, destsize - 1, format, argList);
        KY_ASSERT(rv != -1);
        ret = (UPInt)rv;
        dest[destsize-1] = 0;
    #endif
#else
    KY_UNUSED(destsize);
    ret = (UPInt)vsprintf(dest, format, argList);
    KY_ASSERT(ret < destsize);
#endif
    return ret;
}

wchar_t* KY_CDECL SFwcscpy(wchar_t* dest, UPInt destsize, const wchar_t* src);
wchar_t* KY_CDECL SFwcsncpy(wchar_t* dest, UPInt destsize, const wchar_t* src, UPInt count);
wchar_t* KY_CDECL SFwcscat(wchar_t* dest, UPInt destsize, const wchar_t* src);
UPInt    KY_CDECL SFwcslen(const wchar_t* str);
int      KY_CDECL SFwcscmp(const wchar_t* a, const wchar_t* b);
int      KY_CDECL SFwcsicmp(const wchar_t* a, const wchar_t* b);

inline int KY_CDECL SFwcsicoll(const wchar_t* a, const wchar_t* b)
{
#if defined(KY_OS_WIN32) || defined(KY_OS_XBOX) || defined(KY_OS_XBOX360) || defined(KY_OS_WII)
#if defined(KY_CC_MSVC) && (KY_CC_MSVC >= 1400)
    return ::_wcsicoll(a, b);
#else
    return ::wcsicoll(a, b);
#endif
#else
    // not supported, use regular wcsicmp
    return SFwcsicmp(a, b);
#endif
}

inline int KY_CDECL SFwcscoll(const wchar_t* a, const wchar_t* b)
{
#if defined(KY_OS_WIN32) || defined(KY_OS_XBOX) || defined(KY_OS_XBOX360) || defined(KY_OS_PS3) || defined(KY_OS_WII) || defined(KY_OS_LINUX)
    return wcscoll(a, b);
#else
    // not supported, use regular wcscmp
    return SFwcscmp(a, b);
#endif
}

#ifndef KY_NO_WCTYPE

inline int KY_CDECL UnicodeCharIs(const UInt16* table, wchar_t charCode)
{
    unsigned offset = table[charCode >> 8];
    if (offset == 0) return 0;
    if (offset == 1) return 1;
    return (table[offset + ((charCode >> 4) & 15)] & (1 << (charCode & 15))) != 0;
}

extern const UInt16 UnicodeAlnumBits[];
extern const UInt16 UnicodeAlphaBits[];
extern const UInt16 UnicodeDigitBits[];
extern const UInt16 UnicodeSpaceBits[];
extern const UInt16 UnicodeXDigitBits[];

// Uncomment if necessary
//extern const UInt16 UnicodeCntrlBits[];
//extern const UInt16 UnicodeGraphBits[];
//extern const UInt16 UnicodeLowerBits[];
//extern const UInt16 UnicodePrintBits[];
//extern const UInt16 UnicodePunctBits[];
//extern const UInt16 UnicodeUpperBits[];

inline int KY_CDECL SFiswalnum (wchar_t charCode) { return UnicodeCharIs(UnicodeAlnumBits,  charCode); }
inline int KY_CDECL SFiswalpha (wchar_t charCode) { return UnicodeCharIs(UnicodeAlphaBits,  charCode); }
inline int KY_CDECL SFiswdigit (wchar_t charCode) { return UnicodeCharIs(UnicodeDigitBits,  charCode); }
inline int KY_CDECL SFiswspace (wchar_t charCode) { return UnicodeCharIs(UnicodeSpaceBits,  charCode); }
inline int KY_CDECL SFiswxdigit(wchar_t charCode) { return UnicodeCharIs(UnicodeXDigitBits, charCode); }

// Uncomment if necessary
//inline int KY_CDECL SFiswcntrl (wchar_t charCode) { return UnicodeCharIs(UnicodeCntrlBits,  charCode); }
//inline int KY_CDECL SFiswgraph (wchar_t charCode) { return UnicodeCharIs(UnicodeGraphBits,  charCode); }
//inline int KY_CDECL SFiswlower (wchar_t charCode) { return UnicodeCharIs(UnicodeLowerBits,  charCode); }
//inline int KY_CDECL SFiswprint (wchar_t charCode) { return UnicodeCharIs(UnicodePrintBits,  charCode); }
//inline int KY_CDECL SFiswpunct (wchar_t charCode) { return UnicodeCharIs(UnicodePunctBits,  charCode); }
//inline int KY_CDECL SFiswupper (wchar_t charCode) { return UnicodeCharIs(UnicodeUpperBits,  charCode); }

int KY_CDECL SFtowupper(wchar_t charCode);
int KY_CDECL SFtowlower(wchar_t charCode);

#else // KY_NO_WCTYPE

inline int KY_CDECL SFiswspace(wchar_t c)
{
#if defined(KY_OS_WII) || defined(KY_CC_SNC)
    return ((c) < 128 ? isspace((char)c) : 0);
#else
    return iswspace(c);
#endif
}

inline int KY_CDECL SFiswdigit(wchar_t c)
{
#if defined(KY_OS_WII) || defined(KY_CC_SNC)
    return ((c) < 128 ? isdigit((char)c) : 0);
#else
    return iswdigit(c);
#endif
}

inline int KY_CDECL SFiswxdigit(wchar_t c)
{
#if defined(KY_OS_WII) || defined(KY_CC_SNC)
    return ((c) < 128 ? isxdigit((char)c) : 0);
#else
    return iswxdigit(c);
#endif
}

inline int KY_CDECL SFiswalpha(wchar_t c)
{
#if defined(KY_OS_WII) || defined(KY_CC_SNC)
    return ((c) < 128 ? isalpha((char)c) : 0);
#else
    return iswalpha(c);
#endif
}

inline int KY_CDECL SFiswalnum(wchar_t c)
{
#if defined(KY_OS_WII) || defined(KY_CC_SNC)
    return ((c) < 128 ? isalnum((char)c) : 0);
#else
    return iswalnum(c);
#endif
}

inline wchar_t KY_CDECL SFtowlower(wchar_t c)
{
#if (defined(KY_OS_PS3) && defined(KY_CC_SNC))
    return (wchar_t)tolower((char)c);
#else
    return (wchar_t)towlower(c);
#endif
}

inline wchar_t SFtowupper(wchar_t c)
{
#if (defined(KY_OS_PS3) && defined(KY_CC_SNC))
    return (wchar_t)toupper((char)c);
#else
    return (wchar_t)towupper(c);
#endif
}

#endif // KY_NO_WCTYPE

// ASCII versions of tolower and toupper. Don't use "char"
inline int KY_CDECL SFtolower(int c)
{
    return (c >= 'A' && c <= 'Z') ? c - 'A' + 'a' : c;
}

inline int KY_CDECL SFtoupper(int c)
{
    return (c >= 'a' && c <= 'z') ? c - 'a' + 'A' : c;
}



inline double KY_CDECL SFwcstod(const wchar_t* string, wchar_t** tailptr)
{
#if defined(KY_OS_OTHER)
    KY_UNUSED(tailptr);
    char buffer[64];
    char* tp = NULL;
    UPInt max = SFwcslen(string);
    if (max > 63) max = 63;
    unsigned char c = 0;
    for (UPInt i=0; i < max; i++)
    {
        c = (unsigned char)string[i];
        buffer[i] = ((c) < 128 ? (char)c : '!');
    }
    buffer[max] = 0;
    return SFstrtod(buffer, &tp);
#else
    return wcstod(string, tailptr);
#endif
}

inline long KY_CDECL SFwcstol(const wchar_t* string, wchar_t** tailptr, int radix)
{
#if defined(KY_OS_OTHER)
    KY_UNUSED(tailptr);
    char buffer[64];
    char* tp = NULL;
    UPInt max = SFwcslen(string);
    if (max > 63) max = 63;
    unsigned char c = 0;
    for (UPInt i=0; i < max; i++)
    {
        c = (unsigned char)string[i];
        buffer[i] = ((c) < 128 ? (char)c : '!');
    }
    buffer[max] = 0;
    return strtol(buffer, &tp, radix);
#else
    return wcstol(string, tailptr, radix);
#endif
}

} // Scaleform

#endif // INC_GSTD_H
