// CkCrypt2Progress.h: interface for the CkCrypt2Progress class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKCRYPT2PROGRESS_H
#define _CKCRYPT2PROGRESS_H



#ifndef __sun__
#pragma pack (push, 8)
#endif
 

class CkCrypt2Progress  
{
    public:
	CkCrypt2Progress() { }
	virtual ~CkCrypt2Progress() { }

	// Called periodically to check to see if the method call should be aborted.
	virtual void AbortCheck(bool *abort) { }

	virtual void PercentDone(int pctDone, bool *abort) { }

};
#ifndef __sun__
#pragma pack (pop)
#endif


#endif
