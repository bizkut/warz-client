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
#ifndef MCBODYEXPORT_H
#define MCBODYEXPORT_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMVector3.h"
#include "mcExport.h"

#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable:4481)
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace ME
{

//----------------------------------------------------------------------------------------------------------------------
class AttributeBlockExport;

//----------------------------------------------------------------------------------------------------------------------
/// HamstringExport
//----------------------------------------------------------------------------------------------------------------------
class HamstringExport
{
public:
  virtual ~HamstringExport() { }
  
  bool getEnabled() const;
  float getThreshold() const;
  float getStiffness() const;

  void setThreshold(float threshold);
  void setStiffness(float strength);
  void setEnabled(bool enabled);
  
  bool getTwistBendScaleWeights(float* twistBendScales, uint32_t arraySize) const;
  void setTwistBendScaleWeights(const float* twistBendScales, uint32_t arraySize);
  
  bool getSwing1BendScaleWeights(float* swing1BendScales, uint32_t arraySize) const;
  void setSwing1BendScaleWeights(const float* swing1BendScales, uint32_t arraySize);

  bool getSwing2BendScaleWeights(float* swing2BendScales, uint32_t arraySize) const;
  void setSwing2BendScaleWeights(const float* swing2BendScales, uint32_t arraySize);

  virtual const AttributeBlockExport* getAttributeBlock() const = 0;
  virtual AttributeBlockExport* getAttributeBlock() = 0;
};

//----------------------------------------------------------------------------------------------------------------------
/// LimbDefExport
//----------------------------------------------------------------------------------------------------------------------
class LimbDefExport
{
public:
  virtual ~LimbDefExport() { }

  virtual const char* getName() const = 0;

  void setEndPartName(const char* partName);
  void setRootPartName(const char* partName);
  void setBasePartName(const char* partName);
  void setMidPartName(const char* partName);

  const char* getEndPartName() const;
  const char* getRootPartName() const;
  const char* getBasePartName() const;
  const char* getMidPartName() const;

  const char* getType() const;
  void setType(const char* lType);

  NMP::Matrix34 getReachOffset() const;
  float getReachConeAngle() const;
  float getReachDistance() const;

  void setReachOffset(const NMP::Matrix34& endOffset);
  void setReachConeAngle(float reachAngle);
  void setReachDistance(float reachDistance);

  const NMP::Matrix34 getEndOffset() const;
  const NMP::Matrix34 getRootOffset() const;

  void setEndOffset(const NMP::Matrix34& endOffset);
  void setRootOffset(const NMP::Matrix34& rootOffset);

  virtual const HamstringExport* getHamstring() const = 0;
  virtual HamstringExport* getHamstring() = 0;

  uint32_t getNumExtraParts() const;
  uint32_t getExtraParts(const char* parts[], uint32_t numParts) const;

  float getGuidePoseWeight() const;
  float getNeutralPoseWeight() const;

  void setGuidePoseWeight(float guidePoseWeight);
  void setNeutralPoseWeight(float neutralPoseWeight);

  uint32_t getPositionWeightSize() const;
  uint32_t getOrientationWeightSize() const;

  bool getGuidePoseJoints(bool* guidePoseJoints, uint32_t arraySize) const;
  bool getPositionWeights(float* orientationWeight, uint32_t arraySize) const;
  bool getOrientationWeights(float* orientationWeight, uint32_t arraySize) const;

  void setGuidePoseJoints(const bool* guidePoseJoints, uint32_t weightCount);
  void setPositionWeights(const float* positionWeights, uint32_t weightCount);
  void setOrientationWeights(const float* orientationWeights, uint32_t weightCount);

  virtual const AttributeBlockExport*  getAttributeBlock() const = 0;
  virtual AttributeBlockExport*  getAttributeBlock() = 0;
};

//----------------------------------------------------------------------------------------------------------------------
/// SelfAvoidanceExport
//----------------------------------------------------------------------------------------------------------------------
class SelfAvoidanceExport
{
public:
  virtual ~SelfAvoidanceExport() { }

  virtual void setRadius(float r) = 0;
  virtual float getRadius() const = 0;
};

//----------------------------------------------------------------------------------------------------------------------
/// BodyDefExport
//----------------------------------------------------------------------------------------------------------------------
class BodyDefExport : public AssetExport
{
public:
  virtual void setDestFilename(const char* filename) = 0;

  virtual LimbDefExport* createLimb(const char* name) = 0;

  virtual uint32_t getNumLimbs() const = 0;
  virtual const LimbDefExport* getLimb(uint32_t index) const = 0;

  virtual uint32_t getNumArmLimbs() const;
  virtual uint32_t getNumHeadLimbs() const;
  virtual uint32_t getNumSpineLimbs() const;
  virtual uint32_t getNumLegLimbs() const;

  virtual const AttributeBlockExport*  getAttributeBlock() const = 0;
  virtual AttributeBlockExport*  getAttributeBlock() = 0;

  virtual GUID getRigGUID() const;
  virtual void setRigGUID(GUID guid);

  virtual GUID getPhysicsRigGUID() const;
  virtual void setPhysicsRigGUID(GUID guid);

  const char* getRootLimbName() const;
  void setRootLimbName(const char* rootLimbName);

  virtual SelfAvoidanceExport* createSelfAvoidanceExport() = 0;  
  virtual void getSelfAvoidance(float& radius) const = 0;

  virtual AnimationPoseDefExport* createAnimationPose(const char* name) = 0;
  virtual uint32_t getAnimationPoseCount() const = 0;
  virtual const AnimationPoseDefExport* getAnimationPose(uint32_t index) const = 0;
  virtual const AnimationPoseDefExport* getAnimationPose(const char* poseName) const = 0;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace ME

#ifdef _MSC_VER
  #pragma warning(pop)
#endif

#endif // MCBODYEXPORT_H
