#ifndef _CkSFtpW_H
#define _CkSFtpW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkSFtp.h"

class CkSFtpProgress;
class CkSFtpDirW;
class CkDateTimeW;
class CkSshKeyW;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkSFtpW : public CkWideCharBase
{
    private:
	CkSFtp *m_impl;

        // Disallow assignment or copying this object.
	CkSFtpW(const CkSFtpW &) { }
	CkSFtpW &operator=(const CkSFtpW &) { return *this; }


    public:
	CkSFtpW()
	    {
	    m_impl = new CkSFtp;
	    m_impl->put_Utf8(true);
	    }
	CkSFtpW(CkSFtp *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkSFtp;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkSFtpW(bool bForMono)
	    {
	    m_impl = new CkSFtp;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkSFtp *getMbPtr(void) { return m_impl; }
	CkSFtp &getMbRef(void) { return *m_impl; }
	const CkSFtp *getMbConstPtr(void) const { return m_impl; }
	const CkSFtp &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkSFtpW() { delete m_impl; m_impl = 0; }

void get_AccumulateBuffer(CkByteData &propVal);
void get_ClientIdentifier(CkString &propVal);
void put_ClientIdentifier(const wchar_t *w1);
void get_ClientIpAddress(CkString &propVal);
void put_ClientIpAddress(const wchar_t *w1);
int get_ConnectTimeoutMs(void);
void put_ConnectTimeoutMs(int propVal);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
int get_DisconnectCode(void);
void get_DisconnectReason(CkString &propVal);
bool get_EnableCache(void);
void put_EnableCache(bool propVal);
CkSFtpProgress *get_EventCallbackObject(void);
void put_EventCallbackObject(CkSFtpProgress * propVal);
void get_FilenameCharset(CkString &propVal);
void put_FilenameCharset(const wchar_t *w1);
void get_ForceCipher(CkString &propVal);
void put_ForceCipher(const wchar_t *w1);
bool get_ForceV3(void);
void put_ForceV3(bool propVal);
int get_HeartbeatMs(void);
void put_HeartbeatMs(int propVal);
void get_HostKeyAlg(CkString &propVal);
void put_HostKeyAlg(const wchar_t *w1);
void get_HostKeyFingerprint(CkString &propVal);
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
int get_IdleTimeoutMs(void);
void put_IdleTimeoutMs(int propVal);
bool get_IncludeDotDirs(void);
void put_IncludeDotDirs(bool propVal);
int get_InitializeFailCode(void);
void get_InitializeFailReason(CkString &propVal);
bool get_IsConnected(void);
bool get_KeepSessionLog(void);
void put_KeepSessionLog(bool propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
int get_MaxPacketSize(void);
void put_MaxPacketSize(int propVal);
bool get_PasswordChangeRequested(void);
bool get_PreserveDate(void);
void put_PreserveDate(bool propVal);
int get_ProtocolVersion(void);
void get_SessionLog(CkString &propVal);
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
bool get_TcpNoDelay(void);
void put_TcpNoDelay(bool propVal);
int get_UploadChunkSize(void);
void put_UploadChunkSize(int propVal);
bool get_UtcMode(void);
void put_UtcMode(bool propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
int AccumulateBytes(const wchar_t *w1,int maxBytes);
bool Add64(const wchar_t *w1,const wchar_t *w2,CkString &outStr);
bool AuthenticatePk(const wchar_t *w1,CkSshKeyW &privateKey);
bool AuthenticatePw(const wchar_t *w1,const wchar_t *w2);
bool AuthenticatePwPk(const wchar_t *w1,const wchar_t *w2,CkSshKeyW &privateKey);
void ClearAccumulateBuffer(void);
void ClearCache(void);
void ClearSessionLog(void);
bool CloseHandle(const wchar_t *w1);
bool Connect(const wchar_t *w1,int port);
bool CopyFileAttr(const wchar_t *w1,const wchar_t *w2,bool bIsHandle);
bool CreateDir(const wchar_t *w1);
void Disconnect(void);
bool DownloadFile(const wchar_t *w1,const wchar_t *w2);
bool DownloadFileByName(const wchar_t *w1,const wchar_t *w2);
bool Eof(const wchar_t *w1);
CkDateTimeW *GetFileCreateDt(const wchar_t *w1,bool bFollowLinks,bool bIsHandle);
bool GetFileCreateTime(const wchar_t *w1,bool bFollowLinks,bool bIsHandle,SYSTEMTIME &outSysTime);
bool GetFileCreateTimeStr(const wchar_t *w1,bool bFollowLinks,bool bIsHandle,CkString &outStr);
bool GetFileGroup(const wchar_t *w1,bool bFollowLinks,bool bIsHandle,CkString &outStr);
bool GetFileLastAccess(const wchar_t *w1,bool bFollowLinks,bool bIsHandle,SYSTEMTIME &outSysTime);
CkDateTimeW *GetFileLastAccessDt(const wchar_t *w1,bool bFollowLinks,bool bIsHandle);
bool GetFileLastAccessStr(const wchar_t *w1,bool bFollowLinks,bool bIsHandle,CkString &outStr);
bool GetFileLastModified(const wchar_t *w1,bool bFollowLinks,bool bIsHandle,SYSTEMTIME &outSysTime);
CkDateTimeW *GetFileLastModifiedDt(const wchar_t *w1,bool bFollowLinks,bool bIsHandle);
bool GetFileLastModifiedStr(const wchar_t *w1,bool bFollowLinks,bool bIsHandle,CkString &outStr);
bool GetFileOwner(const wchar_t *w1,bool bFollowLinks,bool bIsHandle,CkString &outStr);
int GetFilePermissions(const wchar_t *w1,bool bFollowLinks,bool bIsHandle);
int GetFileSize32(const wchar_t *w1,bool bFollowLinks,bool bIsHandle);
__int64 GetFileSize64(const wchar_t *w1,bool bFollowLinks,bool bIsHandle);
bool GetFileSizeStr(const wchar_t *w1,bool bFollowLinks,bool bIsHandle,CkString &outStr);
bool InitializeSftp(void);
bool LastReadFailed(const wchar_t *w1);
int LastReadNumBytes(const wchar_t *w1);
bool OpenDir(const wchar_t *w1,CkString &outStr);
bool OpenFile(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,CkString &outStr);
CkSFtpDirW *ReadDir(const wchar_t *w1);
bool ReadFileBytes(const wchar_t *w1,int numBytes,CkByteData &outBytes);
bool ReadFileBytes32(const wchar_t *w1,int offset,int numBytes,CkByteData &outBytes);
bool ReadFileBytes64(const wchar_t *w1,__int64 offset64,int numBytes,CkByteData &outBytes);
bool ReadFileBytes64s(const wchar_t *w1,const wchar_t *w2,int numBytes,CkByteData &outBytes);
bool ReadFileText(const wchar_t *w1,int numBytes,const wchar_t *w2,CkString &outStr);
bool ReadFileText32(const wchar_t *w1,int offset32,int numBytes,const wchar_t *w2,CkString &outStr);
bool ReadFileText64(const wchar_t *w1,__int64 offset64,int numBytes,const wchar_t *w2,CkString &outStr);
bool ReadFileText64s(const wchar_t *w1,const wchar_t *w2,int numBytes,const wchar_t *w3,CkString &outStr);
bool RealPath(const wchar_t *w1,const wchar_t *w2,CkString &outStr);
bool RemoveDir(const wchar_t *w1);
bool RemoveFile(const wchar_t *w1);
bool RenameFileOrDir(const wchar_t *w1,const wchar_t *w2);
bool ResumeDownloadFileByName(const wchar_t *w1,const wchar_t *w2);
bool ResumeUploadFileByName(const wchar_t *w1,const wchar_t *w2);
bool SaveLastError(const wchar_t *w1);
bool SetCreateDt(const wchar_t *w1,bool bIsHandle,CkDateTimeW &createTime);
bool SetCreateTime(const wchar_t *w1,bool bIsHandle,SYSTEMTIME &createTime);
bool SetCreateTimeStr(const wchar_t *w1,bool bIsHandle,const wchar_t *w2);
bool SetLastAccessDt(const wchar_t *w1,bool bIsHandle,CkDateTimeW &createTime);
bool SetLastAccessTime(const wchar_t *w1,bool bIsHandle,SYSTEMTIME &lastAccessTime);
bool SetLastAccessTimeStr(const wchar_t *w1,bool bIsHandle,const wchar_t *w2);
bool SetLastModifiedDt(const wchar_t *w1,bool bIsHandle,CkDateTimeW &createTime);
bool SetLastModifiedTime(const wchar_t *w1,bool bIsHandle,SYSTEMTIME &lastModTime);
bool SetLastModifiedTimeStr(const wchar_t *w1,bool bIsHandle,const wchar_t *w2);
bool SetOwnerAndGroup(const wchar_t *w1,bool bIsHandle,const wchar_t *w2,const wchar_t *w3);
bool SetPermissions(const wchar_t *w1,bool bIsHandle,int perm);
bool UnlockComponent(const wchar_t *w1);
bool UploadFile(const wchar_t *w1,const wchar_t *w2);
bool UploadFileByName(const wchar_t *w1,const wchar_t *w2);
bool WriteFileBytes(const wchar_t *w1,const CkByteData &data);
bool WriteFileBytes32(const wchar_t *w1,int offset,const CkByteData &data);
bool WriteFileBytes64(const wchar_t *w1,__int64 offset64,const CkByteData &data);
bool WriteFileBytes64s(const wchar_t *w1,const wchar_t *w2,const CkByteData &data);
bool WriteFileText(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
bool WriteFileText32(const wchar_t *w1,int offset32,const wchar_t *w2,const wchar_t *w3);
bool WriteFileText64(const wchar_t *w1,__int64 offset64,const wchar_t *w2,const wchar_t *w3);
bool WriteFileText64s(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,const wchar_t *w4);
const wchar_t *add64(const wchar_t *w1,const wchar_t *w2);
const wchar_t *clientIdentifier(void);
const wchar_t *clientIpAddress(void);
const wchar_t *debugLogFilePath(void);
const wchar_t *disconnectReason(void);
const wchar_t *filenameCharset(void);
const wchar_t *forceCipher(void);
const wchar_t *getFileCreateTimeStr(const wchar_t *w1,bool bFollowLinks,bool bIsHandle);
const wchar_t *getFileGroup(const wchar_t *w1,bool bFollowLinks,bool bIsHandle);
const wchar_t *getFileLastAccessStr(const wchar_t *w1,bool bFollowLinks,bool bIsHandle);
const wchar_t *getFileLastModifiedStr(const wchar_t *w1,bool bFollowLinks,bool bIsHandle);
const wchar_t *getFileOwner(const wchar_t *w1,bool bFollowLinks,bool bIsHandle);
const wchar_t *getFileSizeStr(const wchar_t *w1,bool bFollowLinks,bool bIsHandle);
const wchar_t *hostKeyAlg(void);
const wchar_t *hostKeyFingerprint(void);
const wchar_t *httpProxyAuthMethod(void);
const wchar_t *httpProxyDomain(void);
const wchar_t *httpProxyHostname(void);
const wchar_t *httpProxyPassword(void);
const wchar_t *httpProxyUsername(void);
const wchar_t *initializeFailReason(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *openDir(const wchar_t *w1);
const wchar_t *openFile(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
const wchar_t *readFileText(const wchar_t *w1,int numBytes,const wchar_t *w2);
const wchar_t *readFileText32(const wchar_t *w1,int offset32,int numBytes,const wchar_t *w2);
const wchar_t *readFileText64(const wchar_t *w1,__int64 offset64,int numBytes,const wchar_t *w2);
const wchar_t *readFileText64s(const wchar_t *w1,const wchar_t *w2,int numBytes,const wchar_t *w3);
const wchar_t *realPath(const wchar_t *w1,const wchar_t *w2);
const wchar_t *sessionLog(void);
const wchar_t *socksHostname(void);
const wchar_t *socksPassword(void);
const wchar_t *socksUsername(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
