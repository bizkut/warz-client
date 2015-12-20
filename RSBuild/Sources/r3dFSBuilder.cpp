#include "r3dPCH.h"
#include "r3d.h"

// for PatchMatchSpec
#include "shlwapi.h"
#pragma comment(lib, "shlwapi.lib")

#include "r3dFSBuilder.h"
#include "FileSystem/r3dFSStructs.h"
#include "FileSystem/r3dFSCompress.h"
#include "r3dMeshConvert.h"

bool pattern_match(const char *str_, const char *pattern) 
{
#if 1
    return PathMatchSpec(str_, pattern) == TRUE;
#else
    enum State {
        Exact,      	// exact match
        Any,        	// ?
        AnyRepeat    	// *
    };
    
    char str[MAX_PATH];
    strcpy(str, str_);
    strlwr(str);

    const char *s = str;
    const char *p = pattern;
    const char *q = 0;
    int state = 0;

    bool match = true;
    while (match && *p) {
        if (*p == '*') {
            state = AnyRepeat;
            q = p+1;
        } else if (*p == '?') state = Any;
        else state = Exact;

        if (*s == 0) break;

        switch (state) {
            case Exact:
                match = *s == *p;
                s++;
                p++;
                break;

            case Any:
                match = true;
                s++;
                p++;
                break;

            case AnyRepeat:
                match = true;
                s++;

                if (*s == *q) p++;
                break;
        }
    }

    if (state == AnyRepeat) return (*s == *q);
    else if (state == Any) return (*s == *p);
    else return match && (*s == *p);
#endif    
} 

r3dFSBuilder::r3dFSBuilder(const char* path)
{
  basefs_ = NULL;
  fs_     = NULL;
  
  basePath_[0]  = 0;
  outputDir_[0] = 0;
  webCDNDir_[0] = 0;
  
  r3dscpy(outputBaseName_, "wz");

  if(path == NULL || *path == 0) {
    sprintf(basePath_, "");
  } else {
    sprintf(basePath_, "%s\\", path);
  }

  SYSTEMTIME t;
  GetSystemTime(&t);
  DWORD year  = (t.wYear - 2010) << 28;
  DWORD month = t.wMonth << 24;
  DWORD day   = t.wDay   << 16;
  DWORD hour  = t.wHour  << 8;
  DWORD min   = t.wMinute;
  buildVersion_ = year | month | day | hour | min;
  
  return;
}

r3dFSBuilder::~r3dFSBuilder()
{
}

void r3dFSBuilder::AddExclude(const char* name, EMatchType type)
{
  match_s match;
  match.type = type;
  strcpy(match.name, name);
  strlwr(match.name);
  
  excludes_.push_back(match);
}

void r3dFSBuilder::AddInclude(const char* name, EMatchType type)
{
  match_s match;
  match.type = type;
  strcpy(match.name, name);
  strlwr(match.name);
  
  includes_.push_back(match);
}

bool r3dFSBuilder::IsDefaultSkippedDir(const char* name)
{
  if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
    return true;

  // skip .svn dir
  if(stricmp(name, ".svn") == 0)
    return true;
    
  return false;
}

bool r3dFSBuilder::ShouldInclude(const char* full, const char* name, int isDir)
{
  if(isDir && IsDefaultSkippedDir(name))
    return false;

  for(size_t i=0; i<includes_.size(); i++)
  {
    const match_s& match = includes_[i];
    if(match.type == MATCH_FULL) 
    {
      if(pattern_match(full, match.name)) {
        //printf("!!! included %s because of %s\n", full, match.name);
        return true;
      }
    } 
    else if(match.type == MATCH_PART) 
    {
      if(pattern_match(name, match.name)) {
        //printf("!!! partially included %s because of %s\n", full, match.name);
        return true;
      }
    } 
    else 
    {
      assert(0);
    }
  }

  return false;    
}

bool r3dFSBuilder::ShouldExclude(const char* full, const char* name, int isDir)
{
  if(isDir && IsDefaultSkippedDir(name))
    return true;
  
  for(size_t i=0; i<excludes_.size(); i++)
  {
    const match_s& match = excludes_[i];
    if(match.type == MATCH_FULL) 
    {
      if(pattern_match(full, match.name)) {
        //printf("!!! ignored %s because of %s\n", full, match.name);
        return true;
      }
    } 
    else if(match.type == MATCH_PART) 
    {
      if(pattern_match(name, match.name)) {
        //printf("!!! partially ignored %s because of %s\n", full, match.name);
        return true;
      }
    } 
    else 
    {
      assert(0);
    }
  }

  return false;    
}

