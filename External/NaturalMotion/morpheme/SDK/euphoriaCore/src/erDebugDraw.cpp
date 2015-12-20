// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#include "euphoria/erDebugDraw.h"

//----------------------------------------------------------------------------------------------------------------------
namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 getDefaultColourForControl(DebugControl control)
{
  switch (control)
  {
  case aimControl:             return NMP::Vector3(0.6f, 0.3f, 0.1f);   /* BROWN*/
  case animateControl:         return NMP::Vector3(1.0f, 0.0f, 1.0f);   /* PURPLE*/
  case avoidControl:           return NMP::Vector3(0.4f, 1.0f, 1.0f);   /* PALE AQUA */
  case bufferControl:          return NMP::Vector3(0.5f, 0.0f, 0.0f);   /* DARK RED */
  case dodgeControl:           return NMP::Vector3(1.0f, 0.9f, 0.0f);   /* YELLOW */
  case freefallControl:        return NMP::Vector3(1.0f, 0.4f, 1.0f);   /* LILAC */
  case grabControl:            return NMP::Vector3(1.0f, 1.0f, 1.0f);   /* WHITE*/
  case holdingSupportControl:  return NMP::Vector3(0.0f, 0.2f, 0.1f);   /* DARK BLUE GREEN */
  case interceptControl:       return NMP::Vector3(1.0f, 0.0f, 0.0f);   /* RED */
  case objectSeenControl:           return NMP::Vector3(0.0f, 0.8f, 0.0f, 0.8f); /* GREEN */
  case objectSweepSucceededControl: return NMP::Vector3(1.0f, 0.6f, 0.6f, 1.0f); /* LIGHT PINK */
  case placementControl:       return NMP::Vector3(1.0f, 1.0f, 0.7f);   /* PALE YELLOW */
  case pointControl:           return NMP::Vector3(0.0f, 0.0f, 0.5f);   /* DARK BLUE */
  case poseControl:            return NMP::Vector3(0.0f, 0.3f, 0.7f);   /* BLUE TURQUOISE*/
  case reachControl:           return NMP::Vector3(0.0f, 0.5f, 0.5f);   /* TURQUOISE*/
  case sitControl:             return NMP::Vector3(1.0f, 0.3f, 0.0f);   /* DARK ORANGE */
  case sittingSupportControl:  return NMP::Vector3(0.0f, 0.4f, 0.0f);   /* DARK GREEN */
  case spinControl:            return NMP::Vector3(0.7f, 0.5f, 0.0f);   /* DARK YELLOW */
  case standingSupportControl: return NMP::Vector3(0.4f, 0.8f, 0.0f);   /* YELLOW GREEN */
  case stepControl:            return NMP::Vector3(0.0f, 0.0f, 1.0f);   /* BLUE */
  case supportControl:         return NMP::Vector3(0.0f, 1.0f, 0.0f);   /* GREEN */
  case swingControl:           return NMP::Vector3(1.0f, 0.9f, 0.0f);   /* YELLOW */
  case writheControl:          return NMP::Vector3(1.0f, 0.6f, 0.6f);   /* PINK */
  case debugControlMax:        break; // No default so that missing enums can get picked up by the compiler
  }

  // Fall through for failure
  return NMP::Vector3(1.0f, 1.0f, 1.0f);
}

//----------------------------------------------------------------------------------------------------------------------
const char* getDebugControlTypeName(uint32_t control)
{
  switch (control)
  {
    case aimControl:                    return "Aim";
    case animateControl:                return "Animate";
    case avoidControl:                  return "Avoid";
    case bufferControl:                 return "Buffer";
    case dodgeControl:                  return "Dodge";
    case freefallControl:               return "Freefall";
    case grabControl:                   return "Grab";
    case holdingSupportControl:         return "HoldingSupport";
    case interceptControl:              return "Intercept";
    case objectSeenControl:             return "ObjectSeen";
    case objectSweepSucceededControl:   return "ObjectSweepSucceeded";
    case placementControl:              return "Placement";
    case pointControl:                  return "Point";
    case poseControl:                   return "Pose";
    case reachControl:                  return "Reach";
    case sitControl:                    return "Sit";
    case sittingSupportControl:         return "SittingSupport";
    case spinControl:                   return "Spin";
    case standingSupportControl:        return "StandingSupport";
    case stepControl:                   return "Step";
    case supportControl:                return "Support";
    case swingControl:                  return "Swing";
    case writheControl:                 return "Writhe";
    case debugControlMax: break; // No default so that missing enums can get picked up by the compiler
  }

  return "Unknown control";
}


//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 convertControlAmountsToDefaultColour(const LimbControlAmounts& controlAmounts)
{
  NMP::Vector3 result(NMP::Vector3::InitZero);
  float total = 0.0f;
  for (uint32_t i = 0 ; i != debugControlMax ; ++i)
  {
    result += getDefaultColourForControl((DebugControl) i) * controlAmounts.m_controlAmounts[i];
    total += controlAmounts.m_controlAmounts[i];
  }
  if (total > 0.0f)
    result /= total;
  return result;
}




}