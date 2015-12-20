// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef NM_ERDEFINES_H
#define NM_ERDEFINES_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrAttribData.h"

/// Turn param to a string literal.
#define ER_FN_NAME(x) #x, ER::x

//----------------------------------------------------------------------------------------------------------------------
namespace ER
{

enum AttribDataSemantic
{
  ATTRIB_SEMANTIC_BODY_DEF = MR::ATTRIB_SEMANTIC_NM_MAX + 1, ///< ER::BodyDef
  ATTRIB_SEMANTIC_INTERACTION_PROXY_DEF,      ///< ER::InteractionProxyDef
  ATTRIB_SEMANTIC_CHARACTER,                  ///< Reference to an ER::Character.
  ATTRIB_SEMANTIC_BEHAVIOUR_PARAMETERS,       ///< Params passed to behaviours
  ATTRIB_SEMANTIC_DEFAULT_POSE,               ///< Default pose
  ATTRIB_SEMANTIC_GUIDE_POSE,                 ///< Guide pose
};

enum AttribDataTypeEnum
{
  ATTRIB_TYPE_BODY_DEF = MR::ATTRIB_TYPES_CORE_MAX,
  ATTRIB_TYPE_INTERACTION_PROXY_DEF,      ///< Holds a pointer to the interaction proxy def
  ATTRIB_TYPE_CHARACTER,
  ATTRIB_TYPE_BEHAVIOUR_SETUP,
  ATTRIB_TYPE_BEHAVIOUR_PARAMETERS,
  ATTRIB_TYPE_BEHAVIOUR_STATE,

  ATTRIB_TYPE_OPERATOR_HIT_DEF,                     ///< Hit operator definition
  ATTRIB_TYPE_OPERATOR_HIT_STATE,                   ///< A type to hold hit state
  ATTRIB_TYPE_OPERATOR_ROLLDOWNSTAIRS_DEF,          ///< RollDownStairs definition
  ATTRIB_TYPE_OPERATOR_CONTACTREPORTER_DEF,         ///< Hit detector definition
  ATTRIB_TYPE_OPERATOR_PHYSICALCONSTRAINT_DEF,      ///< Physical constraint operator definition
  ATTRIB_TYPE_OPERATOR_PHYSICALCONSTRAINT_STATE,    ///< Physical constraint operator state
  ATTRIB_TYPE_OPERATOR_ORIENTATIONINFREEFALL_DEF,   ///< Orientation in freefall definition
  ATTRIB_TYPE_OPERATOR_ORIENTATIONINFREEFALL_STATE, ///< Orientation in freefall definition
  ATTRIB_TYPE_OPERATOR_FALLOVERWALL_DEF,
  ATTRIB_TYPE_OPERATOR_FALLOVERWALL_STATE,

  ATTRIB_TYPES_EUPHORIA_MAX,
};

} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_ERDEFINES_H
//----------------------------------------------------------------------------------------------------------------------
