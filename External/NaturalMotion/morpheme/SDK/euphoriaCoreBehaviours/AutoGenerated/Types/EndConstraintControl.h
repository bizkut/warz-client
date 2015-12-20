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

#ifndef NM_MDF_TYPE_ENDCONSTRAINTCONTROL_H
#define NM_MDF_TYPE_ENDCONSTRAINTCONTROL_H

// include definition file to create project dependency
#include "./Definition/Types/Hold.types"

// external types
#include "euphoria/erLimbTransforms.h"
#include "NMPlatform/NMMatrix34.h"

// for combiners
#include "euphoria/erJunction.h"

// constants
#include "NetworkConstants.h"

// hook functions
#include "euphoria/erValueValidators.h"
#include "euphoria/erValuePostCombiners.h"

namespace ER { class DimensionalScaling; } 

#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// Declaration from 'Hold.types'
// Data Payload: 96 Bytes
// Alignment: 16

//----------------------------------------------------------------------------------------------------------------------
// Describes how the constraint between an end-effector and a physics actor behaves.
//----------------------------------------------------------------------------------------------------------------------

struct EndConstraintControl
{

  ER::HandFootTransform desiredTM;  ///< Desired pose of the end-effector in world space.

  int64_t targetShapeID;  ///< Cast from pointer to physics actor.

  float createDistance;  ///< Distance between endEff and target at which constraint is created.  (Length)

  float destroyDistance;  ///< Distance between endEff and target at which constraint is destroyed.  (Length)

  float startOrientationAngle;  ///< Angular distance between endEff and target orientation to start locking orientation.  (Angle)

  float stopOrientationAngle;  ///< Angular distance between endEff and target orientation to stop locking orientation.  (Angle)

  uint16_t lockedLinearDofs;  ///< Bitmask specifying linear dofs locked by constraint.

  uint16_t lockedAngularDofs;  ///< Bitmask specifying angular dofs locked by constraint.

  bool disableCollisions;  ///< Toggle collisions between end-effector and constrainee.

  bool constrainOnContact;  ///< Allow constraining of end-effector to any physical actor it is in contact with.

  bool useCheatForces;  ///< Assist hand positioning and chest orientation with helper forces.

  bool disableAngularDofsUntilHanging;  ///< Wait until hanging vertically before locking the angular dofs


  // functions

  void initialise(const ER::DimensionalScaling& scaling);
  /// Set the desired orientation of the end-effector in world space.
  void setDesiredOrientation(NMP::Matrix34& desTM);
  /// Provide a pointer (cast to int) to the object to constrain to.
  void setTargetShapeID(int64_t shapeID);
  // Disable collisions between end-effector and constrainee.
  void setDisableCollisions(bool disable);


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(createDistance), "createDistance");
    NM_VALIDATOR(FloatNonNegative(destroyDistance), "destroyDistance");
    NM_VALIDATOR(FloatValid(startOrientationAngle), "startOrientationAngle");
    NM_VALIDATOR(FloatValid(stopOrientationAngle), "stopOrientationAngle");
#endif // NM_CALL_VALIDATORS
  }

}; // struct EndConstraintControl


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_ENDCONSTRAINTCONTROL_H

