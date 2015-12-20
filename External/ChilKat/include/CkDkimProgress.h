// CkDkimProgress.h: interface for the CkDkimProgress class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKDKIMPROGRESS_H
#define _CKDKIMPROGRESS_H



#ifndef __sun__
#pragma pack (push, 8)
#endif
 

class CkDkimProgress  
{
    public:
	CkDkimProgress() { }
	virtual ~CkDkimProgress() { }

	// Called periodically to check to see if the method call should be aborted.
	virtual void AbortCheck(bool *abort) { }

};
#ifndef __sun__
#pragma pack (pop)
#endif


#endif
