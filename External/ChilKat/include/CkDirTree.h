// CkDirTree.h: interface for the CkDirTree class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CkDirTree_H
#define _CkDirTree_H



#include "CkString.h"
#include "CkMultiByteBase.h"

#ifndef WIN32
#include "int64.h"
#endif

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkDirTree
class CkDirTree  : public CkMultiByteBase
{
    private:
	// Don't allow assignment or copying these objects.
	CkDirTree(const CkDirTree &);
	CkDirTree &operator=(const CkDirTree &);


    public:
	CkDirTree(void *impl);

	CkDirTree();
	virtual ~CkDirTree();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE


	// BASEDIR_BEGIN
	void get_BaseDir(CkString &str);
	const char *baseDir(void);
	void put_BaseDir(const char *newVal);
	// BASEDIR_END
	// RECURSE_BEGIN
	bool get_Recurse(void);
	void put_Recurse(bool newVal);
	// RECURSE_END
	// DONEITERATING_BEGIN
	bool get_DoneIterating(void);
	// DONEITERATING_END
	// BEGINITERATE_BEGIN
	bool BeginIterate(void);
	// BEGINITERATE_END
	// ADVANCEPOSITION_BEGIN
	bool AdvancePosition(void);
	// ADVANCEPOSITION_END
	// ISDIRECTORY_BEGIN
	bool get_IsDirectory(void);
	// ISDIRECTORY_END
	// RELATIVEPATH_BEGIN
	void get_RelativePath(CkString &str);
	const char *relativePath(void);
	// RELATIVEPATH_END
	// FULLPATH_BEGIN
	void get_FullPath(CkString &str);
	const char *fullPath(void);
	// FULLPATH_END
	// FULLUNCPATH_BEGIN
	void get_FullUncPath(CkString &str);
	const char *fullUncPath(void);
	// FULLUNCPATH_END
	// FILESIZE32_BEGIN
	int get_FileSize32(void);
	// FILESIZE32_END
	// FILESIZE64_BEGIN
	__int64 get_FileSize64(void);
	// FILESIZE64_END

	// DIRTREE_INSERT_POINT

	// END PUBLIC INTERFACE



};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif


