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
#include "ParserEngine.h"
#include "ParserMemory.h"
#include "StringBuilder.h"
#include "LibMDF.h"


//----------------------------------------------------------------------------------------------------------------------
inline void assignRootToken(TokenStack *tsTop, TokenStack *tsNew, Token **firstToken)
{
  if (tsTop != NULL) 
  {
    *firstToken = tsTop->m_token;
  }
  if (tsNew != NULL) 
  {
    *firstToken = tsNew->m_token;
  }
}

//----------------------------------------------------------------------------------------------------------------------
ParseResult ParserEngine::parse(Token **rootOutputToken, SavedComments& savedComments)
{
  // Comment processing.
  StringBuilder commentStorage(128);
  bool commentUnderConstruction = false; // If true there is a comment saved in commentStorage buffer waiting to be processed.
  unsigned int lastLineSaved = 0;        // Stores line number under which the last comment was saved in hash-table.
  bool isTrailingComment = false;        // Trailing comment i.e. containing "<" symbol just after "//" or "///".
                                         // Trailing comments are assumed to take priority over preceding comments.
                                         // If a variable has both types of comments, the preceding comment will be overwritten
                                         // by the trailing comment.
  bool isPrvTrailingComment = false;
  unsigned int trailingCommentLine = 0;  // Stores line number of the most recent trailing comment found.
  unsigned int lastValidTokenLine = 0;

  // Token processing.
  TokenStack *tokenStack = 0;
  TokenStack *work = 0;
  int LALRState = m_grammar->m_initialLalrState, commentLevel = 0;
  LALRResult lalrResult;

  *rootOutputToken = NULL;

  // bail on empty input
  if (m_pfc->isEmpty())
  {
    return(prEmptyFile);
  }

  // accept tokens until finished
  TokenStack *reuse = 0;
  for(;;) 
  {
    // if a token was pushed back as re-usable (eg. we just got whitespace)
    // then switch it in as a new token to work with; otherwise allocate.
    if (reuse)
    {
      work = reuse;
      reuse = 0;
    }
    else
    {
      assert(ActiveMemoryManager::get());
      work = ActiveMemoryManager::get()->alloc<TokenStack>();
      work->m_token = ActiveMemoryManager::get()->alloc<Token>();
    }

    work->m_LALRState = LALRState;

    work->m_token->m_reductionRule = -1;
    work->m_token->m_line = m_pfc->getLine();
    work->m_token->m_column = m_pfc->getColumn();
    work->m_token->m_pos = m_pfc->getOffset();

    // parse a token from the input stream
    work->m_token->m_data = scanForToken(&work->m_token->m_symbol, work->m_token->m_length);
    const Symbol& tokenSymbol = m_grammar->m_symbolArray[work->m_token->m_symbol];

    /* If we are inside a comment then ignore everything except the end
       of the comment, or the start of a nested comment. */
    if (commentLevel > 0)
    {
      if (m_shInterface)
      {
        m_shInterface->apply(work->m_token->m_line - 1, work->m_token->m_column - 1, work->m_token->m_length, MDFSyntaxHighlightingInterface::sComment);
      }

      // beginning a nested comment block
      if (tokenSymbol.m_kind == skCommentStart) 
      {
        // Push the Token on the TokenStack to keep track of line+column.
        work->m_nextToken = tokenStack;
        tokenStack = work;

        commentLevel ++;
        continue;
      }

      // end of comment block
      if (tokenSymbol.m_kind == skCommentEnd) 
      {
        // Pop the comment-start Token from the TokenStack and delete that as well
        work = tokenStack;
        tokenStack = work->m_nextToken;

        reuse = work;

        -- commentLevel;
        continue;
      }

      // we hit the end of file mid-comment; bail out
      if (tokenSymbol.m_kind == skEOF) 
      {
        assignRootToken(tokenStack, NULL, rootOutputToken);
        return prCommentError;
      }

      reuse = work;
      continue;
    }

    /* If the token is the start of a comment then increment the
       CommentLevel and loop. The routine will keep reading tokens
       until the end of the comment. */
    if (tokenSymbol.m_kind == skCommentStart)
    {
      if (m_shInterface)
      {
        m_shInterface->apply(work->m_token->m_line - 1, work->m_token->m_column - 1, work->m_token->m_length, MDFSyntaxHighlightingInterface::sComment);
      }

      // push the Token on the TokenStack to keep track of line+column.
      work->m_nextToken = tokenStack;
      tokenStack = work;

      commentLevel ++;
      continue;
    }

    // Comments.
    // Code inside if block is a comment accumulator that walks past symbols like '/', '<' or ' ',
    // consuming comment to the end of the line, saving it to the temporary commentStorage and continues.
    if (tokenSymbol.m_kind == skCommentLine)
    {
      unsigned int markLine = m_pfc->getLine();
      unsigned int markColumn = m_pfc->getColumn() - work->m_token->m_length;
      unsigned int markOffset = m_pfc->getOffset();

      isTrailingComment = false;
      reuse = work;

      if (!commentUnderConstruction)
      {
        commentStorage.reset();
        commentUnderConstruction = true;
      }

      // Append comment tag to comment storage.
      for (unsigned int i = 0; i < work->m_token->m_length; ++i)
      {
        char c = work->m_token->m_data[i];
        if (c != '#') // avoid #if 0
        {
          commentStorage.appendChar(c);
        }
      }

      if ((m_pfc->getOffset() < m_pfc->getDataSize()) &&
        (m_pfc->get() == '/'))
      {
        commentStorage.appendChar(m_pfc->get());
        m_pfc->incOffset();
      }

      // Is it a trailing comment?
      if ((m_pfc->getOffset() < m_pfc->getDataSize()) &&
        (m_pfc->get() == '<'))
      {
        commentStorage.appendChar(m_pfc->get());
        m_pfc->incOffset();
        isTrailingComment = true;
        trailingCommentLine = m_pfc->getLine();
      }

      // Consume to the end of the line.
      while ((m_pfc->getOffset() < m_pfc->getDataSize()) &&
        (m_pfc->get() != '\r') &&
        (m_pfc->get() != '\n'))
      {
        // Store the consumed character to the storage buffer.
        commentStorage.appendChar(m_pfc->get());
        m_pfc->incOffset();
      }

      if ((m_pfc->getOffset() < m_pfc->getDataSize()) &&
        (m_pfc->get() == '\r'))
      {
        m_pfc->incOffset();
      }

      if ((m_pfc->getOffset() < m_pfc->getDataSize()) &&
        (m_pfc->get() == '\n'))
      {
        if (!isTrailingComment)
        {
          commentStorage.appendChar(m_pfc->get()); // Add end of the line.
        }
        m_pfc->incOffset();
      }

      if (m_shInterface)
      {
        m_shInterface->apply(markLine - 1, markColumn - 1, (m_pfc->getOffset() - markOffset) + 1, MDFSyntaxHighlightingInterface::sComment);
      }

      m_pfc->incLine();

      continue;
    }
    else // Other (non skCommentLine) kind of token.
    //
    // This is the comment manager that decides whether or not to store accumulated temporary comment
    // inside the savedComments hash table.
    // Manager also overwrites preceding comment with the trailing comment inside savedComments hash table
    // if both of these types of comments exist.
    {
      if (commentUnderConstruction)
      {
        if (tokenSymbol.m_kind == skWhitespace)
        {
          if (isTrailingComment)
          {
            // Was there a preceding comment?
            if (trailingCommentLine == lastLineSaved && !isPrvTrailingComment)
            {
              // Overwrite preceding comment with trailing comment.
              if (commentStorage.getLength())
              {
                const char* comment = commentStorage.c_str();
                int commentLength = commentStorage.getLength();
                char* scData = (char*)ActiveMemoryManager::get()->alloc(commentLength + 1);
                NMP_STRNCPY_S(scData, commentLength + 1, comment);
                savedComments.erase(lastLineSaved);
                savedComments.insert(lastLineSaved, scData);
              }

              commentUnderConstruction = false;
              isPrvTrailingComment = true;
              commentStorage.reset();

              // Note that we are in branch of logic where token symbol kind is skWhitespace. This kind can be ignored.
              reuse = work;
              continue;
            }
            else // Save accumulated comments.
            {
              if (commentStorage.getLength())
              {
                const char* comment = commentStorage.c_str();
                int commentLength = commentStorage.getLength();
                char* scData = (char*)ActiveMemoryManager::get()->alloc(commentLength + 1);
                NMP_STRNCPY_S(scData, commentLength + 1, comment);
                lastLineSaved = lastValidTokenLine;
                savedComments.insert(lastLineSaved, scData);
              }

              commentUnderConstruction = false;
              isPrvTrailingComment = true;
              commentStorage.reset();

              // Note that we are in branch of logic where token symbol kind is skWhitespace. This kind can be ignored.
              reuse = work;
              continue;
            }
          }
          else // No trailing comment.
          {
            isPrvTrailingComment = false;
            if (m_pfc->getLine() == work->m_token->m_line)
            {
              // This must be " ", ignore then.
              reuse = work;
              continue;
            }
            else // This must be \r i.e. there is an empty line below the comment, drop the comment in this case.
            {
              commentUnderConstruction = false;
              commentStorage.reset();
              reuse = work;
              continue;
            }
          }
        }
        else // We have a valid token and an accumulated comment to save.
        {
          if (commentStorage.getLength())
          {
            const char* comment = commentStorage.c_str();
            int commentLength = commentStorage.getLength();

            // Store the comment record.
            char* scData = (char*)ActiveMemoryManager::get()->alloc(commentLength + 1);
            NMP_STRNCPY_S(scData, commentLength + 1, comment);
            lastLineSaved = (isTrailingComment) ? lastValidTokenLine : m_pfc->getLine();
            savedComments.insert(lastLineSaved, scData);
          }

          isPrvTrailingComment = (isTrailingComment) ? true : false;
          commentUnderConstruction = false;
          commentStorage.reset();
          // Note that token is processed further.
        }
      }
      else // There is currently no comment under construction.
      {
        if (tokenSymbol.m_kind == skWhitespace)
        {
          if (m_pfc->getLine() > work->m_token->m_line)
          {
            lastLineSaved = 0;
          }
          reuse = work;
          continue;
        }
        // Otherwise process token.
      }
    } // End of comments handling.

    // if parse error then exit
    if (tokenSymbol.m_kind == skError) 
    {
      assignRootToken(tokenStack, work, rootOutputToken);
      return prLexError;
    }

    // ignore whitespace
    if (tokenSymbol.m_kind == skWhitespace) 
    {
      reuse = work;
      continue;
    }

    // the tokenizer should never return a non-terminal symbol
    if (tokenSymbol.m_kind == skNonTerminal) 
    {
      assignRootToken(tokenStack, work, rootOutputToken);
      return prTokenError;
    }

    // feed the Symbol to the LALR state machine. It can do several
    // things, such as wind back and iteratively call itself
    lalrResult = parseToken(&LALRState, &tokenStack, work);

    lastValidTokenLine = work->m_token->m_line;

    // if syntax error then exit
    if (lalrResult == lrSyntaxError) 
    {
      // return LALR state in the Token->m_symbol
      work->m_token->m_symbol = LALRState;
      assignRootToken(tokenStack, work, rootOutputToken);
      return prSyntaxError;
    }

    // successful exit from LALR, we're done
    if (lalrResult == lrAccept) 
    {
      assignRootToken(tokenStack, NULL, rootOutputToken);
      return prSuccess;
    }

    // push the token onto the TokenStack
    work->m_nextToken = tokenStack;
    tokenStack = work;
  }
}


