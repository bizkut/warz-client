// CkBz2.h: interface for the CkBz2 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CkBz2_H
#define _CkBz2_H



#include "CkString.h"
class CkByteData;

/*
    IMPORTANT: Objects returned by methods as non-const pointers must be deleted
    by the calling application. 

  */

#include "CkMultiByteBase.h"

class CkBz2Progress;

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkBz2
class CkBz2  : public CkMultiByteBase
{
    private:
	CkBz2Progress *m_callback;

	// Don't allow assignment or copying these objects.
	CkBz2(const CkBz2 &) { } 
	CkBz2 &operator=(const CkBz2 &) { return *this; }
	CkBz2(void *impl);

    public:

	CkBz2();
	virtual ~CkBz2();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);


	// BEGIN PUBLIC INTERFACE
	
	CkBz2Progress *get_EventCallbackObject(void) const;
	void put_EventCallbackObject(CkBz2Progress *progress);

	// UNCOMPRESSFILE_BEGIN
	// eventCallbacks
	bool UncompressFile(const char *inPath, const char *toPath);
	// UNCOMPRESSFILE_END
	// COMPRESSFILE_BEGIN
	// eventCallbacks
	bool CompressFile(const char *inPath, const char *toPath);
	// COMPRESSFILE_END
	// UNCOMPRESSFILETOMEM_BEGIN
	// eventCallbacks
	bool UncompressFileToMem(const char *inPath, CkByteData &outBytes);
	// UNCOMPRESSFILETOMEM_END
	// COMPRESSFILETOMEM_BEGIN
	// eventCallbacks
	bool CompressFileToMem(const char *inPath, CkByteData &outBytes);
	// COMPRESSFILETOMEM_END
	// COMPRESSMEMTOFILE_BEGIN
	// eventCallbacks
	bool CompressMemToFile(const CkByteData &inData, const char *toPath);
	// COMPRESSMEMTOFILE_END
	// UNCOMPRESSMEMTOFILE_BEGIN
	// eventCallbacks
	bool UncompressMemToFile(const CkByteData &inData, const char *toPath);
	// UNCOMPRESSMEMTOFILE_END
	// COMPRESSMEMORY_BEGIN
	// eventCallbacks
	bool CompressMemory(const CkByteData &inData, CkByteData &outBytes);
	// COMPRESSMEMORY_END
	// UNCOMPRESSMEMORY_BEGIN
	// eventCallbacks
	bool UncompressMemory(const CkByteData &inData, CkByteData &outBytes);
	// UNCOMPRESSMEMORY_END
	// UNLOCKCOMPONENT_BEGIN
	bool UnlockComponent(const char *unlockCode);
	// UNLOCKCOMPONENT_END

	// BZ2_INSERT_POINT

	// END PUBLIC INTERFACE

	


};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif


