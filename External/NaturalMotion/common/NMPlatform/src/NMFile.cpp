// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMFile.h"
#include "NMPlatform/NMMemory.h"

//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
NMFile::NMFile()
{
  init();
}

//----------------------------------------------------------------------------------------------------------------------
NMFile::~NMFile()
{
  close();
}

//----------------------------------------------------------------------------------------------------------------------
void NMFile::init()
{
  m_file = NULL;
  m_mode = NM_FILE_OPEN_MODE_NONE;
}

//----------------------------------------------------------------------------------------------------------------------
int64_t NMFile::allocAndLoad(const char* fileName, void** buffer, int64_t* bufferSize, uint32_t alignment)
{
  NMP_ASSERT(fileName && buffer && bufferSize && alignment > 0);

  *bufferSize = -1;

  FILE* file = NULL;
  NM_FOPEN(file, fileName, "rb");

  if (file == NULL)
    return -1;

  fseek(file, 0, SEEK_END);
  int64_t fileSize = (int64_t) ftell(file);
  if (fileSize <= 0)
  {
    fclose(file);
    return -1;
  }

  *bufferSize = NMP::Memory::align((ptrdiff_t) fileSize, alignment);
  *buffer = NMPMemoryAllocAligned((size_t) (*bufferSize), alignment);
  NMP_ASSERT(buffer);
  fseek(file, 0, SEEK_SET);
  int64_t bytesRead = (int64_t) fread(*buffer, 1, (size_t) fileSize, file);
  fclose(file);
  NMP_ASSERT(bytesRead == fileSize);

  return bytesRead;
}

//----------------------------------------------------------------------------------------------------------------------
int64_t NMFile::load(const char* fileName, void* buffer, uint64_t bufferSize)
{
  NMP_ASSERT(fileName && buffer && bufferSize);

  FILE* file = NULL;
  NM_FOPEN(file, fileName, "rb");
  if (file == NULL)
    return -1;

  fseek(file, 0, SEEK_SET);
  int64_t bytesRead = (int64_t) fread(buffer, 1, (size_t) bufferSize, file);
  fclose(file);
  NMP_ASSERT(bytesRead > 0);

  return bytesRead;
}

//----------------------------------------------------------------------------------------------------------------------
int64_t NMFile::save(const char* fileName, void* buffer, uint64_t bufferSize)
{
  FILE* file = NULL;
  NM_FOPEN(file, fileName, "wb");

  if (file == NULL)
    return -1;

  fseek(file, 0, SEEK_SET);

  int64_t bytesWritten = (int64_t) fwrite(buffer, 1, (size_t) bufferSize, file);
  fclose(file);

  return bytesWritten;
}

//----------------------------------------------------------------------------------------------------------------------
int64_t NMFile::getSize(const char* fileName)
{
  FILE* file = NULL;
  NM_FOPEN(file, fileName, "rb");

  if (file == NULL)
    return -1;

  fseek(file, 0, SEEK_END);
  int64_t retval = (int64_t) ftell(file);
  if (retval <= 0)
    retval = -1;
  fclose(file);

  return retval;
}

//----------------------------------------------------------------------------------------------------------------------
int64_t NMFile::getSize()
{
  int64_t retval = -1;
  if (isOpen())
  {
    fseek(m_file, 0, SEEK_END);
    retval = (int64_t) ftell(m_file);
    fseek(m_file, 0, SEEK_SET);
  }
  return retval;
}

//----------------------------------------------------------------------------------------------------------------------
bool NMFile::getExists(const char* fileName)
{
#ifdef NM_PLATFORM_SUPPORTS_STAT
  //just a directory query instead of open-close - querying the directory is much faster (factor 100 or above!)
  struct stat animFileStatus;
  int32_t intStatus = stat(fileName, &animFileStatus);
  return (intStatus == 0);
#else // NM_PLATFORM_SUPPORTS_STAT
  // the faster stat-check done for other platform is mostly only relevant for the asset compiler anyway.
  // thus we should be able to live with the expensive check here.
  FILE* file = fopen(fileName, "rb");
  if (file == NULL)
    return false;

  fclose(file);
  return true;
#endif // NM_PLATFORM_SUPPORTS_STAT
}

