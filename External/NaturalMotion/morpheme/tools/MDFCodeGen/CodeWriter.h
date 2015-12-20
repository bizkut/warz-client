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

#include <string>
#include <vector>

//----------------------------------------------------------------------------------------------------------------------

class HeaderGuard;

//----------------------------------------------------------------------------------------------------------------------
class CodeWriter
{
public:
  typedef int BraceID;

  CodeWriter(int maxColumns = kDefaultMaxColunns);
  ~CodeWriter();

  void begin();
  void writeContentsAndClose(FILE *fileHandle);

  const char* getContents() const { return m_writeBuf; }

  BraceID openBrace(BraceID parent=-1, bool addNewLine=true);
  void closeBrace(bool addNewLine=true);
  void closeBraceWithSemicolon(bool addNewLine=true);
  void closeBraceWithComma(bool addNewLine=true);
  void closeAllBraces();
  void write(const char* format, ...);
  void writeComment(const char* format, ...);
  void writeDoxygenMemberComment(const char* comment, ...);
  void writeDoxygenClassComment(const char* className, const char* group, const char* brief, const char* desc, ...);
  void writeRaw(const char* format, ...);
  inline void newLine() { doWrite("\n", 1); }
  void newLines(int nLines=1);
  void writeHorizontalSeparator();
  std::string makeTabString(int extraTabs = 0) const;
  void writeTab();
  void writeTabs();
  void writeTabsLessOne();
  void writeSpacing(int numSpaces);
  void writeWithWrapping(const char* input, const char* prefix, int maxcolumns, bool prefixFirstLine);
  void indent(int offset) { m_openBraceCount += offset; }

  void writeNMCopyrightHeader();
  void writeHeaderTop(const HeaderGuard& hg);
  void writeHeaderBottom(const HeaderGuard& hg);

  // comma-separated-list helper
  void startCommaSeparatedList();
  void addToCommaSeparatedList(const char* format, ...);
  void emitCommaSeparatedList(int extraTabs = 0, int eolEveryNth = 1, bool addFirstComma = false);
  bool hasCommaSeparatedListEntries();

  // unique headers helpers
  void startUniqueHeaders();
  unsigned int addUniqueHeaderGroup(const char* group);
  void addUniqueHeader(unsigned int groupID, const char* format, ...);
  void emitUniqueHeaders();

  enum Accessibility
  {
    kDefault = -1,
    kPublic,
    kProtected,
    kPrivate,
    kNumAccessibilityBlocks
  };

  void resetAccessibility(Accessibility ac);
  void switchAccessibility(Accessibility ac);

private:

  void doWrite(const char* buf, int sz = -1);

  enum Constants
  {
    kBufferSize = 4096,
    kWriteBufferSize = 1024 * 1024,
    kDefaultTabSize = 2,
    kDefaultMaxColunns = 80,
    kPrebuiltCharLengths = 64
  };

  int             m_openBraceCount;
  int             m_maxColumns;

  char            m_tabs[kPrebuiltCharLengths];
  char            m_spaces[kPrebuiltCharLengths];

  typedef std::vector<std::string> Strings;
  Strings         m_csl;

  // for formatted printing
  char            m_buffer[kBufferSize];

  unsigned int    m_writeBufIndex,
                  m_writeBufReallocations;
  char           *m_writeBuf;

  Accessibility   m_accessibility;

  // 'unique header' support
  struct UniqueHeader
  {
    UniqueHeader(char *incfile, unsigned int group) :
      m_file(incfile), m_group(group) {}

    char*         m_file;
    unsigned int  m_group;
  };
  typedef std::vector<UniqueHeader>   UniqueHeaders;
  typedef std::vector<std::string>    UniqueHeaderGroups;
  static bool sortByGroupID(UniqueHeader& elem1, UniqueHeader& elem2);

  void cleanupUniqueHeaders();

  UniqueHeaders       m_headers;
  UniqueHeaderGroups  m_headerGroups;

};

//----------------------------------------------------------------------------------------------------------------------
/**
 * little helper class that generates consistent header guard labels, eg
 * NM_MDF_FOO_H
 */
class HeaderGuard
{
public:

  explicit HeaderGuard(const char* label, ...);

  const char* c_str() const { return m_buf; }
  size_t length() const { return m_bufLen; }

protected:

  char    m_buf[128];
  size_t  m_bufLen;
};
