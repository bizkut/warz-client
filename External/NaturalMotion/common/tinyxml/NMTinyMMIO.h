#ifndef NM_TINYXML_MMIO
#define NM_TINYXML_MMIO
#include <string>
#include <assert.h>

#include "NMTinyFastHeap.h"
#include "NMTinyStr.h"

namespace NM
{

class TINYXML_PUBLIC TiXmlMemMapReader
{
public:

  TiXmlMemMapReader();
  ~TiXmlMemMapReader();

  bool open(const char* path);
  bool close();

  inline unsigned char* getData() const { return m_base; }
  inline size_t getFileSize() const { return m_fileSize; }

protected:

  unsigned char*  m_base;
  size_t          m_fileSize;
  void*           m_fileHandle;
  void*           m_fileMapHandle;
};


// This is writing to a memory block and writing to the target file on
// "destruction", so while being not really a memory mapped file, it's
// much faster than writing directly, anyway.
//
class TINYXML_PUBLIC TiXmlBufferedTextWriter
{
public:

  /// \brief constructs a new stream with the given amount of reserved bytes.
  TiXmlBufferedTextWriter(const char* fn, unsigned int initialSize = 1024);

  /// \brief writes (if not already written) and releases the allocated memory
  ~TiXmlBufferedTextWriter();

  /// \brief writes the file
  bool writeToFile(int *error = NULL);

  /// \brief returns true if the current position is the last one.
  inline bool eof() const;

  /// \brief reads the amount of data passed - returns NULL if the data could not be read.
  /// \note currently returns a "window" into the buffered memory to prevent memory copies!
  const unsigned char* read(unsigned int numBytes) const;

  //----------------------------------------------------------------------------------------------------------------------
  // Stream operations for simple interactions
  //----------------------------------------------------------------------------------------------------------------------
  // writes
  inline TiXmlBufferedTextWriter& operator<<(const char* str);
  inline TiXmlBufferedTextWriter& operator<<(const std::string& str);
  inline TiXmlBufferedTextWriter& operator<<(const TiXmlString& str);
  inline TiXmlBufferedTextWriter& operator<<(const unsigned char val);

private:

  /// \brief save the buffer to the given file (overwrites) - returns false if save failed.
  bool fileSave(const char* fn, int *error = NULL) const;

  /// \brief sets current buffer position to the position passed.
  void seekAbs(unsigned int pos);

  /// \brief sets current buffer position relative to the position passed.
  void seek(int offset);

  /// \brief returns the current buffer position
  inline unsigned int size() const;

  /// \brief returns the current buffer position
  inline unsigned int pos() const;

  /// \brief writes the data to the buffer - resizes buffer if needed
  void resize(unsigned int targetSize);

  /// \brief writes the data to the buffer - resizes buffer if needed
  void write(void *data, unsigned int count);

  unsigned char* m_buffer;         ///< the actual memory buffer - gets doubled on each resize.
  unsigned int   m_bufferLimit;    ///< this it the read-limit that might not match the size if the stream is memory-only
  unsigned int   m_bufferSize;     ///< buffer size of the the memory allocated and stored in m_Buffer
  unsigned char* m_bufferPosition; ///< actual pointer to the current reading-position
  const char*    m_targetFn;
};

//----------------------------------------------------------------------------------------------------------------------
inline bool TiXmlBufferedTextWriter::eof() const
{
  return pos() >= m_bufferLimit;
}

//----------------------------------------------------------------------------------------------------------------------
inline unsigned int TiXmlBufferedTextWriter::pos() const
{
  return (unsigned int)(m_bufferPosition - m_buffer);
}

//----------------------------------------------------------------------------------------------------------------------
inline unsigned int TiXmlBufferedTextWriter::size() const
{
  return m_bufferSize;
}

//----------------------------------------------------------------------------------------------------------------------
inline TiXmlBufferedTextWriter& TiXmlBufferedTextWriter::operator<<(const char* str)
{
  unsigned int len = (unsigned int)strlen(str);
  write((void*)str, len);
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
inline TiXmlBufferedTextWriter& TiXmlBufferedTextWriter::operator<<(const std::string& str)
{
  unsigned int len = (unsigned int)str.length();
  write((void*)str.c_str(), len);
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
inline TiXmlBufferedTextWriter& TiXmlBufferedTextWriter::operator<<(const TiXmlString& str)
{
  unsigned int len = (unsigned int)str.length();
  write((void*)str.c_str(), len);
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
inline TiXmlBufferedTextWriter& TiXmlBufferedTextWriter::operator<<(const unsigned char val)
{
  write((void*)&val, 1);
  return *this;
}


} // namespace NM

#endif // TINYXML_MMIO
