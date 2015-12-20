// CkBz2Progress.h: interface for the CkBz2Progress class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CkBz2PROGRESS_H
#define _CkBz2PROGRESS_H



#ifndef __sun__
#pragma pack (push, 8)
#endif
 

class CkBz2Progress  
{
    public:
	CkBz2Progress() { }
	virtual ~CkBz2Progress() { }

	// Called periodically to check to see if the method call should be aborted.
	virtual void AbortCheck(bool *abort) { }

};
#ifndef __sun__
#pragma pack (pop)
#endif


#endif
