// CkBounce.h: interface for the CkBounce class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKBOUNCE_H
#define _CKBOUNCE_H




class CkString;
class CkEmail;
#include "CkObject.h"
#include "CkMultiByteBase.h"

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkBounce
class CkBounce  : public CkMultiByteBase
{
    private:
	// Don't allow assignment or copying these objects.
	CkBounce(const CkBounce &) { } 
	CkBounce &operator=(const CkBounce &) { return *this; }
	CkBounce(void *impl);

    public:

	CkBounce();
	virtual ~CkBounce();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE
	bool UnlockComponent(const char *unlockCode);
	bool ExamineEmail(const CkEmail &email);
	bool ExamineMime(const char *mimeString);
	bool ExamineEml(const char *emlPath);

	int get_BounceType(void) const;
	void get_BounceAddress(CkString &bouncedEmailAddr);
	void get_BounceData(CkString &mailBodyText);

	const char *bounceAddress(void);
	const char *bounceData(void);

	// BOUNCE_INSERT_POINT

	// END PUBLIC INTERFACE



};


#ifndef __sun__
#pragma pack (pop)
#endif



#endif
