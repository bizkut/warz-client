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

#include "MDFTypesGrammar.h"
#include "StringBuilder.h"

namespace MDFTypes
{

  // --------------------------------------------------------------------
  // Symbols
  // --------------------------------------------------------------------

  const Symbol SymbolArray[66] = 
  {
    /*  0 */  {(SymbolKind)3, "EOF"},
    /*  1 */  {(SymbolKind)7, "Error"},
    /*  2 */  {(SymbolKind)2, "Whitespace"},
    /*  3 */  {(SymbolKind)5, "Comment End"},
    /*  4 */  {(SymbolKind)6, "Comment Line"},
    /*  5 */  {(SymbolKind)4, "Comment Start"},
    /*  6 */  {(SymbolKind)1, "-"},
    /*  7 */  {(SymbolKind)1, "("},
    /*  8 */  {(SymbolKind)1, ")"},
    /*  9 */  {(SymbolKind)1, ","},
    /* 10 */  {(SymbolKind)1, ":"},
    /* 11 */  {(SymbolKind)1, "::"},
    /* 12 */  {(SymbolKind)1, ";"},
    /* 13 */  {(SymbolKind)1, "["},
    /* 14 */  {(SymbolKind)1, "]"},
    /* 15 */  {(SymbolKind)1, "{"},
    /* 16 */  {(SymbolKind)1, "}"},
    /* 17 */  {(SymbolKind)1, "};"},
    /* 18 */  {(SymbolKind)1, "+"},
    /* 19 */  {(SymbolKind)1, "="},
    /* 20 */  {(SymbolKind)1, "=>"},
    /* 21 */  {(SymbolKind)1, "alias"},
    /* 22 */  {(SymbolKind)1, "ArraySpecifier"},
    /* 23 */  {(SymbolKind)1, "DecLiteral"},
    /* 24 */  {(SymbolKind)1, "enum"},
    /* 25 */  {(SymbolKind)1, "FloatLiteral"},
    /* 26 */  {(SymbolKind)1, "IdentifierTerm"},
    /* 27 */  {(SymbolKind)1, "IdRef"},
    /* 28 */  {(SymbolKind)1, "IdStar"},
    /* 29 */  {(SymbolKind)1, "ModKeyword"},
    /* 30 */  {(SymbolKind)1, "namespace"},
    /* 31 */  {(SymbolKind)1, "private:"},
    /* 32 */  {(SymbolKind)1, "public:"},
    /* 33 */  {(SymbolKind)1, "StringLiteral"},
    /* 34 */  {(SymbolKind)1, "struct"},
    /* 35 */  {(SymbolKind)1, "TSizeSpecifier"},
    /* 36 */  {(SymbolKind)1, "typedef"},
    /* 37 */  {(SymbolKind)0, "CustomOption"},
    /* 38 */  {(SymbolKind)0, "CustomOptionKeyword"},
    /* 39 */  {(SymbolKind)0, "CustomOptions"},
    /* 40 */  {(SymbolKind)0, "DataBlockDef"},
    /* 41 */  {(SymbolKind)0, "DataBlockDefs"},
    /* 42 */  {(SymbolKind)0, "Enum"},
    /* 43 */  {(SymbolKind)0, "EnumExpr"},
    /* 44 */  {(SymbolKind)0, "EnumExprValue"},
    /* 45 */  {(SymbolKind)0, "Enums"},
    /* 46 */  {(SymbolKind)0, "FnArg"},
    /* 47 */  {(SymbolKind)0, "FnArgs"},
    /* 48 */  {(SymbolKind)0, "FnDecl"},
    /* 49 */  {(SymbolKind)0, "FnIsConst"},
    /* 50 */  {(SymbolKind)0, "FnLiteral"},
    /* 51 */  {(SymbolKind)0, "FnLiterals"},
    /* 52 */  {(SymbolKind)0, "FnName"},
    /* 53 */  {(SymbolKind)0, "FnNameAndReturn"},
    /* 54 */  {(SymbolKind)0, "FnNumLiteral"},
    /* 55 */  {(SymbolKind)0, "Root"},
    /* 56 */  {(SymbolKind)0, "Roots"},
    /* 57 */  {(SymbolKind)0, "StructInheritance"},
    /* 58 */  {(SymbolKind)0, "TypesRoots"},
    /* 59 */  {(SymbolKind)0, "VarCascade"},
    /* 60 */  {(SymbolKind)0, "VarDecl"},
    /* 61 */  {(SymbolKind)0, "VarIdentifier"},
    /* 62 */  {(SymbolKind)0, "VarSpec"},
    /* 63 */  {(SymbolKind)0, "VarSpecs"},
    /* 64 */  {(SymbolKind)0, "VarType"},
    /* 65 */  {(SymbolKind)0, "VarTypeList"} 
  };


  // --------------------------------------------------------------------
  // Grammar Character Set Definitions
  // --------------------------------------------------------------------

  // Set 0 : [&#9;&#10;&#11;&#12;&#13; ]
  const unsigned int  Charset0Count = 6;
  const char          Charset0[6+1] = {9,10,11,12,13,32,0};

  // Set 1 : [#]
  const unsigned int  Charset1Count = 1;
  const char          Charset1[1+1] = {35,0};

  // Set 2 : [(]
  const unsigned int  Charset2Count = 1;
  const char          Charset2[1+1] = {40,0};

  // Set 3 : [)]
  const unsigned int  Charset3Count = 1;
  const char          Charset3[1+1] = {41,0};

  // Set 4 : [,]
  const unsigned int  Charset4Count = 1;
  const char          Charset4[1+1] = {44,0};

  // Set 5 : [;]
  const unsigned int  Charset5Count = 1;
  const char          Charset5[1+1] = {59,0};

  // Set 6 : []]
  const unsigned int  Charset6Count = 1;
  const char          Charset6[1+1] = {93,0};

  // Set 7 : [+]
  const unsigned int  Charset7Count = 1;
  const char          Charset7[1+1] = {43,0};

  // Set 8 : [.]
  const unsigned int  Charset8Count = 1;
  const char          Charset8[1+1] = {46,0};

  // Set 9 : [ABCDEFGHIJKLMNOPQRSTUVWXYZbcdfghijklmoqruvwxyz]
  const unsigned int  Charset9Count = 46;
  const char          Charset9[46+1] = {65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,98,99,100,102,103,104,105,106,107,108,109,111,113,114,117,118,119,120,121,122,0};

  // Set 10 : [&#38;]
  const unsigned int  Charset10Count = 1;
  const char          Charset10[1+1] = {38,0};

  // Set 11 : [_]
  const unsigned int  Charset11Count = 1;
  const char          Charset11[1+1] = {95,0};

  // Set 12 : [&#34;]
  const unsigned int  Charset12Count = 1;
  const char          Charset12[1+1] = {34,0};

  // Set 13 : [*]
  const unsigned int  Charset13Count = 1;
  const char          Charset13[1+1] = {42,0};

  // Set 14 : [-]
  const unsigned int  Charset14Count = 1;
  const char          Charset14[1+1] = {45,0};

  // Set 15 : [/]
  const unsigned int  Charset15Count = 1;
  const char          Charset15[1+1] = {47,0};

  // Set 16 : [0123456789]
  const unsigned int  Charset16Count = 10;
  const char          Charset16[10+1] = {48,49,50,51,52,53,54,55,56,57,0};

  // Set 17 : [:]
  const unsigned int  Charset17Count = 1;
  const char          Charset17[1+1] = {58,0};

  // Set 18 : [=]
  const unsigned int  Charset18Count = 1;
  const char          Charset18[1+1] = {61,0};

  // Set 19 : [[]
  const unsigned int  Charset19Count = 1;
  const char          Charset19[1+1] = {91,0};

  // Set 20 : [a]
  const unsigned int  Charset20Count = 1;
  const char          Charset20[1+1] = {97,0};

  // Set 21 : [e]
  const unsigned int  Charset21Count = 1;
  const char          Charset21[1+1] = {101,0};

  // Set 22 : [n]
  const unsigned int  Charset22Count = 1;
  const char          Charset22[1+1] = {110,0};

  // Set 23 : [p]
  const unsigned int  Charset23Count = 1;
  const char          Charset23[1+1] = {112,0};

  // Set 24 : [s]
  const unsigned int  Charset24Count = 1;
  const char          Charset24[1+1] = {115,0};

  // Set 25 : [t]
  const unsigned int  Charset25Count = 1;
  const char          Charset25[1+1] = {116,0};

  // Set 26 : [{]
  const unsigned int  Charset26Count = 1;
  const char          Charset26[1+1] = {123,0};

  // Set 27 : [}]
  const unsigned int  Charset27Count = 1;
  const char          Charset27[1+1] = {125,0};

  // Set 28 : [f]
  const unsigned int  Charset28Count = 1;
  const char          Charset28[1+1] = {102,0};

  // Set 29 : [0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz]
  const unsigned int  Charset29Count = 63;
  const char          Charset29[63+1] = {48,49,50,51,52,53,54,55,56,57,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,95,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,0};

  // Set 30 : [0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz]
  const unsigned int  Charset30Count = 62;
  const char          Charset30[62+1] = {48,49,50,51,52,53,54,55,56,57,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,0};

  // Set 31 : [ !#$%&#38;&#39;()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~]
  const unsigned int  Charset31Count = 93;
  const char          Charset31[93+1] = {32,33,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,0};

  // Set 32 : [\]
  const unsigned int  Charset32Count = 1;
  const char          Charset32[1+1] = {92,0};

  // Set 33 : [>]
  const unsigned int  Charset33Count = 1;
  const char          Charset33[1+1] = {62,0};

  // Set 34 : [0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijkmnopqrstuvwxyz]
  const unsigned int  Charset34Count = 62;
  const char          Charset34[62+1] = {48,49,50,51,52,53,54,55,56,57,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,95,97,98,99,100,101,102,103,104,105,106,107,109,110,111,112,113,114,115,116,117,118,119,120,121,122,0};

  // Set 35 : [l]
  const unsigned int  Charset35Count = 1;
  const char          Charset35[1+1] = {108,0};

  // Set 36 : [0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghjklmnopqrstuvwxyz]
  const unsigned int  Charset36Count = 62;
  const char          Charset36[62+1] = {48,49,50,51,52,53,54,55,56,57,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,95,97,98,99,100,101,102,103,104,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,0};

  // Set 37 : [i]
  const unsigned int  Charset37Count = 1;
  const char          Charset37[1+1] = {105,0};

  // Set 38 : [0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_bcdefghijklmnopqrstuvwxyz]
  const unsigned int  Charset38Count = 62;
  const char          Charset38[62+1] = {48,49,50,51,52,53,54,55,56,57,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,95,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,0};

  // Set 39 : [0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrtuvwxyz]
  const unsigned int  Charset39Count = 62;
  const char          Charset39[62+1] = {48,49,50,51,52,53,54,55,56,57,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,95,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,116,117,118,119,120,121,122,0};

  // Set 40 : [0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmopqrstuvwxyz]
  const unsigned int  Charset40Count = 62;
  const char          Charset40[62+1] = {48,49,50,51,52,53,54,55,56,57,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,95,97,98,99,100,101,102,103,104,105,106,107,108,109,111,112,113,114,115,116,117,118,119,120,121,122,0};

  // Set 41 : [0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstvwxyz]
  const unsigned int  Charset41Count = 62;
  const char          Charset41[62+1] = {48,49,50,51,52,53,54,55,56,57,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,95,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,118,119,120,121,122,0};

  // Set 42 : [u]
  const unsigned int  Charset42Count = 1;
  const char          Charset42[1+1] = {117,0};

  // Set 43 : [0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklnopqrstuvwxyz]
  const unsigned int  Charset43Count = 62;
  const char          Charset43[62+1] = {48,49,50,51,52,53,54,55,56,57,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,95,97,98,99,100,101,102,103,104,105,106,107,108,110,111,112,113,114,115,116,117,118,119,120,121,122,0};

  // Set 44 : [m]
  const unsigned int  Charset44Count = 1;
  const char          Charset44[1+1] = {109,0};

  // Set 45 : [0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdfghijklmnopqrstuvwxyz]
  const unsigned int  Charset45Count = 62;
  const char          Charset45[62+1] = {48,49,50,51,52,53,54,55,56,57,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,95,97,98,99,100,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,0};

  // Set 46 : [0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnoqrstuvwxyz]
  const unsigned int  Charset46Count = 62;
  const char          Charset46[62+1] = {48,49,50,51,52,53,54,55,56,57,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,95,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,113,114,115,116,117,118,119,120,121,122,0};

  // Set 47 : [0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abdefghijklmnopqrstuvwxyz]
  const unsigned int  Charset47Count = 62;
  const char          Charset47[62+1] = {48,49,50,51,52,53,54,55,56,57,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,95,97,98,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,0};

  // Set 48 : [c]
  const unsigned int  Charset48Count = 1;
  const char          Charset48[1+1] = {99,0};

  // Set 49 : [0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqstvwxyz]
  const unsigned int  Charset49Count = 61;
  const char          Charset49[61+1] = {48,49,50,51,52,53,54,55,56,57,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,95,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,115,116,118,119,120,121,122,0};

  // Set 50 : [r]
  const unsigned int  Charset50Count = 1;
  const char          Charset50[1+1] = {114,0};

  // Set 51 : [0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuwxyz]
  const unsigned int  Charset51Count = 62;
  const char          Charset51[62+1] = {48,49,50,51,52,53,54,55,56,57,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,95,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,119,120,121,122,0};

  // Set 52 : [v]
  const unsigned int  Charset52Count = 1;
  const char          Charset52[1+1] = {118,0};

