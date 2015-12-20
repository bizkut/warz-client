// CkImap.h: interface for the CkImap class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKIMAP_H
#define _CKIMAP_H



class CkByteData;
#include "CkString.h"
class CkMailboxes;
class CkMessageSet;
class CkEmail;
class CkEmailBundle;
class CkStringArray;
class CkImapProgress;
class CkCert;
class CkPrivateKey;
class CkSshKey;
class CkCsp;

#if !defined(WIN32) && !defined(WINCE)
#include "SystemTime.h"
#endif

#include "CkMultiByteBase.h"

#undef Copy

/*
    IMPORTANT: Objects returned by methods as non-const pointers must be deleted
    by the calling application. 

  */

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkImap
class CkImap  : public CkMultiByteBase
{
    public:
	CkImap();
	virtual ~CkImap();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// 
	CkEmailBundle *FetchChunk(int startSeqNum, int count, CkMessageSet &failedSet, CkMessageSet &fetchedSet);

	// BEGIN PUBLIC INTERFACE
	// SOSNDBUF_BEGIN
	int get_SoSndBuf(void);
	void put_SoSndBuf(int newVal);
	// SOSNDBUF_END

	// SORCVBUF_BEGIN
	int get_SoRcvBuf(void);
	void put_SoRcvBuf(int newVal);
	// SORCVBUF_END

	// SEARCHCHARSET_BEGIN
	void get_SearchCharset(CkString &str);
	const char *searchCharset(void);
	void put_SearchCharset(const char *newVal);
	// SEARCHCHARSET_END

	// APPENDMIMEWITHDATESTR_BEGIN
	// eventCallbacks
	bool AppendMimeWithDateStr(const char *mailbox, const char *mimeText, const char *internalDateStr);
	// APPENDMIMEWITHDATESTR_END

	// LISTSUBSCRIBED_BEGIN
	// eventCallbacks
	CkMailboxes *ListSubscribed(const char *reference, const char *wildcardedMailbox);
	// LISTSUBSCRIBED_END

	// CHECKFORNEWEMAIL_BEGIN
	// eventCallbacks
	CkMessageSet *CheckForNewEmail(void);
	// CHECKFORNEWEMAIL_END

	// CLOSEMAILBOX_BEGIN
	// eventCallbacks
	bool CloseMailbox(const char *mailbox);
	// CLOSEMAILBOX_END

	// HTTPPROXYDOMAIN_BEGIN
	void get_HttpProxyDomain(CkString &str);
	const char *httpProxyDomain(void);
	void put_HttpProxyDomain(const char *newVal);
	// HTTPPROXYDOMAIN_END


	bool SetDecryptCert2(const CkCert &cert, CkPrivateKey &key);

	// eventCallbacks
	bool Connect(const char *hostname);
	// eventCallbacks
	bool Disconnect(void);
	bool IsConnected(void);

	// eventCallbacks
	bool Login(const char *login, const char *password);
	// eventCallbacks
	bool Logout(void);
	bool IsLoggedIn(void);

	void get_LoggedInUser(CkString &str);
	void get_ConnectedToHost(CkString &str);

	// eventCallbacks
	bool CreateMailbox(const char *mailbox);
	// eventCallbacks
	bool DeleteMailbox(const char *mailbox);
	// eventCallbacks
	bool RenameMailbox(const char *fromMailbox, const char *toMailbox);

	// Gets the last response returned by the IMAP server.
	// This is a property:  get_LastResponse.
	//void LastResponse(CkString &outStr);

	// eventCallbacks
	CkMailboxes *ListMailboxes(const char *reference, const char *wildcardedMailbox);

	// eventCallbacks
	bool SelectMailbox(const char *mailbox);    // Email flags are modified when full email is fetched.
	// eventCallbacks
	bool ExamineMailbox(const char *mailbox);   // Email flags are not modified, mailbox cannot be modified.

	// eventCallbacks
	bool Subscribe(const char *mailbox);   
	// eventCallbacks
	bool Unsubscribe(const char *mailbox);   

	bool SetSslClientCert(CkCert &cert);
	CkCert *GetSslServerCert(void);
	bool get_SslServerCertVerified(void);

	// Search for messages and return a message set of either UIDs (if bUid = true) or sequence numbers.
	// eventCallbacks
	CkMessageSet *Search(const char *criteria, bool bUid);

