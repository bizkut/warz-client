/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_GeneratorFile_H
#define GwNavGen_GeneratorFile_H


#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/kernel/SF_String.h"
#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/base/fileopener.h"


namespace Kaim
{

class GeneratorSystem;
class File;


class GeneratorFile
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_CLASS_WITHOUT_COPY(GeneratorFile)
public:
	enum MakeDir { DO_MAKE_DIR = 0, DO_NOT_MAKE_DIR = 1, MakeDir_FORCE32 = 0xFFFFFFFF };

	GeneratorFile();
	GeneratorFile(GeneratorSystem* sys, const char* fileName, const FileOpenerMode openMode, MakeDir makeDir = DO_MAKE_DIR);
	~GeneratorFile();

	void Init(GeneratorSystem* sys, const char* fileName, const FileOpenerMode openMode, MakeDir makeDir = DO_MAKE_DIR);

	enum DoErrorMessage { DO_ERROR_MESSAGE, DO_NOT_ERROR_MESSAGE };
	File* GetFile(DoErrorMessage = DO_ERROR_MESSAGE);

	void Close();

private:
	GeneratorSystem* m_sys;
	String m_fileName;
	FileOpenerMode m_openMode;
	MakeDir m_makeDir;
	Ptr<File> m_file;
};


}


#endif
