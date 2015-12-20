// CkDh.h: interface for the CkDh class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKDH_H
#define _CKDH_H



#include "CkString.h"
class CkByteData;

/*
    IMPORTANT: Objects returned by methods as non-const pointers must be deleted
    by the calling application. 

  */

#include "CkMultiByteBase.h"

class CkDhProgress;

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkDh
class CkDh  : public CkMultiByteBase
{
    private:
	CkDhProgress *m_callback;

	// Don't allow assignment or copying these objects.
	CkDh(const CkDh &) { } 
	CkDh &operator=(const CkDh &) { return *this; }
	CkDh(void *impl);

    public:

	CkDh();
	virtual ~CkDh();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);


	// BEGIN PUBLIC INTERFACE
	
	CkDhProgress *get_EventCallbackObject(void) const;
	void put_EventCallbackObject(CkDhProgress *progress);


	void UseKnownPrime(int index);
	// G_BEGIN
	int get_G(void);
	// G_END
	// P_BEGIN
	void get_P(CkString &str);
	const char *p(void);
	// P_END
	// SETPG_BEGIN
	bool SetPG(const char *p, int g);
	// SETPG_END
	// CREATEE_BEGIN
	bool CreateE(int numBits, CkString &outStr);
	const char *createE(int numBits);
	// CREATEE_END
	// FINDK_BEGIN
	bool FindK(const char *e, CkString &outStr);
	const char *findK(const char *e);
	// FINDK_END
	// GENPG_BEGIN
	bool GenPG(int numBits, int g);
	// GENPG_END
	// UNLOCKCOMPONENT_BEGIN
	bool UnlockComponent(const char *unlockCode);
	// UNLOCKCOMPONENT_END

	// DH_INSERT_POINT

	// END PUBLIC INTERFACE

	

};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif


