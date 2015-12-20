// CkZip.h: interface for the CkZip class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKZIP_H
#define _CKZIP_H




#include "CkString.h"
class CkByteData;
class CkZipProgress;
class CkZipEntry;
class CkStringArray;
#include "CkMultiByteBase.h"

/*
    IMPORTANT: Objects returned by methods as non-const pointers must be deleted
    by the calling application. 

*/
#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkZip
class CkZip  : public CkMultiByteBase
{
    public:
	CkZip();
	virtual ~CkZip();

	bool OpenFromMemory(const char *pByteData, unsigned long szByteData);
	bool AddEmbeddedFromMemory(const char *exeFilename, 
	    const char *resourceName, const char *pByteData, 
	    unsigned long szByteData);
	CkZipEntry *AppendData(const char *fileName, const unsigned char *pByteData, unsigned long szByteData);

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);


	// BEGIN PUBLIC INTERFACE

	CkZipProgress *get_EventCallbackObject(void) const;
	void put_EventCallbackObject(CkZipProgress *callback);

	// If true, use standard WinZip password-protection.
	bool get_PasswordProtect(void);
	void put_PasswordProtect(bool newVal);

	// If != 0, use encryption, which is different from WinZip password
	// protection and requires that an application using Chilkat Zip must
	// be used to decrypt the Zip.
	// 0 = none, 1 = blowfish, 2 = twofish, 3 = rijndael (AES)
	long get_Encryption(void);
	void put_Encryption(long newVal);

	// This property defaults to the OEM code page of the computer.
	// Set it to read Zips created on computers in other languages,
	// or to create a Zip targeted for another language.
	long get_OemCodePage(void);
	void put_OemCodePage(long newVal);

	// Set to 128, 192, or 256-bit AES encryption.
	long get_EncryptKeyLength(void);
	void put_EncryptKeyLength(long newVal);

	// The temporary directory to use when unzipping files. 
	void get_TempDir(CkString &pVal);
	void put_TempDir(const char *newVal);

	// When files are appended, they are appended from this directory.
	void get_AppendFromDir(CkString &pVal);
	void put_AppendFromDir(const char *newVal);

	// A proxy to use with the OpenFromWeb method.
	void get_Proxy(CkString &pVal);
	void put_Proxy(const char *newVal);

#if defined(WIN32) && !defined(WINCE)
	// (For creating self-extracting EXEs) 
	// This is the name of a file within the Zip that is designated to be automatically run after 
	// a self-extracting EXE unzips. If set, then WriteExe will create a self-extracting EXE that 
	// automatically runs this EXE after unzipping. The typical use for this 
	// is to set AutoRun = "Setup.exe" for installation EXEs
	void get_AutoRun(CkString &pVal);
	void put_AutoRun(const char *newVal);

	void get_ExeXmlConfig(CkString &pVal);
	void put_ExeXmlConfig(const char *newVal);
	const char *exeXmlConfig(void);

	// (For creating self-extracting EXEs) 
	// If True then the EXE created by WriteExe will automatically unzip to a temp directory 
	// when double-clicked. This option is only valid when an AutoRun filename is also set. 
	// If set to False, the self-extracting EXE will prompt for an unzip directory.
	bool get_AutoTemp(void);
	void put_AutoTemp(bool newVal);

	bool get_ExeNoInterface(void);
	void put_ExeNoInterface(bool newVal);

	void get_AutoRunParams(CkString &pVal);
	void put_AutoRunParams(const char *newVal);

	void get_ExeTitle(CkString &pVal);
	void put_ExeTitle(const char *newVal);

	void get_ExeUnzipCaption(CkString &pVal);
	void put_ExeUnzipCaption(const char *newVal);

	void get_ExeUnzipDir(CkString &pVal);
	void put_ExeUnzipDir(const char *newVal);

	void get_ExeDefaultDir(CkString &pVal);
	void put_ExeDefaultDir(const char *newVal);
	const char *exeDefaultDir(void);

	void get_ExeIconFile(CkString &pVal);
	void put_ExeIconFile(const char *newVal);

	void put_ExeFinishNotifier(bool newVal);
	bool get_ExeFinishNotifier(void);

	void put_ExeSilentProgress(bool newVal);
	bool get_ExeSilentProgress(void);

