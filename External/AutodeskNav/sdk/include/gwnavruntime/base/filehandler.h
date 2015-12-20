/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_Filehandler_H
#define Navigation_Filehandler_H


#include "gwnavruntime/base/types.h"
#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/base/fileopener.h"


namespace Kaim
{

class File;
class FileOpenerBase;


class FileHandler
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_CLASS_WITHOUT_COPY(FileHandler)
public:
	enum ErrorReportMode { NO_ERROR_REPORT = 0, DO_REPORT_REPORT};

	FileHandler() : m_file(KY_NULL) {}

	File* OpenForRead(const char* fileName, FileOpenerBase* fileOpener = KY_NULL, ErrorReportMode errorReportMode = DO_REPORT_REPORT);
	File* OpenForWrite(const char* fileName, FileOpenerBase* fileOpener = KY_NULL, ErrorReportMode errorReportMode = DO_REPORT_REPORT);

	File* GetFile() { return m_file.GetPtr(); }

	~FileHandler() { Close(); }

	void Close();

private:
	void Open(const char* fileName, FileOpenerBase* fileOpener, FileOpenerMode openMode, ErrorReportMode errorReportMode);

private:
	Ptr<File> m_file;
};

}

#endif
