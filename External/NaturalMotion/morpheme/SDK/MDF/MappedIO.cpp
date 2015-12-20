// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------

#include "MDFPrecompiled.h"
#include "MappedIO.h"

//----------------------------------------------------------------------------------------------------------------------
MappedIO::MappedIO() :
  m_base(0), 
  m_fileSize(0), 
  m_fileHandle(0), 
  m_fileMapHandle(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
MappedIO::~MappedIO()
{
  close();
}

//----------------------------------------------------------------------------------------------------------------------
bool MappedIO::open(const char* path)
{
  // hint to the IO subsystem that we are reading sequentially
  DWORD fileHintFlags = FILE_ATTRIBUTE_READONLY | FILE_FLAG_SEQUENTIAL_SCAN;

  // open file on disk  
  DWORD fileAccessFlags = GENERIC_READ;
  m_fileHandle = CreateFile(path, fileAccessFlags, 0, NULL, OPEN_EXISTING, fileHintFlags, NULL);
  if (m_fileHandle == INVALID_HANDLE_VALUE) 
  {
    return false;
  }

  // store file size and FILETIME values while we have the handle
  m_fileSize = GetFileSize(m_fileHandle, NULL);
  GetFileTime(m_fileHandle, &m_creationTime, &m_lastAccessTime, &m_lastWriteTime);

  // create file mapping  
  DWORD accessFlags = PAGE_READONLY;
  m_fileMapHandle = CreateFileMapping(m_fileHandle, NULL, accessFlags, 0, m_fileSize, 0);
  if (!m_fileMapHandle)
  {
    close();
    return false;
  }

  // acquire pointer to mapped memory address for this file
  DWORD viewFlags = FILE_MAP_READ;
  m_base = (char*)MapViewOfFile(m_fileMapHandle, viewFlags, 0, 0, 0);
  if (!m_base)
  {
    close();
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool MappedIO::close()
{
  if (m_base)
    UnmapViewOfFile(m_base);

  if (m_fileMapHandle)
    CloseHandle(m_fileMapHandle);

  if (m_fileHandle)
    CloseHandle(m_fileHandle);

  m_base = 0;
  m_fileSize = 0;
  m_fileMapHandle = 0;
  m_fileHandle = 0;

  return true;
}
