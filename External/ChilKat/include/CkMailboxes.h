// CkMailboxes.h: interface for the (IMAP) CkMailboxes class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKMAILBOXES_H
#define _CKMAILBOXES_H



#include "CkString.h"
#include "CkMultiByteBase.h"

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkMailboxes
class CkMailboxes  : public CkMultiByteBase
{
    public:
	CkMailboxes();
	virtual ~CkMailboxes();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE

	bool IsMarked(long index);
	bool HasInferiors(long index);
	bool IsSelectable(long index);
	// Return the Nth mailbox name in strName.  Return false if index is out of range.
	bool GetName(long index, CkString &outStrName);
	long get_Count(void);

	const char *getName(long index);


	// CK_INSERT_POINT

	// END PUBLIC INTERFACE


    // For internal use only.
    private:

	// Don't allow assignment or copying these objects.
	CkMailboxes(const CkMailboxes &) { } 
	CkMailboxes &operator=(const CkMailboxes &) { return *this; }
	CkMailboxes(void *impl);

};

#ifndef __sun__
#pragma pack (pop)
#endif


#endif


