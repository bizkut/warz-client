// CkCertChain.h: interface for the CkCertChain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CKCERTCHAIN_H__CB7CF92E_A9AC_469E_B628_C76FAA866AB7__INCLUDED_)
#define AFX_CKCERTCHAIN_H__CB7CF92E_A9AC_469E_B628_C76FAA866AB7__INCLUDED_


class CkCert;
class CkCertCollection;
#include "CkMultiByteBase.h"
#include "CkString.h"

/*
    IMPORTANT: Objects returned by methods as non-const pointers must be deleted
    by the calling application. 

  */

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkCertChain
class CkCertChain  : public CkMultiByteBase
{
    private:
	// Don't allow assignment or copying these objects.
	CkCertChain(const CkCertChain &) { } 
	CkCertChain &operator=(const CkCertChain &) { return *this; }
	CkCertChain(void *impl);

    public:
	CkCertChain();
	virtual ~CkCertChain();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE

	bool BuildChain(const CkCert &cert, CkCertCollection *extraCerts);

	CkCert *GetCert(int index);

	int get_NumCerts(void) const;


	// CK_INSERT_POINT

	// END PUBLIC INTERFACE


};
#ifndef __sun__
#pragma pack (pop)
#endif


#endif // !defined(AFX_CKCERTCHAIN_H__CB7CF92E_A9AC_469E_B628_C76FAA866AB7__INCLUDED_)

