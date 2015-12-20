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

#include "ParserBase.h"
#include "PString.h"
#include "MDF.h"

namespace MDFBehaviour
{

//----------------------------------------------------------------------------------------------------------------------
struct Context : ContextBase
{
  Context() : ContextBase(),
    m_modulePath(0),
    m_enumdecl(0),
    m_attrGroup(0),
    m_attrVar(0),
    m_animInput(0),
    m_def(0) 
  {}

  BehaviourDef::ModuleToEnable     *m_modulePath;

  // enum being built
  BehaviourDef::AttributeGroup::EnumDecl
                                   *m_enumdecl;

  // current group
  BehaviourDef::AttributeGroup     *m_attrGroup;

  // current group
  BehaviourDef::ControlParamGroup  *m_paramGroup;

  // current attribute
  BehaviourDef::AttributeVariable  *m_attrVar;

  // current attribute
  BehaviourDef::AnimationInput     *m_animInput;

  BhDefVariableType                 m_attrVT;

  enum AttributeDataBlock
  {
    ADBDefault,
    ADBMin,
    ADBMax
  };

  enum { kMaxLiterals = 4 };

  int                               m_dataBlock;
  BehaviourDef::AttributeData       m_literals[4], m_data;
  unsigned int                      m_literalsIndex;

  //--------------------------------------------------------------------------------------------------------------------
  // definition currently being populated

  BehaviourDef                     *m_def;
};

} // namespace MDFTypes
