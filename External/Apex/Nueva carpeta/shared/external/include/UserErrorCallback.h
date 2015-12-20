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

#ifndef USER_ERROR_CALLBACK_H
#define USER_ERROR_CALLBACK_H

#include "foundation/PxErrorCallback.h"
#include "foundation/PxErrors.h"
#include <PsString.h>
#include <PsShare.h>

#include <map>
#include <string>
#include <vector>

class NxUserOutputStream;
class PhysX28OutputStream;

class UserErrorCallback : public physx::PxErrorCallback
{
public:
	UserErrorCallback(const char* filename, const char* mode, bool header, bool reportErrors);
	~UserErrorCallback();

	void		printError(const char* message, const char* errorCode = NULL, const char* file = NULL, int line = 0);
	void		reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line);
	void		printError(physx::PxErrorCode::Enum code, const char* file, int line, const char* fmt, ...);
	int			getNumErrors();
	void		clearErrorCounter();
	const char*	getFirstEror();
	void		addFilteredMessage(const char* msg, bool fullMatch, bool* trigger = NULL);

	void		reportErrors(bool enabled);

	NxUserOutputStream* get28PhysXOutputStream();

	static UserErrorCallback* instance()
	{
		if (!s_instance)
		{
			// Allocate a stub (bitbucket) error handler
			s_instance = ::new UserErrorCallback(NULL, NULL, false, false);
		}
		return s_instance;
	}

private:
	bool	messageFiltered(const char * code, const char * msg);
	void	openFile();

	physx::PxU32				mNumErrors;
	FILE*						mOutFile;
	std::string					mOutFileName;
	const char*					mOutFileMode;
	bool						mOutFileHeader;
	bool						mReportErrors;
	char						mFirstErrorBuffer[2048];
	PhysX28OutputStream*		m28OutputStream;

	std::map<std::string, bool*> mFilteredMessages;
	std::vector<std::pair<std::string, bool*> >	mFilteredParts;

	static UserErrorCallback* s_instance;

	friend class PhysX28OutputStream;
};

// gcc uses names ...s
#define ERRORSTREAM_INVALID_PARAMETER(_A, ...) \
	UserErrorCallback::instance()->printError(physx::PxErrorCode::eINVALID_PARAMETER, __FILE__, __LINE__, _A, ##__VA_ARGS__)
#define ERRORSTREAM_INVALID_OPERATION(_A, ...) \
	UserErrorCallback::instance()->printError(physx::PxErrorCode::eINVALID_OPERATION, __FILE__, __LINE__, _A, ##__VA_ARGS__)
#define ERRORSTREAM_DEBUG_ERROR(_A, ...) \
	UserErrorCallback::instance()->printError(physx::PxErrorCode::eINTERNAL_ERROR   , __FILE__, __LINE__, _A, ##__VA_ARGS__)
#define ERRORSTREAM_DEBUG_INFO(_A, ...) \
	UserErrorCallback::instance()->printError(physx::PxErrorCode::eDEBUG_INFO       , __FILE__, __LINE__, _A, ##__VA_ARGS__)
#define ERRORSTREAM_DEBUG_WARNING(_A, ...) \
	UserErrorCallback::instance()->printError(physx::PxErrorCode::eDEBUG_WARNING    , __FILE__, __LINE__, _A, ##__VA_ARGS__)

#endif
