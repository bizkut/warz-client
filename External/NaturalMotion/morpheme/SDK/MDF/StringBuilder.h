#pragma once
// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------

#include <assert.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "NMPlatform/NMPlatform.h"
#include "nmtl/hashfunc.h"

typedef void (*ch_pfn)(char *);
typedef void (__stdcall *stdcall_ch_pfn)(char *);   // for Win32 API

/**
 * StringBuilder provides a useful container for working with strings, minimizing allocation
 * and object creation. Similar in purpose to the C# equivalent.
 */
class StringBuilder
{
public:

  // allocate a string buffer of 'initialCapacity' * sizeof(char)
  inline StringBuilder(int initialCapacity);
  inline StringBuilder(const StringBuilder& rhs);

  // pass an existing string pointer to use as a buffer; eg. for working entirely on the stack,
  // _alloca() a buffer, pass it and the size of the buffer 
  // NOTE: no reallocation takes place; if you exceed the buffer, asserts will fire and functions will early-out
  inline StringBuilder(char* externalPtr, int maxSize);
  inline ~StringBuilder();

  inline bool appendf(const char* formatString, ...);               ///< printf() onto the end of the buffer
  inline bool prependf(const char* formatString, ...);              ///< printf() insert into the beginning

  // append various types of basic data; all return false if buffer resizing failed (usually only will happen
  // if the builder was constructed with an external pointer)
  inline bool appendCharBuf(const char* string, unsigned int length);    ///< copy the given buffer for 'length' characters; length does not include null terminator, eg. L"abc" = length:3
  inline bool appendCharBuf(const char* string) { return appendCharBuf(string, (unsigned int)strlen(string)); }
  inline bool appendPString(const PString& pstring) { return appendCharBuf(pstring.c_str(), (unsigned int)pstring.length()); }
  inline bool appendChar(char character);                       ///< append a single character
  inline bool appendInt(int i);                                  ///< convert an integer into the buffer (itow)

  inline void trimFront(int amount);
  inline void trimCharsFromFront(char charToStrip);
  inline void trimEnd(int amount);
  inline void trimCharsFromEnd(char charToStrip);

  inline const char* convertToUpperCase() { NMP_STRUPR(m_buffer, NMP_STRLEN(m_buffer)); return m_buffer;}

  // access to results
  inline const char* getBufferPtr(int &length) const { length = m_writeIndex-1; return m_buffer; }
  inline const char* getBufferPtr() const { return m_buffer; }
  inline int getLength() const { return m_writeIndex-1; }
  inline char getChar(int index) const { assert(index < getLength()); return m_buffer[index]; }
  inline void setChar(int index, char newCh) { assert(index < getLength()); m_buffer[index] = newCh; }

  // smoosh the contents into a 64-bit hash code
  inline __int64 asHash64() const;

  // alias for porting from std::string
  inline const char* c_str() const { return getBufferPtr(); }

  // reset to empty buffer, doesn't throw away memory
  inline void reset();

  // simple one-stage undo mechanism, giving the ability to unwind back to a previous state
  // this is really useful for code that generates paths, where a repeated part of a path can be built into the buffer,
  // save()d and then the looping code can restore() and append custom suffixes without rebuilding the common component
  inline void save();
  inline bool restore();                  ///< returns false if save() wasn't called first

  // call with a function pointer to have that function executed on the internal buffer;
  // obviously, don't pass it something that will destroy things, this is designed to allow direct use of
  // say, the SHLWAPI functions without having to create loads of temporary buffers
  inline void invoke(ch_pfn fn)
  {
    fn(m_buffer);
    m_writeIndex = strlen(m_buffer) + 1;
  }
  inline void invoke(stdcall_ch_pfn fn)
  {
    fn(m_buffer);
    m_writeIndex = strlen(m_buffer) + 1;
  }

private:

  StringBuilder();

  // minimum size the buffer is resized if we run out of space
  enum { ReallocChunkSize = 256 };

  inline bool ensureExtraCapacity(int extra);

