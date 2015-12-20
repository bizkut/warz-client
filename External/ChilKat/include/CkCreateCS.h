// CkCreateCS.h: interface for the CkCreateCS class.
//
//////////////////////////////////////////////////////////////////////
#if defined(WIN32) && !defined(__MINGW32__)

#ifndef _CKCREATECS_H
#define _CKCREATECS_H




class CkCertStore;
#include "CkString.h"
#include "CkMultiByteBase.h"

/*
    IMPORTANT: Objects returned by methods as non-const pointers must be deleted
    by the calling application. 

  */

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkCreateCS
class CkCreateCS  : public CkMultiByteBase
{
    private:
	// Don't allow assignment or copying these objects.
	CkCreateCS(const CkCreateCS &) { } 
	CkCreateCS &operator=(const CkCreateCS &) { return *this; }
	CkCreateCS(void *impl);

    public:

	CkCreateCS();
	virtual ~CkCreateCS();

	// BEGIN PUBLIC INTERFACE

	bool get_ReadOnly(void);
	void put_ReadOnly(bool newVal);

	CkCertStore *CreateRegistryStore(const char *regRoot, const char *regPath);
	CkCertStore *OpenRegistryStore(const char *regRoot, const char *regPath);
	CkCertStore *CreateFileStore(const char *path);
	CkCertStore *OpenFileStore(const char *path);
	CkCertStore *CreateMemoryStore(void);
	CkCertStore *OpenChilkatStore(void);
	CkCertStore *OpenOutlookStore(void);
	CkCertStore *OpenLocalSystemStore(void);
	CkCertStore *OpenCurrentUserStore(void);

	// CREATECS_INSERT_POINT

	// END PUBLIC INTERFACE



};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif

#endif
