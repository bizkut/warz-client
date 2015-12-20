#pragma once

/*
* Copyright (c) 2012 NaturalMotion Ltd. All rights reserved.
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

#ifndef NM_LIMBCONTACT_H
#define NM_LIMBCONTACT_H

#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
#error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
//----------------------------------------------------------------------------------------------------------------------
inline void updateLimbGroundContactFeedback(
  const ContactInfo& contactInfo,
  const NMP::Vector3& upDirection,
  const bool forceGroundContact,
  const float timeStep,
  const float collidingSupportTime,
  const float cosMaxSlopeForGround,
  float& groundCollidingTime,
  float& onGroundAmount,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  // We differentiate between any collisions and collisions with the ground - get that from the end contact
  bool inGroundContact = false;

  if (contactInfo.inContact)
  {
    MR_DEBUG_DRAW_VECTOR(
      pDebugDrawInst,
      MR::VT_Delta,
      contactInfo.lastPosition,
      contactInfo.lastForce,
      NMP::Colour::BLUE);

    float dot = contactInfo.lastNormal.dot(upDirection);
    if (dot > cosMaxSlopeForGround)
    {
      inGroundContact = true;
    }
  }

  // below implements a time based colliding flag.
  // +ve means amount of time it has been colliding,
  // -ve is amount of time since it was last colliding
  if (inGroundContact || forceGroundContact)
  {
    if (groundCollidingTime < 0.0f)
    {
      groundCollidingTime = 0.0f;
    }
    groundCollidingTime += timeStep;
  }
  else
  {
    if (groundCollidingTime > 0.0f)
    {
      groundCollidingTime = 0.0f;
    }
    groundCollidingTime -= timeStep;
  }

  // When we're in the air still consider the last ground contact as providing some residual support
  // that ramps down over time.
  if (groundCollidingTime > -collidingSupportTime)
  {
    onGroundAmount = NMP::clampValue(
      (groundCollidingTime + collidingSupportTime) / collidingSupportTime, 0.0f, 1.0f);
  }
  else
  {
    onGroundAmount = 0.0f;
  }
}

//----------------------------------------------------------------------------------------------------------------------
inline void updateLimbCollidingTime(
  const bool isColliding,
  const float timeStep,
  float& collidingTime)
{
  if (isColliding)
  {
    if (collidingTime < 0.0f)
    {
      collidingTime = 0.0f;
    }
    collidingTime += timeStep;
  }
  else
  {
    if (collidingTime > 0.0f)
    {
      collidingTime = 0.0f;
    }
    collidingTime -= timeStep;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template< typename FeedbackPakageType, typename LimbSharedStateType> 
inline void updateLimbEndContactFeedback(
  FeedbackPakageType& pkg,
  const bool forceGroundContact,
  const LimbSharedStateType& limbState,
  const float timeStep,
  const float collidingSupportTime,
  MR::InstanceDebugInterface* pDebugDrawInst)
{
  updateLimbGroundContactFeedback(
    limbState.m_endContact,//pkg.feedIn->getEndContact(),
    pkg.owner->data->up,
    forceGroundContact,
    timeStep,
    collidingSupportTime,
    pkg.owner->data->cosMaxSlopeForGround,
    pkg.data->endGroundCollidingTime,
    pkg.data->endOnGroundAmount,
    pDebugDrawInst);
}

//----------------------------------------------------------------------------------------------------------------------
template< typename FeedbackPakageType, typename LimbSharedStateType> 
inline void updateLimbRootContactFeedback(
  FeedbackPakageType& pkg,
  const bool forceGroundContact,
  const LimbSharedStateType& limbState,
  const float timeStep,
  const float collidingSupportTime,
  MR::InstanceDebugInterface* pDebugDrawInst)
{
  updateLimbGroundContactFeedback(
    limbState.m_rootContact, //pkg.feedIn->getRootContact(),
    pkg.owner->data->up,
    forceGroundContact,
    timeStep,
    collidingSupportTime,
    pkg.owner->data->cosMaxSlopeForGround,
    pkg.data->rootGroundCollidingTime,
    pkg.data->rootOnGroundAmount,
    pDebugDrawInst);
}

}

#endif // NM_LIMBCONTACT_H
