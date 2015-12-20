#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "NMTinyMMIO.h"

namespace NM
{

//----------------------------------------------------------------------------------------------------------------------
TiXmlMemMapReader::TiXmlMemMapReader() :
  m_base(0), 
  m_fileSize(0), 
  m_fileHandle(0), 
  m_fileMapHandle(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
TiXmlMemMapReader::~TiXmlMemMapReader()
{
  close();
}

//----------------------------------------------------------------------------------------------------------------------
bool TiXmlMemMapReader::open(const char* path)
{
  // open file on disk  
  DWORD fileAccessFlags = GENERIC_READ;
  m_fileHandle = CreateFile(path, fileAccessFlags, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
  if (m_fileHandle == INVALID_HANDLE_VALUE) 
  {
    return false;
  }

  m_fileSize = GetFileSize(m_fileHandle, NULL);

  // create file mapping  
  DWORD accessFlags = PAGE_READONLY;
  m_fileMapHandle = CreateFileMapping(m_fileHandle, NULL, accessFlags, 0, (DWORD)m_fileSize, 0);

  if (!m_fileMapHandle)
  {
    close();
    return false;
  }

  DWORD viewFlags = FILE_MAP_READ;
  m_base = (unsigned char*)MapViewOfFile(m_fileMapHandle, viewFlags, 0, 0, 0);
  if (!m_base)
  {
    close();
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool TiXmlMemMapReader::close()
{
  if (m_base)
    UnmapViewOfFile(m_base);

  if (m_fileMapHandle)
    CloseHandle(m_fileMapHandle);

  if (m_fileHandle)
    CloseHandle(m_fileHandle);

  m_base = 0;
  m_fileMapHandle = 0;
  m_fileHandle = 0;

  return true;
}


//----------------------------------------------------------------------------------------------------------------------
TiXmlBufferedTextWriter::TiXmlBufferedTextWriter(const char *fn, unsigned int initialSize)
: m_buffer(NULL), m_bufferLimit(0), m_bufferSize(0), m_bufferPosition(NULL), m_targetFn(fn)
{
  resize(initialSize);
}

//----------------------------------------------------------------------------------------------------------------------
TiXmlBufferedTextWriter::~TiXmlBufferedTextWriter()
{
  if (m_targetFn)
  {
    fileSave(m_targetFn);
  }

  delete[] m_buffer;
}

//----------------------------------------------------------------------------------------------------------------------
bool TiXmlBufferedTextWriter::writeToFile(int *error)
{
  bool result = false;
  if (m_targetFn)
  {
    result = fileSave(m_targetFn, error);
    m_targetFn = NULL;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const unsigned char* TiXmlBufferedTextWriter::read(unsigned int numBytes) const
{
  if ((pos() + numBytes) < m_bufferSize)
  {
    return m_bufferPosition;
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
void TiXmlBufferedTextWriter::seekAbs(unsigned int pos)
{
  assert(pos < m_bufferSize);
  m_bufferPosition = m_buffer + (ptrdiff_t)pos;
}

//----------------------------------------------------------------------------------------------------------------------
void TiXmlBufferedTextWriter::seek(int offset)
{
  int absSeekPos = (int)pos() + offset;
  assert(absSeekPos >= 0 && absSeekPos < (int)m_bufferSize);
  
  if (absSeekPos < 0)
  {
    absSeekPos = 0;
  }
  else if (absSeekPos >= (int)m_bufferSize)
  {
    absSeekPos = m_bufferSize - 1;
  }

  seekAbs((unsigned int)absSeekPos);
}

//----------------------------------------------------------------------------------------------------------------------
void TiXmlBufferedTextWriter::write(void *data, unsigned int count)
{
  const unsigned int sizeNeeded = (unsigned int)(m_bufferPosition - m_buffer) + count;
  
  if (sizeNeeded > m_bufferSize)
  {
    resize(sizeNeeded);
  }

  memcpy_s(m_bufferPosition, m_bufferSize, data, count);
  m_bufferPosition += (ptrdiff_t)count;
  m_bufferLimit = pos() > m_bufferLimit ? pos() : m_bufferLimit;
}

//----------------------------------------------------------------------------------------------------------------------
void TiXmlBufferedTextWriter::resize(unsigned int targetSize)
{
  if (targetSize > m_bufferSize)
  {
    // pre-allocate more memory to prevent often resizing
    targetSize *=2;
    const ptrdiff_t offset = m_bufferPosition - m_buffer;
    unsigned char *newBuffer = new unsigned char[targetSize];
    memcpy_s(newBuffer, targetSize, m_buffer, m_bufferSize);
    delete[] m_buffer;
    m_buffer = newBuffer;
    m_bufferSize = targetSize;
    m_bufferPosition = m_buffer + offset;
  }
}


//----------------------------------------------------------------------------------------------------------------------
bool TiXmlBufferedTextWriter::fileSave(const char* fn, int *error) const
{
  FILE *m_file = fopen(fn, "wb");  
  if (m_file)
  {
    fwrite(m_buffer, 1, m_bufferLimit, m_file);
    fclose(m_file);
    return true;
  }
   
  if (error)
  {
    *error = errno;
  }
  return false;
}

} // namespace NM
