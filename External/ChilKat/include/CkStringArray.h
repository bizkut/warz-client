// CkStringArray.h: interface for the CkStringArray class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKSTRINGARRAY_H_INCLUDED
#define _CKSTRINGARRAY_H_INCLUDED

#include "CkString.h"
class CkByteData;

#include "CkMultiByteBase.h"

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkStringArray
class CkStringArray : public CkMultiByteBase  
{
    private:
	// Don't allow assignment or copying these objects.
	CkStringArray(const CkStringArray &c) { } 
	CkStringArray &operator=(const CkStringArray &) { return *this; }
	CkStringArray(void *impl);


    public:
	void inject(void *impl);

	CkStringArray();
	virtual ~CkStringArray();

	static CkStringArray *createNew(void);

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE
	// LASTSTRING_BEGIN
	bool LastString(CkString &outStr);
	const char *lastString(void);
	// LASTSTRING_END

	// GETSTRING_BEGIN
	bool GetString(int index, CkString &outStr);
	const char *getString(int index);
	// GETSTRING_END

	// REMOVEAT_BEGIN
	bool RemoveAt(int index);
	// REMOVEAT_END

	// POP_BEGIN
	bool Pop(CkString &outStr);
	const char *pop(void);
	// POP_END

	// APPEND_BEGIN
	bool Append(const char *str);
	// APPEND_END

	// SAVETOFILE2_BEGIN
	bool SaveToFile2(const char *path, const char *charset);
	// SAVETOFILE2_END

	// LOADFROMFILE2_BEGIN
	bool LoadFromFile2(const char *path, const char *charset);
	// LOADFROMFILE2_END



	// Add a string to the array.
	//void Append(const char *str);

	// Return the number of strings in the array.
	int get_Count();

	// Loads a file into an array of strings -- one per line.
	// CRLFs are not included in each string.
	// Blank lines are ignored and not included in the array.
	bool LoadFromFile(const char *path);

	// Saves the string array to a file.  Line endings are controlled
	// by the Crlf property (get_Crlf/put_Crlf)
	bool SaveToFile(const char *path);


	void InsertAt(int index, const char *str);

	// Find an exact match, case sensitive.
	// Begin searching at firstIndex.
	int Find(const char *str, int firstIndex);

	void Prepend(const char *str);

        // If true, always convert to CRLF before adding, and before returning.
        // If false, convert to bare LF before adding and before returning.
	bool get_Crlf();
	void put_Crlf(bool newVal);

        // If true, always trim whitespace from both ends of the string before
        // adding.
	bool get_Trim();
	void put_Trim(bool newVal);

        // If true, then duplicates cannot exist in the collection.
	// Methods such as Append will do nothing if the string already
	// exists.
	// The default is false.
	bool get_Unique();
	void put_Unique(bool newVal);

	// Serialize the entire string array into a single Base64-encoded string.
	bool Serialize(CkString &outStr);
	// Append from a previously serialized string array.
	bool AppendSerialized(const char *encodedStr);

	void Subtract(CkStringArray &sa);

	// Remove the string that exactly matches str (case sensitive).
	void Remove(const char *str);

	// Return true if the string array contains the str (case sensitive)
	bool Contains(const char *str);

	// Remove all strings from the array.
	void Clear();

	// Sort the string array in lexicographic order.
	void Sort(bool ascending);

	// Split a string at a boundary (1 character or more)
	// and append the individual strings to this string array.
	void SplitAndAppend(const char *str, const char *boundary);


        const char *strAt(int index);
        const char *serialize(void);
	// LOADFROMTEXT_BEGIN
	void LoadFromText(const char *str);
	// LOADFROMTEXT_END
	// SAVENTHTOFILE_BEGIN
	bool SaveNthToFile(int index, const char *path);
	// SAVENTHTOFILE_END
	// SAVETOTEXT_BEGIN
	bool SaveToText(CkString &outStr);
	const char *saveToText(void);
	// SAVETOTEXT_END
	// FINDFIRSTMATCH_BEGIN
	int FindFirstMatch(const char *str, int firstIndex);
	// FINDFIRSTMATCH_END
	// UNION_BEGIN
	void Union(CkStringArray &sa);
	// UNION_END
	// GETSTRINGLEN_BEGIN
	int GetStringLen(int index);
	// GETSTRINGLEN_END

	// STRINGARRAY_INSERT_POINT

	// END PUBLIC INTERFACE


};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif

