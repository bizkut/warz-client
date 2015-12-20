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
#include "Utils.h"
#include "LibMDF.h"

namespace MDFTypes
{
  #include "CustomOptionKeyword.inl"

  /* <VarIdentifier> ::= IdStar IdentifierTerm */
  void Parser::Rule_VarIdentifier_IdStar_IdentifierTerm(Token *token)
  {
    getContext()->m_vardecl.m_storageSpecifier = token->m_tokens[0]->m_data[0];
    getContext()->m_vardecl.m_varname = token->m_tokens[1]->m_data;

    if (getContext()->m_keywords->isReservedKeyword(getContext()->m_vardecl.m_varname.c_str()))
    {
      throw(new RuleExecutionException(token, getContext()->m_vardecl.m_varname.c_str(), "invalid use of reserved keyword (C++ / MDF)"));
    }

    ParsedTypeDependency tyd(getContext()->m_vdc, true);
    getContext()->m_activeStruct->m_typeDependencies.push_back(tyd);

    SHLApply(1, sIdentifier);
  }

  /* <VarIdentifier> ::= IdRef IdentifierTerm */
  void Parser::Rule_VarIdentifier_IdRef_IdentifierTerm(Token *token)
  {
    getContext()->m_vardecl.m_storageSpecifier = token->m_tokens[0]->m_data[0];
    getContext()->m_vardecl.m_varname = token->m_tokens[1]->m_data;

    if (getContext()->m_keywords->isReservedKeyword(getContext()->m_vardecl.m_varname.c_str()))
    {
      throw(new RuleExecutionException(token, getContext()->m_vardecl.m_varname.c_str(), "invalid use of reserved keyword (C++ / MDF)"));
    }

    SHLApply(1, sIdentifier);

    ParsedTypeDependency tyd(getContext()->m_vdc, true);
    getContext()->m_activeStruct->m_typeDependencies.push_back(tyd);
  }

  /* <VarIdentifier> ::= IdentifierTerm */
  void Parser::Rule_VarIdentifier_IdentifierTerm(Token *token)
  {
    getContext()->m_vardecl.m_varname = token->m_tokens[0]->m_data;

    if (getContext()->m_keywords->isReservedKeyword(getContext()->m_vardecl.m_varname.c_str()))
    {
      throw(new RuleExecutionException(token, getContext()->m_vardecl.m_varname.c_str(), "invalid use of reserved keyword (C++ / MDF)"));
    }

    SHLApply(0, sIdentifier);

    ParsedTypeDependency tyd(getContext()->m_vdc, false);
    getContext()->m_activeStruct->m_typeDependencies.push_back(tyd);
  }

