// CkSFtpProgress.h: interface for the CkSFtpProgress class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKSFTPPROGRESS_H
#define _CKSFTPPROGRESS_H

#include "CkObject.h"

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

class CkSFtpProgress : public CkObject 
{
    public:
	CkSFtpProgress() { }
	virtual ~CkSFtpProgress() { }

	// Called periodically to check to see if the method call should be aborted.
	virtual void AbortCheck(bool *abort) { }
	virtual void PercentDone(int pctDone, bool *abort) { }

	virtual void UploadRate(unsigned long byteCount, unsigned long bytesPerSec) { }
	virtual void DownloadRate(unsigned long byteCount, unsigned long bytesPerSec) { }

	virtual void ProgressInfo(const char *name, const char *value) { }

};
#ifndef __sun__
#pragma pack (pop)
#endif


#endif