	// eventCallbacks
	CkMessageSet *GetAllUids(void);

	// eventCallbacks
	CkEmail *FetchSingle(long msgId, bool bUid);
	// eventCallbacks
	CkEmail *FetchSingleHeader(long msgId, bool bUid);
	// eventCallbacks
	CkEmailBundle *FetchBundle(const CkMessageSet &messageSet);
	// eventCallbacks
	CkEmailBundle *FetchHeaders(const CkMessageSet &messageSet);

	// eventCallbacks
	CkStringArray *FetchSequenceAsMime(int startSeqNum, int count);
	// eventCallbacks
	CkEmailBundle *FetchSequence(int startSeqNum, int numMessages);
	// eventCallbacks
	CkEmailBundle *FetchSequenceHeaders(int startSeqNum, int numMessages);

	// eventCallbacks
	bool FetchSingleAsMime(long msgId, bool bUid, CkString &outStrMime);
	// eventCallbacks
	CkStringArray *FetchBundleAsMime(const CkMessageSet &messageSet);

	// eventCallbacks
	bool Expunge(void);
	// eventCallbacks
	bool ExpungeAndClose(void);

	// eventCallbacks
	bool StoreFlags(long msgId, bool bUid, const char *flagName, long value);
	// eventCallbacks
	bool FetchFlags(long msgId, bool bUid, CkString &outStrFlags);

	// eventCallbacks
	bool SetFlag(long msgId, bool bUid, const char *flagName, long value);
	// eventCallbacks
	bool SetFlags(const CkMessageSet &messageSet, const char *flagName, long value);

	void get_AuthMethod(CkString &str);
	void put_AuthMethod(const char *str);

	char get_SeparatorChar(void);
	void put_SeparatorChar(char c);

	void get_Domain(CkString &str);
	void put_Domain(const char *str);

	int get_Port(void);
	void put_Port(int port);

	bool get_Ssl(void);
	void put_Ssl(bool s);

	void ClearSessionLog(void);
	void get_SessionLog(CkString &str);
	bool get_KeepSessionLog(void);
	void put_KeepSessionLog(bool newVal);

	void get_LastResponse(CkString &str);
	void get_LastIntermediateResponse(CkString &str);
	void get_LastCommand(CkString &str);

	// These times are in milliseconds.
	long get_HeartbeatMs(void);
	void put_HeartbeatMs(long newVal);

	// eventCallbacks
	bool AppendMime(const char *mailbox, const char *mimeText);
	// eventCallbacks
	bool AppendMail(const char *mailbox, const CkEmail &email);

	// Copy a message from the currently selected mailbox to a new mailbox.
	// eventCallbacks
	bool Copy(long msgId, bool bUid, const char *copyToMailbox);
	// eventCallbacks
	bool CopyMultiple(CkMessageSet &messageSet, const char *copyToMailbox);
	// eventCallbacks
	bool CopySequence(int startSeqNum, int count, const char *copyToMailbox);

	// eventCallbacks
	bool SetMailFlag(const CkEmail &email, const char *flagName, long value);
	// eventCallbacks
	long GetMailFlag(const CkEmail &email, const char *flagName);
	// Query the IMAP server get get up-to-date flag information.
	// eventCallbacks
	bool RefetchMailFlags(CkEmail &emailInOut);

	long get_NumMessages(void);

	// When headers are downloaded, information about the email can be retrieved
	// using these methods.
	long GetMailSize(const CkEmail &email);
	long GetMailNumAttach(const CkEmail &email);
	long GetMailAttachSize(const CkEmail &email, long attachIndex);
	bool GetMailAttachFilename(const CkEmail &email, long attachIndex, CkString &outStrFilename);

	// Automatically decrypt and/or verify signatures (the default).
	// Results of the "unwrapping" are available via the CkEmail object.
	//bool get_AutoUnwrapSecurity(void);
	//void put_AutoUnwrapSecurity(bool bUnwrap);

	// When peek-mode is true, fetching entire emails will not affect
	// the email flags, regardless of whether the mailbox was "selected"
	// using SelectMailbox or "examined" with ExamineMailbox.
	bool get_PeekMode(void);
	void put_PeekMode(bool bPeek);

