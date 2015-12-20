#pragma once

/*
 * Copyright (c) 2013 NaturalMotion Ltd. All rights reserved.
 *
 * Not to be copied, adapted, modified, used, distributed, sold,
 * licensed or commercially exploited in any manner without the
 * written consent of NaturalMotion.
 *
 * All non public elements of this software are the confidential
 * information of NaturalMotion and may not be disclosed to any
 * person nor used for any purpose not expressly approved by
 * NaturalMotion in writing.
 *
 */

//----------------------------------------------------------------------------------------------------------------------
//                                  This file is auto-generated
//----------------------------------------------------------------------------------------------------------------------

#ifndef NM_MDF_ARM_PKG_H
#define NM_MDF_ARM_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/Arm.module"

#include "ArmData.h"
#include "ArmGrabData.h"
#include "ArmAimData.h"
#include "ArmHoldData.h"
#include "ArmBraceData.h"
#include "ArmStandingSupportData.h"
#include "ArmHoldingSupportData.h"
#include "ArmSittingStepData.h"
#include "ArmStepData.h"
#include "ArmSpinData.h"
#include "ArmSwingData.h"
#include "ArmReachForBodyPartData.h"
#include "ArmReachForWorldData.h"
#include "ArmPlacementData.h"
#include "ArmPoseData.h"
#include "ArmReachReactionData.h"
#include "ArmWritheData.h"

#include "MyNetworkPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

struct ArmGrabUpdatePackage;
struct ArmGrabFeedbackPackage;
class ArmGrab_Con;
struct ArmAimUpdatePackage;
struct ArmAimFeedbackPackage;
class ArmAim_Con;
struct ArmHoldUpdatePackage;
struct ArmHoldFeedbackPackage;
class ArmHold_Con;
struct ArmBraceUpdatePackage;
struct ArmBraceFeedbackPackage;
class ArmBrace_Con;
struct ArmStandingSupportUpdatePackage;
struct ArmStandingSupportFeedbackPackage;
class ArmStandingSupport_Con;
struct ArmHoldingSupportUpdatePackage;
struct ArmHoldingSupportFeedbackPackage;
class ArmHoldingSupport_Con;
struct ArmSittingStepUpdatePackage;
struct ArmSittingStepFeedbackPackage;
class ArmSittingStep_Con;
struct ArmStepUpdatePackage;
struct ArmStepFeedbackPackage;
class ArmStep_Con;
struct ArmSpinUpdatePackage;
struct ArmSpinFeedbackPackage;
class ArmSpin_Con;
struct ArmSwingUpdatePackage;
struct ArmSwingFeedbackPackage;
class ArmSwing_Con;
struct ArmReachForBodyPartUpdatePackage;
struct ArmReachForBodyPartFeedbackPackage;
class ArmReachForBodyPart_Con;
struct ArmReachForWorldUpdatePackage;
struct ArmReachForWorldFeedbackPackage;
class ArmReachForWorld_Con;
struct ArmPlacementUpdatePackage;
struct ArmPlacementFeedbackPackage;
class ArmPlacement_Con;
struct ArmPoseUpdatePackage;
struct ArmPoseFeedbackPackage;
class ArmPose_Con;
struct ArmReachReactionUpdatePackage;
struct ArmReachReactionFeedbackPackage;
class ArmReachReaction_Con;
struct ArmWritheUpdatePackage;
struct ArmWritheFeedbackPackage;
class ArmWrithe_Con;

//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct ArmAPIBase
{
  ArmAPIBase(
    const ArmData* const _data, 
    const ArmInputs* const _in, 
    const MyNetworkAPIBase* const _owner  ) :data(_data) ,in(_in) ,owner(_owner)  {}

  const ArmData* const data;
  const ArmInputs* const in;

  const MyNetworkAPIBase* const owner;

  ArmAPIBase(const ArmAPIBase& rhs);
  ArmAPIBase& operator = (const ArmAPIBase& rhs);

  // public API functions declared in module definition

  bool canReachPoint(const NMP::Vector3& position, bool& canReach, float lookOutOfRangeWeight=0, MR::InstanceDebugInterface* pDebugDrawInst=0) const;
  float lowerTargetWhenBehindBack(const NMP::Vector3& worldTarget, NMP::Vector3& desiredPosition, const NMP::Vector3& shoulderOffset, MR::InstanceDebugInterface* pDebugDrawInst=0) const;
  NMP::Vector3 avoidSpineUsingAngleCoefficient(const NMP::Vector3& target, const NMP::Vector3& pelvisPos, float selfAvoidanceSlackAngle, MR::InstanceDebugInterface* pDebugDrawInst, float* effectorToTargetAngleOffset=0, NMP::Vector3* adjustedApproach=0) const;

};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct ArmUpdatePackage : public ArmAPIBase
{
  ArmUpdatePackage(
    ArmData* const _data, 
    const ArmInputs* const _in, 
    ArmOutputs* const _out, 
    const MyNetworkAPIBase* const _owner  ) : ArmAPIBase(_data ,_in ,_owner ), data(_data), out(_out) 
  {
  }

  ArmData* const data;

  ArmOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmUpdatePackage(const ArmUpdatePackage& rhs);
  ArmUpdatePackage& operator = (const ArmUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct ArmFeedbackPackage : public ArmAPIBase
{
  ArmFeedbackPackage(
    ArmData* const _data, 
    const ArmInputs* const _in, 
    ArmFeedbackOutputs* const _feedOut, 
    const MyNetworkAPIBase* const _owner  ) : ArmAPIBase(_data ,_in ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  ArmData* const data;

  ArmFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmFeedbackPackage(const ArmFeedbackPackage& rhs);
  ArmFeedbackPackage& operator = (const ArmFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARM_PKG_H

