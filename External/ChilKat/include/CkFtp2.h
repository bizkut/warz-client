// CkFtp2.h: interface for the CkFtp2 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKFTP2_H
#define _CKFTP2_H




class CkByteData;


class CkCert;


#ifndef WIN32
#include "int64.h"
#endif

#include "CkString.h"
#include "CkMultiByteBase.h"

#if !defined(WIN32) && !defined(WINCE)
#include "SystemTime.h"
#include "FileTime.h"
#endif

#ifdef WIN32
#pragma warning( disable : 4068 )
#pragma unmanaged
#endif

class CkDateTime;

class CkFtpProgress;
/*
    IMPORTANT: Objects returned by methods as non-const pointers must be deleted
    by the calling application. 

  */

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkFtp2
class CkFtp2  : public CkMultiByteBase
{
    public:
	CkFtp2();
	virtual ~CkFtp2();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	//bool PutTree(const char *localDir, CkFtpProgress &progress);
	//bool AppendFile2(const char *localFilename,  const char *remoteFilename, CkFtpProgress &progress);
	//bool AppendFileFromBinaryData2(const char *remoteFilename, const CkByteData &binaryData, CkFtpProgress &progress);
	//bool AppendFileFromTextData2(const char *remoteFilename, const char *textData, CkFtpProgress &progress);
	// Return the number of files retrieved.
	//long MGetFiles2(const char *remotePattern, const char *localDir, CkFtpProgress &progress);
	// Return the number of files uploaded.
	//long MPutFiles2(const char *pattern, CkFtpProgress &progress);

	//bool GetFile2(const char *remoteFilename,  const char *localFilename, CkFtpProgress &progress);
	//bool PutFile2(const char *localFilename,  const char *remoteFilename, CkFtpProgress &progress);
	//bool PutFileFromBinaryData2(const char *remoteFilename, const CkByteData &binaryData, CkFtpProgress &progress);
	//bool PutFileFromTextData2(const char *remoteFilename, const char *textData, CkFtpProgress &progress);

	//bool GetRemoteFileBinaryData2(const char *remoteFilename, CkByteData &data, CkFtpProgress &progress);
	//bool GetRemoteFileTextData2(const char *remoteFilename, CkString &str, CkFtpProgress &progress);


	// BEGIN PUBLIC INTERFACE
	// ASYNCBYTESSENTSTR_BEGIN
	void get_AsyncBytesSentStr(CkString &str);
	const char *asyncBytesSentStr(void);
	// ASYNCBYTESSENTSTR_END

	// ASYNCBYTESRECEIVEDSTR_BEGIN
	void get_AsyncBytesReceivedStr(CkString &str);
	const char *asyncBytesReceivedStr(void);
	// ASYNCBYTESRECEIVEDSTR_END

	// SKIPFINALREPLY_BEGIN
	bool get_SkipFinalReply(void);
	void put_SkipFinalReply(bool newVal);
	// SKIPFINALREPLY_END

	// SORCVBUF_BEGIN
	int get_SoRcvBuf(void);
	void put_SoRcvBuf(int newVal);
	// SORCVBUF_END

	// SOSNDBUF_BEGIN
	int get_SoSndBuf(void);
	void put_SoSndBuf(int newVal);
	// SOSNDBUF_END

	// SETREMOTEFILEDATETIMESTR_BEGIN
	// eventCallbacks
	bool SetRemoteFileDateTimeStr(const char *dateTimeStr, const char *remoteFilename);
	// SETREMOTEFILEDATETIMESTR_END

	// SETOLDESTDATESTR_BEGIN
	void SetOldestDateStr(const char *oldestDateTimeStr);
	// SETOLDESTDATESTR_END

	// GETCREATETIMEBYNAMESTR_BEGIN
	// eventCallbacks
	bool GetCreateTimeByNameStr(const char *filename, CkString &outStr);
	const char *getCreateTimeByNameStr(const char *filename);
	// GETCREATETIMEBYNAMESTR_END

	// GETLASTMODIFIEDTIMEBYNAMESTR_BEGIN
	// eventCallbacks
	bool GetLastModifiedTimeByNameStr(const char *filename, CkString &outStr);
	const char *getLastModifiedTimeByNameStr(const char *filename);
	// GETLASTMODIFIEDTIMEBYNAMESTR_END