#endif

	bool get_ClearArchiveAttribute(void);
	void put_ClearArchiveAttribute(bool newVal);


	// Number of entries within the Zip.
	long get_NumEntries(void);

	// The Zip filename.
	void get_FileName(CkString &pVal);
	void put_FileName(const char *newVal);

	// The comment stored within the Zip file header.
	void get_Comment(CkString &pVal);
	void put_Comment(const char *newVal);

	// Add this prefix to all filenames when zipping.
	void get_PathPrefix(CkString &pVal);
	void put_PathPrefix(const char *newVal);

	// Discard filename paths when zipping.
	bool get_DiscardPaths(void);
	void put_DiscardPaths(bool newVal);

	void get_DecryptPassword(CkString &pVal);
	void put_DecryptPassword(const char *newVal);
	void get_EncryptPassword(CkString &pVal);
	void put_EncryptPassword(const char *newVal);
	const char *encryptPassword(void);
	const char *decryptPassword(void);

	// Returns the first entry in the Zip.
	CkZipEntry *FirstEntry(void);

	// Call this any number of times before calling AppendFiles
	// to exclude entire directory subtrees.
	void ExcludeDir(const char *dirName);

	// True if this Zip is standard WinZip password-protected.
	bool IsPasswordProtected(const char *zipPath);

	// If an image of a Zip file is already in memory, it can be opened using this method.
	bool OpenFromMemory(const unsigned char *pByteData, unsigned long szByteData);

	bool OpenFromByteData(const CkByteData &byteData);

#if defined(WIN32) && !defined(WINCE)
	// Opens a ZIP embedded within an EXE. This only works for EXEs created by Chilkat Zip.
	bool OpenEmbedded(const char *exePath, const char *resourceName);

	// Opens the ZIP embedded in the caller's EXE. 
	bool OpenMyEmbedded(const char *resourceName);

	// Embeds a ZIP file into an EXE. The ZIP is stored as a named resource within the EXE. 
	// You may store more than one ZIP within a single EXE, so long as the resource names are unique.
	bool AddEmbedded(const char *exePath, const char *resourceName, const char *zipFilename);

	// Replaces a ZIP embedded in an EXE with another one.
	bool ReplaceEmbedded(const char *exePath, const char *resourceName, const char *zipFilename);
	
	// Removes an embedded ZIP from an EXE.
	bool RemoveEmbedded(const char *exePath, const char *resourceName);
