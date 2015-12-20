// CkRssProgress.h: interface for the CkRssProgress class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKRssPROGRESS_H
#define _CKRssPROGRESS_H



#ifndef __sun__
#pragma pack (push, 8)
#endif
 

class CkRssProgress  
{
    public:
	CkRssProgress() { }
	virtual ~CkRssProgress() { }

	// Called periodically to check to see if the method call should be aborted.
	virtual void AbortCheck(bool *abort) { }

};
#ifndef __sun__
#pragma pack (pop)
#endif


#endif