	// GETLASTACCESSTIMEBYNAMESTR_BEGIN
	// eventCallbacks
	bool GetLastAccessTimeByNameStr(const char *filename, CkString &outStr);
	const char *getLastAccessTimeByNameStr(const char *filename);
	// GETLASTACCESSTIMEBYNAMESTR_END

	// GETLASTMODIFIEDTIMESTR_BEGIN
	// eventCallbacks
	bool GetLastModifiedTimeStr(int index, CkString &outStr);
	const char *getLastModifiedTimeStr(int index);
	// GETLASTMODIFIEDTIMESTR_END

	// GETLASTACCESSTIMESTR_BEGIN
	// eventCallbacks
	bool GetLastAccessTimeStr(int index, CkString &outStr);
	const char *getLastAccessTimeStr(int index);
	// GETLASTACCESSTIMESTR_END

	// GETCREATETIMESTR_BEGIN
	// eventCallbacks
	bool GetCreateTimeStr(int index, CkString &outStr);
	const char *getCreateTimeStr(int index);
	// GETCREATETIMESTR_END



	// LOGINAFTERCONNECTONLY_BEGIN
	// eventCallbacks
	bool LoginAfterConnectOnly(void);
	// LOGINAFTERCONNECTONLY_END

	// CONNECTONLY_BEGIN
	// eventCallbacks
	bool ConnectOnly(void);
	// CONNECTONLY_END

	// SETREMOTEFILEDT_BEGIN
	// eventCallbacks
	bool SetRemoteFileDt(CkDateTime &dt, const char *remoteFilename);
	// SETREMOTEFILEDT_END

	// GETLASTACCESSDT_BEGIN
	// eventCallbacks
	CkDateTime *GetLastAccessDt(int index);
	// GETLASTACCESSDT_END

	// GETLASTMODDT_BEGIN
	// eventCallbacks
	CkDateTime *GetLastModDt(int index);
	// GETLASTMODDT_END

	// GETCREATEDT_BEGIN
	// eventCallbacks
	CkDateTime *GetCreateDt(int index);
	// GETCREATEDT_END

	// GETCREATEDTBYNAME_BEGIN
	// eventCallbacks
	CkDateTime *GetCreateDtByName(const char *filename);
	// GETCREATEDTBYNAME_END

	// GETLASTACCESSDTBYNAME_BEGIN
	// eventCallbacks
	CkDateTime *GetLastAccessDtByName(const char *filename);
	// GETLASTACCESSDTBYNAME_END

	// GETLASTMODDTBYNAME_BEGIN
	// eventCallbacks
	CkDateTime *GetLastModDtByName(const char *filename);
	// GETLASTMODDTBYNAME_END



	void SleepMs(int millisec) const;

	bool UnlockComponent(const char *code);
	bool IsUnlocked(void) const;

	long get_BandwidthThrottleUp(void);
	void put_BandwidthThrottleUp(long bytesPerSec);
	long get_BandwidthThrottleDown(void);
	void put_BandwidthThrottleDown(long bytesPerSec);

	CkFtpProgress *get_EventCallbackObject(void) const;
	void put_EventCallbackObject(CkFtpProgress *progress);

	// eventCallbacks
	bool get_IsConnected(void);

	bool get_PartialTransfer(void);
	bool get_LoginVerified(void);
	bool get_ConnectVerified(void);

	// Return the number of files deleted.
	// eventCallbacks
	long DeleteMatching(const char *remotePattern);

	// Return the number of files retrieved.
	// eventCallbacks
	long MGetFiles(const char *remotePattern, const char *localDir);
	// Return the number of files uploaded.
	// eventCallbacks
	long MPutFiles(const char *pattern);

	// eventCallbacks
	bool PutTree(const char *localDir);

	// eventCallbacks
	bool GetFile(const char *remoteFilename,  const char *localPath);
	// eventCallbacks
	bool PutFile(const char *localPath,  const char *remoteFilename);

	// eventCallbacks
	bool PutFileFromBinaryData(const char *remoteFilename, const CkByteData &binaryData);
	// eventCallbacks
	bool PutFileFromTextData(const char *remoteFilename, const char *textData, const char *charset);

	// eventCallbacks
	bool GetRemoteFileBinaryData(const char *remoteFilename, CkByteData &outData);
	// eventCallbacks
	bool GetRemoteFileTextData(const char *remoteFilename, CkString &outStr);
	// eventCallbacks
	bool GetRemoteFileTextC(const char *remoteFilename, const char *charset, CkString &outStr);

