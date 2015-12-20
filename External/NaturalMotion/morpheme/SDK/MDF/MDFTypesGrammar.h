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
 
/*
 * --- THIS FILE IS GENERATED FROM GOLD GRAMMAR DEFINITIONS ---
 *       To edit, change the PGT templates and re-export!
 */

#include "ParserDefs.h"
#include "ParserBase.h"
#include <assert.h>

class MDFFeedbackInterface;
class MDFSyntaxHighlightingInterface;

/*
 * MDFTypesGrammar Grammar, Version 1
 * NaturalMotion ltd.
 */
namespace MDFTypes
{
  struct Context;
  
  // --------------------------------------------------------------------
  // Symbol IDs
  // --------------------------------------------------------------------

  enum SymbolID
  {
    /*  0 */  Symbol_EOF,
    /*  1 */  Symbol_Error,
    /*  2 */  Symbol_Whitespace,
    /*  3 */  Symbol_CommentEnd,
    /*  4 */  Symbol_CommentLine,
    /*  5 */  Symbol_CommentStart,
    /*  6 */  Symbol_Minus,
    /*  7 */  Symbol_LParan,
    /*  8 */  Symbol_RParan,
    /*  9 */  Symbol_Comma,
    /* 10 */  Symbol_Colon,
    /* 11 */  Symbol_ColonColon,
    /* 12 */  Symbol_Semi,
    /* 13 */  Symbol_LBracket,
    /* 14 */  Symbol_RBracket,
    /* 15 */  Symbol_LBrace,
    /* 16 */  Symbol_RBrace,
    /* 17 */  Symbol_RBraceSemi,
    /* 18 */  Symbol_Plus,
    /* 19 */  Symbol_Eq,
    /* 20 */  Symbol_EqGt,
    /* 21 */  Symbol_alias,
    /* 22 */  Symbol_ArraySpecifier,
    /* 23 */  Symbol_DecLiteral,
    /* 24 */  Symbol_enum,
    /* 25 */  Symbol_FloatLiteral,
    /* 26 */  Symbol_IdentifierTerm,
    /* 27 */  Symbol_IdRef,
    /* 28 */  Symbol_IdStar,
    /* 29 */  Symbol_ModKeyword,
    /* 30 */  Symbol_namespace,
    /* 31 */  Symbol_privateColon,
    /* 32 */  Symbol_publicColon,
    /* 33 */  Symbol_StringLiteral,
    /* 34 */  Symbol_struct,
    /* 35 */  Symbol_TSizeSpecifier,
    /* 36 */  Symbol_typedef,
    /* 37 */  Symbol_CustomOption,
    /* 38 */  Symbol_CustomOptionKeyword,
    /* 39 */  Symbol_CustomOptions,
    /* 40 */  Symbol_DataBlockDef,
    /* 41 */  Symbol_DataBlockDefs,
    /* 42 */  Symbol_Enum2,
    /* 43 */  Symbol_EnumExpr,
    /* 44 */  Symbol_EnumExprValue,
    /* 45 */  Symbol_Enums,
    /* 46 */  Symbol_FnArg,
    /* 47 */  Symbol_FnArgs,
    /* 48 */  Symbol_FnDecl,
    /* 49 */  Symbol_FnIsConst,
    /* 50 */  Symbol_FnLiteral,
    /* 51 */  Symbol_FnLiterals,
    /* 52 */  Symbol_FnName,
    /* 53 */  Symbol_FnNameAndReturn,
    /* 54 */  Symbol_FnNumLiteral,
    /* 55 */  Symbol_Root,
    /* 56 */  Symbol_Roots,
    /* 57 */  Symbol_StructInheritance,
    /* 58 */  Symbol_TypesRoots,
    /* 59 */  Symbol_VarCascade,
    /* 60 */  Symbol_VarDecl,
    /* 61 */  Symbol_VarIdentifier,
    /* 62 */  Symbol_VarSpec,
    /* 63 */  Symbol_VarSpecs,
    /* 64 */  Symbol_VarType,
    /* 65 */  Symbol_VarTypeList 
  };



  // --------------------------------------------------------------------
  // Rule IDs
  // --------------------------------------------------------------------