bool r3dFSBuilder::BuildFileList()
{
  r3dOutToLog("Building file list...\n");

  r3d_assert(flist_.size() == 0);
  flist_.reserve(32000);
  
  ScanDirectory(NULL);
  r3dOutToLog(" %d files\n", flist_.size());
  
  return true;
}

void r3dFSBuilder::ScanDirectory(const char* dir)
{
  WIN32_FIND_DATA ffblk;
  HANDLE hFind;
  char buf[MAX_PATH];

  if(dir == NULL)
    sprintf(buf, "%s%s", basePath_, "*.*");
  else 
    sprintf(buf, "%s%s\\%s", basePath_, dir, "*.*");
  
  hFind = FindFirstFile(buf, &ffblk);
  if(hFind == INVALID_HANDLE_VALUE) {
    return;
  }
  
  do 
  {
    if(dir == NULL)
      sprintf(buf, "%s%s", basePath_, ffblk.cFileName);
    else 
      sprintf(buf, "%s%s\\%s", basePath_, dir, ffblk.cFileName);
    
    int isDir = (ffblk.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
    if(ShouldExclude(buf, ffblk.cFileName, isDir))
    {
      if(!ShouldInclude(buf, ffblk.cFileName, isDir))
        continue;
    }
    
    //printf("%s\n", buf);

    // recurse to directory
    if(isDir) 
    {
      ScanDirectory(buf);
    } 
    else 
    {
      if(ffblk.nFileSizeHigh != 0) {
        r3dError("file %s is bigger that 4gb\n", buf);
        continue;
      }
    
      file_s f;
      strcpy(f.name, buf);
      f.size  = ffblk.nFileSizeLow;
      flist_.push_back(f);
    }
    
  } while(FindNextFile(hFind, &ffblk) != 0);
  
  FindClose(hFind);
  
  return;
}

DWORD r3dFSBuilder::DetectFileFlags(const char* fname)
{
  if(pattern_match(fname, "*.exe") || 
     pattern_match(fname, "*.dll") || 
     pattern_match(fname, "*.manifest") || 
     pattern_match(fname, "crashrpt*.ini"))
  {
    return r3dFS_FileEntry::FLAG_EXTRACT;
  }
    
  return 0;
}

void r3dFSBuilder::CreateFileSystem()
{
  r3d_assert(fs_ == NULL);
  r3dOutToLog("Checking for new files...\n");
  CLOG_INDENT;

  size_t numFiles = flist_.size();
  r3d_assert(numFiles);
  
  fs_ = new r3dFileSystem();
  fs_->fl_.files_.reserve(numFiles);
  fs_->fl_.buildVersion_ = buildVersion_;
  //fs_->VOLUME_SIZE = 250 * 1024 * 1024; // force s3 amazon max upload size
  fs_->VOLUME_SIZE = 1024 * 1024 * 1024; // no more problems with amazon. using BucketExplorer to upload files

  for(size_t i=0; i<numFiles; i++) 
  {
    if(ExistInBaseArchive(flist_[i], i, numFiles))
      continue;
  
    r3dFS_FileEntry* fe = fs_->fl_.AddNew(flist_[i].name);
    fe->size = flist_[i].size;
    fe->flags |= DetectFileFlags(fe->name);
  }
  
  return;
}

bool r3dFSBuilder::OpenBaseArchive(const char* base)
{
  r3d_assert(flist_.size() == 0 && "must be called before creating file list");

  r3dOutToLog("Using %s as base archive\n", base);

  r3d_assert(basefs_ == NULL);
  basefs_ = new r3dFileSystem();
  if(!basefs_->OpenArchive(base)) {
    r3dError("can't open base archive %s\n", base);
    return false;
  }
  
  return true;
}

bool r3dFSBuilder::ExistInBaseArchive(const file_s& file, int curFile, int totFiles)
{
  if(basefs_ == NULL)
    return false;
    
  const r3dFS_FileEntry* fe = basefs_->GetFileEntry(file.name);
  if(fe == NULL)
    return false;
    
  if(fe->size != file.size) {
    r3dOutToLog("changed file %s\n", file.name);
    // size was changed, file updated
    return false;
  }
  
  // need to compare CRC
  r3dOutToLog("comparing [%04d/%04d]\r", curFile+1, totFiles); //file.name

  DWORD crc32;
  DWORD size;
  if(!r3dGetFileCrc32(file.name, &crc32, &size))
    r3dError("Can't get crc for file %s\n", file.name);
  if(crc32 != fe->crc32) {
    r3dOutToLog("changed file %s\n", file.name);
    return false;
  }
   
  return true;
}

bool r3dFSBuilder::CheckTextures()
{
  if(flist_.size() != 0) {
    r3dError("CheckTextures() must be called before all\n");
    return false;
  }
  
  // do a temporary scan for files, search SCOs after that
  r3dOutToLog("Scanning for .DDS files\n");
  CLOG_INDENT;
  
  flist_.reserve(32000);
  ScanDirectory(NULL);
  
  int numBads = 0;
  
  for(size_t i=0; i<flist_.size(); i++)
  {
    const char* dds = flist_[i].name;
    if(stricmp(dds + strlen(dds) - 4, ".dds") != 0)
      continue;
      
    D3DXIMAGE_INFO im = {0};
    D3DXGetImageInfoFromFile(dds, &im);
    switch(im.Format) {
      case D3DFMT_DXT1:
      case D3DFMT_DXT2:
      case D3DFMT_DXT3:
      case D3DFMT_DXT4:
      case D3DFMT_DXT5:
        if((im.Height % 4) != 0 || (im.Width % 4) != 0) {
          r3dOutToLog("%s: bad dimension %dx%d\n", dds, im.Width, im.Height);
          numBads++;
        }
        break;
     }
  }

  flist_.clear();
  
  if(numBads > 0) {
    r3dOutToLog("%d bad dds found\n", numBads);
    return false;
  }
  
  return true;
}

bool r3dFSBuilder::ReconvertAllSCO()
{
  if(flist_.size() != 0) {
    r3dError("ReconvertAllSCO() must be called before all\n");
    return false;
  }
  
  #if !RECONVERT_SCO_MESHES
  r3dOutToLog("ReconvertAllSCO disabled\n");
  return true;
  #endif
    
  // do a temporary scan for files, search SCOs after that
  r3dOutToLog("Scanning for .SCO files\n");
  CLOG_INDENT;
  
  flist_.reserve(32000);
  ScanDirectory(NULL);
  
  int stat1 = 0;
  int stat2 = 0;
  
  for(size_t i=0; i<flist_.size(); i++)
  {
    const char* sco = flist_[i].name;
    if(stricmp(sco + strlen(sco) - 4, ".sco") != 0)
      continue;

    char scb[MAX_PATH];
    strcpy(scb, sco);
    scb[strlen(scb)-1] = 'b';
    
    FILETIME t1, t2;
    if(!getFileTimestamp(sco, t1)) {
      int err = GetLastError();
      r3dError("failed to get timestamp for %s: %d\n", sco, err);
    }

    // check for .scb
    if(!getFileTimestamp(scb, t2)) {
      r3dOutToLog("+ %s\n", sco);
      if(int cnvres = convertScoToBinary(sco, scb) != 0) {
      #if _DEBUG
        //convertScoToBinary(sco, scb);
      #endif
        r3dError("failed to convert %s, err=%d\n", sco, cnvres);
      }
      stat1++;
      continue;
    }

    // check for scb version
    {
      FILE* f = fopen(scb, "rb");
      r3d_assert(f);
      uint32_t version;
      fread(&version, sizeof(uint32_t), 1, f);
      fclose(f);

      extern uint32_t R3DMESH_BINARY_VERSION_get();
      if(version != R3DMESH_BINARY_VERSION_get()) {
        r3dOutToLog("v %s\n", sco);
        _unlink(scb);
        convertScoToBinary(sco, scb);
        stat2++;
        continue;
      }
    }

    // check for timestamp
    if(t1.dwLowDateTime != t2.dwLowDateTime || t1.dwHighDateTime != t2.dwHighDateTime) {
      r3dOutToLog("* %s\n", sco);
      convertScoToBinary(sco, scb);
      stat2++;
      continue;
    }
  }
  
  r3dOutToLog("%d .sco converted, %d .sco reconverted\n", stat1, stat2);
  
  flist_.clear();
  
  return true;
}

void r3dFSBuilder::FilterOutSCO()
{
  r3dOutToLog("Filtering out unneeded .sco\n");
  CLOG_INDENT;

  int stat1 = 0;
  int stat2 = 0;
  
  // store array of filenames from r3dFS_FileEntry to delete
  std::vector<const char*> todelete;

  r3dFS_FileList& fl = fs_->fl_;
  for(size_t i=0; i<fl.files_.size(); i++) 
  {
    const r3dFS_FileEntry* fe = fl.files_[i];

    if(stricmp(fe->name + strlen(fe->name) - 4, ".sco") != 0)
      continue;
      
    // check if we have .scb with same name
    char scb[MAX_PATH];
    strcpy(scb, fe->name);
    scb[strlen(scb)-1] = 'b';
    
    const r3dFS_FileEntry* fe2 = fl.Find(scb);
    if(fe2 == NULL) {
      // incremental build, see if basefs have that .scb 
      if(basefs_ == NULL || basefs_->fl_.Find(scb) == NULL) {
        stat1++;
        continue;
      }
    }

    FILETIME t1, t2;
    if(!getFileTimestamp(fe->name, t1) || !getFileTimestamp(scb, t2)) {
      stat1++;
      continue;
    }
    
    if(t1.dwLowDateTime != t2.dwLowDateTime || t1.dwHighDateTime != t2.dwHighDateTime) {
      stat2++;
      continue;
    }

    // sco match scb
    todelete.push_back(fe->name);
  }
  
  // delete them from filesystem
  for(size_t i=0; i<todelete.size(); i++) {
    const char* fname = todelete[i];
    fl.Remove(fname);
    // please note that fname will be invalid at the moment. fe->name was freed
  }
  
  r3dOutToLog("%d .sco missed binary, %d .sco have outdated binary\n", stat1, stat2);
  r3dOutToLog("%d .sco removed from archive\n", todelete.size());
  
  return;
}

void r3dFSBuilder::FilterAndDeleteLostSCB()
{
  r3dOutToLog("Filtering out lost .scb\n");
  CLOG_INDENT;

  int stat1 = 0;
  
  // store array of filenames from r3dFS_FileEntry to delete
  std::vector<const char*> todelete;

  r3dFS_FileList& fl = fs_->fl_;
  for(size_t i=0; i<fl.files_.size(); i++) 
  {
    const char* scb = fl.files_[i]->name;
    if(stricmp(scb + strlen(scb) - 4, ".scb") != 0)
      continue;
      
    // check if we have .scb with same name
    char sco[MAX_PATH];
    strcpy(sco, scb);
    sco[strlen(sco)-1] = 'o';
    
    const r3dFS_FileEntry* fe2 = fl.Find(sco);
    if(fe2 != NULL) {
      continue;
    }
    
    // we have lost .SCB (without .SCO counterpart)
    stat1++;
    //r3dOutToLog("%s is lost\n", scb);
    _unlink(scb);

    // remove it from archive
    todelete.push_back(scb);
  }
  
  // delete them from filesystem
  for(size_t i=0; i<todelete.size(); i++) {
    const char* fname = todelete[i];
    fl.Remove(fname);
    // please note that fname will be invalid at the moment. fe->name was freed
  }
  
  r3dOutToLog("%d lost .scb deleted\n", stat1);
  
  return;
}

bool r3dFSBuilder::CreateArchive()
{
  if(*outputDir_ == 0) r3dError("no output directory");
  if(*webCDNDir_ == 0) r3dError("no web cdn url");
  
  if(flist_.size() == 0) {
    r3dError("no files\n");
    return false;
  }

  CreateFileSystem();

  FilterAndDeleteLostSCB(); // need to do it before filtering SCO
  FilterOutSCO();

  // check if no new files was added
  if(fs_->GetNumFiles() == 0) {
    r3dOutToLog("No files was changed or added\n");
    return true;
  }

  char vname[MAX_PATH];
  sprintf(vname, "%s\\%s%08X", outputDir_, outputBaseName_, buildVersion_);

  r3dOutToLog("Creating%s archive %s\n", basefs_ ? " incremental" : "", vname);
  CLOG_INDENT;
  fs_->BuildNewArchive(vname);

  char xmlname[MAX_PATH];
  sprintf(xmlname, "%s\\%s.xml", outputDir_, outputBaseName_);
  FILE* f = fopen_for_write(xmlname, "wt");
  
  if(basefs_ != NULL)
  {
    // incremental update
    r3d_assert(basefs_->fl_.IsValid());
    fprintf(f, "<p>\n");
    fprintf(f, " <d ver=\"%u\" base=\"%s%s%08X\" inc=\"%s%s%08X\"", 
      buildVersion_, 
      webCDNDir_, outputBaseName_, basefs_->fl_.buildVersion_,
      webCDNDir_, outputBaseName_, buildVersion_);
    fprintf(f, "/>\n");
    fprintf(f, "</p>\n");
  }
  else
  {
    // full update
    fprintf(f, "<p>\n");
    fprintf(f, " <d ver=\"%u\" base=\"%s%s%08X\"", 
      buildVersion_, 
      webCDNDir_, outputBaseName_, buildVersion_);
    fprintf(f, "/>\n");
    fprintf(f, "</p>\n");
  }
  
  fclose(f);
  return true;
}

void r3dFSBuilder::DeleteVolumes()
{
  if(fs_ == NULL)
    return;
    
  fs_->CloseVolumes();
  fs_->RemoveVolumeFiles();
}