	// If AppendSeen is true (the default), then AppendMail and AppendMime will append
	// an email as already-seen, otherwise it will be initialized to "unseen".
	bool get_AppendSeen(void);
	void put_AppendSeen(bool alreadySeen);

	bool UnlockComponent(const char *unlockCode);
	bool IsUnlocked(void) const;

	// timeouts in seconds...
	int get_ReadTimeout(void) const;
	void put_ReadTimeout(int numSeconds);

	int get_ConnectTimeout(void) const;
	void put_ConnectTimeout(int numSeconds);


	// eventCallbacks
	bool SendRawCommand(const char *rawCommand, CkString &outRawResponse);

	const char *getMailAttachFilename(const CkEmail &email, long attachIndex);
	const char *fetchFlags(long msgId, bool bUid);
        const char *authMethod(void);
        const char *domain(void);
	const char *fetchSingleAsMime(long msgId, bool bUid);
	const char *loggedInUser(void);
	const char *connectedToHost(void);
	const char *sendRawCommand(const char *rawCommand);
	const char *sessionLog(void);

	const char *lastResponse(void);
	const char *lastCommand(void);
	const char *lastIntermediateResponse(void);
	// LASTAPPENDEDMIME_BEGIN
	void get_LastAppendedMime(CkString &str);
	const char *lastAppendedMime(void);
	// LASTAPPENDEDMIME_END
	// APPENDMIMEWITHFLAGS_BEGIN
	// eventCallbacks
	bool AppendMimeWithFlags(const char *mailbox, const char *mimeText, bool seen, bool flagged, bool answered, bool draft);
	// APPENDMIMEWITHFLAGS_END
	// FETCHSINGLEHEADERASMIME_BEGIN
	// eventCallbacks
	bool FetchSingleHeaderAsMime(int msgId, bool bUID, CkString &outStr);
	const char *fetchSingleHeaderAsMime(int msgId, bool bUID);
	// FETCHSINGLEHEADERASMIME_END