	// eventCallbacks
	bool AppendFile(const char *localPath,  const char *remoteFilename);
	// eventCallbacks
	bool AppendFileFromBinaryData(const char *remoteFilename, const CkByteData &binaryData);
	// eventCallbacks
	bool AppendFileFromTextData(const char *remoteFilename, const char *textData, const char *charset);

	// eventCallbacks
	bool RenameRemoteFile(const char *existingFilename, const char *newFilename);
	// eventCallbacks
	bool DeleteRemoteFile(const char *filename);
	// eventCallbacks
	bool RemoveRemoteDir(const char *dir);
	// eventCallbacks
	bool CreateRemoteDir(const char *dir);

	// eventCallbacks
	bool Disconnect(void);
	// eventCallbacks
	bool Connect(void);

	// eventCallbacks
	bool GetCurrentRemoteDir(CkString &outStr);
	// eventCallbacks
	bool ChangeRemoteDir(const char *relativeDirPath);

	// eventCallbacks
	bool ClearControlChannel(void);

	void get_DirListingCharset(CkString &str);
	void put_DirListingCharset(const char *newVal);

	void get_Account(CkString &str);
	void put_Account(const char *newVal);
	const char *account(void);

	void get_ListPattern(CkString &strPattern);
	void put_ListPattern(const char *pattern);
	void get_Password(CkString &str);
	void put_Password(const char *newVal);
	void get_Username(CkString &str);
	void put_Username(const char *newVal);
	unsigned long get_AllocateSize(void);
	void put_AllocateSize(unsigned long newVal);
	long get_Port(void);
	void put_Port(long newVal);
	long get_ActivePortRangeStart(void);
	void put_ActivePortRangeStart(long newVal);
	long get_ActivePortRangeEnd(void);
	void put_ActivePortRangeEnd(long newVal);
	long get_ProxyPort(void);
	void put_ProxyPort(long newVal);
	void get_Hostname(CkString &str);
	void put_Hostname(const char *newVal);
	void get_SessionLog(CkString &str);
	bool get_Passive(void);
	void put_Passive(bool newVal);
	bool get_Ssl(void);
	void put_Ssl(bool newVal);
	bool get_AuthTls(void);
	void put_AuthTls(bool newVal);
	bool get_KeepSessionLog(void);
	void put_KeepSessionLog(bool newVal);
	bool get_PassiveUseHostAddr(void);
	void put_PassiveUseHostAddr(bool newVal);

	bool get_HasModeZ(void);

	bool get_RestartNext(void);
	void put_RestartNext(bool newVal);

	bool get_AutoXcrc(void);
	void put_AutoXcrc(bool newVal);
	bool get_AutoFeat(void);
	void put_AutoFeat(bool newVal);
	bool get_AutoSyst(void);
	void put_AutoSyst(bool newVal);
	// eventCallbacks
	bool Feat(CkString &outStr);
	const char *feat(void);
	

	// eventCallbacks
	bool GetSizeStr(long index, CkString &outStr);
	// eventCallbacks
	long GetSize(long index);
	// eventCallbacks
	bool GetFilename(long index, CkString &outStr);
	// eventCallbacks
	bool GetIsDirectory(long index);
	// eventCallbacks
	bool GetIsSymbolicLink(long index);

	void ClearDirCache(void);

	// eventCallbacks
	long GetSizeByName(const char *filname);

	// eventCallbacks
	bool GetCreateFTime(long index, FILETIME &outFileTime);
	// eventCallbacks
	bool GetLastAccessFTime(long index, FILETIME &outFileTime);
	// eventCallbacks
	bool GetLastModifiedFTime(long index, FILETIME &outFileTime);

	// eventCallbacks
	bool GetCreateTime(long index, SYSTEMTIME &outSysTime);
	// eventCallbacks
	bool GetLastAccessTime(long index, SYSTEMTIME &outSysTime);
	// eventCallbacks
	bool GetLastModifiedTime(long index, SYSTEMTIME &outSysTime);

	// eventCallbacks
	long get_NumFilesAndDirs(void);

	void get_ProxyHostname(CkString &str);
	void put_ProxyHostname(const char *newVal);
	void get_ProxyUsername(CkString &str);
	void put_ProxyUsername(const char *newVal);
	void get_ProxyPassword(CkString &str);
	void put_ProxyPassword(const char *newVal);
	long get_ProxyMethod(void);
	void put_ProxyMethod(long newVal);

	void ClearSessionLog(void);

