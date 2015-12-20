// CkCharset.h: interface for the CkCharset class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKCHARSET_H
#define _CKCHARSET_H




class CkByteData;
#include "CkMultiByteBase.h"
#include "CkString.h"

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkCharset
class CkCharset  : public CkMultiByteBase
{
    private:
	// Don't allow assignment or copying these objects.
	CkCharset(const CkCharset &) { } 
	CkCharset &operator=(const CkCharset &) { return *this; }

    public:
	CkCharset();
	virtual ~CkCharset();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	bool ConvertData2(const char *inData, unsigned long inDataLen, CkByteData &outData);
	bool GetHtmlCharset2(const char *htmlData, unsigned long htmlDataLen, CkString &outCharset);
	bool ConvertHtml2(const char *htmlIn, unsigned long htmlInLen, CkByteData &outHtml);
	bool WriteFile2(const char *path, const char *pByteData, unsigned long szByteData);
	bool ConvertFromUnicode2(const char *pByteData, unsigned long szByteData, CkByteData &outMbData);
	bool ConvertToUnicode2(const char *pByteData, unsigned long szByteData, CkByteData &outUniData);
	void SetErrorBytes(const char *pByteData, unsigned long szByteData);
	const char *getHtmlCharset2(const char *htmlData, unsigned long htmlDataLen);

	bool ConvertHtml3(const unsigned char *pByteData, unsigned long szByteData, CkByteData &outHtml);
	bool GetHtmlCharset3(const unsigned char *pByteData, unsigned long szByteData, CkString &outCharset);
	bool WriteFile3(const char *path, const unsigned char *pByteData, unsigned long szByteData);
	bool ConvertData3(const unsigned char *pByteData, unsigned long szByteData, CkByteData &outData);
	bool ConvertFromUnicode3(const unsigned char *pByteData, unsigned long szByteData, CkByteData &outMbData);
	bool ConvertToUnicode3(const unsigned char *pByteData, unsigned long szByteData, CkByteData &outUniData);
	const char *getHtmlCharset3(const unsigned char *pByteData, unsigned long szByteData);


	// Convert byte data into the memory buffer you provide. outDataLen is set
	// to the size of the output buffer 
	bool ConvertData4(const unsigned char *pByteData, unsigned long szByteData,unsigned char *toAddr, unsigned long *toSize);


	// BEGIN PUBLIC INTERFACE
	// READFILETOSTRING_BEGIN
	bool ReadFileToString(const char *path, const char *srcCharset, CkString &outStr);
	const char *readFileToString(const char *path, const char *srcCharset);
	// READFILETOSTRING_END

	// WRITESTRINGTOFILE_BEGIN
	bool WriteStringToFile(const char *str, const char *path, const char *charset);
	// WRITESTRINGTOFILE_END


	const char *upperCase(const char *inStr);
	const char *lowerCase(const char *inStr);
	bool UpperCase(const char *inStr, CkString &outStr);
	bool LowerCase(const char *inStr, CkString &outStr);

	bool GetHtmlFileCharset(const char *htmlPath, CkString &outCharset);
	bool GetHtmlCharset(const CkByteData &htmlData, CkString &outCharset);
	bool ConvertHtmlFile(const char *srcPath, const char *destPath);
	bool ConvertHtml(const CkByteData &htmlIn, CkByteData &outHtml);

	void get_LastOutputAsQP(CkString &str);
	void get_LastInputAsQP(CkString &str);
	void get_LastOutputAsHex(CkString &str);
	void get_LastInputAsHex(CkString &str);
	void put_SaveLast(bool value);
	bool get_SaveLast(void);

	bool WriteFile(const char *path, const CkByteData &dataBuf);
	bool ReadFile(const char *path, CkByteData &outData);

	bool ConvertFromUnicode(const CkByteData &uniData, CkByteData &outMbData);
	bool ConvertToUnicode(const CkByteData &mbData, CkByteData &outUniData);

	bool VerifyFile(const char *charset, const char *path);
	bool VerifyData(const char *charset, const CkByteData &charData);

	bool ConvertFile(const char *srcPath, const char *destPath);

	bool HtmlEntityDecode(const CkByteData &inData, CkByteData &outData);
	bool HtmlDecodeToStr(const char *str, CkString &outStr);
	bool HtmlEntityDecodeFile(const char *srcPath, const char *destPath);

	bool EntityEncodeHex(const char *inStr, CkString &outStr);
	bool EntityEncodeDec(const char *inStr, CkString &outStr);
	const char *entityEncodeHex(const char *inStr);
	const char *entityEncodeDec(const char *inStr);

	bool ConvertData(const CkByteData &inData, CkByteData &outData);

	void get_ToCharset(CkString &str);
	void put_ToCharset(const char *charset);
	void get_FromCharset(CkString &str);
	void put_FromCharset(const char *charset);

	bool UnlockComponent(const char *unlockCode);
	bool IsUnlocked(void) const;

	int CharsetToCodePage(const char *charsetName);
	bool CodePageToCharset(int codePage, CkString &outCharset);

	// Error actions:
	// 0: drop error character.
	// 1: substitute default bytes
	// 2: hex numeric escape &#xXXXX;          
	// 3: RESERVED
	// 4: RESERVED
	// 5: RESERVED
	// 6: use alternate To charset
	// 7: pass through non-convertable bytes unchanged.
	int get_ErrorAction(void);
	void put_ErrorAction(int val);

	void get_AltToCharset(CkString &str);
	void put_AltToCharset(const char *charsetName);

	void SetErrorBytes(const unsigned char *pByteData, unsigned long szByteData);
	void SetErrorString(const char *str, const char *charset);

	const char *getHtmlFileCharset(const char *htmlPath);
	const char *getHtmlCharset(const CkByteData &htmlData);
	const char *lastOutputAsQP(void);
	const char *lastInputAsQP(void);
	const char *lastOutputAsHex(void);
	const char *lastInputAsHex(void);
	const char *htmlDecodeToStr(const char *str);
	const char *toCharset(void);
	const char *fromCharset(void);
	const char *codePageToCharset(int codePage);
	const char *altToCharset(void);
	// URLDECODESTR_BEGIN
	bool UrlDecodeStr(const char *inStr, CkString &outStr);
	const char *urlDecodeStr(const char *inStr);
	// URLDECODESTR_END
	// CONVERTFILENOPREAMBLE_BEGIN
	bool ConvertFileNoPreamble(const char *srcPath, const char *destPath);
	// CONVERTFILENOPREAMBLE_END

	// CHARSET_INSERT_POINT

	// END PUBLIC INTERFACE



};
#ifndef __sun__
#pragma pack (pop)
#endif


#endif
