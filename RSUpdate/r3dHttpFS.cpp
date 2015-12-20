#include "r3dPCH.h"
#include "r3d.h"
#include "FileSystem/r3dFSStructs.h"
#include "FileSystem/r3dFSCompress.h"

#include "r3dHttpFS.h"
#include "HttpDownload.h"

r3dHttpFS::r3dHttpFS() 
{
  baseUrl_[0] = 0;

  for(int i=0; i<r3dFileSystem::MAX_VOLUMES; i++)
    volumeUrl_[i][0] = 0;
}

r3dHttpFS::~r3dHttpFS() 
{
}

bool r3dHttpFS::OpenFromWeb(const char* baseUrl, updProgress* upd)
{
  //r3dOutToLog("r3dHttpFS::OpenFromWeb %s\n", baseUrl); CLOG_INDENT;
    r3dOutToLog("OpenFromWeb\n");

  r3d_assert(baseUrl_[0] == 0);
  strcpy(baseUrl_, baseUrl);

  char url[512];
  sprintf(url, "%s_%02d.bin", baseUrl_, 0);

  CkByteData data;
  HttpDownload http(upd);
  if(!http.Get(url, data))
    return false;

  // crease filelist
  if(!__super::Create(data.getData(), data.getSize(), url))
    return false;

  // fill basic volume urls
  for(int i=0; i<r3dFileSystem::MAX_VOLUMES; i++)
    sprintf(volumeUrl_[i], "%s_%02d.bin", baseUrl_, i + 1);
    
  // detect max volume file
  int maxVolume = 0;
  for(size_t j=0; j<files_.size(); j++) {
    maxVolume = R3D_MAX(maxVolume, (int)files_[j]->volume);
  }

  // ptumik:
  // disabled redirect. Right now CDN doesn't redirect us, and for some people this code isn't working. Just being stuck forever. 
  // I think it might be something to do with putting a HTML header range-bytes:0-0, as I think it is trying to read the whole file, which is one gig.
 /* r3dOutToLog("CheckRedirect\n");
  // and check if volume urls was redirected
  for(int i=0; i<=maxVolume; i++)
  {
    HttpDownload http2;
    CkByteData data2;
    if(!http2.Get(volumeUrl_[i], data2, 0, 1))
      continue;

    const char* reloc_url = http2.RelocatedUrl();
    if(*reloc_url) {
      //r3dOutToLog("volume%d changed to %s\n", i, reloc_url);
      strcpy(volumeUrl_[i], reloc_url);
    }
  }
  r3dOutToLog("CheckRedirect - finished\n");*/

  r3dOutToLog("OpenFromWeb - finished\n");
  return true;
}

bool r3dHttpFS::GetFile(HttpDownload& http, const r3dFS_FileEntry& fe, CkByteData& data) const
{
  r3d_assert(IsValid());

  const char* url = volumeUrl_[fe.volume];
  if(!http.Get(url, data, fe.offset, fe.csize + sizeof(r3dFS_FileHeader))) {
    return false;
  }

  // validate receiving data
  BYTE* out_data = NULL;
  try
  {
    // check for correct size
    if(data.getSize() != fe.csize + sizeof(r3dFS_FileHeader))
      throw "bad received size";
  
    // check for correct header
    const r3dFS_FileHeader& hdr1 = *(r3dFS_FileHeader*)data.getBytes();
    if(hdr1.id != hdr1.ID)
      throw "bad header";
      
    // validate unpacking
    r3dFSCompress comp;
    comp.failOnError = false;
    DWORD out_size;
    out_data = new BYTE[fe.size + 1];
    if(!comp.Decompress("", fe.cmethod, data.getBytes() + sizeof(r3dFS_FileHeader), fe.csize, fe.size, out_data, &out_size))
      throw "decompress failed";
      
    // CRC of data
    DWORD crc32 = r3dCRC32(out_data, out_size);
    if(crc32 != fe.crc32)
      throw "bad crc";
      
    SAFE_DELETE(out_data);
  } 
  catch(const char* err)
  {
    SAFE_DELETE(out_data);
    r3dOutToLog("httpFS (%d:%d), %s", fe.volume, fe.offset, err);
    return false;
  }

  // remove header from data
  data.removeChunk(0, sizeof(r3dFS_FileHeader));
  r3d_assert(fe.csize == data.getSize());
  
  return true;
}
