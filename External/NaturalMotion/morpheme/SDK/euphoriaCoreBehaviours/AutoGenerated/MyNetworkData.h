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

#ifndef NM_MDF_MYNETWORK_DATA_H
#define NM_MDF_MYNETWORK_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/MyNetwork.module"

// external types
#include "NMPlatform/NMVector3.h"
#include "euphoria/erDimensionalScaling.h"
#include "NMPlatform/NMMatrix34.h"
#include "Helpers/LimbSharedState.h"

// hook functions
#include "euphoria/erValueValidators.h"
#include "euphoria/erValuePostCombiners.h"

// constants
#include "NetworkConstants.h"

#include <string.h>

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

struct LimbControl; 

#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable: 4324) // structure was padded due to __declspec(align()) 
#endif // _MSC_VER

/** 
 * \class MyNetworkData
 * \ingroup MyNetwork
 * \brief Data This is the root container module for the network, non-spu so it can
 *  interface with physics engine and the behaviour technology
 *
 * Data packet definition (1268 bytes, 1280 aligned)
 */
NMP_ALIGN_PREFIX(32) struct MyNetworkData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(MyNetworkData));
  }

  // Note - indices into the network arrays
  enum firstLimbNetworkIndices
  {
    /*  0 */ firstArmNetworkIndex = 0,  
    /*  2 */ firstHeadNetworkIndex = (NetworkConstants::networkMaxNumArms),  
    /*  3 */ firstLegNetworkIndex = (NetworkConstants::networkMaxNumArms + NetworkConstants::networkMaxNumHeads),  
    /*  5 */ firstSpineNetworkIndex = (NetworkConstants::networkMaxNumArms + NetworkConstants::networkMaxNumHeads + NetworkConstants::networkMaxNumLegs),  
  };

  NMP::Matrix34 defaultPoseEndsRelativeToRoot[NetworkConstants::networkMaxNumLimbs];  // These will be calculated once at initialisation, and will then be available to all modules. Each module can get
  // them via either the limbRigIndex in the limb module, or using offsets and the first*NetworkIndex above.  (Transform)
  NMP::Matrix34 zeroPoseEndsRelativeToRoot[NetworkConstants::networkMaxNumLimbs];  ///< (Transform)
  NMP::Matrix34 averageLegDefaultPoseEndRelativeToRoot;  ///< (Transform)
  NMP::Matrix34 averageArmDefaultPoseEndRelativeToRoot;  ///< (Transform)
  NMP::Vector3 down;  ///< Unit length in direction of gravity  (Direction)
  BodyLimbSharedState bodyLimbSharedState;
  NMP::Vector3 up;  ///< Unit length opposing gravity  (Direction)
  NMP::Vector3 gravity;  ///< Gravity vector  (Acceleration)
  NMP::Vector3 averageOfAllFootPositions;    ///< (Position)
  NMP::Vector3 averageOfAllHandPositions;  // Average end effector positions
  //  (Position)
  HeadLimbSharedState headLimbSharedStates[NetworkConstants::networkMaxNumHeads];  // Limb state info
  //
  SpineLimbSharedState spineLimbSharedStates[NetworkConstants::networkMaxNumSpines];
  ArmAndLegLimbSharedState armLimbSharedStates[NetworkConstants::networkMaxNumArms];
  ArmAndLegLimbSharedState legLimbSharedStates[NetworkConstants::networkMaxNumLegs];
  ER::DimensionalScaling dimensionalScaling;
  float collidingSupportTime;  ///< When a supporting contact is lost, the character still considers itself partially supported during this time period. This is so that it does not suddenly "panic" if it hops off the ground.  (TimePeriod)
  float awarenessPredictionTime;  ///< Interact with objects when they're predicted to reach us within this time.  (TimePeriod)
  float cosMaxSlopeForGround;  ///< If a surface slopes more than cos(angle in degrees), then it will not be considered "ground" for the purposes of balance etc.  (Angle)
  uint32_t numArms;  // These are indices etc into the rig limbs, so not typically useful as most network processing works on
  uint32_t numHeads;
  uint32_t numLegs;
  uint32_t numSpines;
  float relaxStiffness;  ///< Stiffness when the character is awake but relaxed  (Stiffness)
  uint32_t headRigIndices[NetworkConstants::networkMaxNumHeads];
  uint32_t legRigIndices[NetworkConstants::networkMaxNumLegs];
  uint32_t spineRigIndices[NetworkConstants::networkMaxNumSpines];
  uint32_t firstArmRigIndex;  // Note - indices into the rig limbs
  uint32_t firstHeadRigIndex;
  uint32_t firstLegRigIndex;
  uint32_t firstSpineRigIndex;
  float minDamping;  ///< Damping when the character is awake but relaxed, and also minimum damping  (Frequency)
  float relaxDriveCompensation;              ///< (TimePeriod)
  float defaultPoseLimbLengths[NetworkConstants::networkMaxNumLimbs];  ///< (Length)
  float gravityMagnitude;  ///< Gravity vector magnitude  (AccelerationScalar)
  float minStiffnessForActing;  ///< This is used to decide when to start an action (like a cushion or brace) based on the stiffness needed  (Stiffness)
  float maxStiffness;  ///< This should generally be fixed per character, it is a physical limit  (Stiffness)
  float selfAvoidanceRadius;  ///< Used for self avoidance in reaching  (Length)
  float normalStiffness;  ///< This could potentially vary with time (e.g. with health)  (Stiffness)
  uint32_t armRigIndices[NetworkConstants::networkMaxNumArms];
  float normalDampingRatio;  ///< 1 is critical damping  (DampingRatio)
  float normalDriveCompensation;  ///< Larger is more controlled, less spongey  (TimePeriod)
  float minInterestingRelativeMass;  ///< Unitless, e.g. <0.01 of character mass is not interesting  (Weight)
  float totalMass;  ///< Mass of whole character, e.g. approx 75kg for an adult male  (Mass)
  float baseToEndLength[NetworkConstants::networkMaxNumLimbs];  ///< (Length)

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(defaultPoseEndsRelativeToRoot[0]), "defaultPoseEndsRelativeToRoot[0]");
    NM_VALIDATOR(Matrix34Orthonormal(defaultPoseEndsRelativeToRoot[1]), "defaultPoseEndsRelativeToRoot[1]");
    NM_VALIDATOR(Matrix34Orthonormal(defaultPoseEndsRelativeToRoot[2]), "defaultPoseEndsRelativeToRoot[2]");
    NM_VALIDATOR(Matrix34Orthonormal(defaultPoseEndsRelativeToRoot[3]), "defaultPoseEndsRelativeToRoot[3]");
    NM_VALIDATOR(Matrix34Orthonormal(defaultPoseEndsRelativeToRoot[4]), "defaultPoseEndsRelativeToRoot[4]");
    NM_VALIDATOR(Matrix34Orthonormal(defaultPoseEndsRelativeToRoot[5]), "defaultPoseEndsRelativeToRoot[5]");
    NM_VALIDATOR(Matrix34Orthonormal(zeroPoseEndsRelativeToRoot[0]), "zeroPoseEndsRelativeToRoot[0]");
    NM_VALIDATOR(Matrix34Orthonormal(zeroPoseEndsRelativeToRoot[1]), "zeroPoseEndsRelativeToRoot[1]");
    NM_VALIDATOR(Matrix34Orthonormal(zeroPoseEndsRelativeToRoot[2]), "zeroPoseEndsRelativeToRoot[2]");
    NM_VALIDATOR(Matrix34Orthonormal(zeroPoseEndsRelativeToRoot[3]), "zeroPoseEndsRelativeToRoot[3]");
    NM_VALIDATOR(Matrix34Orthonormal(zeroPoseEndsRelativeToRoot[4]), "zeroPoseEndsRelativeToRoot[4]");
    NM_VALIDATOR(Matrix34Orthonormal(zeroPoseEndsRelativeToRoot[5]), "zeroPoseEndsRelativeToRoot[5]");
    NM_VALIDATOR(Matrix34Orthonormal(averageLegDefaultPoseEndRelativeToRoot), "averageLegDefaultPoseEndRelativeToRoot");
    NM_VALIDATOR(Matrix34Orthonormal(averageArmDefaultPoseEndRelativeToRoot), "averageArmDefaultPoseEndRelativeToRoot");
    NM_VALIDATOR(Vector3Normalised(down), "down");
    NM_VALIDATOR(Vector3Normalised(up), "up");
    NM_VALIDATOR(Vector3Valid(gravity), "gravity");
    NM_VALIDATOR(Vector3Valid(averageOfAllFootPositions), "averageOfAllFootPositions");
    NM_VALIDATOR(Vector3Valid(averageOfAllHandPositions), "averageOfAllHandPositions");
    NM_VALIDATOR(FloatValid(collidingSupportTime), "collidingSupportTime");
    NM_VALIDATOR(FloatValid(awarenessPredictionTime), "awarenessPredictionTime");
    NM_VALIDATOR(FloatValid(cosMaxSlopeForGround), "cosMaxSlopeForGround");
    NM_VALIDATOR(FloatNonNegative(relaxStiffness), "relaxStiffness");
    NM_VALIDATOR(FloatNonNegative(minDamping), "minDamping");
    NM_VALIDATOR(FloatValid(relaxDriveCompensation), "relaxDriveCompensation");
    NM_VALIDATOR(FloatNonNegative(defaultPoseLimbLengths[0]), "defaultPoseLimbLengths[0]");
    NM_VALIDATOR(FloatNonNegative(defaultPoseLimbLengths[1]), "defaultPoseLimbLengths[1]");
    NM_VALIDATOR(FloatNonNegative(defaultPoseLimbLengths[2]), "defaultPoseLimbLengths[2]");
    NM_VALIDATOR(FloatNonNegative(defaultPoseLimbLengths[3]), "defaultPoseLimbLengths[3]");
    NM_VALIDATOR(FloatNonNegative(defaultPoseLimbLengths[4]), "defaultPoseLimbLengths[4]");
    NM_VALIDATOR(FloatNonNegative(defaultPoseLimbLengths[5]), "defaultPoseLimbLengths[5]");
    NM_VALIDATOR(FloatValid(gravityMagnitude), "gravityMagnitude");
    NM_VALIDATOR(FloatNonNegative(minStiffnessForActing), "minStiffnessForActing");
    NM_VALIDATOR(FloatNonNegative(maxStiffness), "maxStiffness");
    NM_VALIDATOR(FloatNonNegative(selfAvoidanceRadius), "selfAvoidanceRadius");
    NM_VALIDATOR(FloatNonNegative(normalStiffness), "normalStiffness");
    NM_VALIDATOR(FloatNonNegative(normalDampingRatio), "normalDampingRatio");
    NM_VALIDATOR(FloatValid(normalDriveCompensation), "normalDriveCompensation");
    NM_VALIDATOR(FloatNonNegative(minInterestingRelativeMass), "minInterestingRelativeMass");
    NM_VALIDATOR(FloatNonNegative(totalMass), "totalMass");
    NM_VALIDATOR(FloatNonNegative(baseToEndLength[0]), "baseToEndLength[0]");
    NM_VALIDATOR(FloatNonNegative(baseToEndLength[1]), "baseToEndLength[1]");
    NM_VALIDATOR(FloatNonNegative(baseToEndLength[2]), "baseToEndLength[2]");
    NM_VALIDATOR(FloatNonNegative(baseToEndLength[3]), "baseToEndLength[3]");
    NM_VALIDATOR(FloatNonNegative(baseToEndLength[4]), "baseToEndLength[4]");
    NM_VALIDATOR(FloatNonNegative(baseToEndLength[5]), "baseToEndLength[5]");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_MYNETWORK_DATA_H

