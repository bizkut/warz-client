#include "r3dPCH.h"
#include "r3d.h"
#pragma comment(lib, "DbgHelp.lib")

#include "r3dFileSystem.h"
#include "r3dFSStructs.h"
#include "r3dFSCompress.h"

extern void r3dCopyFixedFileName(const char* in_fname, char* out_fname);

CRITICAL_SECTION g_FileSysCritSection;

r3dFS_FileList::r3dFS_FileList()
{
  buildVersion_ = 0;
}

r3dFS_FileList::~r3dFS_FileList()
{
  namesHash_.Clear();

  for(size_t i=0; i<files_.size(); i++) {
    delete files_[i];    
  }
  files_.clear();
}

const char* r3dFS_FileList::GetBuildDate(DWORD buildVersion, char* buf)
{
  DWORD year  = 2010 + ((buildVersion >> 28) & 0xF);
  DWORD month = (buildVersion >> 24) & 0xF;
  DWORD day   = (buildVersion >> 16) & 0xFF;
  DWORD hour  = (buildVersion >> 8) & 0xFF;
  DWORD min   = (buildVersion) & 0xFF;
  const char *monthName = "January";
  switch(month)
  {
  case 1:
	  monthName = "January";
	  break;
  case 2:
	  monthName = "February";
	  break;
  case 3:
	  monthName = "March";
	  break;
  case 4:
	  monthName = "April";
	  break;
  case 5:
	  monthName = "May";
	  break;
  case 6:
	  monthName = "June";
	  break;
  case 7:
	  monthName = "July";
	  break;
  case 8:
	  monthName = "August";
	  break;
  case 9:
	  monthName = "September";
	  break;
  case 10:
	  monthName = "October";
	  break;
  case 11:
	  monthName = "November";
	  break;
  case 12:
	  monthName = "December";
	  break;
  }
  
  sprintf(buf, "%s %d, %04d %d:%02d", monthName, day, year, hour, min);
  return buf;
}

void r3dFS_FileList::AddToNameHash(const r3dFS_FileEntry* fe)
{
  r3dCSHolderWithDeviceQueue csHolder(g_FileSysCritSection);

  r3d_assert(fe->name[0]);
  if(Find(fe->name) != NULL)
    r3dError("file %s already exist\n", fe->name);

  char name[MAX_PATH];
  r3dscpy(name, fe->name);
  strlwr(name);
  namesHash_.Add(name, fe);
}

void r3dFS_FileList::InitAllNamesHash()
{
  r3dCSHolderWithDeviceQueue csHolder(g_FileSysCritSection);
  // populate hash
  r3d_assert(namesHash_.IsEmpty());
  for(size_t i=0; i<files_.size(); i++) 
  {
    AddToNameHash(files_[i]);
  }
  
  return;
}

const r3dFS_FileEntry* r3dFS_FileList::Find(const char* in_fname) const
{
  r3dCSHolderWithDeviceQueue csHolder(g_FileSysCritSection);

  char fname[MAX_PATH];
  r3dCopyFixedFileName(in_fname, fname);
  strlwr(fname);
  
  const r3dFS_FileEntry* fe = NULL;
  if(namesHash_.GetObject(fname, &fe)) {
    r3d_assert(fe);
    return fe;
  }
    
/*  
  // search in normal array
  for(size_t i=0; i<files_.size(); i++) 
  {
    const r3dFS_FileEntry* fe = files_[i];
    
    if(stricmp(fe->name, fname) == 0) {
      return fe;
    }
  }
*/
  
  return NULL;
}

r3dFS_FileEntry* r3dFS_FileList::AddNew(const char* fname)
{
  r3dCSHolderWithDeviceQueue csHolder(g_FileSysCritSection);

  r3dFS_FileEntry* fe = game_new r3dFS_FileEntry();
  r3dscpy(fe->name, fname);
  files_.push_back(fe);
  AddToNameHash(fe);

  return fe;
}

