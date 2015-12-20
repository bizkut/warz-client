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
#include "MDFKeywords.h"
#include "PString.h"
#include "StringBuilder.h"

#include "nmtl/unique_vector.h"

//----------------------------------------------------------------------------------------------------------------------
struct KeywordsManagerData
{
  KeywordsManagerData() :
    m_reservedKeywords(64, ActiveMemoryManager::getForNMTL())
  {
    // what the blocks are called in the module def file, eg. in connection paths
    SectionNames[msData]                  = "data";
    SectionNames[msFeedInputs]            = "feedIn";
    SectionNames[msInputs]                = "in";
    SectionNames[msUserInputs]            = "userIn";
    SectionNames[msFeedOutputs]           = "feedOut";
    SectionNames[msOutputs]               = "out";
    SectionNames[msOwner]                 = "owner";
    SectionNames[msChildren]              = "children";
    SectionNames[msPrivateAPI]            = "privateAPI";
    SectionNames[msPublicAPI]             = "publicAPI";
    SectionNames[msConnections]           = "connection";

    // block names for function names, etc
    SectionLabels[msData]                 = "Data";
    SectionLabels[msFeedInputs]           = "FeedbackInputs";
    SectionLabels[msInputs]               = "Inputs";
    SectionLabels[msUserInputs]           = "UserInputs";
    SectionLabels[msFeedOutputs]          = "FeedbackOutputs";
    SectionLabels[msOutputs]              = "Outputs";
    SectionLabels[msOwner]                = "Owner";
    SectionLabels[msChildren]             = "Children";
    SectionLabels[msPrivateAPI]           = "PrivateAPI";
    SectionLabels[msPublicAPI]            = "PublicAPI";
    SectionLabels[msConnections]          = "Connections";

    ConnectionTaskNames[ctCreateJunction] = "Create Junction";
    ConnectionTaskNames[ctAssignJunction] = "Assign Junction";

    ConnectionContextNames[ccOwner]       = "owner";
    ConnectionContextNames[ccJunctions]   = "junctions";
    ConnectionContextNames[ccFeedInputs]  = "feedIn";
    ConnectionContextNames[ccInputs]      = "in";
    ConnectionContextNames[ccUserInputs]  = "userIn";
    ConnectionContextNames[ccFeedOutputs] = "feedOut";
    ConnectionContextNames[ccOutputs]     = "out";

    JunctionModeNames[jmDirectInput]      = "directInput";
    JunctionModeNames[jmAverage]          = "average";
    JunctionModeNames[jmWinnerTakesAll]   = "winnerTakesAll";
    JunctionModeNames[jmPriority]         = "priority";
    JunctionModeNames[jmSum]              = "sum";
    JunctionModeNames[jmMaxFloat]         = "maxFloat";
    JunctionModeNames[jmMaxFloatValue]    = "maxFloatValue";
    JunctionModeNames[jmIsBoolSet]        = "isBoolSet";

    BehaviourDefVariableTypes[bhvtFloat]  = "float";
    BehaviourDefVariableTypes[bhvtInteger]= "int";
    BehaviourDefVariableTypes[bhvtBool]   = "bool";
    BehaviourDefVariableTypes[bhvtVector3]= "vector3";
    BehaviourDefVariableTypes[bhvtPhysicsObjectID]= "physicsObjectID";
    BehaviourDefVariableTypes[bhvtMessage]= "message";


    // build lookup of all reserved strings that are not allowed as
    // variable names, module names, etc during parsing
    static const char* gReservedKeywords[] = 
    {
      "alias",
      "asm",
      "attributes",
      "animationInputs",
      "auto",
      "break",
      "behaviour_def",
      "behaviour",
      "controlParams",
      "case",
      "catch",
      "children",
      "connection",
      "class",
      "const",
      "const_cast",
      "continue",
      "data",
      "default",
      "def",
      "delete",
      "do",
      "dynamic_cast", 
      "else",
      "enum",
      "explicit",
      "export",
      "extern",
      "false",
      "feedIn",
      "feedOut",
      "for",
      "friend",
      "goto",
      "if",
      "inline",
      "in",
      "module_def",
      "modules",
      "mutable",
      "namespace",
      "new",
      "global_access",
      "out",
      "operator",
      "owner",
      "private",
      "protected",
      "public",
      "privateAPI",
      "publicAPI",
      "register",
      "reinterpret_cast",
      "return",
      "sizeof",
      "static",
      "static_cast",
      "struct",
      "switch",
      "template",
      "this",
      "typedef",
      "typeid",
      "throw",
      "true",
      "try",
      "typename",
      "union",
      "unsigned",
      "using",
      "virtual",
      "volatile",
      "while",
      "vector3",
      "physicsObjectID",
    };
    static const unsigned int numKeywords = sizeof(gReservedKeywords) / sizeof(gReservedKeywords[0]); 

    for (unsigned int i=0; i<numKeywords; i++)
    {
      bool didAdd = false;
      didAdd = m_reservedKeywords.add(gReservedKeywords[i]);
      assert(didAdd);
    }

    ModuleModifierKeywords[mmGenerateInterface]            = Keyword("__generate_interface__",Keyword::vrNone,        "");
    ModuleModifierKeywords[mmNoUpdate]                     = Keyword("__no_update__",         Keyword::vrNone,        "");
    ModuleModifierKeywords[mmNoFeedback]                   = Keyword("__no_feedback__",       Keyword::vrNone,        "");
    ModuleModifierKeywords[mmGlobalAccess]                 = Keyword("__global_access__",     Keyword::vrNone,        "");
    ModuleModifierKeywords[mmLimb]                         = Keyword("__limb__",              Keyword::vrIdentifier,  "pluralization of module name");
    ModuleModifierKeywords[mmPIX]                          = Keyword("__pix__",               Keyword::vrNone,        "");

    TypeModifierKeywords[tmAlign]                          = Keyword("__align__",             Keyword::vrDecimal,     "non-zero alignment value");
    TypeModifierKeywords[tmValidator]                      = Keyword("__validator__",         Keyword::vrIdentifier,  "function name for validation");
    TypeModifierKeywords[tmPreProcessor]                   = Keyword("__preprocessor__",      Keyword::vrIdentifier,  "preprocessor macro that must be defined");
    TypeModifierKeywords[tmPostCombine]                    = Keyword("__post_combine__",      Keyword::vrIdentifier,  "function name for post-combine");
    TypeModifierKeywords[tmExternalClass]                  = Keyword("__extern_class__",      Keyword::vrString,      "path to header containing type definition");
    TypeModifierKeywords[tmExternalStruct]                 = Keyword("__extern_struct__",     Keyword::vrString,      "path to header containing type definition");
    TypeModifierKeywords[tmImportAsBuiltIn]                = Keyword("__import_builtin__",    Keyword::vrString,      "path to header containing type definition");
    TypeModifierKeywords[tmBlendable]                      = Keyword("__blendable__",         Keyword::vrNone,        "");

    VarDeclModifierKeywords[vdmValidator]                  = Keyword("__validator__",         Keyword::vrIdentifier,  "function name for validation");

    StructModifierKeywords[smNoCombine]                    = Keyword("__no_combiner__",       Keyword::vrNone,        "");
    StructModifierKeywords[smValidator]                    = Keyword("__validator__",         Keyword::vrIdentifier,  "function name for validation");

    BehaviourVariableModifierKeywords[bvmPrivate]          = Keyword("__private__",           Keyword::vrNone,        "");
    BehaviourVariableModifierKeywords[bvmEnumDefaultIndex] = Keyword("__enum_default__",      Keyword::vrIdentifier,  "need index name for default enum");
    BehaviourVariableModifierKeywords[bvmEnumNoInstance]   = Keyword("__enum_no_instance__",  Keyword::vrNone,        "");
    BehaviourVariableModifierKeywords[bvmPerAnimSet]       = Keyword("__per_anim_set__",      Keyword::vrNone,        "");
    BehaviourVariableModifierKeywords[bvmCompulsoryInput]  = Keyword("__compulsory_input__",  Keyword::vrNone,        "");
    BehaviourVariableModifierKeywords[bvmTitle]            = Keyword("__title__",             Keyword::vrString,      "");
    BehaviourVariableModifierKeywords[bvmLimbIndex]        = Keyword("__limb_index__",        Keyword::vrNone,      "");
    BehaviourVariableModifierKeywords[bvmLimbPartIndex]    = Keyword("__limb_part_index__",   Keyword::vrNone,      "");
    BehaviourVariableModifierKeywords[bvmPhysicsPartIndex] = Keyword("__physics_part_index__",Keyword::vrNone,      "");
    BehaviourVariableModifierKeywords[bvmExposedAsPin]     = Keyword("__expose_as_pin__",     Keyword::vrNone,      "");

    ConnectionSetModifierKeywords[csmNoSetInheritance]     = Keyword("__no_inheritance__",    Keyword::vrNone,        "");

    ConnectionTaskModifierKeywords[ctmOverride]            = Keyword("__override__",          Keyword::vrNone,        "");
  }

  typedef nmtl::unique_vector<const char*>  ReservedKeywords;

  PString SectionNames[msNumSections];
  PString SectionLabels[msNumSections];
  PString ConnectionTaskNames[ctNumTasks];
  PString ConnectionContextNames[ccNumComponents];
  PString JunctionModeNames[jmNumJunctionModes];

  PString BehaviourDefVariableTypes[bhvtNumVariableTypes];

  ReservedKeywords m_reservedKeywords;

  Keyword ModuleModifierKeywords[numModuleModifierKeywords];
  Keyword TypeModifierKeywords[numTypeModifierKeywords];
  Keyword VarDeclModifierKeywords[numVarDeclModifierKeywords];
  Keyword StructModifierKeywords[numStructModifierKeywords];
  Keyword BehaviourVariableModifierKeywords[numBehaviourVariableKeywords];
  Keyword ConnectionSetModifierKeywords[numConnectionSetKeywords];
  Keyword ConnectionTaskModifierKeywords[numConnectionTaskKeywords];


  PString m_statString;
};


//----------------------------------------------------------------------------------------------------------------------
KeywordsManager::KeywordsManager()
{
  parserMemoryAllocDirect(KeywordsManagerData, m_data);
}

//----------------------------------------------------------------------------------------------------------------------
ModuleSection KeywordsManager::toModuleSection(const PString& str) const
{
  for (unsigned int i=0; i<msNumSections; i++)
  {
    if (str == m_data->SectionNames[i])
      return (ModuleSection)i;
  }

  return msInvalid;
}

