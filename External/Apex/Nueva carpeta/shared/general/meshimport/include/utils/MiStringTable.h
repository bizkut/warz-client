// This code contains NVIDIA Confidential Information and is disclosed to you 
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and 
// any modifications thereto. Any use, reproduction, disclosure, or 
// distribution of this software and related documentation without an express 
// license agreement from NVIDIA Corporation is strictly prohibited.
// 
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2012 NVIDIA Corporation. All rights reserved.

#ifndef MI_STRING_TABLE_H

#define MI_STRING_TABLE_H

#pragma warning( push )
#pragma warning( disable: 4555 ) // expression has no effect; expected expression with side-effect
#pragma warning( pop )

#include <assert.h>
#include "MiPlatformConfig.h"

//********** String Table
#pragma warning(push)
#pragma warning(disable:4996)

namespace mimp
{

class StringSort
{
public:
	bool operator()(const char *str1,const char *str2) const
	{
		return MESH_IMPORT_STRING::stricmp(str1,str2) < 0;
	}
};

typedef STDNAME::map< const char *, const char *, StringSort> StringMap;

class StringTable : public mimp::MeshImportAllocated
{
public:

	StringTable(void)
	{
	};

	~StringTable(void)
	{
		release();
	}

	void release(void)
	{
		for (StringMap::iterator i=mStrings.begin(); i!=mStrings.end(); ++i)
        {
            const char *string = (*i).second;
			MI_FREE( (void *)string );
        }
        mStrings.clear();
	}

	const char * Get(const char *str,bool &first)
	{
		str = str ? str : "";
		const char *ret;
		StringMap::iterator found = mStrings.find(str);
		if ( found == mStrings.end() )
		{
			MiU32 slen = (MiU32)strlen(str);
			char *string = (char *)MI_ALLOC(slen+1);
			strcpy(string,str);
			mStrings[string] = string;
			ret = string;
			first = true;
		}
		else
		{
			first = false;
			ret = (*found).second;
		}
        return ret;
	};


private:
    StringMap       mStrings;
	char				mScratch[512];
};

class StringTableInt
{
public:
	typedef STDNAME::map< size_t, MiU32 > StringIntMap;

	bool Get(const char *str,MiU32 &v)
	{
		bool ret = false;
		bool first;
		str = mStringTable.Get(str,first);
		size_t index = (size_t)str;

		StringIntMap::iterator found = mStringInt.find(index);
		if ( found != mStringInt.end() )
		{
			v = (*found).second;
			ret = true;
		}
		return ret;
	}

	MiU32 Get(const char *str)
	{
		MiU32 ret=0;
		Get(str,ret);
		return ret;
	}

	void Add(const char *str,MiU32 v)
	{
		bool first;
		str = mStringTable.Get(str,first);
		size_t index = (size_t)str;
		StringIntMap::iterator found = mStringInt.find(index);
		if ( found != mStringInt.end() )
		{
			assert(0);
		}
		else
		{
			mStringInt[index] = v;
		}
	}

private:
	StringTable		mStringTable;
	StringIntMap	mStringInt;
};

}; // end of namespace

#pragma warning(pop)

#endif
