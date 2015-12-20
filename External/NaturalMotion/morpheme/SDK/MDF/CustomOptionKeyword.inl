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
#pragma once

//----------------------------------------------------------------------------------------------------------------------
/* <CustomOptionKeyword> ::= ModKeyword */
void Parser::Rule_CustomOptionKeyword_ModKeyword(Token *token)
{
  assert(getContext()->m_modifierOptionsStack);
  getContext()->m_modifierOptionsStack->keywords = getContext()->m_keywords;

  if (getContext()->m_modifierOptionsStack->stack.isFull())
  {
    throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "too many modifiers applied"));
  }

  SHLApply(0, sModifierAttribute);

  ModifierOption& modopt = getContext()->m_modifierOptionsStack->stack.push();

  modopt.m_keyword = token->m_tokens[0]->m_data;
  modopt.m_valueType = Keyword::vrNone;
}

//----------------------------------------------------------------------------------------------------------------------
/* <CustomOptionKeyword> ::= ModKeyword '(' IdentifierTerm ')' */
void Parser::Rule_CustomOptionKeyword_ModKeyword_LParan_IdentifierTerm_RParan(Token *token)
{
  assert(getContext()->m_modifierOptionsStack);
  getContext()->m_modifierOptionsStack->keywords = getContext()->m_keywords;

  if (getContext()->m_modifierOptionsStack->stack.isFull())
  {
    throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "too many modifiers applied"));
  }

  CheckTokenForReservedKeyword(2);
  SHLApply(0, sModifierAttribute);
  SHLApply(2, sIdentifier);

  ModifierOption& modopt = getContext()->m_modifierOptionsStack->stack.push();

  modopt.m_keyword = token->m_tokens[0]->m_data;
  modopt.m_value = token->m_tokens[2]->m_data;
  modopt.m_valueType = Keyword::vrIdentifier;
}

//----------------------------------------------------------------------------------------------------------------------
/* <CustomOptionKeyword> ::= ModKeyword '(' StringLiteral ')' */
void Parser::Rule_CustomOptionKeyword_ModKeyword_LParan_StringLiteral_RParan(Token *token)
{
  assert(getContext()->m_modifierOptionsStack);
  getContext()->m_modifierOptionsStack->keywords = getContext()->m_keywords;

  if (getContext()->m_modifierOptionsStack->stack.isFull())
  {
    throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "too many modifiers applied"));
  }

  SHLApply(0, sModifierAttribute);
  SHLApply(2, sString);

  ModifierOption& modopt = getContext()->m_modifierOptionsStack->stack.push();

  modopt.m_keyword = token->m_tokens[0]->m_data;
  modopt.m_value = token->m_tokens[2]->m_data;
  modopt.m_valueType = Keyword::vrString;
}

//----------------------------------------------------------------------------------------------------------------------
/* <CustomOptionKeyword> ::= ModKeyword '(' DecLiteral ')' */
void Parser::Rule_CustomOptionKeyword_ModKeyword_LParan_DecLiteral_RParan(Token *token)
{
  assert(getContext()->m_modifierOptionsStack);
  getContext()->m_modifierOptionsStack->keywords = getContext()->m_keywords;

  if (getContext()->m_modifierOptionsStack->stack.isFull())
  {
    throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "too many modifiers applied"));
  }

  SHLApply(0, sModifierAttribute);
  SHLApply(2, sNumeric);

  ModifierOption& modopt = getContext()->m_modifierOptionsStack->stack.push();

  modopt.m_keyword = token->m_tokens[0]->m_data;
  modopt.m_value = token->m_tokens[2]->m_data;
  modopt.m_valueType = Keyword::vrDecimal;
}

//----------------------------------------------------------------------------------------------------------------------
/* <CustomOptions> ::= <CustomOptionKeyword> */
void Parser::Rule_CustomOptions(Token *token)
{
  executeRule(token->m_tokens[0]);
}

//----------------------------------------------------------------------------------------------------------------------
/* <CustomOptions> ::= <CustomOptions> <CustomOptionKeyword> */
void Parser::Rule_CustomOptions2(Token *token)
{
  evaluateTokens(token, reRule_CustomOptions2);
}

//----------------------------------------------------------------------------------------------------------------------
/* <CustomOption> ::= <CustomOptions> */
void Parser::Rule_CustomOption(Token *token)
{
  executeRule(token->m_tokens[0]);
}

//----------------------------------------------------------------------------------------------------------------------
/* <CustomOption> ::=  */
void Parser::Rule_CustomOption2(Token *)
{
}