#endif

	// Sets the password used for either WinZip password-protection or AES encryption.
	void SetPassword(const char *password);

	// You can specify wildcarded patterns of files to exclude from zipping, such as "*.log", 
	// which would prevent any file ending in ".log" from being added to a zip. Any number of 
	// exclusions can be added, and the exclusions pertain to any of the Append* methods. 
	void SetExclusions(const CkStringArray &excludePatterns) { this->SetExclusions(&excludePatterns); }

	// Returns the exclusions previously set by SetExclusions.
	CkStringArray *GetExclusions(void);

	// Returns the first Zip entry whose filename matches the FilePattern. 
	// The pattern can contain any number of "*" or "?" wildcards. "*" means 0 or more of any 
	// character, "?" means any single character.
	CkZipEntry *FirstMatchingEntry(const char *pattern);

	// Returns an XML formatted listing of the contents of the Zip archive. An example is shown below: 
	/*
	<zip_contents>
		  <dir name="Images">
			  <file>flag.jpg</file> 
		  </dir>
		  <dir name="Sounds">
			  <file>SOUND45.WAV</file> 
			  <file>SOUND49.WAV</file> 
			  <file>SOUND49D.WAV</file> 
		  </dir>
		  <dir name="Source">
			  <dir name="Include">
				  <file>OLEACC.H</file> 
				  <file>OLEAUTO.H</file> 
				  <file>OLECTL.H</file> 
			  </dir>
			  <file>ATL.CPP</file> 
			  <file>REGOBJ.CPP</file> 
			  <file>STDAFX.CPP</file> 
		  </dir>
		  <dir name="Spreadsheets">
			  <file>SAMPLES.XLS</file> 
		  </dir>
		  <file>bullet.gif</file> 
		  <file>chilkatLicense.txt</file> 
		  <file>default.asp</file> 
		  <file>file.gif</file> 
		  <file>global.asa</file> 
		  <file>guestbk.asp</file> 
		  <file>il941.pdf</file> 
		  <file>myfiles.asp</file> 
		  <file>signbook.asp</file> 
		  <file>sub.inc</file> 
		  <file>theme.inc</file> 
		  <file>themes.inc</file> 
	</zip_contents>
	*/
	bool GetDirectoryAsXML(CkString &outXml);

	// Extract all the files that match the pattern. Subdirectories are automatically created as needed. 
	// The pattern can contain any number of "*" or "?" wildcards. "*" means 0 or more of any character, "?" means any single character.
	// eventCallbacks
	bool ExtractMatching(const char *dirPath, const char *pattern);

	// Retrieves a Zip entry by ID. Chilkat Zip automatically assigns a unique ID to each Zip 
	// entry in memory. The IDs are not persisted to disk. 
	CkZipEntry *GetEntryByID(long entryID);

	// Writes the Zip to disk using the FileName property for the name of the file.
	// A Zip file is never updated on disk until one of the Write methods is called.
	// eventCallbacks
	bool WriteZip(void);

	// Same as WriteZip, but the file is closed after writing.
	// eventCallbacks
	bool WriteZipAndClose(void);

	// Loads the contents of a Zip file into the Chilkat Zip object. 
	// eventCallbacks
	bool OpenZip(const char *zipPath);

	// Opens a Zip from a URL.
	// eventCallbacks
	bool OpenFromWeb(const char *url);

	// Clears the contents of the Zip object, and initializes the FileName property. 
	// This does not create a file on disk, it simply initializes the Zip object to a new and empty state.
	bool NewZip(const char *zipPath);

	// Returns a Zip entry by filename.
	CkZipEntry *GetEntryByName(const char *entryName);

	// Returns the Nth Zip entry.  The first entry is at index 0.
	CkZipEntry *GetEntryByIndex(long index);

	// Unzip all the files into the specified directory. Subdirectories are automatically created 
	// as needed.
	// eventCallbacks
	bool Extract(const char *dirPath);

	// Extracts only the files that have more recent last-modified-times than the files on disk. 
	// This allows you to easily refresh only the files that have been updated.
	// eventCallbacks
	bool ExtractNewer(const char *dirPath);

	// Append all the files from within another Zip file into this one.
	bool AppendZip(const char *zipPath);

	// Appends a single file, or a directory (not the files in the directory, but the
	// directory entry itself) to a Zip file.
	// eventCallbacks
	bool AppendOneFileOrDir(const char *path, bool saveExtraPath);

	// Appends one or more files to the Zip object. The FilePattern can use the "*" or "?" 
	// wildcard characters. "*" means 0 or more of any character, and "?" means any single character. 
	// Set Recurse equal to true to recursively add all subdirectories, or false to only add files 
	// in the current directory.
	// eventCallbacks
	bool AppendFiles(const char *filePattern, bool recurse);

	// Same as AppendFiles, but extra parameters are to control whether or not the full pathname 
	// is included with the Zip entry, or whether files with the Archive, Hidden, or 
	// System attributes are included. true = yes, false = no.
	// eventCallbacks
	bool AppendFilesEx(const char *filePattern, bool recurse, 
	    bool saveExtraPath, bool archiveOnly, bool includeHidden, 
	    bool includeSystem);

	// Same as calling AppendFiles multiple times - once for each file pattern in the CkStringArray.
	//bool AppendMultiple(const CkStringArray &fileSpecs, bool recurse)
	    //{return this->AppendMultiple(&fileSpecs,recurse);}

	// Creates a new Zip entry and initializes it with uncompressed memory data. 
	// The CkZipEntry's FileName property is initialized to fileName. 
	CkZipEntry *AppendData(const char *pathInZip, const CkByteData &data);
