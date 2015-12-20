// CkGzipProgress.h: interface for the CkGzipProgress class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKGZIPPROGRESS_H
#define _CKGZIPPROGRESS_H



#ifndef __sun__
#pragma pack (push, 8)
#endif
 

class CkGzipProgress  
{
    public:
	CkGzipProgress() { }
	virtual ~CkGzipProgress() { }

	// Called periodically to check to see if the method call should be aborted.
	virtual void AbortCheck(bool *abort) { }
	virtual void PercentDone(int pctDone, bool *abort) { }

};
#ifndef __sun__
#pragma pack (pop)
#endif


#endif
