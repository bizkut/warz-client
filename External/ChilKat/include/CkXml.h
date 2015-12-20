// CkXml.h: interface for the CkXml class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKXML_H
#define _CKXML_H



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
 

// CLASS: CkXml
class CkXml : public CkMultiByteBase
{
    private:
	// Don't allow assignment or copying these objects.
	CkXml(const CkXml &) { } 
	CkXml &operator=(const CkXml &) { return *this; }
	CkXml(void *impl);

    public:
	CkXml();
	virtual ~CkXml();

	static CkXml *createNew(void);

	void inject(void *impl);

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	bool SetBinaryContent(const char *pByteData, unsigned long szByteData, bool zipFlag, bool encryptFlag, const char *password);

	bool AddChildTree(const CkXml *tree);
	bool SwapTree(const CkXml *tree);
	bool SwapNode(const CkXml *node);
	void CopyRef(CkXml *node);


	bool GetChildIntValue(const char *tag, int &value);


	// BEGIN PUBLIC INTERFACE

#if !defined(CHILKAT_MONO)
	bool SetBinaryContent2(const unsigned char *pByteData, unsigned long szByteData,bool zipFlag, bool encryptFlag, const char *password);
#endif
	bool SetBinaryContent(const CkByteData &data, bool zipFlag, bool encryptFlag, const char *password);

	bool GetBinaryContent(bool unzipFlag, bool decryptFlag, const char *password, CkByteData &outData);

	bool ZipTree();
	bool ZipContent();
	bool UnzipTree();
	bool UnzipContent();

	bool EncryptContent(const char *password);
	bool DecryptContent(const char *password);

	void GetRoot2(void);

	CkXml *GetRoot(void);
	CkXml *GetChildWithTag(const char *tag);
	CkXml *GetChildWithContent(const char *content);
	CkXml *GetChildExact(const char *tag, const char *content);

	long get_TreeId(void);
	CkXml *PreviousSibling(void);
	CkXml *NextSibling(void);
	bool PreviousSibling2(void);
	bool NextSibling2(void);
	CkXml *LastChild(void);
	CkXml *FirstChild(void);
	bool Clear();
	void Copy(const CkXml &node);
	CkXml *GetParent(void);
	bool GetParent2(void);
	bool FirstChild2(void);
	bool LastChild2(void);

	CkXml *SearchForAttribute(const CkXml *after, const char *tag, const char *attr, const char *valuePattern);
	CkXml *SearchAllForContent(const CkXml *after, const char *contentPattern);
	CkXml *SearchForContent(const CkXml *after, const char *tag, const char *contentPattern);
	CkXml *SearchForTag(const CkXml *after, const char *tag);

	bool SearchForAttribute2(const CkXml *after, const char *tag, const char *attr, const char *valuePattern);
	bool SearchAllForContent2(const CkXml *after, const char *contentPattern);
	bool SearchForContent2(const CkXml *after, const char *tag, const char *contentPattern);
	bool SearchForTag2(const CkXml *after, const char *tag);
	bool GetNthChildWithTag2(const char *tag, long n);




	CkXml *FindChild(const char *tag);
	CkXml *FindOrAddNewChild(const char *tag);
	CkXml *NewChildBefore(int index, const char *tag, const char *content);
	CkXml *NewChildAfter(int index, const char *tag, const char *content);
	CkXml *NewChild(const char *tag, const char *content);
	void NewChild2(const char *tag, const char *content);
	void NewChildInt2(const char *tag, int value);
	CkXml *GetNthChildWithTag(const char *tag, long n);
	long NumChildrenHavingTag(const char *tag);
	CkXml *ExtractChildByName(const char *tag, const char *attrName, const char *attrValue);

	CkXml *ExtractChildByIndex(long index);
	void RemoveFromTree(void);
	CkXml *GetChild(long index);
	bool GetChild2(long index);

	bool AddChildTree(const CkXml &tree) { return AddChildTree(&tree); }

	bool InsertChildTreeAfter(int index, const CkXml &tree);
	bool InsertChildTreeBefore(int index, const CkXml &tree);

	bool SwapTree(const CkXml &tree) { return SwapTree(&tree); }
	bool SwapNode(const CkXml &node) { return SwapNode(&node); }

	bool HasAttrWithValue(const char *name, const char *value);