#if !defined(CHILKAT_MONO)
	CkZipEntry *AppendData2(const char *pathInZip, const unsigned char *pByteData, unsigned long szByteData);
#endif
	CkZipEntry *AppendString(const char *pathInZip, const char *str);
	CkZipEntry *AppendString2(const char *pathInZip, const char *str, const char *charset);

	// Appends a new and empty entry to the Zip object.
	CkZipEntry *AppendNew(const char *pathInZip);
	CkZipEntry *AppendNewDir(const char *pathInZip);

	// Inserts a new and empty entry to the Zip object.
	CkZipEntry *InsertNew(const char *pathInZip, long beforeIndex);

	// Creates a new Zip entry and initializes it with already-compressed data. 
	// The CkZipEntry's FileName property is initialized to fileName.
	CkZipEntry *AppendCompressed(const char *pathInZip, const CkByteData &data);

	// Removes an entry from the Zip object. The procedure for removing a file from an existing Zip 
	// is to (1) open the Zip, (2) remove the entry by calling this method, 
	// and (3) writing the Zip.
	//bool DeleteEntry(const CkZipEntry &entry) { return this->DeleteEntry(&entry); }

	// Creates a new Zip entry and initializes it with already-compressed data that is base-64 encoded. 
	// The CkZipEntry's FileName property is initialized to fileName. 
	CkZipEntry *AppendBase64(const char *pathInZip, const char *data);

	// Creates a new Zip entry and initializes it with already-compressed data that is Hex encoded. 
	// The CkZipEntry's FileName property is initialized to fileName. 
	CkZipEntry *AppendHex(const char *pathInZip, const char *data);

	// Creates a self-extracting EXE.
#if defined(WIN32) && !defined(WINCE)
	// eventCallbacks
	bool WriteExe(const char *path);
#endif
	// Creates a image of a Zip file in memory
	// eventCallbacks
	bool WriteToMemory(CkByteData &outData);

	// Closes the Zip and releases the Zip file if one was open for reading and/or writing.
	void CloseZip(void);

	// Must be called once at the beginning of a program.
	bool UnlockComponent(const char *regCode);
	bool IsUnlocked(void) const;

	// Returns -1 for failure, otherwise the return value is the number of files unzipped.
	// These are identical to the equivalent "Extract" methods, except they return the
	// number of files unzipped.
	// eventCallbacks
	int Unzip(const char *dirPath);
	// eventCallbacks
	int UnzipNewer(const char *dirPath);
	// eventCallbacks
	int UnzipInto(const char *dirPath);
	// eventCallbacks
	int UnzipMatching(const char *dirPath, const char *pattern, bool verbose);
	// eventCallbacks
	int UnzipMatchingInto(const char *dirPath, const char *pattern, bool verbose);

#if defined(WIN32) && !defined(WINCE)
	void SetExeConfigParam(const char *name, const char *value);
	bool GetExeConfigParam(const char *name, CkString &outStr);
	const char *getExeConfigParam(const char *name);
#endif

	// Append this zip to a zip on disk...
	bool QuickAppend(const char *zipPath);

	// Set the compression level for all entries in the Zip.
	void SetCompressionLevel(int level);

#if defined(WIN32) && !defined(WINCE)
	// eventCallbacks
	bool WriteExe2(const char *exePath, const char *destExePath, bool bAesEncrypt, 
	    int keyLength, const char *password);
