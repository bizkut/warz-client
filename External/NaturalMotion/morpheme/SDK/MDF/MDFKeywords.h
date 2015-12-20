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

//----------------------------------------------------------------------------------------------------------------------

class PString;
class StringBuilder;

//----------------------------------------------------------------------------------------------------------------------
// list of all the different blocks available in the MDF
enum ModuleSection
{
  msInvalid = -1,

  // -----------------
  // any sections that list vardecls come first
  msData = 0,
  msFeedInputs,
  msInputs,
  msUserInputs,
  msFeedOutputs,
  msOutputs,

  // -----------------
  msOwner,
  msChildren,
  msPrivateAPI,
  msPublicAPI,
  msConnections,

  // -----------------
  msNumSections,
  msNumVarDeclBlocks = (msOutputs + 1)  // bottom of vardecl block
};


//----------------------------------------------------------------------------------------------------------------------
// list of all the different things a single connection can do
enum ConnectionTask
{
  ctInvalid = -1,

  // -----------------
  ctCreateJunction = 0,       // this defines a new junction, eg. "junctions.data = priority(in1, in2);"
  ctAssignJunction,           // assigns a junction to a module vardecl, eg. "in.value = junctions.data;"

  // -----------------
  ctGatherOp,                 // meta-task that generates multiple junctions/assignments automatically for arrays

  // -----------------
  ctNumTasks
};


//----------------------------------------------------------------------------------------------------------------------
enum ComponentContext
{
  ccInvalid = -8,

  // -----------------
  // this block is reserved for types that are assigned during
  // analysis phase, not found by string look-up
  ccJunctionName,
  ccVariableName,
  ccModuleName,

  // instance variable types
  ccIVChildIndex,

  // -----------------
  // Unknown is not a 'bad' token, just means a path component
  // that hasn't matched any of the below; it will be reassigned
  // to one of the above during analysis
  ccUnknown = -1,

  // -----------------
  // standard types, found by string match during parsing
  ccOwner = 0,
  ccJunctions,
  ccFeedInputs,
  ccInputs,
  ccUserInputs,
  ccFeedOutputs,
  ccOutputs,

  // -----------------
  ccNumComponents
};


//----------------------------------------------------------------------------------------------------------------------
// list of all junction modes that euphoria can utilize
enum JunctionMode
{
  jmInvalid = -1,

  // -----------------
  jmDirectInput = 0,
  jmAverage,
  jmWinnerTakesAll,
  jmPriority,
  jmSum,
  jmMaxFloat,
  jmMaxFloatValue,
  jmIsBoolSet,

  // -----------------
  jmNumJunctionModes
};


//----------------------------------------------------------------------------------------------------------------------
// we have a specific, known set of types that can used during communication with morpheme, and these are they
enum BhDefVariableType
{
  bhvtUnknown = -1,
  bhvtFloat,
  bhvtInteger,
  bhvtBool,
  bhvtVector3,
  bhvtPhysicsObjectID,
  bhvtMessage,
  bhvtNumVariableTypes
};


//----------------------------------------------------------------------------------------------------------------------
/**
 * keyword declaration structure
 */
struct Keyword
{
  enum ValueRequirement
  {
    vrNone,
    vrString,
    vrIdentifier,
    vrDecimal,

    vrNumRequirementTypes
  };

  Keyword() {}
  Keyword(const char* text, ValueRequirement vq, const char* vqDesc) :
    m_text(text),
    m_valueRequirement(vq),
    m_valueErrorDesc(vqDesc)
  {}

  PString           m_text;               // eg. __keyword__
  ValueRequirement  m_valueRequirement;   // type of extra configuration allowed/required
  PString           m_valueErrorDesc;
};


//----------------------------------------------------------------------------------------------------------------------
struct ModifierOption
{
  PString       m_keyword;
  PString       m_value;              // eg. __keyword__(value)

  Keyword::ValueRequirement   m_valueType;
};

//----------------------------------------------------------------------------------------------------------------------
struct ModifierOptions
{
  ModifierOptions() : keywords(0) {}