	bool GetAttrValue(const char *name, CkString &outStr);
	bool GetAttributeValue(long index, CkString &outStr);
	bool GetAttributeName(long index, CkString &outStr);

	bool RemoveAllAttributes();

	void RemoveChild(const char *tag);

	bool RemoveAttribute(const char *name);
	bool AddAttribute(const char *name, const char *value);
	bool AppendToContent(const char *str);

	bool GetXml(CkString &outStr);

	bool SaveXml(const char *path);
	bool LoadXmlFile(const char *path);
	bool LoadXml(const char *xmlData);
	bool LoadXmlFile2(const char *path, bool autoTrim);
	bool LoadXml2(const char *xmlData, bool autoTrim);

	bool get_Cdata();
	bool put_Cdata(bool newVal);

	bool get_SortCaseInsensitive();
	void put_SortCaseInsensitive(bool newVal);

	long get_NumChildren();

	void get_Content(CkString &str);
	void put_Content(const char *newVal);

	bool GetChildContent(const char *tag, CkString &outStr);

	// Replace the content of the child having a specific tag,
	// or if it doesn't already exist, create a new child having the tag/value.
	void UpdateChildContent(const char *tag, const char *value);

	// Return true if the node's content matches the wildcarded pattern.
	// A '*' matches 0 or more of any character, a '?' matches any one character.
	bool ContentMatches(const char *pattern, bool caseSensitive);

	// Same as ContentMatches, but matches the content of the child having a specific tag.
	bool ChildContentMatches(const char *tag, const char *pattern, bool caseSensitive);

	void get_Tag(CkString &str);
	void put_Tag(const char *newVal);

	long get_NumAttributes();

	bool get_Standalone(void);
	void put_Standalone(bool newVal);

	void get_Encoding(CkString &str);  // such as iso-8859-1
	void put_Encoding(const char *newVal);


	bool QEncodeContent(const char *charset, const CkByteData &db);
	bool BEncodeContent(const char *charset, const CkByteData &db);
	bool DecodeContent(CkByteData &outData);

	bool UpdateAttribute(const char *attrName, const char *attrValue);

	void SortByTag(bool ascending);
	void SortByContent(bool ascending);
	void SortByAttribute(const char *attrName, bool ascending);

	void SortRecordsByContent(const char *sortTag, bool ascending);
	void SortRecordsByAttribute(const char *sortTag, const char *attrName, bool ascending);
	CkXml *FindNextRecord(const char *tag, const char *contentPattern);

	bool HasChildWithTag(const char *tag);

	bool GetChildBoolValue(const char *tag);
	int GetChildIntValue(const char *tag);


	void RemoveChildWithContent(const char *content);
	bool HasChildWithTagAndContent(const char *tag, const char *content);
	bool HasChildWithContent(const char *content);
	void RemoveAllChildren(void);
	void RemoveChildByIndex(long index);
	bool AccumulateTagContent(const char *tag, const char *skipTags, CkString &outStr);
	bool GetChildContentByIndex(long index, CkString &outStr);
	bool GetChildTag(long index, CkString &outStr);
	void AddToAttribute(const char *name, long amount);
	void AddToContent(long amount);
	void AddToChildContent(const char *tag, long amount);
	void AddOrUpdateAttributeI(const char *name, long value);
	void AddOrUpdateAttribute(const char *name, const char *value);
	bool HasAttribute(const char *name);

	const char *tag(void);
	const char *content(void);
	const char *encoding(void);
	const char *getXml(void);
	const char *getChildContent(const char *tag);
	const char *getAttrValue(const char *name);
	const char *getAttributeValue(long index);
	const char *getAttributeName(long index);
	const char *accumulateTagContent(const char *tag, const char *skipTags);
	const char *getChildContentByIndex(long index);
	const char *getChildTag(long index);

