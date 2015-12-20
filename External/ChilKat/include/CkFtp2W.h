#ifndef _CkFtp2W_H
#define _CkFtp2W_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkFtp2.h"

class CkCertW;
class CkFtpProgress;
class CkDateTimeW;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkFtp2W : public CkWideCharBase
{
    private:
	CkFtp2 *m_impl;

        // Disallow assignment or copying this object.
	CkFtp2W(const CkFtp2W &) { }
	CkFtp2W &operator=(const CkFtp2W &) { return *this; }


    public:
	CkFtp2W()
	    {
	    m_impl = new CkFtp2;
	    m_impl->put_Utf8(true);
	    }
	CkFtp2W(CkFtp2 *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkFtp2;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkFtp2W(bool bForMono)
	    {
	    m_impl = new CkFtp2;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkFtp2 *getMbPtr(void) { return m_impl; }
	CkFtp2 &getMbRef(void) { return *m_impl; }
	const CkFtp2 *getMbConstPtr(void) const { return m_impl; }
	const CkFtp2 &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkFtp2W() { delete m_impl; m_impl = 0; }

void get_Account(CkString &propVal);
void put_Account(const wchar_t *w1);
long get_ActivePortRangeEnd(void);
void put_ActivePortRangeEnd(long propVal);
long get_ActivePortRangeStart(void);
void put_ActivePortRangeStart(long propVal);
unsigned long get_AllocateSize(void);
void put_AllocateSize(unsigned long propVal);
unsigned long get_AsyncBytesReceived(void);
__int64 get_AsyncBytesReceived64(void);
void get_AsyncBytesReceivedStr(CkString &propVal);
unsigned long get_AsyncBytesSent(void);
__int64 get_AsyncBytesSent64(void);
void get_AsyncBytesSentStr(CkString &propVal);
bool get_AsyncFinished(void);
void get_AsyncLog(CkString &propVal);
bool get_AsyncSuccess(void);
bool get_AuthSsl(void);
void put_AuthSsl(bool propVal);
bool get_AuthTls(void);
void put_AuthTls(bool propVal);
bool get_AutoFeat(void);
void put_AutoFeat(bool propVal);
bool get_AutoFix(void);
void put_AutoFix(bool propVal);
bool get_AutoGetSizeForProgress(void);
void put_AutoGetSizeForProgress(bool propVal);
bool get_AutoSyst(void);
void put_AutoSyst(bool propVal);
bool get_AutoXcrc(void);
void put_AutoXcrc(bool propVal);
long get_BandwidthThrottleDown(void);
void put_BandwidthThrottleDown(long propVal);
long get_BandwidthThrottleUp(void);
void put_BandwidthThrottleUp(long propVal);
void get_ClientIpAddress(CkString &propVal);
void put_ClientIpAddress(const wchar_t *w1);
long get_ConnectFailReason(void);
long get_ConnectTimeout(void);
void put_ConnectTimeout(long propVal);
bool get_ConnectVerified(void);
long get_CrlfMode(void);
void put_CrlfMode(long propVal);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void get_DirListingCharset(CkString &propVal);
void put_DirListingCharset(const wchar_t *w1);
long get_DownloadRate(void);
CkFtpProgress *get_EventCallbackObject(void);
void put_EventCallbackObject(CkFtpProgress * propVal);
void get_ForcePortIpAddress(CkString &propVal);
void put_ForcePortIpAddress(const wchar_t *w1);
void get_Greeting(CkString &propVal);
bool get_HasModeZ(void);
long get_HeartbeatMs(void);
void put_HeartbeatMs(long propVal);
void get_Hostname(CkString &propVal);
void put_Hostname(const wchar_t *w1);
void get_HttpProxyAuthMethod(CkString &propVal);
void put_HttpProxyAuthMethod(const wchar_t *w1);
void get_HttpProxyDomain(CkString &propVal);
void put_HttpProxyDomain(const wchar_t *w1);
void get_HttpProxyHostname(CkString &propVal);
void put_HttpProxyHostname(const wchar_t *w1);
void get_HttpProxyPassword(CkString &propVal);
void put_HttpProxyPassword(const wchar_t *w1);
int get_HttpProxyPort(void);
void put_HttpProxyPort(int propVal);
void get_HttpProxyUsername(CkString &propVal);
void put_HttpProxyUsername(const wchar_t *w1);
long get_IdleTimeoutMs(void);
void put_IdleTimeoutMs(long propVal);
bool get_IsConnected(void);
bool get_KeepSessionLog(void);
void put_KeepSessionLog(bool propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
void get_ListPattern(CkString &propVal);
void put_ListPattern(const wchar_t *w1);
bool get_LoginVerified(void);
long get_NumFilesAndDirs(void);
bool get_PartialTransfer(void);
bool get_Passive(void);
void put_Passive(bool propVal);
bool get_PassiveUseHostAddr(void);
void put_PassiveUseHostAddr(bool propVal);
void get_Password(CkString &propVal);
void put_Password(const wchar_t *w1);
long get_Port(void);
void put_Port(long propVal);
bool get_PreferNlst(void);
void put_PreferNlst(bool propVal);
long get_ProgressMonSize(void);
void put_ProgressMonSize(long propVal);
__int64 get_ProgressMonSize64(void);
void put_ProgressMonSize64(__int64 propVal);
void get_ProxyHostname(CkString &propVal);
void put_ProxyHostname(const wchar_t *w1);
long get_ProxyMethod(void);
void put_ProxyMethod(long propVal);
void get_ProxyPassword(CkString &propVal);
void put_ProxyPassword(const wchar_t *w1);
long get_ProxyPort(void);
void put_ProxyPort(long propVal);
void get_ProxyUsername(CkString &propVal);
void put_ProxyUsername(const wchar_t *w1);
long get_ReadTimeout(void);
void put_ReadTimeout(long propVal);
bool get_RequireSslCertVerify(void);
void put_RequireSslCertVerify(bool propVal);
bool get_RestartNext(void);
void put_RestartNext(bool propVal);
int get_SendBufferSize(void);
void put_SendBufferSize(int propVal);
void get_SessionLog(CkString &propVal);
bool get_SkipFinalReply(void);
void put_SkipFinalReply(bool propVal);
int get_SoRcvBuf(void);
void put_SoRcvBuf(int propVal);
int get_SoSndBuf(void);
void put_SoSndBuf(int propVal);
void get_SocksHostname(CkString &propVal);
void put_SocksHostname(const wchar_t *w1);
void get_SocksPassword(CkString &propVal);
void put_SocksPassword(const wchar_t *w1);
int get_SocksPort(void);
void put_SocksPort(int propVal);
void get_SocksUsername(CkString &propVal);
void put_SocksUsername(const wchar_t *w1);
int get_SocksVersion(void);
void put_SocksVersion(int propVal);
bool get_Ssl(void);
void put_Ssl(bool propVal);
void get_SslProtocol(CkString &propVal);
void put_SslProtocol(const wchar_t *w1);
bool get_SslServerCertVerified(void);
void get_SyncPreview(CkString &propVal);
long get_UploadRate(void);
bool get_UseEpsv(void);
void put_UseEpsv(bool propVal);
void get_Username(CkString &propVal);
void put_Username(const wchar_t *w1);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
bool AppendFile(const wchar_t *w1,const wchar_t *w2);
bool AppendFileFromBinaryData(const wchar_t *w1,const CkByteData &binaryData);
bool AppendFileFromTextData(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
void AsyncAbort(void);
bool AsyncAppendFileStart(const wchar_t *w1,const wchar_t *w2);
bool AsyncGetFileStart(const wchar_t *w1,const wchar_t *w2);
bool AsyncPutFileStart(const wchar_t *w1,const wchar_t *w2);
bool ChangeRemoteDir(const wchar_t *w1);
bool ClearControlChannel(void);
void ClearDirCache(void);
void ClearSessionLog(void);
bool Connect(void);
bool ConnectOnly(void);
bool ConvertToTls(void);
bool CreatePlan(const wchar_t *w1,CkString &outStr);
bool CreateRemoteDir(const wchar_t *w1);
long DeleteMatching(const wchar_t *w1);
bool DeleteRemoteFile(const wchar_t *w1);
bool DeleteTree(void);
int DetermineProxyMethod(void);
bool DetermineSettings(CkString &outXmlReport);
bool DirTreeXml(CkString &outStrXml);
bool Disconnect(void);
bool DownloadTree(const wchar_t *w1);
bool Feat(CkString &outStr);
CkDateTimeW *GetCreateDt(int index);
CkDateTimeW *GetCreateDtByName(const wchar_t *w1);
bool GetCreateFTime(long index,FILETIME &outFileTime);
bool GetCreateTime(long index,SYSTEMTIME &outSysTime);
bool GetCreateTimeByName(const wchar_t *w1,SYSTEMTIME &outSysTime);
bool GetCreateTimeByNameStr(const wchar_t *w1,CkString &outStr);
bool GetCreateTimeStr(int index,CkString &outStr);
bool GetCurrentRemoteDir(CkString &outStr);
bool GetFile(const wchar_t *w1,const wchar_t *w2);
bool GetFilename(long index,CkString &outStr);
bool GetIsDirectory(long index);
bool GetIsSymbolicLink(long index);
CkDateTimeW *GetLastAccessDt(int index);
CkDateTimeW *GetLastAccessDtByName(const wchar_t *w1);
bool GetLastAccessFTime(long index,FILETIME &outFileTime);
bool GetLastAccessTime(long index,SYSTEMTIME &outSysTime);
bool GetLastAccessTimeByName(const wchar_t *w1,SYSTEMTIME &outSysTime);
bool GetLastAccessTimeByNameStr(const wchar_t *w1,CkString &outStr);
bool GetLastAccessTimeStr(int index,CkString &outStr);
CkDateTimeW *GetLastModDt(int index);
CkDateTimeW *GetLastModDtByName(const wchar_t *w1);
bool GetLastModifiedFTime(long index,FILETIME &outFileTime);
bool GetLastModifiedTime(long index,SYSTEMTIME &outSysTime);
bool GetLastModifiedTimeByName(const wchar_t *w1,SYSTEMTIME &outSysTime);
bool GetLastModifiedTimeByNameStr(const wchar_t *w1,CkString &outStr);
bool GetLastModifiedTimeStr(int index,CkString &outStr);
bool GetRemoteFileBinaryData(const wchar_t *w1,CkByteData &outData);
bool GetRemoteFileTextC(const wchar_t *w1,const wchar_t *w2,CkString &outStr);
bool GetRemoteFileTextData(const wchar_t *w1,CkString &outStr);
long GetSize(long index);
__int64 GetSize64(int index);
long GetSizeByName(const wchar_t *w1);
__int64 GetSizeByName64(const wchar_t *w1);
bool GetSizeStr(long index,CkString &outStr);
bool GetSizeStrByName(const wchar_t *w1,CkString &outStr);
CkCertW *GetSslServerCert(void);
bool GetTextDirListing(const wchar_t *w1,CkString &outStrRawListing);
bool GetXmlDirListing(const wchar_t *w1,CkString &outStrXmlListing);
bool IsUnlocked(void);
bool LoginAfterConnectOnly(void);
long MGetFiles(const wchar_t *w1,const wchar_t *w2);
long MPutFiles(const wchar_t *w1);
bool NlstXml(const wchar_t *w1,CkString &outStr);
bool Noop(void);
bool PutFile(const wchar_t *w1,const wchar_t *w2);
bool PutFileFromBinaryData(const wchar_t *w1,const CkByteData &binaryData);
bool PutFileFromTextData(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
bool PutPlan(const wchar_t *w1,const wchar_t *w2);
bool PutTree(const wchar_t *w1);
bool Quote(const wchar_t *w1);
bool RemoveRemoteDir(const wchar_t *w1);
bool RenameRemoteFile(const wchar_t *w1,const wchar_t *w2);
bool SaveLastError(const wchar_t *w1);
bool SendCommand(const wchar_t *w1,CkString &outReply);
bool SetModeZ(void);
void SetOldestDate(SYSTEMTIME &oldestDateTime);
void SetOldestDateStr(const wchar_t *w1);
bool SetRemoteFileDateTime(SYSTEMTIME &dateTime,const wchar_t *w1);
bool SetRemoteFileDateTimeStr(const wchar_t *w1,const wchar_t *w2);
bool SetRemoteFileDt(CkDateTimeW &dt,const wchar_t *w1);
void SetSslCertRequirement(const wchar_t *w1,const wchar_t *w2);
bool SetSslClientCert(CkCertW &cert);
bool SetSslClientCertPem(const wchar_t *w1,const wchar_t *w2);
bool SetSslClientCertPfx(const wchar_t *w1,const wchar_t *w2);
bool SetTypeAscii(void);
bool SetTypeBinary(void);
bool Site(const wchar_t *w1);
void SleepMs(int millisec);
bool Stat(CkString &outStr);
bool SyncLocalTree(const wchar_t *w1,int mode);
bool SyncRemoteTree(const wchar_t *w1,int mode);
bool SyncRemoteTree2(const wchar_t *w1,int mode,bool bDescend,bool bPreviewOnly);
bool Syst(CkString &outStr);
bool UnlockComponent(const wchar_t *w1);
const wchar_t *account(void);
const wchar_t *asyncBytesReceivedStr(void);
const wchar_t *asyncBytesSentStr(void);
const wchar_t *asyncLog(void);
const wchar_t *ck_stat(void);
const wchar_t *clientIpAddress(void);
const wchar_t *createPlan(const wchar_t *w1);
const wchar_t *debugLogFilePath(void);
const wchar_t *determineSettings(void);
const wchar_t *dirListingCharset(void);
const wchar_t *dirTreeXml(void);
const wchar_t *feat(void);
const wchar_t *forcePortIpAddress(void);
const wchar_t *getCreateTimeByNameStr(const wchar_t *w1);
const wchar_t *getCreateTimeStr(int index);
const wchar_t *getCurrentRemoteDir(void);
const wchar_t *getFilename(long index);
const wchar_t *getLastAccessTimeByNameStr(const wchar_t *w1);
const wchar_t *getLastAccessTimeStr(int index);
const wchar_t *getLastModifiedTimeByNameStr(const wchar_t *w1);
const wchar_t *getLastModifiedTimeStr(int index);
const wchar_t *getRemoteFileTextC(const wchar_t *w1,const wchar_t *w2);
const wchar_t *getRemoteFileTextData(const wchar_t *w1);
const wchar_t *getSizeStr(long index);
const wchar_t *getSizeStrByName(const wchar_t *w1);
const wchar_t *getTextDirListing(const wchar_t *w1);
const wchar_t *getXmlDirListing(const wchar_t *w1);
const wchar_t *greeting(void);
const wchar_t *hostname(void);
const wchar_t *httpProxyAuthMethod(void);
const wchar_t *httpProxyDomain(void);
const wchar_t *httpProxyHostname(void);
const wchar_t *httpProxyPassword(void);
const wchar_t *httpProxyUsername(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *listPattern(void);
const wchar_t *nlstXml(const wchar_t *w1);
const wchar_t *password(void);
const wchar_t *proxyHostname(void);
const wchar_t *proxyPassword(void);
const wchar_t *proxyUsername(void);
const wchar_t *sendCommand(const wchar_t *w1);
const wchar_t *sessionLog(void);
const wchar_t *socksHostname(void);
const wchar_t *socksPassword(void);
const wchar_t *socksUsername(void);
const wchar_t *sslProtocol(void);
const wchar_t *syncPreview(void);
const wchar_t *syst(void);
const wchar_t *username(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
