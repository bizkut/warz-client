/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: MAMU
#ifndef GwNavGen_GeneratorFileSystem_H
#define GwNavGen_GeneratorFileSystem_H


#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/kernel/SF_String.h"


namespace Kaim
{

/*
---------- Vocabulary (Absolute) ----------
FileName:        C:/MyDir1/MyDir2/MyFile.txt
FileDirectory:   C:/MyDir1/MyDir2
FileSimpleName:                   MyFile.txt

---------- Vocabulary (Relative) ----------
FileName:        MyDir1/MyDir2/MyFile.txt
FileDirectory:   MyDir1/MyDir2
FileSimpleName:                MyFile.txt
*/
class GeneratorFileSystem
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	enum FileOrDirType { NOT_FOUND, TYPE_FILE, TYPE_DIRECTORY };

public:
	void Setup(const char* absoluteOutputBaseDir, const char* relativeOutputDir);

	const String& GetAbsoluteOutputDirectory() const { return m_absoluteOutputDirectory; }

	String GetAbsoluteFileName(const char* relativeFileName) const
	{
		return ConcatToPath(m_absoluteOutputDirectory, relativeFileName);
	}

	String GetRelativeFileName(const char* relativeFileName) const
	{
		return ConcatToPath(m_relativeOutputDirectory, relativeFileName);
	}

	KyResult MakeDirectory(const char* relativeFileName) const
	{
		return MakeDirectory_Absolute(GetAbsoluteFileName(relativeFileName).ToCStr());
	}

	KyResult MakeDirectoryForFile(const char* relativeFileName) const
	{
		return MakeDirectoryForFile_Absolute(GetAbsoluteFileName(relativeFileName).ToCStr());
	}

	bool DoesFileOrDirExist(const char* relativeFileName) const
	{
		return DoesFileOrDirExist_Absolute(GetAbsoluteFileName(relativeFileName).ToCStr());
	}

	FileOrDirType GetFileOrDirType(const char* relativeFileOrDirName) const
	{
		return GetFileOrDirType_Absolute(GetAbsoluteFileName(relativeFileOrDirName).ToCStr());
	}

public:
	KyResult RemoveFile(const char* fileName);

	// wont remove sub directories
	KyResult RemoveAllFilesInDirectory(const char* dirNameRelativeToOutputDir);

	// Will remove all the files contained in a particular dir and call rmdir afterward
	// Note that this function wont delete sub folders and rmdir will fail if the directory
	// has subdirectories... 
	KyResult RemoveDirectoryAndFilesInIt(const char* dirNameRelativeToOutputDir);

public: // works with relativeFileNames and absoluteFileNames
	static KyUInt32 GetLastSeparatorIdx(const char* fileOrDirName);
	static KyUInt32 GetLastSeparatorIdx(const String& fileOrDirName);
	static String GetContainingDirectory(const char* fileOrDirName);
	static String GetContainingDirectory(const String& fileOrDirName);
	static String GetFileSimpleName(const char* fileName);
	static String GetFileNameExtension(const char* fileName);
	static String GetFileNameWithoutExtension(const char* fileName);
	static String GetFileNameWithoutExtension(const String& fileName);
	static void RemoveTrailingSlashs(String& fileName);
	static String RemoveTrailingSlashs(const char* fileName);
	static bool IsWinDrive(const char* dirName);
	static bool IsAbsolute(const char* fileOrDirName);

public:
	static KyResult MakeDirectory_Absolute(const char* absoluteDirName);
	static KyResult MakeDirectory_Absolute(const String& absoluteDirName);
	static KyResult MakeDirectoryForFile_Absolute(const char* absoluteFileName);
	static bool DoesFileOrDirExist_Absolute(const char* absoluteFileName);
	static FileOrDirType GetFileOrDirType_Absolute(const char* absoluteFileName);
	static FileOrDirType GetFileOrDirType_Absolute(const String& absoluteFileName);
	static KyResult ListFilesInAbsoluteDirectory(const char* dirName, KyArray<String>& filesList);

private:
	String ConcatToPath(const String& basePath, const char* relativeFileName) const;

public:
	String m_absoluteOutputBaseDirectory; // absolute base directory m_relativeOutputDirectory is relative to
	String m_relativeOutputDirectory; // relative directory to be embedded into the generated data
	String m_absoluteOutputDirectory; // concatenation of absoluteOutputBaseDir passed to Setup and m_relativeOutputDirectory
};


}


#endif

