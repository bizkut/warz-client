// CkTar.h: interface for the CkTar class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKTAR_H
#define _CKTAR_H




class CkByteData;
class CkTarProgress;

#include "CkString.h"
#include "CkMultiByteBase.h"

#ifdef WIN32
#pragma warning( disable : 4068 )
#pragma unmanaged
#endif


/*
    IMPORTANT: Objects returned by methods as non-const pointers must be deleted
    by the calling application. 

  */
#ifndef __sun__
#pragma pack (push, 8)
#endif
 


// CLASS: CkTar
class CkTar  : public CkMultiByteBase
{
    private:
	CkTarProgress *m_callback;


    public:
	CkTar();
	virtual ~CkTar();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE

	CkTarProgress *get_EventCallbackObject(void) const;
	void put_EventCallbackObject(CkTarProgress *progress);

	// NOABSOLUTEPATHS_BEGIN
	bool get_NoAbsolutePaths(void);
	void put_NoAbsolutePaths(bool newVal);
	// NOABSOLUTEPATHS_END
	// UNTARCASESENSITIVE_BEGIN
	bool get_UntarCaseSensitive(void);
	void put_UntarCaseSensitive(bool newVal);
	// UNTARCASESENSITIVE_END
	// UNTARDEBUGLOG_BEGIN
	bool get_UntarDebugLog(void);
	void put_UntarDebugLog(bool newVal);
	// UNTARDEBUGLOG_END
	// UNTARDISCARDPATHS_BEGIN
	bool get_UntarDiscardPaths(void);
	void put_UntarDiscardPaths(bool newVal);
	// UNTARDISCARDPATHS_END
	// UNTARFROMDIR_BEGIN
	void get_UntarFromDir(CkString &str);
	const char *untarFromDir(void);
	void put_UntarFromDir(const char *newVal);
	// UNTARFROMDIR_END
	// UNTARMATCHPATTERN_BEGIN
	void get_UntarMatchPattern(CkString &str);
	const char *untarMatchPattern(void);
	void put_UntarMatchPattern(const char *newVal);
	// UNTARMATCHPATTERN_END
	// UNTARMAXCOUNT_BEGIN
	int get_UntarMaxCount(void);
	void put_UntarMaxCount(int newVal);
	// UNTARMAXCOUNT_END
	// UNLOCKCOMPONENT_BEGIN
	bool UnlockComponent(const char *unlockCode);
	// UNLOCKCOMPONENT_END
	// UNTAR_BEGIN
	// eventCallbacks
	int Untar(const char *tarPath);
	// UNTAR_END
	// UNTARFROMMEMORY_BEGIN
	// eventCallbacks
	int UntarFromMemory(const CkByteData &tarFileBytes);
	// UNTARFROMMEMORY_END
	// UNTARFIRSTMATCHINGTOMEMORY_BEGIN
	bool UntarFirstMatchingToMemory(const CkByteData &tarFileBytes, const char *matchPattern, CkByteData &outBytes);
	// UNTARFIRSTMATCHINGTOMEMORY_END
	// ADDDIRROOT_BEGIN
	bool AddDirRoot(const char *dirPath);
	// ADDDIRROOT_END
	// NUMDIRROOTS_BEGIN
	int get_NumDirRoots(void);
	// NUMDIRROOTS_END
	// GETDIRROOT_BEGIN
	bool GetDirRoot(int index, CkString &outStr);
	const char *getDirRoot(int index);
	// GETDIRROOT_END
	// WRITETAR_BEGIN
	// eventCallbacks
	bool WriteTar(const char *tarPath);
	// WRITETAR_END
	// WRITETARGZ_BEGIN
	// eventCallbacks
	bool WriteTarGz(const char *gzPath);
	// WRITETARGZ_END
	// WRITETARBZ2_BEGIN
	// eventCallbacks
	bool WriteTarBz2(const char *bz2Path);
	// WRITETARBZ2_END
	// WRITETARZ_BEGIN
	//bool WriteTarZ(const char *zPath);
	// WRITETARZ_END
	// LISTXML_BEGIN
	// eventCallbacks
	bool ListXml(const char *tarPath, CkString &outStr);
	const char *listXml(const char *tarPath);
	// LISTXML_END
	// CHARSET_BEGIN
	void get_Charset(CkString &str);
	const char *charset(void);
	void put_Charset(const char *newVal);
	// CHARSET_END
	// UNTARZ_BEGIN
	// eventCallbacks
	bool UntarZ(const char *tarPath);
	// UNTARZ_END
	// UNTARGZ_BEGIN
	// eventCallbacks
	bool UntarGz(const char *tarPath);
	// UNTARGZ_END
	// UNTARBZ2_BEGIN
	// eventCallbacks
	bool UntarBz2(const char *tarPath);
	// UNTARBZ2_END
	// USERNAME_BEGIN
	void get_UserName(CkString &str);
	const char *userName(void);
	void put_UserName(const char *newVal);
	// USERNAME_END
	// GROUPNAME_BEGIN
	void get_GroupName(CkString &str);
	const char *groupName(void);
	void put_GroupName(const char *newVal);
	// GROUPNAME_END
	// USERID_BEGIN
	int get_UserId(void);
	void put_UserId(int newVal);
	// USERID_END
	// GROUPID_BEGIN
	int get_GroupId(void);
	void put_GroupId(int newVal);
	// GROUPID_END
	// DIRMODE_BEGIN
	int get_DirMode(void);
	void put_DirMode(int newVal);
	// DIRMODE_END
	// FILEMODE_BEGIN
	int get_FileMode(void);
	void put_FileMode(int newVal);
	// FILEMODE_END
	// VERBOSELOGGING_BEGIN
	bool get_VerboseLogging(void);
	void put_VerboseLogging(bool newVal);
	// VERBOSELOGGING_END
	// WRITEFORMAT_BEGIN
	void get_WriteFormat(CkString &str);
	const char *writeFormat(void);
	void put_WriteFormat(const char *newVal);
	// WRITEFORMAT_END
	// HEARTBEATMS_BEGIN
	int get_HeartbeatMs(void);
	void put_HeartbeatMs(int newVal);
	// HEARTBEATMS_END
	// SCRIPTFILEMODE_BEGIN
	int get_ScriptFileMode(void);
	void put_ScriptFileMode(int newVal);
	// SCRIPTFILEMODE_END
	// DIRPREFIX_BEGIN
	void get_DirPrefix(CkString &str);
	const char *dirPrefix(void);
	void put_DirPrefix(const char *newVal);
	// DIRPREFIX_END
	// VERIFYTAR_BEGIN
	// eventCallbacks
	bool VerifyTar(const char *tarPath);
	// VERIFYTAR_END

	// TAR_INSERT_POINT

	// END PUBLIC INTERFACE


    // For internal use only.
    private:
	// Don't allow assignment or copying these objects.
	CkTar(const CkTar &) { } 
	CkTar &operator=(const CkTar &) { return *this; }
	CkTar(void *impl);



};

#ifndef __sun__
#pragma pack (pop)
#endif


#endif


