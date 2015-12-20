// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
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
#ifndef NM_PHYSICSNODES_H
#define NM_PHYSICSNODES_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrManager.h"
#include "morpheme/mrDispatcher.h"
#include "physics/mrPhysics.h"
#include "physics/mrPhysicsAttribData.h"
#include "physics/Nodes/mrNodeControlParamPhysicsObjectPointer.h"
#include "physics/Nodes/mrNodeExpandLimits.h"
#include "physics/Nodes/mrNodePhysics.h"
#include "physics/Nodes/mrNodePhysicsGrouper.h"
#include "physics/Nodes/mrNodeSetNonPhysicsTransforms.h"
#include "physics/Nodes/mrNodeApplyPhysicsJointLimits.h"
#include "physics/Nodes/mrNodeOperatorApplyImpulse.h"
#include "physics/Nodes/mrTransitConditionGroundContact.h"
#include "physics/Nodes/mrTransitConditionPhysicsAvailable.h"
#include "physics/Nodes/mrTransitConditionPhysicsInUse.h"
#include "physics/Nodes/mrTransitConditionPhysicsMoving.h"
#include "physics/Nodes/mrNodePhysicsTransit.h"
#include "physics/Nodes/mrTransitConditionSKDeviation.h"
#include "physics/Nodes/mrNodeOperatorPhysicsInfo.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief Register the functions which handle loading binary assets associated with the morpheme physics plugin
//----------------------------------------------------------------------------------------------------------------------
void registerPhysicsAssets();

//----------------------------------------------------------------------------------------------------------------------
/// \brief Register the task queuing and outputCP functions which are used by nodes in the morpheme physics plugin
//----------------------------------------------------------------------------------------------------------------------
void registerPhysicsQueuingFnsAndOutputCPTasks();

#ifdef NM_HOST_CELL_SPU
  #define NMP_NULL_ON_SPU(_wat) NULL
  #define NMP_NULL_NOT_ON_SPU(_wat) _wat
#else
  #define NMP_NULL_ON_SPU(_wat) _wat
  #define NMP_NULL_NOT_ON_SPU(_wat) NULL
#endif //NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
/// \brief Register the transit conditions which are added by the morpheme physics plugin
//----------------------------------------------------------------------------------------------------------------------
void registerPhysicsTransitConditions();

//----------------------------------------------------------------------------------------------------------------------
/// \brief Register the new attrib data types which are used by the morpheme physics plugin
//----------------------------------------------------------------------------------------------------------------------
void registerPhysicsAttribDataTypes();

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void registerPhysicsAttribSemantics(
  bool NMP_UNUSED(computeRegistryRequirements) ///< true:  Determines required array sizes for storage of registered functions, data types etc.
                                               ///< false: Registers functions, data types etc.
  )
{
}

//----------------------------------------------------------------------------------------------------------------------
/// Helper functions providing simple initialization of the morpheme physics module. All the API used is
/// public so the contents of this function can be called directly for finer control over initialization.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \brief Registers task queuing functions, attribute data types etc with Manager. Accumulates manager registry requirements etc.
void initMorphemePhysics(uint32_t numDispatchers, Dispatcher** dispatchers);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Registration of semantics etc with the manager, post Manager::allocateRegistry().
void finaliseInitMorphemePhysics();

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_PHYSICSNODES_H
//----------------------------------------------------------------------------------------------------------------------