//----------------------------------------------------------------------------------------------------------------------
bool NMFile::create(const char* fileName, NMFileFlags flags)
{
  NMP_ASSERT(!m_file && m_mode == NM_FILE_OPEN_MODE_NONE);
  if (flags & NM_FILE_OPEN_MODE_READ)
  {
    m_mode = NM_FILE_OPEN_MODE_READ | NM_FILE_OPEN_MODE_WRITE;
    NM_FOPEN(m_file, fileName, "bw+");  // Create for reading and writing.
  }
  else
  {
    m_mode = NM_FILE_OPEN_MODE_WRITE;
    NM_FOPEN(m_file, fileName, "wb");   // Create for writing only.
  }

  if (m_file == NULL)
  {
    m_mode = NM_FILE_OPEN_MODE_NONE;
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool NMFile::open(const char* fileName, NMFileFlags flags)
{
  NMP_ASSERT(!m_file && m_mode == NM_FILE_OPEN_MODE_NONE);

  if (flags & NM_FILE_OPEN_MODE_WRITE)
  {
    if (flags & NM_FILE_OPEN_MODE_READ)
    {
      m_mode = NM_FILE_OPEN_MODE_WRITE | NM_FILE_OPEN_MODE_READ;
      NM_FOPEN(m_file, fileName, "r+");  // Open for reading and writing.
    }
    else
    {
      m_mode = NM_FILE_OPEN_MODE_WRITE;
      NM_FOPEN(m_file, fileName, "wb");  // Open for writing only.
    }
  }
  else
  {
    m_mode = NM_FILE_OPEN_MODE_READ;
    NM_FOPEN(m_file, fileName, "r");   // Open for reading only.
  }

  if (m_file == NULL)
  {
    m_mode = NM_FILE_OPEN_MODE_NONE;
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool NMFile::close()
{
  if (isOpen())
  {
    if (fclose(m_file) != 0)
      return false;
  }

  m_file = NULL;
  m_mode = NM_FILE_OPEN_MODE_NONE;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
int64_t NMFile::read(void* buffer, uint64_t bufferSize)
{
  NMP_ASSERT(buffer && (bufferSize > 0) && m_file && (m_mode & NM_FILE_OPEN_MODE_READ));

  int64_t bytesRead = (int64_t) fread(buffer, 1, (size_t) bufferSize, m_file);
  return bytesRead;
}

//----------------------------------------------------------------------------------------------------------------------
int64_t NMFile::write(void* buffer, uint64_t bufferSize)
{
  NMP_ASSERT(buffer && (bufferSize > 0) && m_file && (m_mode & NM_FILE_OPEN_MODE_WRITE));

  int64_t bytesWritten = (int64_t) fwrite(buffer, 1, (size_t) bufferSize, m_file);
  return bytesWritten;
}

//----------------------------------------------------------------------------------------------------------------------
bool NMFile::seek(uint64_t offset, NMFileSeekMode seekMode)
{
  int32_t seekFlag;
  int32_t result;

  NMP_ASSERT(m_file);
  switch (seekMode)
  {
  case NM_FILE_SEEK_BEGIN:
    seekFlag = SEEK_SET;
    break;
  case NM_FILE_SEEK_CURRENT:
    seekFlag = SEEK_CUR;
    break;
  case NM_FILE_SEEK_END:
    seekFlag = SEEK_END;
    break;
  default:
    NMP_DEBUG_MSG("Invalid file seek option.");
    NMP_ASSERT_FAIL();
    return false;
  }

  result = NM_FSEEK(m_file, (uint32_t)offset, seekFlag);

  if (result == 0)
    return true; // success.

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool NMFile::isOpen() const
{
  if (m_file && m_mode != NM_FILE_OPEN_MODE_NONE)
    return true;
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool NMFile::flush()
{
  if (isOpen())
  {
    uint64_t curPos = (uint64_t)ftell(m_file);
    fflush(m_file);
    seek(curPos, NM_FILE_SEEK_BEGIN);
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
int64_t NMFile::size() const
{
  if (!isOpen())
    return -1;

  int64_t curPos = (int64_t) ftell(m_file);
  fseek(m_file, 0, SEEK_END);
  int64_t retval = (int64_t) ftell(m_file);
  if (retval <= 0)
    retval = -1;
  fseek(m_file, (long)curPos, SEEK_SET);

  return retval;
}

//----------------------------------------------------------------------------------------------------------------------
bool NMFile::removeQuotesFromPath(const char *inPath, uint32_t NMP_USED_FOR_ASSERTS(inPathLen), char *outPath, uint32_t outPathLen)
{
  NMP_ASSERT(outPathLen >= inPathLen);

  if (inPath[0] == '\"')
  {
    // we have a leading quote so remove it
    NMP_STRNCPY_S(outPath, outPathLen, inPath + 1);
    const size_t lastIndex = NMP_STRLEN(outPath) - 1;
    if (outPath[lastIndex] == '\"')
    {
      // remove the trailing quote
      outPath[lastIndex] = '\0';
    }
    else
    {
      // We had a leading quote but no trailing quote
      NMP_ASSERT_FAIL_MSG("Invalid filename: %s", inPath);
      return false;
    }
  }
  else
  {
    // No quotes, so just copy
    NMP_STRNCPY_S(outPath, outPathLen, inPath);
  }
  return true;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
