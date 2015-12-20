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
#include "nmtl/vector.h"
#include "nmtl/hashfunc.h"

#include "FixedStack.h"
#include "ParserMemory.h"

/**
 * String container for use by the parser. 
 *
 * Holds a hashed representation of the current string, so comparison and such 
 * (which the parser does a lot of) is cheap. It allocates in 4-byte chunks
 * to both speed up hashing as well as to reduce resizing during re-use.
 *
 * eg. (^ marks the string terminator, a null byte, | marks 4 byte chunk)
 *     values set left/right, subsequent set()s top/bottom
 *
 *     "Foo"    "C"      "ModuleDef"
 *     |Foo^|   |C^  |   |Modu|leDe|f^  |
 *
 *                       "Bhv"
 *                       |Bhv^|    |    |
 *
 * It uses two 32-bit hash values to try and make sure that false-positive matches
 * are very unlikely.
 *
 * IMPORTANT: PStrings allocate from the active parser heap and are therefore
 *            only valid for use inside the parse loop. Do not use for code-gen 
 *            tasks or similar (except in a 'read only' context from the MDF DB)
 */
class PString
{
  enum
  {
    // don't change the seeds without fixing the zero mix ones
    // or rather, just don't change any of these values!
    PSTRING_HASH_SEED_A = 0x04C11DB7,
    PSTRING_HASH_SEED_B = 0x741B8CD7,

    // the 'zero mix' values are the results of passing the
    // above seeds through the Hsieh hash with no input text; 
    // eg. these are the hash values for the string when it is empty.
    PSTRING_HASH_SEED_A_ZEROMIX = 0x01db65c9,
    PSTRING_HASH_SEED_B_ZEROMIX = 0x3e83205a
  };

public:

  PString()
  {
    resetInternal();
  }

  explicit PString(unsigned int amt)
  {
    resetInternal();
    amt = align4(amt);
    adjustBuffer(amt);
    memset(m_text, 0, amt);
  }

  PString(const char* text)
  {
    resetInternal();
    setText(text);
  }

  PString(const PString& rhs)
  {
    resetInternal();
    copyFrom(rhs);
  }

  PString& operator=(const PString& rhs)
  {
    copyFrom(rhs);
    return *this;
  }

  PString& operator=(const char* rhs)
  {
    setText(rhs);
    return *this;
  }

  ~PString()
  {
    // as we are not free'ing the string data, do the ptr invalidation ourselves
    // so we can catch any use of discarded string data
    m_text = (char*)0xDDDDDDDD;
    m_storeLength = m_textLength = 0;
  }

  inline void clear()
  {
    m_textLength = 0;
    if (m_text && m_storeLength >= 1)
      m_text[0] = '\0';

    m_hashA = PSTRING_HASH_SEED_A_ZEROMIX;
    m_hashB = PSTRING_HASH_SEED_B_ZEROMIX;
  }

  inline void set(const char* text)
  {
    setText(text);
  }

  inline void set(const char* text, unsigned int length)
  {
    setText(text, length);
  }

  inline const char* c_str() const
  {
    return (m_text==0)?"":m_text;
  }

  // remove 'amount' characters from the end of the string;
  // if 'amount' is longer than the string, it just trims to 
  // a zero-length, empty string
  inline void trimEnd(unsigned int amount)
  {
    if (amount == 0 || 
        m_textLength == 0 || 
        m_text == 0)  // nothing to do?
      return;

    // if we are trimming more than we've got, it simply
    // zeros out the string
    if (amount >= m_textLength)
    {
      m_text[0] = '\0';
      m_textLength = 0;
    }
    else
    {
      // trim and re-terminate
      m_textLength -= amount;
      m_text[m_textLength] = '\0';
    }

    calcHash();
  }

  inline unsigned int length() const 
  {
    return m_textLength;
  }

  inline unsigned int hashValueA() const 
  {
    return m_hashA;
  }

  inline unsigned int hashValueB() const 
  {
    return m_hashB;
  }

  inline bool empty() const
  {
    return (m_text == 0 || length() == 0);
  }

  inline bool operator == (const PString& rhs) const
  {
    bool hashResult = (m_hashA == rhs.m_hashA) && (m_hashB == rhs.m_hashB);
#ifdef _DEBUG
    // sanity check that we are not providing a false-positive on hash value comparison
    bool exhaustive = ( 
      (m_text == 0 && rhs.m_text == 0) ||       // both strings are empty OR
      (m_text != 0 && rhs.m_text != 0 &&        // both strings have contents...
          strcmp(rhs.m_text, m_text) == 0) );   // ...and the contents match
    assert(exhaustive == hashResult);
#endif // _DEBUG
    return hashResult;
  }

  inline bool operator != (const PString& rhs) const
  {
    return (m_hashA != rhs.m_hashA) || (m_hashB != rhs.m_hashB);
  }

protected:

  __forceinline unsigned int align4(unsigned int v)
  {
    return ((v) + 3) & ~3;
  }

