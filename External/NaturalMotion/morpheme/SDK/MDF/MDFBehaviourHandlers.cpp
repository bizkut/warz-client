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

namespace MDFBehaviour
{
  #include "CustomOptionKeyword.inl"

  //--------------------------------------------------------------------------------------------------------------------
  /* <NumericLiteral> ::= FloatLiteral */
  void Parser::Rule_NumericLiteral_FloatLiteral(Token *token)
  {
    if (getContext()->m_literalsIndex >= Context::kMaxLiterals)
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "declaring >3 literals in single group, unsupported"));
    }
    SHLApply(0, sNumeric);

    double fLiteral = atof(token->m_tokens[0]->m_data);
    getContext()->m_literals[getContext()->m_literalsIndex].f = (float)fLiteral;

    getContext()->m_literalsIndex ++;
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <NumericLiteral> ::= DecLiteral */
  void Parser::Rule_NumericLiteral_DecLiteral(Token *token)
  {
    if (getContext()->m_literalsIndex >= Context::kMaxLiterals)
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "declaring >3 literals in single group, unsupported"));
    }
    SHLApply(0, sNumeric);

    int iLiteral = atoi(token->m_tokens[0]->m_data);
    if (getContext()->m_animInput)
    {
      getContext()->m_literals[getContext()->m_literalsIndex].f = (float)iLiteral;
    }
    else if (getContext()->m_attrVar->getType() == bhvtFloat ||
        getContext()->m_attrVar->getType() == bhvtVector3)
      getContext()->m_literals[getContext()->m_literalsIndex].f = (float)iLiteral;
    else
      getContext()->m_literals[getContext()->m_literalsIndex].i = iLiteral;

    getContext()->m_literalsIndex ++;
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <NumericLiteral> ::= BoolLiteral */
  void Parser::Rule_NumericLiteral_BoolLiteral(Token *token)
  {
    if (getContext()->m_animInput)
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "boolean literals not supported for this type"));
    }
    if (getContext()->m_attrVar->getType() != bhvtBool)
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "boolean literals not supported for this type"));
    }

    if (getContext()->m_literalsIndex != 0)
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "boolean literals not supported in multi-literal groups"));
    }
    SHLApply(0, sKeyword1);

    getContext()->m_literals[getContext()->m_literalsIndex].b = !strcmp(token->m_tokens[0]->m_data, "true");

    getContext()->m_literalsIndex ++;
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <NumericLiterals> ::= <NumericLiteral> */
  void Parser::Rule_NumericLiterals(Token *token)
  {
    evaluateTokens(token, reRule_NumericLiterals);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <NumericLiterals> ::= <NumericLiterals> ',' <NumericLiteral> */
  void Parser::Rule_NumericLiterals_Comma(Token *token)
  {
    evaluateTokens(token, reRule_NumericLiterals_Comma);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <NumericLiteralsGroup> ::= '(' <NumericLiterals> ')' */
  void Parser::Rule_NumericLiteralsGroup_LParan_RParan(Token *token)
  {
    if (getContext()->m_attrVar->getType() != bhvtVector3)
    {
      throw(new RuleExecutionException(token, getContext()->m_attrVar->m_name.c_str(), "(...) initializer syntax only used for vector3 types"));
    }

    executeRule(token->m_tokens[1]);

    if (getContext()->m_literalsIndex != 3)
    {
      throw(new RuleExecutionException(token, getContext()->m_attrVar->m_name.c_str(), "requires 3 literals to initialize a vector3 type"));
    }

    getContext()->m_data.v[0] = getContext()->m_literals[0].f;
    getContext()->m_data.v[1] = getContext()->m_literals[1].f;
    getContext()->m_data.v[2] = getContext()->m_literals[2].f;
  }


  //--------------------------------------------------------------------------------------------------------------------
  /* <ModuleIdentifier> ::= IdentifierTerm */
  void Parser::Rule_ModuleIdentifier_IdentifierTerm(Token *token)
  {
    parserMemoryAllocArgs(BehaviourDef::ModuleToEnable::PathItem, pi, token->m_tokens[0]->m_data);
    SHLApply(0, sIdentifier);
    getContext()->m_modulePath->m_path.push(pi);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <ModuleIdentifier> ::= IdentifierTerm ArraySpecifier */
  void Parser::Rule_ModuleIdentifier_IdentifierTerm_ArraySpecifier(Token *token)
  {
    parserMemoryAllocArgs(BehaviourDef::ModuleToEnable::PathItem, pi, token->m_tokens[0]->m_data);
    pi->m_arrayAccess = true;

    // catch invalid array indices early
    int arraySpec = atoi(&token->m_tokens[1]->m_data[1]);
    if (arraySpec < 0 || arraySpec > VarDeclBase::vdMaxArraySize)
    {
      throw(new RuleExecutionException(token, token->m_tokens[1]->m_data, "array index out of range (negative / too large)"));
    }

    SHLApply(0, sIdentifier);
    SHLApply(1, sNumeric);

    pi->m_arrayIndex = arraySpec;
    getContext()->m_modulePath->m_path.push(pi);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <ModuleIdentifier> ::= IdentifierTerm [ IdStar ] */
  void Parser::Rule_ModuleIdentifier_IdentifierTerm_LBracket_IdStar_RBracket(Token *token)
  {
    parserMemoryAllocArgs(BehaviourDef::ModuleToEnable::PathItem, pi, token->m_tokens[0]->m_data);
    pi->m_arrayAccess = true;

    SHLApply(0, sIdentifier);
    SHLApply(2, sNumeric);

    getContext()->m_modulePath->m_path.push(pi);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <ModuleWiring> ::= <ModuleIdentifier> */
  void Parser::Rule_ModuleWiring(Token *token)
  {
    executeRule(token->m_tokens[0]);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <ModuleWiring> ::= <ModuleIdentifier> '.' <ModuleWiring> */
  void Parser::Rule_ModuleWiring_Dot(Token *token)
  {
    evaluateTokens(token, reRule_ModuleWiring_Dot);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <ModuleWiring> ::= IdStar */
  void Parser::Rule_ModuleWiring_IdStar(Token *token)
  {
    parserMemoryAllocArgs(BehaviourDef::ModuleToEnable::PathItem, pi, token->m_tokens[0]->m_data);
    SHLApply(0, sNumeric);
    getContext()->m_modulePath->m_path.push(pi);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <ModuleDef> ::= <ModuleWiring> ';' */
  void Parser::Rule_ModuleDef_Semi(Token *token)
  {
    getContext()->m_def->m_modulesToEnable.push_back(BehaviourDef::ModuleToEnable());
    getContext()->m_modulePath = &getContext()->m_def->m_modulesToEnable.back();

    executeRule(token->m_tokens[0]);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <ModuleDefs> ::= <ModuleDef> */
  void Parser::Rule_ModuleDefs(Token *token)
  {
    evaluateTokens(token, reRule_ModuleDefs);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <ModuleDefs> ::= <ModuleDefs> <ModuleDef> */
  void Parser::Rule_ModuleDefs2(Token *token)
  {
    evaluateTokens(token, reRule_ModuleDefs2);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <AttributeGroupSpec> ::= ( StringLiteral )
  void Parser::Rule_AttributeGroupSpec_LParan_StringLiteral_RParan(Token *token)
  {
    SHLApply(1, sString);

    size_t stringLiteralLen = strlen(token->m_tokens[1]->m_data);
    char *stripped = (char*)_alloca(stringLiteralLen);
    assert(stripped);

    // clip off the " "
    NMP_STRNCPY_S(stripped, stringLiteralLen, &token->m_tokens[1]->m_data[1]);
    stripped[stringLiteralLen - 2] = '\0';

    // find or create a group with the given name
    getContext()->m_attrGroup = getContext()->m_def->findOrCreateAttributeGroup(stripped);
    getContext()->m_attrGroup->m_lineDefined = token->m_tokens[0]->m_line;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <AttributeGroupSpec> ::= ( StringLiteral , StringLiteral )
  void Parser::Rule_AttributeGroupDef_group_LBrace_RBrace(Token *token)
  {
    SHLApply(1, sKeyword1);

    evaluateTokens(token, reRule_AttributeGroupDef_group_LBrace_RBrace);
    getContext()->m_attrGroup->m_lineDefined = token->m_tokens[0]->m_line;
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AttributeGroupDef> ::= '{' <AttributeLists> '}' */
  void Parser::Rule_AttributeGroupDef_LBrace_RBrace(Token *token)
  {
    // find or create the default attribute group
    getContext()->m_attrGroup = getContext()->m_def->findOrCreateAttributeGroup("Default");

    evaluateTokens(token, reRule_AttributeGroupDef_LBrace_RBrace);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // search both the local enums and then the global constants table for the given identifier string, returning
  // the resulting evaluated integer as well as the qualified enum string in sb
  int lookupIdentifierForArraySize(MDFBehaviour::Context* context, Token *token, int index, StringBuilder& sb)
  {
    int arraySizeVar;
    if (!context->m_constants->find(token->m_tokens[index]->m_data, &arraySizeVar))
    {
      const EnumDeclBase::Entry* ent = searchAllEnumEntries(token->m_tokens[index]->m_data, context->m_attrGroup->m_enumDecl);
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
      sb.appendf("%sBehaviourData::%s", context->m_def->m_name.c_str(), token->m_tokens[index]->m_data);
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


  //--------------------------------------------------------------------------------------------------------------------
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

  //--------------------------------------------------------------------------------------------------------------------
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

  //--------------------------------------------------------------------------------------------------------------------
  /* <EnumExpr> ::= <EnumExpr> + <EnumExprValue> */
  void Parser::Rule_EnumExpr_Plus(Token *token)
  {
    executeRule(token->m_tokens[0]);
    getContext()->m_arithComp.m_ops.push(ArithmeticComputer::acOp_Add);
    getContext()->m_arithComp.m_strings.push(token->m_tokens[1]->m_data);
    executeRule(token->m_tokens[2]);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <EnumExpr> ::= <EnumExpr> - <EnumExprValue> */
  void Parser::Rule_EnumExpr_Minus(Token *token)
  {
    executeRule(token->m_tokens[0]);
    getContext()->m_arithComp.m_ops.push(ArithmeticComputer::acOp_Sub);
    getContext()->m_arithComp.m_strings.push(token->m_tokens[1]->m_data);
    executeRule(token->m_tokens[2]);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <EnumExpr> ::= <EnumExpr> IdStar <EnumExprValue> */
  void Parser::Rule_EnumExpr_IdStar(Token *token)
  {
    executeRule(token->m_tokens[0]);
    getContext()->m_arithComp.m_ops.push(ArithmeticComputer::acOp_Mul);
    getContext()->m_arithComp.m_strings.push(token->m_tokens[1]->m_data);
    executeRule(token->m_tokens[2]);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <EnumExpr> ::= <EnumExprValue> */
  void Parser::Rule_EnumExpr(Token *token)
  {
    executeRule(token->m_tokens[0]);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AttributeEnum> ::= IdentifierTerm */
  void Parser::Rule_AttributeEnum_IdentifierTerm(Token *token)
  {
    assert(getContext()->m_attrGroup);

    if (searchAllEnumEntries(token->m_tokens[0]->m_data, getContext()->m_attrGroup->m_enumDecl))
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "enum value already defined elsewhere in this group"));
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

  //--------------------------------------------------------------------------------------------------------------------
  /* <AttributeEnum> ::= IdentifierTerm = <EnumExpr> */
  void Parser::Rule_AttributeEnum_IdentifierTerm_Eq(Token *token)
  {
    assert(getContext()->m_attrGroup);

    if (searchAllEnumEntries(token->m_tokens[0]->m_data, getContext()->m_attrGroup->m_enumDecl))
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "enum value already defined elsewhere in this group"));
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

  //--------------------------------------------------------------------------------------------------------------------
  /* <AttributeEnums> ::= <AttributeEnum> */
  void Parser::Rule_AttributeEnums(Token *token)
  {
    evaluateTokens(token, reRule_AttributeEnums);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AttributeEnums> ::= <AttributeEnums> ',' <AttributeEnum> */
  void Parser::Rule_AttributeEnums_Comma(Token *token)
  {
    evaluateTokens(token, reRule_AttributeEnums_Comma);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AttributeGroupDefs> ::= <AttributeGroupDef> */
  void Parser::Rule_AttributeGroupDefs(Token *token)
  {
    evaluateTokens(token, reRule_AttributeGroupDefs);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AttributeGroupDefs> ::= <AttributeGroupDefs> <AttributeGroupDef> */
  void Parser::Rule_AttributeGroupDefs2(Token *token)
  {
    evaluateTokens(token, reRule_AttributeGroupDefs2);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AttributeDefault> ::= <NumericLiteralsGroup> */
  void Parser::Rule_AttributeDefault(Token *token)
  {
    getContext()->m_literalsIndex = 0;
    executeRule(token->m_tokens[0]);

    MDFBehaviour::Context::AttributeDataBlock adb = (MDFBehaviour::Context::AttributeDataBlock)getContext()->m_dataBlock;
    switch (adb)
    {
      case MDFBehaviour::Context::ADBDefault:
        getContext()->m_attrVar->m_default = getContext()->m_data;
        getContext()->m_attrVar->m_hasDefault = true;
        break;
      case MDFBehaviour::Context::ADBMin:
        getContext()->m_attrVar->m_min = getContext()->m_data;
        getContext()->m_attrVar->m_hasMin = true;
        break;
      case MDFBehaviour::Context::ADBMax:
        getContext()->m_attrVar->m_max = getContext()->m_data;
        getContext()->m_attrVar->m_hasMax = true;
        break;
    }

    getContext()->m_dataBlock ++;
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AttributeDefault> ::= <NumericLiteral> */
  void Parser::Rule_AttributeDefault2(Token *token)
  {
    getContext()->m_literalsIndex = 0;
    executeRule(token->m_tokens[0]);

    MDFBehaviour::Context::AttributeDataBlock adb = (MDFBehaviour::Context::AttributeDataBlock)getContext()->m_dataBlock;
    BehaviourDef::AttributeData *adata = 0;
    switch (adb)
    {
      case MDFBehaviour::Context::ADBDefault:
        adata = &getContext()->m_attrVar->m_default;
        getContext()->m_attrVar->m_hasDefault = true;
        break;
      case MDFBehaviour::Context::ADBMin:
        {
          if (getContext()->m_attrVar->getType() == bhvtBool)
          {
            throw(new RuleExecutionException(token, getContext()->m_attrVar->m_name.c_str(), "cannot supply minimum/maximum values for boolean types"));
          }
          adata = &getContext()->m_attrVar->m_min;
          getContext()->m_attrVar->m_hasMin = true;
        }
        break;
      case MDFBehaviour::Context::ADBMax:
        {
          if (getContext()->m_attrVar->getType() == bhvtBool)
          {
            throw(new RuleExecutionException(token, getContext()->m_attrVar->m_name.c_str(), "cannot supply minimum/maximum values for boolean types"));
          }
          adata = &getContext()->m_attrVar->m_max;
          getContext()->m_attrVar->m_hasMax = true;
        }
        break;

      default:
        throw(new RuleExecutionException(token, getContext()->m_attrVar->m_name.c_str(), "cannot specify more than 3 initializers (default, min, max)"));
        break;
    }

    switch (getContext()->m_attrVar->getType())
    {
      case bhvtFloat:
        assert(getContext()->m_literalsIndex == 1);
        adata->f = getContext()->m_literals[0].f;
        break;
      case bhvtInteger:
        assert(getContext()->m_literalsIndex == 1);
        adata->i = getContext()->m_literals[0].i;
        break;
      case bhvtBool:
        assert(getContext()->m_literalsIndex == 1);
        adata->b = getContext()->m_literals[0].b;
        break;

      case bhvtVector3:
        throw(new RuleExecutionException(token, getContext()->m_attrVar->m_name.c_str(), "need to specify vector3 initializers (x,y,z)"));
        break;
      default:
        assert(0);
        break;
    }

    getContext()->m_dataBlock ++;
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AttributeDefaults> ::= <AttributeDefault> */
  void Parser::Rule_AttributeDefaults(Token *token)
  {
    evaluateTokens(token, reRule_AttributeDefaults);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AttributeDefaults> ::= <AttributeDefaults> ',' <AttributeDefault> */
  void Parser::Rule_AttributeDefaults_Comma(Token *token)
  {
    evaluateTokens(token, reRule_AttributeDefaults_Comma);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AttributeSpec> ::= IdentifierTerm */
  void Parser::Rule_AttributeSpec_IdentifierTerm(Token *token)
  {
    CheckTokenForReservedKeyword(0);
    SHLApply(0, sIdentifier);

    // add new attribute
    BehaviourDef::AttributeVariables& decls = getContext()->m_attrGroup->m_vars;
    decls.push_back(BehaviourDef::AttributeVariable(getContext()->m_attrVT, token->m_tokens[0]->m_data, 0));
    getContext()->m_attrVar = &decls.back();
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AttributeSpec> ::= IdentifierTerm ArraySpecifier */
  void Parser::Rule_AttributeSpec_IdentifierTerm_ArraySpecifier(Token *token)
  {
    CheckTokenForReservedKeyword(0);
    SHLApply(0, sIdentifier);
    SHLApply(1, sNumeric);

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

    // add new attribute
    BehaviourDef::AttributeVariables& decls = getContext()->m_attrGroup->m_vars;
    decls.push_back(BehaviourDef::AttributeVariable(getContext()->m_attrVT, token->m_tokens[0]->m_data, (unsigned int)arraySpec));
    getContext()->m_attrVar = &decls.back();
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AttributeSpec> ::= IdentifierTerm [ IdentifierTerm ] */
  void Parser::Rule_AttributeSpec_IdentifierTerm_LBracket_IdentifierTerm_RBracket(Token *token)
  {
    CheckTokenForReservedKeyword(0);
    SHLApply(0, sIdentifier);
    SHLApply(2, sEnumEntry);

    int arraySizeVar = lookupIdentifierForArraySize(getContext(), token, 2, *m_stringBuilder);

    // add new attribute
    BehaviourDef::AttributeVariables& decls = getContext()->m_attrGroup->m_vars;
    decls.push_back(BehaviourDef::AttributeVariable(getContext()->m_attrVT, token->m_tokens[0]->m_data, (unsigned int)arraySizeVar));
    getContext()->m_attrVar = &decls.back();

    // record array string, if applicable
    getContext()->m_attrVar->m_arraySizeAsString = m_stringBuilder->c_str();
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AttributeList> ::= IdentifierTerm <AttributeSpec> = <AttributeDefaults> <CustomOption> ; */
  void Parser::Rule_AttributeList_IdentifierTerm_Eq_Semi(Token *token)
  {
    // get known type
    BhDefVariableType vt = getContext()->m_keywords->behaviourDefVariableTypeFromString(token->m_tokens[0]->m_data);
    if (vt == bhvtUnknown)
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "unknown variable type used for attribute"));
    }
    SHLApply(0, sTypeName);

    getContext()->m_attrVT = vt;
    executeRule(token->m_tokens[1]);

    // gather modifier options
    getContext()->m_modifierOptionsStack = &getContext()->m_attrVar->m_modifiers;
    getContext()->m_modifierOptionsStack->stack.reset();
    executeRule(token->m_tokens[4]);

    getContext()->m_keywords->validateBehaviourVariableModifiers(token, *getContext()->m_modifierOptionsStack);

    // read defaults/min/max
    getContext()->m_dataBlock = (int)MDFBehaviour::Context::ADBDefault;
    getContext()->m_literalsIndex = 0;
    executeRule(token->m_tokens[3]);

    getContext()->m_attrVar->m_lineDefined = token->m_tokens[5]->m_line;
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AttributeList> ::= enum IdentifierTerm '{' <AttributeEnums> '}' <CustomOption> ';' */
  void Parser::Rule_AttributeList_enum_IdentifierTerm_LBrace_RBrace_Semi(Token *token)
  {
    CheckTokenForReservedKeyword(1);
    SHLApply(0, sKeyword1);
    SHLApply(1, sTypeName);

    // add new decl
    BehaviourDef::AttributeGroup::EnumDecls& decls = getContext()->m_attrGroup->m_enumDecl;
    decls.push_back(BehaviourDef::AttributeGroup::EnumDecl());
    getContext()->m_enumdecl = &decls.back();

    // setup basics
    getContext()->m_enumdecl->m_name.set(token->m_tokens[1]->m_data);
    getContext()->m_enumdecl->m_ownerBehaviourDef = getContext()->m_def;
    getContext()->m_enumdecl->m_lineDefined = token->m_line;

    // gather modifier options
    getContext()->m_modifierOptionsStack = &getContext()->m_enumdecl->m_modifiers;
    getContext()->m_modifierOptionsStack->stack.reset();
    executeRule(token->m_tokens[5]);

    getContext()->m_keywords->validateBehaviourVariableModifiers(token, getContext()->m_enumdecl->m_modifiers);

    // walk the enum entries
    executeRule(token->m_tokens[3]);

    // MORPH-22020: investigate suspicious line in parser rule that breaks enumeration preceding comments
    getContext()->m_enumdecl->m_lineDefined = token->m_tokens[6]->m_line;
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AttributeLists> ::= <AttributeList> */
  void Parser::Rule_AttributeLists(Token *token)
  {
    evaluateTokens(token, reRule_AttributeLists);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AttributeLists> ::= <AttributeLists> <AttributeList> */
  void Parser::Rule_AttributeLists2(Token *token)
  {
    evaluateTokens(token, reRule_AttributeLists2);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <ControlParamDefault> ::= <NumericLiteralsGroup>
  void Parser::Rule_ControlParamDefault(Token *token)
  {
    getContext()->m_literalsIndex = 0;
    executeRule(token->m_tokens[0]);

    MDFBehaviour::Context::AttributeDataBlock adb = (MDFBehaviour::Context::AttributeDataBlock)getContext()->m_dataBlock;
    switch (adb)
    {
    case MDFBehaviour::Context::ADBDefault:
      getContext()->m_attrVar->m_default = getContext()->m_data;
      getContext()->m_attrVar->m_hasDefault = true;
      break;
    case MDFBehaviour::Context::ADBMin:
      getContext()->m_attrVar->m_min = getContext()->m_data;
      getContext()->m_attrVar->m_hasMin = true;
      break;
    case MDFBehaviour::Context::ADBMax:
      getContext()->m_attrVar->m_max = getContext()->m_data;
      getContext()->m_attrVar->m_hasMax = true;
      break;
    }

    getContext()->m_dataBlock ++;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <ControlParamDefault> ::= <NumericLiteral>
  void Parser::Rule_ControlParamDefault2(Token *token)
  {
    getContext()->m_literalsIndex = 0;
    executeRule(token->m_tokens[0]);

    MDFBehaviour::Context::AttributeDataBlock adb = (MDFBehaviour::Context::AttributeDataBlock)getContext()->m_dataBlock;
    BehaviourDef::AttributeData *adata = 0;
    switch (adb)
    {
    case MDFBehaviour::Context::ADBDefault:
      adata = &getContext()->m_attrVar->m_default;
      getContext()->m_attrVar->m_hasDefault = true;
      break;
    case MDFBehaviour::Context::ADBMin:
      {
        if (getContext()->m_attrVar->getType() == bhvtBool)
        {
          throw(new RuleExecutionException(token, getContext()->m_attrVar->m_name.c_str(), "cannot supply minimum/maximum values for boolean types"));
        }
        adata = &getContext()->m_attrVar->m_min;
        getContext()->m_attrVar->m_hasMin = true;
      }
      break;
    case MDFBehaviour::Context::ADBMax:
      {
        if (getContext()->m_attrVar->getType() == bhvtBool)
        {
          throw(new RuleExecutionException(token, getContext()->m_attrVar->m_name.c_str(), "cannot supply minimum/maximum values for boolean types"));
        }
        adata = &getContext()->m_attrVar->m_max;
        getContext()->m_attrVar->m_hasMax = true;
      }
      break;

    default:
      throw(new RuleExecutionException(token, getContext()->m_attrVar->m_name.c_str(), "cannot specify more than 3 initializers (default, min, max)"));
      break;
    }

    switch (getContext()->m_attrVar->getType())
    {
    case bhvtFloat:
      assert(getContext()->m_literalsIndex == 1);
      adata->f = getContext()->m_literals[0].f;
      break;
    case bhvtInteger:
      assert(getContext()->m_literalsIndex == 1);
      adata->i = getContext()->m_literals[0].i;
      break;
    case bhvtBool:
      assert(getContext()->m_literalsIndex == 1);
      adata->b = getContext()->m_literals[0].b;
      break;

    case bhvtVector3:
      throw(new RuleExecutionException(token, getContext()->m_attrVar->m_name.c_str(), "need to specify vector3 initializers (x,y,z)"));
      break;
    default:
      assert(0);
      break;
    }

    getContext()->m_dataBlock ++;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <ControlParamDefaults> ::= <ControlParamDefault>
  void Parser::Rule_ControlParamDefaults(Token *token)
  {
    evaluateTokens(token, reRule_AttributeDefaults);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <ControlParamDefaults> ::= <ControlParamDefaults> , <ControlParamDefault>
  void Parser::Rule_ControlParamDefaults_Comma(Token *token)
  {
    evaluateTokens(token, reRule_AttributeDefaults_Comma);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <ControlParamSpec> ::= IdentifierTerm */
  void Parser::Rule_ControlParamSpec_IdentifierTerm(Token *token)
  {
    CheckTokenForReservedKeyword(0);
    SHLApply(0, sIdentifier);

    // add new control param
    getContext()->m_paramGroup->m_params.push_back(BehaviourDef::ControlParam(
      getContext()->m_attrVT, token->m_tokens[0]->m_data, 0));

    BehaviourDef::ControlParam& cp = getContext()->m_paramGroup->m_params.back();

    getContext()->m_attrVar = (BehaviourDef::AttributeVariable *) &cp;
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <ControlParamSpec> ::= IdentifierTerm ArraySpecifier */
  void Parser::Rule_ControlParamSpec_IdentifierTerm_ArraySpecifier(Token *token)
  {
    CheckTokenForReservedKeyword(0);
    SHLApply(0, sIdentifier);
    SHLApply(1, sNumeric);

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

    // add new control param
    getContext()->m_paramGroup->m_params.push_back(BehaviourDef::ControlParam(getContext()->m_attrVT, token->m_tokens[0]->m_data, (unsigned int)arraySpec));

    BehaviourDef::ControlParam& cp = getContext()->m_paramGroup->m_params.back();

    getContext()->m_attrVar = (BehaviourDef::AttributeVariable *) &cp;
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <ControlParamSpec> ::= IdentifierTerm [ IdentifierTerm ] */
  void Parser::Rule_ControlParamSpec_IdentifierTerm_LBracket_IdentifierTerm_RBracket(Token *token)
  {
    CheckTokenForReservedKeyword(0);
    SHLApply(0, sIdentifier);
    SHLApply(2, sIdentifier);

    int arraySizeVar = lookupIdentifierForArraySize(getContext(), token, 2, *m_stringBuilder);

    // add new control param
    getContext()->m_paramGroup->m_params.push_back(BehaviourDef::ControlParam(getContext()->m_attrVT, token->m_tokens[0]->m_data, (unsigned int)arraySizeVar));
    BehaviourDef::ControlParam& cp = getContext()->m_paramGroup->m_params.back();

    getContext()->m_attrVar = (BehaviourDef::AttributeVariable *) &cp;

    // record array string, if applicable
    cp.m_arraySizeAsString = m_stringBuilder->c_str();
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <ControlParamDef> ::= IdentifierTerm <ControlParamSpec> <CustomOption> ; */
  void Parser::Rule_ControlParamDef_IdentifierTerm_Eq_Semi(Token *token)
  {
    // get known type
    BhDefVariableType vt = getContext()->m_keywords->behaviourDefVariableTypeFromString(token->m_tokens[0]->m_data);
    if (vt == bhvtUnknown)
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "unknown variable type used for control parameter"));
    }
    SHLApply(0, sTypeName);

    getContext()->m_attrVT = vt;
    executeRule(token->m_tokens[1]);

    BehaviourDef::ControlParam& cp = getContext()->m_paramGroup->m_params.back();

    // gather modifier options
    getContext()->m_modifierOptionsStack = &cp.m_modifiers;
    getContext()->m_modifierOptionsStack->stack.reset();
    executeRule(token->m_tokens[4]);

    getContext()->m_keywords->validateBehaviourVariableModifiers(token, cp.m_modifiers);

    // read defaults/min/max
    getContext()->m_dataBlock = (int)MDFBehaviour::Context::ADBDefault;
    getContext()->m_literalsIndex = 0;
    executeRule(token->m_tokens[3]);

    cp.m_lineDefined = token->m_tokens[5]->m_line;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <ControlParamDef> ::= IdentifierTerm <ControlParamSpec> ;
  void Parser::Rule_ControlParamDef_IdentifierTerm_Semi(Token *token)
  {
    // get known type
    BhDefVariableType vt = getContext()->m_keywords->behaviourDefVariableTypeFromString(token->m_tokens[0]->m_data);
    if (vt == bhvtUnknown)
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "unknown variable type used for control parameter"));
    }
    SHLApply(0, sTypeName);

    getContext()->m_attrVT = vt;
    executeRule(token->m_tokens[1]);

    BehaviourDef::ControlParam& cp = getContext()->m_paramGroup->m_params.back();
    cp.m_lineDefined = token->m_tokens[3]->m_line;

    // gather modifier options
    getContext()->m_modifierOptionsStack = &cp.m_modifiers;
    getContext()->m_modifierOptionsStack->stack.reset();
    executeRule(token->m_tokens[2]);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <ControlParamGroupDef> ::= <CustomOption> group '(' StringLiteral ')' '{' <ControlParamLists> '}' */
  void Parser::Rule_ControlParamGroupDef_group_LParan_StringLiteral_RParan_LBrace_RBrace(Token *token)
  {
    size_t stringLiteralLen = strlen(token->m_tokens[3]->m_data);
    char *stripped = (char*)_alloca(stringLiteralLen);
    assert(stripped);

    // clip off the " "
    NMP_STRNCPY_S(stripped, stringLiteralLen, &token->m_tokens[3]->m_data[1]);
    stripped[stringLiteralLen - 2] = '\0';

    SHLApply(1, sKeyword1);
    SHLApply(3, sString);

    // find or create a group with the given name
    getContext()->m_paramGroup = getContext()->m_def->findOrCreateControlParamGroup(stripped);

    evaluateTokens(token, reRule_ControlParamGroupDef_group_LParan_StringLiteral_RParan_LBrace_RBrace);

    getContext()->m_paramGroup->m_lineDefined = token->m_tokens[4]->m_line;
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <ControlParamGroupDef> ::= '{' <ControlParamLists> '}' */
  void Parser::Rule_ControlParamGroupDef_LBrace_RBrace(Token *token)
  {
    // find or create the default attribute group
    getContext()->m_paramGroup = getContext()->m_def->findOrCreateControlParamGroup("Input Defaults");

    evaluateTokens(token, reRule_ControlParamGroupDef_LBrace_RBrace);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <ControlParamGroupDefs> ::= <ControlParamGroupDef> */
  void Parser::Rule_ControlParamGroupDefs(Token *token)
  {
    evaluateTokens(token, reRule_ControlParamGroupDefs);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <ControlParamGroupDefs> ::= <ControlParamGroupDefs> <ControlParamGroupDef> */
  void Parser::Rule_ControlParamGroupDefs2(Token *token)
  {
    evaluateTokens(token, reRule_ControlParamGroupDefs2);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <ControlParamDefs> ::= <ControlParamDef> */
  void Parser::Rule_ControlParamDefs(Token *token)
  {
    evaluateTokens(token, reRule_ControlParamDefs);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <ControlParamDefs> ::= <ControlParamDefs> <ControlParamDef> */
  void Parser::Rule_ControlParamDefs2(Token *token)
  {
    evaluateTokens(token, reRule_ControlParamDefs2);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <OutputControlParamDefault> ::= <NumericLiteralsGroup>
  void Parser::Rule_OutputControlParamDefault(Token *token)
  {
    getContext()->m_literalsIndex = 0;
    executeRule(token->m_tokens[0]);

    MDFBehaviour::Context::AttributeDataBlock adb = (MDFBehaviour::Context::AttributeDataBlock)getContext()->m_dataBlock;
    switch (adb)
    {
    case MDFBehaviour::Context::ADBDefault:
      getContext()->m_attrVar->m_default = getContext()->m_data;
      getContext()->m_attrVar->m_hasDefault = true;
      break;
    case MDFBehaviour::Context::ADBMin:
      getContext()->m_attrVar->m_min = getContext()->m_data;
      getContext()->m_attrVar->m_hasMin = true;
      break;
    case MDFBehaviour::Context::ADBMax:
      getContext()->m_attrVar->m_max = getContext()->m_data;
      getContext()->m_attrVar->m_hasMax = true;
      break;
    }

    getContext()->m_dataBlock ++;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <OutputControlParamDefault> ::= <NumericLiteral>
  void Parser::Rule_OutputControlParamDefault2(Token *token)
  {
    getContext()->m_literalsIndex = 0;
    executeRule(token->m_tokens[0]);

    MDFBehaviour::Context::AttributeDataBlock adb = (MDFBehaviour::Context::AttributeDataBlock)getContext()->m_dataBlock;
    BehaviourDef::AttributeData *adata = 0;
    switch (adb)
    {
    case MDFBehaviour::Context::ADBDefault:
      adata = &getContext()->m_attrVar->m_default;
      getContext()->m_attrVar->m_hasDefault = true;
      break;
    case MDFBehaviour::Context::ADBMin:
      {
        if (getContext()->m_attrVar->getType() == bhvtBool)
        {
          throw(new RuleExecutionException(token, getContext()->m_attrVar->m_name.c_str(), "cannot supply minimum/maximum values for boolean types"));
        }
        adata = &getContext()->m_attrVar->m_min;
        getContext()->m_attrVar->m_hasMin = true;
      }
      break;
    case MDFBehaviour::Context::ADBMax:
      {
        if (getContext()->m_attrVar->getType() == bhvtBool)
        {
          throw(new RuleExecutionException(token, getContext()->m_attrVar->m_name.c_str(), "cannot supply minimum/maximum values for boolean types"));
        }
        adata = &getContext()->m_attrVar->m_max;
        getContext()->m_attrVar->m_hasMax = true;
      }
      break;

    default:
      throw(new RuleExecutionException(token, getContext()->m_attrVar->m_name.c_str(), "cannot specify more than 3 initializers (default, min, max)"));
      break;
    }

    switch (getContext()->m_attrVar->getType())
    {
    case bhvtFloat:
      assert(getContext()->m_literalsIndex == 1);
      adata->f = getContext()->m_literals[0].f;
      break;
    case bhvtInteger:
      assert(getContext()->m_literalsIndex == 1);
      adata->i = getContext()->m_literals[0].i;
      break;
    case bhvtBool:
      assert(getContext()->m_literalsIndex == 1);
      adata->b = getContext()->m_literals[0].b;
      break;

    case bhvtVector3:
      throw(new RuleExecutionException(token, getContext()->m_attrVar->m_name.c_str(), "need to specify vector3 initializers (x,y,z)"));
      break;
    default:
      assert(0);
      break;
    }

    getContext()->m_dataBlock ++;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <OutputControlParamDefaults> ::= <OutputControlParamDefault>
  void Parser::Rule_OutputControlParamDefaults(Token *token)
  {
    evaluateTokens(token, reRule_AttributeDefaults);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <OutputControlParamDefaults> ::= <OutputControlParamDefaults> , <OutputControlParamDefault>
  void Parser::Rule_OutputControlParamDefaults_Comma(Token *token)
  {
    evaluateTokens(token, reRule_AttributeDefaults_Comma);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <OutputControlParamSpec> ::= IdentifierTerm */
  void Parser::Rule_OutputControlParamSpec_IdentifierTerm(Token *token)
  {
    CheckTokenForReservedKeyword(0);
    SHLApply(0, sIdentifier);

    // add new outputControl param
    getContext()->m_def->m_outputControlParams.push_back(BehaviourDef::OutputControlParam(
      getContext()->m_attrVT, token->m_tokens[0]->m_data, 0));

    BehaviourDef::OutputControlParam& cp = getContext()->m_def->m_outputControlParams.back();

    getContext()->m_attrVar = (BehaviourDef::AttributeVariable *) &cp;
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <OutputControlParamSpec> ::= IdentifierTerm ArraySpecifier */
  void Parser::Rule_OutputControlParamSpec_IdentifierTerm_ArraySpecifier(Token *token)
  {
    CheckTokenForReservedKeyword(0);
    SHLApply(0, sIdentifier);
    SHLApply(1, sNumeric);

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

    // add new outputControl param
    getContext()->m_def->m_outputControlParams.push_back(BehaviourDef::OutputControlParam(getContext()->m_attrVT, token->m_tokens[0]->m_data, (unsigned int)arraySpec));

    BehaviourDef::OutputControlParam& cp = getContext()->m_def->m_outputControlParams.back();

    getContext()->m_attrVar = (BehaviourDef::AttributeVariable *) &cp;
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <OutputControlParamSpec> ::= IdentifierTerm [ IdentifierTerm ] */
  void Parser::Rule_OutputControlParamSpec_IdentifierTerm_LBracket_IdentifierTerm_RBracket(Token *token)
  {
    CheckTokenForReservedKeyword(0);
    SHLApply(0, sIdentifier);
    SHLApply(2, sIdentifier);

    int arraySizeVar = lookupIdentifierForArraySize(getContext(), token, 2, *m_stringBuilder);

    // add new outputControl param
    getContext()->m_def->m_outputControlParams.push_back(BehaviourDef::OutputControlParam(getContext()->m_attrVT, token->m_tokens[0]->m_data, (unsigned int)arraySizeVar));
    BehaviourDef::OutputControlParam& cp = getContext()->m_def->m_outputControlParams.back();

    getContext()->m_attrVar = (BehaviourDef::AttributeVariable *) &cp;

    // record array string, if applicable
    cp.m_arraySizeAsString = m_stringBuilder->c_str();
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <OutputControlParamDef> ::= IdentifierTerm <OutputControlParamSpec> <CustomOption> ; */
  void Parser::Rule_OutputControlParamDef_IdentifierTerm_Eq_Semi(Token *token)
  {
    // get known type
    BhDefVariableType vt = getContext()->m_keywords->behaviourDefVariableTypeFromString(token->m_tokens[0]->m_data);
    if (vt == bhvtUnknown)
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "unknown variable type used for outputControl parameter"));
    }
    SHLApply(0, sTypeName);

    getContext()->m_attrVT = vt;
    executeRule(token->m_tokens[1]);

    BehaviourDef::OutputControlParam& cp = getContext()->m_def->m_outputControlParams.back();

    // gather modifier options
    getContext()->m_modifierOptionsStack = &cp.m_modifiers;
    getContext()->m_modifierOptionsStack->stack.reset();
    executeRule(token->m_tokens[4]);

    getContext()->m_keywords->validateBehaviourVariableModifiers(token, cp.m_modifiers);

    // read defaults/min/max
    getContext()->m_dataBlock = (int)MDFBehaviour::Context::ADBDefault;
    getContext()->m_literalsIndex = 0;
    executeRule(token->m_tokens[3]);

    cp.m_lineDefined = token->m_tokens[5]->m_line;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <OutputControlParamDef> ::= IdentifierTerm <OutputControlParamSpec> ;
  void Parser::Rule_OutputControlParamDef_IdentifierTerm_Semi(Token *token)
  {
    // get known type
    BhDefVariableType vt = getContext()->m_keywords->behaviourDefVariableTypeFromString(token->m_tokens[0]->m_data);
    if (vt == bhvtUnknown)
    {
      throw(new RuleExecutionException(token, token->m_tokens[0]->m_data, "unknown variable type used for outputControl parameter"));
    }
    SHLApply(0, sTypeName);

    getContext()->m_attrVT = vt;
    executeRule(token->m_tokens[1]);

    BehaviourDef::OutputControlParam& cp = getContext()->m_def->m_outputControlParams.back();
    cp.m_lineDefined = token->m_tokens[3]->m_line;

    // gather modifier options
    getContext()->m_modifierOptionsStack = &cp.m_modifiers;
    getContext()->m_modifierOptionsStack->stack.reset();
    executeRule(token->m_tokens[2]);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <OutputControlParamDefs> ::= <OutputControlParamDef> */
  void Parser::Rule_OutputControlParamDefs(Token *token)
  {
    evaluateTokens(token, reRule_OutputControlParamDefs);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <OutputControlParamDefs> ::= <OutputControlParamDefs> <OutputControlParamDef> */
  void Parser::Rule_OutputControlParamDefs2(Token *token)
  {
    evaluateTokens(token, reRule_OutputControlParamDefs2);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <InterfaceBlock> ::= interface:
  void Parser::Rule_InterfaceBlock_interfaceColon(Token *token)
  {
    SHLApply(0, sKeyword2);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <InterfaceBlock> ::= interface: <InterfaceDefs>
  void Parser::Rule_InterfaceBlock_interfaceColon2(Token *token)
  {
    SHLApply(0, sKeyword2);
    evaluateTokens(token, reRule_InterfaceBlock_interfaceColon2);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <InterfaceDef> ::= version = DecLiteral ;
  void Parser::Rule_InterfaceDef_version_Eq_DecLiteral_Semi(Token *token)
  {
    SHLApply(0, sIdentifier);
    getContext()->m_def->m_version = token->m_tokens[2]->m_data;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <InterfaceDef> ::= include = StringLiteral ;
  void Parser::Rule_InterfaceDef_include_Eq_StringLiteral_Semi(Token *token)
  {
    SHLApply(0, sIdentifier);

    size_t stringLiteralLen = strlen(token->m_tokens[2]->m_data);
    char *stripped = (char*)_alloca(stringLiteralLen);
    assert(stripped);

    // clip off the " "
    NMP_STRNCPY_S(stripped, stringLiteralLen, &token->m_tokens[2]->m_data[1]);
    stripped[stringLiteralLen - 2] = '\0';

    getContext()->m_def->m_include = stripped;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <InterfaceDef> ::= displayname = StringLiteral ;
  void Parser::Rule_InterfaceDef_displayname_Eq_StringLiteral_Semi(Token *token)
  {
    size_t stringLiteralLen = strlen(token->m_tokens[2]->m_data);
    char *stripped = (char*)_alloca(stringLiteralLen);
    assert(stripped);

    // clip off the " "
    NMP_STRNCPY_S(stripped, stringLiteralLen, &token->m_tokens[2]->m_data[1]);
    stripped[stringLiteralLen - 2] = '\0';

    getContext()->m_def->m_displayName = stripped;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <InterfaceDef> ::= group = StringLiteral ;
  void Parser::Rule_InterfaceDef_group_Eq_StringLiteral_Semi(Token *token)
  {
    size_t stringLiteralLen = strlen(token->m_tokens[2]->m_data);
    char *stripped = (char*)_alloca(stringLiteralLen);
    assert(stripped);

    // clip off the " "
    NMP_STRNCPY_S(stripped, stringLiteralLen, &token->m_tokens[2]->m_data[1]);
    stripped[stringLiteralLen - 2] = '\0';

    getContext()->m_def->m_group = stripped;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <InterfaceDef> ::= pingroups = StringLiteral ;
  void Parser::Rule_InterfaceDef_pingroups_Eq_StringLiteral_Semi(Token *token)
  {
    size_t stringLiteralLen = strlen(token->m_tokens[2]->m_data);
    char *stripped = (char*)_alloca(stringLiteralLen);
    assert(stripped);

    // clip off the " "
    NMP_STRNCPY_S(stripped, stringLiteralLen, &token->m_tokens[2]->m_data[1]);
    stripped[stringLiteralLen - 2] = '\0';

    getContext()->m_def->m_pingroups = stripped;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <InterfaceDef> ::= upgrade = StringLiteral ;
  void Parser::Rule_InterfaceDef_upgrade_Eq_StringLiteral_Semi(Token *token)
  {
    size_t stringLiteralLen = strlen(token->m_tokens[2]->m_data);
    char *stripped = (char*)_alloca(stringLiteralLen);
    assert(stripped);

    // clip off the " "
    NMP_STRNCPY_S(stripped, stringLiteralLen, &token->m_tokens[2]->m_data[1]);
    stripped[stringLiteralLen - 2] = '\0';

    getContext()->m_def->m_upgrade = stripped;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <InterfaceDef> ::= defaultsDisplayFunc = StringLiteral ;
  void Parser::Rule_InterfaceDef_defaultsDisplayFunc_Eq_StringLiteral_Semi(Token *token)
  {
    size_t stringLiteralLen = strlen(token->m_tokens[2]->m_data);
    char *stripped = (char*)_alloca(stringLiteralLen);
    assert(stripped);

    // clip off the " "
    NMP_STRNCPY_S(stripped, stringLiteralLen, &token->m_tokens[2]->m_data[1]);
    stripped[stringLiteralLen - 2] = '\0';

    getContext()->m_def->m_defaultsDisplayFunc = stripped;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <InterfaceDef> ::= displayFunc = StringLiteral ;
  void Parser::Rule_InterfaceDef_displayFunc_Eq_StringLiteral_Semi(Token *token)
  {
    size_t stringLiteralLen = strlen(token->m_tokens[2]->m_data);
    char *stripped = (char*)_alloca(stringLiteralLen);
    assert(stripped);

    // clip off the " "
    NMP_STRNCPY_S(stripped, stringLiteralLen, &token->m_tokens[2]->m_data[1]);
    stripped[stringLiteralLen - 2] = '\0';

    getContext()->m_def->m_displayFunc = stripped;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <InterfaceDef> ::= development = BoolLiteral ;
  void Parser::Rule_InterfaceDef_development_Eq_BoolLiteral_Semi(Token *token)
  {
    bool development = !strcmp(token->m_tokens[2]->m_data, "true");

    getContext()->m_def->m_development = development;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <InterfaceDefs> ::= <InterfaceDef>
  void Parser::Rule_InterfaceDefs(Token *token)
  {
    evaluateTokens(token, reRule_InterfaceDefs);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <InterfaceDefs> ::= <InterfaceDefs> <InterfaceDef>
  void Parser::Rule_InterfaceDefs2(Token *token)
  {
    evaluateTokens(token, reRule_InterfaceDefs2);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <EmittedMessagesDef> ::= IdentifierTerm ; */
  void Parser::Rule_EmittedMessagesDef_IdentifierTerm_Semi(Token *token)
  {
    CheckTokenForReservedKeyword(0);
    SHLApply(0, sIdentifier);

    // add new EmittedMessage
    BehaviourDef::EmittedMessage newMessage(token->m_tokens[0]->m_data);
    newMessage.m_lineDefined = token->m_tokens[0]->m_line;
    getContext()->m_def->m_emittedMessages.push_back(newMessage); 
}

  //--------------------------------------------------------------------------------------------------------------------
  /* <EmittedMessagesDefs> ::= <EmittedMessagesDef> */
  void Parser::Rule_EmittedMessagesDefs(Token *token)
  {
    evaluateTokens(token, reRule_EmittedMessagesDefs);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <EmittedMessagesDefs> ::= <EmittedMessagesDefs> <EmittedMessagesDef> */
  void Parser::Rule_EmittedMessagesDefs2(Token *token)
  {
    evaluateTokens(token, reRule_EmittedMessagesDefs2);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AnimationInputDef> ::= IdentifierTerm IdentifierTerm <CustomOption> ';' */
  void Parser::Rule_AnimationInputDef_IdentifierTerm_IdentifierTerm_Semi(Token *token)
  {
    CheckTokenForReservedKeyword(1);
    SHLApply(0, sTypeName);
    SHLApply(1, sIdentifier);

    BehaviourDef::AnimationInputs& decls = getContext()->m_def->m_animationInputs;
    decls.push_back(BehaviourDef::AnimationInput(token->m_tokens[1]->m_data));
    getContext()->m_animInput = &decls.back();
    getContext()->m_animInput->m_lineDefined = token->m_tokens[3]->m_line;

    // gather modifier options
    getContext()->m_modifierOptionsStack = &getContext()->m_animInput->m_modifiers;
    getContext()->m_modifierOptionsStack->stack.reset();
    executeRule(token->m_tokens[2]);

    // MORPH-21325: validate mods

    getContext()->m_animInput = NULL;
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AnimationDefaults> ::= <AnimationDefault> */
  void Parser::Rule_AnimationDefaults(Token *token)
  {
    evaluateTokens(token, reRule_AnimationDefaults);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AnimationDefaults> ::= <AnimationDefaults> ',' <AnimationDefault> */
  void Parser::Rule_AnimationDefaults_Comma(Token *token)
  {
    evaluateTokens(token, reRule_AnimationDefaults_Comma);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <AnimationInputDef> ::= IdentifierTerm IdentifierTerm = <AnimationDefault> <CustomOption> ;
  void Parser::Rule_AnimationInputDef_IdentifierTerm_IdentifierTerm_Eq_Semi(Token *token)
  {
    CheckTokenForReservedKeyword(1);
    SHLApply(0, sTypeName);
    SHLApply(1, sIdentifier);

    BehaviourDef::AnimationInputs& decls = getContext()->m_def->m_animationInputs;
    decls.push_back(BehaviourDef::AnimationInput(token->m_tokens[1]->m_data));
    getContext()->m_animInput = &decls.back();
    getContext()->m_animInput->m_lineDefined = token->m_tokens[4]->m_line;

    // MORPH-21325: validate mods

    // read defaults
    getContext()->m_dataBlock = (int)MDFBehaviour::Context::ADBDefault;
    getContext()->m_literalsIndex = 0;
    executeRule(token->m_tokens[3]);

    BehaviourDef::AnimationInput *adata = getContext()->m_animInput;
    for (unsigned int i = 0 ; i < getContext()->m_literalsIndex ; i++)
    {
      adata->m_defaults[i] = getContext()->m_literals[i].f;
    }

    // gather modifier options
    getContext()->m_modifierOptionsStack = &getContext()->m_animInput->m_modifiers;
    getContext()->m_modifierOptionsStack->stack.reset();
    executeRule(token->m_tokens[4]);

    getContext()->m_animInput = NULL;
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AnimationDefault> ::= <NumericLiteral> */
  void Parser::Rule_AnimationDefault(Token *token)
  {
    evaluateTokens(token, reRule_AnimationDefault);

    getContext()->m_dataBlock ++;
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AnimationInputDefs> ::= <AnimationInputDef> */
  void Parser::Rule_AnimationInputDefs(Token *token)
  {
    evaluateTokens(token, reRule_AnimationInputDefs);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AnimationInputDefs> ::= <AnimationInputDefs> <AnimationInputDef> */
  void Parser::Rule_AnimationInputDefs2(Token *token)
  {
    evaluateTokens(token, reRule_AnimationInputDefs2);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <ModulesBlock> ::= 'modules:' */
  void Parser::Rule_ModulesBlock_modulesColon(Token *token)
  {
    SHLApply(0, sKeyword2);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <ModulesBlock> ::= 'modules:' <ModuleDefs> */
  void Parser::Rule_ModulesBlock_modulesColon2(Token *token)
  {
    SHLApply(0, sKeyword2);
    evaluateTokens(token, reRule_ModulesBlock_modulesColon2);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AttributesBlock> ::= 'attributes:' */
  void Parser::Rule_AttributesBlock_attributesColon(Token *token)
  {
    SHLApply(0, sKeyword2);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AttributesBlock> ::= 'attributes:' <AttributeGroupDefs> */
  void Parser::Rule_AttributesBlock_attributesColon2(Token *token)
  {
    SHLApply(0, sKeyword2);
    evaluateTokens(token, reRule_AttributesBlock_attributesColon2);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <ControlParamsBlock> ::= 'controlParams:' */
  void Parser::Rule_ControlParamsBlock_controlParamsColon(Token *token)
  {
    SHLApply(0, sKeyword2);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <ControlParamsBlock> ::= 'controlParams:' <ControlParamDefs> */
  void Parser::Rule_ControlParamsBlock_controlParamsColon2(Token *token)
  {
    SHLApply(0, sKeyword2);
    evaluateTokens(token, reRule_ControlParamsBlock_controlParamsColon2);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <OutputControlParamsBlock> ::= 'outputControlParams:' */
  void Parser::Rule_OutputControlParamsBlock_outputControlParamsColon(Token *token)
  {
    SHLApply(0, sKeyword2);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <OutputControlParamsBlock> ::= 'outputControlParams:' <OutputControlParamDefs> */
  void Parser::Rule_OutputControlParamsBlock_outputControlParamsColon2(Token *token)
  {
    SHLApply(0, sKeyword2);
    evaluateTokens(token, reRule_OutputControlParamsBlock_outputControlParamsColon2);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <EmittedMessagesBlock> ::= emittedMessages:
  void Parser::Rule_EmittedMessagesBlock_emittedMessagesColon(Token *token)
  {
    SHLApply(0, sKeyword2);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // <EmittedMessagesBlock> ::= emittedMessages: <EmittedMessagesDefs>
  void Parser::Rule_EmittedMessagesBlock_emittedMessagesColon2(Token *token)
  {
    SHLApply(0, sKeyword2);
    evaluateTokens(token, reRule_EmittedMessagesBlock_emittedMessagesColon2);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AnimInputsBlock> ::= 'animationInputs:' */
  void Parser::Rule_AnimInputsBlock_animationInputsColon(Token *token)
  {
    SHLApply(0, sKeyword2);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <AnimInputsBlock> ::= 'animationInputs:' <AnimationInputDefs> */
  void Parser::Rule_AnimInputsBlock_animationInputsColon2(Token *token)
  {
    SHLApply(0, sKeyword2);
    evaluateTokens(token, reRule_AnimInputsBlock_animationInputsColon2);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <Block> ::= <ModulesBlock> */
  void Parser::Rule_Block(Token *token)
  {
    evaluateTokens(token, reRule_Block);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <Block> ::= <AttributesBlock> */
  void Parser::Rule_Block2(Token *token)
  {
    evaluateTokens(token, reRule_Block2);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <Block> ::= <ControlParamsBlock> */
  void Parser::Rule_Block3(Token *token)
  {
    evaluateTokens(token, reRule_Block3);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <Block> ::= <OutputControlParamsBlock> */
  void Parser::Rule_Block4(Token *token)
  {
    evaluateTokens(token, reRule_Block4);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <Block> ::= <EmittedMessagesBlock> */
  void Parser::Rule_Block5(Token *token)
  {
    evaluateTokens(token, reRule_Block5);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <Block> ::= <AnimInputsBlock> */
  void Parser::Rule_Block6(Token *token)
  {
    evaluateTokens(token, reRule_Block6);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <Block> ::= <InterfaceBlock> */
  void Parser::Rule_Block7(Token *token)
  {
    evaluateTokens(token, reRule_Block7);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <Blocks> ::= <Block> */
  void Parser::Rule_Blocks(Token *token)
  {
    evaluateTokens(token, reRule_Blocks);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <Blocks> ::= <Blocks> <Block> */
  void Parser::Rule_Blocks2(Token *token)
  {
    evaluateTokens(token, reRule_Blocks2);
  }

  //--------------------------------------------------------------------------------------------------------------------
  /* <BehaviourDefinition> ::= <CustomOption> 'behaviour_def' IdentifierTerm '{' <Blocks> '}' */
  void Parser::Rule_BehaviourDefinition_behaviour_def_IdentifierTerm_LBrace_RBrace(Token *token)
  {
    CheckTokenForReservedKeyword(2);
    SHLApply(1, sKeyword2);
    SHLApply(2, sTypeName);

    // gather modifier options
    getContext()->m_modifierOptionsStack = &getContext()->m_def->m_modifiers;
    getContext()->m_modifierOptionsStack->stack.reset();
    getContext()->m_def->m_lineDefined = token->m_tokens[0]->m_line;
    executeRule(token->m_tokens[0]);

    // MORPH-21325: validate mods

    // store name
    getContext()->m_def->m_name = token->m_tokens[2]->m_data;

    // go walk the def
    executeRule(token->m_tokens[4]);

    SHLMarkBlock(token->m_tokens[1]->m_line, token->m_tokens[5]->m_line);
  }

  //--------------------------------------------------------------------------------------------------------------------
  void Parser::processSavedComments(const SavedComments &savedComments)
  {
    const char* scData;
    
    if (savedComments.find(getContext()->m_def->m_lineDefined, &scData))
    {
      MDFBehaviour::Context* context = getContext();
      context->m_def->m_shortDesc.set(stripWhiteSpace(scData));
    }

    // walk attributes
    iterate_begin(BehaviourDef::AttributeGroup, getContext()->m_def->m_attributeGroups, ag)
    {
      if (savedComments.find(ag.m_lineDefined, &scData))
      {
        ag.m_comment.set(stripWhiteSpace(scData));
      }

      iterate_begin(BehaviourDef::AttributeVariable, ag.m_vars, av)
      {
        if (savedComments.find(av.m_lineDefined, &scData))
        {
          av.m_comment.set(stripWhiteSpace(scData));
        }
      }
      iterate_end

      // Enumerations handling.
      iterate_begin(BehaviourDef::AttributeGroup::EnumDecl, ag.m_enumDecl, enm)
      {
        // Enum main comment.
        if (savedComments.find(enm.m_lineDefined, &scData))
        {
          enm.m_comment.set(stripWhiteSpace(scData));
        }

        // Iterate enum entries to find any comments attached to them.
        EnumDeclBase::Entries::const_iterator eIt = enm.m_entries.begin();
        EnumDeclBase::Entries::const_iterator eEnd = enm.m_entries.end();

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
    iterate_end

    iterate_begin(BehaviourDef::AnimationInput, getContext()->m_def->m_animationInputs, ain)
    {
      if (savedComments.find(ain.m_lineDefined, &scData))
      {
        ain.m_comment.set(stripWhiteSpace(scData));
      }
    }
    iterate_end

    iterate_begin(BehaviourDef::ControlParamGroup, getContext()->m_def->m_controlParamGroups, cg)
    {
      if (savedComments.find(cg.m_lineDefined, &scData))
      {
        cg.m_comment.set(stripWhiteSpace(scData));
      }

      iterate_begin(BehaviourDef::ControlParam, cg.m_params, cp)
      {
        if (savedComments.find(cp.m_lineDefined, &scData))
        {
          cp.m_comment.set(stripWhiteSpace(scData));
        }
      }
      iterate_end
    }
    iterate_end

    iterate_begin(BehaviourDef::OutputControlParam, getContext()->m_def->m_outputControlParams, cp)
    {
      if (savedComments.find(cp.m_lineDefined, &scData))
      {
        cp.m_comment.set(stripWhiteSpace(scData));
      }
    }
    iterate_end

    iterate_begin(BehaviourDef::EmittedMessage, getContext()->m_def->m_emittedMessages, em)
    {
      if (savedComments.find(em.m_lineDefined, &scData))
      {
        em.m_comment.set(stripWhiteSpace(scData));
      }
    }
    iterate_end
  }
  
} // namespace 
