#pragma once

#include "FileSystem/r3dFileSystem.h"
#include "CkByteData.h"

class HttpDownload;
class updProgress;

class r3dHttpFS : public r3dFS_FileList
{
  public:
	char		baseUrl_[512];
	char		volumeUrl_[r3dFileSystem::MAX_VOLUMES][1024];

  public:
	r3dHttpFS();
	~r3dHttpFS();

	bool		OpenFromWeb(const char* baseUrl, updProgress* upd);
	bool		GetFile(HttpDownload& http, const r3dFS_FileEntry& fe, CkByteData& data) const;
};