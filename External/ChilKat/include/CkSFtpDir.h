// CkSFtpDir.h: interface for the CkSFtpDir class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CkSFtpDir_H
#define _CkSFtpDir_H



#include "CkString.h"
#include "CkMultiByteBase.h"

class CkSFtpFile;

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkSFtpDir
class CkSFtpDir  : public CkMultiByteBase
{
    private:
	// Don't allow assignment or copying these objects.
	CkSFtpDir(const CkSFtpDir &);
	CkSFtpDir &operator=(const CkSFtpDir &);


    public:
	CkSFtpDir(void *impl);

	CkSFtpDir();
	virtual ~CkSFtpDir();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE

		// NUMFILESANDDIRS_BEGIN
	int get_NumFilesAndDirs(void);
	// NUMFILESANDDIRS_END
	// ORIGINALPATH_BEGIN
	void get_OriginalPath(CkString &str);
	const char *originalPath(void);
	// ORIGINALPATH_END
	// GETFILEOBJECT_BEGIN
	CkSFtpFile *GetFileObject(int index);
	// GETFILEOBJECT_END
	// GETFILENAME_BEGIN
	bool GetFilename(int index, CkString &outStr);
	const char *getFilename(int index);
	// GETFILENAME_END

	// SFTPDIR_INSERT_POINT

	// END PUBLIC INTERFACE

};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif


