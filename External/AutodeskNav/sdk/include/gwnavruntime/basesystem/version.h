/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// authors: GUAL
#ifndef Navigation_Version_H
#define Navigation_Version_H

#include "gwnavruntime/base/types.h"
#include "gwnavruntime/kernel/SF_String.h"


namespace Kaim
{

/// This class provides information about Gameware Navigation version.
class Version
{
public:
	static const char* GetVersionName(); // Major.Minor.Patch[-Specific]
	static const char* GetVersionBuild(); // Platform BuildMode GamewareBuild|ClientBuild [Eval]
	static const char* GetFullVersion(); //< Concatenate GetVersionName() and GetVersionBuild()
	static const char* GetBuildDate();

	static KyUInt32 GetMajorVersion();
	static KyUInt32 GetMinorVersion();
	static KyUInt32 GetPatchVersion();
	static const char* GetSpecificVersion();
	
	static bool HasSourceCode();
	static bool IsGamewareBuild();

	static bool GetFromFullVersionString(Version& version, const char* fullVersionString);

public:
	Version();

private:
	void Invalidate();

public:
	KyUInt32 m_majorVersion;
	KyUInt32 m_minorVersion;
	KyUInt32 m_patchVersion;
	String m_specificVersion;
	String m_platform;
	String m_buildMode;
	bool m_isGamewareBuild;
	bool m_hasSourceCode;

public: // internal
	static void RegisterVersion();
};

}

#endif

