// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef NM_LIMBIK_H
#define NM_LIMBIK_H

#include "NMPlatform/NMBitArray.h"
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMQuat.h"
#include "erDebugDraw.h"

// Forward declarations
namespace NMRU
{
  namespace HybridIK
  {
    struct Params;
  };
  namespace GeomUtils
  {
    struct PosQuat;
    class DebugDraw;
  };
  namespace JointLimits
  {
    struct Params;
  };
};

namespace MR
{
  struct PhysicsSerialisationBuffer;
  class PhysicsRigDef;
}

namespace ER
{
class DimensionalScaling;
class LimbDef;

//----------------------------------------------------------------------------------------------------------------------
/// Iterative IK solver.
/// Minimal interface to support position, orientation, normal and guide pose constraints in any weighted combination.
/// Depends only on NMP and debugDraw, so re-usable.
//----------------------------------------------------------------------------------------------------------------------
class LimbIK 
{
  static const int s_maxNumJoints = 10;
public:
  //--------------------------------------------------------------------------------------------------------------------
  /// Clean extendable interface for debug flags
  //--------------------------------------------------------------------------------------------------------------------
  enum DebugDrawId
  {
    kDrawLimits,
    kDrawTarget,
    kDrawFinalResult,
    kDrawInputPose,
    kDrawEachIteration,
    kDrawJointMatrices,
    kDrawBodyMatrices,
    kDrawGuidePose,
    kDrawRelativeToRoot,
    kNumDebugDrawFlags,
  };
  //--------------------------------------------------------------------------------------------------------------------
  /// Clean extendable interface for feature flags
  //--------------------------------------------------------------------------------------------------------------------
  enum FeatureId
  {
    kFeatureUseJointLimits,
    kNumFeatureFlags,
  };
  //--------------------------------------------------------------------------------------------------------------------
  // Local type definitions
  //--------------------------------------------------------------------------------------------------------------------
  struct OperatingParams
  {
    OperatingParams();

    float m_positionWeight;
    float m_orientationWeight;
    float m_normalWeight;

    float m_positionWeights[s_maxNumJoints];
    float m_orientationWeights[s_maxNumJoints];
    float m_guidePoseWeights[s_maxNumJoints];
    float m_neutralPoseWeight;

    bool m_useGuidePose;
    bool  m_limitsEnabled;

#if defined(MR_OUTPUT_DEBUGGING)
    /// \brief Identifies which step of the IK to debug draw as well as the initial state.
    /// \brief If -1 draw all
    int32_t m_NMIKDebugDrawStep;

    /// \brief Run an additional solve just to produce debug output, but discard result.
    bool m_NMIKTestSolve;

    /// \name The target to solve for in the test solve.
    /// \{
    float m_NMIKTestTargetTranslationX;
    float m_NMIKTestTargetTranslationY;
    float m_NMIKTestTargetTranslationZ;
    float m_NMIKTestTargetRotationX;
    float m_NMIKTestTargetRotationY;
    float m_NMIKTestTargetRotationZ;
    /// \}

    /// \brief When doing the test solve, ignore writing to the IK geometry to enforce a continuous solve.
    /// Typically you want to do this so that the test solve represents the converged result after multiple updates,
    /// but if you switch off you will see what the real system will do on the very next update.
    bool m_NMIKTestSolveForceContinuous;
#endif
    int m_maxIterations;
  };

  //--------------------------------------------------------------------------------------------------------------------
  /// Class for caching LimbIK solver error data
  //--------------------------------------------------------------------------------------------------------------------
  class SolverErrorData
  {
  public:

    /// Initialisation.
    void zeroData()
    {
      m_positionError = m_normalError = m_orientationError = 0.0f;
      m_positionErrorWeight = m_normalErrorWeight = m_orientationErrorWeight = 0.0f;
    }

    // Errors on completion of last solve
    float m_positionError;
    float m_normalError;
    float m_orientationError;

    // Set from limb ctrl (set by behaviours)
    float m_positionErrorWeight;
    float m_normalErrorWeight;
    float m_orientationErrorWeight;
  }; // class SolverErrorData

  //--------------------------------------------------------------------------------------------------------------------
  /// Internal representation of a "joint"
  //--------------------------------------------------------------------------------------------------------------------
  struct JointDefinition
  {
    NMP::Matrix34 parentBodyToJoint;
    NMP::Matrix34 childBodyToJointInv;
  };

#if defined(MR_OUTPUT_DEBUGGING)
  //----------------------------------------------------------------------------------------------------------------------
  // Minimal amount of info required to draw a joint.
  //----------------------------------------------------------------------------------------------------------------------
  struct JointDrawData
  {
    NMP::Matrix34 jTMw;             // TM of fixed joint frame: "j" in world coords
    NMP::Matrix34 j_TMw;            // TM of moving joint frame: "j_" in world coords
  };

  //----------------------------------------------------------------------------------------------------------------------
  // Minimal amount of info required to draw a limb
  //----------------------------------------------------------------------------------------------------------------------
  struct LimbDrawData
  {
    NMP::Matrix34 root;
    NMP::Matrix34 end;
    JointDrawData* jdd;
  };
#endif

public:

  // APIs
  //---------------------------------------------------------------------------------
  void init(
    const MR::PhysicsRigDef* physicsRigDef,
    const ER::LimbDef* limbDef);
  void resetInternalState(); ///< Setting the old IK position to the zero angle state etc.
  void deinit();