  enum RuleEnum
  {
    reRule_CustomOptionKeyword_ModKeyword,
    reRule_CustomOptionKeyword_ModKeyword_LParan_IdentifierTerm_RParan,
    reRule_CustomOptionKeyword_ModKeyword_LParan_StringLiteral_RParan,
    reRule_CustomOptionKeyword_ModKeyword_LParan_DecLiteral_RParan,
    reRule_VarIdentifier_IdStar_IdentifierTerm,
    reRule_VarIdentifier_IdRef_IdentifierTerm,
    reRule_VarIdentifier_IdentifierTerm,
    reRule_VarCascade_IdentifierTerm_ColonColon,
    reRule_VarCascade_IdentifierTerm,
    reRule_VarType,
    reRule_VarType2,
    reRule_VarTypeList,
    reRule_VarTypeList2,
    reRule_VarSpec,
    reRule_VarSpec_ArraySpecifier,
    reRule_VarSpec_LBracket_IdentifierTerm_RBracket,
    reRule_VarSpecs,
    reRule_VarSpecs_Comma,
    reRule_EnumExprValue_DecLiteral,
    reRule_EnumExprValue_IdentifierTerm,
    reRule_EnumExpr_Plus,
    reRule_EnumExpr_Minus,
    reRule_EnumExpr_IdStar,
    reRule_EnumExpr,
    reRule_Enum_IdentifierTerm,
    reRule_Enum_IdentifierTerm_Eq,
    reRule_Enums,
    reRule_Enums_Comma,
    reRule_VarDecl_Semi,
    reRule_VarDecl_enum_IdentifierTerm_LBrace_RBraceSemi,
    reRule_FnIsConst_IdentifierTerm,
    reRule_FnIsConst,
    reRule_FnNumLiteral_FloatLiteral,
    reRule_FnNumLiteral_DecLiteral,
    reRule_FnLiteral,
    reRule_FnLiteral_IdentifierTerm,
    reRule_FnLiterals,
    reRule_FnLiterals_Comma,
    reRule_FnArg,
    reRule_FnArg_Eq_LParan_RParan,
    reRule_FnArg_Eq,
    reRule_FnArgs,
    reRule_FnArgs_Comma,
    reRule_FnArgs2,
    reRule_FnName_IdStar_IdentifierTerm,
    reRule_FnName_IdRef_IdentifierTerm,
    reRule_FnName_IdentifierTerm,
    reRule_FnNameAndReturn,
    reRule_FnNameAndReturn2,
    reRule_FnDecl_Plus_LParan_RParan_Semi,
    reRule_DataBlockDef,
    reRule_DataBlockDef2,
    reRule_DataBlockDef_privateColon,
    reRule_DataBlockDef_publicColon,
    reRule_DataBlockDefs,
    reRule_DataBlockDefs2,
    reRule_CustomOptions,
    reRule_CustomOptions2,
    reRule_CustomOption,
    reRule_CustomOption2,
    reRule_StructInheritance_Colon,
    reRule_StructInheritance,
    reRule_Root_namespace_IdentifierTerm_LBrace_RBrace,
    reRule_Root_struct_IdentifierTerm_LBrace_RBraceSemi,
    reRule_Root_alias_EqGt_IdentifierTerm_Semi,
    reRule_Root_typedef_TSizeSpecifier_Semi,
    reRule_Roots,
    reRule_Roots2,
    reRule_TypesRoots 
  };


  // --------------------------------------------------------------------
  // 
  // 
  // --------------------------------------------------------------------

  class Parser : public ParserBase
  {
  private:

    // <CustomOptionKeyword> ::= ModKeyword
    void Rule_CustomOptionKeyword_ModKeyword(Token *token);

    // <CustomOptionKeyword> ::= ModKeyword ( IdentifierTerm )
    void Rule_CustomOptionKeyword_ModKeyword_LParan_IdentifierTerm_RParan(Token *token);

    // <CustomOptionKeyword> ::= ModKeyword ( StringLiteral )
    void Rule_CustomOptionKeyword_ModKeyword_LParan_StringLiteral_RParan(Token *token);

    // <CustomOptionKeyword> ::= ModKeyword ( DecLiteral )
    void Rule_CustomOptionKeyword_ModKeyword_LParan_DecLiteral_RParan(Token *token);

    // <VarIdentifier> ::= IdStar IdentifierTerm
    void Rule_VarIdentifier_IdStar_IdentifierTerm(Token *token);

    // <VarIdentifier> ::= IdRef IdentifierTerm
    void Rule_VarIdentifier_IdRef_IdentifierTerm(Token *token);

