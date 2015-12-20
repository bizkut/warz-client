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
#ifndef MR_PHYSICS_RIG_DEF_H
#define MR_PHYSICS_RIG_DEF_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "morpheme/mrRig.h"
#include "morpheme/mrManager.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
  struct PhysicsJointDef;


//----------------------------------------------------------------------------------------------------------------------
/// \class MR::PhysicsDriverData
/// \brief Describes the physics driver specific data of a physics object, all physics driver specific
///   data classes inherit from this. This class allows physics rig def classes to have a typed pointer
///   instead of a void pointer.
//----------------------------------------------------------------------------------------------------------------------
struct PhysicsDriverData
{
};

//----------------------------------------------------------------------------------------------------------------------
class PhysicsSixDOFJointModifiableLimits
{
public:
  PhysicsSixDOFJointModifiableLimits();
  float getSwing1Limit() const { return m_swing1Limit; }
  float getSwing2Limit() const { return m_swing2Limit; }
  float getTwistLimitLow() const { return m_twistLimitLow; }
  float getTwistLimitHigh() const { return m_twistLimitHigh; }

  void setSwingLimit(float swing1Limit, float swing2Limit)
  {
    m_swing1Limit = swing1Limit;
    m_swing2Limit = swing2Limit;
  }

  void setTwistLimit(float twistLimitLow, float twistLimitHigh)
  {
    NMP_ASSERT(twistLimitLow <= twistLimitHigh);
    m_twistLimitLow  = twistLimitLow;
    m_twistLimitHigh = twistLimitHigh;
  }

  /// \brief Modifies q, the rotation from parent to child joint body according to the limits in desc.
  ///   The limits are multiplied by limitFrac to allow conservative limiting. If origQ is passed
  ///   in then the clamping is done to the ellipse that is expanded if the old orientation is
  ///   outside the limit, so we can prevent rotations going even further outside the limits,
  ///   without snapping them back.
  void clampToLimits(
    NMP::Quat& q,
    float limitFrac,
    const PhysicsJointDef& jointDef,
    const NMP::Quat& origQ) const;

  /// \brief Modifies q, the rotation from parent to child joint body according to the limits in desc.
  ///   The limits are multiplied by limitFrac to allow conservative limiting. 
  void clampToLimits(
    NMP::Quat& q,
    float limitFrac,
    const PhysicsJointDef& jointDef) const;

  /// \brief Expand joint limits to accommodate the supplied transform.
  void expand(const NMP::Quat& orientation, const PhysicsJointDef& jointDef);

  /// \brief Scale joint limits by the specified amount.
  void scale(float scaleFactor);

  void endianSwap();

protected:
  float m_swing1Limit;    ///< The angular limit of the swing1 axis.
  float m_swing2Limit;    ///< The angular limit of the swing2 axis.
  float m_twistLimitLow;  ///< The lower angular limit of the twist axis.
  float m_twistLimitHigh; ///< The higher angular limit of the twist axis.
};

//----------------------------------------------------------------------------------------------------------------------
class PhysicsSixDOFJointLimits
{
public:

  PhysicsSixDOFJointLimits();

  float getSwing1Limit() const { return m_modifiableLimits.getSwing1Limit(); }
  float getSwing2Limit() const { return m_modifiableLimits.getSwing2Limit(); }
  float getTwistLimitLow() const { return m_modifiableLimits.getTwistLimitLow(); }
  float getTwistLimitHigh() const { return m_modifiableLimits.getTwistLimitHigh(); }

  float getSwingLimitContactDistance() const { return m_swingLimitContactDistance; }
  float getTwistLimitContactDistance() const { return m_twistLimitContactDistance; }

  void setSwingLimit(float swing1Limit, float swing2Limit, float contactDistance)
  {
    m_modifiableLimits.setSwingLimit(swing1Limit, swing2Limit);
    m_swingLimitContactDistance = NMP::minimum(contactDistance, NMP::minimum(swing1Limit, swing2Limit));
  }

