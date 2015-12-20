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

#ifndef NM_MDF_BDEF_REACHFORBODY_H
#define NM_MDF_BDEF_REACHFORBODY_H

// include definition file to create project dependency
#include "./Definition/Behaviours/ReachForBody.behaviour"

#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMVector3.h"
#include "physics/mrPhysicsSerialisationBuffer.h"
#include "euphoria/erBehaviour.h"
#include "euphoria/erLimb.h"

#include "NetworkDescriptor.h"
#include "NetworkConstants.h"


namespace ER
{
  class Character;
  class Module;
}




#ifdef NM_COMPILER_MSVC
# pragma warning ( push )
# pragma warning ( disable : 4324 ) // structure was padded due to __declspec(align()) 
#endif // NM_COMPILER_MSVC

#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

struct ReachForBodyPoseRequirements
{
  bool enablePoseInput_ArmReachForBodyPose;

  ReachForBodyPoseRequirements()
  {
    enablePoseInput_ArmReachForBodyPose = true;
  }
};

struct ReachForBodyBehaviourData
{
  friend class ReachForBodyBehaviour;

private:


  struct ControlParams
  {
    NMP::Vector3 Position[NetworkConstants::networkMaxNumArms];  ///< Local surface point to reach to on the specified limb part (in frame of a physics joint).
    NMP::Vector3 Normal[NetworkConstants::networkMaxNumArms];  ///< Local surface normal of point to be reached to (in frame of a physics joint).
    float Strength[NetworkConstants::networkMaxNumArms];  ///< Sets the emphasis placed on the reach behaviour for this limb. A reach toward a target can be effectively faded out by ramping this value from 1 to 0.
    float EffectorSpeedLimit[NetworkConstants::networkMaxNumArms];  ///< Sets the maximum speed of the hand when moving towards the target. In m/s (standard character).
    int32_t LimbIndex[NetworkConstants::networkMaxNumArms];  ///< Index of the limb to reach for
    int32_t PartIndex[NetworkConstants::networkMaxNumArms];  ///< Index of the part to reach for, e.g. the upper arm, or lower leg.
    bool newHit[NetworkConstants::networkMaxNumArms];  ///< Indicates that a new target has been supplied. Should be set to true on the frame in which the target is changed.
  };
  ControlParams m_cp;

  BodyPoseData  pd_ArmReachForBodyPose;
  float OutOfReachTimeout[NetworkConstants::networkMaxNumArms];  ///< Give up after this time out of reach, in seconds (standard character).
  float RampDownFailedDuration[NetworkConstants::networkMaxNumArms];  ///< Back off ramp (after failed reach) duration, in seconds (standard character).
  float WithinReachTimeout[NetworkConstants::networkMaxNumArms];  ///< Give up after this time within reach, in seconds (standard character)..
  float RampDownCompletedDuration[NetworkConstants::networkMaxNumArms];  ///< Back off ramp (after successful reach) duration, in seconds (standard character)..
  float OutOfReachDistance[NetworkConstants::networkMaxNumArms];  ///< On target when within this distance, in metres (standard character).
  int32_t SwivelMode[NetworkConstants::networkMaxNumArms];  /// Defines how or whether arm swivel should be applied to the reach: "None" means swivel is not required; "Automatic" means swivel will be determined procedurally by the behaviour based on the reach target, "Specified" means the behaviour will use the fixed swivel amount specified below.
  
  float SwivelAmount[NetworkConstants::networkMaxNumArms];  /// Swivel value to use when Swivel Mode is "Specified". Value in range [-1,1],  positive values drive elbows out and up, negative drive elbows in and down.
  
  float MaxArmExtensionScale[NetworkConstants::networkMaxNumArms];  ///< Limit the maximum extension of the arm. Expressed as a proportion of the total arm length.
  float SurfacePenetration[NetworkConstants::networkMaxNumArms];  ///< Small offset, to drive hand against surface, in metres (standard character).
  float TorsoAvoidanceRadiusMultiplier[NetworkConstants::networkMaxNumArms];  ///< Scales the self avoidance radius set on the spine limb.
  int32_t CollisionGroupIndex;  ///< Collision group used to disable hand-hand collision when reaching. This is the collision group within the collision groups editor.
  float ArmReachForBodyPose_ApplyToArm_1;  // Set how much of the pose to use on this Arm.
  float ArmReachForBodyPose_ApplyToArm_2;  // Set how much of the pose to use on this Arm.
  float ArmReachForBodyPose_ApplyToHead_1;  // Set how much of the pose to use on this Head.
  float ArmReachForBodyPose_ApplyToLeg_1;  // Set how much of the pose to use on this Leg.
  float ArmReachForBodyPose_ApplyToLeg_2;  // Set how much of the pose to use on this Leg.
  float ArmReachForBodyPose_ApplyToSpine_1;  // Set how much of the pose to use on this Spine.


public:

  ReachForBodyBehaviourData()
  {
    OutOfReachTimeout[0] = float(1);
    OutOfReachTimeout[1] = float(1);
    RampDownFailedDuration[0] = float(1);
    RampDownFailedDuration[1] = float(1);
    WithinReachTimeout[0] = float(5);
    WithinReachTimeout[1] = float(5);
    RampDownCompletedDuration[0] = float(2);
    RampDownCompletedDuration[1] = float(2);
    OutOfReachDistance[0] = float(0.15);
    OutOfReachDistance[1] = float(0.15);
    SwivelMode[0] = 0;
    SwivelMode[1] = 0;
    SwivelAmount[0] = float(0);
    SwivelAmount[1] = float(0);
    MaxArmExtensionScale[0] = float(1);
    MaxArmExtensionScale[1] = float(1);
    SurfacePenetration[0] = float(0.01);
    SurfacePenetration[1] = float(0.01);
    TorsoAvoidanceRadiusMultiplier[0] = float(1);
    TorsoAvoidanceRadiusMultiplier[1] = float(1);
    CollisionGroupIndex = -1;
    ArmReachForBodyPose_ApplyToArm_1 = float(1);
    ArmReachForBodyPose_ApplyToArm_2 = float(1);
    ArmReachForBodyPose_ApplyToHead_1 = float(1);
    ArmReachForBodyPose_ApplyToLeg_1 = float(1);
    ArmReachForBodyPose_ApplyToLeg_2 = float(1);
    ArmReachForBodyPose_ApplyToSpine_1 = float(1);
  }

  void updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams);

  void updateAndClearEmittedMessages(uint32_t& messages);

  void updateFromMessageParams(const ER::AttribDataBehaviourParameters* data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 24);
    NMP_ASSERT(data->m_ints->m_numValues == 3);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);


    OutOfReachTimeout[0] = data->m_floats->m_values[0];
    OutOfReachTimeout[1] = data->m_floats->m_values[1];
    RampDownFailedDuration[0] = data->m_floats->m_values[2];
    RampDownFailedDuration[1] = data->m_floats->m_values[3];
    WithinReachTimeout[0] = data->m_floats->m_values[4];
    WithinReachTimeout[1] = data->m_floats->m_values[5];
    RampDownCompletedDuration[0] = data->m_floats->m_values[6];
    RampDownCompletedDuration[1] = data->m_floats->m_values[7];
    OutOfReachDistance[0] = data->m_floats->m_values[8];
    OutOfReachDistance[1] = data->m_floats->m_values[9];
    SwivelMode[0] = data->m_ints->m_values[0];
    SwivelMode[1] = data->m_ints->m_values[1];
    SwivelAmount[0] = data->m_floats->m_values[10];
    SwivelAmount[1] = data->m_floats->m_values[11];
    MaxArmExtensionScale[0] = data->m_floats->m_values[12];
    MaxArmExtensionScale[1] = data->m_floats->m_values[13];
    SurfacePenetration[0] = data->m_floats->m_values[14];
    SurfacePenetration[1] = data->m_floats->m_values[15];
    TorsoAvoidanceRadiusMultiplier[0] = data->m_floats->m_values[16];
    TorsoAvoidanceRadiusMultiplier[1] = data->m_floats->m_values[17];
    CollisionGroupIndex = data->m_ints->m_values[2];
    ArmReachForBodyPose_ApplyToArm_1 = data->m_floats->m_values[18];
    ArmReachForBodyPose_ApplyToArm_2 = data->m_floats->m_values[19];
    ArmReachForBodyPose_ApplyToHead_1 = data->m_floats->m_values[20];
    ArmReachForBodyPose_ApplyToLeg_1 = data->m_floats->m_values[21];
    ArmReachForBodyPose_ApplyToLeg_2 = data->m_floats->m_values[22];
    ArmReachForBodyPose_ApplyToSpine_1 = data->m_floats->m_values[23];

    pd_ArmReachForBodyPose.m_poseWeights[0] = ArmReachForBodyPose_ApplyToArm_1;
    pd_ArmReachForBodyPose.m_poseWeights[1] = ArmReachForBodyPose_ApplyToArm_2;
    pd_ArmReachForBodyPose.m_poseWeights[2] = ArmReachForBodyPose_ApplyToHead_1;
    pd_ArmReachForBodyPose.m_poseWeights[3] = ArmReachForBodyPose_ApplyToLeg_1;
    pd_ArmReachForBodyPose.m_poseWeights[4] = ArmReachForBodyPose_ApplyToLeg_2;
    pd_ArmReachForBodyPose.m_poseWeights[5] = ArmReachForBodyPose_ApplyToSpine_1;
  }

//----------------------------------------------------------------------------------------------------------------------
  void updateFromControlParams(const ER::AttribDataBehaviourState* const data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 4);
    NMP_ASSERT(data->m_ints->m_numValues == 6);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);
    // Vector 3 are stored in a float array with 4 entries per item
    NMP_ASSERT(data->m_vector3Data->m_numValues == 16);

    m_cp.newHit[0] = (data->m_ints->m_values[0] != 0);
    m_cp.newHit[1] = (data->m_ints->m_values[1] != 0);
    m_cp.Strength[0] = data->m_floats->m_values[0];
    m_cp.Strength[1] = data->m_floats->m_values[1];
    m_cp.EffectorSpeedLimit[0] = data->m_floats->m_values[2];
    m_cp.EffectorSpeedLimit[1] = data->m_floats->m_values[3];
    m_cp.LimbIndex[0] = data->m_ints->m_values[2];
    m_cp.LimbIndex[1] = data->m_ints->m_values[3];
    m_cp.PartIndex[0] = data->m_ints->m_values[4];
    m_cp.PartIndex[1] = data->m_ints->m_values[5];
    m_cp.Position[0].set(data->m_vector3Data->m_values[0], data->m_vector3Data->m_values[1], data->m_vector3Data->m_values[2]);
    m_cp.Position[1].set(data->m_vector3Data->m_values[4], data->m_vector3Data->m_values[5], data->m_vector3Data->m_values[6]);
    m_cp.Normal[0].set(data->m_vector3Data->m_values[8], data->m_vector3Data->m_values[9], data->m_vector3Data->m_values[10]);
    m_cp.Normal[1].set(data->m_vector3Data->m_values[12], data->m_vector3Data->m_values[13], data->m_vector3Data->m_values[14]);
  }


  /// Give up after this time out of reach, in seconds (standard character).
  float getOutOfReachTimeout(unsigned int index) const { NMP_ASSERT(index < 2); return OutOfReachTimeout[index]; }
  enum { maxOutOfReachTimeout = 2 };
  /// Back off ramp (after failed reach) duration, in seconds (standard character).
  float getRampDownFailedDuration(unsigned int index) const { NMP_ASSERT(index < 2); return RampDownFailedDuration[index]; }
  enum { maxRampDownFailedDuration = 2 };

  /// Give up after this time within reach, in seconds (standard character)..
  float getWithinReachTimeout(unsigned int index) const { NMP_ASSERT(index < 2); return WithinReachTimeout[index]; }
  enum { maxWithinReachTimeout = 2 };
  /// Back off ramp (after successful reach) duration, in seconds (standard character)..
  float getRampDownCompletedDuration(unsigned int index) const { NMP_ASSERT(index < 2); return RampDownCompletedDuration[index]; }
  enum { maxRampDownCompletedDuration = 2 };

  /// Set the target position an orientation for each arm.

  /// On target when within this distance, in metres (standard character).
  float getOutOfReachDistance(unsigned int index) const { NMP_ASSERT(index < 2); return OutOfReachDistance[index]; }
  enum { maxOutOfReachDistance = 2 };

  /// Defines how or whether arm swivel should be applied to the reach: "None" means swivel is not required; "Automatic" means swivel will be determined procedurally by the behaviour based on the reach target, "Specified" means the behaviour will use the fixed swivel amount specified below.
  int32_t getSwivelMode(unsigned int index) const { NMP_ASSERT(index < 2); return SwivelMode[index]; }
  enum { maxSwivelMode = 2 };
  /// Swivel value to use when Swivel Mode is "Specified". Value in range [-1,1],  positive values drive elbows out and up, negative drive elbows in and down.
  float getSwivelAmount(unsigned int index) const { NMP_ASSERT(index < 2); return SwivelAmount[index]; }
  enum { maxSwivelAmount = 2 };
  /// Limit the maximum extension of the arm. Expressed as a proportion of the total arm length.
  float getMaxArmExtensionScale(unsigned int index) const { NMP_ASSERT(index < 2); return MaxArmExtensionScale[index]; }
  enum { maxMaxArmExtensionScale = 2 };
  /// Small offset, to drive hand against surface, in metres (standard character).
  float getSurfacePenetration(unsigned int index) const { NMP_ASSERT(index < 2); return SurfacePenetration[index]; }
  enum { maxSurfacePenetration = 2 };

  /// Configure he behaviour to avoid positioning the hands within a specified distance from the spine.

  /// Scales the self avoidance radius set on the spine limb.
  float getTorsoAvoidanceRadiusMultiplier(unsigned int index) const { NMP_ASSERT(index < 2); return TorsoAvoidanceRadiusMultiplier[index]; }
  enum { maxTorsoAvoidanceRadiusMultiplier = 2 };
  /// Collision group used to disable hand-hand collision when reaching. This is the collision group within the collision groups editor.
  int32_t getCollisionGroupIndex() const { return CollisionGroupIndex; }


  /// Sets a suitable pose for an arm that has become a reach target for another arm. Typically this pose will place the target arm in a position where it can be reached by another arm.


  /// Sets a suitable pose for an arm that has become a reach target for another arm. Typically this pose will place the target arm in a position where it can be reached by another arm.
  const BodyPoseData& getArmReachForBodyPose() const { return pd_ArmReachForBodyPose; }

  // control parameters
  /// Indicates that a new target has been supplied. Should be set to true on the frame in which the target is changed.
  bool getNewHitCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.newHit[index]; }
  enum { maxNewHitCP = 2 };
  /// Sets the emphasis placed on the reach behaviour for this limb. A reach toward a target can be effectively faded out by ramping this value from 1 to 0.
  float getStrengthCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.Strength[index]; }
  enum { maxStrengthCP = 2 };
  /// Sets the maximum speed of the hand when moving towards the target. In m/s (standard character).
  float getEffectorSpeedLimitCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.EffectorSpeedLimit[index]; }
  enum { maxEffectorSpeedLimitCP = 2 };
  /// Set the target position an orientation for each arm.

  /// Index of the limb to reach for
  int getLimbIndexCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.LimbIndex[index]; }
  enum { maxLimbIndexCP = 2 };
  /// Index of the part to reach for, e.g. the upper arm, or lower leg.
  int getPartIndexCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.PartIndex[index]; }
  enum { maxPartIndexCP = 2 };
  /// Local surface point to reach to on the specified limb part (in frame of a physics joint).
  const NMP::Vector3& getPositionCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.Position[index]; }
  enum { maxPositionCP = 2 };
  /// Local surface normal of point to be reached to (in frame of a physics joint).
  const NMP::Vector3& getNormalCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.Normal[index]; }
  enum { maxNormalCP = 2 };

  // output control parameters
};


class ReachForBodyBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "ReachForBody"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 23; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  class ReachForBodyBehaviourDef* m_definition;

  enum AnimInputIDs
  {
    ArmReachForBodyPose_ID = 0,
  };

  enum ControlParamIDs
  {
    newHit_0_CP_ID = 0,
    newHit_1_CP_ID = 1,
    Strength_0_CP_ID = 2,
    Strength_1_CP_ID = 3,
    EffectorSpeedLimit_0_CP_ID = 4,
    EffectorSpeedLimit_1_CP_ID = 5,
    LimbIndex_0_CP_ID = 6,
    LimbIndex_1_CP_ID = 7,
    PartIndex_0_CP_ID = 8,
    PartIndex_1_CP_ID = 9,
    Position_0_CP_ID = 10,
    Position_1_CP_ID = 11,
    Normal_0_CP_ID = 12,
    Normal_1_CP_ID = 13,
  };

  const ReachForBodyBehaviourData& getParams() const { return m_params; }
  ReachForBodyBehaviourData& getParams() { return m_params; }
  ReachForBodyPoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  ReachForBodyBehaviourData m_params;
  ReachForBodyPoseRequirements m_poseRequirements;
};

class ReachForBodyBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(ReachForBodyBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline ReachForBodyBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(ReachForBodyBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) ReachForBodyBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_REACHFORBODY_H

