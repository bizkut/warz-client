#pragma once

#pragma pack(push)
#pragma pack(1)


struct r3dFS_DirEntry // for no uncompress file
{
	char		name_[MAX_PATH];
	WORD		idx_;
	WORD		parent_;
	DWORD		reserved1_;
	DWORD		reserved2_;
	DWORD		reserved3_;
};


class r3dFS_FileEntry
{
  public:
	char		name[MAX_PATH];
	BYTE		cmethod;		// compression method
	BYTE		volume;
	DWORD		size;
	DWORD		crc32;
	DWORD		offset;	
	DWORD		csize;
	
	
	enum 
	{
	  FLAG_EXTRACT      = (1<<0),
	  FLAG_UPDATE_EXIST = (1<<20),
	};
	
	DWORD		flags;
	WORD		dirIdx;

	DWORD		reserved1;
	DWORD		reserved2;
	DWORD		reserved3;
  
  public:
	r3dFS_FileEntry()
	{
	  memset(name, 0, sizeof(name));
	  cmethod   = 0;
	  volume    = 0xFF;
	  size      = 0xFFFFFFFF;
	  crc32     = 0;
	  offset    = 0xFFFFFFFF;	  
	  csize     = 0xFFFFFFFF;
	  flags     = 0;
	  dirIdx    = 0;
	  
	  reserved1 = 0;
	  reserved2 = 0;
	  reserved3 = 0;
	}
	
	bool		IsValid() const 
	{
	  if(name[0] == 0) return false;
	  if(cmethod == 0) return false;
	  if(volume == 0xFF) return false;
	  if(offset == 0xFFFFFFFF) return false;
	  if(size == 0xFFFFFFFF) return false;
	  if(csize == 0xFFFFFFFF) return false;
	  //flags       = 0;
	  
	  return true;
	}
};

struct r3dFS_ListHeader
{
	enum { ID = 'lfra', };
	DWORD		id;
	DWORD		version;
	
	r3dFS_ListHeader() {
	  id = 0;
	  version = 0;
	}
};

struct r3dFS_ListHeader_v1
{
	DWORD		buildVersion;
	DWORD		numFiles;
	DWORD		offset; // for no uncompress file
	DWORD		csize;		// compressed size of file list data
	DWORD		crc32;
};

struct r3dFS_FileHeader
{
	enum { ID = 'dfra', };
	DWORD		id;
	
	r3dFS_FileHeader() {
	  id = 0;
	}
};

#pragma pack(pop)