  void setTwistLimit(float twistLimitLow, float twistLimitHigh, float contactDistance)
  {
    m_modifiableLimits.setTwistLimit(twistLimitLow, twistLimitHigh);
    m_twistLimitContactDistance = NMP::minimum(contactDistance, (twistLimitHigh - twistLimitLow) * 0.5f);
  }

  void endianSwap();

  const PhysicsSixDOFJointModifiableLimits& getModifiableLimits() const {return m_modifiableLimits;}

protected:
  PhysicsSixDOFJointModifiableLimits m_modifiableLimits;

  float m_swingLimitContactDistance; ///< The padding around the hard swing limit
  float m_twistLimitContactDistance; ///< The padding around the hard twist limit
};


class PhysicsRagdollJointLimits
{
public:

  float getConeAngle() const      { return m_coneAngle; }
  float getPlaneMinAngle() const  { return m_planeMinAngle; }
  float getPlaneMaxAngle() const  { return m_planeMaxAngle; }
  float getTwistMinAngle() const  { return m_twistMinAngle; }
  float getTwistMaxAngle() const  { return m_twistMaxAngle; }

  void setConeAngle( const float value )     { m_coneAngle = value; }
  void setPlaneMinAngle( const float value ) { m_planeMinAngle = value; }
  void setPlaneMaxAngle( const float value ) { m_planeMaxAngle = value; }
  void setTwistMinAngle( const float value ) { m_twistMinAngle = value; }
  void setTwistMaxAngle( const float value ) { m_twistMaxAngle = value; }

  /// \brief Clamp the given orientation to the given fraction of the limits of this joint. The orientation is assumed
  ///   to be given in parent body space and will be internally converted to joint space and back inside this function.
  /// \param orientation - the orientation to clamp in parent body space.
  /// \param limitFraction - the fraction of the limit values to clamp to, range [0-1].
  void clampToLimits(
    NMP::Quat&              orientation,
    float                   limitFraction,
    const PhysicsJointDef&  jointDef) const;

  /// \brief Expand joint limits to accommodate the supplied transform.
  void expand(const NMP::Quat& NMP_UNUSED(orientation), const PhysicsJointDef& NMP_UNUSED(jointDef));

  /// \brief Scale joint limits by the specified amount.
  void scale(float scaleFactor);
  
  void endianSwap();

private:

  float         m_coneAngle;      ///< The angle in radians of the allowed cone of motion.
  float         m_planeMinAngle;  ///< The angle in radians between the cone axis and the side of one of the plane cones.
  float         m_planeMaxAngle;  ///< The angle in radians between the cone axis and the side of one of the plane cones.
  float         m_twistMinAngle;  ///< The minimum angle in radians of allowed twist.
  float         m_twistMaxAngle;  ///< The maximum angle in radians of allowed twist.
};


//----------------------------------------------------------------------------------------------------------------------
/// \class MR::PhysicsJointDef
/// \brief Describes the properties of a single joint between two bodies in the physics rig.
//----------------------------------------------------------------------------------------------------------------------
struct PhysicsJointDef
{
  enum JointTypes
  {
    JOINT_TYPE_SIX_DOF, ///< A twist swing limited joint type.
    JOINT_TYPE_RAGDOLL, ///< A plane cone limited joint type.
    JOINT_TYPE_FORCE_INT = 0xFFFFFFFF
  };

  NMP::Matrix34 m_parentPartFrame;  ///< The joint pose in the parent part's frame.
  NMP::Matrix34 m_childPartFrame;   ///< The joint pose in the child part's frame.
  NMP::Quat m_parentFrameQuat; ///< The joint pose rotation in the parent part's frame
  NMP::Quat m_childFrameQuat;  ///< The joint pose rotation in the child part's frame

