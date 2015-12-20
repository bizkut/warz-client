// CkImapProgress.h: interface for the CkImapProgress class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKIMAPPROGRESS_H
#define _CKIMAPPROGRESS_H

#include "CkObject.h"

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

class CkImapProgress : public CkObject 
{
    public:
	CkImapProgress() { }
	virtual ~CkImapProgress() { }

	// Called periodically to check to see if the method call should be aborted.
	virtual void AbortCheck(bool *abort) { }
	virtual void PercentDone(int pctDone, bool *abort) { }

	virtual void ProgressInfo(const char *name, const char *value) { }

};
#ifndef __sun__
#pragma pack (pop)
#endif


#endif