  // Set 53 : [0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrsuvwxyz]
  const unsigned int  Charset53Count = 62;
  const char          Charset53[62+1] = {48,49,50,51,52,53,54,55,56,57,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,95,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,117,118,119,120,121,122,0};

  // Set 54 : [0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_acdefghijklmnopqrstuvwxyz]
  const unsigned int  Charset54Count = 62;
  const char          Charset54[62+1] = {48,49,50,51,52,53,54,55,56,57,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,95,97,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,0};

  // Set 55 : [b]
  const unsigned int  Charset55Count = 1;
  const char          Charset55[1+1] = {98,0};

  // Set 56 : [0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqstuvwxyz]
  const unsigned int  Charset56Count = 62;
  const char          Charset56[62+1] = {48,49,50,51,52,53,54,55,56,57,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,95,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,115,116,117,118,119,120,121,122,0};

  // Set 57 : [0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxz]
  const unsigned int  Charset57Count = 62;
  const char          Charset57[62+1] = {48,49,50,51,52,53,54,55,56,57,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,95,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,122,0};

  // Set 58 : [y]
  const unsigned int  Charset58Count = 1;
  const char          Charset58[1+1] = {121,0};

  // Set 59 : [0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcefghijklmnopqrstuvwxyz]
  const unsigned int  Charset59Count = 62;
  const char          Charset59[62+1] = {48,49,50,51,52,53,54,55,56,57,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,95,97,98,99,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,0};

  // Set 60 : [d]
  const unsigned int  Charset60Count = 1;
  const char          Charset60[1+1] = {100,0};

  // Set 61 : [0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdeghijklmnopqrstuvwxyz]
  const unsigned int  Charset61Count = 62;
  const char          Charset61[62+1] = {48,49,50,51,52,53,54,55,56,57,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,95,97,98,99,100,101,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,0};




  // --------------------------------------------------------------------
  // Deterministic FSM Automaton (DFA)
  //
  //                     target state, character count, character set ptr
  // --------------------------------------------------------------------

  // State: 0, Accept: -1
  const DFAEdge DFAEdgeArray0[28+1] = 
  {
    {1, Charset0Count, Charset0},
    {2, Charset1Count, Charset1},
    {3, Charset2Count, Charset2},
    {4, Charset3Count, Charset3},
    {5, Charset4Count, Charset4},
    {6, Charset5Count, Charset5},
    {7, Charset6Count, Charset6},
    {8, Charset7Count, Charset7},
    {9, Charset8Count, Charset8},
    {12, Charset9Count, Charset9},
    {14, Charset10Count, Charset10},
    {15, Charset11Count, Charset11},
    {20, Charset12Count, Charset12},
    {27, Charset13Count, Charset13},
    {29, Charset14Count, Charset14},
    {31, Charset15Count, Charset15},
    {30, Charset16Count, Charset16},
    {34, Charset17Count, Charset17},
    {36, Charset18Count, Charset18},
    {38, Charset19Count, Charset19},
    {41, Charset20Count, Charset20},
    {46, Charset21Count, Charset21},
    {50, Charset22Count, Charset22},
    {59, Charset23Count, Charset23},
    {73, Charset24Count, Charset24},
    {79, Charset25Count, Charset25},
    {86, Charset26Count, Charset26},
    {89, Charset27Count, Charset27},
    {-1, 0, NULL}
  };
  
  // State: 1, Accept: 2
  const DFAEdge DFAEdgeArray1[1+1] = 
  {
    {1, Charset0Count, Charset0},
    {-1, 0, NULL}
  };
  
  // State: 2, Accept: 4
  const DFAEdge DFAEdgeArray2[0+1] = 
  {
    {-1, 0, NULL}
  };
  
  // State: 3, Accept: 7
  const DFAEdge DFAEdgeArray3[0+1] = 
  {
    {-1, 0, NULL}
  };
  
  // State: 4, Accept: 8
  const DFAEdge DFAEdgeArray4[0+1] = 
  {
    {-1, 0, NULL}
  };
  
  // State: 5, Accept: 9
  const DFAEdge DFAEdgeArray5[0+1] = 
  {
    {-1, 0, NULL}
  };
  
  // State: 6, Accept: 12
  const DFAEdge DFAEdgeArray6[0+1] = 
  {
    {-1, 0, NULL}
  };
  
  // State: 7, Accept: 14
  const DFAEdge DFAEdgeArray7[0+1] = 
  {
    {-1, 0, NULL}
  };
  
  // State: 8, Accept: 18
  const DFAEdge DFAEdgeArray8[0+1] = 
  {
    {-1, 0, NULL}
  };
  
  // State: 9, Accept: -1
  const DFAEdge DFAEdgeArray9[1+1] = 
  {
    {10, Charset16Count, Charset16},
    {-1, 0, NULL}
  };
  
  // State: 10, Accept: -1
  const DFAEdge DFAEdgeArray10[2+1] = 
  {
    {10, Charset16Count, Charset16},
    {11, Charset28Count, Charset28},
    {-1, 0, NULL}
  };
  
  // State: 11, Accept: 25
  const DFAEdge DFAEdgeArray11[0+1] = 
  {
    {-1, 0, NULL}
  };
  
  // State: 12, Accept: 26
  const DFAEdge DFAEdgeArray12[1+1] = 
  {
    {13, Charset29Count, Charset29},
    {-1, 0, NULL}
  };
  
  // State: 13, Accept: 26
  const DFAEdge DFAEdgeArray13[1+1] = 
  {
    {13, Charset29Count, Charset29},
    {-1, 0, NULL}
  };
  
  // State: 14, Accept: 27
  const DFAEdge DFAEdgeArray14[0+1] = 
  {
    {-1, 0, NULL}
  };
  
  // State: 15, Accept: -1
  const DFAEdge DFAEdgeArray15[1+1] = 
  {
    {16, Charset11Count, Charset11},
    {-1, 0, NULL}
  };
  
  // State: 16, Accept: -1
  const DFAEdge DFAEdgeArray16[2+1] = 
  {
    {17, Charset30Count, Charset30},
    {18, Charset11Count, Charset11},
    {-1, 0, NULL}
  };
  
  // State: 17, Accept: -1
  const DFAEdge DFAEdgeArray17[2+1] = 
  {
    {17, Charset30Count, Charset30},
    {18, Charset11Count, Charset11},
    {-1, 0, NULL}
  };
  
  // State: 18, Accept: -1
  const DFAEdge DFAEdgeArray18[2+1] = 
  {
    {17, Charset30Count, Charset30},
    {19, Charset11Count, Charset11},
    {-1, 0, NULL}
  };
  
  // State: 19, Accept: 29
  const DFAEdge DFAEdgeArray19[2+1] = 
  {
    {17, Charset30Count, Charset30},
    {19, Charset11Count, Charset11},
    {-1, 0, NULL}
  };
  
  // State: 20, Accept: -1
  const DFAEdge DFAEdgeArray20[3+1] = 
  {
    {21, Charset31Count, Charset31},
    {22, Charset12Count, Charset12},
    {23, Charset32Count, Charset32},
    {-1, 0, NULL}
  };
  
  // State: 21, Accept: -1
  const DFAEdge DFAEdgeArray21[3+1] = 
  {
    {21, Charset31Count, Charset31},
    {22, Charset12Count, Charset12},
    {23, Charset32Count, Charset32},
    {-1, 0, NULL}
  };
  
  // State: 22, Accept: 33
  const DFAEdge DFAEdgeArray22[0+1] = 
  {
    {-1, 0, NULL}
  };
  
  // State: 23, Accept: -1
  const DFAEdge DFAEdgeArray23[3+1] = 
  {
    {24, Charset31Count, Charset31},
    {25, Charset12Count, Charset12},
    {26, Charset32Count, Charset32},
    {-1, 0, NULL}
  };
  
  // State: 24, Accept: -1
  const DFAEdge DFAEdgeArray24[3+1] = 
  {
    {21, Charset31Count, Charset31},
    {22, Charset12Count, Charset12},
    {23, Charset32Count, Charset32},
    {-1, 0, NULL}
  };
  
  // State: 25, Accept: 33
  const DFAEdge DFAEdgeArray25[3+1] = 
  {
    {21, Charset31Count, Charset31},
    {22, Charset12Count, Charset12},
    {23, Charset32Count, Charset32},
    {-1, 0, NULL}
  };
  
  // State: 26, Accept: -1
  const DFAEdge DFAEdgeArray26[3+1] = 
  {
    {24, Charset31Count, Charset31},
    {25, Charset12Count, Charset12},
    {26, Charset32Count, Charset32},
    {-1, 0, NULL}
  };
  
  // State: 27, Accept: 28
  const DFAEdge DFAEdgeArray27[1+1] = 
  {
    {28, Charset15Count, Charset15},
    {-1, 0, NULL}
  };
  
  // State: 28, Accept: 3
  const DFAEdge DFAEdgeArray28[0+1] = 
  {
    {-1, 0, NULL}
  };
  
  // State: 29, Accept: 6
  const DFAEdge DFAEdgeArray29[2+1] = 
  {
    {9, Charset8Count, Charset8},
    {30, Charset16Count, Charset16},
    {-1, 0, NULL}
  };
  
  // State: 30, Accept: 23
  const DFAEdge DFAEdgeArray30[2+1] = 
  {
    {9, Charset8Count, Charset8},
    {30, Charset16Count, Charset16},
    {-1, 0, NULL}
  };
  
  // State: 31, Accept: -1
  const DFAEdge DFAEdgeArray31[2+1] = 
  {
    {32, Charset15Count, Charset15},
    {33, Charset13Count, Charset13},
    {-1, 0, NULL}
  };
  
  // State: 32, Accept: 4
  const DFAEdge DFAEdgeArray32[0+1] = 
  {
    {-1, 0, NULL}
  };
  
  // State: 33, Accept: 5
  const DFAEdge DFAEdgeArray33[0+1] = 
  {
    {-1, 0, NULL}
  };
  
  // State: 34, Accept: 10
  const DFAEdge DFAEdgeArray34[1+1] = 
  {
    {35, Charset17Count, Charset17},
    {-1, 0, NULL}
  };
  
  // State: 35, Accept: 11
  const DFAEdge DFAEdgeArray35[0+1] = 
  {
    {-1, 0, NULL}
  };
  
  // State: 36, Accept: 19
  const DFAEdge DFAEdgeArray36[1+1] = 
  {
    {37, Charset33Count, Charset33},
    {-1, 0, NULL}
  };
  
  // State: 37, Accept: 20
  const DFAEdge DFAEdgeArray37[0+1] = 
  {
    {-1, 0, NULL}
  };
  
  // State: 38, Accept: 13
  const DFAEdge DFAEdgeArray38[1+1] = 
  {
    {39, Charset16Count, Charset16},
    {-1, 0, NULL}
  };
  
  // State: 39, Accept: -1
  const DFAEdge DFAEdgeArray39[2+1] = 
  {
    {39, Charset16Count, Charset16},
    {40, Charset6Count, Charset6},
    {-1, 0, NULL}
  };
  
  // State: 40, Accept: 22
  const DFAEdge DFAEdgeArray40[0+1] = 
  {
    {-1, 0, NULL}
  };
  
  // State: 41, Accept: 26
  const DFAEdge DFAEdgeArray41[2+1] = 
  {
    {13, Charset34Count, Charset34},
    {42, Charset35Count, Charset35},
    {-1, 0, NULL}
  };
  
  // State: 42, Accept: 26
  const DFAEdge DFAEdgeArray42[2+1] = 
  {
    {13, Charset36Count, Charset36},
    {43, Charset37Count, Charset37},
    {-1, 0, NULL}
  };
  
  // State: 43, Accept: 26
  const DFAEdge DFAEdgeArray43[2+1] = 
  {
    {13, Charset38Count, Charset38},
    {44, Charset20Count, Charset20},
    {-1, 0, NULL}
  };
  
  // State: 44, Accept: 26
  const DFAEdge DFAEdgeArray44[2+1] = 
  {
    {13, Charset39Count, Charset39},
    {45, Charset24Count, Charset24},
    {-1, 0, NULL}
  };
  
  // State: 45, Accept: 21
  const DFAEdge DFAEdgeArray45[1+1] = 
  {
    {13, Charset29Count, Charset29},
    {-1, 0, NULL}
  };
  
  // State: 46, Accept: 26
  const DFAEdge DFAEdgeArray46[2+1] = 
  {
    {13, Charset40Count, Charset40},
    {47, Charset22Count, Charset22},
    {-1, 0, NULL}
  };
  
  // State: 47, Accept: 26
  const DFAEdge DFAEdgeArray47[2+1] = 
  {
    {13, Charset41Count, Charset41},
    {48, Charset42Count, Charset42},
    {-1, 0, NULL}
  };
  
  // State: 48, Accept: 26
  const DFAEdge DFAEdgeArray48[2+1] = 
  {
    {13, Charset43Count, Charset43},
    {49, Charset44Count, Charset44},
    {-1, 0, NULL}
  };
  
  // State: 49, Accept: 24
  const DFAEdge DFAEdgeArray49[1+1] = 
  {
    {13, Charset29Count, Charset29},
    {-1, 0, NULL}
  };
  
  // State: 50, Accept: 26
  const DFAEdge DFAEdgeArray50[2+1] = 
  {
    {13, Charset38Count, Charset38},
    {51, Charset20Count, Charset20},
    {-1, 0, NULL}
  };
  
  // State: 51, Accept: 26
  const DFAEdge DFAEdgeArray51[2+1] = 
  {
    {13, Charset43Count, Charset43},
    {52, Charset44Count, Charset44},
    {-1, 0, NULL}
  };
  
  // State: 52, Accept: 26
  const DFAEdge DFAEdgeArray52[2+1] = 
  {
    {13, Charset45Count, Charset45},
    {53, Charset21Count, Charset21},
    {-1, 0, NULL}
  };
  