//----------------------------------------------------------------------------------------------------------------------
// snip a length of text from the input file, copy it to the internal memory area
//
char *ParserEngine::copyString(unsigned int length)
{
  char *snippet = (char*)ActiveMemoryManager::get()->alloc(length + 1);
  unsigned int i;

  for (i=0; i<length; i++) 
  {
    // ensure we didn't run off the end of the stream
    if (m_pfc->getOffset() < m_pfc->getDataSize()) 
    {
      // snippet may flow across newlines; bump the line counter if so
      if (m_pfc->get() == '\r')
      {
        if ( (m_pfc->getOffset() + 1) < m_pfc->getDataSize() && (m_pfc->get(m_pfc->getOffset() + 1) != '\n')) 
        {
          m_pfc->incLine();
        }
      }
      else if (m_pfc->get() == '\n')
      {
        m_pfc->incLine();
      }
      else
      {
        m_pfc->incColumn();
      }


      // store this character
      snippet[i] = m_pfc->get();

      // next character in the stream
      m_pfc->incOffset();
    } 
  }

  // terminate; necessary given the allocation buffer
  // may not be cleared to zero on subsequent parse runs
  assert(i == length);
  snippet[i] = '\0';

  return snippet;
}


//----------------------------------------------------------------------------------------------------------------------
// Parse a symbol from the input stream.
// 
// Symbols are lexical entities of 1 or more characters. The parser
// uses the m_grammar->DfaArray[] compiled by GOLD, which is basically
// a tree of character sets. The parser takes characters from the
// input and descends the tree by looking for the character in the
// character set. At the last branch of the tree is an index into the
// m_grammar->m_symbolsArray[], also compiled by GOLD.
// Return the index into m_grammar->m_symbolsArray[], and a pointer to a
// string with characters copied from the input.
//
// Symbol 0 is 'SYMBOLEOF' (string is NULL)
// Symbol 1 is 'SYMBOLERROR' (string is 1 character).
//
char *ParserEngine::scanForToken(int *outSymbol, unsigned int &lengthOfReturnedData) 
{
  int DFAIndex, acceptIndex;
  unsigned int length, acceptLength, i;

  // paranoia check for empty input
  if (m_pfc->isEmpty()) 
  {
    lengthOfReturnedData = 0;
    *outSymbol = 0;
    return NULL;
  }

  // if there are no more characters in the input then return SYMBOLEOF and NULL
  if (m_pfc->getOffset() >= m_pfc->getDataSize()) 
  {
    lengthOfReturnedData = 0;
    *outSymbol = 0;
    return NULL;
  }

  // search DFA character sets until we find a suitable match for the input character
  DFAIndex = m_grammar->m_initialDFAState;
  length = 0;
  acceptLength = 0;
  acceptIndex = -1;

  while (m_pfc->getOffset() + length < m_pfc->getDataSize()) 
  {
    /* If this is a valid symbol-terminal then save it. We know the
       input matches the symbol, but there may be a longer symbol that
       matches so we have to keep scanning. */
    if (m_grammar->m_DFAArray[DFAIndex].m_acceptSymbol >= 0) 
    {
      acceptIndex = DFAIndex;
      acceptLength = length;
    }

    // walk through the edges and scan the character set of each edge for the current character
    const DFAState& dfaState = m_grammar->m_DFAArray[DFAIndex];
    int currentChar = (int)m_pfc->get(m_pfc->getOffset() + length);

    for (i=0; i<dfaState.m_edgeCount; i++) 
    {      
      const DFAEdge& edge = dfaState.m_edges[i];

      // early-out test for single character match
      if (edge.m_charCount == 1 && edge.m_characterSet[0] == currentChar)
        break;
      // check if character is in range defined by character set
      else if (currentChar >= edge.m_characterSet[0] && 
               currentChar <= edge.m_characterSet[edge.m_charCount - 1])
      {
        if (strchr(edge.m_characterSet, currentChar))
          break;
      }
    }

    // if not found then exit the loop
    if (i >= dfaState.m_edgeCount) 
      break;

    // jump to the TargetState, which points to another set of DFA edges describing the next character
    DFAIndex = dfaState.m_edges[i].m_targetState;

    // examine the next character in the stream
    length = length + 1;
  }

  // if the DFA is a terminal then return the Symbol, and Length characters from the input
  if (m_grammar->m_DFAArray[DFAIndex].m_acceptSymbol >= 0) 
  {
    *outSymbol = m_grammar->m_DFAArray[DFAIndex].m_acceptSymbol;
    lengthOfReturnedData = length;
    return copyString(length);
  }

  // if we found a shorter terminal before, then return that Symbol, and it's characters
  if (acceptIndex >= 0) 
  {
    *outSymbol = m_grammar->m_DFAArray[acceptIndex].m_acceptSymbol;
    lengthOfReturnedData = acceptLength;
    return copyString(acceptLength);
  }

  // fall-through return SYMBOLERROR and a string with 1 character from the input
  *outSymbol = 1;
  lengthOfReturnedData = 1;
  return copyString(1);
}


