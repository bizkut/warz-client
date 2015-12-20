// CkMhtProgress.h: interface for the CkMhtProgress class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKMHTPROGRESS_H
#define _CKMHTPROGRESS_H



#ifndef __sun__
#pragma pack (push, 8)
#endif
 

class CkMhtProgress  
{
    public:
	CkMhtProgress() { }
	virtual ~CkMhtProgress() { }

	// Called periodically to check to see if the method call should be aborted.
	virtual void AbortCheck(bool *abort) { }

	virtual void DownloadingUrl(const char *url) { }

	virtual void ProgressInfo(const char *name, const char *value) { }
};
#ifndef __sun__
#pragma pack (pop)
#endif


#endif
