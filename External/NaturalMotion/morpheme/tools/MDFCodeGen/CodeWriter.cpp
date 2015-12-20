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

#include "Precompiled.h"
#include "CodeWriter.h"

//----------------------------------------------------------------------------------------------------------------------
CodeWriter::CodeWriter(int maxColumns) : 
  m_openBraceCount(0),
  m_writeBufReallocations(1),
  m_writeBufIndex(0),
  m_accessibility(kDefault)
{
  m_writeBuf = (char*)malloc(kWriteBufferSize);
  memset(m_spaces, ' ', kPrebuiltCharLengths);

  m_maxColumns = maxColumns;

  m_headers.reserve(24);
  m_headerGroups.reserve(16);
}

//----------------------------------------------------------------------------------------------------------------------
CodeWriter::~CodeWriter()
{
  cleanupUniqueHeaders();
  free(m_writeBuf);
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::begin()
{
  m_accessibility = kDefault;
  m_writeBufIndex = 0;
  m_openBraceCount = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::writeContentsAndClose(FILE *fileHandle)
{
  if (m_writeBufIndex > 0)
  {
    fwrite(m_writeBuf, m_writeBufIndex, 1, fileHandle);
  }

  NMP_ASSERT(m_openBraceCount == 0);
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::doWrite(const char* buf, int sz)
{
  if (sz < 0)
  {
    sz = (int)strlen(buf);
  }

  if (sz == 0)
    return;

  // bump the write buffer if we run out of space
  if (m_writeBufIndex + sz >= (m_writeBufReallocations * kWriteBufferSize))
  {
    m_writeBufReallocations ++;
    m_writeBuf = (char*)realloc(m_writeBuf, m_writeBufReallocations * kWriteBufferSize);
  }

  // copy this new data in
  memcpy(&m_writeBuf[m_writeBufIndex], buf, sz);

  m_writeBufIndex += sz;
  m_writeBuf[m_writeBufIndex] = '\0';
}

//----------------------------------------------------------------------------------------------------------------------
CodeWriter::BraceID CodeWriter::openBrace(BraceID parent, bool addNewLine)
{
  if (parent != -1)
  {
    while(m_openBraceCount > parent)
      closeBrace();
  }

  writeTabs();

  doWrite("{", 1);

  if (addNewLine)
    newLine();
  m_openBraceCount++;

  return m_openBraceCount;
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::write(const char* format, ...)
{
  va_list args;
  va_start(args, format);

  size_t bufLength = NMP_VSPRINTF(m_buffer, kBufferSize - 1, format, args);
  va_end(args);

  // failed to sprintf?
  NMP_ASSERT(bufLength != -1);

  // Indent labels by one less (for public:, etc)
  // or not at all, for #pragma, etc
  if (m_buffer[0] != '#')
  {
    if (m_buffer[bufLength - 1] == ':')
      writeTabsLessOne();
    else
      writeTabs();
  }

  doWrite(m_buffer, (int)bufLength);
  newLine();
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::writeComment(const char* format, ...)
{
  va_list args;
  va_start(args, format);

  NMP_VSPRINTF(m_buffer, kBufferSize - 1, format, args);
  va_end(args);

  writeWithWrapping(m_buffer, (makeTabString() + "// ").c_str(), m_maxColumns, true);
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::writeDoxygenMemberComment(const char* format, ...)
{
  va_list args;
  va_start(args, format);

  NMP_VSPRINTF(m_buffer, kBufferSize - 1, format, args);
  va_end(args);    

  std::string tabstring = makeTabString();
  doWrite(tabstring.c_str(), tabstring.length());
  doWrite("/** \n", 5);
  
  writeWithWrapping(m_buffer, (tabstring + " * ").c_str(), m_maxColumns, true);

  doWrite(tabstring.c_str(), tabstring.length());
  doWrite(" */\n", 4);
}

//----------------------------------------------------------------------------------------------------------------------
// Writes a doxygen comment that is suitable for use as a description for a class.
//
void CodeWriter::writeDoxygenClassComment(const char* className, const char* group, 
                                            const char* brief, const char* desc, ...) 
{
  va_list args;
  va_start(args, desc);

  NMP_VSPRINTF(m_buffer, kBufferSize - 1, desc, args);
  va_end(args);    

  std::string tabstring = makeTabString();
  doWrite(tabstring.c_str(), tabstring.length());
  doWrite("/** \n", 5);

  if (className)
  {
    size_t cLen = strlen(className);
    if (cLen > 0)
    {
      doWrite(tabstring.c_str(), tabstring.length());
      doWrite(" * \\class ");
      doWrite(className, cLen);
      doWrite("\n", 1);
    }
  }
  
  if (group)
  {
    size_t gLen = strlen(group);
    if (gLen > 0)
    {
      doWrite(tabstring.c_str(), tabstring.length());
      doWrite(" * \\ingroup ");
      doWrite(group, gLen);
      doWrite("\n", 1);
    }
  }
  
  if (brief)
  {
    size_t bLen = strlen(brief);
    if (bLen > 0)
    {
      doWrite(tabstring.c_str(), tabstring.length());
      doWrite(" * \\brief ");
      doWrite(brief, bLen);
    }
  }
  
  if (desc && strlen(desc) > 0)
  {
    size_t dLen = strlen(desc);
    if (dLen > 0)
    {
      doWrite(tabstring.c_str(), tabstring.length());
      doWrite(" *\n", 3);

      writeWithWrapping(m_buffer, (tabstring + " * ").c_str(), m_maxColumns, true);
    }
  }

  doWrite(tabstring.c_str(), tabstring.length());
  doWrite(" */\n", 4);
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::writeRaw(const char* format, ...)
{
  va_list args;
  va_start(args, format);

  int rLen = NMP_VSPRINTF(m_buffer, kBufferSize - 1, format, args);
  va_end(args);

  doWrite(m_buffer, rLen);
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::newLines(int nLines)
{
  for (int i = 0; i < nLines; ++i)
    newLine();
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::writeHorizontalSeparator()
{
  doWrite("//----------------------------------------------------------------------------------------------------------------------\n");
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::closeAllBraces()
{
  while (m_openBraceCount)
    closeBrace();

  newLine();
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::closeBrace(bool addNewLine)
{
  if (m_openBraceCount > 0)
  {
    m_openBraceCount--;
    writeTabs();

    doWrite("}", 1);

    if (addNewLine)
      newLine();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::closeBraceWithSemicolon(bool addNewLine)
{
  closeBrace(false);
  doWrite(";", 1);

  if (addNewLine)
    newLine();
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::closeBraceWithComma(bool addNewLine)
{
  closeBrace(false);
  doWrite(",", 1);

  if (addNewLine)
    newLine();
}

//----------------------------------------------------------------------------------------------------------------------
std::string CodeWriter::makeTabString(int extraTabs) const
{
  std::string tabString;
  int braceCount = m_openBraceCount + extraTabs;

  tabString.reserve(braceCount * 2);
  for (int i = 0; i < braceCount; i++) 
    tabString += "  ";
  return tabString;
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::writeTab()
{
  doWrite(m_spaces, kDefaultTabSize);
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::writeTabs()
{
  NMP_ASSERT(m_openBraceCount * kDefaultTabSize < kPrebuiltCharLengths);
  doWrite(m_spaces, m_openBraceCount * kDefaultTabSize);
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::writeTabsLessOne()
{
  NMP_ASSERT((m_openBraceCount - 1) * kDefaultTabSize < kPrebuiltCharLengths);
  doWrite(m_spaces, (m_openBraceCount - 1) * kDefaultTabSize);
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::writeSpacing(int numSpaces)
{
  NMP_ASSERT(numSpaces < kPrebuiltCharLengths);
  doWrite(m_spaces, numSpaces);
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::writeNMCopyrightHeader()
{
  struct tm localTime;
  time_t ltime;
  time(&ltime);
  localtime_s(&localTime, &ltime);

  static const char* nmCopyrightA = 
  {"\
/*\n\
 * Copyright (c) "
  };
  static const char* nmCopyrightB = 
  {" NaturalMotion Ltd. All rights reserved.\n\
 *\n\
 * Not to be copied, adapted, modified, used, distributed, sold,\n\
 * licensed or commercially exploited in any manner without the\n\
 * written consent of NaturalMotion.\n\
 *\n\
 * All non public elements of this software are the confidential\n\
 * information of NaturalMotion and may not be disclosed to any\n\
 * person nor used for any purpose not expressly approved by\n\
 * NaturalMotion in writing.\n\
 *\n\
 */\n\n"
  };

  char itBuf[33];
  _itoa_s( (int)(localTime.tm_year + 1900), itBuf, sizeof(itBuf), 10);

  doWrite(nmCopyrightA);
  doWrite(itBuf);
  doWrite(nmCopyrightB);

  doWrite("//----------------------------------------------------------------------------------------------------------------------\n");
  doWrite("//                                  This file is auto-generated\n");
  doWrite("//----------------------------------------------------------------------------------------------------------------------\n\n");
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::writeHeaderTop(const HeaderGuard& hg)
{
  doWrite("#pragma once");
  newLines(2);

  writeNMCopyrightHeader();

  doWrite("#ifndef ");
  doWrite(hg.c_str(), hg.length());
  newLine();

  doWrite("#define ");
  doWrite(hg.c_str(), hg.length());
  newLines(2);
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::writeHeaderBottom(const HeaderGuard& hg)
{
  doWrite("#endif // ");
  doWrite(hg.c_str(), hg.length());
  newLines(2);
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::writeWithWrapping(const char* input, const char* prefix, int maxcolumns, bool prefixFirstLine)
{
  std::string separators(" ");
  std::string whitespace(" \n");
  bool doPrefix = prefixFirstLine;

  size_t prefixLength = strlen(prefix);
  unsigned int maxwidth = 0;
  {
    size_t maxw = maxcolumns - prefixLength - 1;
    if(maxw < 1) 
      return;
    maxwidth = (unsigned)maxw;
  }

  std::string text(input);

  while(text.length() > maxwidth)
  {
    std::string output(text.substr(0, maxwidth));
    std::string::size_type breaklocation = output.find_last_of(separators);
    if(breaklocation != std::string::npos) 
    {
      output.erase(breaklocation, output.length() - breaklocation);
      text.erase(0, output.length());
    }
    else
    {
      // if no separator, continue until next separator
      text.erase(0, output.length());
      std::string::size_type extrachars = text.find_first_of(separators);
      if(extrachars == std::string::npos)
        extrachars = text.length();
      output.append(text, 0, extrachars);
      text.erase(0, extrachars);
    }

    // check for newline
    std::string::size_type newlinepos = output.find("\n");
    if(newlinepos != std::string::npos)
    {
      text = output.substr(newlinepos+1, output.length()-newlinepos-1) + text;
      output.erase(newlinepos, output.length()-newlinepos);
    }

    // trim leading whitespace
    std::string::size_type begintext = text.find_first_not_of(whitespace);
    if(begintext == std::string::npos) // all whitespace
      text.erase();
    else if(begintext != 0)
      text.erase(0, begintext);

    if (doPrefix)
      doWrite(prefix, (int)prefixLength);


    doWrite(output.c_str(), output.length());
    newLine();
    doPrefix = true;
  }

  if(text.length() > 0)
  {
    if (doPrefix)
      doWrite(prefix, (int)prefixLength);

    doWrite(text.c_str(), text.length());
    newLine();
  }
}

// -------------------------------------------------------------------------------------------------
void CodeWriter::startCommaSeparatedList()
{
  m_csl.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::addToCommaSeparatedList(const char* format, ...)
{
  va_list args;
  va_start(args, format);

  NMP_VSPRINTF(m_buffer, kBufferSize - 1, format, args);
  va_end(args);

  m_csl.push_back(m_buffer);
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::emitCommaSeparatedList(int extraTabs, int eolEveryNth, bool addFirstComma)
{
  if (m_csl.size() == 0) 
    return;

  Strings::const_iterator it = m_csl.begin(),
                          end = m_csl.end(),
                          penultimate = --m_csl.end();

  doWrite(m_spaces, extraTabs * kDefaultTabSize);
  writeTabs();

  if (addFirstComma)
    doWrite(", ", 2);

  int eolCount = eolEveryNth;
  for (; it != end; ++it)
  {
    eolCount --;

    doWrite((*it).c_str(), (*it).length());

    if (it != penultimate)
    {
      doWrite(", ", 2);

      if (eolCount == 0)
      {
        newLine();

        doWrite(m_spaces, extraTabs * kDefaultTabSize);
        writeTabs();

        eolCount = eolEveryNth;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool CodeWriter::hasCommaSeparatedListEntries()
{
  return (!m_csl.empty());
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::cleanupUniqueHeaders()
{
  UniqueHeaders::const_iterator it, end;
  it = m_headers.begin();
  end = m_headers.end();
  for (; it != end; ++it)
  {
    delete (*it).m_file;
  }

  m_headers.clear();
  m_headerGroups.clear();

  // add a default so IDs start from 1 (we ignore any 0-index groupIDs)
  static const std::string csDefault("default");
  m_headerGroups.push_back(csDefault);
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::startUniqueHeaders()
{
  cleanupUniqueHeaders();
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int CodeWriter::addUniqueHeaderGroup(const char* group)
{
  unsigned int grpID = (unsigned int)m_headerGroups.size();
  m_headerGroups.push_back(group);

  return grpID;
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::addUniqueHeader(unsigned int groupID, const char* format, ...)
{
  va_list args;
  va_start(args, format);
  NMP_VSPRINTF(m_buffer, kBufferSize - 1, format, args);
  va_end(args);    


  UniqueHeaders::iterator it, end;
  it = m_headers.begin();
  end = m_headers.end();

  // Awooga, linear search; might be quicker with a hash table
  for (; it != end; ++it)
  {
    const char* ed = (*it).m_file;
    if (strcmp(ed, m_buffer) == 0)
    {
      // allow a lower group ID to take priority, moving
      // the exiting item up the list, post-sort
      if (groupID < (*it).m_group)
        (*it).m_group = groupID;

      return;
    }
  }

  // take a copy and add to the list
  char *copy = (char*)malloc(strlen(m_buffer) + 1);
  NMP_STRNCPY_S(copy, strlen(m_buffer) + 1, m_buffer);
  m_headers.push_back(UniqueHeader(copy, groupID));
}

//----------------------------------------------------------------------------------------------------------------------
bool CodeWriter::sortByGroupID(UniqueHeader& elem1, UniqueHeader& elem2)
{
  return (elem1.m_group < elem2.m_group);
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::emitUniqueHeaders()
{
  unsigned int curGroupID = 0;
  bool firstIteration = true;

  // sort by groups in ascending order
  std::sort( m_headers.begin(), m_headers.end(), sortByGroupID );

  UniqueHeaders::const_iterator it, end;
  it = m_headers.begin();
  end = m_headers.end();
  for (; it != end; ++it)
  {
    const UniqueHeader& uh = (*it);

    // switching groups? 
    if (curGroupID != uh.m_group)
    {
      // leave a breaker line if this isn't the first
      // thing printed
      if (!firstIteration)
        newLine();

      writeComment(m_headerGroups.at(uh.m_group).c_str());
      curGroupID = uh.m_group;
    }

    write("#include %s", (*it).m_file);
    firstIteration = false;
  }

  cleanupUniqueHeaders();
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::resetAccessibility(Accessibility ac)
{
  NMP_ASSERT(ac > kDefault && ac < kNumAccessibilityBlocks);
  m_accessibility = ac;
}

//----------------------------------------------------------------------------------------------------------------------
void CodeWriter::switchAccessibility(Accessibility ac)
{
  NMP_ASSERT(ac > kDefault && ac < kNumAccessibilityBlocks);
  static char* blockNames[kNumAccessibilityBlocks] = 
  {
    "public:",
    "protected:",
    "private:",
  };

  if (m_accessibility != ac)
  {
    write(blockNames[(int)ac]);
    newLine();
    m_accessibility = ac;
  }
}

//----------------------------------------------------------------------------------------------------------------------
HeaderGuard::HeaderGuard(const char* label, ...)
{
  char buffer[100];
  NMP_ASSERT(strlen(label) < 100);

  va_list args;
  va_start(args, label);
  NMP_VSPRINTF(buffer, 100 - 1, label, args);
  va_end(args);  

  m_bufLen = NMP_SPRINTF(m_buf, "NM_MDF_%s_H", buffer);
  NMP_STRUPR(m_buf, sizeof(m_buf));
}
