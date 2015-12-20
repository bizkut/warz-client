// CkMessageSet.h: interface for the (IMAP) CkMessageSet class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKMESSAGESET_H
#define _CKMESSAGESET_H



class CkByteData;
#include "CkString.h"
#include "CkMultiByteBase.h"

/*
    IMPORTANT: Objects returned by methods as non-const pointers must be deleted
    by the calling application. 

  */

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// Contains a set of IMAP message IDs.  The message set can contain either
// IMAP sequence numbers, or IMAP UIDs.
// CLASS: CkMessageSet
class CkMessageSet  : public CkMultiByteBase
{
    public:
	CkMessageSet();
	virtual ~CkMessageSet();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE

	// Return the complete message set serialized to a string.
	bool ToString(CkString &outStr);
	bool ToCompactString(CkString &outStr);

	// Return true if the message set contains the id.
	bool ContainsId(long id);

	// Remove an id from the message set.
	void RemoveId(long id);

	// Insert an id into the message set (duplicates are not inserted)
	void InsertId(long id);


	// Get the Nth id.  Indexing begins at 0.
	long GetId(long index);

	// Return the number of ids in the set.
	long get_Count(void);

	// Return true if this message set contains UIDs rather than sequence numbers.
	//bool HasUids(void);
	void put_HasUids(bool value);
	bool get_HasUids(void) const;

	const char *toString(void);
	const char *toCompactString(void);
	// FROMCOMPACTSTRING_BEGIN
	bool FromCompactString(const char *str);
	// FROMCOMPACTSTRING_END

	// MESSAGESET_INSERT_POINT

	// END PUBLIC INTERFACE

    // For internal use only.
    private:
	// Don't allow assignment or copying these objects.
	CkMessageSet(const CkMessageSet &);
	CkMessageSet &operator=(const CkMessageSet &);
	CkMessageSet(void *impl);

    public:
	// The following method(s) should not be called by an application.
	// They for internal use only.
	void inject(void *impl);


};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif


