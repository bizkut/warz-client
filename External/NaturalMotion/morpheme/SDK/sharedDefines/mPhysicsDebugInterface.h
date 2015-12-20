// Copyright (c) 2009 NaturalMotion.  All Rights Reserved.
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
# pragma once
#endif
#ifndef M_PHYSICS_DEBUG_INTERFACE_H
#define M_PHYSICS_DEBUG_INTERFACE_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMColour.h"
#include "NMPlatform/NMVector3.h"
#include "mCoreDebugInterface.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \file mPhysicsDebugInterface.h
/// \brief 
/// \ingroup PhysicsModule
//----------------------------------------------------------------------------------------------------------------------

#define MR_INVALID_PHYSICS_OBJECT_ID 0xFFFFFFFF

namespace MR
{

// ensure all these structs are the same layout for all compilers
#pragma pack(push, 4)

typedef uint32_t PhysicsObjectID;

//----------------------------------------------------------------------------------------------------------------------
struct PhysicsRigDefPersistentData
{
  AnimSetIndex  m_animSetIndex;
  int32_t       m_numParts;
  int32_t       m_numJoints;

  /// \brief Endian swaps in place on buffer.
  static inline PhysicsRigDefPersistentData *endianSwap(void *buffer);
  /// \brief Deserialises in place on buffer memory.
  static inline PhysicsRigDefPersistentData *deserialiseTx(void *buffer);
};

//----------------------------------------------------------------------------------------------------------------------
struct PhysicsRigPersistentData
{
  AnimSetIndex  m_animSetIndex;
  int32_t       m_numParts;
  int32_t       m_numJoints;

  /// \brief Endian swaps in place on buffer.
  static PhysicsRigPersistentData *endianSwap(void *buffer);
  /// \brief Deserialises in place on buffer memory.
  static PhysicsRigPersistentData *deserialiseTx(void *buffer);
};

//----------------------------------------------------------------------------------------------------------------------
///
//----------------------------------------------------------------------------------------------------------------------
struct PhysicsRigFrameData
{
  AnimSetIndex  m_animSetIndex;
  int32_t       m_refCount;

  /// \brief Endian swaps in place on buffer.
  static PhysicsRigFrameData *endianSwap(void *buffer);
  /// \brief Deserialises in place on buffer memory.
  static PhysicsRigFrameData *deserialiseTx(void *buffer);
};

//----------------------------------------------------------------------------------------------------------------------
enum PhysicsObjectDataTypes
{
  kPhysicsPartDataType,
  kPhysicsJointDataType,
  kPhysicsSoftLimitDataType,
  kPhysicsPartOverrideColourDataType,
  kLastPhysicsObjectDataType,
  kPhysicsPartTagDataType,
  kPhysicsShapeTagDataType,
};

//----------------------------------------------------------------------------------------------------------------------
struct PhysicsShapePersistentData
{
  NMP::Matrix34 m_localPose;
  int32_t m_parentIndex; ///< The index of this shape in the parent volume.
  uint32_t m_pad[3];

  // additional per shape material info should go here, if it does make sure the padding of all derived classes is fixed up.

  /// \brief Endian swaps in place on buffer.
  static PhysicsShapePersistentData *endianSwap(void *buffer);
  /// \brief Deserialises in place on buffer memory.
  static PhysicsShapePersistentData *deserialiseTx(void *buffer);
};

//----------------------------------------------------------------------------------------------------------------------
struct PhysicsCapsulePersistentData : public PhysicsShapePersistentData
{
  float m_radius;
  float m_height;
  // NMP::Matrix34 is the first member of PhysicsShapeStaticOutputData and is aligned so PhysicsCapsuleStaticOutputData
  // must be padded to allow arrays of shapes.
  float m_pad[2];

  /// \brief Endian swaps in place on buffer.
  static PhysicsCapsulePersistentData *endianSwap(void *buffer);
  /// \brief Deserialises in place on buffer memory.
  static PhysicsCapsulePersistentData *deserialiseTx(void *buffer);
};

//----------------------------------------------------------------------------------------------------------------------
struct PhysicsBoxPersistentData : public PhysicsShapePersistentData
{
  float m_width;
  float m_height;
  float m_depth;
  uint32_t m_pad[1];

  /// \brief Endian swaps in place on buffer.
  static PhysicsBoxPersistentData *endianSwap(void *buffer);
  /// \brief Deserialises in place on buffer memory.
  static PhysicsBoxPersistentData *deserialiseTx(void *buffer);
};

//----------------------------------------------------------------------------------------------------------------------
struct PhysicsSpherePersistentData : public PhysicsShapePersistentData
{
  float m_radius;
  // NMP::Matrix34 is the first member of PhysicsShapeStaticOutputData and is aligned so PhysicsSphereStaticOutputData
  // must be padded to allow arrays of shapes.
  float m_pad[3];

  /// \brief Endian swaps in place on buffer.
  static PhysicsSpherePersistentData *endianSwap(void *buffer);
  /// \brief Deserialises in place on buffer memory.
  static PhysicsSpherePersistentData *deserialiseTx(void *buffer);
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief
//----------------------------------------------------------------------------------------------------------------------
struct PhysicsPartPersistentData
{
  int32_t m_parentIndex;
  MR::PhysicsObjectID m_physicsObjectID;
  uint32_t m_numBoxes;
  uint32_t m_numCapsules;
  uint32_t m_numSpheres;
  StringToken m_nameToken;
  uint8_t  m_pad[10];

  /// \brief Endian swaps in place on buffer.
  static PhysicsPartPersistentData *endianSwap(void *buffer);
  /// \brief Deserialises in place on buffer memory.
  static PhysicsPartPersistentData *deserialiseTx(void *buffer);

  inline PhysicsBoxPersistentData *getBox(uint32_t index) const;
  inline PhysicsCapsulePersistentData *getCapsule(uint32_t index) const;
  inline PhysicsSpherePersistentData *getSphere(uint32_t index) const;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief
//----------------------------------------------------------------------------------------------------------------------
struct PhysicsPartFrameData
{
  NMP::Matrix34 m_globalPose;

  /// \brief Endian swaps in place on buffer.
  static PhysicsPartFrameData *endianSwap(void *buffer);
  /// \brief Deserialises in place on buffer memory.
  static PhysicsPartFrameData *deserialiseTx(void *buffer);
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Describes base joint persistent data to morpheme:connect.
//----------------------------------------------------------------------------------------------------------------------
struct PhysicsJointPersistentData
{
  enum JointTypes
  {
    JOINT_TYPE_SIX_DOF,
    JOINT_TYPE_RAGDOLL,
  };

  // in this order for alignment and size reasons, aligned members first and ordered largest to smallest.
  NMP::Matrix34 m_parentLocalFrame;
  NMP::Matrix34 m_childLocalFrame;
  int32_t       m_parentPartIndex;
  int32_t       m_childPartIndex;
  JointTypes    m_jointType;
  StringToken   m_nameToken;
  uint8_t       m_pad[2];

protected:
  /// \brief Endian swaps in place on buffer.
  static PhysicsJointPersistentData* endianSwap(PhysicsJointPersistentData* persistentData);
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Describes six dof joint persistent data to morpheme:connect.
//----------------------------------------------------------------------------------------------------------------------
struct PhysicsSixDOFJointPersistentData : public PhysicsJointPersistentData
{
  // in this order for alignment and size reasons, aligned members first and ordered largest to smallest.
  float         m_swing1Limit;
  float         m_swing2Limit;
  float         m_twistLimitLow;
  float         m_twistLimitHigh;

  /// \brief Endian swaps in place on buffer.
  static PhysicsSixDOFJointPersistentData *endianSwap(void *buffer);
  /// \brief Deserialises in place on buffer memory.
  static PhysicsSixDOFJointPersistentData *deserialiseTx(void *buffer);
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Describes rag doll joint persistent data to morpheme:connect.
//----------------------------------------------------------------------------------------------------------------------
struct PhysicsRagdollJointPersistentData : public PhysicsJointPersistentData
{
  // in this order for alignment and size reasons, aligned members first and ordered largest to smallest.
  float         m_coneAngle;
  float         m_planeMinAngle;
  float         m_planeMaxAngle;
  float         m_twistMinAngle;
  float         m_twistMaxAngle;

  /// \brief Endian swaps in place on buffer.
  static PhysicsRagdollJointPersistentData *endianSwap(void *buffer);
  /// \brief Deserialises in place on buffer memory.
  static PhysicsRagdollJointPersistentData *deserialiseTx(void *buffer);
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Describes base joint frame buffer data to morpheme:connect.
//----------------------------------------------------------------------------------------------------------------------
struct PhysicsJointFrameData
{
  enum JointTypes
  {
    JOINT_TYPE_SIX_DOF,
    JOINT_TYPE_RAGDOLL,
  };

  JointTypes getJointType() const { return m_jointType; }

  // in this order for alignment and size reasons, aligned members first and ordered largest to smallest.
  JointTypes    m_jointType; // 2 bytes ?
  
  uint8_t       m_pad[14];

protected:
  /// \brief Endian swaps in place on buffer.
  static PhysicsJointFrameData* endianSwap(PhysicsJointFrameData* persistentData);
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Describes six dof joint frame data to morpheme:connect.
//----------------------------------------------------------------------------------------------------------------------
struct PhysicsSixDOFJointFrameData : public PhysicsJointFrameData
{
  // in this order for alignment and size reasons, aligned members first and ordered largest to smallest.
  float m_swing1Limit;
  float m_swing2Limit;
  float m_twistLimitLow;
  float m_twistLimitHigh;

  float getSwing1Limit() const    { return m_swing1Limit; }
  float getSwing2Limit() const    { return m_swing2Limit; }
  float getTwistLimitLow() const  { return m_twistLimitLow; }
  float getTwistLimitHigh() const { return m_twistLimitHigh; }

  /// \brief Endian swaps in place on buffer.
  static PhysicsSixDOFJointFrameData* endianSwap(void *buffer);
  /// \brief Deserialises in place on buffer memory.
  static PhysicsSixDOFJointFrameData* deserialiseTx(void *buffer);
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Describes rag doll joint frame data to morpheme:connect.
//----------------------------------------------------------------------------------------------------------------------
struct PhysicsRagdollJointFrameData : public PhysicsJointFrameData
{
  // in this order for alignment and size reasons, aligned members first and ordered largest to smallest.
  float m_coneAngle;
  float m_planeMinAngle;
  float m_planeMaxAngle;
  float m_twistMinAngle;
  float m_twistMaxAngle;

  float getConeAngle() const      {return m_coneAngle;}
  float getPlaneMinAngle() const  {return m_planeMinAngle;}
  float getPlaneMaxAngle() const  {return m_planeMaxAngle;}
  float getTwistAngle() const     {return m_twistMaxAngle - m_twistMinAngle;}
  float getTwistOffset() const    {return 0.0f;}

  /// \brief Endian swaps in place on buffer.
  static PhysicsRagdollJointFrameData* endianSwap(void *buffer);
  /// \brief Deserialises in place on buffer memory.
  static PhysicsRagdollJointFrameData* deserialiseTx(void *buffer);
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Describes six dof joint persistent data to morpheme:connect.
//----------------------------------------------------------------------------------------------------------------------
struct PhysicsSoftLimitPersistentData
{
  // in this order for alignment and size reasons, aligned members first and ordered largest to smallest.
  NMP::Matrix34 m_softLimitFrame1Local;
  NMP::Matrix34 m_softLimitFrame2Local;
  int32_t       m_parentPartIndex;
  int32_t       m_childPartIndex;
  uint32_t      m_softSwing1Enabled;
  uint32_t      m_softSwing2Enabled;
  uint32_t      m_softTwistEnabled;
  float         m_swing1SoftLimit;
  float         m_swing2SoftLimit;
  float         m_twistSoftLimitLow;
  float         m_twistSoftLimitHigh;
  float         m_softLimitStrength;

  /// \brief Endian swaps in place on buffer.
  static PhysicsSoftLimitPersistentData *endianSwap(void *buffer);
  /// \brief Deserialises in place on buffer memory.
  static PhysicsSoftLimitPersistentData *deserialiseTx(void *buffer);
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Allows the colour of physics part previewed in morpheme:connect to be overridden by the runtime.
//----------------------------------------------------------------------------------------------------------------------
struct PhysicsPartOverrideColour
{
  /// \brief 
  enum OverrideTypeIDs
  {
    kNoOverrideTypeID = 0,
    kCollisionSetOverrideTypeID,
    kNumOverrideTypeIDs,
  };

  typedef uint16_t OverrideTypeID;

  /// \brief The new colour to render the part.
  NMP::Colour m_overrideColour;
  /// \brief The id of the part to render in the new colour
  MR::PhysicsObjectID m_physicsObjectID;
  /// \brief The type of override colour ie. kCollisionSetOverrideTypeID, this allows multiple override
  /// types to be sent at the same time leaving the option to choose which is displayed in morpheme:connect.
  OverrideTypeID m_overrideTypeID;

  /// \brief Endian swaps in place on buffer.
  static PhysicsPartOverrideColour *endianSwap(void *buffer);
  /// \brief Deserialises in place on buffer memory.
  static PhysicsPartOverrideColour *deserialiseTx(void *buffer);
};

// restore the packing alignment set before this file
#pragma pack(pop)

} // namespace MR

#include "sharedDefines/mPhysicsDebugInterface.inl"

//----------------------------------------------------------------------------------------------------------------------
#endif // M_PHYSICS_DEBUG_INTERFACE_H
//----------------------------------------------------------------------------------------------------------------------
