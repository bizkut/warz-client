// CkMailProgress.h: interface for the CkMailProgress class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CkMailProgress_H
#define _CkMailProgress_H


#include "CkObject.h"

/*

  To receive progress events (callbacks), create a C++ class that 
  inherits this one and provides one or more overriding implementations 
  for the events you wish to receive.  

  */
#ifndef __sun__
#pragma pack (push, 8)
#endif
 


class CkMailProgress : public CkObject 
{
    public:
	CkMailProgress();
	virtual ~CkMailProgress();

	virtual void SendPercentDone(long pctDone, bool *abort) { }
	virtual void ReadPercentDone(long pctDone, bool *abort) { }

	// Called periodically to check to see if the email sending/receiving should be aborted.
	virtual void AbortCheck(bool *abort) { }

	virtual void EmailReceived(const char *subject, 
			    const char *fromAddr, const char *fromName, 
			    const char *returnPath, 
			    const char *date, 
			    const char *uidl, 
			    int sizeInBytes) { }

	virtual void ProgressInfo(const char *name, const char *value) { }

};
#ifndef __sun__
#pragma pack (pop)
#endif


#endif