  // State: 53, Accept: 26
  const DFAEdge DFAEdgeArray53[2+1] = 
  {
    {13, Charset39Count, Charset39},
    {54, Charset24Count, Charset24},
    {-1, 0, NULL}
  };
  
  // State: 54, Accept: 26
  const DFAEdge DFAEdgeArray54[2+1] = 
  {
    {13, Charset46Count, Charset46},
    {55, Charset23Count, Charset23},
    {-1, 0, NULL}
  };
  
  // State: 55, Accept: 26
  const DFAEdge DFAEdgeArray55[2+1] = 
  {
    {13, Charset38Count, Charset38},
    {56, Charset20Count, Charset20},
    {-1, 0, NULL}
  };
  
  // State: 56, Accept: 26
  const DFAEdge DFAEdgeArray56[2+1] = 
  {
    {13, Charset47Count, Charset47},
    {57, Charset48Count, Charset48},
    {-1, 0, NULL}
  };
  
  // State: 57, Accept: 26
  const DFAEdge DFAEdgeArray57[2+1] = 
  {
    {13, Charset45Count, Charset45},
    {58, Charset21Count, Charset21},
    {-1, 0, NULL}
  };
  
  // State: 58, Accept: 30
  const DFAEdge DFAEdgeArray58[1+1] = 
  {
    {13, Charset29Count, Charset29},
    {-1, 0, NULL}
  };
  
  // State: 59, Accept: 26
  const DFAEdge DFAEdgeArray59[3+1] = 
  {
    {13, Charset49Count, Charset49},
    {60, Charset50Count, Charset50},
    {67, Charset42Count, Charset42},
    {-1, 0, NULL}
  };
  
  // State: 60, Accept: 26
  const DFAEdge DFAEdgeArray60[2+1] = 
  {
    {13, Charset36Count, Charset36},
    {61, Charset37Count, Charset37},
    {-1, 0, NULL}
  };
  
  // State: 61, Accept: 26
  const DFAEdge DFAEdgeArray61[2+1] = 
  {
    {13, Charset51Count, Charset51},
    {62, Charset52Count, Charset52},
    {-1, 0, NULL}
  };
  
  // State: 62, Accept: 26
  const DFAEdge DFAEdgeArray62[2+1] = 
  {
    {13, Charset38Count, Charset38},
    {63, Charset20Count, Charset20},
    {-1, 0, NULL}
  };
  
  // State: 63, Accept: 26
  const DFAEdge DFAEdgeArray63[2+1] = 
  {
    {13, Charset53Count, Charset53},
    {64, Charset25Count, Charset25},
    {-1, 0, NULL}
  };
  
  // State: 64, Accept: 26
  const DFAEdge DFAEdgeArray64[2+1] = 
  {
    {13, Charset45Count, Charset45},
    {65, Charset21Count, Charset21},
    {-1, 0, NULL}
  };
  
  // State: 65, Accept: 26
  const DFAEdge DFAEdgeArray65[2+1] = 
  {
    {13, Charset29Count, Charset29},
    {66, Charset17Count, Charset17},
    {-1, 0, NULL}
  };
  
  // State: 66, Accept: 31
  const DFAEdge DFAEdgeArray66[0+1] = 
  {
    {-1, 0, NULL}
  };
  
  // State: 67, Accept: 26
  const DFAEdge DFAEdgeArray67[2+1] = 
  {
    {13, Charset54Count, Charset54},
    {68, Charset55Count, Charset55},
    {-1, 0, NULL}
  };
  
  // State: 68, Accept: 26
  const DFAEdge DFAEdgeArray68[2+1] = 
  {
    {13, Charset34Count, Charset34},
    {69, Charset35Count, Charset35},
    {-1, 0, NULL}
  };
  
  // State: 69, Accept: 26
  const DFAEdge DFAEdgeArray69[2+1] = 
  {
    {13, Charset36Count, Charset36},
    {70, Charset37Count, Charset37},
    {-1, 0, NULL}
  };
  
  // State: 70, Accept: 26
  const DFAEdge DFAEdgeArray70[2+1] = 
  {
    {13, Charset47Count, Charset47},
    {71, Charset48Count, Charset48},
    {-1, 0, NULL}
  };
  
  // State: 71, Accept: 26
  const DFAEdge DFAEdgeArray71[2+1] = 
  {
    {13, Charset29Count, Charset29},
    {72, Charset17Count, Charset17},
    {-1, 0, NULL}
  };
  
  // State: 72, Accept: 32
  const DFAEdge DFAEdgeArray72[0+1] = 
  {
    {-1, 0, NULL}
  };
  
  // State: 73, Accept: 26
  const DFAEdge DFAEdgeArray73[2+1] = 
  {
    {13, Charset53Count, Charset53},
    {74, Charset25Count, Charset25},
    {-1, 0, NULL}
  };
  
  // State: 74, Accept: 26
  const DFAEdge DFAEdgeArray74[2+1] = 
  {
    {13, Charset56Count, Charset56},
    {75, Charset50Count, Charset50},
    {-1, 0, NULL}
  };
  
  // State: 75, Accept: 26
  const DFAEdge DFAEdgeArray75[2+1] = 
  {
    {13, Charset41Count, Charset41},
    {76, Charset42Count, Charset42},
    {-1, 0, NULL}
  };
  
  // State: 76, Accept: 26
  const DFAEdge DFAEdgeArray76[2+1] = 
  {
    {13, Charset47Count, Charset47},
    {77, Charset48Count, Charset48},
    {-1, 0, NULL}
  };
  
  // State: 77, Accept: 26
  const DFAEdge DFAEdgeArray77[2+1] = 
  {
    {13, Charset53Count, Charset53},
    {78, Charset25Count, Charset25},
    {-1, 0, NULL}
  };
  
  // State: 78, Accept: 34
  const DFAEdge DFAEdgeArray78[1+1] = 
  {
    {13, Charset29Count, Charset29},
    {-1, 0, NULL}
  };
  
  // State: 79, Accept: 26
  const DFAEdge DFAEdgeArray79[2+1] = 
  {
    {13, Charset57Count, Charset57},
    {80, Charset58Count, Charset58},
    {-1, 0, NULL}
  };
  
  // State: 80, Accept: 26
  const DFAEdge DFAEdgeArray80[2+1] = 
  {
    {13, Charset46Count, Charset46},
    {81, Charset23Count, Charset23},
    {-1, 0, NULL}
  };
  
  // State: 81, Accept: 26
  const DFAEdge DFAEdgeArray81[2+1] = 
  {
    {13, Charset45Count, Charset45},
    {82, Charset21Count, Charset21},
    {-1, 0, NULL}
  };
  
  // State: 82, Accept: 26
  const DFAEdge DFAEdgeArray82[2+1] = 
  {
    {13, Charset59Count, Charset59},
    {83, Charset60Count, Charset60},
    {-1, 0, NULL}
  };
  
  // State: 83, Accept: 26
  const DFAEdge DFAEdgeArray83[2+1] = 
  {
    {13, Charset45Count, Charset45},
    {84, Charset21Count, Charset21},
    {-1, 0, NULL}
  };
  
  // State: 84, Accept: 26
  const DFAEdge DFAEdgeArray84[2+1] = 
  {
    {13, Charset61Count, Charset61},
    {85, Charset28Count, Charset28},
    {-1, 0, NULL}
  };
  
  // State: 85, Accept: 36
  const DFAEdge DFAEdgeArray85[1+1] = 
  {
    {13, Charset29Count, Charset29},
    {-1, 0, NULL}
  };
  
  // State: 86, Accept: 15
  const DFAEdge DFAEdgeArray86[1+1] = 
  {
    {87, Charset16Count, Charset16},
    {-1, 0, NULL}
  };
  
  // State: 87, Accept: -1
  const DFAEdge DFAEdgeArray87[2+1] = 
  {
    {87, Charset16Count, Charset16},
    {88, Charset27Count, Charset27},
    {-1, 0, NULL}
  };
  
  // State: 88, Accept: 35
  const DFAEdge DFAEdgeArray88[0+1] = 
  {
    {-1, 0, NULL}
  };
  
  // State: 89, Accept: 16
  const DFAEdge DFAEdgeArray89[1+1] = 
  {
    {90, Charset5Count, Charset5},
    {-1, 0, NULL}
  };
  
  // State: 90, Accept: 17
  const DFAEdge DFAEdgeArray90[0+1] = 
  {
    {-1, 0, NULL}
  };
  


  // --------------------------------------------------------------------
  // Rules
  // --------------------------------------------------------------------

  // <CustomOptionKeyword> ::= ModKeyword
  const int RuleSymbolArray0[1+1] = 
  {
    29,
    -1
  };
  
  // <CustomOptionKeyword> ::= ModKeyword ( IdentifierTerm )
  const int RuleSymbolArray1[4+1] = 
  {
    29,
    7,
    26,
    8,
    -1
  };
  
  // <CustomOptionKeyword> ::= ModKeyword ( StringLiteral )
  const int RuleSymbolArray2[4+1] = 
  {
    29,
    7,
    33,
    8,
    -1
  };
  
  // <CustomOptionKeyword> ::= ModKeyword ( DecLiteral )
  const int RuleSymbolArray3[4+1] = 
  {
    29,
    7,
    23,
    8,
    -1
  };
  
  // <VarIdentifier> ::= IdStar IdentifierTerm
  const int RuleSymbolArray4[2+1] = 
  {
    28,
    26,
    -1
  };
  
  // <VarIdentifier> ::= IdRef IdentifierTerm
  const int RuleSymbolArray5[2+1] = 
  {
    27,
    26,
    -1
  };
  
  // <VarIdentifier> ::= IdentifierTerm
  const int RuleSymbolArray6[1+1] = 
  {
    26,
    -1
  };
  
  // <VarCascade> ::= IdentifierTerm :: <VarCascade>
  const int RuleSymbolArray7[3+1] = 
  {
    26,
    11,
    59,
    -1
  };
  
  // <VarCascade> ::= IdentifierTerm
  const int RuleSymbolArray8[1+1] = 
  {
    26,
    -1
  };
  
  // <VarType> ::= <VarCascade>
  const int RuleSymbolArray9[1+1] = 
  {
    59,
    -1
  };
  
  // <VarType> ::= <CustomOptionKeyword>
  const int RuleSymbolArray10[1+1] = 
  {
    38,
    -1
  };
  
  // <VarTypeList> ::= <VarType>
  const int RuleSymbolArray11[1+1] = 
  {
    64,
    -1
  };
  
  // <VarTypeList> ::= <VarTypeList> <VarType>
  const int RuleSymbolArray12[2+1] = 
  {
    65,
    64,
    -1
  };
  
  // <VarSpec> ::= <VarIdentifier>
  const int RuleSymbolArray13[1+1] = 
  {
    61,
    -1
  };
  
  // <VarSpec> ::= <VarIdentifier> ArraySpecifier
  const int RuleSymbolArray14[2+1] = 
  {
    61,
    22,
    -1
  };
  
  // <VarSpec> ::= <VarIdentifier> [ IdentifierTerm ]
  const int RuleSymbolArray15[4+1] = 
  {
    61,
    13,
    26,
    14,
    -1
  };
  
  // <VarSpecs> ::= <VarSpec>
  const int RuleSymbolArray16[1+1] = 
  {
    62,
    -1
  };
  
  // <VarSpecs> ::= <VarSpecs> , <VarSpec>
  const int RuleSymbolArray17[3+1] = 
  {
    63,
    9,
    62,
    -1
  };
  
  // <EnumExprValue> ::= DecLiteral
  const int RuleSymbolArray18[1+1] = 
  {
    23,
    -1
  };
  
  // <EnumExprValue> ::= IdentifierTerm
  const int RuleSymbolArray19[1+1] = 
  {
    26,
    -1
  };
  
  // <EnumExpr> ::= <EnumExpr> + <EnumExprValue>
  const int RuleSymbolArray20[3+1] = 
  {
    43,
    18,
    44,
    -1
  };
  
  // <EnumExpr> ::= <EnumExpr> - <EnumExprValue>
  const int RuleSymbolArray21[3+1] = 
  {
    43,
    6,
    44,
    -1
  };
  
  // <EnumExpr> ::= <EnumExpr> IdStar <EnumExprValue>
  const int RuleSymbolArray22[3+1] = 
  {
    43,
    28,
    44,
    -1
  };
  
  // <EnumExpr> ::= <EnumExprValue>
  const int RuleSymbolArray23[1+1] = 
  {
    44,
    -1
  };
  
  // <Enum> ::= IdentifierTerm
  const int RuleSymbolArray24[1+1] = 
  {
    26,
    -1
  };
  
  // <Enum> ::= IdentifierTerm = <EnumExpr>
  const int RuleSymbolArray25[3+1] = 
  {
    26,
    19,
    43,
    -1
  };
  
  // <Enums> ::= <Enum>
  const int RuleSymbolArray26[1+1] = 
  {
    42,
    -1
  };
  
  // <Enums> ::= <Enums> , <Enum>
  const int RuleSymbolArray27[3+1] = 
  {
    45,
    9,
    42,
    -1
  };
  
  // <VarDecl> ::= <VarTypeList> <VarSpecs> ;
  const int RuleSymbolArray28[3+1] = 
  {
    65,
    63,
    12,
    -1
  };
  
  // <VarDecl> ::= enum IdentifierTerm { <Enums> };
  const int RuleSymbolArray29[5+1] = 
  {
    24,
    26,
    15,
    45,
    17,
    -1
  };
  
  // <FnIsConst> ::= IdentifierTerm
  const int RuleSymbolArray30[1+1] = 
  {
    26,
    -1
  };
  