	const char *childContent(const char *tag){ return getChildContent(tag); }
	const char *attr(const char *name){ return getAttrValue(name); }
	const char *attrValue(long index){ return getAttributeValue(index); }
	const char *attrName(long index){ return getAttributeName(index); }
	const char *xml(void){ return getXml(); }
	// TAGEQUALS_BEGIN
	bool TagEquals(const char *tag);
	// TAGEQUALS_END
	// CONTENTINT_BEGIN
	int get_ContentInt(void);
	void put_ContentInt(int newVal);
	// CONTENTINT_END
	// ADDATTRIBUTEINT_BEGIN
	bool AddAttributeInt(const char *name, int value);
	// ADDATTRIBUTEINT_END
	// DECODEENTITIES_BEGIN
	bool DecodeEntities(const char *str, CkString &outStr);
	const char *decodeEntities(const char *str);
	// DECODEENTITIES_END
	// FINDCHILD2_BEGIN
	bool FindChild2(const char *tag);
	// FINDCHILD2_END
	// GETATTRVALUEINT_BEGIN
	int GetAttrValueInt(const char *name);
	// GETATTRVALUEINT_END
	// GETATTRIBUTEVALUEINT_BEGIN
	int GetAttributeValueInt(int index);
	// GETATTRIBUTEVALUEINT_END
	// GETCHILDTAGBYINDEX_BEGIN
	bool GetChildTagByIndex(int index, CkString &outStr);
	const char *getChildTagByIndex(int index);
	// GETCHILDTAGBYINDEX_END
	// SAVEBINARYCONTENT_BEGIN
	bool SaveBinaryContent(const char *path, bool unzipFlag, bool decryptFlag, const char *password);
	// SAVEBINARYCONTENT_END
	// SETBINARYCONTENTFROMFILE_BEGIN
	bool SetBinaryContentFromFile(const char *path, bool zipFlag, bool encryptFlag, const char *password);
	// SETBINARYCONTENTFROMFILE_END
	// SORTBYATTRIBUTEINT_BEGIN
	void SortByAttributeInt(const char *attrName, bool ascending);
	// SORTBYATTRIBUTEINT_END
	// UPDATEATTRIBUTEINT_BEGIN
	bool UpdateAttributeInt(const char *attrName, int value);
	// UPDATEATTRIBUTEINT_END
	// UPDATECHILDCONTENTINT_BEGIN
	void UpdateChildContentInt(const char *tag, int value);
	// UPDATECHILDCONTENTINT_END
	// ADDSTYLESHEET_BEGIN
	void AddStyleSheet(const char *styleSheet);
	// ADDSTYLESHEET_END
	// SORTRECORDSBYCONTENTINT_BEGIN
	void SortRecordsByContentInt(const char *sortTag, bool ascending);
	// SORTRECORDSBYCONTENTINT_END
	// TAGCONTENT_BEGIN
	bool TagContent(const char *tag, CkString &outStr);
	const char *tagContent(const char *tag);
	// TAGCONTENT_END
	// GETSELF_BEGIN
	CkXml *GetSelf(void);
	// GETSELF_END
	// DOCTYPE_BEGIN
	void get_DocType(CkString &str);
	const char *docType(void);
	void put_DocType(const char *newVal);
	// DOCTYPE_END
	// CHILKATPATH_BEGIN
	bool ChilkatPath(const char *cmd, CkString &outStr);
	const char *chilkatPath(const char *cmd);
	// CHILKATPATH_END
	// GETCHILDWITHATTR_BEGIN
	CkXml *GetChildWithAttr(const char *tag, const char *attrName, const char *attrValue);
	// GETCHILDWITHATTR_END
	// COPYREF_BEGIN
	void CopyRef(CkXml &node);
	// COPYREF_END

	// XML_INSERT_POINT

	// END PUBLIC INTERFACE

	void Copy(const CkXml *node);

	CkXml *SearchForAttribute(const CkXml &after, const char *tag, const char *attr, const char *valuePattern){ return SearchForAttribute(&after,tag,attr,valuePattern); }
	CkXml *SearchAllForContent(const CkXml &after, const char *contentPattern){ return SearchAllForContent(&after,contentPattern); }
	CkXml *SearchForContent(const CkXml &after, const char *tag, const char *contentPattern){ return SearchForContent(&after,tag,contentPattern); }
	CkXml *SearchForTag(const CkXml &after, const char *tag){ return SearchForTag(&after,tag); }

	bool SearchForAttribute2(const CkXml &after, const char *tag, const char *attr, const char *valuePattern){ return SearchForAttribute2(&after,tag,attr,valuePattern); }
	bool SearchAllForContent2(const CkXml &after, const char *contentPattern){ return SearchAllForContent2(&after,contentPattern); }
	bool SearchForContent2(const CkXml &after, const char *tag, const char *contentPattern){ return SearchForContent2(&after,tag,contentPattern); }
	bool SearchForTag2(const CkXml &after, const char *tag){ return SearchForTag2(&after,tag); }

};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif


