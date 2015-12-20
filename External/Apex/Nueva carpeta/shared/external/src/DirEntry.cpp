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

#include <PsString.h>

#include "DirEntry.h"

namespace physx
{

DirEntry::DirEntry()
{
#if defined PX_PS3 || defined PX_LINUX || defined PX_ANDROID
	mDir = NULL;
	mEntry = NULL;
	mIdx = 0;
	mCount = 0;
#endif
}


DirEntry::~DirEntry()
{
	if (!isDone())
	{
		while (next());
	}

	// The Find.cpp loop behaves badly and doesn't cleanup the DIR pointer
#if defined PX_PS3 || defined PX_LINUX || defined PX_ANDROID
	if (mDir)
	{
		closedir(mDir);
	}
#endif
}

bool DirEntry::GetFirstEntry(const char* path, DirEntry& dentry)
{
#if defined PX_WINDOWS || defined PX_X360
	char tmp[256];

	if (2 + (PxI32)::strlen(path) > physx::string::sprintf_s(tmp, sizeof(tmp), "%s\\*", path))
	{
		return false;
	}

	// Fix slashes on xbox?

	dentry.mFindHandle = FindFirstFile(tmp, &dentry.mFindData);
	dentry.mIsDone = INVALID_HANDLE_VALUE == dentry.mFindHandle;
#elif defined PX_PS3 || defined PX_LINUX || defined PX_ANDROID 
	dentry.mDir = opendir(path);
	if (!dentry.mDir)
	{
		return false;
	}

	dentry.mIdx = 0;
#if defined PX_PS3
	dentry.mCount = dentry.mDir->count;
	dentry.mEntry = 0 == dentry.mCount ? NULL : readdir(dentry.mDir);
#else
	// count files
	dentry.mCount = 0;
  	if (dentry.mDir != NULL)
  	{
    	while (readdir(dentry.mDir))
    	{
			dentry.mCount++;
		}
	}
	closedir(dentry.mDir);
	dentry.mDir = opendir(path);
	dentry.mEntry = readdir(dentry.mDir);
#endif
#endif

	return true;
}

const char* DirEntry::getName() const
{
#if defined PX_WINDOWS || defined PX_X360
	return mFindData.cFileName;
#elif defined PX_PS3 || defined PX_LINUX || defined PX_ANDROID
	if (mEntry)
	{
		return mEntry->d_name;
	}
	else
	{
		return NULL;
	}
#else
	return NULL;
#endif
}

bool DirEntry::next()
{
#if defined PX_WINDOWS || defined PX_X360
	if (mIsDone)
	{
		return false;
	} 
	else if (!FindNextFile(mFindHandle, &mFindData))
	{
		mIsDone = true;

		// No && !
		return (ERROR_NO_MORE_FILES == GetLastError())
			& (TRUE != FindClose(mFindHandle));
	}
#elif defined PX_PS3 || defined PX_LINUX || defined PX_ANDROID
	if (mIdx < mCount)
	{
		mEntry = readdir(mDir);
		++mIdx;
	}
	else
	{
		bool ret = (0 == closedir(mDir));
		mDir = NULL;
		mEntry = NULL;
		return ret;
	}
#endif

	return true;
}

bool DirEntry::isDone() const
{
#if defined PX_WINDOWS || defined PX_X360
	return mIsDone;
#elif defined PX_PS3 || defined PX_LINUX || defined PX_ANDROID
	return mIdx >= mCount;
#endif
}

bool DirEntry::isDirectory() const
{
#if defined PX_WINDOWS || defined PX_X360
	return 0 != (mFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
#elif defined PX_PS3 || defined PX_LINUX || defined PX_ANDROID
	if (mEntry)
	{
		return DT_DIR == mEntry->d_type;
	}
	else
	{
		return false;
	}
#endif
}

}