//----------------------------------------------------------------------------------------------------------------------
const PString& KeywordsManager::getModuleSectionName(ModuleSection ms) const
{
  assert((int)ms >= 0 && (int)ms < msNumSections);
  return m_data->SectionNames[(int)ms];
}

//----------------------------------------------------------------------------------------------------------------------
const PString& KeywordsManager::getModuleSectionLabel(ModuleSection ms) const
{
  assert((int)ms >= 0 && (int)ms < msNumSections);
  return m_data->SectionLabels[(int)ms];
}


//----------------------------------------------------------------------------------------------------------------------
ConnectionTask KeywordsManager::toConnectionTask(const PString& str) const
{
  for (unsigned int i=0; i<ctNumTasks; i++)
  {
    if (str == m_data->ConnectionTaskNames[i])
      return (ConnectionTask)i;
  }

  return ctInvalid;
}

//----------------------------------------------------------------------------------------------------------------------
const PString& KeywordsManager::getConnectionTaskLabel(ConnectionTask ct) const
{
  assert((int)ct >= 0 && (int)ct < ctNumTasks);
  return m_data->ConnectionTaskNames[(int)ct];
}

//----------------------------------------------------------------------------------------------------------------------
ComponentContext KeywordsManager::toConnectionContext(const PString& str) const
{
  for (unsigned int i=0; i<ccNumComponents; i++)
  {
    if (str == m_data->ConnectionContextNames[i])
      return (ComponentContext)i;
  }

  return ccUnknown;
}