	// eventCallbacks
	bool SetTypeBinary(void);
	// eventCallbacks
	bool SetTypeAscii(void);
	// eventCallbacks
	bool SetModeZ(void);

	// eventCallbacks
	bool Quote(const char *cmd);
	// eventCallbacks
	bool Site(const char *parms);
	// eventCallbacks
	bool Syst(CkString &outStr);
	// eventCallbacks
	bool Stat(CkString &outStr);

	// 0 = success
	// 1-299 = underlying reasons
	// Normal sockets:
	//  1 = empty hostname
	//  2 = DNS lookup failed
	//  3 = DNS timeout
	//  4 = Aborted by application.
	//  5 = Internal failure.
	//  6 = Connect Timed Out
	//  7 = Connect Rejected (or failed for some other reason)
	// SSL:
	//  100 = Internal schannel error
	//  101 = Failed to create credentials
	//  102 = Failed to send initial message to proxy.
	//  103 = Handshake failed.
	//  104 = Failed to obtain remote certificate.
	// Protocol/Component:
	// 300 = asynch op in progress
	// 301 = login failure.
	long get_ConnectFailReason(void);

	// Timeouts are in seconds
	long get_ReadTimeout(void);
	void put_ReadTimeout(long numSeconds);

	// Timeouts are in seconds
	long get_ConnectTimeout(void);
	void put_ConnectTimeout(long numSeconds);

	// These times are in milliseconds.
	long get_HeartbeatMs(void);
	void put_HeartbeatMs(long millisec);

	long get_IdleTimeoutMs(void);
	void put_IdleTimeoutMs(long millisec);

	// CRLF mode for text (ASCII-mode) file downloads
	// 0 = no conversion, 1 = CR+LF, 2 = LF Only, 3 = CR Only
	long get_CrlfMode(void);
	void put_CrlfMode(long newVal);

	// These values are in bytes/second
	long get_UploadRate(void);
	long get_DownloadRate(void);


	// Progress Monitoring size (in bytes) for FTP servers that do not
	// respond with size information for GETs.
	long get_ProgressMonSize(void);
	void put_ProgressMonSize(long newVal);

	__int64 get_ProgressMonSize64(void);
	void put_ProgressMonSize64(__int64 newVal);

	// eventCallbacks
	bool GetXmlDirListing(const char *pattern, CkString &outStrXmlListing);
	// eventCallbacks
	bool GetTextDirListing(const char *pattern, CkString &outStrRawListing);

	// eventCallbacks
	bool DownloadTree(const char *localRoot);
	// eventCallbacks
	bool DeleteTree(void);

	// Fetch the remote directory (rooted at the current remote directory) tree as XML
	// eventCallbacks
	bool DirTreeXml(CkString &outStrXml);
	const char *dirTreeXml(void);

	// Copy local files to the FTP server to synchronize a directory tree.
	// mode: 0 = all files, 1 = only missing files, 2 = missing and newer files, 3 = only newer files (already existing)
	// eventCallbacks
	bool SyncRemoteTree(const char *localRoot, int mode);

	// Copy remote files from an FTP server to synchronize a directory tree.
	// mode: 0 = all files, 1 = only missing files, 2 = missing and newer files, 3 = only newer files (already existing)
	// eventCallbacks
	bool SyncLocalTree(const char *localRoot, int mode);


	// Asynchronous FTP
	bool AsyncGetFileStart(const char *remoteFilename,  const char *localFilename);
	bool AsyncPutFileStart(const char *localPath,  const char *remoteFilename);
	bool AsyncAppendFileStart(const char *localPath,  const char *remoteFilename);

	void AsyncAbort(void);	// Abort the async operation.

	bool get_AsyncFinished(void);	// True if finished, false if in progress.
	void get_AsyncLog(CkString &strLog);
	bool get_AsyncSuccess(void);	// True if successful, false if aborted or failed.

	// Can be called while the asynchronous upload or download is in progress to
	// get the bytes sent/received so far..
	unsigned long get_AsyncBytesReceived(void);
	unsigned long get_AsyncBytesSent(void);

	__int64 get_AsyncBytesReceived64(void);
	__int64 get_AsyncBytesSent64(void);

	// eventCallbacks
	bool CreatePlan(const char *localDir, CkString &outStr);
	//bool PutTree(const char *localDir);
	// eventCallbacks
	bool PutPlan(const char *planUtf8, const char *planLogFilePath);

