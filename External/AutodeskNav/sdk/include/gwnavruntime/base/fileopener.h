/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: JAPA - secondary contact: GUAL

// ***** FileOpenerBase

// A callback interface that is used for opening files.
// Must return either an open file or 0 value for failure.
// The caller of the function is responsible for Releasing
// the File interface.

#ifndef Navigation_FileOpener_H
#define Navigation_FileOpener_H

#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/kernel/SF_File.h"
#include "gwnavruntime/kernel/SF_SysFile.h"

namespace Kaim
{

/// Enumerates the possible modes for opening a file with a class that derives from FileOpenerBase.
enum FileOpenerMode
{
	OpenMode_Read = 0, ///< Opens the file for reading only.
	OpenMode_Write     ///< Opens the file for writing.
};

/// Base interface for a class that opens a file on disk.
class FileOpenerBase : public NewOverrideBase<Stat_Default_Mem>
{
public:
	virtual ~FileOpenerBase() {}

	int GetFlagsFromMode(FileOpenerMode mode)
	{
		if (mode == OpenMode_Read)
			return FileConstants::Open_Read | FileConstants::Open_Buffered;

		else if (mode == OpenMode_Write)
			return FileConstants::Open_Write | FileConstants::Open_Truncate | FileConstants::Open_Create | FileConstants::Open_Buffered;
		
		else
			return 0;
	}

	/// Override to open a file using user-defined function and/or File class.
	virtual Ptr<File> OpenFile(const char* filename, FileOpenerMode mode) = 0;
};


/// Simple default implementation of an object that opens a file on disk.
class DefaultFileOpener : public FileOpenerBase
{
public:
	/// Override to open a file using user-defined function and/or File class.
	/// The default implementation uses buffer-wrapped SysFile.
	virtual Ptr<File> OpenFile(const char* filename, FileOpenerMode mode)
	{
		Ptr<File> file = *KY_NEW Kaim::SysFile(filename, GetFlagsFromMode(mode), FileConstants::Mode_ReadWrite);
		if (file->IsValid() == false)
			return NULL;
		return file;
	}
};

}

#endif //Navigation_FileOpener_H