bool r3dFS_FileList::Remove(const char* fname)
{
  r3dCSHolderWithDeviceQueue csHolder(g_FileSysCritSection);

  const r3dFS_FileEntry* fe = Find(fname);
  if(!fe) {
    r3dError("fs::Remove - can't find %s\n", fname);
    return false;
  }
  
  // remove from hash
  char name[MAX_PATH];
  r3dscpy(name, fe->name);
  strlwr(name);
  if(!namesHash_.Remove(name))
    r3dError("fs::Remove - hash error %s\n", fname);

  // remove from list
  iterator it = std::find(files_.begin(), files_.end(), fe);
  r3d_assert(it != files_.end());
  delete *it; // delete inside r3dFS_FileEntry
  files_.erase(it);

  return true;
}


bool r3dFS_FileList::Create(const BYTE* data, unsigned int size, const char* fname)
{
  r3dCSHolderWithDeviceQueue csHolder(g_FileSysCritSection);

  unsigned int mpos = 0;
  
  // check header
  r3dFS_ListHeader hdr1;
  memcpy(&hdr1, data + mpos, sizeof(hdr1)); mpos += sizeof(hdr1); 
  if(hdr1.id != hdr1.ID) {
    r3dOutToLog("r3dFS: file list file %s is corrupt\n", fname);
    return false;
  }
  
  if(hdr1.version != 0x00000001) {
    r3dOutToLog("r3dFS: file list file %s have unsupported version %d\n", fname, hdr1.version);
    return false;
  }
  
  r3dFS_ListHeader_v1 hdr2;
  if(mpos + sizeof(hdr2) > size) {
    r3dOutToLog("r3dFS: file list file %s EOF#1\n", fname);
    return false;
  }
  memcpy(&hdr2, data + mpos, sizeof(hdr2)); mpos += sizeof(hdr2);

  // buffer for holding r3dFS_FileEntry
  const int numFiles = hdr2.numFiles;
  DWORD fsize = numFiles * sizeof(r3dFS_FileEntry);
  BYTE* fdata = game_new BYTE[fsize + 1];

  if(hdr2.csize == 0)
  {
    // read all data uncompressed
    if(mpos + fsize > size) {
      r3dOutToLog("r3dFS: file list file %s EOF#2\n", fname);
	  delete [] fdata;
      return false;
    }
    memcpy(fdata, data + mpos, fsize);
  }
  else
  {
    // decompress
    r3dFSCompress compress;
    DWORD decomp;
    if(!compress.DecompressInflate(data + mpos, hdr2.csize, fsize, fdata, &decomp))
      r3dError("failed to decompress file list\n");
    if(decomp != fsize)
      r3dError("failed to decompress file list\n");
  }

  files_.reserve(numFiles);
  for(int i=0; i<numFiles; i++) {
    r3dFS_FileEntry* fe = game_new r3dFS_FileEntry();
    memcpy(fe, fdata + i * sizeof(r3dFS_FileEntry), sizeof(r3dFS_FileEntry));
    files_.push_back(fe);
  }
  delete[] fdata;

  buildVersion_ = hdr2.buildVersion;
  char bldDate[128];
  r3dOutToLog("r3dFS: version:%s, %d files\n", GetBuildDate(buildVersion_, bldDate), files_.size());

  InitAllNamesHash();
  
  return true;
}

