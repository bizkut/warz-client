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
#include "ParserBase.h"
#include "ParserEngine.h"
#include "MappedIO.h"
#include "Utils.h"
#include "StringBuilder.h"
#include "LibMDF.h"


//----------------------------------------------------------------------------------------------------------------------
ParserBase::ParserBase(const Grammar& grammar, MDFFeedbackInterface* fbi, MDFSyntaxHighlightingInterface *shi) : 
  m_grammar(grammar),
  m_savedComments(128, ActiveMemoryManager::getForNMTL()),
  m_trackingLastSeenSymbol(-1),
  m_trackingLastHitLine(-1),
  m_shInterface(shi),
  m_feedbackInterface(fbi)
{
  m_stringBuilder = new StringBuilder(128);
}

  //----------------------------------------------------------------------------------------------------------------------
ParserBase::~ParserBase()
{
  delete m_stringBuilder;
}

//----------------------------------------------------------------------------------------------------------------------
ParseResult ParserBase::runFromFile(const char* filename, ContextBase* context)
{
  m_context = context;

  // map types file
  MappedIO mmInput;
  if (!mmInput.open(filename))
  {
    m_feedbackInterface->logMessage(MDFFeedbackInterface::Error, "Could not open '%s' to parse.", filename);
    return prEmptyFile;
  }

  // save file write time
  if (context)
  {
    FILETIME inputFT = mmInput.getWriteTime();
    m_context->m_lowDateTime = inputFT.dwLowDateTime;
    m_context->m_highDateTime = inputFT.dwHighDateTime;

    m_context->m_sourceFilePath = filename;
  }

  ParserContext pfc(mmInput.getData(), mmInput.getFileSize());

  Token *rootOutputToken;
  ParseResult pr = runParser(pfc, &rootOutputToken);

  mmInput.close();

  return executeTokens(pr, rootOutputToken);
}

//----------------------------------------------------------------------------------------------------------------------
ParseResult ParserBase::runFromMemory(const char* stream, size_t streamLength, ContextBase* context)
{
  m_context = context;

  // build a parsing context
  ParserContext pfc(stream, (unsigned int)streamLength);

  Token *rootOutputToken;
  ParseResult pr = runParser(pfc, &rootOutputToken);

  return executeTokens(pr, rootOutputToken);
}

//----------------------------------------------------------------------------------------------------------------------
ParseResult ParserBase::runFromMemory(const wchar_t* stream, size_t streamLength, ContextBase* context)
{
  m_context = context;

  // build a parsing context
  ParserContext pfc((char*)stream, (unsigned int)streamLength, true);

  Token *rootOutputToken;
  ParseResult pr = runParser(pfc, &rootOutputToken);

  return executeTokens(pr, rootOutputToken);
}

//----------------------------------------------------------------------------------------------------------------------
ParseResult ParserBase::runParser(ParserContext& pfc, Token **rootOutputToken)
{
  m_savedComments.clear();

  // lex & parse the file
  ParserEngine prs(&m_grammar, &pfc, m_shInterface);
  return prs.parse(rootOutputToken, m_savedComments);
}

//----------------------------------------------------------------------------------------------------------------------
ParseResult ParserBase::executeTokens(ParseResult parseRunResult, Token *rootOutputToken)
{
  // parse failure, dump error messages and bail out
  if (parseRunResult != prSuccess) 
  {
    printParserError(rootOutputToken, m_context->m_sourceFilePath.c_str(), parseRunResult);
  } 
  else 
  {
    // parse succeeded; now do something with the stack of tokens created.
    try
    {
      // begin rule execution by jumping to the root token; this will then 
      // recursively call its children, and so on, walking the parse tree
      executeRule(rootOutputToken);
    }
    catch (RuleExecutionException* e)
    {
      m_feedbackInterface->onProcessingFailure(
        MDFFeedbackInterface::Execution,
        m_context->m_sourceFilePath.c_str(),
        e->m_token->m_line - 1,
        e->m_msg.c_str(),
        e->m_ref.c_str(),
        m_context->m_clientUserData);

      parseRunResult = prRuleExecuteError;
    }
  }

  // let the handler examine any stored comments (unless we're running the syntax highlighter)
  if (parseRunResult == prSuccess && !m_shInterface && m_savedComments.getNumUsedSlots() > 0)
  {
    processSavedComments(m_savedComments);
  }

  return parseRunResult;
}

//----------------------------------------------------------------------------------------------------------------------
// default handler function that just recursively calls any rules found while
// iterating the given token's child tokens
void ParserBase::evaluateTokens(Token *token, int) 
{
  unsigned int i, symbolCount;
  const char* ruleDesc; 

  symbolCount = m_grammar.m_ruleArray[token->m_reductionRule].m_symbolsCount;
  ruleDesc    = m_grammar.m_ruleArray[token->m_reductionRule].m_description;

  // walk all child tokens, calling the rule handlers for any rules found
  for (i=0; i<symbolCount; i++) 
  {
    // token is a rule
    if (token->m_tokens[i]->m_reductionRule >= 0) 
    {
      // call the rule's subroutine via the RuleJumpTable
      executeRule(token->m_tokens[i]);
    } 
  }
}

//----------------------------------------------------------------------------------------------------------------------
// print a (hopefully) useful error based on a parser failure
//
void ParserBase::printParserError(Token *token, const char* file, ParseResult result)
{
  unsigned int i;
  int symbol;

  m_stringBuilder->reset();

  switch(result) 
  {
    case prEmptyFile:
      m_stringBuilder->appendCharBuf("File is empty ");
      break;
    case prLexError:
      m_stringBuilder->appendCharBuf("Lexical error: ");
      break;
    case prCommentError:
      m_stringBuilder->appendCharBuf("Comment error: ");
      break;
    case prTokenError:
      m_stringBuilder->appendCharBuf("Tokenizer error: ");
      break;
    case prSyntaxError:
      m_stringBuilder->appendCharBuf("Syntax error: ");
      break;
  }

  if (result == prLexError) 
  {
    if (token->m_data != NULL) 
    {
      m_stringBuilder->appendf("The grammar does not specify what to do with '%s'.\n", token->m_data);
    } 
    else 
    {
      m_stringBuilder->appendCharBuf("The grammar does not specify what to do.\n");
    }
  }  
  else if (result == prTokenError) 
  {
    m_stringBuilder->appendCharBuf("The tokenizer returned a non-terminal.\n");
  }
  if (result == prCommentError) 
  {
    m_stringBuilder->appendCharBuf("The comment has no end, it was started but not finished.\n");
  }
  if (result == prSyntaxError) 
  {
    if (token->m_data != NULL) 
    {
      m_stringBuilder->appendf("Encountered '%s', but expected ", token->m_data);
    }
    else 
    {
      m_stringBuilder->appendCharBuf("Expected ");
    }

    for (i=0; i<m_grammar.m_LALRArray[token->m_symbol].m_actionCount; i++) 
    {
      symbol = m_grammar.m_LALRArray[token->m_symbol].m_actions[i].m_entry;
      if (m_grammar.m_symbolArray[symbol].m_kind == skTerminal) 
      {
        if (i > 0) 
        {
          m_stringBuilder->appendCharBuf(", ");
          if (i >= m_grammar.m_LALRArray[token->m_symbol].m_actionCount - 2) 
          {
            m_stringBuilder->appendCharBuf("or ");
          }
        }
        m_stringBuilder->appendf("'%s'", m_grammar.m_symbolArray[symbol].m_name);
      }
    }

    m_stringBuilder->appendCharBuf("\n");
  }

  // report the generated error string to the user
  m_feedbackInterface->onProcessingFailure(
    MDFFeedbackInterface::Parser,
    file,
    (token==NULL)?0:token->m_line,
    m_stringBuilder->c_str(),
    0,
    m_context->m_clientUserData);
}

//----------------------------------------------------------------------------------------------------------------------
const char* ParserBase::functionDeclarationConcat(const Grammar& grammar, Token *token, unsigned int startToken)
{
  m_trackingLastSeenSymbol = 0;
  m_stringBuilder->reset();

  recursiveFunctionDeclarationConcat(grammar, token, startToken);

  return m_stringBuilder->getBufferPtr();
}

//----------------------------------------------------------------------------------------------------------------------
int ArithmeticComputer::compute(StringBuilder& stringRepOut)
{
  assert(m_ops.count() == m_strings.count());
  unsigned int i = 0, count = m_ops.count();

  // get initial value
  int result = m_ops.get(i);

  stringRepOut.reset();
  stringRepOut.appendPString(m_strings.get(i));

  i ++;


  // change the result based on the subsequent operation pairs, eg
  // 2 + 3 - 4 
  // {2}, {+ 3}, {-4}
  //
  for (; i < count; i++)
  {
    int opc = m_ops.get(i);

    stringRepOut.appendChar(' ');
    stringRepOut.appendPString(m_strings.get(i));

    switch (opc)
    {
      case ArithmeticComputer::acOp_Add:
        {
          assert(i + 1 < count);
          result += m_ops.get(i + 1);
          i ++;
        }
        break;

      case ArithmeticComputer::acOp_Sub:
        {
          assert(i + 1 < count);
          result -= m_ops.get(i + 1);
          i ++;
        }
        break;

      case ArithmeticComputer::acOp_Mul:
        {
          assert(i + 1 < count);
          result *= m_ops.get(i + 1);
          i ++;
        }
        break;

      default:
        assert(0);
        break;
    }

    stringRepOut.appendf(" %s", m_strings.get(i));
  }

  return result;
}