//----------------------------------------------------------------------------------------------------------------------
JunctionMode KeywordsManager::toJunctionMode(const PString& str) const
{
  for (unsigned int i=0; i<jmNumJunctionModes; i++)
  {
    if (str == m_data->JunctionModeNames[i])
      return (JunctionMode)i;
  }

  return jmInvalid;
}

//----------------------------------------------------------------------------------------------------------------------
const PString& KeywordsManager::getJunctionModeLabel(JunctionMode jm) const
{
  assert((int)jm >= 0 && (int)jm < jmNumJunctionModes);
  return m_data->JunctionModeNames[(int)jm];
}

//----------------------------------------------------------------------------------------------------------------------
BhDefVariableType KeywordsManager::behaviourDefVariableTypeFromString(const PString& str) const
{
  for (unsigned int i=0; i<bhvtNumVariableTypes; i++)
  {
    if (str == m_data->BehaviourDefVariableTypes[i])
      return (BhDefVariableType)i;
  }

  return bhvtUnknown;
}

//----------------------------------------------------------------------------------------------------------------------
const PString& KeywordsManager::stringFromBehaviourDefVariableType(BhDefVariableType vt) const
{
  assert((int)vt >= 0 && (int)vt < bhvtNumVariableTypes);
  return m_data->BehaviourDefVariableTypes[(int)vt];
}