bool r3dFS_FileList::WriteFileList(FILE* f)
{
  r3dCSHolderWithDeviceQueue csHolder(g_FileSysCritSection);

  r3d_assert(IsValid());
  r3d_assert(f);
  
  r3dFS_ListHeader hdr1;
  hdr1.id      = hdr1.ID;
  hdr1.version = 0x00000001;
  
  //
  long  size = files_.size() * sizeof(r3dFS_FileEntry);
  BYTE* data = game_new BYTE[size + 1];
  for(size_t i=0; i<files_.size(); i++) {
    memcpy(data + i * sizeof(r3dFS_FileEntry), files_[i], sizeof(r3dFS_FileEntry));
  }
  
  r3dFSCompress compress;
  BYTE* cdata;
  DWORD csize;
  if(!compress.CompressInflate(data, size, &cdata, &csize))
    r3dError("failed to compres file list\n");
  
  r3dFS_ListHeader_v1 hdr2;
  hdr2.numFiles     = files_.size();
  hdr2.buildVersion = buildVersion_;
  hdr2.csize        = csize;
  hdr2.crc32        = r3dCRC32(data, size);
  
  // write headers
  fwrite(&hdr1, 1, sizeof(hdr1), f);
  fwrite(&hdr2, 1, sizeof(hdr2), f);
  // write compressed data
  fwrite(cdata, csize, 1, f);
  
  delete[] data;
  delete[] cdata;
  
  return true;
}


r3dFileSystem::r3dFileSystem()
{
  InitializeCriticalSection(&g_FileSysCritSection);
  VOLUME_SIZE = 1 * 1024 * 1024 * 1024; // 1gb
  failOnError_ = true;

  baseName_[0] = 0;
  ResetVolumes();
}

r3dFileSystem::~r3dFileSystem()
{
  DeleteCriticalSection(&g_FileSysCritSection);
  
  CloseVolumes();
}

void r3dFileSystem::GetVolumeName(char* fname, int idx) const
{
  r3dCSHolderWithDeviceQueue csHolder(g_FileSysCritSection);
  r3d_assert(baseName_[0] != 0);
  sprintf(fname, "%s_%02d.bin", baseName_, idx);
}


void r3dFileSystem::RemoveVolumeFiles()
{
  r3dCSHolderWithDeviceQueue csHolder(g_FileSysCritSection);
  CloseVolumes();

  // clean up
  for(size_t i=0; i<MAX_VOLUMES; i++) {
    char fname[MAX_PATH];
    GetVolumeName(fname, i);
    _unlink(fname);
  }
}