  FixedStack<ModifierOption, 8> stack;
  const class KeywordsManager *keywords;
};


//----------------------------------------------------------------------------------------------------------------------
enum ModuleModifiers
{
  mmGenerateInterface,            // Generate in and feedOut interfaces for this module (which must also be mmGlobalAccess)
  mmNoUpdate,                     // No update interface required for this module (do not declare or attempt to call one)
  mmNoFeedback,                   // No feedback interface required for this module (do not declare or attempt to call one)
  mmGlobalAccess,                 // mark this module as having full access (eg. need to interface with physics engine)
  mmLimb,                         // flag this module as a 'limb' type; any instance of it will add a new limb instance to the system during analysis
  mmPIX,                          // add PIX profile points to this module's update & feedback

  // -----------------
  numModuleModifierKeywords
};

//----------------------------------------------------------------------------------------------------------------------
enum TypeModifiers
{
  tmAlign,                        // set the memory alignment (in bytes) for this type, eg. __align__(16)
  tmValidator,                    // nominate a function to call to validate this type (after being set, etc)
  tmPostCombine,                  // nominate a function to call on this type's data immediately following combination
  tmPreProcessor,                 // only use if a preprocessor macro is defined

  tmExternalClass,                // tag as an externally defined type of type 'class <name>'
  tmExternalStruct,               // tag as an externally defined type of type 'struct <name>'
  tmImportAsBuiltIn,              // treat this type as globally available, fundamentally defined in the compiler (eg. float)

  tmBlendable,                    // defines that this type can be blended with floating-point scaling during combine()s

  // -----------------
  numTypeModifierKeywords
};

//----------------------------------------------------------------------------------------------------------------------
enum VarDeclModifiers
{
  vdmValidator,                   // nominate a function to call to validate this type (after being set, etc)

  // -----------------
  numVarDeclModifierKeywords
};

//----------------------------------------------------------------------------------------------------------------------
enum StructModifiers
{
  smNoCombine,                    // don't auto-generate a combine() function for this struct
  smValidator,                    // nominate a function to call to validate this type (after being set, etc)

  // -----------------
  numStructModifierKeywords
};

//----------------------------------------------------------------------------------------------------------------------
enum BehaviourVariableModifiers
{
  bvmPrivate,                     // don't add accessors for this variable when writing behaviour classes
  bvmEnumDefaultIndex,            // when defining an enum {}, this value is the default used when instantiating it
  bvmEnumNoInstance,              // when defining an enum {}, don't treat it as a instantiatable variable, just generate the enum declaration (eg. for constants)
  bvmPerAnimSet,
  bvmCompulsoryInput,             // Used to flag that a node input must be set
  bvmTitle,                       // The title (display name)
  bvmLimbIndex,                   // Indicates that connect UI should treat this variable as a limb index
  bvmLimbPartIndex,               // Indicates that connect UI should treat this variable as a limb part index
  bvmPhysicsPartIndex,            // Indicates that connect UI should treat this variable as an index into the physics rig
  bvmExposedAsPin,

  // -----------------
  numBehaviourVariableKeywords
};

//----------------------------------------------------------------------------------------------------------------------
enum ConnectionSetModifiers
{
  csmNoSetInheritance,            // for named connection sets, do not copy connections from the default (unnamed) set

  // -----------------
  numConnectionSetKeywords
};

//----------------------------------------------------------------------------------------------------------------------
enum ConnectionTaskModifiers
{
  ctmOverride,                    // when redefining a connection in a named set, one has to manually specify that you are overriding (replacing) that connection (like C# virtual fns)

  // -----------------
  numConnectionTaskKeywords
};


//----------------------------------------------------------------------------------------------------------------------
struct KeywordsManagerData;
class KeywordsManager
{
public:
  KeywordsManager();

  
  // returns true if the given string is a match to a reserved keyword (C++ or MDF), and thus we shouldn't
  // allow its use as a type name, alias, variable name, etc etc
  bool isReservedKeyword(const char* inString) const;


  //--------------------------------------------------------------------------------------------------------------------
  ModuleSection toModuleSection(const PString& str) const;
  const PString& getModuleSectionName(ModuleSection ms) const;      // eg "feedIn"
  const PString& getModuleSectionLabel(ModuleSection ms) const;     // eg "FeedbackInputs"

  ConnectionTask toConnectionTask(const PString& str) const;
  const PString& getConnectionTaskLabel(ConnectionTask ct) const;

  ComponentContext toConnectionContext(const PString& str) const;

  JunctionMode toJunctionMode(const PString& str) const;
  const PString& getJunctionModeLabel(JunctionMode jm) const;

  BhDefVariableType behaviourDefVariableTypeFromString(const PString& str) const;
  const PString& stringFromBehaviourDefVariableType(BhDefVariableType vt) const;

  //--------------------------------------------------------------------------------------------------------------------
  void validateModuleModifiers(Token *token, const ModifierOptions& opts) const;
  void validateTypeModifiers(Token *token, const ModifierOptions& opts) const;
  void validateVarDeclModifiers(Token *token, const ModifierOptions& opts) const;
  void validateStructModifiers(Token *token, const ModifierOptions& opts) const;
  void validateBehaviourVariableModifiers(Token *token, const ModifierOptions& opts) const;
  void validateConnectionSetModifiers(Token *token, const ModifierOptions& opts) const;
  void validateConnectionTaskModifiers(Token *token, const ModifierOptions& opts) const;

  void validateModifierKeywords(const Keyword keywords[], unsigned int numKeywords, Token *token, const ModifierOptions& opts) const;
  void dumpModifierOptions(const ModifierOptions& opts, StringBuilder& sb) const;

  //--------------------------------------------------------------------------------------------------------------------
  const ModifierOption* hasModuleModifier(const ModifierOptions& opts, ModuleModifiers mm) const;
  const ModifierOption* hasTypeModifier(const ModifierOptions& opts, TypeModifiers tm) const;
  const ModifierOption* hasVarDeclModifier(const ModifierOptions& opts, VarDeclModifiers vm) const;
  const ModifierOption* hasStructModifier(const ModifierOptions& opts, StructModifiers vm) const;
  const ModifierOption* hasBehaviourVariableModifier(const ModifierOptions& opts, BehaviourVariableModifiers vm) const;
  const ModifierOption* hasConnectionSetModifier(const ModifierOptions& opts, ConnectionSetModifiers vm) const;
  const ModifierOption* hasConnectionTaskModifier(const ModifierOptions& opts, ConnectionTaskModifiers vm) const;

protected:
  KeywordsManagerData  *m_data; 
};

//----------------------------------------------------------------------------------------------------------------------
const ModifierOption* hasModuleModifier(const ModifierOptions& opts, ModuleModifiers mm);
const ModifierOption* hasTypeModifier(const ModifierOptions& opts, TypeModifiers tm);
const ModifierOption* hasVarDeclModifier(const ModifierOptions& opts, VarDeclModifiers vm);
const ModifierOption* hasStructModifier(const ModifierOptions& opts, StructModifiers vm);
const ModifierOption* hasBehaviourVariableModifier(const ModifierOptions& opts, BehaviourVariableModifiers vm);
const ModifierOption* hasConnectionSetModifier(const ModifierOptions& opts, ConnectionSetModifiers vm);
const ModifierOption* hasConnectionTaskModifier(const ModifierOptions& opts, ConnectionTaskModifiers vm);

//----------------------------------------------------------------------------------------------------------------------
// macro to help check for reserved keywords during parsing - automatically throws a rule exception if one is found
#define CheckTokenForReservedKeyword(_tokenIdx) \
  if (getContext()->m_keywords->isReservedKeyword(token->m_tokens[_tokenIdx]->m_data)) \
{ \
  throw(new RuleExecutionException(token, token->m_tokens[_tokenIdx]->m_data, "invalid use of reserved keyword (C++ / MDF)")); \
}