	bool get_SslServerCertVerified(void);

	CkCert *GetSslServerCert(void);
	bool SetSslClientCert(CkCert &cert);
	bool SetSslClientCertPfx(const char *pfxPath, const char *pfxPassword);

	bool get_RequireSslCertVerify(void);
	void put_RequireSslCertVerify(bool newVal);
	void SetSslCertRequirement(const char *name, const char *value);

	bool get_AuthSsl(void);
	void put_AuthSsl(bool newVal);

	int DetermineProxyMethod(void);
	bool DetermineSettings(CkString &outXmlReport);
	const char *determineSettings(void);

	// eventCallbacks
	bool SendCommand(const char *cmd, CkString &outReply);
	const char *sendCommand(const char *cmd);


	const char *getFilename(long index);
	const char *listPattern(void);
	const char *password(void);
	const char *username(void);
	const char *hostname(void);
	const char *sessionLog(void);
	const char *getXmlDirListing(const char *pattern);
	const char *getTextDirListing(const char *pattern);
	const char *getRemoteFileTextData(const char *remoteFilename);
	const char *getRemoteFileTextC(const char *remoteFilename, const char *charset);
	const char *createPlan(const char *localDir);

	const char *getCurrentRemoteDir(void);
	const char *getSizeStr(long index);
	const char *proxyHostname(void);
	const char *proxyUsername(void);
	const char *proxyPassword(void);
	const char *syst(void);
	const char *ck_stat(void);
	const char *asyncLog(void);
	const char *dirListingCharset(void);
	// SYSTEMTIME is defined in <windows.h>
	// For more information, Google "SYSTEMTIME"

	// eventCallbacks
	bool GetCreateTimeByName(const char *filename, SYSTEMTIME &outSysTime);
	// eventCallbacks
	bool GetLastAccessTimeByName(const char *filename, SYSTEMTIME &outSysTime);
	// eventCallbacks
	bool GetLastModifiedTimeByName(const char *filename, SYSTEMTIME &outSysTime);