//----------------------------------------------------------------------------------------------------------------------
#define IMPL_HAS_MODIFIER_FN(_t) \
         const ModifierOption* KeywordsManager::has##_t##Modifier(const ModifierOptions& opts, _t##Modifiers mm) const\
         { \
           unsigned int optsCount = opts.stack.count(); \
           for (unsigned int i=0; i<optsCount; i++) \
           { \
             const ModifierOption& mo = opts.stack.get(i); \
             if (mo.m_keyword == m_data->_t##ModifierKeywords[mm].m_text) \
             { \
               return &mo; \
             } \
           } \
           return 0; \
         } \
         const ModifierOption* has##_t##Modifier(const ModifierOptions& opts, _t##Modifiers mm) \
         { \
           if (opts.keywords == 0) \
           { \
             assert(opts.stack.count() == 0); \
             return 0; \
           } \
           return opts.keywords->has##_t##Modifier(opts, mm); \
         } 

IMPL_HAS_MODIFIER_FN(Type)
IMPL_HAS_MODIFIER_FN(VarDecl)
IMPL_HAS_MODIFIER_FN(Module)
IMPL_HAS_MODIFIER_FN(Struct)
IMPL_HAS_MODIFIER_FN(BehaviourVariable)
IMPL_HAS_MODIFIER_FN(ConnectionSet)
IMPL_HAS_MODIFIER_FN(ConnectionTask)

#undef IMPL_HAS_MODIFIER_FN


//----------------------------------------------------------------------------------------------------------------------
void KeywordsManager::validateModifierKeywords(const Keyword keywords[], unsigned int numKeywords, Token *token, const ModifierOptions& opts) const
{  
  unsigned int modifierUsageBitfield = 0, modifierBit;
  unsigned int optsCount = opts.stack.count();
  for (unsigned int i=0; i<optsCount; i++)
  {
    modifierUsageBitfield = 0;
    
    m_data->m_statString.set( opts.stack.get(i).m_keyword.c_str() );

    for (unsigned int t=0; t<numKeywords; t++)
    {
      if (m_data->m_statString == keywords[t].m_text)
      {
        // check to see if we have already encountered this type of modifier
        modifierBit = 1 << t;
        if ( (modifierUsageBitfield & modifierBit) == modifierBit )
        {
          throw(new RuleExecutionException(token, m_data->m_statString.c_str(), "modifier keyword used more than once"));
        }
        modifierUsageBitfield |= modifierBit;

        // validate any value requirements
        Keyword::ValueRequirement valueTypeProvided = opts.stack.get(i).m_valueType;
        if (keywords[t].m_valueRequirement == Keyword::vrNone)
        {
          // unnecessary value was provided
          if (valueTypeProvided != Keyword::vrNone)
          {
            throw(new RuleExecutionException(token, m_data->m_statString.c_str(), "modifier keyword has no extra configuration options"));
          }
        }
        else
        {
          if (valueTypeProvided != keywords[t].m_valueRequirement)
          {
            static const char* keywordReqString[Keyword::vrNumRequirementTypes] = 
            {
              "None",
              "String",
              "Identifier",
              "Decimal"
            };

            StringBuilder sb(32);
            sb.appendf("modifier keyword requires proper configuration - %s (%s)",
              keywords[t].m_valueErrorDesc.c_str(),
              keywordReqString[keywords[t].m_valueRequirement]);

            throw(new RuleExecutionException(token, m_data->m_statString.c_str(), sb.c_str()));
          }          
        }
      }
    }

    // we didn't clock any recognizable keywords
    if (modifierUsageBitfield == 0)
    {
      throw(new RuleExecutionException(token, m_data->m_statString.c_str(), "invalid modifier keyword"));
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void KeywordsManager::validateModuleModifiers(Token *token, const ModifierOptions& opts) const
{
  validateModifierKeywords(m_data->ModuleModifierKeywords, numModuleModifierKeywords, token, opts);
}

//----------------------------------------------------------------------------------------------------------------------
void KeywordsManager::validateTypeModifiers(Token *token, const ModifierOptions& opts) const
{  
  validateModifierKeywords(m_data->TypeModifierKeywords, numTypeModifierKeywords, token, opts);
}

//----------------------------------------------------------------------------------------------------------------------
void KeywordsManager::validateVarDeclModifiers(Token *token, const ModifierOptions& opts) const
{
  validateModifierKeywords(m_data->VarDeclModifierKeywords, numVarDeclModifierKeywords, token, opts);
}

//----------------------------------------------------------------------------------------------------------------------
void KeywordsManager::validateStructModifiers(Token *token, const ModifierOptions& opts) const
{
  validateModifierKeywords(m_data->StructModifierKeywords, numStructModifierKeywords, token, opts);
}

//----------------------------------------------------------------------------------------------------------------------
void KeywordsManager::validateBehaviourVariableModifiers(Token *token, const ModifierOptions& opts) const
{
  validateModifierKeywords(m_data->BehaviourVariableModifierKeywords, numBehaviourVariableKeywords, token, opts);
}

//----------------------------------------------------------------------------------------------------------------------
void KeywordsManager::validateConnectionSetModifiers(Token *token, const ModifierOptions& opts) const
{
  validateModifierKeywords(m_data->ConnectionSetModifierKeywords, numConnectionSetKeywords, token, opts);
}

//----------------------------------------------------------------------------------------------------------------------
void KeywordsManager::validateConnectionTaskModifiers(Token *token, const ModifierOptions& opts) const
{
  validateModifierKeywords(m_data->ConnectionTaskModifierKeywords, numConnectionTaskKeywords, token, opts);
}

//----------------------------------------------------------------------------------------------------------------------
void KeywordsManager::dumpModifierOptions(const ModifierOptions& opts, StringBuilder& sb) const
{
  unsigned int optsCount = opts.stack.count();
  for (unsigned int i=0; i<optsCount; i++)
  {
    const ModifierOption& mopt = opts.stack.get(i);

    sb.appendPString(mopt.m_keyword);
    if (mopt.m_valueType != Keyword::vrNone)
    {
      sb.appendChar('(');
      sb.appendPString(mopt.m_value);
      sb.appendChar(')');
    }

    sb.appendChar(' ');
  }
}


//----------------------------------------------------------------------------------------------------------------------
bool KeywordsManager::isReservedKeyword(const char* inString) const
{
  if (inString == 0)
    return false;

  return m_data->m_reservedKeywords.contains(inString);
}