  JointTypes    m_jointType;        ///< The type of the derived joint.
  int32_t       m_parentPartIndex;  ///< The index of parent part.
  int32_t       m_childPartIndex;   ///< The index of child part.
  char*         m_name;             ///< The name of the joint for debugging purposes.
#ifdef NM_HOST_64_BIT
  uint32_t pad1[2];                 /// This pad ensures that all 64-bit compilers generate the same member layout
#endif // NM_HOST_64_BIT
protected:
  void locate();
  void dislocate();
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::PhysicsSixDOFJointDef
/// \brief Describes a joint limit between two physics bodies with linear and rotational degrees of freedom. All
///   linear degrees of freedom are locked and the rotational degrees of freedom twist, swing1 and swing2 are either
///   limited to a given angle or completely free to rotate.
//----------------------------------------------------------------------------------------------------------------------
struct PhysicsSixDOFJointDef : public PhysicsJointDef
{
  enum Motion
  {
    MOTION_LIMITED = 0,     ///< Range of motion is limited to a specified range.
    MOTION_FREE,            ///< Range of motion is completely unlimited.
    MOTION_LOCKED,          ///< Range of motion is locked.
    NUM_MOTIONS,            ///< Number of motion types.
    MOTION_PAD = 0xffffffff
  };

  //--------------------------------------------------------------------------------------------------------------------
  /// \class MR::PhysicsSixDOFJointDef::SoftLimit
  /// \brief A soft joint limit that will gently drive the joints to within the allowed range instead of
  ///   completely stopping its motion as a normal joint limit would do.
  //--------------------------------------------------------------------------------------------------------------------
  struct SoftLimit
  {
    void locate();
    void dislocate();

    void endianSwap();

    NMP::Matrix34 m_parentPartFrame;  ///< The joint pose in the parent part's frame.
    NMP::Matrix34 m_childPartFrame;   ///< The joint pose in the child part's frame.

    Motion m_swing1Motion; ///< The allowed motion of the swing1 axis, see Motion for a list of motion types.
    Motion m_swing2Motion; ///< The allowed motion of the swing2 axis, see Motion for a list of motion types.
    Motion m_twistMotion;  ///< The allowed motion of the twist axis, see Motion for a list of motion types.

    float m_swing1Limit;    ///< The angular limit of the swing1 axis, only applies if swing1 motion type is MOTION_LIMITED.
    float m_swing2Limit;    ///< The angular limit of the swing2 axis, only applies if swing2 motion type is MOTION_LIMITED.
    float m_twistLimitLow;  ///< The lower angular limit of the twist axis, only applies if twist motion type is MOTION_LIMITED.
    float m_twistLimitHigh; ///< The higher angular limit of the twist axis, only applies if twist motion type is MOTION_LIMITED.

    float m_strengthScale;
  };

  void locate();
  void dislocate();

#if defined(MR_OUTPUT_DEBUGGING)
  /// \brief Serialize this joint limit data for debugging within Connect.
  uint32_t serializeTx(uint16_t nameToken, void* outputBuffer, uint32_t outputBufferSize) const ;
  /// \brief Serialize this joint limit's soft limit data for debugging within Connect.
  uint32_t serializeSoftLimitTx(uint16_t nameToken, void* outputBuffer, uint32_t outputBufferSize) const;
#endif

  const PhysicsSixDOFJointLimits& limits() const { return m_hardLimits; }

  /// \brief Is there a corresponding soft limit for this joint limit.
  bool hasSoftLimit() const;

  /// \brief Get the orientation of the soft limit in the parent body's space.
  NMP::Quat getSoftLimitParentLocalQuat() const;

  Motion m_swing1Motion; ///< The allowed motion of the swing1 axis, see Motion for a list of motion types.
  Motion m_swing2Motion; ///< The allowed motion of the swing2 axis, see Motion for a list of motion types.
  Motion m_twistMotion;  ///< The allowed motion of the twist axis, see Motion for a list of motion types.