  inline void resetInternal()
  {
    m_text = 0;
    m_textLength = 0;
    m_storeLength = 0;
    m_hashA = PSTRING_HASH_SEED_A_ZEROMIX;
    m_hashB = PSTRING_HASH_SEED_B_ZEROMIX;
  }

  inline void adjustBuffer(unsigned int newSize)
  {
    if (m_storeLength >= newSize)
      return;

    assert(ActiveMemoryManager::get());

    if (m_text)
    {
      char* copyBuf = (char*)ActiveMemoryManager::get()->alloc(newSize * sizeof(char));
      memcpy(copyBuf, m_text, m_storeLength);
      m_text = copyBuf;
    }
    else
    {
      m_text = (char*)ActiveMemoryManager::get()->alloc(newSize * sizeof(char));
    }

    m_storeLength = newSize;
  }

  inline void setText(const char* text, unsigned int length)
  {
    if (text == 0 || text[0] == '\0')
    {
      m_textLength = 0;
      if (m_text && m_storeLength > 0)
        m_text[0] = '\0';
    }
    else
    {
      m_textLength = length;
      unsigned int roundedTextLength = align4(m_textLength + 1); // add terminator, round up to nearest 4-byte boundary
      adjustBuffer(roundedTextLength); 
      assert(m_text);

      // zero out any end-padding before copying in text
      memset(&m_text[m_textLength], 0, roundedTextLength - m_textLength); // lint !e613
      memcpy(m_text, text, m_textLength);
    }

    calcHash();
  }

  inline void setText(const char* text)
  {
    setText(text, text && text[0] != '\0' ? strlen(text) : 0 );
  }

  inline void calcHash()
  {
    unsigned int roundedTextLength = 0;
    if (m_text != 0 && m_text[0] != '\0')
    {
      roundedTextLength = align4(m_textLength + 1);
    }

    // generate the hash
    unsigned int * __restrict iText = reinterpret_cast<unsigned int*>(m_text);
    unsigned int textLengthInInts = (roundedTextLength >> 2);

    // unrolled interleaved SuperFastHash, by Paul Hsieh
    // check nmtl/hashfunc.h
    {
      unsigned int key, tmpA, tmpB;
      m_hashA = PSTRING_HASH_SEED_A ^ textLengthInInts;
      m_hashB = PSTRING_HASH_SEED_B ^ textLengthInInts;
      for (unsigned int i=0; i<textLengthInInts; i++)
      {
        key = (*iText);
        m_hashA += key & 0x0000FFFF;
        tmpA   = ( (key & 0xFFFF0000) >> 5 ) ^ m_hashA;
        tmpB   = ( (key & 0xFFFF0000) >> 5 ) ^ m_hashB;
        iText ++;

        m_hashA  = (m_hashA << 16) ^ tmpA;
        m_hashB  = (m_hashB << 16) ^ tmpB;
        m_hashA += m_hashA >> 11;
        m_hashB += m_hashB >> 11;
      }

      m_hashA ^= m_hashA << 3;
      m_hashB ^= m_hashB << 3;
      m_hashA += m_hashA >> 5;
      m_hashB += m_hashB >> 5;
      m_hashA ^= m_hashA << 4;
      m_hashB ^= m_hashB << 4;
      m_hashA += m_hashA >> 17;
      m_hashB += m_hashB >> 17;
      m_hashA ^= m_hashA << 25;
      m_hashB ^= m_hashB << 25;
      m_hashA += m_hashA >> 6;
      m_hashB += m_hashB >> 6;
    }
#ifdef _DEBUG
    // catch if hashing an empty string doesn't 
    // result in the precomputed zeromix values
    if (m_text == 0 || m_text[0] == '\0')
    {
      assert(m_hashA == PSTRING_HASH_SEED_A_ZEROMIX);
      assert(m_hashB == PSTRING_HASH_SEED_B_ZEROMIX);
    }
#endif // _DEBUG
  }

  inline void copyFrom(const PString& rhs)
  {
    if (rhs.empty())
    {
      clear();
    }
    else
    {
      m_textLength = rhs.m_textLength;
      adjustBuffer(rhs.m_storeLength);

      memcpy(m_text, rhs.m_text, rhs.m_storeLength);
      m_hashA = rhs.m_hashA;
      m_hashB = rhs.m_hashB;

#ifdef _DEBUG
      // sanity check; ensure copied hashes would match
      // a freshly computed pair
      unsigned int cacheA = m_hashA;
      unsigned int cacheB = m_hashB;
      calcHash();
      assert(cacheA == m_hashA);
      assert(cacheB == m_hashB);
#endif // _DEBUG
    }
  }

  char         *m_text;
  unsigned int  m_textLength,   // length in bytes of string text
                m_storeLength,  // size in bytes of storage buffer m_text
                m_hashA,        // two hashes of the m_text value, to try and reduce false-positive matches
                m_hashB;        //    where two different strings can hash to the same value
};


//----------------------------------------------------------------------------------------------------------------------
// string container typedefs
typedef nmtl::vector<PString>     PStrings;
typedef FixedStack<PString, 16>   PStringStack16;
typedef FixedStack<PString, 32>   PStringStack32;
typedef FixedStack<PString, 64>   PStringStack64;
