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

#ifndef STRINGDICT_H
#define STRINGDICT_H

#include "PsShare.h"
#include "PsUserAllocated.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "stable.h"

namespace physx
{
	namespace general_string_parsing2
	{

extern const char *emptystring;

class StringRef : public UserAllocated
{
public:
	StringRef(void)
	{
		mString = "";
	}

	inline StringRef(const StringRef &str);

	operator const char *() const
	{
		return mString;
	}

	const char * Get(void) const { return mString; };

	void Set(const char *str)
	{
		mString = str;
	}

	const StringRef &operator= (const StringRef& rhs )
	{
		mString = rhs.Get();
		return *this;
	}

	bool operator== ( const StringRef& rhs ) const
	{
		return rhs.mString == mString;
	}

	bool operator< ( const StringRef& rhs ) const
	{
		return rhs.mString < mString;
	}

	bool operator!= ( const StringRef& rhs ) const
	{
		return rhs.mString != mString;
	}

	bool operator> ( const StringRef& rhs ) const
	{
		return rhs.mString > mString;
	}

	bool operator<= ( const StringRef& rhs ) const
	{
		return rhs.mString <= mString;
	}

	bool operator>= ( const StringRef& rhs ) const
	{
		return rhs.mString >= mString;
	}

	bool SamePrefix(const char *prefix) const
	{
		PxU32 len = (PxU32)strlen(prefix);
		if ( len && strncmp(mString,prefix,len) == 0 ) return true;
		return false;
	}

	bool SameSuffix(const StringRef &suf) const
	{
		const char *source = mString;
		const char *suffix = suf.mString;
		PxU32 len1 = (PxU32)strlen(source);
		PxU32 len2 = (PxU32)strlen(suffix);
		if ( len1 < len2 ) return false;
		const char *compare = &source[(len1-len2)];
		if ( strcmp(compare,suffix) == 0 ) return true;
		return false;
	}

#ifdef PX_X64
  PxU64 getHash(void) const
  {
    return (PxU64) mString;
  }
#else
#pragma warning(push)
#pragma warning(disable:4311)
#pragma warning(disable:4302)
  PxU32 getHash(void) const
  {
    return (PxU32)mString;
  }
#pragma warning(pop)
#endif

private:
	const char *mString; // the actual char ptr
};


class StringDict : public UserAllocated
{
public:
	StringDict(void)
	{
	}

	~StringDict(void)
	{
	}

	StringRef Get(const char *text)
	{
		StringRef ref;
		if ( text )
		{
			if ( text[0] == '\0')
			{
				ref.Set(emptystring);
			}
			else
			{
				bool first;
				const char *foo = mStringTable.Get(text,first);
				ref.Set(foo);
			}
		}
		return ref;
	}

	StringRef Get(const char *text,bool &first)
	{
		StringRef ref;
		const char *foo = mStringTable.Get(text,first);
		ref.Set(foo);
		return ref;
	}

  void setCaseSensitive(bool state)
  {
    mStringTable.setCaseSensitive(state);
  }

private:
	StringTable mStringTable;
};

typedef Array< StringRef  >  StringRefVector;

inline StringRef::StringRef(const StringRef &str)
{
	mString = str.Get();
}

// This is a helper class so you can easily do an alphabetical sort on an STL vector of StringRefs.
// Usage: USER_STL::sort( list.begin(), list.end(), StringSortRef() );
class StringSortRef : public UserAllocated
{
	public:

	 bool operator()(const StringRef &a,const StringRef &b) const
	 {
		 const char *str1 = a.Get();
		 const char *str2 = b.Get();
		 PxI32 r = physx::string::stricmp(str1,str2);
		 return r < 0;
	 }
};

extern StringDict *gStringDict;

};
using namespace general_string_parsing2;
};


static PX_INLINE physx::StringDict * getGlobalStringDict(void)
{
	if ( physx::gStringDict == 0 )
		physx::gStringDict = PX_NEW(physx::StringDict);
	return physx::gStringDict;
}

#define SGET(x) getGlobalStringDict()->Get(x)

#endif // STRINGDICT_H