  PhysicsDriverData* m_driverData;  ///< Contains physics driver specific limit data.
  SoftLimit* m_softLimit;           ///< Soft limit data for this joint limit, can be NULL if there is no corresponding soft limit.
  PhysicsSixDOFJointLimits m_hardLimits;

protected:
  /// \brief Calls NMP::endianSwap on all non-pointer members.
  void endianSwap();
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::PhysicsRagdollJointDef
/// \brief Describes a joint limit between two physics bodies with rotational degrees of freedom. The permitted range
///   of motion is defined by three cones, one oriented along the cone axis (the x-axis), the other two cones oriented
///   along the plane axis (the z-axis) facing in opposite directions. The cone axis cone is the permitted area of
///   motion for the child joint. Any region in which the other two cones intersect this permitted area is a limited
///   area of motion.
//----------------------------------------------------------------------------------------------------------------------
struct PhysicsRagdollJointDef : public PhysicsJointDef
{
  void locate();
  void dislocate();

  /// \brief Calls NMP::endianSwap on all non-pointer members.
  void endianSwap();

#if defined(MR_OUTPUT_DEBUGGING)
  /// \brief Serialize this joint limit data for debugging within Connect.
  uint32_t serializeTx(uint16_t nameToken, void* outputBuffer, uint32_t outputBufferSize) const ;
#endif

  PhysicsRagdollJointLimits m_limits;     ///< describes hard limits.
  PhysicsDriverData*        m_driverData; ///< Contains physics driver specific limit data.
};


//----------------------------------------------------------------------------------------------------------------------
/// \class MR::PhysicsRigDef
/// \brief The definition of a character's physical body.  The PhysicsRigDef is an authored asset, which is used to
//   create instances of the MR::PhysicsRig class when a morpheme character requires physical interaction with the
//   scene.  This class is currently agnostic of the physics engine used.
//----------------------------------------------------------------------------------------------------------------------
class PhysicsRigDef
{
protected:
  PhysicsRigDef() {}

public:

  //----------------------------------------------------------------------------------------------------------------------
  /// \class MR::PhysicsRigDef::Part
  /// \brief Describes the properties of a single articulated body in the physical rig.
  //----------------------------------------------------------------------------------------------------------------------
  struct Part
  {
    //----------------------------------------------------------------------------------------------------------------------
    /// \class MR::PhysicsRigDef::Volume
    /// \brief Describes the physical volume of this part.
    //----------------------------------------------------------------------------------------------------------------------
    struct Volume
    {
      struct Shape
      {
        NMP::Matrix34 localPose;
        int32_t parentIndex; ///< The index of this shape in the parent volume.
        float density; ///< if density is greater than 0.0 it is used instead of the mass.
        uint32_t materialID;
        PhysicsDriverData* driverData;
#ifdef NM_HOST_64_BIT
        uint32_t pad1[2]; /// This pad ensures that all 64-bit compilers generate the same member layout
#endif // NM_HOST_64_BIT
      };

      struct Sphere : public Shape
      {
        Sphere(float sphereRadius = 0.0f) : radius(sphereRadius) {}
        float radius;
        float pad1[3];
      };

      struct Box : public Shape
      {
        Box(const NMP::Vector3& boxDimensions = NMP::Vector3::InitZero) : dimensions(boxDimensions) {}
        NMP::Vector3 dimensions;
      };

      struct Capsule : public Shape
      {
        Capsule(float capsuleRadius = 0.0f, float capsuleHeight = 0.0f) : radius(capsuleRadius), height(capsuleHeight) {}
        float radius, height;
        float pad1[2];
      };

      Sphere*  spheres;
      Box*     boxes;
      Capsule* capsules;
      int32_t  numSpheres;
      int32_t  numBoxes;
      int32_t  numCapsules;
      char*    name;
    };