	CkImapProgress *get_EventCallbackObject(void) const { return m_callback; }
	void put_EventCallbackObject(CkImapProgress *progress) { m_callback = progress; }
	// APPENDMIMEWITHDATE_BEGIN
	// SYSTEMTIME is defined in <windows.h>
	// For more information, Google "SYSTEMTIME"
	// eventCallbacks
	bool AppendMimeWithDate(const char *mailbox, const char *mimeText, SYSTEMTIME &internalDate);
	// APPENDMIMEWITHDATE_END
	// SELECTEDMAILBOX_BEGIN
	void get_SelectedMailbox(CkString &str);
	const char *selectedMailbox(void);
	// SELECTEDMAILBOX_END
	// APPENDUID_BEGIN
	int get_AppendUid(void);
	// APPENDUID_END
	// AUTHZID_BEGIN
	void get_AuthzId(CkString &str);
	const char *authzId(void);
	void put_AuthzId(const char *newVal);
	// AUTHZID_END
	// SENDBUFFERSIZE_BEGIN
	int get_SendBufferSize(void);
	void put_SendBufferSize(int newVal);
	// SENDBUFFERSIZE_END
	// SSHTUNNEL_BEGIN
	// eventCallbacks
	bool SshTunnel(const char *sshServerHostname, int sshServerPort);
	// SSHTUNNEL_END
	// SSHAUTHENTICATEPW_BEGIN
	// eventCallbacks
	bool SshAuthenticatePw(const char *sshLogin, const char *sshPassword);
	// SSHAUTHENTICATEPW_END
	// SSHAUTHENTICATEPK_BEGIN
	// eventCallbacks
	bool SshAuthenticatePk(const char *sshLogin, CkSshKey &privateKey);
	// SSHAUTHENTICATEPK_END
	// NOOP_BEGIN
	// eventCallbacks
	bool Noop(void);
	// NOOP_END
	// SOCKSHOSTNAME_BEGIN
	void get_SocksHostname(CkString &str);
	const char *socksHostname(void);
	void put_SocksHostname(const char *newVal);
	// SOCKSHOSTNAME_END
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
	// SOCKSPORT_BEGIN
	int get_SocksPort(void);
	void put_SocksPort(int newVal);
	// SOCKSPORT_END
	// SOCKSVERSION_BEGIN
	int get_SocksVersion(void);
	void put_SocksVersion(int newVal);
	// SOCKSVERSION_END
	// SETSSLCLIENTCERTPFX_BEGIN
	bool SetSslClientCertPfx(const char *pfxPath, const char *pfxPassword);
	// SETSSLCLIENTCERTPFX_END
	// STARTTLS_BEGIN
	bool get_StartTls(void);
	void put_StartTls(bool newVal);
	// STARTTLS_END
	// SSLPROTOCOL_BEGIN
	void get_SslProtocol(CkString &str);
	const char *sslProtocol(void);
	void put_SslProtocol(const char *newVal);
	// SSLPROTOCOL_END
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
	// HTTPPROXYAUTHMETHOD_BEGIN
	void get_HttpProxyAuthMethod(CkString &str);
	const char *httpProxyAuthMethod(void);
	void put_HttpProxyAuthMethod(const char *newVal);
	// HTTPPROXYAUTHMETHOD_END
	// HTTPPROXYHOSTNAME_BEGIN
	void get_HttpProxyHostname(CkString &str);
	const char *httpProxyHostname(void);
	void put_HttpProxyHostname(const char *newVal);
	// HTTPPROXYHOSTNAME_END
	// HTTPPROXYPORT_BEGIN
	int get_HttpProxyPort(void);
	void put_HttpProxyPort(int newVal);
	// HTTPPROXYPORT_END
	// UIDNEXT_BEGIN
	int get_UidNext(void);
	// UIDNEXT_END
	// AUTOFIX_BEGIN
	bool get_AutoFix(void);
	void put_AutoFix(bool newVal);
	// AUTOFIX_END
	// ADDPFXSOURCEDATA_BEGIN
	bool AddPfxSourceData(const CkByteData &pfxData, const char *password);
	// ADDPFXSOURCEDATA_END
	// ADDPFXSOURCEFILE_BEGIN
	bool AddPfxSourceFile(const char *pfxFilePath, const char *password);
	// ADDPFXSOURCEFILE_END
	// SETSSLCLIENTCERTPEM_BEGIN
	bool SetSslClientCertPem(const char *pemDataOrPath, const char *pemPassword);
	// SETSSLCLIENTCERTPEM_END
	// SETCSP_BEGIN
#if defined(WIN32) && !defined(WINCE) && !defined(__MINGW32__)
	bool SetCSP(CkCsp &csp);
#endif
	// SETCSP_END
	// UIDVALIDITY_BEGIN
	int get_UidValidity(void);
	// UIDVALIDITY_END
	// CLIENTIPADDRESS_BEGIN
	void get_ClientIpAddress(CkString &str);
	const char *clientIpAddress(void);
	void put_ClientIpAddress(const char *newVal);
	// CLIENTIPADDRESS_END
	// VERBOSELOGGING_BEGIN
	bool get_VerboseLogging(void);
	void put_VerboseLogging(bool newVal);
	// VERBOSELOGGING_END
	// AUTODOWNLOADATTACHMENTS_BEGIN
	bool get_AutoDownloadAttachments(void);
	void put_AutoDownloadAttachments(bool newVal);
	// AUTODOWNLOADATTACHMENTS_END
	// FETCHATTACHMENTBYTES_BEGIN
	// eventCallbacks
	bool FetchAttachmentBytes(CkEmail &email, int attachIndex, CkByteData &outBytes);
	// FETCHATTACHMENTBYTES_END
	// FETCHATTACHMENTSTRING_BEGIN
	// eventCallbacks
	bool FetchAttachmentString(CkEmail &email, int attachIndex, const char *charset, CkString &outStr);
	const char *fetchAttachmentString(CkEmail &email, int attachIndex, const char *charset);
	// FETCHATTACHMENTSTRING_END
	// FETCHATTACHMENT_BEGIN
	// eventCallbacks
	bool FetchAttachment(CkEmail &email, int attachIndex, const char *saveToPath);
	// FETCHATTACHMENT_END

	// IMAP_INSERT_POINT

	// END PUBLIC INTERFACE



    // For internal use only.
    private:
	CkImapProgress *m_callback;

	// Don't allow assignment or copying these objects.
	CkImap(const CkImap &) { } 
	CkImap &operator=(const CkImap &) { return *this; }
	CkImap(void *impl);


};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif


