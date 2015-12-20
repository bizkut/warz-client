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

#include "ParserDefs.h"

/**
 * Wrapper around lexing, tokenizing code. Given a grammar and file context,
 * the ParserEngine creates a chain of output tokens and stored comment data from the input.
 */
class ParserEngine
{
public:

  inline ParserEngine(const Grammar* grammar, ParserContext* pfc, MDFSyntaxHighlightingInterface *shi) : 
    m_grammar(grammar), 
    m_pfc(pfc),
    m_shInterface(shi)
  {}  

  ParseResult parse(
    Token **token,
    SavedComments& savedComments);

protected:

  enum LALRResult
  {
    lrSyntaxError,
    lrAccept,
    lrShift,
    lrGoto
  };

  char *copyString(unsigned int length);
  char *scanForToken(int *outSymbol, unsigned int &lengthOfReturnedData);
  LALRResult parseToken(int *LALRState, TokenStack **tokenStack, TokenStack *inputToken);

  const Grammar                    *m_grammar;
  ParserContext                    *m_pfc;
  MDFSyntaxHighlightingInterface   *m_shInterface;  // if not null, use to mark comment blocks, etc

private:
  ParserEngine();
  ParserEngine(const ParserEngine& rhs);
};