    struct Material
    {
      float               friction;
      float               restitution;
      PhysicsDriverData*  driverData;
    };

    struct Actor
    {
      NMP::Matrix34 globalPose;
      uint32_t      hasCollision;
      uint32_t      isFixed;
      PhysicsDriverData*  driverData;
    };

    //----------------------------------------------------------------------------------------------------------------------
    /// \class MR::PhysicsRigDef::Body
    /// \brief Describes the dynamics properties of this part.
    //----------------------------------------------------------------------------------------------------------------------
    struct Body
    {
      float useCustomCoM;
      float         angularDamping;
      float         linearDamping;
      PhysicsDriverData*  driverData;
    };

    Actor    actor;
    Body     body;
    Volume   volume;
    char*    name;

#if defined(MR_OUTPUT_DEBUGGING)
    uint32_t serializeTx(int32_t parentIndex, uint16_t nameToken, void* outputBuffer, uint32_t outputBufferSize) const;
#endif
  };

  struct CollisionGroup
  {
    int32_t* indices;
    int32_t numIndices;
    bool    enabled;
  };

  struct RootPart
  {
    NMP::Matrix34 transform;
    uint32_t index;
  };

  /// \note Make sure that any driver specific data is located after calling this function.
  bool locate();
  /// \note Make sure that any driver specific data is dislocated before calling this function.
  bool dislocate();

  uint32_t getNumParts() const;
  uint32_t getNumJoints() const;
  uint32_t getNumMaterials() const;
  const char* getPartName(int32_t iPart) const;

  /// Gets the bone index from a name - returns -1 if not found
  int32_t getPartIndexFromName(const char* name, bool matchPartial) const;

  /// Gets the index of the bone that is the parent of childIndex. Returns -1 if not found
  int32_t getParentPart(int32_t childIndex) const;

  /// Gets the TM of the part relative to the root part when the rig is in the zero pose (joints in
  /// middle of limits).
  NMP::Matrix34 getZeroPoseTMRelRoot(uint32_t iPart) const;

  /// Gets the material of a given material ID
  PhysicsRigDef::Part::Material getMaterialFromMaterialID(uint32_t id) const;

  int32_t calculateNumJointsInChain(int32_t endPartId, int32_t rootPartId) const;

#if defined(MR_OUTPUT_DEBUGGING)
  uint32_t serializeTx(AnimSetIndex animSetIndex, void* outputBuffer, uint32_t outputBufferSize) const;
#endif // MR_OUTPUT_DEBUGGING

  RootPart m_rootPart;

  int32_t m_numCollisionGroups;
  int32_t m_numTrajectoryParts;

  uint32_t m_numMaterials;
  Part::Material* m_materials;

  CollisionGroup* m_collisionGroups;
  uint32_t*       m_trajectoryParts;

  int32_t m_numParts;
  int32_t m_numJoints;

  Part*             m_parts;
  PhysicsJointDef** m_joints;
};

//----------------------------------------------------------------------------------------------------------------------
// Inline implementations
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline uint32_t PhysicsRigDef::getNumParts() const
{
  return m_numParts;
}

//----------------------------------------------------------------------------------------------------------------------
inline uint32_t PhysicsRigDef::getNumJoints() const
{
  return m_numJoints;
}

//----------------------------------------------------------------------------------------------------------------------
inline uint32_t PhysicsRigDef::getNumMaterials() const
{
  return m_numMaterials;
}

//----------------------------------------------------------------------------------------------------------------------
inline const char* PhysicsRigDef::getPartName(int32_t iPart) const
{
  if (iPart >= 0 && iPart < m_numParts)
    return m_parts[iPart].name;
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
inline PhysicsRigDef::Part::Material PhysicsRigDef::getMaterialFromMaterialID(uint32_t id) const
{
  return m_materials[id];
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_PHYSICS_RIG_DEF_H
//----------------------------------------------------------------------------------------------------------------------
