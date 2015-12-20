#pragma once
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

/**
 * wrapper around Windows memory-mapped files; provides fast-as-possible sequential file
 * access for parsing.
 */
class MappedIO
{
public:

  MappedIO();
  ~MappedIO();

  bool open(const char* path);
  bool close();

  inline const char* getData() const { return m_base; }
  inline size_t getFileSize() const { return m_fileSize; }

  inline const FILETIME& getCreationTime() const { return m_creationTime; }
  inline const FILETIME& getLastAccessTime() const { return m_lastAccessTime; }
  inline const FILETIME& getWriteTime() const { return m_lastWriteTime; }

protected:

  char*         m_base;
  unsigned int  m_fileSize;
  void*         m_fileHandle;
  void*         m_fileMapHandle;
  FILETIME      m_creationTime,
                m_lastAccessTime,
                m_lastWriteTime;
};
