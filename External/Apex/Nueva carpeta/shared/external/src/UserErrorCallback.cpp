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

#include "NxApexDefs.h"
#include "UserErrorCallback.h"
#include "foundation/PxAssert.h"
#include <PsFile.h>
#include <PsUtilities.h>
#include <algorithm>

#if defined(PX_WINDOWS)
#define NOMINMAX
#include <windows.h>
#include <cstdio>
#include <ctime>
#endif

#if defined(PX_X360)
#include <stdarg.h>
#include <cstdio>
#endif

UserErrorCallback* UserErrorCallback::s_instance /* = NULL */;

UserErrorCallback::UserErrorCallback(const char* filename, const char* mode, bool header, bool reportErrors)
	: mNumErrors(0)
	, mOutFile(NULL)
	, mOutFileName(filename == NULL ? "" : filename)
	, mOutFileMode(mode)
	, mOutFileHeader(header)
	, mReportErrors(reportErrors)
	, m28OutputStream(NULL)
{
	mFirstErrorBuffer[0] = '\0';
	if (!s_instance)
	{
		s_instance =  this;
	}
	else if (s_instance->mOutFileName.empty())
	{
		// replace stub error handler
		delete s_instance;
		s_instance = this;
	}

	// initialize the filtered messages.
	//mFilteredMessages.insert("");
	addFilteredMessage("CUDA not available", true);
	addFilteredMessage("CUDA is not available", true);
}

const char* sanitizeFileName(const char* fullPath)
{
	return std::max(::strrchr(fullPath, '\\'), ::strrchr(fullPath, '/')) + 1;
}

bool UserErrorCallback::messageFiltered(const char * code, const char * msg)
{
	if (0 == strcmp(code, "info"))
	{
		return true;
	}

	std::map<std::string, bool*>::iterator found = mFilteredMessages.find(msg);
	if (found != mFilteredMessages.end())
	{
		if (found->second != NULL)
		{
			// set the trigger
			*(found->second) = true;
		}
		return true;
	}

	for (size_t i = 0; i < mFilteredParts.size(); i++)
	{
		const char* fmsg = mFilteredParts[i].first.c_str();
		if (strstr(msg, fmsg) != NULL)
		{
			if (mFilteredParts[i].second != NULL)
			{
				// set the trigger
				*(mFilteredParts[i].second) = true;
			}
			return true;
		}
	}

	return false;
}

#if defined(PX_WINDOWS)

void logf(FILE* file, const char* format, ...)
{
	enum { BUFFER_SIZE = 512 };
	char buffer[BUFFER_SIZE];

	va_list args;
	va_start(args, format);
	vsnprintf(buffer, BUFFER_SIZE-1, format, args);
	va_end(args);

	// Output to file
	fputs(buffer, file);

	// Output to debug stream
	::OutputDebugString(buffer);
}

#else

#define logf(file, format, ...) fprintf(file, format, __VA_ARGS__)

#endif /* PX_WINDOWS */

void UserErrorCallback::printError(const char* message, const char* errorCode, const char* file, int line)
{
	if (mOutFile == NULL)
	{
		openFile();
	}

	// do not count eDEBUG_INFO "info" messages as errors.
	// errors trigger benchmarks to be exited.
	if (!messageFiltered(errorCode, message))
	{
		mNumErrors++;
	}

	// if this is the first error while running a benchmark
	if (mNumErrors == 1)
	{
		if (errorCode)
		{
			strcpy(mFirstErrorBuffer, errorCode);
			strcat(mFirstErrorBuffer, ": ");
		}
		if (file)
		{
			char lineNumBuf[20];
			strcat(mFirstErrorBuffer, sanitizeFileName(file));
			strcat(mFirstErrorBuffer, ":");
			sprintf(lineNumBuf, "%d: ", line);
			strcat(mFirstErrorBuffer, lineNumBuf);
		}
		if (message)
		{
			strcat(mFirstErrorBuffer, message);
			strcat(mFirstErrorBuffer, "\n");
		}
	}

	if (mOutFile == NULL)
	{
		return;
	}

	if (errorCode != NULL)
	{
		logf(mOutFile, "\n%s: ", errorCode);
	}

	if (file != NULL)
	{
		logf(mOutFile, "%s:%d:\n", sanitizeFileName(file), line);
	}

	if (message != NULL)
	{
		logf(mOutFile, "%s\n", message);
	}

	fflush(mOutFile);
}

int UserErrorCallback::getNumErrors(void)
{
	return(mNumErrors);
}

void UserErrorCallback::clearErrorCounter(void)
{
	mNumErrors = 0;
	mFirstErrorBuffer[0] = '\0';
}

const char* UserErrorCallback::getFirstEror(void)
{
	return(mFirstErrorBuffer);
}

void UserErrorCallback::addFilteredMessage(const char* msg, bool fullMatch, bool* trigger)
{
	if (fullMatch)
	{
		mFilteredMessages.insert(std::pair<std::string, bool*>(msg, trigger));
	}
	else
	{
		mFilteredParts.push_back(std::pair<std::string, bool*>(msg, trigger));
	}
}



void UserErrorCallback::reportErrors(bool enabled)
{
	mReportErrors = false;
}



void UserErrorCallback::reportError(physx::PxErrorCode::Enum e, const char* message, const char* file, int line)
{
	const char* errorCode = NULL;

	switch (e)
	{
	case physx::PxErrorCode::eINVALID_PARAMETER:
		errorCode = "invalid parameter";
		break;
	case physx::PxErrorCode::eINVALID_OPERATION:
		errorCode = "invalid operation";
		break;
	case physx::PxErrorCode::eOUT_OF_MEMORY:
		errorCode = "out of memory";
		break;
	case physx::PxErrorCode::eDEBUG_INFO:
		errorCode = "info";
		break;
	case physx::PxErrorCode::eDEBUG_WARNING:
		errorCode = "warning";
		break;
	default:
		errorCode = "unknown error";
		break;
	}

	PX_ASSERT(errorCode != NULL);
	if (errorCode != NULL)
	{
		printError(message, errorCode, file, line);
	}
}

void UserErrorCallback::printError(physx::PxErrorCode::Enum code, const char* file, int line, const char* fmt, ...)
{
	char buff[2048];
	va_list arg;
	va_start(arg, fmt);
	physx::string::vsprintf_s(buff, sizeof(buff), fmt, arg);
	va_end(arg);
	reportError(code, buff, file, line);
}


void UserErrorCallback::openFile()
{
	if (mOutFile != NULL)
	{
		return;
	}

	if (mOutFileName.empty())
	{
		return;
	}

	PX_ASSERT(mNumErrors == 0);

	if (mOutFileMode == NULL)
	{
		mOutFileMode = "w";
	}

	physx::fopen_s(&mOutFile, mOutFileName.c_str(), mOutFileMode);

	// if that failed, try the temp location on windows
#if defined(PX_WINDOWS)
	if (!mOutFile)
	{
		DWORD pathLen = ::GetTempPathA(0, NULL);
		if (pathLen)
		{
			char *pathStr = (char*)malloc(pathLen);
			GetTempPathA(pathLen, pathStr);
			std::string tmpPath(pathStr);
			tmpPath.append(mOutFileName);
			mOutFileName = tmpPath;
			free(pathStr);
			
			physx::fopen_s(&mOutFile, mOutFileName.c_str(), mOutFileMode);
		}
	}
#endif

	if (mOutFile && mOutFileHeader)
	{
		fprintf(mOutFile,
		        "\n\n"
		        "-------------------------------------\n"
		        "-- new error stream\n");
#if defined(PX_WINDOWS)
		char timeBuf[30];
		time_t rawTime;
		time(&rawTime);
		ctime_s(timeBuf, sizeof(timeBuf), &rawTime);
		fprintf(mOutFile,
		        "--\n"
		        "-- %s", timeBuf);
#endif
		fprintf(mOutFile,
		        "-------------------------------------\n\n");

		fflush(mOutFile);
	}
}


#if NX_SDK_VERSION_MAJOR == 2

#include "NxUserOutputStream.h"

class PhysX28OutputStream : public NxUserOutputStream
{
public:
	PhysX28OutputStream(UserErrorCallback& impl) : mImpl(impl) {}
	~PhysX28OutputStream() {}

	void reportError(NxErrorCode e, const char* message, const char* file, int line);
	NxAssertResponse reportAssertViolation(const char* message, const char* file, int line);
	void print(const char* message);

private:
	UserErrorCallback& mImpl;
};

void PhysX28OutputStream::reportError(NxErrorCode e, const char* message, const char* file, int line)
{
	const char* errorCode = NULL;
	switch (e)
	{
	case NXE_INVALID_PARAMETER:
		errorCode = "invalid parameter";
		break;
	case NXE_INVALID_OPERATION:
		errorCode = "invalid operation";
		break;
	case NXE_OUT_OF_MEMORY:
		errorCode = "out of memory";
		break;
	case NXE_DB_INFO:
		errorCode = "info";
		break;
	case NXE_DB_WARNING:
		errorCode = "warning";
		break;
	default:
		errorCode = "unknown error";
		break;
	}

	PX_ASSERT(errorCode != NULL);
	if (errorCode != NULL)
	{
		mImpl.printError(message, errorCode, file, line);
	}
}

NxAssertResponse PhysX28OutputStream::reportAssertViolation(const char* message, const char* file, int line)
{
	mImpl.printError(message, "Assert Violation", file, line);

#ifdef _DEBUG
	return NX_AR_BREAKPOINT;
#else
	return NX_AR_CONTINUE;
#endif
}

void PhysX28OutputStream::print(const char* message)
{
	mImpl.printError(message, NULL, NULL, 0);
}

NxUserOutputStream* UserErrorCallback::get28PhysXOutputStream()
{
	if (!m28OutputStream)
	{
		m28OutputStream = ::new PhysX28OutputStream(*this);
	}
	return m28OutputStream;
}

#else /* not PhysX 2.x */

NxUserOutputStream* UserErrorCallback::get28PhysXOutputStream()
{
	return NULL;
}

#endif

UserErrorCallback::~UserErrorCallback()
{
#if NX_SDK_VERSION_MAJOR == 2
	if (m28OutputStream)
	{
		::delete m28OutputStream;
	}
#endif

#if defined(PX_WINDOWS)
	if (mNumErrors > 0 && mReportErrors)
	{
		std::string errorString;
		char buf[64];
		physx::string::sprintf_s(buf, sizeof(buf), "The error callback captured %d errors", mNumErrors);

		errorString.append(buf);
		if (mOutFile != stdout && mOutFile != 0)
		{
			errorString.append(" in ");
			errorString.append(mOutFileName);
		}

		::MessageBoxA(NULL, errorString.c_str(), "UserErrorCallback", MB_OK);
	}
#else
	PX_ASSERT(mNumErrors == 0);
#endif

	if (mOutFile != stdout && mOutFile != 0)
	{
		fclose(mOutFile);
		mOutFile = 0;
	}
}
