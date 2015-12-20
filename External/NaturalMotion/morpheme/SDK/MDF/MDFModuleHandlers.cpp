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

namespace MDFModule
{
  #include "CustomOptionKeyword.inl"

  /* <Identifier> ::= IdentifierTerm */
  void Parser::Rule_Identifier_IdentifierTerm(Token *token)
  {
    SHLApply(0, sTypeName);

    getContext()->m_stringTmp = token->m_tokens[0]->m_data;
  }

  /* <Identifier> ::= IdentifierTerm IdentifierSku */
  void Parser::Rule_Identifier_IdentifierTerm_IdentifierSku(Token *token)
  {
    SHLApply(0, sTypeName);
    SHLApply(1, sKeyword1);

    getContext()->m_stringTmp = token->m_tokens[0]->m_data;
    getContext()->m_sku        = &token->m_tokens[1]->m_data[1];  // strip leading '<'
    getContext()->m_sku.trimEnd(1);                               // strip trailing '>'
  }

  /* <VarIdentifier> ::= IdStar IdentifierTerm */
  void Parser::Rule_VarIdentifier_IdStar_IdentifierTerm(Token *token)
  {
    CheckTokenForReservedKeyword(1);
    SHLApply(1, sIdentifier);

    getContext()->m_vardecl.m_storageSpecifier = token->m_tokens[0]->m_data[0];

    getContext()->m_vardecl.m_varname = token->m_tokens[1]->m_data;

    ParsedTypeDependency tyd(getContext()->m_vdc, true);
    getContext()->m_def->m_parsedTypeDependencies.push_back(tyd);
  }

  /* <VarIdentifier> ::= IdRef IdentifierTerm */
  void Parser::Rule_VarIdentifier_IdRef_IdentifierTerm(Token *token)
  {
    CheckTokenForReservedKeyword(1);
    SHLApply(1, sIdentifier);

    getContext()->m_vardecl.m_storageSpecifier = token->m_tokens[0]->m_data[0];

    getContext()->m_vardecl.m_varname = token->m_tokens[1]->m_data;

    ParsedTypeDependency tyd(getContext()->m_vdc, true);
    getContext()->m_def->m_parsedTypeDependencies.push_back(tyd);
  }