  char     *m_buffer;
  int       m_size, 
            m_writeIndex;
  int       m_savedWriteIndex;
  bool      m_ownsBuffer;
};


//----------------------------------------------------------------------------------------------------------------------
StringBuilder::StringBuilder(int initialCapacity) :
  m_buffer(0),
  m_size(0),
  m_writeIndex(0),
  m_savedWriteIndex(-1),
  m_ownsBuffer(true)
{
  m_buffer = (char*)malloc(initialCapacity * sizeof(char));
  m_size = initialCapacity;

  reset();
}

//----------------------------------------------------------------------------------------------------------------------
StringBuilder::StringBuilder(char* externalPtr, int maxSize) :
  m_buffer(externalPtr),
  m_size(maxSize),
  m_writeIndex(0),
  m_ownsBuffer(false)
{
  reset();
}

//----------------------------------------------------------------------------------------------------------------------
StringBuilder::StringBuilder(const StringBuilder& rhs) :
  m_buffer(0),
  m_size(rhs.m_size),
  m_writeIndex(rhs.m_writeIndex),
  m_savedWriteIndex(rhs.m_savedWriteIndex),
  m_ownsBuffer(true)
{
  m_buffer = (char*)malloc(m_size * sizeof(char));
  memcpy(m_buffer, rhs.m_buffer, m_size);
}

//----------------------------------------------------------------------------------------------------------------------
StringBuilder::~StringBuilder()
{
  if (m_ownsBuffer)
  {
    free(m_buffer);
  }

  m_buffer = 0;
  m_size = 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool StringBuilder::appendf(const char* formatString, ...)
{
  va_list args;
  va_start(args, formatString);

  int extraSpaceEstimate = 0;

#ifdef WIN32
  extraSpaceEstimate = _vscprintf(formatString, args);
#else
  // .. now what?
  extraSpaceEstimate = 1024;
#endif

  if (!ensureExtraCapacity(extraSpaceEstimate + 1))
  {
    va_end(args);
    return false;
  }

  int charsWritten = NMP_VSPRINTF(&m_buffer[m_writeIndex-1], size_t(m_size - m_writeIndex) - 1, formatString, args);

  m_writeIndex += charsWritten;

  va_end(args);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool StringBuilder::prependf(const char* formatString, ...)
{
  va_list args;
  va_start(args, formatString);

  int extraSpaceEstimate = 0;

#ifdef WIN32
  extraSpaceEstimate = _vscprintf(formatString, args);
#else
  // .. now what?
  extraSpaceEstimate = 1024;
#endif

  char *tBuf = (char*)_alloca(extraSpaceEstimate + 1);
  int charsWritten = NMP_VSPRINTF(tBuf, extraSpaceEstimate + 1, formatString, args);

  va_end(args);

  size_t curLen = strlen(m_buffer) + 1;

  if (!ensureExtraCapacity(charsWritten + 1))
    return false;

  memcpy(&m_buffer[charsWritten], &m_buffer[0], curLen);
  memcpy(m_buffer, tBuf, charsWritten);

  m_writeIndex += charsWritten;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool StringBuilder::appendCharBuf(const char* string, unsigned int length)
{
  if (length == 0 || string == 0 || string[0] == '\0')
    return true;

  length += 1; // for null

  if (!ensureExtraCapacity(length))
    return false;

  // starting at the buffer's current null-terminator, overwrite
  memcpy(&m_buffer[m_writeIndex - 1], string, (length - 1) * sizeof(char));

  // jump to the new terminator
  m_writeIndex += (length - 1);

  // Make sure that the buffer is null terminated
  m_buffer[m_writeIndex - 1] = 0;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool StringBuilder::appendChar(char character)
{
  if (!ensureExtraCapacity(2))
    return false;

  // overwrite null term, add new one
  m_buffer[m_writeIndex - 1] = character; 
  m_buffer[m_writeIndex] = '\0'; 

  m_writeIndex ++;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool StringBuilder::appendInt(int i)
{
  if (!ensureExtraCapacity(35))  // worst case, 33 + null + sign
    return false;

#ifdef WIN32
  // itoa directly into the buffer
  _itoa_s(i, &m_buffer[m_writeIndex - 1], 34, 10);

  // walk up to the just-added null term
  while (m_buffer[m_writeIndex] != '\0')
    m_writeIndex ++;
  m_writeIndex ++;
#else
  // linux has no itow..?
  m_writeIndex += sprintf(&m_buffer[m_writeIndex - 1], (m_size - m_writeIndex), "%i", i);
#endif

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void StringBuilder::trimFront(int amount)
{
  int tLen = getLength();

  if (amount == 0 || 
    tLen == 0)  // nothing to do?
    return;

  // if we are trimming more than we've got, it simply
  // zeros out the string
  if (amount >= tLen)
  {
    reset();
  }
  else
  {
    memcpy(m_buffer, &m_buffer[amount], tLen - amount);
    m_writeIndex -= amount;
    m_buffer[m_writeIndex - 1] = '\0'; 
  }
}

//----------------------------------------------------------------------------------------------------------------------
void StringBuilder::trimCharsFromFront(char charToStrip)
{
  if (getLength() <= 1)
    return;

  int amountToTrim = 0;
  char* bf = m_buffer;
  while (*bf == charToStrip && amountToTrim < getLength())
  {
    amountToTrim ++;
    bf ++;
  }

  trimFront(amountToTrim);
}

//----------------------------------------------------------------------------------------------------------------------
void StringBuilder::trimEnd(int amount)
{
  int tLen = getLength();

  if (amount == 0 || 
      tLen == 0)  // nothing to do?
    return;

  // if we are trimming more than we've got, it simply
  // zeros out the string
  if (amount >= tLen)
  {
    reset();
  }
  else
  {
    // trim and re-terminate
    m_writeIndex -= amount;
    m_buffer[m_writeIndex - 1] = '\0'; 
  }
}

//----------------------------------------------------------------------------------------------------------------------
void StringBuilder::trimCharsFromEnd(char charToStrip)
{
  if (getLength() <= 1)
    return;

  int amountToTrim = 0;
  char* bf = &m_buffer[getLength() - 1];
  while (*bf == charToStrip && amountToTrim < getLength())
  {
    amountToTrim ++;
    bf --;
  }

  trimEnd(amountToTrim);
}

//----------------------------------------------------------------------------------------------------------------------
__int64 StringBuilder::asHash64() const
{
  unsigned int hashA = nmtl::MurmurHash2(m_buffer, getLength(), 0x1EDC6F41);
  unsigned int hashB = nmtl::MurmurHash2(m_buffer, getLength(), 0xEB31D82E);

  __int64 result = ((__int64)hashB << 32) | ((__int64)hashA);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void StringBuilder::reset()
{
  assert(m_size >= 2);
  m_writeIndex = 1;   // write index always points to 1 past the current null terminator
  m_buffer[0] = '\0';
}

//----------------------------------------------------------------------------------------------------------------------
void StringBuilder::save()
{
  m_savedWriteIndex = m_writeIndex;
}

//----------------------------------------------------------------------------------------------------------------------
bool StringBuilder::restore()
{
  if (m_savedWriteIndex >= 0)
  {
    // try to avoid restore()ing forward into unclaimed territory!
    assert(m_savedWriteIndex <= m_writeIndex);

    m_buffer[m_savedWriteIndex - 1] = '\0'; // block off any added content
    m_writeIndex = m_savedWriteIndex;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool StringBuilder::ensureExtraCapacity(int extra)
{
  if (m_writeIndex + extra >= (m_size))
  {
    if (!m_ownsBuffer)
    {
      assert(0);
      return false;
    }

    // bump it up in chunks of minimum ReallocChunkSize
    int extraCap = (extra > ReallocChunkSize)?(extra):ReallocChunkSize;

    m_size += extraCap;
    m_buffer = (char*)realloc(m_buffer, m_size * sizeof(char));   
  }

  return true;
}
