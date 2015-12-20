/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_CheckHeaderLibDesync_H
#define Navigation_CheckHeaderLibDesync_H



#include "gwnavruntime/basesystem/version.h"
#include "gwnavruntime/kernel/SF_Std.h"
#include "gwnavruntime/base/types.h"
#include "gwnavruntime/basesystem/versionconfig.inl"

namespace Kaim
{

// Declare a function whose implementation only exists in gwnavruntime library.
void KY_CHECK_MATCHING_VERSION_FUNCTION_NAME();

/// This class helps to finding errors in the use of header files and libraries.
class CheckHeaderLibDesync
{
public:
	/// This is called in user code thanks to BaseSystem::Init.
	/// Thus, macros processed here have the values of the user project, 
	/// and they'll be checked against the ones used when the libraries were built.
	/// Note that only the library/executable making the call to BaseSystem::Init will be checked automatically.
	/// Explicit calls to CheckHeaderLibDesync::Check() can be added into .cpp files of libraries not calling BaseSystem::Init 
	/// but that are dependent upon gwnavruntime header files, so such libraries will be checked as well.
	///
	/// So, this function helps to prevent the user project is wrongly setup,
	/// for instance, if debug libraries are used with no debug preprocessor symbol defined,
	/// an 'unresolved external symbol' link error will be raised since Check_RELEASE or Check_SHIPPING will be called 
	/// but only Check_DEBUG will be in the used library.
	/// To solve this error, please add to the preprocessor definitions of your project,
	/// either KY_BUILD_DEBUG, KY_BUILD_RELEASE or KY_BUILD_SHIPPING,
	/// accordingly to Navigation libraries you are linking against (debug, release or shipping respectively)
	///
	/// This function also ensures there's no mismatch between header files and libraries,
	/// e.g. different versions were mixed up when installing the SDK
	/// an 'unresolved external symbol' link error will be raised regarding a function named CheckVersion_major_minor_patch 
	/// where major, minor and patch are the version numbers
	KY_INLINE static void Check()
	{
		// This check that used libraries are in sync with defined macros.
		CheckBuildMode();
		
		// This ensure there's no mismatch between header files and libraries.
		CheckHeaderVsLibraryVersions();
	}


private:
	KY_INLINE static void CheckHeaderVsLibraryVersions()
	{
		KY_CHECK_MATCHING_VERSION_FUNCTION_NAME();
	}


	KY_INLINE static void CheckBuildMode()
	{
#if defined(KY_BUILD_DEBUG)
		Check_DEBUG();
#elif defined(KY_BUILD_RELEASE)
		Check_RELEASE();
#elif defined(KY_BUILD_SHIPPING)
		Check_SHIPPING();
#else
KY_COMPILE_TIME_MESSAGE("Please define KY_BUILD_DEBUG, KY_BUILD_RELEASE or KY_BUILD_SHIPPING, accordingly to the Gameware Navigation libraries you are linking against (debug, release or shipping respectively)");
KY_COMPILER_ASSERT(0);
#endif
	}

	static void Check_DEBUG();     ///< cf. CheckHeaderLibDesync::Check for more information
	static void Check_RELEASE();   ///< cf. CheckHeaderLibDesync::Check for more information
	static void Check_SHIPPING();  ///< cf. CheckHeaderLibDesync::Check for more information
	static KyUInt32 s_dummy;
};

}

#endif
