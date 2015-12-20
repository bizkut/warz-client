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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef NMP_FILE_H
#define NMP_FILE_H
//----------------------------------------------------------------------------------------------------------------------

// Suppress warning coming from xstring in PS3 SDK.
#ifdef NM_COMPILER_SNC
  #pragma diag_push
  #pragma diag_suppress=1669
#endif

#include <stdio.h>

#ifdef NM_COMPILER_SNC
  #pragma diag_pop
#endif

#include "NMPlatform/NMPlatform.h"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif // MAX_PATH

// NOTE: If this include fails to find NMFile.inl, the platform-specific NMPlatform include is missing from your
//  include path.
#include "NMFile.inl"

//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
#define NM_FILE_OPEN_MODE_NONE  (1 << 0)
#define NM_FILE_OPEN_MODE_READ  (1 << 1)
#define NM_FILE_OPEN_MODE_WRITE (1 << 2)

typedef uint32_t NMFileFlags;

//----------------------------------------------------------------------------------------------------------------------
/// \enum  NMP::NMFileSeekMode
/// \brief
enum NMFileSeekMode
{
  NM_FILE_SEEK_BEGIN,
  NM_FILE_SEEK_CURRENT,
  NM_FILE_SEEK_END
};

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::File
/// \brief A logger that does nothing.
/// \ingroup NaturalMotionPlatform
///
/// Implements BasicLogger, but does nothing. Useful when profiling.
//----------------------------------------------------------------------------------------------------------------------
class NMFile
{
public:
  NMFile();
  ~NMFile();

  /// \brief wipe down a class instance ready for use.
  ///
  /// Note the constructor automatically calls this function.
  void init();

  /// \brief Open a file, allocate a buffer of the files size and load its entire contents into the buffer.
  /// \return Size of file read or -1 if load failed;
  static int64_t allocAndLoad(
    const char*  fileName,                        ///< Name of file to load.
    void**       buffer,                          ///< OUT: Allocated buffer.
    int64_t*     bufferSize,                      ///< OUT: Size of allocated buffer or -1 if no allocation occurs.
    uint32_t     alignment = NMP_VECTOR_ALIGNMENT ///< Required alignment of allocated block to write to.
  );

  /// \brief Open a file and load as much of its contents that will fit into the buffer provided.
  /// \return Amount of buffer filled from file -1 if load failed.
  static int64_t load(const char* fileName, void* buffer, uint64_t bufferSize);

  /// \brief Save the contents of a buffer to a file.
  /// \return Number of bytes written to file or -1 if write failed.
  ///
  /// Opens a new file or overwrites an existing one.
  static int64_t save(const char* fileName, void* buffer, uint64_t bufferSize);

  int64_t getSize();
  static int64_t getSize(const char* fileName);
  static bool getExists(const char* fileName);

  /// Destroys any existing file of the same name.
  bool create(const char* fileName, NMFileFlags flags = NM_FILE_OPEN_MODE_WRITE);

  /// File must already exist for operation to succeed.
  bool open(const char* fileName, NMFileFlags flags = NM_FILE_OPEN_MODE_READ);

  /// File must be open for operation to succeed.
  bool close();

  /// force a flush of the filebuffer
  bool flush();

  /// \return Number of bytes read or -1 on failure.
  int64_t read(void* buffer, uint64_t bufferSize);

  /// \return Number of bytes written or -1 on failure.
  int64_t write(void* buffer, uint64_t bufferSize);

  bool seek(uint64_t offset, NMFileSeekMode seekMode);

  /// \return Number of bytes in the file or -1 on failure.
  int64_t size() const;

  /// \brief Is this file handle open.
  bool isOpen() const;

  /// \brief Strips leading and trailing quotes from inPath and returns it in outPath
  static bool removeQuotesFromPath(const char *inPath, uint32_t inPathLen, char *outPath, uint32_t outPathLen);

private:
  FILE*        m_file;
  NMFileFlags  m_mode;   // Reading, writing etc.
};

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NMP_FILE_H
//----------------------------------------------------------------------------------------------------------------------
