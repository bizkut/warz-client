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
 * MDFBehaviourGrammar Grammar, Version 2
 * NaturalMotion ltd.
 */
namespace MDFBehaviour
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
    /* 10 */  Symbol_Dot,
    /* 11 */  Symbol_Semi,
    /* 12 */  Symbol_LBracket,
    /* 13 */  Symbol_RBracket,
    /* 14 */  Symbol_LBrace,
    /* 15 */  Symbol_RBrace,
    /* 16 */  Symbol_Plus,
    /* 17 */  Symbol_Eq,
    /* 18 */  Symbol_animationInputsColon,
    /* 19 */  Symbol_ArraySpecifier,
    /* 20 */  Symbol_attributesColon,
    /* 21 */  Symbol_behaviour_def,
    /* 22 */  Symbol_BoolLiteral,
    /* 23 */  Symbol_controlParamsColon,
    /* 24 */  Symbol_DecLiteral,
    /* 25 */  Symbol_defaultsDisplayFunc,
    /* 26 */  Symbol_development,
    /* 27 */  Symbol_displayFunc,
    /* 28 */  Symbol_displayname,
    /* 29 */  Symbol_emittedMessagesColon,
    /* 30 */  Symbol_enum,
    /* 31 */  Symbol_FloatLiteral,
    /* 32 */  Symbol_group,
    /* 33 */  Symbol_IdentifierTerm,
    /* 34 */  Symbol_IdStar,
    /* 35 */  Symbol_include,
    /* 36 */  Symbol_interfaceColon,
    /* 37 */  Symbol_ModKeyword,
    /* 38 */  Symbol_modulesColon,
    /* 39 */  Symbol_outputControlParamsColon,
    /* 40 */  Symbol_pingroups,
    /* 41 */  Symbol_StringLiteral,
    /* 42 */  Symbol_upgrade,
    /* 43 */  Symbol_version,
    /* 44 */  Symbol_AnimationDefault,
    /* 45 */  Symbol_AnimationDefaults,
    /* 46 */  Symbol_AnimationInputDef,
    /* 47 */  Symbol_AnimationInputDefs,
    /* 48 */  Symbol_AnimInputsBlock,
    /* 49 */  Symbol_AttributeDefault,
    /* 50 */  Symbol_AttributeDefaults,
    /* 51 */  Symbol_AttributeEnum,
    /* 52 */  Symbol_AttributeEnums,
    /* 53 */  Symbol_AttributeGroupDef,
    /* 54 */  Symbol_AttributeGroupDefs,
    /* 55 */  Symbol_AttributeGroupSpec,
    /* 56 */  Symbol_AttributeList,
    /* 57 */  Symbol_AttributeLists,
    /* 58 */  Symbol_AttributesBlock,
    /* 59 */  Symbol_AttributeSpec,
    /* 60 */  Symbol_BehaviourDefinition,
    /* 61 */  Symbol_Block,
    /* 62 */  Symbol_Blocks,
    /* 63 */  Symbol_ControlParamDef,
    /* 64 */  Symbol_ControlParamDefault,
    /* 65 */  Symbol_ControlParamDefaults,
    /* 66 */  Symbol_ControlParamDefs,
    /* 67 */  Symbol_ControlParamGroupDef,
    /* 68 */  Symbol_ControlParamGroupDefs,
    /* 69 */  Symbol_ControlParamsBlock,
    /* 70 */  Symbol_ControlParamSpec,
    /* 71 */  Symbol_CustomOption,
    /* 72 */  Symbol_CustomOptionKeyword,
    /* 73 */  Symbol_CustomOptions,
    /* 74 */  Symbol_EmittedMessagesBlock,
    /* 75 */  Symbol_EmittedMessagesDef,
    /* 76 */  Symbol_EmittedMessagesDefs,
    /* 77 */  Symbol_EnumExpr,
    /* 78 */  Symbol_EnumExprValue,
    /* 79 */  Symbol_InterfaceBlock,
    /* 80 */  Symbol_InterfaceDef,
    /* 81 */  Symbol_InterfaceDefs,
    /* 82 */  Symbol_ModuleDef,
    /* 83 */  Symbol_ModuleDefs,
    /* 84 */  Symbol_ModuleIdentifier,
    /* 85 */  Symbol_ModulesBlock,
    /* 86 */  Symbol_ModuleWiring,
    /* 87 */  Symbol_NumericLiteral,
    /* 88 */  Symbol_NumericLiterals,
    /* 89 */  Symbol_NumericLiteralsGroup,
    /* 90 */  Symbol_OutputControlParamDef,
    /* 91 */  Symbol_OutputControlParamDefault,
    /* 92 */  Symbol_OutputControlParamDefaults,
    /* 93 */  Symbol_OutputControlParamDefs,
    /* 94 */  Symbol_OutputControlParamsBlock,
    /* 95 */  Symbol_OutputControlParamSpec 
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
    reRule_NumericLiteral_FloatLiteral,
    reRule_NumericLiteral_DecLiteral,
    reRule_NumericLiteral_BoolLiteral,
    reRule_NumericLiterals,
    reRule_NumericLiterals_Comma,
    reRule_NumericLiteralsGroup_LParan_RParan,
    reRule_ModuleIdentifier_IdentifierTerm,
    reRule_ModuleIdentifier_IdentifierTerm_ArraySpecifier,
    reRule_ModuleIdentifier_IdentifierTerm_LBracket_IdStar_RBracket,
    reRule_ModuleWiring,
    reRule_ModuleWiring_Dot,
    reRule_ModuleWiring_IdStar,
    reRule_ModuleDef_Semi,
    reRule_ModuleDefs,
    reRule_ModuleDefs2,
    reRule_AttributeGroupSpec_LParan_StringLiteral_RParan,
    reRule_AttributeGroupDef_group_LBrace_RBrace,
    reRule_AttributeGroupDef_LBrace_RBrace,
    reRule_EnumExprValue_DecLiteral,
    reRule_EnumExprValue_IdentifierTerm,
    reRule_EnumExpr_Plus,
    reRule_EnumExpr_Minus,
    reRule_EnumExpr_IdStar,
    reRule_EnumExpr,
    reRule_AttributeEnum_IdentifierTerm,
    reRule_AttributeEnum_IdentifierTerm_Eq,
    reRule_AttributeEnums,
    reRule_AttributeEnums_Comma,
    reRule_AttributeGroupDefs,
    reRule_AttributeGroupDefs2,
    reRule_AttributeDefault,
    reRule_AttributeDefault2,
    reRule_AttributeDefaults,
    reRule_AttributeDefaults_Comma,
    reRule_AttributeSpec_IdentifierTerm,
    reRule_AttributeSpec_IdentifierTerm_ArraySpecifier,
    reRule_AttributeSpec_IdentifierTerm_LBracket_IdentifierTerm_RBracket,
    reRule_AttributeList_IdentifierTerm_Eq_Semi,
    reRule_AttributeList_enum_IdentifierTerm_LBrace_RBrace_Semi,
    reRule_AttributeLists,
    reRule_AttributeLists2,
    reRule_ControlParamDefault,
    reRule_ControlParamDefault2,
    reRule_ControlParamDefaults,
    reRule_ControlParamDefaults_Comma,
    reRule_ControlParamSpec_IdentifierTerm,
    reRule_ControlParamSpec_IdentifierTerm_ArraySpecifier,
    reRule_ControlParamSpec_IdentifierTerm_LBracket_IdentifierTerm_RBracket,
    reRule_ControlParamDef_IdentifierTerm_Eq_Semi,
    reRule_ControlParamDef_IdentifierTerm_Semi,
    reRule_ControlParamDefs,
    reRule_ControlParamDefs2,
    reRule_ControlParamGroupDef_group_LParan_StringLiteral_RParan_LBrace_RBrace,
    reRule_ControlParamGroupDef_LBrace_RBrace,
    reRule_ControlParamGroupDefs,
    reRule_ControlParamGroupDefs2,
    reRule_OutputControlParamDefault,
    reRule_OutputControlParamDefault2,
    reRule_OutputControlParamDefaults,
    reRule_OutputControlParamDefaults_Comma,
    reRule_OutputControlParamSpec_IdentifierTerm,
    reRule_OutputControlParamSpec_IdentifierTerm_ArraySpecifier,
    reRule_OutputControlParamSpec_IdentifierTerm_LBracket_IdentifierTerm_RBracket,
    reRule_OutputControlParamDef_IdentifierTerm_Eq_Semi,
    reRule_OutputControlParamDef_IdentifierTerm_Semi,
    reRule_OutputControlParamDefs,
    reRule_OutputControlParamDefs2,
    reRule_InterfaceDef_version_Eq_DecLiteral_Semi,
    reRule_InterfaceDef_displayname_Eq_StringLiteral_Semi,
    reRule_InterfaceDef_group_Eq_StringLiteral_Semi,
    reRule_InterfaceDef_include_Eq_StringLiteral_Semi,
    reRule_InterfaceDef_pingroups_Eq_StringLiteral_Semi,
    reRule_InterfaceDef_upgrade_Eq_StringLiteral_Semi,
    reRule_InterfaceDef_defaultsDisplayFunc_Eq_StringLiteral_Semi,
    reRule_InterfaceDef_displayFunc_Eq_StringLiteral_Semi,
    reRule_InterfaceDef_development_Eq_BoolLiteral_Semi,
    reRule_InterfaceDefs,
    reRule_InterfaceDefs2,
    reRule_EmittedMessagesDef_IdentifierTerm_Semi,
    reRule_EmittedMessagesDefs,
    reRule_EmittedMessagesDefs2,
    reRule_AnimationDefault,
    reRule_AnimationDefaults,
    reRule_AnimationDefaults_Comma,
    reRule_AnimationInputDef_IdentifierTerm_IdentifierTerm_Semi,
    reRule_AnimationInputDef_IdentifierTerm_IdentifierTerm_Eq_Semi,
    reRule_AnimationInputDefs,
    reRule_AnimationInputDefs2,
    reRule_ModulesBlock_modulesColon,
    reRule_ModulesBlock_modulesColon2,
    reRule_AttributesBlock_attributesColon,
    reRule_AttributesBlock_attributesColon2,
    reRule_ControlParamsBlock_controlParamsColon,
    reRule_ControlParamsBlock_controlParamsColon2,
    reRule_OutputControlParamsBlock_outputControlParamsColon,
    reRule_OutputControlParamsBlock_outputControlParamsColon2,
    reRule_EmittedMessagesBlock_emittedMessagesColon,
    reRule_EmittedMessagesBlock_emittedMessagesColon2,
    reRule_AnimInputsBlock_animationInputsColon,
    reRule_AnimInputsBlock_animationInputsColon2,
    reRule_InterfaceBlock_interfaceColon,
    reRule_InterfaceBlock_interfaceColon2,
    reRule_Block,
    reRule_Block2,
    reRule_Block3,
    reRule_Block4,
    reRule_Block5,
    reRule_Block6,
    reRule_Block7,
    reRule_Blocks,
    reRule_Blocks2,
    reRule_BehaviourDefinition_behaviour_def_IdentifierTerm_LBrace_RBrace 
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

    // <NumericLiteral> ::= FloatLiteral
    void Rule_NumericLiteral_FloatLiteral(Token *token);

    // <NumericLiteral> ::= DecLiteral
    void Rule_NumericLiteral_DecLiteral(Token *token);

    // <NumericLiteral> ::= BoolLiteral
    void Rule_NumericLiteral_BoolLiteral(Token *token);

    // <NumericLiterals> ::= <NumericLiteral>
    void Rule_NumericLiterals(Token *token);

    // <NumericLiterals> ::= <NumericLiterals> , <NumericLiteral>
    void Rule_NumericLiterals_Comma(Token *token);

    // <NumericLiteralsGroup> ::= ( <NumericLiterals> )
    void Rule_NumericLiteralsGroup_LParan_RParan(Token *token);

    // <ModuleIdentifier> ::= IdentifierTerm
    void Rule_ModuleIdentifier_IdentifierTerm(Token *token);

    // <ModuleIdentifier> ::= IdentifierTerm ArraySpecifier
    void Rule_ModuleIdentifier_IdentifierTerm_ArraySpecifier(Token *token);

    // <ModuleIdentifier> ::= IdentifierTerm [ IdStar ]
    void Rule_ModuleIdentifier_IdentifierTerm_LBracket_IdStar_RBracket(Token *token);

    // <ModuleWiring> ::= <ModuleIdentifier>
    void Rule_ModuleWiring(Token *token);

    // <ModuleWiring> ::= <ModuleIdentifier> . <ModuleWiring>
    void Rule_ModuleWiring_Dot(Token *token);

    // <ModuleWiring> ::= IdStar
    void Rule_ModuleWiring_IdStar(Token *token);

    // <ModuleDef> ::= <ModuleWiring> ;
    void Rule_ModuleDef_Semi(Token *token);

    // <ModuleDefs> ::= <ModuleDef>
    void Rule_ModuleDefs(Token *token);

    // <ModuleDefs> ::= <ModuleDefs> <ModuleDef>
    void Rule_ModuleDefs2(Token *token);

    // <AttributeGroupSpec> ::= ( StringLiteral )
    void Rule_AttributeGroupSpec_LParan_StringLiteral_RParan(Token *token);

    // <AttributeGroupDef> ::= <CustomOption> group <AttributeGroupSpec> { <AttributeLists> }
    void Rule_AttributeGroupDef_group_LBrace_RBrace(Token *token);

    // <AttributeGroupDef> ::= { <AttributeLists> }
    void Rule_AttributeGroupDef_LBrace_RBrace(Token *token);

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

    // <AttributeEnum> ::= IdentifierTerm
    void Rule_AttributeEnum_IdentifierTerm(Token *token);

    // <AttributeEnum> ::= IdentifierTerm = <EnumExpr>
    void Rule_AttributeEnum_IdentifierTerm_Eq(Token *token);

    // <AttributeEnums> ::= <AttributeEnum>
    void Rule_AttributeEnums(Token *token);

    // <AttributeEnums> ::= <AttributeEnums> , <AttributeEnum>
    void Rule_AttributeEnums_Comma(Token *token);

    // <AttributeGroupDefs> ::= <AttributeGroupDef>
    void Rule_AttributeGroupDefs(Token *token);

    // <AttributeGroupDefs> ::= <AttributeGroupDefs> <AttributeGroupDef>
    void Rule_AttributeGroupDefs2(Token *token);

    // <AttributeDefault> ::= <NumericLiteralsGroup>
    void Rule_AttributeDefault(Token *token);

    // <AttributeDefault> ::= <NumericLiteral>
    void Rule_AttributeDefault2(Token *token);

    // <AttributeDefaults> ::= <AttributeDefault>
    void Rule_AttributeDefaults(Token *token);

    // <AttributeDefaults> ::= <AttributeDefaults> , <AttributeDefault>
    void Rule_AttributeDefaults_Comma(Token *token);

    // <AttributeSpec> ::= IdentifierTerm
    void Rule_AttributeSpec_IdentifierTerm(Token *token);

    // <AttributeSpec> ::= IdentifierTerm ArraySpecifier
    void Rule_AttributeSpec_IdentifierTerm_ArraySpecifier(Token *token);

    // <AttributeSpec> ::= IdentifierTerm [ IdentifierTerm ]
    void Rule_AttributeSpec_IdentifierTerm_LBracket_IdentifierTerm_RBracket(Token *token);

    // <AttributeList> ::= IdentifierTerm <AttributeSpec> = <AttributeDefaults> <CustomOption> ;
    void Rule_AttributeList_IdentifierTerm_Eq_Semi(Token *token);

    // <AttributeList> ::= enum IdentifierTerm { <AttributeEnums> } <CustomOption> ;
    void Rule_AttributeList_enum_IdentifierTerm_LBrace_RBrace_Semi(Token *token);

    // <AttributeLists> ::= <AttributeList>
    void Rule_AttributeLists(Token *token);

    // <AttributeLists> ::= <AttributeLists> <AttributeList>
    void Rule_AttributeLists2(Token *token);

    // <ControlParamDefault> ::= <NumericLiteralsGroup>
    void Rule_ControlParamDefault(Token *token);

    // <ControlParamDefault> ::= <NumericLiteral>
    void Rule_ControlParamDefault2(Token *token);

    // <ControlParamDefaults> ::= <ControlParamDefault>
    void Rule_ControlParamDefaults(Token *token);

    // <ControlParamDefaults> ::= <ControlParamDefaults> , <ControlParamDefault>
    void Rule_ControlParamDefaults_Comma(Token *token);

    // <ControlParamSpec> ::= IdentifierTerm
    void Rule_ControlParamSpec_IdentifierTerm(Token *token);

    // <ControlParamSpec> ::= IdentifierTerm ArraySpecifier
    void Rule_ControlParamSpec_IdentifierTerm_ArraySpecifier(Token *token);

    // <ControlParamSpec> ::= IdentifierTerm [ IdentifierTerm ]
    void Rule_ControlParamSpec_IdentifierTerm_LBracket_IdentifierTerm_RBracket(Token *token);

    // <ControlParamDef> ::= IdentifierTerm <ControlParamSpec> = <ControlParamDefaults> <CustomOption> ;
    void Rule_ControlParamDef_IdentifierTerm_Eq_Semi(Token *token);

    // <ControlParamDef> ::= IdentifierTerm <ControlParamSpec> <CustomOption> ;
    void Rule_ControlParamDef_IdentifierTerm_Semi(Token *token);

    // <ControlParamDefs> ::= <ControlParamDef>
    void Rule_ControlParamDefs(Token *token);

    // <ControlParamDefs> ::= <ControlParamDefs> <ControlParamDef>
    void Rule_ControlParamDefs2(Token *token);

    // <ControlParamGroupDef> ::= <CustomOption> group ( StringLiteral ) { <ControlParamDefs> }
    void Rule_ControlParamGroupDef_group_LParan_StringLiteral_RParan_LBrace_RBrace(Token *token);

    // <ControlParamGroupDef> ::= { <ControlParamDefs> }
    void Rule_ControlParamGroupDef_LBrace_RBrace(Token *token);

    // <ControlParamGroupDefs> ::= <ControlParamGroupDef>
    void Rule_ControlParamGroupDefs(Token *token);

    // <ControlParamGroupDefs> ::= <ControlParamGroupDefs> <ControlParamGroupDef>
    void Rule_ControlParamGroupDefs2(Token *token);

    // <OutputControlParamDefault> ::= <NumericLiteralsGroup>
    void Rule_OutputControlParamDefault(Token *token);

    // <OutputControlParamDefault> ::= <NumericLiteral>
    void Rule_OutputControlParamDefault2(Token *token);

    // <OutputControlParamDefaults> ::= <OutputControlParamDefault>
    void Rule_OutputControlParamDefaults(Token *token);

    // <OutputControlParamDefaults> ::= <OutputControlParamDefaults> , <OutputControlParamDefault>
    void Rule_OutputControlParamDefaults_Comma(Token *token);

    // <OutputControlParamSpec> ::= IdentifierTerm
    void Rule_OutputControlParamSpec_IdentifierTerm(Token *token);

    // <OutputControlParamSpec> ::= IdentifierTerm ArraySpecifier
    void Rule_OutputControlParamSpec_IdentifierTerm_ArraySpecifier(Token *token);

    // <OutputControlParamSpec> ::= IdentifierTerm [ IdentifierTerm ]
    void Rule_OutputControlParamSpec_IdentifierTerm_LBracket_IdentifierTerm_RBracket(Token *token);

    // <OutputControlParamDef> ::= IdentifierTerm <OutputControlParamSpec> = <OutputControlParamDefaults> <CustomOption> ;
    void Rule_OutputControlParamDef_IdentifierTerm_Eq_Semi(Token *token);

    // <OutputControlParamDef> ::= IdentifierTerm <OutputControlParamSpec> <CustomOption> ;
    void Rule_OutputControlParamDef_IdentifierTerm_Semi(Token *token);

    // <OutputControlParamDefs> ::= <OutputControlParamDef>
    void Rule_OutputControlParamDefs(Token *token);

    // <OutputControlParamDefs> ::= <OutputControlParamDefs> <OutputControlParamDef>
    void Rule_OutputControlParamDefs2(Token *token);

    // <InterfaceDef> ::= version = DecLiteral ;
    void Rule_InterfaceDef_version_Eq_DecLiteral_Semi(Token *token);

    // <InterfaceDef> ::= displayname = StringLiteral ;
    void Rule_InterfaceDef_displayname_Eq_StringLiteral_Semi(Token *token);

    // <InterfaceDef> ::= group = StringLiteral ;
    void Rule_InterfaceDef_group_Eq_StringLiteral_Semi(Token *token);

    // <InterfaceDef> ::= include = StringLiteral ;
    void Rule_InterfaceDef_include_Eq_StringLiteral_Semi(Token *token);

    // <InterfaceDef> ::= pingroups = StringLiteral ;
    void Rule_InterfaceDef_pingroups_Eq_StringLiteral_Semi(Token *token);

    // <InterfaceDef> ::= upgrade = StringLiteral ;
    void Rule_InterfaceDef_upgrade_Eq_StringLiteral_Semi(Token *token);

    // <InterfaceDef> ::= defaultsDisplayFunc = StringLiteral ;
    void Rule_InterfaceDef_defaultsDisplayFunc_Eq_StringLiteral_Semi(Token *token);

    // <InterfaceDef> ::= displayFunc = StringLiteral ;
    void Rule_InterfaceDef_displayFunc_Eq_StringLiteral_Semi(Token *token);

    // <InterfaceDef> ::= development = BoolLiteral ;
    void Rule_InterfaceDef_development_Eq_BoolLiteral_Semi(Token *token);

    // <InterfaceDefs> ::= <InterfaceDef>
    void Rule_InterfaceDefs(Token *token);

    // <InterfaceDefs> ::= <InterfaceDefs> <InterfaceDef>
    void Rule_InterfaceDefs2(Token *token);

    // <EmittedMessagesDef> ::= IdentifierTerm ;
    void Rule_EmittedMessagesDef_IdentifierTerm_Semi(Token *token);

    // <EmittedMessagesDefs> ::= <EmittedMessagesDef>
    void Rule_EmittedMessagesDefs(Token *token);

    // <EmittedMessagesDefs> ::= <EmittedMessagesDefs> <EmittedMessagesDef>
    void Rule_EmittedMessagesDefs2(Token *token);

    // <AnimationDefault> ::= <NumericLiteral>
    void Rule_AnimationDefault(Token *token);

    // <AnimationDefaults> ::= <AnimationDefault>
    void Rule_AnimationDefaults(Token *token);

    // <AnimationDefaults> ::= <AnimationDefaults> , <AnimationDefault>
    void Rule_AnimationDefaults_Comma(Token *token);

    // <AnimationInputDef> ::= IdentifierTerm IdentifierTerm <CustomOption> ;
    void Rule_AnimationInputDef_IdentifierTerm_IdentifierTerm_Semi(Token *token);

    // <AnimationInputDef> ::= IdentifierTerm IdentifierTerm = <AnimationDefaults> <CustomOption> ;
    void Rule_AnimationInputDef_IdentifierTerm_IdentifierTerm_Eq_Semi(Token *token);

    // <AnimationInputDefs> ::= <AnimationInputDef>
    void Rule_AnimationInputDefs(Token *token);

    // <AnimationInputDefs> ::= <AnimationInputDefs> <AnimationInputDef>
    void Rule_AnimationInputDefs2(Token *token);

    // <ModulesBlock> ::= modules:
    void Rule_ModulesBlock_modulesColon(Token *token);

    // <ModulesBlock> ::= modules: <ModuleDefs>
    void Rule_ModulesBlock_modulesColon2(Token *token);

    // <AttributesBlock> ::= attributes:
    void Rule_AttributesBlock_attributesColon(Token *token);

    // <AttributesBlock> ::= attributes: <AttributeGroupDefs>
    void Rule_AttributesBlock_attributesColon2(Token *token);

    // <ControlParamsBlock> ::= controlParams:
    void Rule_ControlParamsBlock_controlParamsColon(Token *token);

    // <ControlParamsBlock> ::= controlParams: <ControlParamGroupDefs>
    void Rule_ControlParamsBlock_controlParamsColon2(Token *token);

    // <OutputControlParamsBlock> ::= outputControlParams:
    void Rule_OutputControlParamsBlock_outputControlParamsColon(Token *token);

    // <OutputControlParamsBlock> ::= outputControlParams: <OutputControlParamDefs>
    void Rule_OutputControlParamsBlock_outputControlParamsColon2(Token *token);

    // <EmittedMessagesBlock> ::= emittedMessages:
    void Rule_EmittedMessagesBlock_emittedMessagesColon(Token *token);

    // <EmittedMessagesBlock> ::= emittedMessages: <EmittedMessagesDefs>
    void Rule_EmittedMessagesBlock_emittedMessagesColon2(Token *token);

    // <AnimInputsBlock> ::= animationInputs:
    void Rule_AnimInputsBlock_animationInputsColon(Token *token);

    // <AnimInputsBlock> ::= animationInputs: <AnimationInputDefs>
    void Rule_AnimInputsBlock_animationInputsColon2(Token *token);

    // <InterfaceBlock> ::= interface:
    void Rule_InterfaceBlock_interfaceColon(Token *token);

    // <InterfaceBlock> ::= interface: <InterfaceDefs>
    void Rule_InterfaceBlock_interfaceColon2(Token *token);

    // <Block> ::= <ModulesBlock>
    void Rule_Block(Token *token);

    // <Block> ::= <AttributesBlock>
    void Rule_Block2(Token *token);

    // <Block> ::= <ControlParamsBlock>
    void Rule_Block3(Token *token);

    // <Block> ::= <OutputControlParamsBlock>
    void Rule_Block4(Token *token);

    // <Block> ::= <EmittedMessagesBlock>
    void Rule_Block5(Token *token);

    // <Block> ::= <AnimInputsBlock>
    void Rule_Block6(Token *token);

    // <Block> ::= <InterfaceBlock>
    void Rule_Block7(Token *token);

    // <Blocks> ::= <Block>
    void Rule_Blocks(Token *token);

    // <Blocks> ::= <Blocks> <Block>
    void Rule_Blocks2(Token *token);

    // <BehaviourDefinition> ::= <CustomOption> behaviour_def IdentifierTerm { <Blocks> }
    void Rule_BehaviourDefinition_behaviour_def_IdentifierTerm_LBrace_RBrace(Token *token);

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
    
    inline MDFBehaviour::Context* getContext() { return reinterpret_cast<MDFBehaviour::Context*>(m_context); }

    virtual void recursiveFunctionDeclarationConcat(const Grammar& , Token *, unsigned int ) {}
    
public:

    static const Grammar sGrammar;

    Parser(MDFFeedbackInterface* fbi, MDFSyntaxHighlightingInterface *shi) : ParserBase(sGrammar, fbi, shi) {}
};

} // namespace MDFBehaviour

