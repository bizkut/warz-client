// CkSshProgress.h: interface for the CkSshProgress class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKSshPROGRESS_H
#define _CKSshPROGRESS_H

#include "CkObject.h"

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

class CkSshProgress : public CkObject 
{
    public:
	CkSshProgress() { }
	virtual ~CkSshProgress() { }

	// Called periodically to check to see if the method call should be aborted.
	virtual void AbortCheck(bool *abort) { }
	virtual void PercentDone(int pctDone, bool *abort) { }

	virtual void ProgressInfo(const char *name, const char *value) { }

};
#ifndef __sun__
#pragma pack (pop)
#endif


#endif
