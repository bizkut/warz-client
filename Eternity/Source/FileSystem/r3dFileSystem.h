#pragma once
#include "Tsg_stl/HashTable.h"
#include "Tsg_stl/TString.h"
#include "dbghelp.h"  // for MakeSureDirectoryPathExists

class r3dFS_FileEntry;

class r3dFS_FileList
{
  public:
	DWORD		buildVersion_;

	typedef r3dgameVector(r3dFS_FileEntry*) FileEntriesVec;
	FileEntriesVec files_;
	typedef FileEntriesVec::iterator iterator;

	struct NameHashFunc_T
	{
		inline int operator () ( const char * szKey )
		{
			return r3dHash::MakeHash( szKey );// & ( LENGTH-1 );
		}
	};
	HashTableDynamic<const r3dFS_FileEntry*, FixedString256, NameHashFunc_T, 1024> namesHash_;
	void		AddToNameHash(const r3dFS_FileEntry* fe);
	void		InitAllNamesHash();

  private:	
	// make copy constructor and assignment operator inaccessible
	r3dFS_FileList(const r3dFS_FileList& rhs);
	r3dFS_FileList& operator=(const r3dFS_FileList& rhs);

  public:
	r3dFS_FileList();
	~r3dFS_FileList();
	
	bool		IsValid() const {
	  return files_.size() > 0;
	}

	bool		Create(const BYTE* data, unsigned int size, const char* fname);
	bool		WriteFileList(FILE* f);

	const r3dFS_FileEntry* Find(const char* fname) const;
	r3dFS_FileEntry* AddNew(const char* fname);
	bool		Remove(const char* fname);

	static const char* GetBuildDate(DWORD buildVersion, char* buf);
};

class r3dFileSystem
{
  public:
	char		baseName_[MAX_PATH];	// base name of volume files
	r3dFS_FileList	fl_;
	bool		failOnError_;
	
	const static int MAX_VOLUMES = 64;
	__int64		VOLUME_SIZE;
	__int64		volumeSizes[MAX_VOLUMES];
	HANDLE		volumeHandles[MAX_VOLUMES];
	void		ResetVolumes() 
	{
	  for(int i=0; i<MAX_VOLUMES; i++) 
	  {
	    volumeSizes[i] = 0;
	    volumeHandles[i] = INVALID_HANDLE_VALUE;
	  }
	}
	bool		AllocSpaceForFile(__int64 size, int* out_volume, DWORD* out_offset);
	void		RemoveVolumeFiles();
	bool		OpenVolumesEx(bool forRead = true, bool fail_if_error = true);
	bool		OpenVolumesForRead(bool fail_if_error = true) {
	  return OpenVolumesEx(true, fail_if_error);
	}
	bool		OpenVolumesForWrite(bool fail_if_error = true) {
	  return OpenVolumesEx(false, fail_if_error);
	}
	void		CloseVolumes();
	
	// writing
	bool		WriteFileList();
	bool		WriteFileData(const r3dFS_FileEntry& fe, const BYTE* cdata, DWORD csize);

	bool		ReadFileList();

  private:	
	// make copy constructor and assignment operator inaccessible
	r3dFileSystem(const r3dFileSystem& rhs);
	r3dFileSystem& operator=(const r3dFileSystem& rhs);

  public:
	r3dFileSystem();
	~r3dFileSystem();
	
	int		GetNumFiles() const {
	  return (int)fl_.files_.size();
	}
	DWORD		GetBuildVersion() const {
	  return fl_.buildVersion_;
	}

	void		GetVolumeName(char* fname, int idx) const;
	
	bool		BuildNewArchive(const char* baseName);

	bool		OpenArchive(const char* baseName);
	void		 DetectVolumeSizes();
	bool		ValidateArchive(bool validateData, __int64& outCheckingTotal, __int64& outCheckingSize);
	float		GetArchiveWastedPerc();
	bool		RebuildArchive(__int64& outTotal, __int64& outSize);
	bool		 RelocateFile(r3dFS_FileEntry& fe, int newVolume, DWORD newOffset);

	const r3dFS_FileEntry* GetFileEntry(const char* fname) const {
	  return fl_.Find(fname);
	}
	bool		IsExist(const char* fname) const {
	  return fl_.Find(fname) != NULL;
	}

	bool		GetFileData(const r3dFS_FileEntry* fe, BYTE** out_data, DWORD* out_size);
	bool		UncompressFileData(const r3dFS_FileEntry* fe, const BYTE* cdata, DWORD csize, BYTE** out_data, DWORD* out_size);
	
	bool		ExtractFile(const r3dFS_FileEntry* fe, const char* outDir);
};