#endif

	const char *tempDir(void);
	const char *appendFromDir(void);
	const char *proxy(void);
	const char *comment(void);
	const char *fileName(void);
	const char *pathPrefix(void);
	const char *getDirectoryAsXML(void);

#if defined(WIN32) && !defined(WINCE)
	const char *autoRun(void);
	const char *autoRunParams(void);
	const char *exeTitle(void);
	const char *exeUnzipCaption(void);
	const char *exeUnzipDir(void);
	const char *exeIconFile(void);

	bool get_ExeWaitForSetup(void);
	void put_ExeWaitForSetup(bool newVal);
#endif

	// FILECOUNT_BEGIN
	int get_FileCount(void);
	// FILECOUNT_END
	// HASZIPFORMATERRORS_BEGIN
	bool get_HasZipFormatErrors(void);
	// HASZIPFORMATERRORS_END
	// TEXTFLAG_BEGIN
	bool get_TextFlag(void);
	void put_TextFlag(bool newVal);
	// TEXTFLAG_END
	// EXTRACTINTO_BEGIN
	// eventCallbacks
	bool ExtractInto(const char *dirPath);
	// EXTRACTINTO_END
	// CASESENSITIVE_BEGIN
	bool get_CaseSensitive(void);
	void put_CaseSensitive(bool newVal);
	// CASESENSITIVE_END
	// VERIFYPASSWORD_BEGIN
	bool VerifyPassword(void);
	// VERIFYPASSWORD_END
	// WRITEEXETOMEMORY_BEGIN
#if defined(WIN32) && !defined(WINCE)
	// eventCallbacks
	bool WriteExeToMemory(CkByteData &outBytes);
#endif
	// WRITEEXETOMEMORY_END
	// OVERWRITEEXISTING_BEGIN
	bool get_OverwriteExisting(void);
	void put_OverwriteExisting(bool newVal);
	// OVERWRITEEXISTING_END
	// ADDNOCOMPRESSIONEXTENSION_BEGIN
	void AddNoCompressExtension(const char *fileExtension);
	// ADDNOCOMPRESSIONEXTENSION_END
	// REMOVENOCOMPRESSIONEXTENSION_BEGIN
	void RemoveNoCompressExtension(const char *fileExtension);
	// REMOVENOCOMPRESSIONEXTENSION_END
	// ISNOCOMPRESSIONEXTENSION_BEGIN
	bool IsNoCompressExtension(const char *fileExtension);
	// ISNOCOMPRESSIONEXTENSION_END
	// VERBOSELOGGING_BEGIN
	bool get_VerboseLogging(void);
	void put_VerboseLogging(bool newVal);
	// VERBOSELOGGING_END
	// IGNOREACCESSDENIED_BEGIN
	bool get_IgnoreAccessDenied(void);
	void put_IgnoreAccessDenied(bool newVal);
	// IGNOREACCESSDENIED_END
	// CLEARREADONLYATTR_BEGIN
	bool get_ClearReadOnlyAttr(void);
	void put_ClearReadOnlyAttr(bool newVal);
	// CLEARREADONLYATTR_END

	// ZIP_INSERT_POINT

	// END PUBLIC INTERFACE


	bool AppendMultiple(const CkStringArray *fileSpecs, bool recurse);

#if defined(WIN32) && !defined(WINCE)
	// Embed data from memory into an EXE.
	bool AddEmbeddedFromMemory(const char *exePath, const char *resourceName, const unsigned char *pByteData, unsigned long szByteData);
#endif
	void SetExclusions(const CkStringArray *excludePatterns);
	bool DeleteEntry(const CkZipEntry *entry);
	bool AppendOneFileOrDir(const char *path);


    // Everything below is for internal use only.
    private:
	CkZipProgress *m_callback;

	// Don't allow assignment or copying these objects.
	CkZip(const CkZip &) { } 
	CkZip &operator=(const CkZip &) { return *this; }
	CkZip(void *impl);



};
#ifndef __sun__
#pragma pack (pop)
#endif


#endif