  void setRootAndEndOffsets(const NMP::Matrix34& rootOffset, const NMP::Matrix34& endOffset);
  void setSingleJointLimitScale(int index, float scale);
  void setSingleJointLimits(const int index, const NMP::Vector3& limits);
  void setRotationWeightDropOff(float dropOff);
  int getNumJointAngles() const { return m_numJoints; };

  void setInitialJointQuats(NMP::Quat* initialPose);
  void setGuidePose(const NMP::Quat* guidePose);
  void getGuidePose(NMP::Quat* guidePose);

  bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  bool restoreState(MR::PhysicsSerialisationBuffer& savedState);

  void getFinalQuat(int i, NMP::Quat& q) const;

  // globals

  void solve(
    const NMP::Quat& rootBodyQuat, const NMP::Vector3& rootBodyPos, const NMP::Quat& targetQuat,
    const NMP::Vector3& targetNormal, const NMP::Vector3& localNormal, const NMP::Vector3& targetPoint,
    LimbIK::SolverErrorData*    sed,
    const float                 timeDelta,
    MR::InstanceDebugInterface* pDebugDrawInst,
    const NMP::Matrix34&        rootBodyMatrixForDebug, 
    const DimensionalScaling&   dimensionalScaling);

  // Simpler access to BitArrays
#if defined(MR_OUTPUT_DEBUGGING)
  bool getDebugDrawFlag(DebugDrawId id) const { return m_debugDrawFlags->isBitSet(id) != 0; }
  void setDebugDrawFlag(DebugDrawId id, bool flag) { m_debugDrawFlags->setBit(id, flag); }
  bool getFeatureFlag(FeatureId id) const { return m_featureFlags->isBitSet(id) != 0; }
  void setFeatureFlag(FeatureId id, bool flag) { m_featureFlags->setBit(id, flag); }
#else
  bool getDebugDrawFlag(DebugDrawId NMP_UNUSED(id)) const { return false; }
  void setDebugDrawFlag(DebugDrawId NMP_UNUSED(id), bool NMP_UNUSED(flag)) {}
  bool getFeatureFlag(FeatureId NMP_UNUSED(id)) const {return true;}
  void setFeatureFlag(FeatureId NMP_UNUSED(id), bool NMP_UNUSED(flag)) {}
#endif

  NMP::Matrix34 calcEndEffectorTMFromRootAndJointQuats(
    const NMP::Quat& rootQuat,
    const NMP::Vector3& rootPos,
    const NMP::Quat* jointQuats) const;

public:
  NMP::Matrix34 m_lastSolutionEndEffectorTM;

  LimbIK::OperatingParams m_OP;
  JointDefinition* m_joint;
  NMP::Matrix34* m_bodyMatrix;        ///< see the results of where it expects to get to
  NMP::Quat* m_finalQuats;

protected:

  /// \brief Sets the parameters for NMIK solves, essentially by copying across the appropriate limbIK params
  void setNMIKParams();
  /// Updates only the parameters changing at runtime, i.e. under control of behaviour
  void updateIKTargetWeights();
  /// set the orientation constraint as a primary or secondary task depending of the IKTargetWeights.
  void setPriorityOrientationConstraint();
  uint32_t m_numIKJoints;                 /// Number of NMIK joints (may be more than m_numJoints)
  uint32_t* m_ikJointIndices;             /// The joint indices in order (in euphoria they just count up from 0)
  NMRU::HybridIK::Params* m_ikParams;     /// \brief NMIK HybridIK parameters
  NMRU::GeomUtils::PosQuat* m_ikGeometry; /// \brief The joint geometry as required by NMIK
  /// \brief NMIK needs a copy of initial limits so they can be modified if parameters change (such as limit scale)
  NMRU::JointLimits::Params* m_ikLimits;

#if defined(MR_OUTPUT_DEBUGGING)
  NMRU::GeomUtils::DebugDraw* m_debugLines;
  NMRU::GeomUtils::PosQuat* m_ikTestSolveGeometry;
  LimbDrawData m_debugDrawData;
#endif

private:
  NMP::Matrix34 m_rootOffset;
  NMP::Matrix34 m_endOffset;
  int m_numJoints;

#if defined(MR_OUTPUT_DEBUGGING)
  NMP::BitArray* m_debugDrawFlags;
  NMP::BitArray* m_featureFlags;

  // debug draw functions
public:

  MR::LimbIndex m_limbIndex;
  void setLimbIndex(MR::LimbIndex limbIndex);
  void drawLimb(
    MR::InstanceDebugInterface* pDebugDrawInst, const NMP::Quat* quats, const NMP::Matrix34& offsetForDebug, const NMP::Vector3& boneColour,
    bool drawLimits, bool drawJointFrames, const DimensionalScaling& dimensionalScaling) const;

private:

  void calcLimbDrawInfo(const NMP::Quat* quats, LimbDrawData& ldd) const; ///< aux for public API drawLimb
  void drawLimb(
    MR::InstanceDebugInterface* pDebugDrawInst, const LimbDrawData& ldd, const NMP::Matrix34& offsetForDebug, const NMP::Vector3& boneColour,
    bool drawLimits, bool drawJointFrames, const DimensionalScaling& dimensionalScaling) const;
  void drawLimit(MR::InstanceDebugInterface* pDebugDrawInst, int jointId, const JointDrawData& jdd, float size) const;
  void drawTwistSwingPointers(MR::InstanceDebugInterface* pDebugDrawInst, const NMP::Matrix34& frame1World, const NMP::Matrix34& frame2World, float size) const;
#endif // defined(MR_OUTPUT_DEBUGGING)

}; // class limbNMIK

} // namespace ER

#endif // NM_LIMBIK_H