bool r3dFileSystem::OpenVolumesEx(bool forRead, bool fail_if_error)
{
  r3dCSHolderWithDeviceQueue csHolder(g_FileSysCritSection);
  for(size_t i=0; i<MAX_VOLUMES; i++) 
  {
    r3d_assert(volumeHandles[i] == INVALID_HANDLE_VALUE);
    if(volumeSizes[i] == 0)
      continue;
    
    char fname[MAX_PATH];
    GetVolumeName(fname, i + 1); // NOTE! data volumes starts from 001
    
    HANDLE h = ::CreateFile(fname, forRead ? GENERIC_READ : (GENERIC_READ | GENERIC_WRITE), FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(h == INVALID_HANDLE_VALUE) 
    {
      if(fail_if_error)
        r3dError("FileSystem corrupt: can't open %s for reading\n", fname);

      r3dOutToLog("r3dFS: can't open %s\n", fname);
      return false;
    }
    
    volumeHandles[i] = h;
  }
  
  return true;
}

void r3dFileSystem::CloseVolumes()
{
  for(size_t i=0; i<MAX_VOLUMES; i++) 
  {
    if(volumeHandles[i] == INVALID_HANDLE_VALUE)
      continue;

    CloseHandle(volumeHandles[i]);
    volumeHandles[i] = INVALID_HANDLE_VALUE;
  }
  
  return;
}

bool r3dFileSystem::AllocSpaceForFile(__int64 size, int* out_volume, DWORD* out_offset)
{
  r3dCSHolderWithDeviceQueue csHolder(g_FileSysCritSection);

  size += sizeof(r3dFS_FileHeader);
  
  // search in all volumes for file to fit
  for(int i=0; i<MAX_VOLUMES; i++)
  {
    if(volumeSizes[i] + size < VOLUME_SIZE)
    {
      *out_volume = i;
      *out_offset = (DWORD)volumeSizes[i];
      volumeSizes[i] += size;
      return true;
    }
  }

  r3dError("file can't fit into volume\n");
  return false;
}

bool r3dFileSystem::WriteFileList()
{
  r3dCSHolderWithDeviceQueue csHolder(g_FileSysCritSection);

  const int numFiles = GetNumFiles();
  if(numFiles == 0)
    r3dError("there is no files");
    
  //@@@TODO: write to temp, then rename
  char fname[MAX_PATH];
  GetVolumeName(fname, 0);
  FILE* f = fopen(fname, "wb");
  if(f == NULL) {
    r3dError("can't open %s for writing\n", fname);
    return false;
  }
  fl_.WriteFileList(f);
  fclose(f);
  
  return true;
}

 
bool r3dFileSystem::ReadFileList()
{
  r3dCSHolderWithDeviceQueue csHolder(g_FileSysCritSection);

  char fname[MAX_PATH];
  GetVolumeName(fname, 0);
  FILE* f = fopen(fname, "rb");
  if(f == NULL) {
    r3dOutToLog("r3dFS: there is no volume file %s\n", fname);
    return false;
  }
  fseek(f, 0, SEEK_END);
  int size = ftell(f);
  fseek(f, 0, SEEK_SET);
  
  BYTE* data = game_new BYTE[size + 1];
  fread(data, size, 1, f);
  fclose(f);
  
  bool res = fl_.Create(data, size, fname);
  delete[] data;

  return res;
}
  
bool r3dFileSystem::WriteFileData(const r3dFS_FileEntry& fe, const BYTE* cdata, DWORD csize)
{
  r3dCSHolderWithDeviceQueue csHolder(g_FileSysCritSection);
  r3d_assert(fe.IsValid());
  r3d_assert(fe.csize == csize);
  
  // open volume handle if it wasn't opened already
  HANDLE h = volumeHandles[fe.volume];
  if(h == INVALID_HANDLE_VALUE) 
  {
    char fname[MAX_PATH];
    GetVolumeName(fname, fe.volume + 1); //NOTE: data volumes starts from 001

    h = ::CreateFile(fname, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(h == INVALID_HANDLE_VALUE) {
      int err = GetLastError();
      r3dError("can't open %s for writing, %d\n", fname, err);
      return false;
    }

    volumeHandles[fe.volume] = h;
  }
  
  r3d_assert(h != INVALID_HANDLE_VALUE);
	       
  if(::SetFilePointer(h, fe.offset, NULL, FILE_BEGIN) != fe.offset)
    r3dError("SetFilePointer failed %d", GetLastError());

  DWORD written = 0;
  
  r3dFS_FileHeader hdr1;
  hdr1.id      = hdr1.ID;
  if(::WriteFile(h, &hdr1, sizeof(hdr1), &written, NULL) == 0) {
    r3dError("WriteFile failed %d", GetLastError());
  }

  ::WriteFile(h, cdata, csize, &written, NULL);
  if(written != csize) {
    r3dError("failed to write %s %d vs %d\n", fe.name, written, csize);
  }
  //CloseHandle(h); -- keep handle open for speeup, builder should call CloseVolumes()

  //r3dOutToLog(", v%d:%08d, %d->%d\n", fe.volume, fe.offset, fe.size, fe.csize);
  return true;
}

bool r3dFileSystem::UncompressFileData(const r3dFS_FileEntry* fe, const BYTE* cdata, DWORD csize, BYTE** out_data, DWORD* out_size)
{
  r3dCSHolderWithDeviceQueue csHolder(g_FileSysCritSection);
  r3d_assert(fe && fe->IsValid());
  r3d_assert(fe->csize == csize);

  *out_data = game_new BYTE[fe->size + 1];
  
  r3dFSCompress compress;
  compress.failOnError = failOnError_;
  int res = compress.Decompress(fe->name, fe->cmethod, cdata, csize, fe->size, *out_data, out_size);
  if(!res) {
    if(failOnError_) r3dError("GetFileData() decompress %s\n", fe->name);
    else             r3dOutToLog("GetFileData() decompress %s\n", fe->name);
    return false;
  }
  
  // check CRC - TODO: remove later
  if(1)
  {
    DWORD crc32 = r3dCRC32(*out_data, *out_size);
    if(crc32 != fe->crc32) {
      if(failOnError_) r3dError("GetFileData() crc failed %s\n", fe->name);
      else             r3dOutToLog("GetFileData() crc failed %s\n", fe->name);
      return false;
    }
  }
  
  return true;
}

bool r3dFileSystem::GetFileData(const r3dFS_FileEntry* fe, BYTE** out_data, DWORD* out_size)
{
  // enter CS, otherwise if entered from two threads at the same time we will crash with Decompress Failed. 
  r3dCSHolderWithDeviceQueue csHolder(g_FileSysCritSection);

  r3d_assert(fe && fe->IsValid());
  
  // early exit for zero length files, because of MapViewOfFile
  if(fe->size == 0)
  {
    *out_size = 0;
    *out_data = game_new BYTE[1];
	
	return true;
  }

  HANDLE h = volumeHandles[fe->volume];
  r3d_assert(h != INVALID_HANDLE_VALUE);
  
  HANDLE hFileMap = ::CreateFileMapping(h, NULL, PAGE_READONLY, 0, 0, NULL);
  if(hFileMap == NULL) {
    r3dError("GetFileData() failed. CreateFileMapping error %d\n", GetLastError());
    return false;
  }
  
  SYSTEM_INFO sysInfo;
  GetSystemInfo(&sysInfo);
  DWORD mmapStart  = (fe->offset / sysInfo.dwAllocationGranularity) * sysInfo.dwAllocationGranularity;
  DWORD mmapOff    = fe->offset - mmapStart;
  DWORD mmapSize   = mmapOff + fe->csize + sizeof(r3dFS_FileHeader);
  const BYTE* mmap = (const BYTE*)MapViewOfFile(hFileMap, FILE_MAP_READ, 0, mmapStart, mmapSize);
  if(mmap == NULL) { // failed
	DWORD lastError = GetLastError();
	if(lastError == 5) // access denied
	{
		MessageBox(NULL, "Error: access denied trying to open data file.\nPlease turn off your anti virus and restart your machine\n", "Error", MB_OK);
		TerminateProcess(r3d_CurrentProcess, 0);
		return false;
	}
	else if(lastError == 1816) // not enough quota to process this request (some kind of out of memory)
	{
		MessageBox(NULL, "Error: out of system memory when opening data file\n", "Error", MB_OK);
		TerminateProcess(r3d_CurrentProcess, 0);
		return false;
	}
	else
		r3dError("GetFileData() failed. MapViewOfFile Error: %d\n", lastError);
    return false;
  }

  const BYTE* data = mmap + mmapOff;
  const r3dFS_FileHeader& hdr1 = *(r3dFS_FileHeader*)data;
  if(hdr1.id != hdr1.ID) {
    if(failOnError_) r3dError("GetFileData() failed. %s, bad header\n", fe->name);
    else             r3dOutToLog("GetFileData() failed. %s, bad header\n", fe->name);
    return false;
  }

  bool res = UncompressFileData(fe, (data + sizeof(r3dFS_FileHeader)), fe->csize, out_data, out_size);
  
  UnmapViewOfFile(mmap);
  CloseHandle(hFileMap);
  
   return res;
}

bool r3dFileSystem::ExtractFile(const r3dFS_FileEntry* fe, const char* outDir)
{
  r3dCSHolderWithDeviceQueue csHolder(g_FileSysCritSection);
  BYTE* data;
  DWORD size;
  GetFileData(fe, &data, &size);

  char fname[MAX_PATH];
  sprintf(fname, "%s%s", outDir, fe->name);
  ::MakeSureDirectoryPathExists(fname);

  FILE* f = fopen(fname, "wb");
  if(!f) {
    delete[] data;
    r3dOutToLog("r3dFS: unable to open %s for writing\n", fname);
    return false;
  }
  
  fwrite(data, 1, size, f);
  fclose(f);
  
  delete[] data;
  return true;
}

bool r3dFileSystem::BuildNewArchive(const char* baseName)
{
  r3dCSHolderWithDeviceQueue csHolder(g_FileSysCritSection);
  const int numFiles = GetNumFiles();
  if(numFiles == 0)
    r3dError("there is no files");
    
  r3dscpy(baseName_, baseName);
  ::MakeSureDirectoryPathExists(baseName_);
    
  RemoveVolumeFiles();
  ResetVolumes();

  // build volumes

  r3dFSCompress compress;
  __int64 stat_size = 0;
  __int64 stat_csize = 0;
    
  for(int i=0; i<numFiles; i++)
  {
    r3dFS_FileEntry& fe = *fl_.files_[i];

    float cperc = (float)stat_size / stat_csize;
	r3dOutToLog("[%04d/%04d] file: %s ratio:%.2fx, size:%.0f mb              \n", i, numFiles, fe.name, cperc, (float)stat_csize / 1024 / 1024);
    OutputDebugStringA(fe.name);
    
    // compress
    int cmethod = r3dFSCompress::COMPRESS_INFLATE; 
    
    // hack: terrain3 files must be uncompressed
    if(strstr(fe.name, "\\Terrain3\\") != NULL) {
      cmethod = r3dFSCompress::COMPRESS_STORE;
      r3dOutToLog("****** storing Terrain3 file: %s\n", fe.name);
    }
    
    BYTE* cdata = NULL;
    DWORD fsize = 0; // uncompressed size
    DWORD csize = 0; // compressed size
    DWORD crc32 = 0;
    compress.CompressFile(fe.name, &cmethod, &cdata, &fsize, &csize, &crc32);
    if(fsize != fe.size)
      r3dError("file %s size was changed %d vs %d\n", fe.name, fsize, fe.size);
      
    stat_size += fsize;
    stat_csize += csize;
    
    // allocate space for file inside archive
    int   volume;
    DWORD offset;
    AllocSpaceForFile(csize, &volume, &offset);

    // fill header
    fe.csize   = csize;
    fe.cmethod = cmethod;
    fe.volume  = (BYTE)volume;
    fe.offset  = offset;
    fe.crc32   = crc32;
    if(!fe.IsValid())
      r3dError("!fe.IsValid()");
      
    WriteFileData(fe, cdata, csize);
    
    delete[] cdata;
  }
  
  CloseVolumes();
  
  r3dOutToLog("== All Compressed: %I64d -> %I64d\n", stat_size, stat_csize);
  
  WriteFileList();
  return true;
}

static bool FileEntrySortByVolume(const r3dFS_FileEntry* d1, const r3dFS_FileEntry* d2)
{
  if(d1->volume != d2->volume)
    return d1->volume < d2->volume;
  return d1->offset < d2->offset;
}

float r3dFileSystem::GetArchiveWastedPerc()
{
  r3dCSHolderWithDeviceQueue csHolder(g_FileSysCritSection);

  std::sort(fl_.files_.begin(), fl_.files_.end(), FileEntrySortByVolume);

  // get total size of archive
  __int64 totalSize = 0;
  for(size_t i=0; i<fl_.files_.size(); i++)
    totalSize += fl_.files_[i]->csize;

  // calc gap size
  __int64 totalGap = 0;
  for(size_t i=0; i<fl_.files_.size()-1; i++) 
  {
    r3dFS_FileEntry& f1 = *fl_.files_[i];
    r3dFS_FileEntry& f2 = *fl_.files_[i+1];
    //r3dOutToLog("%d: %d:%08d %s\n", i, f1.volume, f1.offset, f1.name);

    if(f1.volume != f2.volume)
      continue;
      
    int gap = f2.offset - (f1.offset + f1.csize + sizeof(r3dFS_FileHeader));
    if(gap != 0) {
      //r3dOutToLog("%d bytes gap\n", gap);
      totalGap += gap;
    }
  }
  
  float wasted = (float)totalGap / (float)totalSize;
  return wasted;
}

bool r3dFileSystem::RebuildArchive(__int64& outTotal, __int64& outCur)
{
  r3dCSHolderWithDeviceQueue csHolder(g_FileSysCritSection);
  r3dOutToLog("r3dFS: rebuilding archive\n"); CLOG_INDENT;

  std::sort(fl_.files_.begin(), fl_.files_.end(), FileEntrySortByVolume);

  // get total size of files
  outCur   = 0;
  outTotal = 0;
  for(size_t i=0; i<fl_.files_.size(); i++)
    outTotal += fl_.files_[i]->csize;
    
  // open for WRITE
  if(!OpenVolumesForWrite(false)) {
    return false;
  }
  
  int   curVolume = 0;
  DWORD curOffset = 0;
  for(size_t i=0; i<fl_.files_.size(); i++) 
  {
    r3dFS_FileEntry& fe = *fl_.files_[i];
    
    // check if we need to switch to new volume
    if(curOffset + fe.csize + sizeof(r3dFS_FileHeader) >= VOLUME_SIZE)
    {
      curOffset = 0;
      curVolume++;
    }
    
    if(fe.volume != curVolume || fe.offset != curOffset)
    {
      if(!RelocateFile(fe, curVolume, curOffset))
      {
        // ok, shit happens
        CloseVolumes();
        RemoveVolumeFiles();
        r3dOutToLog("archive is corrupted\n");
        return false;
      }
    }

    // advance offset
    curOffset += fe.csize + sizeof(r3dFS_FileHeader);
    
    // check if we're requested to exit
    extern bool g_bExit;
    if(g_bExit)
      break;
    
    outCur    += fe.csize;
  }
  
  CloseVolumes();
  WriteFileList();
  
  ResetVolumes();
  DetectVolumeSizes();

  // clean up unused volumes
  for(size_t i=0; i<MAX_VOLUMES; i++) {
    if(volumeSizes[i] == 0) {
      char fname[MAX_PATH];
      GetVolumeName(fname, i + 1);
      _unlink(fname);
    }
  }

  return true;
}

bool r3dFileSystem::RelocateFile(r3dFS_FileEntry& fe, int newVolume, DWORD newOffset)
{
  // note: that'll work with zero sized files, because we're relocating whole chunk with header
  int chunksize = fe.csize + sizeof(r3dFS_FileHeader);
  BYTE* chunkdata = game_new BYTE[chunksize + 1];

  HANDLE h = volumeHandles[fe.volume];
  r3d_assert(h != INVALID_HANDLE_VALUE);
  ::SetFilePointer(h, fe.offset, NULL, FILE_BEGIN);
  
  DWORD rsize = 0;
  ::ReadFile(h, chunkdata, chunksize, &rsize, NULL);
  if(rsize != chunksize) {
    r3dOutToLog("RelocateFile read failed %d\n", GetLastError());
    delete[] chunkdata;
    return false;
  }

  // relocate file
  fe.volume = newVolume;
  fe.offset = newOffset;

  // and write it
  h = volumeHandles[fe.volume];
  r3d_assert(h != INVALID_HANDLE_VALUE);
  ::SetFilePointer(h, fe.offset, NULL, FILE_BEGIN);

  DWORD written = 0;
  ::WriteFile(h, chunkdata, chunksize, &written, NULL);
  if(written != chunksize) {
    r3dOutToLog("RelocateFile write failed %d\n", GetLastError());
    delete[] chunkdata;
    return false;
  }

  delete[] chunkdata;
  return true;  
}

bool r3dFileSystem::ValidateArchive(bool validateData, __int64& outCheckingTotal, __int64& outCheckingSize)
{
  r3dOutToLog("r3dFS: checking archive consistency\n"); CLOG_INDENT;

  if(!OpenVolumesForRead(false)) {
    RemoveVolumeFiles();
    return false;
  }

  failOnError_ = false;
  r3dgameVector(r3dSTLString) todelete;
  
  outCheckingTotal = 0;
  outCheckingSize  = 0;
  for(size_t i=0; i<fl_.files_.size(); i++) 
    outCheckingTotal += fl_.files_[i]->size;
  
  for(size_t i=0; i<fl_.files_.size(); i++) 
  {
    // check if we're requested to exit
    extern bool g_bExit;
    if(g_bExit)
      break;

    r3dCSHolderWithDeviceQueue csHolder(g_FileSysCritSection);

    const r3dFS_FileEntry& fe = *fl_.files_[i];

    outCheckingSize += fe.size;

    HANDLE h = volumeHandles[fe.volume];
    ::SetFilePointer(h, fe.offset, NULL, FILE_BEGIN);
  
    r3dFS_FileHeader hdr1;
    DWORD rsize = 0;
    ::ReadFile(h, &hdr1, sizeof(hdr1), &rsize, NULL);
    if(hdr1.id != hdr1.ID) {
      r3dOutToLog("FileSystem corrupt1, file: %s, volume:%d\n", fe.name, fe.volume);
      todelete.push_back(fe.name);
      continue;
    }
    
    if(validateData) 
    {
      BYTE* out_data = NULL;
      DWORD out_size = 0;
      
      bool res = GetFileData(&fe, &out_data, &out_size);
      SAFE_DELETE(out_data);
      
      if(!res) {
        //r3dOutToLog("FileSystem corrupt2, file: %s, volume:%d\n", fe.name, fe.volume);
        todelete.push_back(fe.name);
        continue;
      }
    }
  }

  CloseVolumes();

  failOnError_ = true;
  
  if(todelete.size() == 0) {
    r3dOutToLog("archive Ok\n");
    return true;
  }
  
  // remove damaged entries
  for(size_t i=0; i<todelete.size(); i++) {
    fl_.Remove(todelete[i].c_str());
  }
  
  if(GetNumFiles() == 0) {
    RemoveVolumeFiles();
    r3dOutToLog("archive is corrupted\n");
    return false;
  }
  
  // reset build version, because we have errors
  fl_.buildVersion_ = 0;
  WriteFileList();
  
  r3dOutToLog("archive had %d bad files\n", todelete.size());
  return false;
}

void r3dFileSystem::DetectVolumeSizes()
{
  // detect volume sizes
  for(size_t i=0; i<fl_.files_.size(); i++) 
  {
    const r3dFS_FileEntry& fe = *fl_.files_[i];
    
    __int64 size = (__int64)fe.offset + (__int64)fe.csize + sizeof(r3dFS_FileHeader);
    if(size > volumeSizes[fe.volume])
      volumeSizes[fe.volume] = size;
  }
}

bool r3dFileSystem::OpenArchive(const char* baseName)
{
  r3dCSHolderWithDeviceQueue csHolder(g_FileSysCritSection);
  if(GetNumFiles() != 0)
    r3dError("r3dFileSystem::Archive already opened");
    
  r3dscpy(baseName_, baseName);
  
  ResetVolumes();

  if(!ReadFileList())
  {
    // something failed, no archive at all.
    return false;
  }
  
  DetectVolumeSizes();

  //for(size_t i=0; i<fl_.files_.size(); i++)
  //  r3dOutToLog("%s: %d\n", fl_.files_[i]->name, fl_.files_[i]->size);

  return true;
}