    // <VarIdentifier> ::= IdentifierTerm
    void Rule_VarIdentifier_IdentifierTerm(Token *token);

    // <VarCascade> ::= IdentifierTerm :: <VarCascade>
    void Rule_VarCascade_IdentifierTerm_ColonColon(Token *token);

    // <VarCascade> ::= IdentifierTerm
    void Rule_VarCascade_IdentifierTerm(Token *token);

    // <VarType> ::= <VarCascade>
    void Rule_VarType(Token *token);

    // <VarType> ::= <CustomOptionKeyword>
    void Rule_VarType2(Token *token);

    // <VarTypeList> ::= <VarType>
    void Rule_VarTypeList(Token *token);

    // <VarTypeList> ::= <VarTypeList> <VarType>
    void Rule_VarTypeList2(Token *token);

    // <VarSpec> ::= <VarIdentifier>
    void Rule_VarSpec(Token *token);

    // <VarSpec> ::= <VarIdentifier> ArraySpecifier
    void Rule_VarSpec_ArraySpecifier(Token *token);

    // <VarSpec> ::= <VarIdentifier> [ IdentifierTerm ]
    void Rule_VarSpec_LBracket_IdentifierTerm_RBracket(Token *token);

    // <VarSpecs> ::= <VarSpec>
    void Rule_VarSpecs(Token *token);

    // <VarSpecs> ::= <VarSpecs> , <VarSpec>
    void Rule_VarSpecs_Comma(Token *token);

    // <EnumExprValue> ::= DecLiteral
    void Rule_EnumExprValue_DecLiteral(Token *token);

    // <EnumExprValue> ::= IdentifierTerm
    void Rule_EnumExprValue_IdentifierTerm(Token *token);

    // <EnumExpr> ::= <EnumExpr> + <EnumExprValue>
    void Rule_EnumExpr_Plus(Token *token);

    // <EnumExpr> ::= <EnumExpr> - <EnumExprValue>
    void Rule_EnumExpr_Minus(Token *token);

    // <EnumExpr> ::= <EnumExpr> IdStar <EnumExprValue>
    void Rule_EnumExpr_IdStar(Token *token);

    // <EnumExpr> ::= <EnumExprValue>
    void Rule_EnumExpr(Token *token);

    // <Enum> ::= IdentifierTerm
    void Rule_Enum_IdentifierTerm(Token *token);

    // <Enum> ::= IdentifierTerm = <EnumExpr>
    void Rule_Enum_IdentifierTerm_Eq(Token *token);

    // <Enums> ::= <Enum>
    void Rule_Enums(Token *token);

    // <Enums> ::= <Enums> , <Enum>
    void Rule_Enums_Comma(Token *token);

    // <VarDecl> ::= <VarTypeList> <VarSpecs> ;
    void Rule_VarDecl_Semi(Token *token);

    // <VarDecl> ::= enum IdentifierTerm { <Enums> };
    void Rule_VarDecl_enum_IdentifierTerm_LBrace_RBraceSemi(Token *token);

    // <FnIsConst> ::= IdentifierTerm
    void Rule_FnIsConst_IdentifierTerm(Token *token);

    // <FnIsConst> ::= 
    void Rule_FnIsConst(Token *token);

    // <FnNumLiteral> ::= FloatLiteral
    void Rule_FnNumLiteral_FloatLiteral(Token *token);

    // <FnNumLiteral> ::= DecLiteral
    void Rule_FnNumLiteral_DecLiteral(Token *token);

    // <FnLiteral> ::= <FnNumLiteral>
    void Rule_FnLiteral(Token *token);

    // <FnLiteral> ::= IdentifierTerm
    void Rule_FnLiteral_IdentifierTerm(Token *token);

    // <FnLiterals> ::= <FnLiteral>
    void Rule_FnLiterals(Token *token);

    // <FnLiterals> ::= <FnLiterals> , <FnLiteral>
    void Rule_FnLiterals_Comma(Token *token);

    // <FnArg> ::= <VarTypeList> <VarIdentifier>
    void Rule_FnArg(Token *token);

    // <FnArg> ::= <VarTypeList> <VarIdentifier> = <VarCascade> ( <FnLiterals> )
    void Rule_FnArg_Eq_LParan_RParan(Token *token);

    // <FnArg> ::= <VarTypeList> <VarIdentifier> = <FnNumLiteral>
    void Rule_FnArg_Eq(Token *token);

    // <FnArgs> ::= <FnArg>
    void Rule_FnArgs(Token *token);

