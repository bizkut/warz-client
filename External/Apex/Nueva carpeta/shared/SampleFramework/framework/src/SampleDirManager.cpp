/*
 * Copyright 2008-2012 NVIDIA Corporation.  All rights reserved.
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
#include "SampleDirManager.h"
#include "SampleAssetManager.h"
#include "SamplePlatform.h"
#include "PsUtilities.h"

#include <stdio.h>
#include <string.h>

using namespace SampleFramework;

static void FixPathSeparator(char* pathBuffer)
{
	char separator = SampleFramework::SamplePlatform::platform()->getPathSeparator()[0];

	const size_t length = strlen(pathBuffer);
	for(size_t i=0; i<length ; ++i)
	{
		if ('/' == pathBuffer[i] || '\\' == pathBuffer[i])
			pathBuffer[i] = separator;
	}
}

SampleDirManager::SampleDirManager(const char* relativePathRoot, bool isReadOnly, int maxRecursion) : mIsReadOnly(isReadOnly)
{
	if(!relativePathRoot || !SampleFramework::searchForPath(relativePathRoot, mPathRoot, PX_ARRAY_SIZE(mPathRoot), isReadOnly, maxRecursion))
	{
		printf("path \"%s\" not found\n", relativePathRoot);
		mPathRoot[0] = '\0';
	}
}

#define MAX_PATH_LENGTH 256

const char* SampleDirManager::getFilePath(const char* relativeFilePath, char* pathBuffer, bool testFileValidity)
{
	PX_ASSERT(pathBuffer);

	bool flattenRelativeFilePaths = false;
#if defined (RENDERER_IOS)
	//iOS doesn't allow to share files in folders...
	flattenRelativeFilePaths = !mIsReadOnly;
#endif
	
	strcpy(pathBuffer, getPathRoot());
	strcat(pathBuffer, "/");
	
	if (flattenRelativeFilePaths)
	{
		char flattendPath[MAX_PATH_LENGTH];
		strcpy(flattendPath, relativeFilePath);
		for (size_t i = 0; i < strlen(flattendPath); ++i)
		{
			if ('/' == flattendPath[i] || '\\' == flattendPath[i])
				flattendPath[i] = '_';
		}
		strcat(pathBuffer, flattendPath);
	}
	else
	{
		strcat(pathBuffer, relativeFilePath);
		if (!mIsReadOnly)
		{
			FixPathSeparator(pathBuffer);
			//strip file from path and make sure the output directory exists
			const char* ptr = strrchr(pathBuffer, '/');
			if (!ptr)
				ptr = strrchr(pathBuffer, '\\');
			
			if (ptr)
			{
				char dir[MAX_PATH_LENGTH];
				assert(MAX_PATH_LENGTH >= strlen(pathBuffer));
				strcpy(dir, pathBuffer);
				dir[ptr - pathBuffer] = '\0';
				FixPathSeparator(dir);
				SampleFramework::SamplePlatform::platform()->makeSureDirectoryPathExists(dir);
			}
		}
	}

	FixPathSeparator(pathBuffer);

	if(testFileValidity)
	{
		FILE* fp = fopen(pathBuffer, "rb");
		if(!fp)
		{
			// Kai: user can still get full path in the path buffer (side effect)
			return NULL;
		}
		fclose(fp);
	}

	return pathBuffer;
}