  /* <VarCascade> ::= IdentifierTerm '::' <VarCascade> */
  void Parser::Rule_VarCascade_IdentifierTerm_ColonColon(Token *token)
  {
    if (getContext()->m_vdc.m_typespec.isFull())
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "too many components in a single variable declaration"));
    }
    if (getContext()->m_vdc.m_typesealed)
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "multiple namespaces defined in a single variable declaration?"));
    }
    CheckTokenForReservedKeyword(0);
    
    SHLApply(0, sTypeName);

    // record presence of namespace components
    unsigned int tsCount = getContext()->m_vdc.m_typespec.count();
    if (getContext()->m_vdc.m_namespaceStart == -1)
      getContext()->m_vdc.m_namespaceStart = tsCount;
    getContext()->m_vdc.m_namespaceEnd = tsCount;

    getContext()->m_vdc.m_typespec.push(token->m_tokens[0]->m_data);

    executeRule(token->m_tokens[2]);
  }

  /* <VarCascade> ::= IdentifierTerm */
  void Parser::Rule_VarCascade_IdentifierTerm(Token *token)
  {
    if (getContext()->m_parsingFnDecls)
    {
      if (strcmp(token->m_tokens[0]->m_data, "const")==0)
      {
        SHLApply(0, sKeyword1);
        return;
      }
      if (strcmp(token->m_tokens[0]->m_data, "static")==0)
      {
        SHLApply(0, sKeyword1);
        return;
      }
    }

    if (getContext()->m_vdc.m_typespec.isFull())
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "too many components in a single variable declaration"));
    }
    if (getContext()->m_vdc.m_typesealed)
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "unknown keyword found after variable type declaration"));
    }
    CheckTokenForReservedKeyword(0);

    SHLApply(0, sTypeName);

    // record the type name and mark the decl as 'sealed' - so that we catch any further attempts at type definition or
    // extra namespace entries as errors early on
    getContext()->m_vdc.m_typespec.push(token->m_tokens[0]->m_data);
    getContext()->m_vdc.m_typesealed = true;
    getContext()->m_vdc.m_lineDefined = token->m_line;
  }

  /* <VarType> ::= <VarCascade> */
  void Parser::Rule_VarType(Token *token)
  {
    executeRule(token->m_tokens[0]);
  }

  /* <VarType> ::= <CustomOptionKeyword> */
  void Parser::Rule_VarType2(Token *token)
  {
    // this is a cosmetic enforcement, ensure __keywords__ come before anything else of interest
    if (getContext()->m_vdc.m_namespaceStart != -1 || 
        getContext()->m_vdc.m_typesealed)
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "modifier keywords should appear before type/namespace"));
    }

    getContext()->m_modifierOptionsStack = &getContext()->m_vdc.m_modifiers;
    executeRule(token->m_tokens[0]);

    // check modifiers are valid
    getContext()->m_keywords->validateVarDeclModifiers(token, getContext()->m_vdc.m_modifiers);
  }

  /* <VarTypeList> ::= <VarType> */
  void Parser::Rule_VarTypeList(Token *token)
  {
    evaluateTokens(token, reRule_VarTypeList);
  }

  /* <VarTypeList> ::= <VarTypeList> <VarType> */
  void Parser::Rule_VarTypeList2(Token *token)
  {
    evaluateTokens(token, reRule_VarTypeList2);
  }

  /* <VarSpec> ::= <VarIdentifier> */
  void Parser::Rule_VarSpec(Token *token)
  {
    // assemble var decl data
    executeRule(token->m_tokens[0]);

    // add to the list of variables in the active accessability block
    getContext()->m_vardecl.m_arraySize = 0;
    getContext()->m_activeStruct->m_vardecl[getContext()->m_activeAccess].push_back(getContext()->m_vardecl);
  }

  /* <VarSpec> ::= <VarIdentifier> ArraySpecifier */
  void Parser::Rule_VarSpec_ArraySpecifier(Token *token)
  {
    // assemble var decl data
    executeRule(token->m_tokens[0]);

    // catch invalid array indices early
    int arraySpec = atoi(&token->m_tokens[1]->m_data[1]);
    if (arraySpec < 0 || arraySpec > VarDeclBase::vdMaxArraySize)
    {
      throw(new RuleExecutionException(token, token->m_tokens[1]->m_data, "array index out of range (negative / too large)"));
    }
    if (arraySpec == 0)
    {
      throw(new RuleExecutionException(token, token->m_tokens[1]->m_data, "array cannot be zero elements long"));
    }

    SHLApply(1, sNumeric);

    // add to the list of variables in the active accessibility block
    getContext()->m_vardecl.m_arraySize = arraySpec;
    getContext()->m_activeStruct->m_vardecl[getContext()->m_activeAccess].push_back(getContext()->m_vardecl);
  }


  // search both the local enums and then the global constants table for the given identifier string, returning
  // the resulting evaluated integer as well as the qualified enum string in sb
  int lookupIdentifierForArraySize(MDFTypes::Context* context, Token *token, int index, StringBuilder& sb)
  {
    int arraySizeVar;
    if (!context->m_constants->find(token->m_tokens[index]->m_data, &arraySizeVar))
    {
      const EnumDeclBase::Entry* ent = context->m_activeStruct->findEnumEntry(token->m_tokens[index]->m_data);
      if (!ent)
      {
        throw(new RuleExecutionException(token, token->m_tokens[index]->m_data, "unknown value used to denote array size"));
      }
      if (ent->m_value <= 0)
      {
        throw(new RuleExecutionException(token, token->m_tokens[index]->m_data, "enum value used as array size cannot be <= 0"));
      }
      if (ent->m_value > VarDeclBase::vdMaxArraySize)
      {
        throw(new RuleExecutionException(token, token->m_tokens[index]->m_data, "enum value used as array size is too large"));
      }

      arraySizeVar = ent->m_value;

      sb.reset();
      sb.appendf("%s::%s", context->m_activeStruct->m_name.c_str(), token->m_tokens[index]->m_data);
    }
    else
    {
      if (arraySizeVar <= 0)
      {
        throw(new RuleExecutionException(token, token->m_tokens[index]->m_data, "constant value used as array size cannot be <= 0"));
      }
      if (arraySizeVar > VarDeclBase::vdMaxArraySize)
      {
        throw(new RuleExecutionException(token, token->m_tokens[index]->m_data, "constant value used as array size is too large"));
      }

      sb.reset();
      sb.appendf("NetworkConstants::%s", token->m_tokens[index]->m_data);
    }

    return arraySizeVar;
  }


  /* <VarSpec> ::= <VarIdentifier> '[' IdentifierTerm ']' */
  void Parser::Rule_VarSpec_LBracket_IdentifierTerm_RBracket(Token *token)
  {
    // assemble var decl data
    executeRule(token->m_tokens[0]);

    int arraySizeVar = lookupIdentifierForArraySize(getContext(), token, 2, *m_stringBuilder);

    SHLApply(2, sEnumEntry);

    // add to the list of variables in the active accessibility block
    getContext()->m_vardecl.m_arraySizeAsString = m_stringBuilder->c_str();
    getContext()->m_vardecl.m_arraySize = (unsigned int)arraySizeVar;

    getContext()->m_activeStruct->m_vardecl[getContext()->m_activeAccess].push_back(getContext()->m_vardecl);
  }

  /* <VarSpecs> ::= <VarSpec> */
  void Parser::Rule_VarSpecs(Token *token)
  {
    evaluateTokens(token, reRule_VarSpecs);
  }

  /* <VarSpecs> ::= <VarSpecs> ',' <VarSpec> */
  void Parser::Rule_VarSpecs_Comma(Token *token)
  {
    evaluateTokens(token, reRule_VarSpecs_Comma);
  }


  /* <EnumExprValue> ::= DecLiteral */
  void Parser::Rule_EnumExprValue_DecLiteral(Token *token)
  {
    int literalValue = atoi(token->m_tokens[0]->m_data);
    if (literalValue < 0)
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "value in expression must be > 0"));
    }

    SHLApply(0, sNumeric);

    getContext()->m_arithComp.m_ops.push(literalValue);
    getContext()->m_arithComp.m_strings.push(token->m_tokens[0]->m_data);
  }

  /* <EnumExprValue> ::= IdentifierTerm */
  void Parser::Rule_EnumExprValue_IdentifierTerm(Token *token)
  {
    int identifierValue = lookupIdentifierForArraySize(getContext(), token, 0, *m_stringBuilder);
    if (identifierValue < 0)
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "value in expression must be > 0"));
    }

    SHLApply(0, sEnumEntry);

    getContext()->m_arithComp.m_noIdentifiers = false;
    getContext()->m_arithComp.m_ops.push(identifierValue);
    getContext()->m_arithComp.m_strings.push(m_stringBuilder->c_str());
  }

  /* <EnumExpr> ::= <EnumExpr> + <EnumExprValue> */
  void Parser::Rule_EnumExpr_Plus(Token *token)
  {
    executeRule(token->m_tokens[0]);
    getContext()->m_arithComp.m_ops.push(ArithmeticComputer::acOp_Add);
    getContext()->m_arithComp.m_strings.push(token->m_tokens[1]->m_data);
    executeRule(token->m_tokens[2]);
  }

  /* <EnumExpr> ::= <EnumExpr> - <EnumExprValue> */
  void Parser::Rule_EnumExpr_Minus(Token *token)
  {
    executeRule(token->m_tokens[0]);
    getContext()->m_arithComp.m_ops.push(ArithmeticComputer::acOp_Sub);
    getContext()->m_arithComp.m_strings.push(token->m_tokens[1]->m_data);
    executeRule(token->m_tokens[2]);
  }

  /* <EnumExpr> ::= <EnumExpr> IdStar <EnumExprValue> */
  void Parser::Rule_EnumExpr_IdStar(Token *token)
  {
    executeRule(token->m_tokens[0]);
    getContext()->m_arithComp.m_ops.push(ArithmeticComputer::acOp_Mul);
    getContext()->m_arithComp.m_strings.push(token->m_tokens[1]->m_data);
    executeRule(token->m_tokens[2]);
  }

  /* <EnumExpr> ::= <EnumExprValue> */
  void Parser::Rule_EnumExpr(Token *token)
  {
    executeRule(token->m_tokens[0]);
  }

  /* <Enum> ::= IdentifierTerm */
  void Parser::Rule_Enum_IdentifierTerm(Token *token)
  {
    if (getContext()->m_activeStruct->findEnumEntry(token->m_tokens[0]->m_data))
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "enum value already defined elsewhere"));
    }
    
    // check if this name is already used by the network constants
    int arraySizeVar;
    if (getContext()->m_constants->find(token->m_tokens[0]->m_data, &arraySizeVar))
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "enum value already defined as a global network constant"));
    }

    CheckTokenForReservedKeyword(0);
    SHLApply(0, sEnumEntry);

    getContext()->m_enumdecl->add(token->m_tokens[0]->m_data, token->m_tokens[0]->m_line);
  }

  /* <Enum> ::= IdentifierTerm = <EnumExpr> */
  void Parser::Rule_Enum_IdentifierTerm_Eq(Token *token)
  {
    if (getContext()->m_activeStruct->findEnumEntry(token->m_tokens[0]->m_data))
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "enum value already defined elsewhere"));
    }

    // check if this name is already used by the network constants
    int arraySizeVar;
    if (getContext()->m_constants->find(token->m_tokens[0]->m_data, &arraySizeVar))
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "enum value already defined as a global network constant"));
    }

    CheckTokenForReservedKeyword(0);
    SHLApply(0, sEnumEntry);

    // build up the expression using the computation helper
    getContext()->m_arithComp.clear();
    executeRule(token->m_tokens[2]);

    int computedValue = getContext()->m_arithComp.compute(*m_stringBuilder);

    // if all we gathered was a single literal, don't submit the expression string, as it will be unnecessary/verbose
    if (getContext()->m_arithComp.m_ops.count() == 1 && getContext()->m_arithComp.m_noIdentifiers)
      getContext()->m_enumdecl->add(token->m_tokens[0]->m_data, computedValue, token->m_tokens[0]->m_line);
    else
      getContext()->m_enumdecl->add(token->m_tokens[0]->m_data, computedValue, token->m_tokens[0]->m_line, m_stringBuilder->c_str());
  }

  /* <Enums> ::= <Enum> */
  void Parser::Rule_Enums(Token *token)
  {
    evaluateTokens(token, reRule_Enums);
  }

  /* <Enums> ::= <Enums> ',' <Enum> */
  void Parser::Rule_Enums_Comma(Token *token)
  {
    evaluateTokens(token, reRule_Enums_Comma);
  }

  /* <VarDecl> ::= <VarTypeList> <VarSpecs> ';' */
  void Parser::Rule_VarDecl_Semi(Token *token)
  {
    // fill in the VDC
    getContext()->m_vdc.clear();
    getContext()->m_vdc.m_lineDefined = token->m_line;
    executeRule(token->m_tokens[0]);

    // prep to collect names
    getContext()->m_vardecl.clear();
    getContext()->m_vardecl.m_lineDefined = token->m_line;
    getContext()->m_vardecl.m_ownerStruct = getContext()->m_activeStruct;
    getContext()->m_vardecl.m_accessability = getContext()->m_activeAccess;

    // transfer type data from VDC
    getContext()->m_vdc.getNamespaceFromIndices(*m_stringBuilder);
    getContext()->m_vardecl.m_namespace = m_stringBuilder->getBufferPtr();
    getContext()->m_vardecl.m_typename = getContext()->m_vdc.m_typespec.getLast();
    getContext()->m_vardecl.m_modifiers = getContext()->m_vdc.m_modifiers;

    // process 1 or more names, record each as a new decl
    executeRule(token->m_tokens[1]);
  }

  /* <VarDecl> ::= enum IdentifierTerm '{' <Enums> '};' */
  void Parser::Rule_VarDecl_enum_IdentifierTerm_LBrace_RBraceSemi(Token *token)
  {
    CheckTokenForReservedKeyword(1);

    SHLApply(0, sKeyword1);
    SHLApply(1, sTypeName);

    // add new decl
    Struct::EnumDecls& decls = getContext()->m_activeStruct->m_enumDecl[getContext()->m_activeAccess];
    decls.push_back(Struct::EnumDecl());
    getContext()->m_enumdecl = &decls.back();

    // setup basics
    getContext()->m_enumdecl->m_name.set(token->m_tokens[1]->m_data);
    getContext()->m_enumdecl->m_ownerStruct = getContext()->m_activeStruct;
    getContext()->m_enumdecl->m_accessibility = getContext()->m_activeAccess;
    getContext()->m_enumdecl->m_lineDefined = token->m_line;

    evaluateTokens(token, reRule_VarDecl_enum_IdentifierTerm_LBrace_RBraceSemi);
  }

  /* <FnIsConst> ::= IdentifierTerm */
  void Parser::Rule_FnIsConst_IdentifierTerm(Token *token)
  {
    SHLApply(0, sKeyword1);
  }

  /* <FnIsConst> ::=  */
  void Parser::Rule_FnIsConst(Token *)
  {
  }

  /* <FnNumLiteral> ::= FloatLiteral */
  void Parser::Rule_FnNumLiteral_FloatLiteral(Token *token)
  {
    SHLApply(0, sNumeric);
  }

  /* <FnNumLiteral> ::= DecLiteral */
  void Parser::Rule_FnNumLiteral_DecLiteral(Token *token)
  {
    SHLApply(0, sNumeric);
  }

  /* <FnLiteral> ::= <FnNumLiteral> */
  void Parser::Rule_FnLiteral(Token *token)
  {
    evaluateTokens(token, reRule_FnLiteral);
  }

  /* <FnLiteral> ::= IdentifierTerm */
  void Parser::Rule_FnLiteral_IdentifierTerm(Token *token)
  {
    CheckTokenForReservedKeyword(0);
    SHLApply(0, sIdentifier);
  }

  /* <FnLiterals> ::= <FnLiteral> */
  void Parser::Rule_FnLiterals(Token *token)
  {
    evaluateTokens(token, reRule_FnLiterals);
  }

  /* <FnLiterals> ::= <FnLiterals> ',' <FnLiteral> */
  void Parser::Rule_FnLiterals_Comma(Token *token)
  {
    evaluateTokens(token, reRule_FnLiterals_Comma);
  }

  /* <FnArg> ::= <VarTypeList> <VarIdentifier> */
  void Parser::Rule_FnArg(Token *token)
  {
    getContext()->m_vdc.clear();
    getContext()->m_vardecl.clear();
    executeRule(token->m_tokens[0]);
    executeRule(token->m_tokens[1]);
  }

  /* <FnArg> ::= <VarTypeList> <VarIdentifier> '=' <VarCascade> '(' <FnLiterals> ')' */
  void Parser::Rule_FnArg_Eq_LParan_RParan(Token *token)
  {
    getContext()->m_vdc.clear();
    getContext()->m_vardecl.clear();
    executeRule(token->m_tokens[0]);
    executeRule(token->m_tokens[1]);
  }

  /* <FnArg> ::= <VarTypeList> <VarIdentifier> '=' <FnNumLiteral> */
  void Parser::Rule_FnArg_Eq(Token *token)
  {
    getContext()->m_vdc.clear();
    getContext()->m_vardecl.clear();
    executeRule(token->m_tokens[0]);
    executeRule(token->m_tokens[1]);
  }

  /* <FnArgs> ::= <FnArg> */
  void Parser::Rule_FnArgs(Token *token)
  {
    evaluateTokens(token, reRule_FnArgs);
  }

  /* <FnArgs> ::= <FnArgs> ',' <FnArg> */
  void Parser::Rule_FnArgs_Comma(Token *token)
  {
    evaluateTokens(token, reRule_FnArgs_Comma);
  }

  /* <FnArgs> ::=  */
  void Parser::Rule_FnArgs2(Token *)
  {
  }

  /* <FnName> ::= IdStar IdentifierTerm */
  void Parser::Rule_FnName_IdStar_IdentifierTerm(Token *token)
  {
    CheckTokenForReservedKeyword(1);
    SHLApply(1, sFuncName);

    if (!getContext()->m_vdc.isEmpty())
    {
      // store dependency on function return value
      ParsedTypeDependency tyd(getContext()->m_vdc, true);
      getContext()->m_activeStruct->m_typeDependencies.push_back(tyd);
    }
  }

  /* <FnName> ::= IdRef IdentifierTerm */
  void Parser::Rule_FnName_IdRef_IdentifierTerm(Token *token)
  {
    CheckTokenForReservedKeyword(1);
    SHLApply(1, sFuncName);

    if (!getContext()->m_vdc.isEmpty())
    {
      // store dependency on function return value
      ParsedTypeDependency tyd(getContext()->m_vdc, true);
      getContext()->m_activeStruct->m_typeDependencies.push_back(tyd);
    }
  }

  /* <FnName> ::= IdentifierTerm */
  void Parser::Rule_FnName_IdentifierTerm(Token *token)
  {
    CheckTokenForReservedKeyword(0);
    SHLApply(0, sFuncName);

    if (!getContext()->m_vdc.isEmpty())
    {
      // store dependency on function return value
      ParsedTypeDependency tyd(getContext()->m_vdc, false);
      getContext()->m_activeStruct->m_typeDependencies.push_back(tyd);
    }
  }

  /* <FnNameAndReturn> ::= <VarTypeList> <FnName> */
  void Parser::Rule_FnNameAndReturn(Token *token)
  {
    getContext()->m_vdc.clear();
    evaluateTokens(token, reRule_FnNameAndReturn);
  }

  /* <FnNameAndReturn> ::= <FnName> */
  void Parser::Rule_FnNameAndReturn2(Token *token)
  {
    getContext()->m_vdc.clear();
    evaluateTokens(token, reRule_FnNameAndReturn2);
  }

  /* <FnDecl> ::= '+' <FnNameAndReturn> '(' <FnArgs> ')' <FnIsConst> ';' */
  void Parser::Rule_FnDecl_Plus_LParan_RParan_Semi(Token *token)
  {
    // mash all the tokens that make up the decl together
    const char* fnDecl = functionDeclarationConcat(m_grammar, token, 1);

    // evaluate tokens to store dependencies
    getContext()->m_parsingFnDecls = true;
    evaluateTokens(token, reRule_FnDecl_Plus_LParan_RParan_Semi);
    getContext()->m_parsingFnDecls = false;

    // store
    Struct::FnDecls& decls = getContext()->m_activeStruct->m_functionDecl[getContext()->m_activeAccess];
    decls.push_back(Struct::FnDecl());
    decls.back().m_fn.set(fnDecl);
    decls.back().m_lineDefined = token->m_line;
  }

  /* <DataBlockDef> ::= <VarDecl> */
  void Parser::Rule_DataBlockDef(Token *token)
  {
    evaluateTokens(token, reRule_DataBlockDef);
  }

  /* <DataBlockDef> ::= <FnDecl> */
  void Parser::Rule_DataBlockDef2(Token *token)
  {
    evaluateTokens(token, reRule_DataBlockDef2);
  }

  /* <DataBlockDef> ::= 'private:' */
  void Parser::Rule_DataBlockDef_privateColon(Token *token)
  {
    getContext()->m_activeAccess = Struct::abPrivate;
    SHLApply(0, sKeyword1);
  }

  /* <DataBlockDef> ::= 'public:' */
  void Parser::Rule_DataBlockDef_publicColon(Token *token)
  {
    getContext()->m_activeAccess = Struct::abPublic;
    SHLApply(0, sKeyword1);
  }

  /* <DataBlockDefs> ::= <DataBlockDef> */
  void Parser::Rule_DataBlockDefs(Token *token)
  {
    evaluateTokens(token, reRule_DataBlockDefs);
  }

  /* <DataBlockDefs> ::= <DataBlockDefs> <DataBlockDef> */
  void Parser::Rule_DataBlockDefs2(Token *token)
  {
    evaluateTokens(token, reRule_DataBlockDefs2);
  }

  /* <StructInheritance> ::= ':' <VarCascade> */
  void Parser::Rule_StructInheritance_Colon(Token *token)
  {
    // execute to fill the active vardecl with type/namespace data
    getContext()->m_vdc.clear();
    executeRule(token->m_tokens[1]);

    // build namespace string, if start/end params set
    getContext()->m_vdc.getNamespaceFromIndices(*m_stringBuilder);
    getContext()->m_activeStruct->m_inheritanceNamespace = m_stringBuilder->getBufferPtr();

    // store name
    getContext()->m_activeStruct->m_inheritanceName.set(getContext()->m_vdc.m_typespec.getLast().c_str());
  }

  /* <StructInheritance> ::=  */
  void Parser::Rule_StructInheritance(Token *)
  {
     getContext()->m_activeStruct->m_inheritanceNamespace.clear();
     getContext()->m_activeStruct->m_inheritanceName.clear();
  }

  /* <Root> ::= namespace IdentifierTerm '{' <Roots> '}' */
  void Parser::Rule_Root_namespace_IdentifierTerm_LBrace_RBrace(Token *token)
  {
    CheckTokenForReservedKeyword(1);
    SHLApply(0, sKeyword1);
    SHLApply(1, sTypeName);

    getContext()->m_namespaceStack.push(token->m_tokens[1]->m_data);
    executeRule(token->m_tokens[3]);
    getContext()->m_namespaceStack.pop();
  }

  /* <Root> ::= <CustomOption> struct IdentifierTerm <StructInheritance> '{' <DataBlockDefs> '};' */
  void Parser::Rule_Root_struct_IdentifierTerm_LBrace_RBraceSemi(Token *token)
  {
    CheckTokenForReservedKeyword(2);
    SHLApply(1, sKeyword1);
    SHLApply(2, sTypeName);

    // reset access mode before defining new struct
    getContext()->m_activeAccess = Struct::abPublic;

    parserMemoryAllocDirect(Struct, getContext()->m_activeStruct);
    getContext()->m_activeStruct->m_lineDefined = token->m_line;
    getContext()->m_activeStruct->m_pathToDef = getContext()->m_sourceFilePath;
    getContext()->m_activeStruct->m_clientUserData = getContext()->m_clientUserData;
    getContext()->m_activeStruct->m_filetime.dwLowDateTime = getContext()->m_lowDateTime;
    getContext()->m_activeStruct->m_filetime.dwHighDateTime = getContext()->m_highDateTime;
    getContext()->m_activeStruct->m_name.set(token->m_tokens[2]->m_data);

    // build a namespace list 
    bool latterNamespace = false;
    m_stringBuilder->reset();
    for (unsigned int i=0; i<getContext()->m_namespaceStack.count(); i++)
    {
      const PString& str = getContext()->m_namespaceStack.get(i);

      if (latterNamespace)
        m_stringBuilder->appendCharBuf("::", 2);

      m_stringBuilder->appendPString(str);
      latterNamespace = true;
    }
    getContext()->m_activeStruct->m_namespaceStack = getContext()->m_namespaceStack;
    getContext()->m_activeStruct->m_namespace.set(m_stringBuilder->getBufferPtr());
    
    getContext()->m_modifierOptionsStack = &getContext()->m_activeStruct->m_modifiers;
    getContext()->m_modifierOptionsStack->stack.reset();

    // populate the struct
    evaluateTokens(token, reRule_Root_struct_IdentifierTerm_LBrace_RBraceSemi);

    // validate any modifiers found
    getContext()->m_keywords->validateStructModifiers(token, getContext()->m_activeStruct->m_modifiers);

    // register new struct
    TypeKey64 structKey = getContext()->m_activeStruct->m_name.hashValueA();
    structKey |= ((TypeKey64)getContext()->m_activeStruct->m_namespace.hashValueA()) << 32;

    // quick check to see if it has already been registered
    Struct *clashStruct;
    if (getContext()->m_structMap->find(structKey, &clashStruct))
    {
      throw(new RuleExecutionException(token, clashStruct->m_name.c_str(), "struct already registered"));
    }
    getContext()->m_structMap->insert(structKey, getContext()->m_activeStruct);

    SHLMarkBlock(token->m_tokens[1]->m_line, token->m_tokens[6]->m_line);
  }

  /* <Root> ::= <CustomOption> alias <VarCascade> '=>' IdentifierTerm <CustomOption> ';' */
  void Parser::Rule_Root_alias_EqGt_IdentifierTerm_Semi(Token *token)
  {
    CheckTokenForReservedKeyword(4);
    SHLApply(1, sKeyword1);
    SHLApply(4, sTypeName);

    getContext()->m_vdc.clear();
    getContext()->m_vardecl.clear();

    // evaluate variable def
    executeRule(token->m_tokens[2]);

    // formulate namespace
    getContext()->m_vdc.getNamespaceFromIndices(*m_stringBuilder);

    // lookup type to alias to
    TypesRegistry::Alias tAlias;
    tAlias.m_type = getContext()->m_typesRegistry->lookupByName(m_stringBuilder->getBufferPtr(), getContext()->m_vdc.m_typespec.getLast().c_str());
    if (!tAlias.m_type)
    {
      throw(new RuleExecutionException(token, token->m_tokens[4]->m_data, "cannot alias unknown type"));
    }
   
    getContext()->m_modifierOptionsStack = &tAlias.m_modifiers;
    getContext()->m_modifierOptionsStack->stack.reset();

    // gather both sets of type options, check they are valid
    executeRule(token->m_tokens[0]);
    executeRule(token->m_tokens[5]);
    getContext()->m_keywords->validateTypeModifiers(token, tAlias.m_modifiers);

    if (!getContext()->m_typesRegistry->addTypeAlias(token->m_tokens[4]->m_data, &tAlias))
    {
      throw(new RuleExecutionException(token, token->m_tokens[4]->m_data, "could not alias type, already aliased / duplicate name?"));
    }
  }

  /* <Root> ::= <CustomOption> typedef <VarCascade> TSizeSpecifier <CustomOption> ';' */
  void Parser::Rule_Root_typedef_TSizeSpecifier_Semi(Token *token)
  {
    SHLApply(1, sKeyword1);
    SHLApply(3, sNumeric);

    getContext()->m_vdc.clear();
    getContext()->m_vardecl.clear();

    // evaluate variable def
    executeRule(token->m_tokens[2]);

    // snag size-in-bytes
    int typeSizeInBytes = atoi(&token->m_tokens[3]->m_data[1]);

    // we have enough information to create the new type
    getContext()->m_vdc.getNamespaceFromIndices(*m_stringBuilder);
    parserMemoryAllocArgs(Type, t, 
      m_stringBuilder->getBufferPtr(), 
      getContext()->m_vdc.m_typespec.getLast().c_str(), 
      typeSizeInBytes);

    getContext()->m_modifierOptionsStack = &t->m_modifiers;
    getContext()->m_modifierOptionsStack->stack.reset();

    // gather both sets of modifier options, check they are valid
    executeRule(token->m_tokens[0]);
    executeRule(token->m_tokens[4]);
    getContext()->m_keywords->validateTypeModifiers(token, t->m_modifiers);

    // check to see if the alignment size requested was larger than the type itself;
    // if so, we assume the type will be stored and padded to the alignment boundary by 
    // the compiler, so set the type size appropriately
    //
    // eg. typedef MyType {64} __align__(128);
    //
    const ModifierOption* alignMo = hasTypeModifier(t->m_modifiers, tmAlign);
    if (alignMo)
    {
      unsigned int alignmentBoundary = (unsigned int)atoi(alignMo->m_value.c_str());
      if (alignmentBoundary > t->m_sizeInBytes)
        t->m_sizeInBytes = alignmentBoundary;
    }
    
    // store file information
    t->m_pathToDef = getContext()->m_sourceFilePath;
    t->m_clientUserData = getContext()->m_clientUserData;
    t->m_filetime.dwLowDateTime = getContext()->m_lowDateTime;
    t->m_filetime.dwHighDateTime = getContext()->m_highDateTime;

    getContext()->m_vdc.createNamespaceStack(t->m_namespaceStack);

    // all done, register it
    if (!getContext()->m_typesRegistry->registerType(t))
    {
      throw(new RuleExecutionException(token, m_stringBuilder->getBufferPtr(), "type already defined elsewhere"));
    }
  }

  /* <Roots> ::= <Root> */
  void Parser::Rule_Roots(Token *token)
  {
    evaluateTokens(token, reRule_Roots);
  }

  /* <Roots> ::= <Roots> <Root> */
  void Parser::Rule_Roots2(Token *token)
  {
    evaluateTokens(token, reRule_Roots2);
  }

  /* <TypesRoots> ::= <Roots> */
  void Parser::Rule_TypesRoots(Token *token)
  {
    evaluateTokens(token, reRule_TypesRoots);
  }

  void Parser::processSavedComments(const SavedComments &savedComments)
  {
    const char* scData;

    // Walk through all structs inside the types file.
    StructMap::value_walker structs = getContext()->m_structMap->walker();

    while (structs.next())
    {
      // Quick check to see if the comments have already been processed for this structure.
      TypeKey64 structKey = structs.key();
      Struct* clashStruct;
      if (getContext()->m_commentMap->find(structKey, &clashStruct))
      {
        // Comments for this structure have been already processed.
        continue;
      }
      else
      {
        Struct* s = structs();

        // Save information that structure under this key is being processed.
        getContext()->m_commentMap->insert(structKey, s);

        // Does the struct have preceding comment?
        if (savedComments.find(s->m_lineDefined, &scData))
        {
          s->m_shortDesc.set(stripWhiteSpace(scData));
        }

        // Inside struct, traverse public variables and private next.
        for (int n = 0; n < s->abNumAccessabilityBlocks; n++)
        {
          iterate_begin(Struct::VarDecl, s->m_vardecl[n], vd)
          {
            if (savedComments.find(vd.m_lineDefined, &scData))
            {
              vd.m_comment.set(stripWhiteSpace(scData));
            }
          }
          iterate_end

          // Enumerations handling.
          iterate_begin(Struct::EnumDecl, s->m_enumDecl[n], ed)
          {
            // Enum main comment.
            if (savedComments.find(ed.m_lineDefined, &scData))
            {
              ed.m_comment.set(stripWhiteSpace(scData));
            }

            // Iterate enum entries to find any comments attached to them.
            EnumDeclBase::Entries::const_iterator eIt = ed.m_entries.begin();
            EnumDeclBase::Entries::const_iterator eEnd = ed.m_entries.end();

            for (; eIt != eEnd; ++eIt)
            {
              EnumDeclBase::Entry& enumEntry = (*eIt);
              if (savedComments.find(enumEntry.m_lineDefined, &scData))
              {
                enumEntry.m_comment.set(stripWhiteSpace(scData));
              }
            }
          }
          iterate_end

          // Function comments.
          iterate_begin(Struct::FnDecl, s->m_functionDecl[n], fd)
          {
            if (savedComments.find(fd.m_lineDefined, &scData))
            {
              fd.m_shortDesc.set(stripWhiteSpace(scData));
            }
          }
          iterate_end
        } // End of abNumAccessabilityBlocks for loop.
      }
    } // End of structs while loop.
  }
} // namespace 
