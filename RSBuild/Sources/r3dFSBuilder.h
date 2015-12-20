#pragma once

#include "FileSystem/r3dFileSystem.h"

class r3dFSBuilder
{
  public:
	struct file_s
	{
	  char		name[MAX_PATH];
	  DWORD		size;
	  
	  file_s()
	  {
	    name[0] = 0;
	    size    = -1;
	  }
	};
	
	char		webCDNDir_[MAX_PATH]; // base path of update files
	char		outputDir_[MAX_PATH];
	char		outputBaseName_[MAX_PATH];
	char		basePath_[MAX_PATH];
	DWORD		buildVersion_;
	
	enum EMatchType
	{
	  MATCH_PART,
	  MATCH_FULL,
	};

	struct match_s
	{
	  int		type;
	  char		name[MAX_PATH];
	};
	
	std::vector<file_s> flist_;

	std::vector<match_s> excludes_;
	std::vector<match_s> includes_;
	bool		ShouldInclude(const char* full, const char* name, int isDir);
	bool		ShouldExclude(const char* full, const char* name, int isDir);
	bool		IsDefaultSkippedDir(const char* name);

	void		ScanDirectory(const char* dir);
	DWORD		DetectFileFlags(const char* fname);

	r3dFileSystem*	basefs_;
	bool		ExistInBaseArchive(const file_s& file, int curFile, int totFiles);
	
	r3dFileSystem*	fs_;
	void		CreateFileSystem();
	void		FilterOutSCO();
	void		FilterAndDeleteLostSCB();
  
  public:
	r3dFSBuilder(const char* path);
	~r3dFSBuilder();
	
	void		AddExclude(const char* name, EMatchType type = MATCH_FULL);
	//@NOTE: include does work only on file level, excluded directories skipped completely for now
	void		AddInclude(const char* name, EMatchType type = MATCH_FULL);

	bool		ReconvertAllSCO();
	bool		CheckTextures();
		
	bool		OpenBaseArchive(const char* base);
	bool		BuildFileList();
	bool		CreateArchive();
	
	void		DeleteVolumes();
};