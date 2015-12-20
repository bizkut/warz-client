// CkDhProgress.h: interface for the CkDhProgress class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKDHPROGRESS_H
#define _CKDHPROGRESS_H



#ifndef __sun__
#pragma pack (push, 8)
#endif
 

class CkDhProgress  
{
    public:
	CkDhProgress() { }
	virtual ~CkDhProgress() { }

	// Called periodically to check to see if the method call should be aborted.
	virtual void AbortCheck(bool *abort) { }

};
#ifndef __sun__
#pragma pack (pop)
#endif


#endif