//----------------------------------------------------------------------------------------------------------------------
// LookAhead LR Parser (LALR) state machine
// Depending on the Token->m_symbol the machine will change it's state and perform actions, 
// such as reduce the TokenStack and iteratively call itself.
//
ParserEngine::LALRResult ParserEngine::parseToken(int *LALRState, TokenStack **tokenStack, TokenStack *inputToken)
{
  TokenStack *PopToken;
  TokenStack *Reduction;
  unsigned int action, rule, i, counter;

  const Action* LALRAction = 0;

  // find the token->m_symbol in the LALR table.
  action = 0;
  counter = m_grammar->m_LALRArray[*LALRState].m_actionCount;
  while (action < counter) 
  {
    if (m_grammar->m_LALRArray[*LALRState].m_actions[action].m_entry == inputToken->m_token->m_symbol) 
    {
      LALRAction = &m_grammar->m_LALRArray[*LALRState].m_actions[action];
      break;
    }
    action++;
  }

  // if not found then exit with lrSyntaxError, token is not allowed in this context.
  if (LALRAction == 0) 
  {
    return lrSyntaxError;
  }

  // ACCEPT: exit with success
  if (LALRAction->m_action == Action::ACCEPT) 
  {
    return lrAccept;
  }

  // SHIFT / GOTO: switch the LALR state and return
  if (LALRAction->m_action == Action::SHIFT || 
      LALRAction->m_action == Action::GOTO)
  {
    *LALRState = LALRAction->m_target;
    return lrShift;
  }


  /* ACTIONREDUCE:
     Create a new Reduction according to the Rule that is specified by the action.
     - Create a new Reduction in the ReductionArray.
     - Pop tokens from the TokenStack and add them to the Reduction.
     - Push a new token on the TokenStack for the Reduction.
     - Iterate.
     */
  rule = LALRAction->m_target;

  // Allocate and initialize memory for the Reduction
  Reduction = ActiveMemoryManager::get()->alloc<TokenStack>();
  Reduction->m_token = ActiveMemoryManager::get()->alloc<Token>();

  Reduction->m_token->m_reductionRule = rule;
  Reduction->m_token->m_tokens = ActiveMemoryManager::get()->allocArray<Token*>(m_grammar->m_ruleArray[rule].m_symbolsCount);

  Reduction->m_token->m_symbol = m_grammar->m_ruleArray[rule].m_head;
  Reduction->m_token->m_data = NULL;
  Reduction->m_token->m_line = inputToken->m_token->m_line;
  Reduction->m_token->m_column = inputToken->m_token->m_column;
  Reduction->m_LALRState = *LALRState;
  Reduction->m_nextToken = NULL;

  // Reduce tokens from the TokenStack by moving them to the Reduction.
  // The Lalr state will be rewound to the state it was for the first
  // symbol of the rule.
  for (i = m_grammar->m_ruleArray[rule].m_symbolsCount; i > 0; i--) 
  {
    PopToken = *tokenStack;
    *tokenStack = PopToken->m_nextToken;
    PopToken->m_nextToken = NULL;

    Reduction->m_token->m_tokens[i - 1] = PopToken->m_token;
    *LALRState = PopToken->m_LALRState;
    Reduction->m_LALRState = PopToken->m_LALRState;
    Reduction->m_token->m_line = PopToken->m_token->m_line;
    Reduction->m_token->m_column = PopToken->m_token->m_column;
  }

  // call the LALR state machine with the Symbol of the Rule
  parseToken(LALRState, tokenStack, Reduction);

  // push new Token on the TokenStack for the Reduction
  Reduction->m_nextToken = *tokenStack;
  *tokenStack = Reduction;

  // save the current LALR state in the InputToken. We need this to be
  // able to rewind the state when reducing.
  inputToken->m_LALRState = *LALRState;

  // Call the LALR state machine with the InputToken. The state has
  // changed because of the reduction, so we must accept the token
  // again.
  return parseToken(LALRState, tokenStack, inputToken);
}