  /* <VarIdentifier> ::= IdentifierTerm */
  void Parser::Rule_VarIdentifier_IdentifierTerm(Token *token)
  {
    SHLApply(0, sIdentifier);

    getContext()->m_vardecl.m_varname = token->m_tokens[0]->m_data;

    if (getContext()->m_keywords->isReservedKeyword(getContext()->m_vardecl.m_varname.c_str()))
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "invalid use of reserved keyword (C++ / MDF)"));
    }

    ParsedTypeDependency tyd(getContext()->m_vdc, false);
    getContext()->m_def->m_parsedTypeDependencies.push_back(tyd);
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

    evaluateTokens(token, reRule_VarCascade_IdentifierTerm_ColonColon);
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
    evaluateTokens(token, reRule_VarSpec);
  }

  /* <VarSpec> ::= <VarIdentifier> ArraySpecifier */
  void Parser::Rule_VarSpec_ArraySpecifier(Token *token)
  {
    executeRule(token->m_tokens[0]);

    // catch invalid array indices early
    int arraySpec = atoi(&token->m_tokens[1]->m_data[1]);
    if (arraySpec < 0 || arraySpec > VarDeclBase::vdMaxArraySize)
    {
      throw(new RuleExecutionException(token, token->m_tokens[1]->m_data, "array index out of range (negative / too large)"));
    }

    // array needs to be non-zero size
    if (arraySpec == 0)
    {
      throw(new RuleExecutionException(token, token->m_tokens[1]->m_data, "array cannot be zero elements long"));
    }
    SHLApply(1, sNumeric);

    getContext()->m_vardecl.m_arraySize = arraySpec;
  }

  // search both the local enums and then the global constants table for the given identifier string, returning
  // the resulting evaluated integer as well as the qualified enum string in sb
  int lookupIdentifierForArraySize(MDFModule::Context* context, Token *token, int index, StringBuilder& sb)
  {
    int arraySizeVar;
    if (!context->m_constants->find(token->m_tokens[index]->m_data, &arraySizeVar))
    {
      const EnumDeclBase::Entry* ent = searchAllEnumEntries(token->m_tokens[index]->m_data, context->m_def->m_enumDecl);
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
      sb.appendf("%sData::%s", context->m_def->m_name.c_str(), token->m_tokens[index]->m_data);
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

    getContext()->m_vardecl.m_arraySizeAsString = m_stringBuilder->c_str();
    getContext()->m_vardecl.m_arraySize = (unsigned int)arraySizeVar;
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
    // ensure the name is unique per module
    if (searchAllEnumEntries(token->m_tokens[0]->m_data, getContext()->m_def->m_enumDecl))
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "enum value already defined in this module"));
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
    // ensure the name is unique per module
    if (searchAllEnumEntries(token->m_tokens[0]->m_data, getContext()->m_def->m_enumDecl))
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "enum value already defined in this module"));
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

  /* <VarDecl> ::= <VarTypeList> <VarSpec> ';' */
  void Parser::Rule_VarDecl_Semi(Token *token)
  {
    // fill in the VDC
    getContext()->m_vdc.clear();
    executeRule(token->m_tokens[0]);

    // prep to collect names
    getContext()->m_vardecl.clear();
    getContext()->m_vardecl.m_lineDefined = token->m_line;
    getContext()->m_vardecl.m_ownerBlock = getContext()->m_activeBlock;

    // transfer type data from VDC
    getContext()->m_vdc.getNamespaceFromIndices(*m_stringBuilder);
    getContext()->m_vardecl.m_namespace = m_stringBuilder->getBufferPtr();
    getContext()->m_vardecl.m_typename = getContext()->m_vdc.m_typespec.getLast();
    getContext()->m_vardecl.m_modifiers = getContext()->m_vdc.m_modifiers;
    
    // process 1 or more names, record each as a new decl
    executeRule(token->m_tokens[1]);


    // buffer should now have a var decl string in it
    if (getContext()->m_activeBlock >= msNumVarDeclBlocks)
    {
      throw(new RuleExecutionException(token, 0, "variable declaration found outside of [data|feedIn|in|feedOut|out] block"));
    }

    getContext()->m_vardecl.m_modulePtr = getContext()->m_def;

    // is this variable a pointer/reference? 
    if (getContext()->m_vardecl.m_storageSpecifier != '\0')
    {
      if (!hasModuleModifier(getContext()->m_def->m_modifiers, mmGlobalAccess))
      {
        throw(new RuleExecutionException(token, getContext()->m_vardecl.m_varname.c_str(), "pointer/reference variable found in module"));
      }

      // for global access modules, we store ptr/refs in a private area so they don't appear in 
      // data packaging structures that are then pushed about during child update/feedback
      getContext()->m_def->m_private.push_back(getContext()->m_vardecl);
    }
    else
    {
      getContext()->m_def->m_vardecl[getContext()->m_activeBlock].push_back(getContext()->m_vardecl);
    }
  }

  /* <VarDecl> ::= enum IdentifierTerm '{' <Enums> '};' */
  void Parser::Rule_VarDecl_enum_IdentifierTerm_LBrace_RBraceSemi(Token *token)
  {
    // only allow enums in the data section, given that they are written out into 
    // the module's data packaging structure
    if (getContext()->m_activeBlock != msData)
    {
      throw(new RuleExecutionException(token, token->m_tokens[1]->m_data, "can only define enums in the module data section"));
    }

    CheckTokenForReservedKeyword(1);
    SHLApply(0, sKeyword1);
    SHLApply(1, sTypeName);

    // add new decl
    ModuleDef::EnumDecls& decls = getContext()->m_def->m_enumDecl;
    decls.push_back(ModuleDef::EnumDecl());
    getContext()->m_enumdecl = &decls.back();

    // setup basics
    getContext()->m_enumdecl->m_name.set(token->m_tokens[1]->m_data);
    getContext()->m_enumdecl->m_modulePtr = getContext()->m_def;
    getContext()->m_enumdecl->m_lineDefined = token->m_line;

    evaluateTokens(token, reRule_VarDecl_enum_IdentifierTerm_LBrace_RBraceSemi);
  }

  /* <DataBlockDefs> ::= <VarDecl> */
  void Parser::Rule_DataBlockDefs(Token *token)
  {
    evaluateTokens(token, reRule_DataBlockDefs);
  }

  /* <DataBlockDefs> ::= <DataBlockDefs> <VarDecl> */
  void Parser::Rule_DataBlockDefs2(Token *token)
  {
    evaluateTokens(token, reRule_DataBlockDefs2);
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
      getContext()->m_def->m_parsedTypeDependencies.push_back(tyd);
    }

    getContext()->m_lastSeenFnName = token->m_tokens[1]->m_data;
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
      getContext()->m_def->m_parsedTypeDependencies.push_back(tyd);
    }

    getContext()->m_lastSeenFnName = token->m_tokens[1]->m_data;
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
      getContext()->m_def->m_parsedTypeDependencies.push_back(tyd);
    }

    getContext()->m_lastSeenFnName = token->m_tokens[0]->m_data;
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

  /* <FnDecl> ::= <FnNameAndReturn> '(' <FnArgs> ')' <FnIsConst> ';' */
  void Parser::Rule_FnDecl_LParan_RParan_Semi(Token *token)
  {
    // mash all the tokens that make up the decl together
    const char* fnDecl = functionDeclarationConcat(m_grammar, token);

    getContext()->m_vdc.clear();
    getContext()->m_vardecl.clear();

    // evaluate tokens to store dependencies
    getContext()->m_parsingFnDecls = true;
    evaluateTokens(token, reRule_FnDecl_LParan_RParan_Semi);
    getContext()->m_parsingFnDecls = false;

    // store
    if (getContext()->m_activeBlock == msPublicAPI)
    {
      // ensure the public API funcs are marked as const, as they are designed to be read-only 
      // (all the data they have access to is const already, this is just to keep people in line)
      if (token->m_tokens[4]->m_reductionRule != reRule_FnIsConst_IdentifierTerm)
      {
        throw(new RuleExecutionException(token, getContext()->m_lastSeenFnName.c_str(), "public API function must be const"));
      }

      getContext()->m_def->m_publicAPI.push_back(ModuleDef::FnDecl());
      getContext()->m_def->m_publicAPI.back().m_fn.set(fnDecl);
      getContext()->m_def->m_publicAPI.back().m_lineDefined = token->m_line;
    }
    else if (getContext()->m_activeBlock == msPrivateAPI)
    {
      getContext()->m_def->m_privateAPI.push_back(ModuleDef::FnDecl());
      getContext()->m_def->m_privateAPI.back().m_fn.set(fnDecl);
      getContext()->m_def->m_privateAPI.back().m_lineDefined = token->m_line;
    }
    else
    {
      throw(new RuleExecutionException(token, getContext()->m_lastSeenFnName.c_str(), "function declared in non-function block?"));
    }
  }

  /* <FnDecls> ::= <FnDecl> */
  void Parser::Rule_FnDecls(Token *token)
  {
    evaluateTokens(token, reRule_FnDecls);
  }

  /* <FnDecls> ::= <FnDecls> <FnDecl> */
  void Parser::Rule_FnDecls2(Token *token)
  {
    evaluateTokens(token, reRule_FnDecls2);
  }


  /* <ChildDecl> ::= IdentifierTerm */
  void Parser::Rule_ChildDecl_IdentifierTerm(Token *token)
  {
    assert(getContext()->m_childDecl);

    CheckTokenForReservedKeyword(0);
    SHLApply(0, sIdentifier);

    getContext()->m_childDecl->m_name.set(token->m_tokens[0]->m_data);
  }

  /* <ChildDecl> ::= IdentifierTerm ArraySpecifier */
  void Parser::Rule_ChildDecl_IdentifierTerm_ArraySpecifier(Token *token)
  {
    assert(getContext()->m_childDecl);

    CheckTokenForReservedKeyword(0);
    SHLApply(0, sIdentifier);
    SHLApply(1, sNumeric);

    getContext()->m_childDecl->m_name.set(token->m_tokens[0]->m_data);

    // catch invalid array indices early
    int arraySpec = atoi(&token->m_tokens[1]->m_data[1]);
    if (arraySpec < 0 || arraySpec > ModuleDef::VarDecl::vdMaxArraySize)
    {
      throw(new RuleExecutionException(token, 0, "child array size out of range"));
    }

    getContext()->m_childDecl->m_arraySize = (unsigned int)arraySpec;
  }

  /* <ChildDecl> ::= IdentifierTerm '[' IdentifierTerm ']' */
  void Parser::Rule_ChildDecl_IdentifierTerm_LBracket_IdentifierTerm_RBracket(Token *token)
  {
    assert(getContext()->m_childDecl);

    CheckTokenForReservedKeyword(0);
    SHLApply(0, sIdentifier);
    SHLApply(2, sEnumEntry);

    getContext()->m_childDecl->m_name.set(token->m_tokens[0]->m_data);

    int arraySizeFromConstant;
    if (!getContext()->m_constants->find(token->m_tokens[2]->m_data, &arraySizeFromConstant))
    {
      throw(new RuleExecutionException(token, token->m_tokens[2]->m_data, "cannot find specified constant value"));
    }
    if (arraySizeFromConstant <= 0)
    {
      throw(new RuleExecutionException(token, token->m_tokens[2]->m_data, "constant value used as array size cannot be <= 0"));
    }
    if (arraySizeFromConstant > VarDeclBase::vdMaxArraySize)
    {
      throw(new RuleExecutionException(token, token->m_tokens[2]->m_data, "constant value used as array size is too large"));
    }

    getContext()->m_childDecl->m_arraySize = (unsigned int)arraySizeFromConstant;
  }

  /* <ChildDef> ::= <CustomOption> <Identifier> <ChildDecl> <CustomOption> ';' */
  void Parser::Rule_ChildDef_Semi(Token *token)
  {
    ModuleDef::Child newChildDef;

    getContext()->m_stringTmp.clear();
    getContext()->m_sku.clear();

    executeRule(token->m_tokens[1]);

    newChildDef.m_lineDefined = token->m_line;
    newChildDef.m_module.set(getContext()->m_stringTmp.c_str());
    newChildDef.m_sku.set(getContext()->m_sku.c_str());

    // execute name rule to gather both name / array params
    getContext()->m_childDecl = &newChildDef;
    executeRule(token->m_tokens[2]);
    getContext()->m_childDecl = 0;

    getContext()->m_def->m_children.push_back(newChildDef);
  }

  /* <ChildDefs> ::= <ChildDef> */
  void Parser::Rule_ChildDefs(Token *token)
  {
    evaluateTokens(token, reRule_ChildDefs);
  }

  /* <ChildDefs> ::= <ChildDefs> <ChildDef> */
  void Parser::Rule_ChildDefs2(Token *token)
  {
    evaluateTokens(token, reRule_ChildDefs2);
  }

  /* <OwnerBlock> ::= 'owner:' */
  void Parser::Rule_OwnerBlock_ownerColon(Token *token)
  {
    SHLApply(0, sKeyword2);
  }

  /* <OwnerBlock> ::= 'owner:' <Identifier> */
  void Parser::Rule_OwnerBlock_ownerColon2(Token *token)
  {
    SHLApply(0, sKeyword2);

    getContext()->m_activeBlock = msOwner;
    evaluateTokens(token, reRule_OwnerBlock_ownerColon2);

    getContext()->m_def->m_moduleOwner.set(getContext()->m_stringTmp.c_str());
    getContext()->m_def->m_moduleOwnerSku.set(getContext()->m_sku.c_str());
  }

  /* <DataBlock> ::= 'data:' */
  void Parser::Rule_DataBlock_dataColon(Token *token)
  {
    SHLApply(0, sKeyword2);
  }

  /* <DataBlock> ::= 'data:' <DataBlockDefs> */
  void Parser::Rule_DataBlock_dataColon2(Token *token)
  {
    SHLApply(0, sKeyword2);

    getContext()->m_activeBlock = msData;
    evaluateTokens(token, reRule_DataBlock_dataColon2);
  }

  /* <FeedInputsBlock> ::= 'feedIn:' */
  void Parser::Rule_FeedInputsBlock_feedInColon(Token *token)
  {
    SHLApply(0, sKeyword2);
  }

  /* <FeedInputsBlock> ::= 'feedIn:' <DataBlockDefs> */
  void Parser::Rule_FeedInputsBlock_feedInColon2(Token *token)
  {
    SHLApply(0, sKeyword2);

    getContext()->m_activeBlock = msFeedInputs;
    evaluateTokens(token, reRule_FeedInputsBlock_feedInColon2);
  }

  /* <InputsBlock> ::= 'in:' */
  void Parser::Rule_InputsBlock_inColon(Token *token)
  {
    SHLApply(0, sKeyword2);
  }

  /* <InputsBlock> ::= 'in:' <DataBlockDefs> */
  void Parser::Rule_InputsBlock_inColon2(Token *token)
  {
    SHLApply(0, sKeyword2);

    getContext()->m_activeBlock = msInputs;
    evaluateTokens(token, reRule_InputsBlock_inColon2);
  }

  /* <UserInputsBlock> ::= 'userIn:' */
  void Parser::Rule_UserInputsBlock_userInColon(Token *token)
  {
    SHLApply(0, sKeyword2);
  }

  /* <UserInputsBlock> ::= 'userIn:' <DataBlockDefs> */
  void Parser::Rule_UserInputsBlock_userInColon2(Token *token)
  {
    SHLApply(0, sKeyword2);

    getContext()->m_activeBlock = msUserInputs;
    evaluateTokens(token, reRule_UserInputsBlock_userInColon2);
  }

  /* <FeedOutputsBlock> ::= 'feedOut:' */
  void Parser::Rule_FeedOutputsBlock_feedOutColon(Token *token)
  {
    SHLApply(0, sKeyword2);
  }

  /* <FeedOutputsBlock> ::= 'feedOut:' <DataBlockDefs> */
  void Parser::Rule_FeedOutputsBlock_feedOutColon2(Token *token)
  {
    SHLApply(0, sKeyword2);

    getContext()->m_activeBlock = msFeedOutputs;
    evaluateTokens(token, reRule_FeedOutputsBlock_feedOutColon2);
  }

  /* <OutputsBlock> ::= 'out:' */
  void Parser::Rule_OutputsBlock_outColon(Token *token)
  {
    SHLApply(0, sKeyword2);
  }

  /* <OutputsBlock> ::= 'out:' <DataBlockDefs> */
  void Parser::Rule_OutputsBlock_outColon2(Token *token)
  {
    SHLApply(0, sKeyword2);

    getContext()->m_activeBlock = msOutputs;
    evaluateTokens(token, reRule_OutputsBlock_outColon2);
  }

  /* <PrivateFunctionsBlock> ::= 'privateAPI:' */
  void Parser::Rule_PrivateFunctionsBlock_privateAPIColon(Token *token)
  {
    SHLApply(0, sKeyword2);
  }

  /* <PrivateFunctionsBlock> ::= 'privateAPI:' <FnDecls> */
  void Parser::Rule_PrivateFunctionsBlock_privateAPIColon2(Token *token)
  {
    SHLApply(0, sKeyword2);

    getContext()->m_activeBlock = msPrivateAPI;
    evaluateTokens(token, reRule_PrivateFunctionsBlock_privateAPIColon2);
  }

  /* <PublicFunctionsBlock> ::= 'publicAPI:' */
  void Parser::Rule_PublicFunctionsBlock_publicAPIColon(Token *token)
  {
    SHLApply(0, sKeyword2);
  }

  /* <PublicFunctionsBlock> ::= 'publicAPI:' <FnDecls> */
  void Parser::Rule_PublicFunctionsBlock_publicAPIColon2(Token *token)
  {
    SHLApply(0, sKeyword2);

    getContext()->m_activeBlock = msPublicAPI;
    evaluateTokens(token, reRule_PublicFunctionsBlock_publicAPIColon2);
  }

  /* <ChildrenBlock> ::= 'children:' */
  void Parser::Rule_ChildrenBlock_childrenColon(Token *token)
  {
    SHLApply(0, sKeyword2);
  }

  /* <ChildrenBlock> ::= 'children:' <ChildDefs> */
  void Parser::Rule_ChildrenBlock_childrenColon2(Token *token)
  {
    SHLApply(0, sKeyword2);

    getContext()->m_activeBlock = msChildren;
    evaluateTokens(token, reRule_ChildrenBlock_childrenColon2);
  }

  /* <ArrayLinkPath> ::= '$' IdentifierTerm */
  void Parser::Rule_ArrayLinkPath_Dollar_IdentifierTerm(Token *token)
  {
    SHLApply(0, sEnumEntry);
    SHLApply(1, sEnumEntry);

    parserMemoryAllocArgs(ModuleDef::Connection::Path::ComponentInstanceVariable, comp, token->m_tokens[1]->m_data);
    comp->m_context = getContext()->m_keywords->toConnectionContext(comp->m_text);

    getContext()->m_connectionPath->m_components.push_back(comp);
  }

  /* <ArrayLinkPath> ::= IdentifierTerm '.' <ArrayLinkPath> */
  void Parser::Rule_ArrayLinkPath_IdentifierTerm_Dot(Token *token)
  {
    if (m_shInterface &&
        m_context->m_keywords->toConnectionContext(token->m_tokens[0]->m_data) != ccUnknown)
    {
      SHLApply(0, sKeyword1);
    }
    else
    {
      SHLApply(0, sTypeName);
    }

    parserMemoryAllocArgs(ModuleDef::Connection::Path::Component, comp, token->m_tokens[0]->m_data);
    comp->m_context = getContext()->m_keywords->toConnectionContext(comp->m_text);

    getContext()->m_connectionPath->m_components.push_back(comp);

    // continue down the path
    executeRule(token->m_tokens[2]);
  }

  /* <ConnectionLink> ::= IdentifierTerm */
  void Parser::Rule_ConnectionLink_IdentifierTerm(Token *token)
  {
    if (m_shInterface &&
        m_context->m_keywords->toConnectionContext(token->m_tokens[0]->m_data) != ccUnknown)
    {
      SHLApply(0, sKeyword1);
    }
    else
    {
      SHLApply(0, sTypeName);
    }

    parserMemoryAllocArgs(ModuleDef::Connection::Path::Component, comp, token->m_tokens[0]->m_data);
    comp->m_context = getContext()->m_keywords->toConnectionContext(comp->m_text);

    getContext()->m_connectionPath->m_components.push_back(comp);
  }

  /* <ConnectionLink> ::= IdentifierTerm ArraySpecifier */
  void Parser::Rule_ConnectionLink_IdentifierTerm_ArraySpecifier(Token *token)
  {
    if (m_shInterface &&
        m_context->m_keywords->toConnectionContext(token->m_tokens[0]->m_data) != ccUnknown)
    {
      SHLApply(0, sKeyword1);
    }
    else
    {
      SHLApply(0, sTypeName);
    }

    parserMemoryAllocArgs(ModuleDef::Connection::Path::ComponentArrayAccess, comp, token->m_tokens[0]->m_data);
    comp->m_context = getContext()->m_keywords->toConnectionContext(comp->m_text);

    // catch invalid array indices early
    int arraySpec = atoi(&token->m_tokens[1]->m_data[1]);
    if (arraySpec < 0 || arraySpec > ModuleDef::VarDecl::vdMaxArraySize)
    {
      throw(new RuleExecutionException(token, 0, "array index out of range"));
    }

    comp->m_arrayIndex = (unsigned int)arraySpec;

    getContext()->m_connectionPath->m_components.push_back(comp);
  }

  /* <ConnectionLink> ::= IdentifierTerm '[*]' */
  void Parser::Rule_ConnectionLink_IdentifierTerm_LBracketTimesRBracket(Token *token)
  {
    if (m_shInterface &&
        m_context->m_keywords->toConnectionContext(token->m_tokens[0]->m_data) != ccUnknown)
    {
      SHLApply(0, sKeyword1);
    }
    else
    {
      SHLApply(0, sTypeName);
    }

    parserMemoryAllocArgs(ModuleDef::Connection::Path::ComponentExpandArray, comp, token->m_tokens[0]->m_data);
    comp->m_context = getContext()->m_keywords->toConnectionContext(comp->m_text);

    getContext()->m_connectionPath->m_components.push_back(comp);
  }

  /* <ConnectionLink> ::= IdentifierTerm '[]' */
  void Parser::Rule_ConnectionLink_IdentifierTerm_LBracketRBracket(Token *token)
  {
    if (m_shInterface &&
        m_context->m_keywords->toConnectionContext(token->m_tokens[0]->m_data) != ccUnknown)
    {
      SHLApply(0, sKeyword1);
    }
    else
    {
      SHLApply(0, sTypeName);
    }
    SHLApply(1, sNumeric);

    parserMemoryAllocArgs(ModuleDef::Connection::Path::ComponentIterateArray, comp, token->m_tokens[0]->m_data);
    comp->m_context = getContext()->m_keywords->toConnectionContext(comp->m_text);

    getContext()->m_connectionPath->m_components.push_back(comp);
  }

  /* <ConnectionLink> ::= IdentifierTerm '[' <ArrayLinkPath> ']' */
  void Parser::Rule_ConnectionLink_IdentifierTerm_LBracket_RBracket(Token *token)
  {
    if (m_shInterface &&
        m_context->m_keywords->toConnectionContext(token->m_tokens[0]->m_data) != ccUnknown)
    {
      SHLApply(0, sKeyword1);
    }
    else
    {
      SHLApply(0, sTypeName);
    }

    parserMemoryAllocArgs(ModuleDef::Connection::Path::ComponentArrayAccessByLink, comp, token->m_tokens[0]->m_data);
    comp->m_context = getContext()->m_keywords->toConnectionContext(comp->m_text);

    // switch the path being built to be the one stored in the component
    ModuleDef::Connection::Path *savedPath = getContext()->m_connectionPath;

    parserMemoryAlloc(ModuleDef::Connection::Path, linkPath);
    comp->m_linkPath = linkPath;
    getContext()->m_connectionPath = linkPath;

    // execute to fill it
    executeRule(token->m_tokens[2]);

    // revert back to the main path being built, add our new component
    getContext()->m_connectionPath = savedPath;
    getContext()->m_connectionPath->m_components.push_back(comp);
  }


  /* <ConnectionWiring> ::= <ConnectionLink> */
  void Parser::Rule_ConnectionWiring(Token *token)
  {
    evaluateTokens(token, reRule_ConnectionWiring);
  }

  /* <ConnectionWiring> ::= <ConnectionLink> '.' <ConnectionWiring> */
  void Parser::Rule_ConnectionWiring_Dot(Token *token)
  {
    evaluateTokens(token, reRule_ConnectionWiring_Dot);
  }

  /* <ConnectionWiringStart> ::= <ConnectionLink> '.' <ConnectionWiring> */
  void Parser::Rule_ConnectionWiringStart_Dot(Token *token)
  {
    parserMemoryAlloc(ModuleDef::Connection::Path, conPath);

    // begin filling a new wiring structure
    getContext()->m_connectionPath = conPath;
    
    executeRule(token->m_tokens[0]);
    executeRule(token->m_tokens[2]);

    // create hash key of assigned components
    getContext()->m_connectionPath->generateHashKey();

    // if a connection is being built, add this path to it's list of paths
    // otherwise leave it to the calling rule to handle where to store the resulting data
    if (getContext()->m_connection)
      getContext()->m_connection->m_paths.push_back(conPath);
  }

  /* <ConnectionWiringList> ::= <ConnectionWiringStart> */
  void Parser::Rule_ConnectionWiringList(Token *token)
  {
    evaluateTokens(token, reRule_ConnectionWiringList);
  }

  /* <ConnectionWiringList> ::= <ConnectionWiringList> ',' <ConnectionWiringStart> */
  void Parser::Rule_ConnectionWiringList_Comma(Token *token)
  {
    evaluateTokens(token, reRule_ConnectionWiringList_Comma);
  }

  /* <ConnectionNewJnc> ::= IdentifierTerm '(' <ConnectionWiringList> ')' */
  void Parser::Rule_ConnectionNewJnc_IdentifierTerm_LParan_RParan(Token *token)
  {
    JunctionMode jm = getContext()->m_keywords->toJunctionMode(token->m_tokens[0]->m_data);
    if (jm == jmInvalid)
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "unknown junction type specified"));
    }
    SHLApply(0, sIdentifier);

    getContext()->m_connection->m_newJunctionMode = jm;
    executeRule(token->m_tokens[2]);
  }

  /* <ConnectionEntry> ::= <ConnectionWiringStart> '=' <ConnectionNewJnc> <CustomOption> ';' */
  void Parser::Rule_ConnectionEntry_Eq_Semi(Token *token)
  {
    // evaluate target path
    executeRule(token->m_tokens[0]);

    // create a new connection, set task to be 'create a new junction'
    parserMemoryAllocArgs(ModuleDef::Connection, cd, token->m_line, ctCreateJunction);

    getContext()->m_connectionSet->m_connectionDefs.push_back(cd);
    getContext()->m_connection = cd;
    getContext()->m_connection->m_target = getContext()->m_connectionPath;

    // recurse and fill in the details
    executeRule(token->m_tokens[2]);

    // gather modifier options
    getContext()->m_modifierOptionsStack = &cd->m_modifiers;
    getContext()->m_modifierOptionsStack->stack.reset();
    executeRule(token->m_tokens[3]);
    getContext()->m_keywords->validateConnectionTaskModifiers(token, *getContext()->m_modifierOptionsStack);

    getContext()->m_connection = 0;
  }

  /* <ConnectionEntry> ::= <ConnectionWiringStart> '=' <ConnectionWiringStart> <CustomOption> ';' */
  void Parser::Rule_ConnectionEntry_Eq_Semi2(Token *token)
  {
    // evaluate target path
    executeRule(token->m_tokens[0]);

    // create a new connection, set task to be 'assign junction output to member variable'
    parserMemoryAllocArgs(ModuleDef::Connection, cd, token->m_line, ctAssignJunction);

    getContext()->m_connectionSet->m_connectionDefs.push_back(cd);
    getContext()->m_connection = cd;
    getContext()->m_connection->m_target = getContext()->m_connectionPath;

    // recurse and fill in the details
    executeRule(token->m_tokens[2]);

    // gather modifier options
    getContext()->m_modifierOptionsStack = &cd->m_modifiers;
    getContext()->m_modifierOptionsStack->stack.reset();
    executeRule(token->m_tokens[3]);
    getContext()->m_keywords->validateConnectionTaskModifiers(token, *getContext()->m_modifierOptionsStack);

    getContext()->m_connection = 0;
  }

  /* <ConnectionEntry> ::= <ConnectionWiringStart> '=' <ConnectionWiringStart> '=' <ConnectionNewJnc> <CustomOption> ';' */
  void Parser::Rule_ConnectionEntry_Eq_Eq_Semi(Token *token)
  {
    // just split this shorthand into two tasks, identical to code above
    {
      // evaluate target path
      executeRule(token->m_tokens[2]);

      // create a new connection, set task to be 'create a new junction'
      parserMemoryAllocArgs(ModuleDef::Connection, cd, token->m_line, ctCreateJunction);

      getContext()->m_connectionSet->m_connectionDefs.push_back(cd);
      getContext()->m_connection = cd;
      getContext()->m_connection->m_target = getContext()->m_connectionPath;

      // recurse and fill in the details
      executeRule(token->m_tokens[4]);

      // gather modifier options
      getContext()->m_modifierOptionsStack = &cd->m_modifiers;
      getContext()->m_modifierOptionsStack->stack.reset();
      executeRule(token->m_tokens[5]);
      getContext()->m_keywords->validateConnectionTaskModifiers(token, *getContext()->m_modifierOptionsStack);

      getContext()->m_connection = 0;
    }
    {
      // evaluate target path
      executeRule(token->m_tokens[0]);

      // create a new connection, set task to be 'assign junction output to member variable'
      parserMemoryAllocArgs(ModuleDef::Connection, cd, token->m_line, ctAssignJunction);

      getContext()->m_connectionSet->m_connectionDefs.push_back(cd);
      getContext()->m_connection = cd;
      getContext()->m_connection->m_target = getContext()->m_connectionPath;

      // recurse and fill in the details
      executeRule(token->m_tokens[2]);

      // gather modifier options
      getContext()->m_modifierOptionsStack = &cd->m_modifiers;
      getContext()->m_modifierOptionsStack->stack.reset();
      executeRule(token->m_tokens[5]);
      getContext()->m_keywords->validateConnectionTaskModifiers(token, *getContext()->m_modifierOptionsStack);

      getContext()->m_connection = 0;
    }
  }

  /* <ConnectionEntry> ::= <ConnectionWiringStart> = ( <ConnectionWiringStart> ) <CustomOption> ; */
  void Parser::Rule_ConnectionEntry_Eq_LParan_RParan_Semi(Token *token)
  {
    // shorthand for creation of a directInput junction
    // allows for syntax:
    // "in.isUnderSupport = (owner.upperBody.out.isInSupport);"
    StringBuilder generatedJuncName(32);
    ModuleDef::Connection::Path *autoJuncPath;
    {
      // evaluate target path
      executeRule(token->m_tokens[0]);

      // assemble a junction name from the target variable names, eg "feedIn_armSwing"
      iterate_begin_ptr(ModuleDef::Connection::Path::Component, getContext()->m_connectionPath->m_components, jComp)
      {
        if (generatedJuncName.getLength() != 0)
          generatedJuncName.appendChar('_');

        generatedJuncName.appendPString(jComp->m_text);
      }
      iterate_end

      // create a junction path of type 'junctions.<concatinated target names>'
      parserMemoryAllocDirect(ModuleDef::Connection::Path, autoJuncPath);
      {
        parserMemoryAllocArgs(ModuleDef::Connection::Path::Component, comp, "junctions");
        comp->m_context = getContext()->m_keywords->toConnectionContext(comp->m_text);
        autoJuncPath->m_components.push_back(comp);
      }
      {
        parserMemoryAllocArgs(ModuleDef::Connection::Path::Component, comp, generatedJuncName.c_str());
        comp->m_context = getContext()->m_keywords->toConnectionContext(comp->m_text);
        autoJuncPath->m_components.push_back(comp);
      }
      autoJuncPath->generateHashKey();

      // create a new connection, set task to be 'assign junction output to member variable'
      parserMemoryAllocArgs(ModuleDef::Connection, cd, token->m_line, ctAssignJunction);

      getContext()->m_connectionSet->m_connectionDefs.push_back(cd);
      getContext()->m_connection = cd;
      getContext()->m_connection->m_target = getContext()->m_connectionPath;

      getContext()->m_connection->m_paths.push_back(autoJuncPath);

      // gather modifier options
      getContext()->m_modifierOptionsStack = &cd->m_modifiers;
      getContext()->m_modifierOptionsStack->stack.reset();
      executeRule(token->m_tokens[5]);
      getContext()->m_keywords->validateConnectionTaskModifiers(token, *getContext()->m_modifierOptionsStack);

      getContext()->m_connection = 0;
    }

    {
      // create a new connection, set task to be 'create a new junction'
      parserMemoryAllocArgs(ModuleDef::Connection, cd, token->m_line, ctCreateJunction);

      getContext()->m_connectionSet->m_connectionDefs.push_back(cd);
      getContext()->m_connection = cd;

      getContext()->m_connection->m_target = autoJuncPath->deepCopy();
      getContext()->m_connection->m_newJunctionMode = jmDirectInput;

      // recurse and fill in the details
      executeRule(token->m_tokens[3]);

      // gather modifier options
      getContext()->m_modifierOptionsStack = &cd->m_modifiers;
      getContext()->m_modifierOptionsStack->stack.reset();
      executeRule(token->m_tokens[5]);
      getContext()->m_keywords->validateConnectionTaskModifiers(token, *getContext()->m_modifierOptionsStack);

      getContext()->m_connection = 0;
    }
  }


  /* <ConnectionEntries> ::= <ConnectionEntry> */
  void Parser::Rule_ConnectionEntries(Token *token)
  {
    evaluateTokens(token, reRule_ConnectionEntries);
  }

  /* <ConnectionEntries> ::= <ConnectionEntries> <ConnectionEntry> */
  void Parser::Rule_ConnectionEntries2(Token *token)
  {
    evaluateTokens(token, reRule_ConnectionEntries2);
  }

  /* <ConnectionName> ::= IdentifierTerm */
  void Parser::Rule_ConnectionName_IdentifierTerm(Token *token)
  {
    SHLApply(0, sIdentifier);

    getContext()->m_connectionSet->m_name = token->m_tokens[0]->m_data;
  }

  /* <ConnectionName> ::=  */
  void Parser::Rule_ConnectionName(Token *)
  {
    getContext()->m_connectionSet->m_name.clear();
  }

  /* <Connection> ::= 'connections<' <ConnectionName> '>:' <CustomOption> <ConnectionEntries> */
  void Parser::Rule_Connection_connectionsLt_GtColon(Token *token)
  {
    SHLApply(0, sKeyword2);
    SHLApply(2, sKeyword2);

    // create a new connection set, store pointer to it for filling during evaluation
    parserMemoryAlloc(ModuleDef::ConnectionSet, ms);

    getContext()->m_def->m_connectionSets.push_back(ms);
    getContext()->m_connectionSet = ms;
    executeRule(token->m_tokens[1]);

    // gather modifier options
    getContext()->m_modifierOptionsStack = &getContext()->m_connectionSet->m_modifiers;
    getContext()->m_modifierOptionsStack->stack.reset();
    executeRule(token->m_tokens[3]);
    getContext()->m_keywords->validateConnectionSetModifiers(token, *getContext()->m_modifierOptionsStack);

    getContext()->m_activeBlock = msConnections;
    executeRule(token->m_tokens[4]);

    getContext()->m_connectionSet = 0;

    SHLMarkBlock(token->m_tokens[0]->m_line, m_trackingLastHitLine);
  }

  /* <Block> ::= <OwnerBlock> */
  void Parser::Rule_Block(Token *token)
  {
    evaluateTokens(token, reRule_Block);
  }

  /* <Block> ::= <DataBlock> */
  void Parser::Rule_Block2(Token *token)
  {
    evaluateTokens(token, reRule_Block2);
  }

  /* <Block> ::= <FeedInputsBlock> */
  void Parser::Rule_Block3(Token *token)
  {
    evaluateTokens(token, reRule_Block3);
  }

  /* <Block> ::= <InputsBlock> */
  void Parser::Rule_Block4(Token *token)
  {
    evaluateTokens(token, reRule_Block4);
  }

  /* <Block> ::= <UserInputsBlock> */
  void Parser::Rule_Block5(Token *token)
  {
    evaluateTokens(token, reRule_Block5);
  }

  /* <Block> ::= <FeedOutputsBlock> */
  void Parser::Rule_Block6(Token *token)
  {
    evaluateTokens(token, reRule_Block6);
  }

  /* <Block> ::= <OutputsBlock> */
  void Parser::Rule_Block7(Token *token)
  {
    evaluateTokens(token, reRule_Block7);
  }

  /* <Block> ::= <PrivateFunctionsBlock> */
  void Parser::Rule_Block8(Token *token)
  {
    evaluateTokens(token, reRule_Block8);
  }

  /* <Block> ::= <PublicFunctionsBlock> */
  void Parser::Rule_Block9(Token *token)
  {
    evaluateTokens(token, reRule_Block9);
  }

  /* <Block> ::= <ChildrenBlock> */
  void Parser::Rule_Block10(Token *token)
  {
    evaluateTokens(token, reRule_Block10);
  }

  /* <Block> ::= <Connection> */
  void Parser::Rule_Block11(Token *token)
  {
    evaluateTokens(token, reRule_Block11);
  }

  /* <Blocks> ::= <Block> */
  void Parser::Rule_Blocks(Token *token)
  {
    evaluateTokens(token, reRule_Blocks);
  }

  /* <Blocks> ::= <Blocks> <Block> */
  void Parser::Rule_Blocks2(Token *token)
  {
    evaluateTokens(token, reRule_Blocks2);
  }

  /* <ModuleDefinition> ::= <CustomOption> module_def IdentifierTerm '{' <Blocks> '}' */
  void Parser::Rule_ModuleDefinition_module_def_IdentifierTerm_LBrace_RBrace(Token *token)
  {
    CheckTokenForReservedKeyword(2);
    SHLApply(1, sKeyword2);
    SHLApply(2, sTypeName);

    // store keywords
    getContext()->m_modifierOptionsStack = &getContext()->m_def->m_modifiers;
    executeRule(token->m_tokens[0]);
    getContext()->m_keywords->validateModuleModifiers(token, getContext()->m_def->m_modifiers);

    getContext()->m_def->m_name = token->m_tokens[2]->m_data;
    getContext()->m_def->m_lineDefined = token->m_line;

    executeRule(token->m_tokens[4]);

    SHLMarkBlock(token->m_tokens[1]->m_line, token->m_tokens[5]->m_line);
  }

  void Parser::processSavedComments(const SavedComments &savedComments)
  {
    const char* scData;

    // Does the module have preceding comment?
    if (savedComments.find(getContext()->m_def->m_lineDefined, &scData))
    {
      MDFModule::Context* context = getContext();
      context->m_def->m_shortDesc.set(stripWhiteSpace(scData));
    }

    // Private API Function comments.
    iterate_begin(ModuleDef::FnDecl, getContext()->m_def->m_privateAPI, fd)
    {
      if (savedComments.find(fd.m_lineDefined, &scData))
      {
        fd.m_shortDesc.set(stripWhiteSpace(scData));
      }
    }
    iterate_end

    // Public API function comments.
    iterate_begin(ModuleDef::FnDecl, getContext()->m_def->m_publicAPI, fd)
    {
      if (savedComments.find(fd.m_lineDefined, &scData))
      {
        fd.m_shortDesc.set(stripWhiteSpace(scData));
      }
    }
    iterate_end

    // try to connect saved comments to vardecl instances, by doing a brute force
    // search across their m_lineDefined values
    for (unsigned int n=0; n<msNumVarDeclBlocks; n++)
    {
      iterate_begin(ModuleDef::VarDecl, getContext()->m_def->m_vardecl[n], vd)
      {
        if (savedComments.find(vd.m_lineDefined, &scData))
        {
          vd.m_comment.set(stripWhiteSpace(scData));
        }
      }
      iterate_end
    }

    // Enumerations handling.
    iterate_begin(ModuleDef::EnumDecl, getContext()->m_def->m_enumDecl, ed)
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
  }
} // namespace 

