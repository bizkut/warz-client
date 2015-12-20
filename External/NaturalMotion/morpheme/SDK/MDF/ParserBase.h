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

#include "MDF.h"

class MDFSyntaxHighlightingInterface;
class MDFFeedbackInterface;

//----------------------------------------------------------------------------------------------------------------------
// helper class used to compute fancy enum declarations, eg.
//
// eNumSocks = 10 + rigNumLegs * 2,
//
class ArithmeticComputer
{
public:

  enum
  {
    acOp_Add = -3,
    acOp_Sub = -2,
    acOp_Mul = -1,
  };

  inline void clear()
  {
    m_ops.reset();
    m_strings.reset();
    m_noIdentifiers = true;
  }

  int compute(StringBuilder& stringRepOut);

  FixedStack<int, 16>   m_ops;
  PStringStack16        m_strings;
  bool                  m_noIdentifiers;
};

//----------------------------------------------------------------------------------------------------------------------
// base class for all the context objects that are used to track progress during a parse; different grammars will derive
// from this base and add their own transient data - eg. 'module being built', 'current variable being structured', etc
//
class ContextBase
{
public:

  ContextBase() : 
    m_lowDateTime(0),
    m_highDateTime(0),
    m_clientUserData(0),
    m_keywords(0),
    m_typesRegistry(0),
    m_constants(0),
    m_modifierOptionsStack(0)
  {}
  
  virtual ~ContextBase() {}

  // data populated during file parse
  unsigned long         m_lowDateTime,    // the high/low of 64bit file time
                        m_highDateTime;
  PString               m_sourceFilePath; // may be empty, if parsing from memory stream
  void                 *m_clientUserData;

  // instances of global managers/tools
  KeywordsManager      *m_keywords;
  TypesRegistry        *m_typesRegistry;
  NetworkConstants     *m_constants;

  // modifiers being built - this is used by all the grammars
  ModifierOptions      *m_modifierOptionsStack;

  // simple +/-/* computer used with fancy enum definitions
  ArithmeticComputer    m_arithComp;

  // variable declaration under construction - used by all grammars, this
  // stores the intermediary state as we walk and assemble a complete variable declaration
  VarDeclConstruction   m_vdc;
};

//----------------------------------------------------------------------------------------------------------------------
// the base class for all other grammar parsers in the system
//
class ParserBase
{
public:

  virtual ~ParserBase();

  // the parser entrypoints; running from a file or direct from memory, with the option
  // of doing stream-time conversion from wchar_t
  ParseResult runFromFile(const char* filename, ContextBase* context);
  ParseResult runFromMemory(const char* stream, size_t streamLength, ContextBase* context);
  ParseResult runFromMemory(const wchar_t* stream, size_t streamLength, ContextBase* context);

protected:

  // two stages of running the parse; separated because it is required for any file locks
  // to be reqlinquished before token execution begins.
  // call runParser() to create the token hierarchy, then pass the result of that call into
  // executeTokens() to generate the final result. 
  ParseResult runParser(ParserContext& pfc, Token **rootOutputToken);
  ParseResult executeTokens(ParseResult parseRunResult, Token *rootOutputToken);

  // called by derived parsers to pass their grammar defs back down to the base
  ParserBase(const Grammar& grammar, MDFFeedbackInterface* fbi, MDFSyntaxHighlightingInterface *shi);

  void evaluateTokens(Token* token, int ruleEnum);
  const char* functionDeclarationConcat(const Grammar& grammar, Token *token, unsigned int startToken = 0);

  ContextBase                      *m_context;                  // context structure set per-grammar type
  const Grammar                    &m_grammar;                  // grammar set by auto-generated inheritant class

  StringBuilder                    *m_stringBuilder;            // a string builder for general use during parsing
  SavedComments                     m_savedComments;            // comment look-up table, built during runParser(), acted upon in executeTokens()

  int                               m_trackingLastSeenSymbol,   // used in recursive appends, stores last symbol ID stored in buffer
                                    m_trackingLastHitLine;      // line of last executed token; used when figuring sending blocks to SHL

  MDFSyntaxHighlightingInterface   *m_shInterface;              // null if we're not running in syntax highlighting mode
  MDFFeedbackInterface             *m_feedbackInterface;        // instance of feedback class to be used to report on progress, failures, etc

private:

  virtual void executeRule(Token *token)=0;
  virtual void processSavedComments(const SavedComments &savedComments)=0;
  void printParserError(Token *token, const char* file, ParseResult result);
  virtual void recursiveFunctionDeclarationConcat(const Grammar& grammar, Token *token, unsigned int startToken)=0;

  ParserBase();
  ParserBase(const ParserBase& rhs);
  ParserBase& operator = (const ParserBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// thrown by exceptions during rule execution
//
struct RuleExecutionException 
{
  RuleExecutionException(Token *token, const char* ref, const char* msg) :
    m_token(token),
    m_ref(ref),
    m_msg(msg)
  {
  }

  Token          *m_token;
  PString         m_ref;
  PString         m_msg;

private:
  RuleExecutionException();
};

//----------------------------------------------------------------------------------------------------------------------
// thrown by exceptions during validation phases
//
struct ValidationException 
{
  ValidationException(const FileResource* fr, unsigned int atLine, const char* ref, const char* msg) :
    m_fileResc(fr), 
    m_atLine(atLine), 
    m_ref(ref), 
    m_msg(msg) 
  {
  }

  const FileResource   *m_fileResc;
  unsigned int          m_atLine;
  PString               m_ref;
  PString               m_msg;

private:
  ValidationException();
};