  // <FnIsConst> ::= 
  const int RuleSymbolArray31[0+1] = 
  {
    -1
  };
  
  // <FnNumLiteral> ::= FloatLiteral
  const int RuleSymbolArray32[1+1] = 
  {
    25,
    -1
  };
  
  // <FnNumLiteral> ::= DecLiteral
  const int RuleSymbolArray33[1+1] = 
  {
    23,
    -1
  };
  
  // <FnLiteral> ::= <FnNumLiteral>
  const int RuleSymbolArray34[1+1] = 
  {
    54,
    -1
  };
  
  // <FnLiteral> ::= IdentifierTerm
  const int RuleSymbolArray35[1+1] = 
  {
    26,
    -1
  };
  
  // <FnLiterals> ::= <FnLiteral>
  const int RuleSymbolArray36[1+1] = 
  {
    50,
    -1
  };
  
  // <FnLiterals> ::= <FnLiterals> , <FnLiteral>
  const int RuleSymbolArray37[3+1] = 
  {
    51,
    9,
    50,
    -1
  };
  
  // <FnArg> ::= <VarTypeList> <VarIdentifier>
  const int RuleSymbolArray38[2+1] = 
  {
    65,
    61,
    -1
  };
  
  // <FnArg> ::= <VarTypeList> <VarIdentifier> = <VarCascade> ( <FnLiterals> )
  const int RuleSymbolArray39[7+1] = 
  {
    65,
    61,
    19,
    59,
    7,
    51,
    8,
    -1
  };
  
  // <FnArg> ::= <VarTypeList> <VarIdentifier> = <FnNumLiteral>
  const int RuleSymbolArray40[4+1] = 
  {
    65,
    61,
    19,
    54,
    -1
  };
  
  // <FnArgs> ::= <FnArg>
  const int RuleSymbolArray41[1+1] = 
  {
    46,
    -1
  };
  
  // <FnArgs> ::= <FnArgs> , <FnArg>
  const int RuleSymbolArray42[3+1] = 
  {
    47,
    9,
    46,
    -1
  };
  
  // <FnArgs> ::= 
  const int RuleSymbolArray43[0+1] = 
  {
    -1
  };
  
  // <FnName> ::= IdStar IdentifierTerm
  const int RuleSymbolArray44[2+1] = 
  {
    28,
    26,
    -1
  };
  
  // <FnName> ::= IdRef IdentifierTerm
  const int RuleSymbolArray45[2+1] = 
  {
    27,
    26,
    -1
  };
  
  // <FnName> ::= IdentifierTerm
  const int RuleSymbolArray46[1+1] = 
  {
    26,
    -1
  };
  
  // <FnNameAndReturn> ::= <VarTypeList> <FnName>
  const int RuleSymbolArray47[2+1] = 
  {
    65,
    52,
    -1
  };
  
  // <FnNameAndReturn> ::= <FnName>
  const int RuleSymbolArray48[1+1] = 
  {
    52,
    -1
  };
  
  // <FnDecl> ::= + <FnNameAndReturn> ( <FnArgs> ) <FnIsConst> ;
  const int RuleSymbolArray49[7+1] = 
  {
    18,
    53,
    7,
    47,
    8,
    49,
    12,
    -1
  };
  
  // <DataBlockDef> ::= <VarDecl>
  const int RuleSymbolArray50[1+1] = 
  {
    60,
    -1
  };
  
  // <DataBlockDef> ::= <FnDecl>
  const int RuleSymbolArray51[1+1] = 
  {
    48,
    -1
  };
  
  // <DataBlockDef> ::= private:
  const int RuleSymbolArray52[1+1] = 
  {
    31,
    -1
  };
  
  // <DataBlockDef> ::= public:
  const int RuleSymbolArray53[1+1] = 
  {
    32,
    -1
  };
  
  // <DataBlockDefs> ::= <DataBlockDef>
  const int RuleSymbolArray54[1+1] = 
  {
    40,
    -1
  };
  
  // <DataBlockDefs> ::= <DataBlockDefs> <DataBlockDef>
  const int RuleSymbolArray55[2+1] = 
  {
    41,
    40,
    -1
  };
  
  // <CustomOptions> ::= <CustomOptionKeyword>
  const int RuleSymbolArray56[1+1] = 
  {
    38,
    -1
  };
  
  // <CustomOptions> ::= <CustomOptions> <CustomOptionKeyword>
  const int RuleSymbolArray57[2+1] = 
  {
    39,
    38,
    -1
  };
  
  // <CustomOption> ::= <CustomOptions>
  const int RuleSymbolArray58[1+1] = 
  {
    39,
    -1
  };
  
  // <CustomOption> ::= 
  const int RuleSymbolArray59[0+1] = 
  {
    -1
  };
  
  // <StructInheritance> ::= : <VarCascade>
  const int RuleSymbolArray60[2+1] = 
  {
    10,
    59,
    -1
  };
  
  // <StructInheritance> ::= 
  const int RuleSymbolArray61[0+1] = 
  {
    -1
  };
  
  // <Root> ::= namespace IdentifierTerm { <Roots> }
  const int RuleSymbolArray62[5+1] = 
  {
    30,
    26,
    15,
    56,
    16,
    -1
  };
  
  // <Root> ::= <CustomOption> struct IdentifierTerm <StructInheritance> { <DataBlockDefs> };
  const int RuleSymbolArray63[7+1] = 
  {
    37,
    34,
    26,
    57,
    15,
    41,
    17,
    -1
  };
  
  // <Root> ::= <CustomOption> alias <VarCascade> => IdentifierTerm <CustomOption> ;
  const int RuleSymbolArray64[7+1] = 
  {
    37,
    21,
    59,
    20,
    26,
    37,
    12,
    -1
  };
  
  // <Root> ::= <CustomOption> typedef <VarCascade> TSizeSpecifier <CustomOption> ;
  const int RuleSymbolArray65[6+1] = 
  {
    37,
    36,
    59,
    35,
    37,
    12,
    -1
  };
  
  // <Roots> ::= <Root>
  const int RuleSymbolArray66[1+1] = 
  {
    55,
    -1
  };
  
  // <Roots> ::= <Roots> <Root>
  const int RuleSymbolArray67[2+1] = 
  {
    56,
    55,
    -1
  };
  
  // <TypesRoots> ::= <Roots>
  const int RuleSymbolArray68[1+1] = 
  {
    56,
    -1
  };
  


  // --------------------------------------------------------------------
  // LALR
  //                                             entry, action op, target
  // --------------------------------------------------------------------

  const Action LALRActionArray0[11+1] = 
  {
    {29, Action::Op(1), 1},   // Shift
    {30, Action::Op(1), 2},   // Shift
    {37, Action::Op(3), 3},   // Goto
    {38, Action::Op(3), 4},   // Goto
    {39, Action::Op(3), 5},   // Goto
    {55, Action::Op(3), 6},   // Goto
    {56, Action::Op(3), 7},   // Goto
    {58, Action::Op(3), 8},   // Goto
    {21, Action::Op(2), 59},   // Reduce Rule
    {34, Action::Op(2), 59},   // Reduce Rule
    {36, Action::Op(2), 59},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray1[9+1] = 
  {
    {7, Action::Op(1), 9},   // Shift
    {12, Action::Op(2), 0},   // Reduce Rule
    {21, Action::Op(2), 0},   // Reduce Rule
    {26, Action::Op(2), 0},   // Reduce Rule
    {27, Action::Op(2), 0},   // Reduce Rule
    {28, Action::Op(2), 0},   // Reduce Rule
    {29, Action::Op(2), 0},   // Reduce Rule
    {34, Action::Op(2), 0},   // Reduce Rule
    {36, Action::Op(2), 0},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray2[1+1] = 
  {
    {26, Action::Op(1), 10},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray3[3+1] = 
  {
    {21, Action::Op(1), 11},   // Shift
    {34, Action::Op(1), 12},   // Shift
    {36, Action::Op(1), 13},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray4[5+1] = 
  {
    {12, Action::Op(2), 56},   // Reduce Rule
    {21, Action::Op(2), 56},   // Reduce Rule
    {29, Action::Op(2), 56},   // Reduce Rule
    {34, Action::Op(2), 56},   // Reduce Rule
    {36, Action::Op(2), 56},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray5[6+1] = 
  {
    {29, Action::Op(1), 1},   // Shift
    {38, Action::Op(3), 14},   // Goto
    {12, Action::Op(2), 58},   // Reduce Rule
    {21, Action::Op(2), 58},   // Reduce Rule
    {34, Action::Op(2), 58},   // Reduce Rule
    {36, Action::Op(2), 58},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray6[7+1] = 
  {
    {0, Action::Op(2), 66},   // Reduce Rule
    {16, Action::Op(2), 66},   // Reduce Rule
    {21, Action::Op(2), 66},   // Reduce Rule
    {29, Action::Op(2), 66},   // Reduce Rule
    {30, Action::Op(2), 66},   // Reduce Rule
    {34, Action::Op(2), 66},   // Reduce Rule
    {36, Action::Op(2), 66},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray7[10+1] = 
  {
    {29, Action::Op(1), 1},   // Shift
    {30, Action::Op(1), 2},   // Shift
    {37, Action::Op(3), 3},   // Goto
    {38, Action::Op(3), 4},   // Goto
    {39, Action::Op(3), 5},   // Goto
    {55, Action::Op(3), 15},   // Goto
    {0, Action::Op(2), 68},   // Reduce Rule
    {21, Action::Op(2), 59},   // Reduce Rule
    {34, Action::Op(2), 59},   // Reduce Rule
    {36, Action::Op(2), 59},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray8[1+1] = 
  {
    {0, Action::Op(4), 0},   // Accept
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray9[3+1] = 
  {
    {23, Action::Op(1), 16},   // Shift
    {26, Action::Op(1), 17},   // Shift
    {33, Action::Op(1), 18},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray10[1+1] = 
  {
    {15, Action::Op(1), 19},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray11[2+1] = 
  {
    {26, Action::Op(1), 20},   // Shift
    {59, Action::Op(3), 21},   // Goto
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray12[1+1] = 
  {
    {26, Action::Op(1), 22},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray13[2+1] = 
  {
    {26, Action::Op(1), 20},   // Shift
    {59, Action::Op(3), 23},   // Goto
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray14[5+1] = 
  {
    {12, Action::Op(2), 57},   // Reduce Rule
    {21, Action::Op(2), 57},   // Reduce Rule
    {29, Action::Op(2), 57},   // Reduce Rule
    {34, Action::Op(2), 57},   // Reduce Rule
    {36, Action::Op(2), 57},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray15[7+1] = 
  {
    {0, Action::Op(2), 67},   // Reduce Rule
    {16, Action::Op(2), 67},   // Reduce Rule
    {21, Action::Op(2), 67},   // Reduce Rule
    {29, Action::Op(2), 67},   // Reduce Rule
    {30, Action::Op(2), 67},   // Reduce Rule
    {34, Action::Op(2), 67},   // Reduce Rule
    {36, Action::Op(2), 67},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray16[1+1] = 
  {
    {8, Action::Op(1), 24},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray17[1+1] = 
  {
    {8, Action::Op(1), 25},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray18[1+1] = 
  {
    {8, Action::Op(1), 26},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray19[10+1] = 
  {
    {29, Action::Op(1), 1},   // Shift
    {30, Action::Op(1), 2},   // Shift
    {37, Action::Op(3), 3},   // Goto
    {38, Action::Op(3), 4},   // Goto
    {39, Action::Op(3), 5},   // Goto
    {55, Action::Op(3), 6},   // Goto
    {56, Action::Op(3), 27},   // Goto
    {21, Action::Op(2), 59},   // Reduce Rule
    {34, Action::Op(2), 59},   // Reduce Rule
    {36, Action::Op(2), 59},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray20[9+1] = 
  {
    {11, Action::Op(1), 28},   // Shift
    {7, Action::Op(2), 8},   // Reduce Rule
    {15, Action::Op(2), 8},   // Reduce Rule
    {20, Action::Op(2), 8},   // Reduce Rule
    {26, Action::Op(2), 8},   // Reduce Rule
    {27, Action::Op(2), 8},   // Reduce Rule
    {28, Action::Op(2), 8},   // Reduce Rule
    {29, Action::Op(2), 8},   // Reduce Rule
    {35, Action::Op(2), 8},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray21[1+1] = 
  {
    {20, Action::Op(1), 29},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray22[3+1] = 
  {
    {10, Action::Op(1), 30},   // Shift
    {57, Action::Op(3), 31},   // Goto
    {15, Action::Op(2), 61},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray23[1+1] = 
  {
    {35, Action::Op(1), 32},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray24[8+1] = 
  {
    {12, Action::Op(2), 3},   // Reduce Rule
    {21, Action::Op(2), 3},   // Reduce Rule
    {26, Action::Op(2), 3},   // Reduce Rule
    {27, Action::Op(2), 3},   // Reduce Rule
    {28, Action::Op(2), 3},   // Reduce Rule
    {29, Action::Op(2), 3},   // Reduce Rule
    {34, Action::Op(2), 3},   // Reduce Rule
    {36, Action::Op(2), 3},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray25[8+1] = 
  {
    {12, Action::Op(2), 1},   // Reduce Rule
    {21, Action::Op(2), 1},   // Reduce Rule
    {26, Action::Op(2), 1},   // Reduce Rule
    {27, Action::Op(2), 1},   // Reduce Rule
    {28, Action::Op(2), 1},   // Reduce Rule
    {29, Action::Op(2), 1},   // Reduce Rule
    {34, Action::Op(2), 1},   // Reduce Rule
    {36, Action::Op(2), 1},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray26[8+1] = 
  {
    {12, Action::Op(2), 2},   // Reduce Rule
    {21, Action::Op(2), 2},   // Reduce Rule
    {26, Action::Op(2), 2},   // Reduce Rule
    {27, Action::Op(2), 2},   // Reduce Rule
    {28, Action::Op(2), 2},   // Reduce Rule
    {29, Action::Op(2), 2},   // Reduce Rule
    {34, Action::Op(2), 2},   // Reduce Rule
    {36, Action::Op(2), 2},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray27[10+1] = 
  {
    {16, Action::Op(1), 33},   // Shift
    {29, Action::Op(1), 1},   // Shift
    {30, Action::Op(1), 2},   // Shift
    {37, Action::Op(3), 3},   // Goto
    {38, Action::Op(3), 4},   // Goto
    {39, Action::Op(3), 5},   // Goto
    {55, Action::Op(3), 15},   // Goto
    {21, Action::Op(2), 59},   // Reduce Rule
    {34, Action::Op(2), 59},   // Reduce Rule
    {36, Action::Op(2), 59},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray28[2+1] = 
  {
    {26, Action::Op(1), 20},   // Shift
    {59, Action::Op(3), 34},   // Goto
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray29[1+1] = 
  {
    {26, Action::Op(1), 35},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray30[2+1] = 
  {
    {26, Action::Op(1), 20},   // Shift
    {59, Action::Op(3), 36},   // Goto
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray31[1+1] = 
  {
    {15, Action::Op(1), 37},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray32[5+1] = 
  {
    {29, Action::Op(1), 1},   // Shift
    {37, Action::Op(3), 38},   // Goto
    {38, Action::Op(3), 4},   // Goto
    {39, Action::Op(3), 5},   // Goto
    {12, Action::Op(2), 59},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray33[7+1] = 
  {
    {0, Action::Op(2), 62},   // Reduce Rule
    {16, Action::Op(2), 62},   // Reduce Rule
    {21, Action::Op(2), 62},   // Reduce Rule
    {29, Action::Op(2), 62},   // Reduce Rule
    {30, Action::Op(2), 62},   // Reduce Rule
    {34, Action::Op(2), 62},   // Reduce Rule
    {36, Action::Op(2), 62},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray34[8+1] = 
  {
    {7, Action::Op(2), 7},   // Reduce Rule
    {15, Action::Op(2), 7},   // Reduce Rule
    {20, Action::Op(2), 7},   // Reduce Rule
    {26, Action::Op(2), 7},   // Reduce Rule
    {27, Action::Op(2), 7},   // Reduce Rule
    {28, Action::Op(2), 7},   // Reduce Rule
    {29, Action::Op(2), 7},   // Reduce Rule
    {35, Action::Op(2), 7},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray35[5+1] = 
  {
    {29, Action::Op(1), 1},   // Shift
    {37, Action::Op(3), 39},   // Goto
    {38, Action::Op(3), 4},   // Goto
    {39, Action::Op(3), 5},   // Goto
    {12, Action::Op(2), 59},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray36[1+1] = 
  {
    {15, Action::Op(2), 60},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray37[14+1] = 
  {
    {18, Action::Op(1), 40},   // Shift
    {24, Action::Op(1), 41},   // Shift
    {26, Action::Op(1), 20},   // Shift
    {29, Action::Op(1), 1},   // Shift
    {31, Action::Op(1), 42},   // Shift
    {32, Action::Op(1), 43},   // Shift
    {38, Action::Op(3), 44},   // Goto
    {40, Action::Op(3), 45},   // Goto
    {41, Action::Op(3), 46},   // Goto
    {48, Action::Op(3), 47},   // Goto
    {59, Action::Op(3), 48},   // Goto
    {60, Action::Op(3), 49},   // Goto
    {64, Action::Op(3), 50},   // Goto
    {65, Action::Op(3), 51},   // Goto
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray38[1+1] = 
  {
    {12, Action::Op(1), 52},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray39[1+1] = 
  {
    {12, Action::Op(1), 53},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray40[10+1] = 
  {
    {26, Action::Op(1), 54},   // Shift
    {27, Action::Op(1), 55},   // Shift
    {28, Action::Op(1), 56},   // Shift
    {29, Action::Op(1), 1},   // Shift
    {38, Action::Op(3), 44},   // Goto
    {52, Action::Op(3), 57},   // Goto
    {53, Action::Op(3), 58},   // Goto
    {59, Action::Op(3), 48},   // Goto
    {64, Action::Op(3), 50},   // Goto
    {65, Action::Op(3), 59},   // Goto
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray41[1+1] = 
  {
    {26, Action::Op(1), 60},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray42[7+1] = 
  {
    {17, Action::Op(2), 52},   // Reduce Rule
    {18, Action::Op(2), 52},   // Reduce Rule
    {24, Action::Op(2), 52},   // Reduce Rule
    {26, Action::Op(2), 52},   // Reduce Rule
    {29, Action::Op(2), 52},   // Reduce Rule
    {31, Action::Op(2), 52},   // Reduce Rule
    {32, Action::Op(2), 52},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray43[7+1] = 
  {
    {17, Action::Op(2), 53},   // Reduce Rule
    {18, Action::Op(2), 53},   // Reduce Rule
    {24, Action::Op(2), 53},   // Reduce Rule
    {26, Action::Op(2), 53},   // Reduce Rule
    {29, Action::Op(2), 53},   // Reduce Rule
    {31, Action::Op(2), 53},   // Reduce Rule
    {32, Action::Op(2), 53},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray44[4+1] = 
  {
    {26, Action::Op(2), 10},   // Reduce Rule
    {27, Action::Op(2), 10},   // Reduce Rule
    {28, Action::Op(2), 10},   // Reduce Rule
    {29, Action::Op(2), 10},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray45[7+1] = 
  {
    {17, Action::Op(2), 54},   // Reduce Rule
    {18, Action::Op(2), 54},   // Reduce Rule
    {24, Action::Op(2), 54},   // Reduce Rule
    {26, Action::Op(2), 54},   // Reduce Rule
    {29, Action::Op(2), 54},   // Reduce Rule
    {31, Action::Op(2), 54},   // Reduce Rule
    {32, Action::Op(2), 54},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray46[14+1] = 
  {
    {17, Action::Op(1), 61},   // Shift
    {18, Action::Op(1), 40},   // Shift
    {24, Action::Op(1), 41},   // Shift
    {26, Action::Op(1), 20},   // Shift
    {29, Action::Op(1), 1},   // Shift
    {31, Action::Op(1), 42},   // Shift
    {32, Action::Op(1), 43},   // Shift
    {38, Action::Op(3), 44},   // Goto
    {40, Action::Op(3), 62},   // Goto
    {48, Action::Op(3), 47},   // Goto
    {59, Action::Op(3), 48},   // Goto
    {60, Action::Op(3), 49},   // Goto
    {64, Action::Op(3), 50},   // Goto
    {65, Action::Op(3), 51},   // Goto
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray47[7+1] = 
  {
    {17, Action::Op(2), 51},   // Reduce Rule
    {18, Action::Op(2), 51},   // Reduce Rule
    {24, Action::Op(2), 51},   // Reduce Rule
    {26, Action::Op(2), 51},   // Reduce Rule
    {29, Action::Op(2), 51},   // Reduce Rule
    {31, Action::Op(2), 51},   // Reduce Rule
    {32, Action::Op(2), 51},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray48[4+1] = 
  {
    {26, Action::Op(2), 9},   // Reduce Rule
    {27, Action::Op(2), 9},   // Reduce Rule
    {28, Action::Op(2), 9},   // Reduce Rule
    {29, Action::Op(2), 9},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray49[7+1] = 
  {
    {17, Action::Op(2), 50},   // Reduce Rule
    {18, Action::Op(2), 50},   // Reduce Rule
    {24, Action::Op(2), 50},   // Reduce Rule
    {26, Action::Op(2), 50},   // Reduce Rule
    {29, Action::Op(2), 50},   // Reduce Rule
    {31, Action::Op(2), 50},   // Reduce Rule
    {32, Action::Op(2), 50},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray50[4+1] = 
  {
    {26, Action::Op(2), 11},   // Reduce Rule
    {27, Action::Op(2), 11},   // Reduce Rule
    {28, Action::Op(2), 11},   // Reduce Rule
    {29, Action::Op(2), 11},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray51[10+1] = 
  {
    {26, Action::Op(1), 63},   // Shift
    {27, Action::Op(1), 64},   // Shift
    {28, Action::Op(1), 65},   // Shift
    {29, Action::Op(1), 1},   // Shift
    {38, Action::Op(3), 44},   // Goto
    {59, Action::Op(3), 48},   // Goto
    {61, Action::Op(3), 66},   // Goto
    {62, Action::Op(3), 67},   // Goto
    {63, Action::Op(3), 68},   // Goto
    {64, Action::Op(3), 69},   // Goto
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray52[7+1] = 
  {
    {0, Action::Op(2), 65},   // Reduce Rule
    {16, Action::Op(2), 65},   // Reduce Rule
    {21, Action::Op(2), 65},   // Reduce Rule
    {29, Action::Op(2), 65},   // Reduce Rule
    {30, Action::Op(2), 65},   // Reduce Rule
    {34, Action::Op(2), 65},   // Reduce Rule
    {36, Action::Op(2), 65},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray53[7+1] = 
  {
    {0, Action::Op(2), 64},   // Reduce Rule
    {16, Action::Op(2), 64},   // Reduce Rule
    {21, Action::Op(2), 64},   // Reduce Rule
    {29, Action::Op(2), 64},   // Reduce Rule
    {30, Action::Op(2), 64},   // Reduce Rule
    {34, Action::Op(2), 64},   // Reduce Rule
    {36, Action::Op(2), 64},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray54[6+1] = 
  {
    {11, Action::Op(1), 28},   // Shift
    {26, Action::Op(2), 8},   // Reduce Rule
    {27, Action::Op(2), 8},   // Reduce Rule
    {28, Action::Op(2), 8},   // Reduce Rule
    {29, Action::Op(2), 8},   // Reduce Rule
    {7, Action::Op(2), 46},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray55[1+1] = 
  {
    {26, Action::Op(1), 70},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray56[1+1] = 
  {
    {26, Action::Op(1), 71},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray57[1+1] = 
  {
    {7, Action::Op(2), 48},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray58[1+1] = 
  {
    {7, Action::Op(1), 72},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray59[8+1] = 
  {
    {26, Action::Op(1), 54},   // Shift
    {27, Action::Op(1), 55},   // Shift
    {28, Action::Op(1), 56},   // Shift
    {29, Action::Op(1), 1},   // Shift
    {38, Action::Op(3), 44},   // Goto
    {52, Action::Op(3), 73},   // Goto
    {59, Action::Op(3), 48},   // Goto
    {64, Action::Op(3), 69},   // Goto
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray60[1+1] = 
  {
    {15, Action::Op(1), 74},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray61[7+1] = 
  {
    {0, Action::Op(2), 63},   // Reduce Rule
    {16, Action::Op(2), 63},   // Reduce Rule
    {21, Action::Op(2), 63},   // Reduce Rule
    {29, Action::Op(2), 63},   // Reduce Rule
    {30, Action::Op(2), 63},   // Reduce Rule
    {34, Action::Op(2), 63},   // Reduce Rule
    {36, Action::Op(2), 63},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray62[7+1] = 
  {
    {17, Action::Op(2), 55},   // Reduce Rule
    {18, Action::Op(2), 55},   // Reduce Rule
    {24, Action::Op(2), 55},   // Reduce Rule
    {26, Action::Op(2), 55},   // Reduce Rule
    {29, Action::Op(2), 55},   // Reduce Rule
    {31, Action::Op(2), 55},   // Reduce Rule
    {32, Action::Op(2), 55},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray63[11+1] = 
  {
    {11, Action::Op(1), 28},   // Shift
    {8, Action::Op(2), 6},   // Reduce Rule
    {9, Action::Op(2), 6},   // Reduce Rule
    {12, Action::Op(2), 6},   // Reduce Rule
    {13, Action::Op(2), 6},   // Reduce Rule
    {19, Action::Op(2), 6},   // Reduce Rule
    {22, Action::Op(2), 6},   // Reduce Rule
    {26, Action::Op(2), 8},   // Reduce Rule
    {27, Action::Op(2), 8},   // Reduce Rule
    {28, Action::Op(2), 8},   // Reduce Rule
    {29, Action::Op(2), 8},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray64[1+1] = 
  {
    {26, Action::Op(1), 75},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray65[1+1] = 
  {
    {26, Action::Op(1), 76},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray66[4+1] = 
  {
    {13, Action::Op(1), 77},   // Shift
    {22, Action::Op(1), 78},   // Shift
    {9, Action::Op(2), 13},   // Reduce Rule
    {12, Action::Op(2), 13},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray67[2+1] = 
  {
    {9, Action::Op(2), 16},   // Reduce Rule
    {12, Action::Op(2), 16},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray68[2+1] = 
  {
    {9, Action::Op(1), 79},   // Shift
    {12, Action::Op(1), 80},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray69[4+1] = 
  {
    {26, Action::Op(2), 12},   // Reduce Rule
    {27, Action::Op(2), 12},   // Reduce Rule
    {28, Action::Op(2), 12},   // Reduce Rule
    {29, Action::Op(2), 12},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray70[1+1] = 
  {
    {7, Action::Op(2), 45},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray71[1+1] = 
  {
    {7, Action::Op(2), 44},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray72[10+1] = 
  {
    {26, Action::Op(1), 20},   // Shift
    {29, Action::Op(1), 1},   // Shift
    {38, Action::Op(3), 44},   // Goto
    {46, Action::Op(3), 81},   // Goto
    {47, Action::Op(3), 82},   // Goto
    {59, Action::Op(3), 48},   // Goto
    {64, Action::Op(3), 50},   // Goto
    {65, Action::Op(3), 83},   // Goto
    {8, Action::Op(2), 43},   // Reduce Rule
    {9, Action::Op(2), 43},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray73[1+1] = 
  {
    {7, Action::Op(2), 47},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray74[3+1] = 
  {
    {26, Action::Op(1), 84},   // Shift
    {42, Action::Op(3), 85},   // Goto
    {45, Action::Op(3), 86},   // Goto
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray75[6+1] = 
  {
    {8, Action::Op(2), 5},   // Reduce Rule
    {9, Action::Op(2), 5},   // Reduce Rule
    {12, Action::Op(2), 5},   // Reduce Rule
    {13, Action::Op(2), 5},   // Reduce Rule
    {19, Action::Op(2), 5},   // Reduce Rule
    {22, Action::Op(2), 5},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray76[6+1] = 
  {
    {8, Action::Op(2), 4},   // Reduce Rule
    {9, Action::Op(2), 4},   // Reduce Rule
    {12, Action::Op(2), 4},   // Reduce Rule
    {13, Action::Op(2), 4},   // Reduce Rule
    {19, Action::Op(2), 4},   // Reduce Rule
    {22, Action::Op(2), 4},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray77[1+1] = 
  {
    {26, Action::Op(1), 87},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray78[2+1] = 
  {
    {9, Action::Op(2), 14},   // Reduce Rule
    {12, Action::Op(2), 14},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray79[5+1] = 
  {
    {26, Action::Op(1), 88},   // Shift
    {27, Action::Op(1), 64},   // Shift
    {28, Action::Op(1), 65},   // Shift
    {61, Action::Op(3), 66},   // Goto
    {62, Action::Op(3), 89},   // Goto
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray80[7+1] = 
  {
    {17, Action::Op(2), 28},   // Reduce Rule
    {18, Action::Op(2), 28},   // Reduce Rule
    {24, Action::Op(2), 28},   // Reduce Rule
    {26, Action::Op(2), 28},   // Reduce Rule
    {29, Action::Op(2), 28},   // Reduce Rule
    {31, Action::Op(2), 28},   // Reduce Rule
    {32, Action::Op(2), 28},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray81[2+1] = 
  {
    {8, Action::Op(2), 41},   // Reduce Rule
    {9, Action::Op(2), 41},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray82[2+1] = 
  {
    {8, Action::Op(1), 90},   // Shift
    {9, Action::Op(1), 91},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray83[8+1] = 
  {
    {26, Action::Op(1), 63},   // Shift
    {27, Action::Op(1), 64},   // Shift
    {28, Action::Op(1), 65},   // Shift
    {29, Action::Op(1), 1},   // Shift
    {38, Action::Op(3), 44},   // Goto
    {59, Action::Op(3), 48},   // Goto
    {61, Action::Op(3), 92},   // Goto
    {64, Action::Op(3), 69},   // Goto
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray84[3+1] = 
  {
    {19, Action::Op(1), 93},   // Shift
    {9, Action::Op(2), 24},   // Reduce Rule
    {17, Action::Op(2), 24},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray85[2+1] = 
  {
    {9, Action::Op(2), 26},   // Reduce Rule
    {17, Action::Op(2), 26},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray86[2+1] = 
  {
    {9, Action::Op(1), 94},   // Shift
    {17, Action::Op(1), 95},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray87[1+1] = 
  {
    {14, Action::Op(1), 96},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray88[4+1] = 
  {
    {9, Action::Op(2), 6},   // Reduce Rule
    {12, Action::Op(2), 6},   // Reduce Rule
    {13, Action::Op(2), 6},   // Reduce Rule
    {22, Action::Op(2), 6},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray89[2+1] = 
  {
    {9, Action::Op(2), 17},   // Reduce Rule
    {12, Action::Op(2), 17},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray90[3+1] = 
  {
    {26, Action::Op(1), 97},   // Shift
    {49, Action::Op(3), 98},   // Goto
    {12, Action::Op(2), 31},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray91[7+1] = 
  {
    {26, Action::Op(1), 20},   // Shift
    {29, Action::Op(1), 1},   // Shift
    {38, Action::Op(3), 44},   // Goto
    {46, Action::Op(3), 99},   // Goto
    {59, Action::Op(3), 48},   // Goto
    {64, Action::Op(3), 50},   // Goto
    {65, Action::Op(3), 83},   // Goto
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray92[3+1] = 
  {
    {19, Action::Op(1), 100},   // Shift
    {8, Action::Op(2), 38},   // Reduce Rule
    {9, Action::Op(2), 38},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray93[4+1] = 
  {
    {23, Action::Op(1), 101},   // Shift
    {26, Action::Op(1), 102},   // Shift
    {43, Action::Op(3), 103},   // Goto
    {44, Action::Op(3), 104},   // Goto
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray94[2+1] = 
  {
    {26, Action::Op(1), 84},   // Shift
    {42, Action::Op(3), 105},   // Goto
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray95[7+1] = 
  {
    {17, Action::Op(2), 29},   // Reduce Rule
    {18, Action::Op(2), 29},   // Reduce Rule
    {24, Action::Op(2), 29},   // Reduce Rule
    {26, Action::Op(2), 29},   // Reduce Rule
    {29, Action::Op(2), 29},   // Reduce Rule
    {31, Action::Op(2), 29},   // Reduce Rule
    {32, Action::Op(2), 29},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray96[2+1] = 
  {
    {9, Action::Op(2), 15},   // Reduce Rule
    {12, Action::Op(2), 15},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray97[1+1] = 
  {
    {12, Action::Op(2), 30},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray98[1+1] = 
  {
    {12, Action::Op(1), 106},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray99[2+1] = 
  {
    {8, Action::Op(2), 42},   // Reduce Rule
    {9, Action::Op(2), 42},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray100[5+1] = 
  {
    {23, Action::Op(1), 107},   // Shift
    {25, Action::Op(1), 108},   // Shift
    {26, Action::Op(1), 20},   // Shift
    {54, Action::Op(3), 109},   // Goto
    {59, Action::Op(3), 110},   // Goto
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray101[5+1] = 
  {
    {6, Action::Op(2), 18},   // Reduce Rule
    {9, Action::Op(2), 18},   // Reduce Rule
    {17, Action::Op(2), 18},   // Reduce Rule
    {18, Action::Op(2), 18},   // Reduce Rule
    {28, Action::Op(2), 18},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray102[5+1] = 
  {
    {6, Action::Op(2), 19},   // Reduce Rule
    {9, Action::Op(2), 19},   // Reduce Rule
    {17, Action::Op(2), 19},   // Reduce Rule
    {18, Action::Op(2), 19},   // Reduce Rule
    {28, Action::Op(2), 19},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray103[5+1] = 
  {
    {6, Action::Op(1), 111},   // Shift
    {18, Action::Op(1), 112},   // Shift
    {28, Action::Op(1), 113},   // Shift
    {9, Action::Op(2), 25},   // Reduce Rule
    {17, Action::Op(2), 25},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray104[5+1] = 
  {
    {6, Action::Op(2), 23},   // Reduce Rule
    {9, Action::Op(2), 23},   // Reduce Rule
    {17, Action::Op(2), 23},   // Reduce Rule
    {18, Action::Op(2), 23},   // Reduce Rule
    {28, Action::Op(2), 23},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray105[2+1] = 
  {
    {9, Action::Op(2), 27},   // Reduce Rule
    {17, Action::Op(2), 27},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray106[7+1] = 
  {
    {17, Action::Op(2), 49},   // Reduce Rule
    {18, Action::Op(2), 49},   // Reduce Rule
    {24, Action::Op(2), 49},   // Reduce Rule
    {26, Action::Op(2), 49},   // Reduce Rule
    {29, Action::Op(2), 49},   // Reduce Rule
    {31, Action::Op(2), 49},   // Reduce Rule
    {32, Action::Op(2), 49},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray107[2+1] = 
  {
    {8, Action::Op(2), 33},   // Reduce Rule
    {9, Action::Op(2), 33},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray108[2+1] = 
  {
    {8, Action::Op(2), 32},   // Reduce Rule
    {9, Action::Op(2), 32},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray109[2+1] = 
  {
    {8, Action::Op(2), 40},   // Reduce Rule
    {9, Action::Op(2), 40},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray110[1+1] = 
  {
    {7, Action::Op(1), 114},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray111[3+1] = 
  {
    {23, Action::Op(1), 101},   // Shift
    {26, Action::Op(1), 102},   // Shift
    {44, Action::Op(3), 115},   // Goto
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray112[3+1] = 
  {
    {23, Action::Op(1), 101},   // Shift
    {26, Action::Op(1), 102},   // Shift
    {44, Action::Op(3), 116},   // Goto
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray113[3+1] = 
  {
    {23, Action::Op(1), 101},   // Shift
    {26, Action::Op(1), 102},   // Shift
    {44, Action::Op(3), 117},   // Goto
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray114[6+1] = 
  {
    {23, Action::Op(1), 107},   // Shift
    {25, Action::Op(1), 108},   // Shift
    {26, Action::Op(1), 118},   // Shift
    {50, Action::Op(3), 119},   // Goto
    {51, Action::Op(3), 120},   // Goto
    {54, Action::Op(3), 121},   // Goto
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray115[5+1] = 
  {
    {6, Action::Op(2), 21},   // Reduce Rule
    {9, Action::Op(2), 21},   // Reduce Rule
    {17, Action::Op(2), 21},   // Reduce Rule
    {18, Action::Op(2), 21},   // Reduce Rule
    {28, Action::Op(2), 21},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray116[5+1] = 
  {
    {6, Action::Op(2), 20},   // Reduce Rule
    {9, Action::Op(2), 20},   // Reduce Rule
    {17, Action::Op(2), 20},   // Reduce Rule
    {18, Action::Op(2), 20},   // Reduce Rule
    {28, Action::Op(2), 20},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray117[5+1] = 
  {
    {6, Action::Op(2), 22},   // Reduce Rule
    {9, Action::Op(2), 22},   // Reduce Rule
    {17, Action::Op(2), 22},   // Reduce Rule
    {18, Action::Op(2), 22},   // Reduce Rule
    {28, Action::Op(2), 22},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray118[2+1] = 
  {
    {8, Action::Op(2), 35},   // Reduce Rule
    {9, Action::Op(2), 35},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray119[2+1] = 
  {
    {8, Action::Op(2), 36},   // Reduce Rule
    {9, Action::Op(2), 36},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray120[2+1] = 
  {
    {8, Action::Op(1), 122},   // Shift
    {9, Action::Op(1), 123},   // Shift
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray121[2+1] = 
  {
    {8, Action::Op(2), 34},   // Reduce Rule
    {9, Action::Op(2), 34},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray122[2+1] = 
  {
    {8, Action::Op(2), 39},   // Reduce Rule
    {9, Action::Op(2), 39},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray123[5+1] = 
  {
    {23, Action::Op(1), 107},   // Shift
    {25, Action::Op(1), 108},   // Shift
    {26, Action::Op(1), 118},   // Shift
    {50, Action::Op(3), 124},   // Goto
    {54, Action::Op(3), 121},   // Goto
    {-1, Action::INVALID, -1}
  };
  
  const Action LALRActionArray124[2+1] = 
  {
    {8, Action::Op(2), 37},   // Reduce Rule
    {9, Action::Op(2), 37},   // Reduce Rule
    {-1, Action::INVALID, -1}
  };
  


  // --------------------------------------------------------------------
  // DFA Table
  //                                accept symbol, edge count, edge array
  // --------------------------------------------------------------------

  const DFAState DFAStateArray[91+1] = 
  {
    /* 0 */   {-1, 28, DFAEdgeArray0},
    /* 1 */   {2, 1, DFAEdgeArray1},
    /* 2 */   {4, 0, DFAEdgeArray2},
    /* 3 */   {7, 0, DFAEdgeArray3},
    /* 4 */   {8, 0, DFAEdgeArray4},
    /* 5 */   {9, 0, DFAEdgeArray5},
    /* 6 */   {12, 0, DFAEdgeArray6},
    /* 7 */   {14, 0, DFAEdgeArray7},
    /* 8 */   {18, 0, DFAEdgeArray8},
    /* 9 */   {-1, 1, DFAEdgeArray9},
    /* 10 */   {-1, 2, DFAEdgeArray10},
    /* 11 */   {25, 0, DFAEdgeArray11},
    /* 12 */   {26, 1, DFAEdgeArray12},
    /* 13 */   {26, 1, DFAEdgeArray13},
    /* 14 */   {27, 0, DFAEdgeArray14},
    /* 15 */   {-1, 1, DFAEdgeArray15},
    /* 16 */   {-1, 2, DFAEdgeArray16},
    /* 17 */   {-1, 2, DFAEdgeArray17},
    /* 18 */   {-1, 2, DFAEdgeArray18},
    /* 19 */   {29, 2, DFAEdgeArray19},
    /* 20 */   {-1, 3, DFAEdgeArray20},
    /* 21 */   {-1, 3, DFAEdgeArray21},
    /* 22 */   {33, 0, DFAEdgeArray22},
    /* 23 */   {-1, 3, DFAEdgeArray23},
    /* 24 */   {-1, 3, DFAEdgeArray24},
    /* 25 */   {33, 3, DFAEdgeArray25},
    /* 26 */   {-1, 3, DFAEdgeArray26},
    /* 27 */   {28, 1, DFAEdgeArray27},
    /* 28 */   {3, 0, DFAEdgeArray28},
    /* 29 */   {6, 2, DFAEdgeArray29},
    /* 30 */   {23, 2, DFAEdgeArray30},
    /* 31 */   {-1, 2, DFAEdgeArray31},
    /* 32 */   {4, 0, DFAEdgeArray32},
    /* 33 */   {5, 0, DFAEdgeArray33},
    /* 34 */   {10, 1, DFAEdgeArray34},
    /* 35 */   {11, 0, DFAEdgeArray35},
    /* 36 */   {19, 1, DFAEdgeArray36},
    /* 37 */   {20, 0, DFAEdgeArray37},
    /* 38 */   {13, 1, DFAEdgeArray38},
    /* 39 */   {-1, 2, DFAEdgeArray39},
    /* 40 */   {22, 0, DFAEdgeArray40},
    /* 41 */   {26, 2, DFAEdgeArray41},
    /* 42 */   {26, 2, DFAEdgeArray42},
    /* 43 */   {26, 2, DFAEdgeArray43},
    /* 44 */   {26, 2, DFAEdgeArray44},
    /* 45 */   {21, 1, DFAEdgeArray45},
    /* 46 */   {26, 2, DFAEdgeArray46},
    /* 47 */   {26, 2, DFAEdgeArray47},
    /* 48 */   {26, 2, DFAEdgeArray48},
    /* 49 */   {24, 1, DFAEdgeArray49},
    /* 50 */   {26, 2, DFAEdgeArray50},
    /* 51 */   {26, 2, DFAEdgeArray51},
    /* 52 */   {26, 2, DFAEdgeArray52},
    /* 53 */   {26, 2, DFAEdgeArray53},
    /* 54 */   {26, 2, DFAEdgeArray54},
    /* 55 */   {26, 2, DFAEdgeArray55},
    /* 56 */   {26, 2, DFAEdgeArray56},
    /* 57 */   {26, 2, DFAEdgeArray57},
    /* 58 */   {30, 1, DFAEdgeArray58},
    /* 59 */   {26, 3, DFAEdgeArray59},
    /* 60 */   {26, 2, DFAEdgeArray60},
    /* 61 */   {26, 2, DFAEdgeArray61},
    /* 62 */   {26, 2, DFAEdgeArray62},
    /* 63 */   {26, 2, DFAEdgeArray63},
    /* 64 */   {26, 2, DFAEdgeArray64},
    /* 65 */   {26, 2, DFAEdgeArray65},
    /* 66 */   {31, 0, DFAEdgeArray66},
    /* 67 */   {26, 2, DFAEdgeArray67},
    /* 68 */   {26, 2, DFAEdgeArray68},
    /* 69 */   {26, 2, DFAEdgeArray69},
    /* 70 */   {26, 2, DFAEdgeArray70},
    /* 71 */   {26, 2, DFAEdgeArray71},
    /* 72 */   {32, 0, DFAEdgeArray72},
    /* 73 */   {26, 2, DFAEdgeArray73},
    /* 74 */   {26, 2, DFAEdgeArray74},
    /* 75 */   {26, 2, DFAEdgeArray75},
    /* 76 */   {26, 2, DFAEdgeArray76},
    /* 77 */   {26, 2, DFAEdgeArray77},
    /* 78 */   {34, 1, DFAEdgeArray78},
    /* 79 */   {26, 2, DFAEdgeArray79},
    /* 80 */   {26, 2, DFAEdgeArray80},
    /* 81 */   {26, 2, DFAEdgeArray81},
    /* 82 */   {26, 2, DFAEdgeArray82},
    /* 83 */   {26, 2, DFAEdgeArray83},
    /* 84 */   {26, 2, DFAEdgeArray84},
    /* 85 */   {36, 1, DFAEdgeArray85},
    /* 86 */   {15, 1, DFAEdgeArray86},
    /* 87 */   {-1, 2, DFAEdgeArray87},
    /* 88 */   {35, 0, DFAEdgeArray88},
    /* 89 */   {16, 1, DFAEdgeArray89},
    /* 90 */   {17, 0, DFAEdgeArray90},
    {-1, 0, NULL}
  };



  // --------------------------------------------------------------------
  // Rule Table
  //                        head, symbol count, symbol array, description
  // --------------------------------------------------------------------

  const Rule RuleArray[69] = 
  {
    /*  0 */   {38, 1, RuleSymbolArray0, "<CustomOptionKeyword> ::= ModKeyword"},
    /*  1 */   {38, 4, RuleSymbolArray1, "<CustomOptionKeyword> ::= ModKeyword ( IdentifierTerm )"},
    /*  2 */   {38, 4, RuleSymbolArray2, "<CustomOptionKeyword> ::= ModKeyword ( StringLiteral )"},
    /*  3 */   {38, 4, RuleSymbolArray3, "<CustomOptionKeyword> ::= ModKeyword ( DecLiteral )"},
    /*  4 */   {61, 2, RuleSymbolArray4, "<VarIdentifier> ::= IdStar IdentifierTerm"},
    /*  5 */   {61, 2, RuleSymbolArray5, "<VarIdentifier> ::= IdRef IdentifierTerm"},
    /*  6 */   {61, 1, RuleSymbolArray6, "<VarIdentifier> ::= IdentifierTerm"},
    /*  7 */   {59, 3, RuleSymbolArray7, "<VarCascade> ::= IdentifierTerm :: <VarCascade>"},
    /*  8 */   {59, 1, RuleSymbolArray8, "<VarCascade> ::= IdentifierTerm"},
    /*  9 */   {64, 1, RuleSymbolArray9, "<VarType> ::= <VarCascade>"},
    /* 10 */   {64, 1, RuleSymbolArray10, "<VarType> ::= <CustomOptionKeyword>"},
    /* 11 */   {65, 1, RuleSymbolArray11, "<VarTypeList> ::= <VarType>"},
    /* 12 */   {65, 2, RuleSymbolArray12, "<VarTypeList> ::= <VarTypeList> <VarType>"},
    /* 13 */   {62, 1, RuleSymbolArray13, "<VarSpec> ::= <VarIdentifier>"},
    /* 14 */   {62, 2, RuleSymbolArray14, "<VarSpec> ::= <VarIdentifier> ArraySpecifier"},
    /* 15 */   {62, 4, RuleSymbolArray15, "<VarSpec> ::= <VarIdentifier> [ IdentifierTerm ]"},
    /* 16 */   {63, 1, RuleSymbolArray16, "<VarSpecs> ::= <VarSpec>"},
    /* 17 */   {63, 3, RuleSymbolArray17, "<VarSpecs> ::= <VarSpecs> , <VarSpec>"},
    /* 18 */   {44, 1, RuleSymbolArray18, "<EnumExprValue> ::= DecLiteral"},
    /* 19 */   {44, 1, RuleSymbolArray19, "<EnumExprValue> ::= IdentifierTerm"},
    /* 20 */   {43, 3, RuleSymbolArray20, "<EnumExpr> ::= <EnumExpr> + <EnumExprValue>"},
    /* 21 */   {43, 3, RuleSymbolArray21, "<EnumExpr> ::= <EnumExpr> - <EnumExprValue>"},
    /* 22 */   {43, 3, RuleSymbolArray22, "<EnumExpr> ::= <EnumExpr> IdStar <EnumExprValue>"},
    /* 23 */   {43, 1, RuleSymbolArray23, "<EnumExpr> ::= <EnumExprValue>"},
    /* 24 */   {42, 1, RuleSymbolArray24, "<Enum> ::= IdentifierTerm"},
    /* 25 */   {42, 3, RuleSymbolArray25, "<Enum> ::= IdentifierTerm = <EnumExpr>"},
    /* 26 */   {45, 1, RuleSymbolArray26, "<Enums> ::= <Enum>"},
    /* 27 */   {45, 3, RuleSymbolArray27, "<Enums> ::= <Enums> , <Enum>"},
    /* 28 */   {60, 3, RuleSymbolArray28, "<VarDecl> ::= <VarTypeList> <VarSpecs> ;"},
    /* 29 */   {60, 5, RuleSymbolArray29, "<VarDecl> ::= enum IdentifierTerm { <Enums> };"},
    /* 30 */   {49, 1, RuleSymbolArray30, "<FnIsConst> ::= IdentifierTerm"},
    /* 31 */   {49, 0, RuleSymbolArray31, "<FnIsConst> ::= "},
    /* 32 */   {54, 1, RuleSymbolArray32, "<FnNumLiteral> ::= FloatLiteral"},
    /* 33 */   {54, 1, RuleSymbolArray33, "<FnNumLiteral> ::= DecLiteral"},
    /* 34 */   {50, 1, RuleSymbolArray34, "<FnLiteral> ::= <FnNumLiteral>"},
    /* 35 */   {50, 1, RuleSymbolArray35, "<FnLiteral> ::= IdentifierTerm"},
    /* 36 */   {51, 1, RuleSymbolArray36, "<FnLiterals> ::= <FnLiteral>"},
    /* 37 */   {51, 3, RuleSymbolArray37, "<FnLiterals> ::= <FnLiterals> , <FnLiteral>"},
    /* 38 */   {46, 2, RuleSymbolArray38, "<FnArg> ::= <VarTypeList> <VarIdentifier>"},
    /* 39 */   {46, 7, RuleSymbolArray39, "<FnArg> ::= <VarTypeList> <VarIdentifier> = <VarCascade> ( <FnLiterals> )"},
    /* 40 */   {46, 4, RuleSymbolArray40, "<FnArg> ::= <VarTypeList> <VarIdentifier> = <FnNumLiteral>"},
    /* 41 */   {47, 1, RuleSymbolArray41, "<FnArgs> ::= <FnArg>"},
    /* 42 */   {47, 3, RuleSymbolArray42, "<FnArgs> ::= <FnArgs> , <FnArg>"},
    /* 43 */   {47, 0, RuleSymbolArray43, "<FnArgs> ::= "},
    /* 44 */   {52, 2, RuleSymbolArray44, "<FnName> ::= IdStar IdentifierTerm"},
    /* 45 */   {52, 2, RuleSymbolArray45, "<FnName> ::= IdRef IdentifierTerm"},
    /* 46 */   {52, 1, RuleSymbolArray46, "<FnName> ::= IdentifierTerm"},
    /* 47 */   {53, 2, RuleSymbolArray47, "<FnNameAndReturn> ::= <VarTypeList> <FnName>"},
    /* 48 */   {53, 1, RuleSymbolArray48, "<FnNameAndReturn> ::= <FnName>"},
    /* 49 */   {48, 7, RuleSymbolArray49, "<FnDecl> ::= + <FnNameAndReturn> ( <FnArgs> ) <FnIsConst> ;"},
    /* 50 */   {40, 1, RuleSymbolArray50, "<DataBlockDef> ::= <VarDecl>"},
    /* 51 */   {40, 1, RuleSymbolArray51, "<DataBlockDef> ::= <FnDecl>"},
    /* 52 */   {40, 1, RuleSymbolArray52, "<DataBlockDef> ::= private:"},
    /* 53 */   {40, 1, RuleSymbolArray53, "<DataBlockDef> ::= public:"},
    /* 54 */   {41, 1, RuleSymbolArray54, "<DataBlockDefs> ::= <DataBlockDef>"},
    /* 55 */   {41, 2, RuleSymbolArray55, "<DataBlockDefs> ::= <DataBlockDefs> <DataBlockDef>"},
    /* 56 */   {39, 1, RuleSymbolArray56, "<CustomOptions> ::= <CustomOptionKeyword>"},
    /* 57 */   {39, 2, RuleSymbolArray57, "<CustomOptions> ::= <CustomOptions> <CustomOptionKeyword>"},
    /* 58 */   {37, 1, RuleSymbolArray58, "<CustomOption> ::= <CustomOptions>"},
    /* 59 */   {37, 0, RuleSymbolArray59, "<CustomOption> ::= "},
    /* 60 */   {57, 2, RuleSymbolArray60, "<StructInheritance> ::= : <VarCascade>"},
    /* 61 */   {57, 0, RuleSymbolArray61, "<StructInheritance> ::= "},
    /* 62 */   {55, 5, RuleSymbolArray62, "<Root> ::= namespace IdentifierTerm { <Roots> }"},
    /* 63 */   {55, 7, RuleSymbolArray63, "<Root> ::= <CustomOption> struct IdentifierTerm <StructInheritance> { <DataBlockDefs> };"},
    /* 64 */   {55, 7, RuleSymbolArray64, "<Root> ::= <CustomOption> alias <VarCascade> => IdentifierTerm <CustomOption> ;"},
    /* 65 */   {55, 6, RuleSymbolArray65, "<Root> ::= <CustomOption> typedef <VarCascade> TSizeSpecifier <CustomOption> ;"},
    /* 66 */   {56, 1, RuleSymbolArray66, "<Roots> ::= <Root>"},
    /* 67 */   {56, 2, RuleSymbolArray67, "<Roots> ::= <Roots> <Root>"},
    /* 68 */   {58, 1, RuleSymbolArray68, "<TypesRoots> ::= <Roots>"} 
  };


  // --------------------------------------------------------------------
  // LALR State Table
  //                                           action count, action array
  // --------------------------------------------------------------------

  const LALRState LALRStateArray[125+1] = 
  {
    /* 0 */   {11, LALRActionArray0},
    /* 1 */   {9, LALRActionArray1},
    /* 2 */   {1, LALRActionArray2},
    /* 3 */   {3, LALRActionArray3},
    /* 4 */   {5, LALRActionArray4},
    /* 5 */   {6, LALRActionArray5},
    /* 6 */   {7, LALRActionArray6},
    /* 7 */   {10, LALRActionArray7},
    /* 8 */   {1, LALRActionArray8},
    /* 9 */   {3, LALRActionArray9},
    /* 10 */   {1, LALRActionArray10},
    /* 11 */   {2, LALRActionArray11},
    /* 12 */   {1, LALRActionArray12},
    /* 13 */   {2, LALRActionArray13},
    /* 14 */   {5, LALRActionArray14},
    /* 15 */   {7, LALRActionArray15},
    /* 16 */   {1, LALRActionArray16},
    /* 17 */   {1, LALRActionArray17},
    /* 18 */   {1, LALRActionArray18},
    /* 19 */   {10, LALRActionArray19},
    /* 20 */   {9, LALRActionArray20},
    /* 21 */   {1, LALRActionArray21},
    /* 22 */   {3, LALRActionArray22},
    /* 23 */   {1, LALRActionArray23},
    /* 24 */   {8, LALRActionArray24},
    /* 25 */   {8, LALRActionArray25},
    /* 26 */   {8, LALRActionArray26},
    /* 27 */   {10, LALRActionArray27},
    /* 28 */   {2, LALRActionArray28},
    /* 29 */   {1, LALRActionArray29},
    /* 30 */   {2, LALRActionArray30},
    /* 31 */   {1, LALRActionArray31},
    /* 32 */   {5, LALRActionArray32},
    /* 33 */   {7, LALRActionArray33},
    /* 34 */   {8, LALRActionArray34},
    /* 35 */   {5, LALRActionArray35},
    /* 36 */   {1, LALRActionArray36},
    /* 37 */   {14, LALRActionArray37},
    /* 38 */   {1, LALRActionArray38},
    /* 39 */   {1, LALRActionArray39},
    /* 40 */   {10, LALRActionArray40},
    /* 41 */   {1, LALRActionArray41},
    /* 42 */   {7, LALRActionArray42},
    /* 43 */   {7, LALRActionArray43},
    /* 44 */   {4, LALRActionArray44},
    /* 45 */   {7, LALRActionArray45},
    /* 46 */   {14, LALRActionArray46},
    /* 47 */   {7, LALRActionArray47},
    /* 48 */   {4, LALRActionArray48},
    /* 49 */   {7, LALRActionArray49},
    /* 50 */   {4, LALRActionArray50},
    /* 51 */   {10, LALRActionArray51},
    /* 52 */   {7, LALRActionArray52},
    /* 53 */   {7, LALRActionArray53},
    /* 54 */   {6, LALRActionArray54},
    /* 55 */   {1, LALRActionArray55},
    /* 56 */   {1, LALRActionArray56},
    /* 57 */   {1, LALRActionArray57},
    /* 58 */   {1, LALRActionArray58},
    /* 59 */   {8, LALRActionArray59},
    /* 60 */   {1, LALRActionArray60},
    /* 61 */   {7, LALRActionArray61},
    /* 62 */   {7, LALRActionArray62},
    /* 63 */   {11, LALRActionArray63},
    /* 64 */   {1, LALRActionArray64},
    /* 65 */   {1, LALRActionArray65},
    /* 66 */   {4, LALRActionArray66},
    /* 67 */   {2, LALRActionArray67},
    /* 68 */   {2, LALRActionArray68},
    /* 69 */   {4, LALRActionArray69},
    /* 70 */   {1, LALRActionArray70},
    /* 71 */   {1, LALRActionArray71},
    /* 72 */   {10, LALRActionArray72},
    /* 73 */   {1, LALRActionArray73},
    /* 74 */   {3, LALRActionArray74},
    /* 75 */   {6, LALRActionArray75},
    /* 76 */   {6, LALRActionArray76},
    /* 77 */   {1, LALRActionArray77},
    /* 78 */   {2, LALRActionArray78},
    /* 79 */   {5, LALRActionArray79},
    /* 80 */   {7, LALRActionArray80},
    /* 81 */   {2, LALRActionArray81},
    /* 82 */   {2, LALRActionArray82},
    /* 83 */   {8, LALRActionArray83},
    /* 84 */   {3, LALRActionArray84},
    /* 85 */   {2, LALRActionArray85},
    /* 86 */   {2, LALRActionArray86},
    /* 87 */   {1, LALRActionArray87},
    /* 88 */   {4, LALRActionArray88},
    /* 89 */   {2, LALRActionArray89},
    /* 90 */   {3, LALRActionArray90},
    /* 91 */   {7, LALRActionArray91},
    /* 92 */   {3, LALRActionArray92},
    /* 93 */   {4, LALRActionArray93},
    /* 94 */   {2, LALRActionArray94},
    /* 95 */   {7, LALRActionArray95},
    /* 96 */   {2, LALRActionArray96},
    /* 97 */   {1, LALRActionArray97},
    /* 98 */   {1, LALRActionArray98},
    /* 99 */   {2, LALRActionArray99},
    /* 100 */   {5, LALRActionArray100},
    /* 101 */   {5, LALRActionArray101},
    /* 102 */   {5, LALRActionArray102},
    /* 103 */   {5, LALRActionArray103},
    /* 104 */   {5, LALRActionArray104},
    /* 105 */   {2, LALRActionArray105},
    /* 106 */   {7, LALRActionArray106},
    /* 107 */   {2, LALRActionArray107},
    /* 108 */   {2, LALRActionArray108},
    /* 109 */   {2, LALRActionArray109},
    /* 110 */   {1, LALRActionArray110},
    /* 111 */   {3, LALRActionArray111},
    /* 112 */   {3, LALRActionArray112},
    /* 113 */   {3, LALRActionArray113},
    /* 114 */   {6, LALRActionArray114},
    /* 115 */   {5, LALRActionArray115},
    /* 116 */   {5, LALRActionArray116},
    /* 117 */   {5, LALRActionArray117},
    /* 118 */   {2, LALRActionArray118},
    /* 119 */   {2, LALRActionArray119},
    /* 120 */   {2, LALRActionArray120},
    /* 121 */   {2, LALRActionArray121},
    /* 122 */   {2, LALRActionArray122},
    /* 123 */   {5, LALRActionArray123},
    /* 124 */   {2, LALRActionArray124},
    {0, NULL}
  };


  // --------------------------------------------------------------------
  // 
  // --------------------------------------------------------------------

  const Grammar Parser::sGrammar = 
  {
    True,
    58,    // start symbol
    0,     // initial DFA
    0,     // initial LALR

    66, SymbolArray,
    69, RuleArray,
    91, DFAStateArray,
    125, LALRStateArray
  };

  const Parser::RuleHandler Parser::gRuleJumpTable[69] = 
  {
    /*  0 */ &Parser::Rule_CustomOptionKeyword_ModKeyword,
    /*  1 */ &Parser::Rule_CustomOptionKeyword_ModKeyword_LParan_IdentifierTerm_RParan,
    /*  2 */ &Parser::Rule_CustomOptionKeyword_ModKeyword_LParan_StringLiteral_RParan,
    /*  3 */ &Parser::Rule_CustomOptionKeyword_ModKeyword_LParan_DecLiteral_RParan,
    /*  4 */ &Parser::Rule_VarIdentifier_IdStar_IdentifierTerm,
    /*  5 */ &Parser::Rule_VarIdentifier_IdRef_IdentifierTerm,
    /*  6 */ &Parser::Rule_VarIdentifier_IdentifierTerm,
    /*  7 */ &Parser::Rule_VarCascade_IdentifierTerm_ColonColon,
    /*  8 */ &Parser::Rule_VarCascade_IdentifierTerm,
    /*  9 */ &Parser::Rule_VarType,
    /* 10 */ &Parser::Rule_VarType2,
    /* 11 */ &Parser::Rule_VarTypeList,
    /* 12 */ &Parser::Rule_VarTypeList2,
    /* 13 */ &Parser::Rule_VarSpec,
    /* 14 */ &Parser::Rule_VarSpec_ArraySpecifier,
    /* 15 */ &Parser::Rule_VarSpec_LBracket_IdentifierTerm_RBracket,
    /* 16 */ &Parser::Rule_VarSpecs,
    /* 17 */ &Parser::Rule_VarSpecs_Comma,
    /* 18 */ &Parser::Rule_EnumExprValue_DecLiteral,
    /* 19 */ &Parser::Rule_EnumExprValue_IdentifierTerm,
    /* 20 */ &Parser::Rule_EnumExpr_Plus,
    /* 21 */ &Parser::Rule_EnumExpr_Minus,
    /* 22 */ &Parser::Rule_EnumExpr_IdStar,
    /* 23 */ &Parser::Rule_EnumExpr,
    /* 24 */ &Parser::Rule_Enum_IdentifierTerm,
    /* 25 */ &Parser::Rule_Enum_IdentifierTerm_Eq,
    /* 26 */ &Parser::Rule_Enums,
    /* 27 */ &Parser::Rule_Enums_Comma,
    /* 28 */ &Parser::Rule_VarDecl_Semi,
    /* 29 */ &Parser::Rule_VarDecl_enum_IdentifierTerm_LBrace_RBraceSemi,
    /* 30 */ &Parser::Rule_FnIsConst_IdentifierTerm,
    /* 31 */ &Parser::Rule_FnIsConst,
    /* 32 */ &Parser::Rule_FnNumLiteral_FloatLiteral,
    /* 33 */ &Parser::Rule_FnNumLiteral_DecLiteral,
    /* 34 */ &Parser::Rule_FnLiteral,
    /* 35 */ &Parser::Rule_FnLiteral_IdentifierTerm,
    /* 36 */ &Parser::Rule_FnLiterals,
    /* 37 */ &Parser::Rule_FnLiterals_Comma,
    /* 38 */ &Parser::Rule_FnArg,
    /* 39 */ &Parser::Rule_FnArg_Eq_LParan_RParan,
    /* 40 */ &Parser::Rule_FnArg_Eq,
    /* 41 */ &Parser::Rule_FnArgs,
    /* 42 */ &Parser::Rule_FnArgs_Comma,
    /* 43 */ &Parser::Rule_FnArgs2,
    /* 44 */ &Parser::Rule_FnName_IdStar_IdentifierTerm,
    /* 45 */ &Parser::Rule_FnName_IdRef_IdentifierTerm,
    /* 46 */ &Parser::Rule_FnName_IdentifierTerm,
    /* 47 */ &Parser::Rule_FnNameAndReturn,
    /* 48 */ &Parser::Rule_FnNameAndReturn2,
    /* 49 */ &Parser::Rule_FnDecl_Plus_LParan_RParan_Semi,
    /* 50 */ &Parser::Rule_DataBlockDef,
    /* 51 */ &Parser::Rule_DataBlockDef2,
    /* 52 */ &Parser::Rule_DataBlockDef_privateColon,
    /* 53 */ &Parser::Rule_DataBlockDef_publicColon,
    /* 54 */ &Parser::Rule_DataBlockDefs,
    /* 55 */ &Parser::Rule_DataBlockDefs2,
    /* 56 */ &Parser::Rule_CustomOptions,
    /* 57 */ &Parser::Rule_CustomOptions2,
    /* 58 */ &Parser::Rule_CustomOption,
    /* 59 */ &Parser::Rule_CustomOption2,
    /* 60 */ &Parser::Rule_StructInheritance_Colon,
    /* 61 */ &Parser::Rule_StructInheritance,
    /* 62 */ &Parser::Rule_Root_namespace_IdentifierTerm_LBrace_RBrace,
    /* 63 */ &Parser::Rule_Root_struct_IdentifierTerm_LBrace_RBraceSemi,
    /* 64 */ &Parser::Rule_Root_alias_EqGt_IdentifierTerm_Semi,
    /* 65 */ &Parser::Rule_Root_typedef_TSizeSpecifier_Semi,
    /* 66 */ &Parser::Rule_Roots,
    /* 67 */ &Parser::Rule_Roots2,
    /* 68 */ &Parser::Rule_TypesRoots 
  };

  /**
   * used to cram together the tokens that make up a function declaration
   */
  void Parser::recursiveFunctionDeclarationConcat(const Grammar& grammar, Token *token, unsigned int startToken) 
  {
    unsigned int i, symbolCount;
    int symbolID;

    symbolCount = grammar.m_ruleArray[token->m_reductionRule].m_symbolsCount;

    for (i=startToken; i<symbolCount; i++) 
    {
      Token *tk = token->m_tokens[i];
      symbolID = tk->m_symbol;

      if (tk->m_reductionRule < 0) 
      {
        // add spacing based on the last token appended to the buffer 
        if ((symbolID == Symbol_IdentifierTerm) &&                        // IdentifierTerm
          (
          (m_trackingLastSeenSymbol == Symbol_IdentifierTerm) ||    // IdentifierTerm <- IdentifierTerm
          (m_trackingLastSeenSymbol == Symbol_IdRef)          ||    // & <- IdentifierTerm
          (m_trackingLastSeenSymbol == Symbol_IdStar)         ||    // * <- IdentifierTerm
          (m_trackingLastSeenSymbol == Symbol_Comma)                // , <- IdentifierTerm
          )
          )
        {
          m_stringBuilder->appendChar(' ');
        }
        if (symbolID == Symbol_IdentifierTerm &&                 // eg 'const'
            m_trackingLastSeenSymbol == Symbol_RParan)                   // closing brace ')'
        {
          // we only know about 'const' being able to follow a fnDecl
          if (strcmp(tk->m_data, "const"))
          {
            throw(new RuleExecutionException(token, tk->m_data, "unknown identifier following function declaration (expecting 'const')"));
          }

          m_stringBuilder->appendChar(' ');
        }

        m_stringBuilder->appendCharBuf(tk->m_data);
        m_trackingLastSeenSymbol = symbolID;
      } 
      else 
      {
        // rule, call to expand it's contents
        recursiveFunctionDeclarationConcat(grammar, tk, 0);
      }
    }
  }



} // namespace MDFTypes