	// NOOP_BEGIN
	// eventCallbacks
	bool Noop(void);
	// NOOP_END
	// SETOLDESTDATE_BEGIN
	void SetOldestDate(SYSTEMTIME &oldestDateTime);
	// SETOLDESTDATE_END
	// CONVERTTOTLS_BEGIN
	// eventCallbacks
	bool ConvertToTls(void);
	// CONVERTTOTLS_END
	// GETSIZESTRBYNAME_BEGIN
	// eventCallbacks
	bool GetSizeStrByName(const char *filename, CkString &outStr);
	const char *getSizeStrByName(const char *filename);
	// GETSIZESTRBYNAME_END
	// SETREMOTEFILEDATETIME_BEGIN
	// SYSTEMTIME is defined in <windows.h>
	// For more information, Google "SYSTEMTIME"
	// eventCallbacks
	bool SetRemoteFileDateTime(SYSTEMTIME &dateTime, const char *remoteFilename);
	// SETREMOTEFILEDATETIME_END
	// SENDBUFFERSIZE_BEGIN
	int get_SendBufferSize(void);
	void put_SendBufferSize(int newVal);
	// SENDBUFFERSIZE_END
	// USEEPSV_BEGIN
	bool get_UseEpsv(void);
	void put_UseEpsv(bool newVal);
	// USEEPSV_END
	// FORCEPORTIPADDRESS_BEGIN
	void get_ForcePortIpAddress(CkString &str);
	const char *forcePortIpAddress(void);
	void put_ForcePortIpAddress(const char *newVal);
	// FORCEPORTIPADDRESS_END
	// NLSTXML_BEGIN
	// eventCallbacks
	bool NlstXml(const char *pattern, CkString &outStr);
	const char *nlstXml(const char *pattern);
	// NLSTXML_END
	// SOCKSVERSION_BEGIN
	int get_SocksVersion(void);
	void put_SocksVersion(int newVal);
	// SOCKSVERSION_END
	// SOCKSPORT_BEGIN
	int get_SocksPort(void);
	void put_SocksPort(int newVal);
	// SOCKSPORT_END
	// SOCKSUSERNAME_BEGIN
	void get_SocksUsername(CkString &str);
	const char *socksUsername(void);
	void put_SocksUsername(const char *newVal);
	// SOCKSUSERNAME_END
	// SOCKSPASSWORD_BEGIN
	void get_SocksPassword(CkString &str);
	const char *socksPassword(void);
	void put_SocksPassword(const char *newVal);
	// SOCKSPASSWORD_END
	// SOCKSHOSTNAME_BEGIN
	void get_SocksHostname(CkString &str);
	const char *socksHostname(void);
	void put_SocksHostname(const char *newVal);
	// SOCKSHOSTNAME_END
	// GREETING_BEGIN
	void get_Greeting(CkString &str);
	const char *greeting(void);
	// GREETING_END
	// VERBOSELOGGING_BEGIN
	bool get_VerboseLogging(void);
	void put_VerboseLogging(bool newVal);
	// VERBOSELOGGING_END
	// SSLPROTOCOL_BEGIN
	void get_SslProtocol(CkString &str);
	const char *sslProtocol(void);
	void put_SslProtocol(const char *newVal);
	// SSLPROTOCOL_END
	// AUTOGETSIZEFORPROGRESS_BEGIN
	bool get_AutoGetSizeForProgress(void);
	void put_AutoGetSizeForProgress(bool newVal);
	// AUTOGETSIZEFORPROGRESS_END
	// SYNCPREVIEW_BEGIN
	void get_SyncPreview(CkString &str);
	const char *syncPreview(void);
	// SYNCPREVIEW_END
	// SYNCREMOTETREE2_BEGIN
	// eventCallbacks
	bool SyncRemoteTree2(const char *localRoot, int mode, bool bDescend, bool bPreviewOnly);
	// SYNCREMOTETREE2_END
	// AUTOFIX_BEGIN
	bool get_AutoFix(void);
	void put_AutoFix(bool newVal);
	// AUTOFIX_END
	// CLIENTIPADDRESS_BEGIN
	void get_ClientIpAddress(CkString &str);
	const char *clientIpAddress(void);
	void put_ClientIpAddress(const char *newVal);
	// CLIENTIPADDRESS_END
	// SETSSLCLIENTCERTPEM_BEGIN
	bool SetSslClientCertPem(const char *pemDataOrFilename, const char *pemPassword);
	// SETSSLCLIENTCERTPEM_END
	// PREFERNLST_BEGIN
	bool get_PreferNlst(void);
	void put_PreferNlst(bool newVal);
	// PREFERNLST_END
	// GETSIZEBYNAME64_BEGIN
	// eventCallbacks
	__int64 GetSizeByName64(const char *filename);
	// GETSIZEBYNAME64_END
	// GETSIZE64_BEGIN
	// eventCallbacks
	__int64 GetSize64(int index);
	// GETSIZE64_END
	// HTTPPROXYAUTHMETHOD_BEGIN
	void get_HttpProxyAuthMethod(CkString &str);
	const char *httpProxyAuthMethod(void);
	void put_HttpProxyAuthMethod(const char *newVal);
	// HTTPPROXYAUTHMETHOD_END
	// HTTPPROXYUSERNAME_BEGIN
	void get_HttpProxyUsername(CkString &str);
	const char *httpProxyUsername(void);
	void put_HttpProxyUsername(const char *newVal);
	// HTTPPROXYUSERNAME_END
	// HTTPPROXYPASSWORD_BEGIN
	void get_HttpProxyPassword(CkString &str);
	const char *httpProxyPassword(void);
	void put_HttpProxyPassword(const char *newVal);
	// HTTPPROXYPASSWORD_END
	// HTTPPROXYDOMAIN_BEGIN
	void get_HttpProxyDomain(CkString &str);  // NTLM authentication domain
	const char *httpProxyDomain(void);
	void put_HttpProxyDomain(const char *newVal);
	// HTTPPROXYDOMAIN_END
	// HTTPPROXYHOSTNAME_BEGIN
	void get_HttpProxyHostname(CkString &str);
	const char *httpProxyHostname(void);
	void put_HttpProxyHostname(const char *newVal);
	// HTTPPROXYHOSTNAME_END
	// HTTPPROXYPORT_BEGIN
	int get_HttpProxyPort(void);
	void put_HttpProxyPort(int newVal);
	// HTTPPROXYPORT_END

	// FTP2_INSERT_POINT

	// END PUBLIC INTERFACE


    // For internal use only.
    private:
	CkFtpProgress *m_callback;

	// Don't allow assignment or copying these objects.
	CkFtp2(const CkFtp2 &) { } 
	CkFtp2 &operator=(const CkFtp2 &) { return *this; }
	CkFtp2(void *impl);


};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif


