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
 * MDFModuleGrammar Grammar, Version 3
 * NaturalMotion ltd.
 */
namespace MDFModule
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
    /*  7 */  Symbol_Dollar,
    /*  8 */  Symbol_LParan,
    /*  9 */  Symbol_RParan,
    /* 10 */  Symbol_Comma,
    /* 11 */  Symbol_Dot,
    /* 12 */  Symbol_ColonColon,
    /* 13 */  Symbol_Semi,
    /* 14 */  Symbol_LBracket,
    /* 15 */  Symbol_LBracketTimesRBracket,
    /* 16 */  Symbol_LBracketRBracket,
    /* 17 */  Symbol_RBracket,
    /* 18 */  Symbol_LBrace,
    /* 19 */  Symbol_RBrace,
    /* 20 */  Symbol_RBraceSemi,
    /* 21 */  Symbol_Plus,
    /* 22 */  Symbol_Eq,
    /* 23 */  Symbol_GtColon,
    /* 24 */  Symbol_ArraySpecifier,
    /* 25 */  Symbol_childrenColon,
    /* 26 */  Symbol_connectionsLt,
    /* 27 */  Symbol_dataColon,
    /* 28 */  Symbol_DecLiteral,
    /* 29 */  Symbol_enum,
    /* 30 */  Symbol_feedInColon,
    /* 31 */  Symbol_feedOutColon,
    /* 32 */  Symbol_FloatLiteral,
    /* 33 */  Symbol_IdentifierSku,
    /* 34 */  Symbol_IdentifierTerm,
    /* 35 */  Symbol_IdRef,
    /* 36 */  Symbol_IdStar,
    /* 37 */  Symbol_inColon,
    /* 38 */  Symbol_ModKeyword,
    /* 39 */  Symbol_module_def,
    /* 40 */  Symbol_outColon,
    /* 41 */  Symbol_ownerColon,
    /* 42 */  Symbol_privateAPIColon,
    /* 43 */  Symbol_publicAPIColon,
    /* 44 */  Symbol_StringLiteral,
    /* 45 */  Symbol_userInColon,
    /* 46 */  Symbol_ArrayLinkPath,
    /* 47 */  Symbol_Block,
    /* 48 */  Symbol_Blocks,
    /* 49 */  Symbol_ChildDecl,
    /* 50 */  Symbol_ChildDef,
    /* 51 */  Symbol_ChildDefs,
    /* 52 */  Symbol_ChildrenBlock,
    /* 53 */  Symbol_Connection,
    /* 54 */  Symbol_ConnectionEntries,
    /* 55 */  Symbol_ConnectionEntry,
    /* 56 */  Symbol_ConnectionLink,
    /* 57 */  Symbol_ConnectionName,
    /* 58 */  Symbol_ConnectionNewJnc,
    /* 59 */  Symbol_ConnectionWiring,
    /* 60 */  Symbol_ConnectionWiringList,
    /* 61 */  Symbol_ConnectionWiringStart,
    /* 62 */  Symbol_CustomOption,
    /* 63 */  Symbol_CustomOptionKeyword,
    /* 64 */  Symbol_CustomOptions,
    /* 65 */  Symbol_DataBlock,
    /* 66 */  Symbol_DataBlockDefs,
    /* 67 */  Symbol_Enum2,
    /* 68 */  Symbol_EnumExpr,
    /* 69 */  Symbol_EnumExprValue,
    /* 70 */  Symbol_Enums,
    /* 71 */  Symbol_FeedInputsBlock,
    /* 72 */  Symbol_FeedOutputsBlock,
    /* 73 */  Symbol_FnArg,
    /* 74 */  Symbol_FnArgs,
    /* 75 */  Symbol_FnDecl,
    /* 76 */  Symbol_FnDecls,
    /* 77 */  Symbol_FnIsConst,
    /* 78 */  Symbol_FnLiteral,
    /* 79 */  Symbol_FnLiterals,
    /* 80 */  Symbol_FnName,
    /* 81 */  Symbol_FnNameAndReturn,
    /* 82 */  Symbol_FnNumLiteral,
    /* 83 */  Symbol_Identifier,
    /* 84 */  Symbol_InputsBlock,
    /* 85 */  Symbol_ModuleDefinition,
    /* 86 */  Symbol_OutputsBlock,
    /* 87 */  Symbol_OwnerBlock,
    /* 88 */  Symbol_PrivateFunctionsBlock,
    /* 89 */  Symbol_PublicFunctionsBlock,
    /* 90 */  Symbol_UserInputsBlock,
    /* 91 */  Symbol_VarCascade,
    /* 92 */  Symbol_VarDecl,
    /* 93 */  Symbol_VarIdentifier,
    /* 94 */  Symbol_VarSpec,
    /* 95 */  Symbol_VarType,
    /* 96 */  Symbol_VarTypeList 
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
    reRule_CustomOptions,
    reRule_CustomOptions2,
    reRule_CustomOption,
    reRule_CustomOption2,
    reRule_Identifier_IdentifierTerm,
    reRule_Identifier_IdentifierTerm_IdentifierSku,
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
    reRule_DataBlockDefs,
    reRule_DataBlockDefs2,
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
    reRule_FnDecl_LParan_RParan_Semi,
    reRule_FnDecls,
    reRule_FnDecls2,
    reRule_ChildDecl_IdentifierTerm,
    reRule_ChildDecl_IdentifierTerm_ArraySpecifier,
    reRule_ChildDecl_IdentifierTerm_LBracket_IdentifierTerm_RBracket,
    reRule_ChildDef_Semi,
    reRule_ChildDefs,
    reRule_ChildDefs2,
    reRule_OwnerBlock_ownerColon,
    reRule_OwnerBlock_ownerColon2,
    reRule_DataBlock_dataColon,
    reRule_DataBlock_dataColon2,
    reRule_FeedInputsBlock_feedInColon,
    reRule_FeedInputsBlock_feedInColon2,
    reRule_InputsBlock_inColon,
    reRule_InputsBlock_inColon2,
    reRule_UserInputsBlock_userInColon,
    reRule_UserInputsBlock_userInColon2,
    reRule_FeedOutputsBlock_feedOutColon,
    reRule_FeedOutputsBlock_feedOutColon2,
    reRule_OutputsBlock_outColon,
    reRule_OutputsBlock_outColon2,
    reRule_PrivateFunctionsBlock_privateAPIColon,
    reRule_PrivateFunctionsBlock_privateAPIColon2,
    reRule_PublicFunctionsBlock_publicAPIColon,
    reRule_PublicFunctionsBlock_publicAPIColon2,
    reRule_ChildrenBlock_childrenColon,
    reRule_ChildrenBlock_childrenColon2,
    reRule_ArrayLinkPath_Dollar_IdentifierTerm,
    reRule_ArrayLinkPath_IdentifierTerm_Dot,
    reRule_ConnectionLink_IdentifierTerm,
    reRule_ConnectionLink_IdentifierTerm_ArraySpecifier,
    reRule_ConnectionLink_IdentifierTerm_LBracketTimesRBracket,
    reRule_ConnectionLink_IdentifierTerm_LBracketRBracket,
    reRule_ConnectionLink_IdentifierTerm_LBracket_RBracket,
    reRule_ConnectionWiring,
    reRule_ConnectionWiring_Dot,
    reRule_ConnectionWiringStart_Dot,
    reRule_ConnectionWiringList,
    reRule_ConnectionWiringList_Comma,
    reRule_ConnectionNewJnc_IdentifierTerm_LParan_RParan,
    reRule_ConnectionEntry_Eq_Semi,
    reRule_ConnectionEntry_Eq_Semi2,
    reRule_ConnectionEntry_Eq_Eq_Semi,
    reRule_ConnectionEntry_Eq_LParan_RParan_Semi,
    reRule_ConnectionEntries,
    reRule_ConnectionEntries2,
    reRule_ConnectionName_IdentifierTerm,
    reRule_ConnectionName,
    reRule_Connection_connectionsLt_GtColon,
    reRule_Block,
    reRule_Block2,
    reRule_Block3,
    reRule_Block4,
    reRule_Block5,
    reRule_Block6,
    reRule_Block7,
    reRule_Block8,
    reRule_Block9,
    reRule_Block10,
    reRule_Block11,
    reRule_Blocks,
    reRule_Blocks2,
    reRule_ModuleDefinition_module_def_IdentifierTerm_LBrace_RBrace 
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

    // <CustomOptions> ::= <CustomOptionKeyword>
    void Rule_CustomOptions(Token *token);

    // <CustomOptions> ::= <CustomOptions> <CustomOptionKeyword>
    void Rule_CustomOptions2(Token *token);

    // <CustomOption> ::= <CustomOptions>
    void Rule_CustomOption(Token *token);

    // <CustomOption> ::= 
    void Rule_CustomOption2(Token *token);

    // <Identifier> ::= IdentifierTerm
    void Rule_Identifier_IdentifierTerm(Token *token);

    // <Identifier> ::= IdentifierTerm IdentifierSku
    void Rule_Identifier_IdentifierTerm_IdentifierSku(Token *token);

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

    // <VarDecl> ::= <VarTypeList> <VarSpec> ;
    void Rule_VarDecl_Semi(Token *token);

    // <VarDecl> ::= enum IdentifierTerm { <Enums> };
    void Rule_VarDecl_enum_IdentifierTerm_LBrace_RBraceSemi(Token *token);

    // <DataBlockDefs> ::= <VarDecl>
    void Rule_DataBlockDefs(Token *token);

    // <DataBlockDefs> ::= <DataBlockDefs> <VarDecl>
    void Rule_DataBlockDefs2(Token *token);

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

    // <FnDecl> ::= <FnNameAndReturn> ( <FnArgs> ) <FnIsConst> ;
    void Rule_FnDecl_LParan_RParan_Semi(Token *token);

    // <FnDecls> ::= <FnDecl>
    void Rule_FnDecls(Token *token);

    // <FnDecls> ::= <FnDecls> <FnDecl>
    void Rule_FnDecls2(Token *token);

    // <ChildDecl> ::= IdentifierTerm
    void Rule_ChildDecl_IdentifierTerm(Token *token);

    // <ChildDecl> ::= IdentifierTerm ArraySpecifier
    void Rule_ChildDecl_IdentifierTerm_ArraySpecifier(Token *token);

    // <ChildDecl> ::= IdentifierTerm [ IdentifierTerm ]
    void Rule_ChildDecl_IdentifierTerm_LBracket_IdentifierTerm_RBracket(Token *token);

    // <ChildDef> ::= <CustomOption> <Identifier> <ChildDecl> <CustomOption> ;
    void Rule_ChildDef_Semi(Token *token);

    // <ChildDefs> ::= <ChildDef>
    void Rule_ChildDefs(Token *token);

    // <ChildDefs> ::= <ChildDefs> <ChildDef>
    void Rule_ChildDefs2(Token *token);

    // <OwnerBlock> ::= owner:
    void Rule_OwnerBlock_ownerColon(Token *token);

    // <OwnerBlock> ::= owner: <Identifier>
    void Rule_OwnerBlock_ownerColon2(Token *token);

    // <DataBlock> ::= data:
    void Rule_DataBlock_dataColon(Token *token);

    // <DataBlock> ::= data: <DataBlockDefs>
    void Rule_DataBlock_dataColon2(Token *token);

    // <FeedInputsBlock> ::= feedIn:
    void Rule_FeedInputsBlock_feedInColon(Token *token);

    // <FeedInputsBlock> ::= feedIn: <DataBlockDefs>
    void Rule_FeedInputsBlock_feedInColon2(Token *token);

    // <InputsBlock> ::= in:
    void Rule_InputsBlock_inColon(Token *token);

    // <InputsBlock> ::= in: <DataBlockDefs>
    void Rule_InputsBlock_inColon2(Token *token);

    // <UserInputsBlock> ::= userIn:
    void Rule_UserInputsBlock_userInColon(Token *token);

    // <UserInputsBlock> ::= userIn: <DataBlockDefs>
    void Rule_UserInputsBlock_userInColon2(Token *token);

    // <FeedOutputsBlock> ::= feedOut:
    void Rule_FeedOutputsBlock_feedOutColon(Token *token);

    // <FeedOutputsBlock> ::= feedOut: <DataBlockDefs>
    void Rule_FeedOutputsBlock_feedOutColon2(Token *token);

    // <OutputsBlock> ::= out:
    void Rule_OutputsBlock_outColon(Token *token);

    // <OutputsBlock> ::= out: <DataBlockDefs>
    void Rule_OutputsBlock_outColon2(Token *token);

    // <PrivateFunctionsBlock> ::= privateAPI:
    void Rule_PrivateFunctionsBlock_privateAPIColon(Token *token);

    // <PrivateFunctionsBlock> ::= privateAPI: <FnDecls>
    void Rule_PrivateFunctionsBlock_privateAPIColon2(Token *token);

    // <PublicFunctionsBlock> ::= publicAPI:
    void Rule_PublicFunctionsBlock_publicAPIColon(Token *token);

    // <PublicFunctionsBlock> ::= publicAPI: <FnDecls>
    void Rule_PublicFunctionsBlock_publicAPIColon2(Token *token);

    // <ChildrenBlock> ::= children:
    void Rule_ChildrenBlock_childrenColon(Token *token);

    // <ChildrenBlock> ::= children: <ChildDefs>
    void Rule_ChildrenBlock_childrenColon2(Token *token);

    // <ArrayLinkPath> ::= $ IdentifierTerm
    void Rule_ArrayLinkPath_Dollar_IdentifierTerm(Token *token);

    // <ArrayLinkPath> ::= IdentifierTerm . <ArrayLinkPath>
    void Rule_ArrayLinkPath_IdentifierTerm_Dot(Token *token);

    // <ConnectionLink> ::= IdentifierTerm
    void Rule_ConnectionLink_IdentifierTerm(Token *token);

    // <ConnectionLink> ::= IdentifierTerm ArraySpecifier
    void Rule_ConnectionLink_IdentifierTerm_ArraySpecifier(Token *token);

    // <ConnectionLink> ::= IdentifierTerm [*]
    void Rule_ConnectionLink_IdentifierTerm_LBracketTimesRBracket(Token *token);

    // <ConnectionLink> ::= IdentifierTerm []
    void Rule_ConnectionLink_IdentifierTerm_LBracketRBracket(Token *token);

    // <ConnectionLink> ::= IdentifierTerm [ <ArrayLinkPath> ]
    void Rule_ConnectionLink_IdentifierTerm_LBracket_RBracket(Token *token);

    // <ConnectionWiring> ::= <ConnectionLink>
    void Rule_ConnectionWiring(Token *token);

    // <ConnectionWiring> ::= <ConnectionLink> . <ConnectionWiring>
    void Rule_ConnectionWiring_Dot(Token *token);

    // <ConnectionWiringStart> ::= <ConnectionLink> . <ConnectionWiring>
    void Rule_ConnectionWiringStart_Dot(Token *token);

    // <ConnectionWiringList> ::= <ConnectionWiringStart>
    void Rule_ConnectionWiringList(Token *token);

    // <ConnectionWiringList> ::= <ConnectionWiringList> , <ConnectionWiringStart>
    void Rule_ConnectionWiringList_Comma(Token *token);

    // <ConnectionNewJnc> ::= IdentifierTerm ( <ConnectionWiringList> )
    void Rule_ConnectionNewJnc_IdentifierTerm_LParan_RParan(Token *token);

    // <ConnectionEntry> ::= <ConnectionWiringStart> = <ConnectionNewJnc> <CustomOption> ;
    void Rule_ConnectionEntry_Eq_Semi(Token *token);

    // <ConnectionEntry> ::= <ConnectionWiringStart> = <ConnectionWiringStart> <CustomOption> ;
    void Rule_ConnectionEntry_Eq_Semi2(Token *token);

    // <ConnectionEntry> ::= <ConnectionWiringStart> = <ConnectionWiringStart> = <ConnectionNewJnc> <CustomOption> ;
    void Rule_ConnectionEntry_Eq_Eq_Semi(Token *token);

    // <ConnectionEntry> ::= <ConnectionWiringStart> = ( <ConnectionWiringStart> ) <CustomOption> ;
    void Rule_ConnectionEntry_Eq_LParan_RParan_Semi(Token *token);

    // <ConnectionEntries> ::= <ConnectionEntry>
    void Rule_ConnectionEntries(Token *token);

    // <ConnectionEntries> ::= <ConnectionEntries> <ConnectionEntry>
    void Rule_ConnectionEntries2(Token *token);

    // <ConnectionName> ::= IdentifierTerm
    void Rule_ConnectionName_IdentifierTerm(Token *token);

    // <ConnectionName> ::= 
    void Rule_ConnectionName(Token *token);

    // <Connection> ::= connections< <ConnectionName> >: <CustomOption> <ConnectionEntries>
    void Rule_Connection_connectionsLt_GtColon(Token *token);

    // <Block> ::= <OwnerBlock>
    void Rule_Block(Token *token);

    // <Block> ::= <DataBlock>
    void Rule_Block2(Token *token);

    // <Block> ::= <FeedInputsBlock>
    void Rule_Block3(Token *token);

    // <Block> ::= <InputsBlock>
    void Rule_Block4(Token *token);

    // <Block> ::= <UserInputsBlock>
    void Rule_Block5(Token *token);

    // <Block> ::= <FeedOutputsBlock>
    void Rule_Block6(Token *token);

    // <Block> ::= <OutputsBlock>
    void Rule_Block7(Token *token);

    // <Block> ::= <PrivateFunctionsBlock>
    void Rule_Block8(Token *token);

    // <Block> ::= <PublicFunctionsBlock>
    void Rule_Block9(Token *token);

    // <Block> ::= <ChildrenBlock>
    void Rule_Block10(Token *token);

    // <Block> ::= <Connection>
    void Rule_Block11(Token *token);

    // <Blocks> ::= <Block>
    void Rule_Blocks(Token *token);

    // <Blocks> ::= <Blocks> <Block>
    void Rule_Blocks2(Token *token);

    // <ModuleDefinition> ::= <CustomOption> module_def IdentifierTerm { <Blocks> }
    void Rule_ModuleDefinition_module_def_IdentifierTerm_LBrace_RBrace(Token *token);

    typedef void (Parser::*RuleHandler)(Token *token);
    static const RuleHandler gRuleJumpTable[120];

    // call to jump to a given token's rule handler; must only be called on
    // tokens that have a reduction rule
    virtual void executeRule(Token *token)
    {
      assert(token->m_reductionRule >= 0);
      m_trackingLastHitLine = token->m_line;
      (*this.*gRuleJumpTable[token->m_reductionRule])(token);
    }

    virtual void processSavedComments(const SavedComments &savedComments);
    
    inline MDFModule::Context* getContext() { return reinterpret_cast<MDFModule::Context*>(m_context); }

    virtual void recursiveFunctionDeclarationConcat(const Grammar& grammar, Token *token, unsigned int startToken);
    
public:

    static const Grammar sGrammar;

    Parser(MDFFeedbackInterface* fbi, MDFSyntaxHighlightingInterface *shi) : ParserBase(sGrammar, fbi, shi) {}
};

} // namespace MDFModule

