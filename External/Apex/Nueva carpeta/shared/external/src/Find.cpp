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
#include "foundation/PxAssert.h"

#include <DirEntry.h>

#include "Find.h"

namespace physx {
namespace apex {

void Find(const char* root, FileHandler& f, const char** ignoredFiles)
{
	if (!root)
		return;

	// Fix slashes
	char goodRoot[128];
	strcpy(goodRoot, root);
#if defined PX_WINDOWS || defined PX_X360
	for (char* p = goodRoot; *p; ++p)
	{
		if ('/' == *p)
			*p = '\\';
	}
#endif

	physx::DirEntry dentry;
	if (!physx::DirEntry::GetFirstEntry(goodRoot, dentry))
	{
		PX_ALWAYS_ASSERT();
		return;
	}

	for (; !dentry.isDone(); dentry.next())
	{
		const char* filename = dentry.getName();

		if (0 == strcmp(".", filename) || 0 == strcmp("..", filename))
			continue;

		bool doSkip = false;
		for (size_t i = 0; ignoredFiles && ignoredFiles[i]; ++i)
		{
			if (0 == strcmp(filename, ignoredFiles[i]))
			{
				doSkip = true;
				break;
			}
		}

		if (doSkip)
			continue;

		char tmp[128];
		physx::string::sprintf_s(tmp, sizeof(tmp), "%s/%s", goodRoot, filename);

#if defined PX_WINDOWS || defined PX_X360
	for (char* p = tmp; *p; ++p)
	{
		if ('/' == *p)
			*p = '\\';
	}
#endif

		if (dentry.isDirectory())
		{
			Find(tmp, f, ignoredFiles);
			continue;
		}

		f.handle(tmp);
	}
}

}}