    // <FnArgs> ::= <FnArgs> , <FnArg>
    void Rule_FnArgs_Comma(Token *token);

    // <FnArgs> ::= 
    void Rule_FnArgs2(Token *token);

    // <FnName> ::= IdStar IdentifierTerm
    void Rule_FnName_IdStar_IdentifierTerm(Token *token);

    // <FnName> ::= IdRef IdentifierTerm
    void Rule_FnName_IdRef_IdentifierTerm(Token *token);

    // <FnName> ::= IdentifierTerm
    void Rule_FnName_IdentifierTerm(Token *token);

    // <FnNameAndReturn> ::= <VarTypeList> <FnName>
    void Rule_FnNameAndReturn(Token *token);

    // <FnNameAndReturn> ::= <FnName>
    void Rule_FnNameAndReturn2(Token *token);

    // <FnDecl> ::= + <FnNameAndReturn> ( <FnArgs> ) <FnIsConst> ;
    void Rule_FnDecl_Plus_LParan_RParan_Semi(Token *token);

    // <DataBlockDef> ::= <VarDecl>
    void Rule_DataBlockDef(Token *token);

    // <DataBlockDef> ::= <FnDecl>
    void Rule_DataBlockDef2(Token *token);

    // <DataBlockDef> ::= private:
    void Rule_DataBlockDef_privateColon(Token *token);

    // <DataBlockDef> ::= public:
    void Rule_DataBlockDef_publicColon(Token *token);

    // <DataBlockDefs> ::= <DataBlockDef>
    void Rule_DataBlockDefs(Token *token);

    // <DataBlockDefs> ::= <DataBlockDefs> <DataBlockDef>
    void Rule_DataBlockDefs2(Token *token);

    // <CustomOptions> ::= <CustomOptionKeyword>
    void Rule_CustomOptions(Token *token);

    // <CustomOptions> ::= <CustomOptions> <CustomOptionKeyword>
    void Rule_CustomOptions2(Token *token);

    // <CustomOption> ::= <CustomOptions>
    void Rule_CustomOption(Token *token);

    // <CustomOption> ::= 
    void Rule_CustomOption2(Token *token);

    // <StructInheritance> ::= : <VarCascade>
    void Rule_StructInheritance_Colon(Token *token);

    // <StructInheritance> ::= 
    void Rule_StructInheritance(Token *token);

    // <Root> ::= namespace IdentifierTerm { <Roots> }
    void Rule_Root_namespace_IdentifierTerm_LBrace_RBrace(Token *token);

    // <Root> ::= <CustomOption> struct IdentifierTerm <StructInheritance> { <DataBlockDefs> };
    void Rule_Root_struct_IdentifierTerm_LBrace_RBraceSemi(Token *token);

    // <Root> ::= <CustomOption> alias <VarCascade> => IdentifierTerm <CustomOption> ;
    void Rule_Root_alias_EqGt_IdentifierTerm_Semi(Token *token);

    // <Root> ::= <CustomOption> typedef <VarCascade> TSizeSpecifier <CustomOption> ;
    void Rule_Root_typedef_TSizeSpecifier_Semi(Token *token);

    // <Roots> ::= <Root>
    void Rule_Roots(Token *token);

    // <Roots> ::= <Roots> <Root>
    void Rule_Roots2(Token *token);

    // <TypesRoots> ::= <Roots>
    void Rule_TypesRoots(Token *token);

    typedef void (Parser::*RuleHandler)(Token *token);
    static const RuleHandler gRuleJumpTable[69];

    // call to jump to a given token's rule handler; must only be called on
    // tokens that have a reduction rule
    virtual void executeRule(Token *token)
    {
      assert(token->m_reductionRule >= 0);
      m_trackingLastHitLine = token->m_line;
      (*this.*gRuleJumpTable[token->m_reductionRule])(token);
    }

    virtual void processSavedComments(const SavedComments &savedComments);
    
    inline MDFTypes::Context* getContext() { return reinterpret_cast<MDFTypes::Context*>(m_context); }

    virtual void recursiveFunctionDeclarationConcat(const Grammar& grammar, Token *token, unsigned int startToken);
    
public:

    static const Grammar sGrammar;

    Parser(MDFFeedbackInterface* fbi, MDFSyntaxHighlightingInterface *shi) : ParserBase(sGrammar, fbi, shi) {}
};

} // namespace MDFTypes

