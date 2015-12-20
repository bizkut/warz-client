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

#include "nmtl/hashmap_dense_dynamic.h"

//----------------------------------------------------------------------------------------------------------------------
#ifndef False
# define False  false
#endif

#ifndef True
# define True   true
#endif

//----------------------------------------------------------------------------------------------------------------------
enum ParseResult
{
  prSuccess,              
  prEmptyFile,
  prLexError,             // input could not be tokenized
  prTokenError,           // input is an invalid token
  prSyntaxError,          // input does not match any rule
  prCommentError,
  prRuleExecuteError
};

//----------------------------------------------------------------------------------------------------------------------
enum SymbolKind
{
  skNonTerminal,
  skTerminal,
  skWhitespace,
  skEOF,
  skCommentStart,
  skCommentEnd,
  skCommentLine,
  skError
};

//----------------------------------------------------------------------------------------------------------------------
struct Symbol
{
  SymbolKind        m_kind;
  char             *m_name;
};

//----------------------------------------------------------------------------------------------------------------------
struct DFAEdge
{
  int               m_targetState;               // index into Grammar.m_DFAArray
  unsigned int      m_charCount;
  const char       *m_characterSet;
};

//----------------------------------------------------------------------------------------------------------------------
struct DFAState
{
  int               m_acceptSymbol;              // -1 (Terminal), or index into Grammar.m_symbolArray
  unsigned int      m_edgeCount;
  const DFAEdge    *m_edges;
};

//----------------------------------------------------------------------------------------------------------------------
struct Rule
{
  int               m_head;                      // index into Grammar.m_symbolArray
  unsigned int      m_symbolsCount;
  const int        *m_symbols;
  char             *m_description;
};

//----------------------------------------------------------------------------------------------------------------------
struct Action
{ 
  // operation indices match with GOLDs own
  enum Op
  {
    INVALID = -1,
    SHIFT = 1,
    REDUCE,
    GOTO,
    ACCEPT
  };
  int               m_entry;                  // index into Grammar.m_symbolArray
  Op                m_action;
  int               m_target;                 // if m_action=SHIFT then index into Grammar.m_LALRArray
                                              // if m_action=REDUCE then index into Grammar.RuleArray
                                              // if m_action=GOTO then index into Grammar.m_LALRArray
};

//----------------------------------------------------------------------------------------------------------------------
struct LALRState
{
  unsigned int      m_actionCount;
  const Action     *m_actions;
};

//----------------------------------------------------------------------------------------------------------------------
// grammar "entrypoint", the holding structure for all the precomputed tables
struct Grammar 
{
  bool              m_caseSensitive;
  int               m_initialSymbol;                  // index into Grammar.m_symbolArray
  int               m_initialDFAState;                // index into Grammar.m_DFAArray
  int               m_initialLalrState;               // index into Grammar.m_LALRArray
  int               m_symbolCount;
  const Symbol     *m_symbolArray;
  int               m_ruleCount;
  const Rule       *m_ruleArray;
  int               m_DFAStateCount;
  const DFAState   *m_DFAArray;
  int               m_LALRStateCount;
  const LALRState  *m_LALRArray;
};


//----------------------------------------------------------------------------------------------------------------------
struct Token 
{
  int               m_reductionRule;            // Index into Grammar.RuleArray[]
  Token           **m_tokens;                   // Array of reduction Tokens
  int               m_symbol;                   // Index into Grammar.SymbolArray[]
  char             *m_data;                     // String with data from the input
  unsigned int      m_line,
                    m_column,
                    m_pos,
                    m_length;
};


//----------------------------------------------------------------------------------------------------------------------
// FIFO stack of Tokens
struct TokenStack
{
  Token            *m_token;
  int               m_LALRState;
  TokenStack       *m_nextToken;
};


//----------------------------------------------------------------------------------------------------------------------
// comments saved by the parser
// stored as a hashmap, source line mapping to comment data
typedef nmtl::hashmap_dense_dynamic<unsigned int, const char*> SavedComments;


//----------------------------------------------------------------------------------------------------------------------
// A container used to encapsulate the information about
// where in the file stream the parser is currently working
//
class ParserMemory;
struct ParserContext
{
  ParserContext(const char *inputData, unsigned int inputDataSize, bool convertFromWCharT = false) :
    m_data(inputData), 
    m_dataSize(inputDataSize), 
    m_offset(0U), 
    m_line(1U), 
    m_column(1U),
    m_convertFromWideChar(convertFromWCharT)
  {}

  inline void incLine()
  {
    m_line ++;
    m_column = 1;
  }
  inline void incOffset()
  {
    m_offset ++;
  }
  inline void incColumn()
  {
    m_column ++;
  }

  unsigned int getDataSize() const { return m_dataSize; }
  unsigned int getOffset() const { return m_offset; }
  unsigned int getLine() const { return m_line; }
  unsigned int getColumn() const { return m_column; }


  inline const char get() const
  {
    if (m_convertFromWideChar)
    {
      const wchar_t* dataW = (const wchar_t*)m_data;
      char cnv = (char)dataW[m_offset];
      return cnv;
    }
    else
    {
      return m_data[m_offset];
    }
  }

  inline const char get(unsigned int offset) const
  {
    if (m_convertFromWideChar)
    {
      const wchar_t* dataW = (const wchar_t*)m_data;
      char cnv = (char)dataW[offset];
      return cnv;
    }
    else
    {
      return m_data[offset];
    }
  }

  inline bool isEmpty() const
  {
    if ((m_data == 0) || (m_dataSize == 0))
      return true;

    if (m_convertFromWideChar)
    {
      const wchar_t* dataW = (const wchar_t*)m_data;
      return (*dataW == 0);
    }
    else
    {
      return (*m_data == 0);
    }
  }

private:
  const char       *m_data;
  unsigned int      m_dataSize,
                    m_offset,
                    m_line,
                    m_column;
  bool              m_convertFromWideChar;

};


//----------------------------------------------------------------------------------------------------------------------
// helper function used in syntax highlighting rule-set; saves a lot of repeated typing!
#define SHLApply(_tkindex, _grm) \
  if (m_shInterface) \
    m_shInterface->apply(token->m_tokens[_tkindex]->m_line - 1, token->m_tokens[_tkindex]->m_column - 1, token->m_tokens[_tkindex]->m_length, MDFSyntaxHighlightingInterface::_grm);

#define SHLMarkBlock(_startLine, _endLine) \
  if (m_shInterface) \
    m_shInterface->markBlock(_startLine - 1, _endLine);
