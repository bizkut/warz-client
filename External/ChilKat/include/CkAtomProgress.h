// CkAtomProgress.h: interface for the CkAtomProgress class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKAtomPROGRESS_H
#define _CKAtomPROGRESS_H



#ifndef __sun__
#pragma pack (push, 8)
#endif
 

class CkAtomProgress  
{
    public:
	CkAtomProgress() { }
	virtual ~CkAtomProgress() { }

	// Called periodically to check to see if the method call should be aborted.
	virtual void AbortCheck(bool *abort) { }

};
#ifndef __sun__
#pragma pack (pop)
#endif


#endif
