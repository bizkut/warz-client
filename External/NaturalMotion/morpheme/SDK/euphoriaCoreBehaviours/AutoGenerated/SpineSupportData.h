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

#ifndef NM_MDF_SPINESUPPORT_DATA_H
#define NM_MDF_SPINESUPPORT_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/SpineSupport.module"

// external types
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMVector3.h"
#include "euphoria/erLimbTransforms.h"

// known types
#include "Types/RotationRequest.h"
#include "Types/TranslationRequest.h"

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

#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable: 4324) // structure was padded due to __declspec(align()) 
#endif // _MSC_VER

/** 
 * \class SpineSupportInputs
 * \ingroup SpineSupport
 * \brief Inputs keep the spine end supported above its root in a good posture
 *
 * Data packet definition (40 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SpineSupportInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SpineSupportInputs));
  }

  NM_INLINE void setDesiredEndRotationRelRoot(const NMP::Quat& _desiredEndRotationRelRoot, float desiredEndRotationRelRoot_importance = 1.f)
  {
    desiredEndRotationRelRoot = _desiredEndRotationRelRoot;
    postAssignDesiredEndRotationRelRoot(desiredEndRotationRelRoot);
    NMP_ASSERT(desiredEndRotationRelRoot_importance >= 0.0f && desiredEndRotationRelRoot_importance <= 1.0f);
    m_desiredEndRotationRelRootImportance = desiredEndRotationRelRoot_importance;
  }
  NM_INLINE float getDesiredEndRotationRelRootImportance() const { return m_desiredEndRotationRelRootImportance; }
  NM_INLINE const float& getDesiredEndRotationRelRootImportanceRef() const { return m_desiredEndRotationRelRootImportance; }
  NM_INLINE const NMP::Quat& getDesiredEndRotationRelRoot() const { return desiredEndRotationRelRoot; }

  NM_INLINE void setDesiredEndTranslationRelRoot(const NMP::Vector3& _desiredEndTranslationRelRoot, float desiredEndTranslationRelRoot_importance = 1.f)
  {
    desiredEndTranslationRelRoot = _desiredEndTranslationRelRoot;
    postAssignDesiredEndTranslationRelRoot(desiredEndTranslationRelRoot);
    NMP_ASSERT(desiredEndTranslationRelRoot_importance >= 0.0f && desiredEndTranslationRelRoot_importance <= 1.0f);
    m_desiredEndTranslationRelRootImportance = desiredEndTranslationRelRoot_importance;
  }
  NM_INLINE float getDesiredEndTranslationRelRootImportance() const { return m_desiredEndTranslationRelRootImportance; }
  NM_INLINE const float& getDesiredEndTranslationRelRootImportanceRef() const { return m_desiredEndTranslationRelRootImportance; }
  NM_INLINE const NMP::Vector3& getDesiredEndTranslationRelRoot() const { return desiredEndTranslationRelRoot; }

protected:

  NMP::Quat desiredEndRotationRelRoot;  // Desired rotation of the end of the spine relative to the root (in root's space)  (OrientationDelta)
  NMP::Vector3 desiredEndTranslationRelRoot;  ///< (PositionDelta)

  // post-assignment stubs
  NM_INLINE void postAssignDesiredEndRotationRelRoot(const NMP::Quat& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(QuatNormalised(v), "desiredEndRotationRelRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignDesiredEndTranslationRelRoot(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "desiredEndTranslationRelRoot");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_desiredEndRotationRelRootImportance, 
     m_desiredEndTranslationRelRootImportance;

  friend class SpineSupport_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getDesiredEndRotationRelRootImportance() > 0.0f)
    {
      NM_VALIDATOR(QuatNormalised(desiredEndRotationRelRoot), "desiredEndRotationRelRoot");
    }
    if (getDesiredEndTranslationRelRootImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(desiredEndTranslationRelRoot), "desiredEndTranslationRelRoot");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class SpineSupportOutputs
 * \ingroup SpineSupport
 * \brief Outputs keep the spine end supported above its root in a good posture
 *
 * Data packet definition (1312 bytes, 1344 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SpineSupportOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SpineSupportOutputs));
  }

  NM_INLINE void setUpperRotationRequestLocal(const RotationRequest& _upperRotationRequestLocal, float upperRotationRequestLocal_importance = 1.f)
  {
    upperRotationRequestLocal = _upperRotationRequestLocal;
    postAssignUpperRotationRequestLocal(upperRotationRequestLocal);
    NMP_ASSERT(upperRotationRequestLocal_importance >= 0.0f && upperRotationRequestLocal_importance <= 1.0f);
    m_upperRotationRequestLocalImportance = upperRotationRequestLocal_importance;
  }
  NM_INLINE RotationRequest& startUpperRotationRequestLocalModification()
  {
    m_upperRotationRequestLocalImportance = -1.0f; // set invalid until stopUpperRotationRequestLocalModification()
    return upperRotationRequestLocal;
  }
  NM_INLINE void stopUpperRotationRequestLocalModification(float upperRotationRequestLocal_importance = 1.f)
  {
    postAssignUpperRotationRequestLocal(upperRotationRequestLocal);
    NMP_ASSERT(upperRotationRequestLocal_importance >= 0.0f && upperRotationRequestLocal_importance <= 1.0f);
    m_upperRotationRequestLocalImportance = upperRotationRequestLocal_importance;
  }
  NM_INLINE float getUpperRotationRequestLocalImportance() const { return m_upperRotationRequestLocalImportance; }
  NM_INLINE const float& getUpperRotationRequestLocalImportanceRef() const { return m_upperRotationRequestLocalImportance; }
  NM_INLINE const RotationRequest& getUpperRotationRequestLocal() const { return upperRotationRequestLocal; }

  NM_INLINE void setLowerRotationRequestLocal(const RotationRequest& _lowerRotationRequestLocal, float lowerRotationRequestLocal_importance = 1.f)
  {
    lowerRotationRequestLocal = _lowerRotationRequestLocal;
    postAssignLowerRotationRequestLocal(lowerRotationRequestLocal);
    NMP_ASSERT(lowerRotationRequestLocal_importance >= 0.0f && lowerRotationRequestLocal_importance <= 1.0f);
    m_lowerRotationRequestLocalImportance = lowerRotationRequestLocal_importance;
  }
  NM_INLINE RotationRequest& startLowerRotationRequestLocalModification()
  {
    m_lowerRotationRequestLocalImportance = -1.0f; // set invalid until stopLowerRotationRequestLocalModification()
    return lowerRotationRequestLocal;
  }
  NM_INLINE void stopLowerRotationRequestLocalModification(float lowerRotationRequestLocal_importance = 1.f)
  {
    postAssignLowerRotationRequestLocal(lowerRotationRequestLocal);
    NMP_ASSERT(lowerRotationRequestLocal_importance >= 0.0f && lowerRotationRequestLocal_importance <= 1.0f);
    m_lowerRotationRequestLocalImportance = lowerRotationRequestLocal_importance;
  }
  NM_INLINE float getLowerRotationRequestLocalImportance() const { return m_lowerRotationRequestLocalImportance; }
  NM_INLINE const float& getLowerRotationRequestLocalImportanceRef() const { return m_lowerRotationRequestLocalImportance; }
  NM_INLINE const RotationRequest& getLowerRotationRequestLocal() const { return lowerRotationRequestLocal; }

  NM_INLINE void setUpperTranslationRequestLocal(const TranslationRequest& _upperTranslationRequestLocal, float upperTranslationRequestLocal_importance = 1.f)
  {
    upperTranslationRequestLocal = _upperTranslationRequestLocal;
    postAssignUpperTranslationRequestLocal(upperTranslationRequestLocal);
    NMP_ASSERT(upperTranslationRequestLocal_importance >= 0.0f && upperTranslationRequestLocal_importance <= 1.0f);
    m_upperTranslationRequestLocalImportance = upperTranslationRequestLocal_importance;
  }
  NM_INLINE TranslationRequest& startUpperTranslationRequestLocalModification()
  {
    m_upperTranslationRequestLocalImportance = -1.0f; // set invalid until stopUpperTranslationRequestLocalModification()
    return upperTranslationRequestLocal;
  }
  NM_INLINE void stopUpperTranslationRequestLocalModification(float upperTranslationRequestLocal_importance = 1.f)
  {
    postAssignUpperTranslationRequestLocal(upperTranslationRequestLocal);
    NMP_ASSERT(upperTranslationRequestLocal_importance >= 0.0f && upperTranslationRequestLocal_importance <= 1.0f);
    m_upperTranslationRequestLocalImportance = upperTranslationRequestLocal_importance;
  }
  NM_INLINE float getUpperTranslationRequestLocalImportance() const { return m_upperTranslationRequestLocalImportance; }
  NM_INLINE const float& getUpperTranslationRequestLocalImportanceRef() const { return m_upperTranslationRequestLocalImportance; }
  NM_INLINE const TranslationRequest& getUpperTranslationRequestLocal() const { return upperTranslationRequestLocal; }

  NM_INLINE void setLowerTranslationRequestLocal(const TranslationRequest& _lowerTranslationRequestLocal, float lowerTranslationRequestLocal_importance = 1.f)
  {
    lowerTranslationRequestLocal = _lowerTranslationRequestLocal;
    postAssignLowerTranslationRequestLocal(lowerTranslationRequestLocal);
    NMP_ASSERT(lowerTranslationRequestLocal_importance >= 0.0f && lowerTranslationRequestLocal_importance <= 1.0f);
    m_lowerTranslationRequestLocalImportance = lowerTranslationRequestLocal_importance;
  }
  NM_INLINE TranslationRequest& startLowerTranslationRequestLocalModification()
  {
    m_lowerTranslationRequestLocalImportance = -1.0f; // set invalid until stopLowerTranslationRequestLocalModification()
    return lowerTranslationRequestLocal;
  }
  NM_INLINE void stopLowerTranslationRequestLocalModification(float lowerTranslationRequestLocal_importance = 1.f)
  {
    postAssignLowerTranslationRequestLocal(lowerTranslationRequestLocal);
    NMP_ASSERT(lowerTranslationRequestLocal_importance >= 0.0f && lowerTranslationRequestLocal_importance <= 1.0f);
    m_lowerTranslationRequestLocalImportance = lowerTranslationRequestLocal_importance;
  }
  NM_INLINE float getLowerTranslationRequestLocalImportance() const { return m_lowerTranslationRequestLocalImportance; }
  NM_INLINE const float& getLowerTranslationRequestLocalImportanceRef() const { return m_lowerTranslationRequestLocalImportance; }
  NM_INLINE const TranslationRequest& getLowerTranslationRequestLocal() const { return lowerTranslationRequestLocal; }

  NM_INLINE void setUpperRotationRequestWorld(const RotationRequest& _upperRotationRequestWorld, float upperRotationRequestWorld_importance = 1.f)
  {
    upperRotationRequestWorld = _upperRotationRequestWorld;
    postAssignUpperRotationRequestWorld(upperRotationRequestWorld);
    NMP_ASSERT(upperRotationRequestWorld_importance >= 0.0f && upperRotationRequestWorld_importance <= 1.0f);
    m_upperRotationRequestWorldImportance = upperRotationRequestWorld_importance;
  }
  NM_INLINE RotationRequest& startUpperRotationRequestWorldModification()
  {
    m_upperRotationRequestWorldImportance = -1.0f; // set invalid until stopUpperRotationRequestWorldModification()
    return upperRotationRequestWorld;
  }
  NM_INLINE void stopUpperRotationRequestWorldModification(float upperRotationRequestWorld_importance = 1.f)
  {
    postAssignUpperRotationRequestWorld(upperRotationRequestWorld);
    NMP_ASSERT(upperRotationRequestWorld_importance >= 0.0f && upperRotationRequestWorld_importance <= 1.0f);
    m_upperRotationRequestWorldImportance = upperRotationRequestWorld_importance;
  }
  NM_INLINE float getUpperRotationRequestWorldImportance() const { return m_upperRotationRequestWorldImportance; }
  NM_INLINE const float& getUpperRotationRequestWorldImportanceRef() const { return m_upperRotationRequestWorldImportance; }
  NM_INLINE const RotationRequest& getUpperRotationRequestWorld() const { return upperRotationRequestWorld; }

  NM_INLINE void setLowerRotationRequestWorld(const RotationRequest& _lowerRotationRequestWorld, float lowerRotationRequestWorld_importance = 1.f)
  {
    lowerRotationRequestWorld = _lowerRotationRequestWorld;
    postAssignLowerRotationRequestWorld(lowerRotationRequestWorld);
    NMP_ASSERT(lowerRotationRequestWorld_importance >= 0.0f && lowerRotationRequestWorld_importance <= 1.0f);
    m_lowerRotationRequestWorldImportance = lowerRotationRequestWorld_importance;
  }
  NM_INLINE RotationRequest& startLowerRotationRequestWorldModification()
  {
    m_lowerRotationRequestWorldImportance = -1.0f; // set invalid until stopLowerRotationRequestWorldModification()
    return lowerRotationRequestWorld;
  }
  NM_INLINE void stopLowerRotationRequestWorldModification(float lowerRotationRequestWorld_importance = 1.f)
  {
    postAssignLowerRotationRequestWorld(lowerRotationRequestWorld);
    NMP_ASSERT(lowerRotationRequestWorld_importance >= 0.0f && lowerRotationRequestWorld_importance <= 1.0f);
    m_lowerRotationRequestWorldImportance = lowerRotationRequestWorld_importance;
  }
  NM_INLINE float getLowerRotationRequestWorldImportance() const { return m_lowerRotationRequestWorldImportance; }
  NM_INLINE const float& getLowerRotationRequestWorldImportanceRef() const { return m_lowerRotationRequestWorldImportance; }
  NM_INLINE const RotationRequest& getLowerRotationRequestWorld() const { return lowerRotationRequestWorld; }

  NM_INLINE void setUpperTranslationRequestWorld(const TranslationRequest& _upperTranslationRequestWorld, float upperTranslationRequestWorld_importance = 1.f)
  {
    upperTranslationRequestWorld = _upperTranslationRequestWorld;
    postAssignUpperTranslationRequestWorld(upperTranslationRequestWorld);
    NMP_ASSERT(upperTranslationRequestWorld_importance >= 0.0f && upperTranslationRequestWorld_importance <= 1.0f);
    m_upperTranslationRequestWorldImportance = upperTranslationRequestWorld_importance;
  }
  NM_INLINE TranslationRequest& startUpperTranslationRequestWorldModification()
  {
    m_upperTranslationRequestWorldImportance = -1.0f; // set invalid until stopUpperTranslationRequestWorldModification()
    return upperTranslationRequestWorld;
  }
  NM_INLINE void stopUpperTranslationRequestWorldModification(float upperTranslationRequestWorld_importance = 1.f)
  {
    postAssignUpperTranslationRequestWorld(upperTranslationRequestWorld);
    NMP_ASSERT(upperTranslationRequestWorld_importance >= 0.0f && upperTranslationRequestWorld_importance <= 1.0f);
    m_upperTranslationRequestWorldImportance = upperTranslationRequestWorld_importance;
  }
  NM_INLINE float getUpperTranslationRequestWorldImportance() const { return m_upperTranslationRequestWorldImportance; }
  NM_INLINE const float& getUpperTranslationRequestWorldImportanceRef() const { return m_upperTranslationRequestWorldImportance; }
  NM_INLINE const TranslationRequest& getUpperTranslationRequestWorld() const { return upperTranslationRequestWorld; }

  NM_INLINE void setLowerTranslationRequestWorld(const TranslationRequest& _lowerTranslationRequestWorld, float lowerTranslationRequestWorld_importance = 1.f)
  {
    lowerTranslationRequestWorld = _lowerTranslationRequestWorld;
    postAssignLowerTranslationRequestWorld(lowerTranslationRequestWorld);
    NMP_ASSERT(lowerTranslationRequestWorld_importance >= 0.0f && lowerTranslationRequestWorld_importance <= 1.0f);
    m_lowerTranslationRequestWorldImportance = lowerTranslationRequestWorld_importance;
  }
  NM_INLINE TranslationRequest& startLowerTranslationRequestWorldModification()
  {
    m_lowerTranslationRequestWorldImportance = -1.0f; // set invalid until stopLowerTranslationRequestWorldModification()
    return lowerTranslationRequestWorld;
  }
  NM_INLINE void stopLowerTranslationRequestWorldModification(float lowerTranslationRequestWorld_importance = 1.f)
  {
    postAssignLowerTranslationRequestWorld(lowerTranslationRequestWorld);
    NMP_ASSERT(lowerTranslationRequestWorld_importance >= 0.0f && lowerTranslationRequestWorld_importance <= 1.0f);
    m_lowerTranslationRequestWorldImportance = lowerTranslationRequestWorld_importance;
  }
  NM_INLINE float getLowerTranslationRequestWorldImportance() const { return m_lowerTranslationRequestWorldImportance; }
  NM_INLINE const float& getLowerTranslationRequestWorldImportanceRef() const { return m_lowerTranslationRequestWorldImportance; }
  NM_INLINE const TranslationRequest& getLowerTranslationRequestWorld() const { return lowerTranslationRequestWorld; }

protected:

  RotationRequest upperRotationRequestLocal;  // these below are generated from local space requests
  RotationRequest lowerRotationRequestLocal;
  TranslationRequest upperTranslationRequestLocal;
  TranslationRequest lowerTranslationRequestLocal;
  RotationRequest upperRotationRequestWorld;  // these below come from the world space requests
  RotationRequest lowerRotationRequestWorld;
  TranslationRequest upperTranslationRequestWorld;
  TranslationRequest lowerTranslationRequestWorld;

  // post-assignment stubs
  NM_INLINE void postAssignUpperRotationRequestLocal(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLowerRotationRequestLocal(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignUpperTranslationRequestLocal(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLowerTranslationRequestLocal(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignUpperRotationRequestWorld(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLowerRotationRequestWorld(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignUpperTranslationRequestWorld(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLowerTranslationRequestWorld(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_upperRotationRequestLocalImportance, 
     m_lowerRotationRequestLocalImportance, 
     m_upperTranslationRequestLocalImportance, 
     m_lowerTranslationRequestLocalImportance, 
     m_upperRotationRequestWorldImportance, 
     m_lowerRotationRequestWorldImportance, 
     m_upperTranslationRequestWorldImportance, 
     m_lowerTranslationRequestWorldImportance;

  friend class SpineSupport_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getUpperRotationRequestLocalImportance() > 0.0f)
    {
      upperRotationRequestLocal.validate();
    }
    if (getLowerRotationRequestLocalImportance() > 0.0f)
    {
      lowerRotationRequestLocal.validate();
    }
    if (getUpperTranslationRequestLocalImportance() > 0.0f)
    {
      upperTranslationRequestLocal.validate();
    }
    if (getLowerTranslationRequestLocalImportance() > 0.0f)
    {
      lowerTranslationRequestLocal.validate();
    }
    if (getUpperRotationRequestWorldImportance() > 0.0f)
    {
      upperRotationRequestWorld.validate();
    }
    if (getLowerRotationRequestWorldImportance() > 0.0f)
    {
      lowerRotationRequestWorld.validate();
    }
    if (getUpperTranslationRequestWorldImportance() > 0.0f)
    {
      upperTranslationRequestWorld.validate();
    }
    if (getLowerTranslationRequestWorldImportance() > 0.0f)
    {
      lowerTranslationRequestWorld.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class SpineSupportFeedbackInputs
 * \ingroup SpineSupport
 * \brief FeedbackInputs keep the spine end supported above its root in a good posture
 *
 * Data packet definition (68 bytes, 96 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SpineSupportFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SpineSupportFeedbackInputs));
  }

  NM_INLINE void setTargetPelvisTM(const ER::LimbTransform& _targetPelvisTM, float targetPelvisTM_importance = 1.f)
  {
    targetPelvisTM = _targetPelvisTM;
    NMP_ASSERT(targetPelvisTM_importance >= 0.0f && targetPelvisTM_importance <= 1.0f);
    m_targetPelvisTMImportance = targetPelvisTM_importance;
  }
  NM_INLINE ER::LimbTransform& startTargetPelvisTMModification()
  {
    m_targetPelvisTMImportance = -1.0f; // set invalid until stopTargetPelvisTMModification()
    return targetPelvisTM;
  }
  NM_INLINE void stopTargetPelvisTMModification(float targetPelvisTM_importance = 1.f)
  {
    NMP_ASSERT(targetPelvisTM_importance >= 0.0f && targetPelvisTM_importance <= 1.0f);
    m_targetPelvisTMImportance = targetPelvisTM_importance;
  }
  NM_INLINE float getTargetPelvisTMImportance() const { return m_targetPelvisTMImportance; }
  NM_INLINE const float& getTargetPelvisTMImportanceRef() const { return m_targetPelvisTMImportance; }
  NM_INLINE const ER::LimbTransform& getTargetPelvisTM() const { return targetPelvisTM; }

protected:

  ER::LimbTransform targetPelvisTM;

  // importance values
  float
     m_targetPelvisTMImportance;

  friend class SpineSupport_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class SpineSupportFeedbackOutputs
 * \ingroup SpineSupport
 * \brief FeedbackOutputs keep the spine end supported above its root in a good posture
 *
 * Data packet definition (20 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SpineSupportFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SpineSupportFeedbackOutputs));
  }

  NM_INLINE void setRootSupportPoint(const NMP::Vector3& _rootSupportPoint, float rootSupportPoint_importance = 1.f)
  {
    rootSupportPoint = _rootSupportPoint;
    postAssignRootSupportPoint(rootSupportPoint);
    NMP_ASSERT(rootSupportPoint_importance >= 0.0f && rootSupportPoint_importance <= 1.0f);
    m_rootSupportPointImportance = rootSupportPoint_importance;
  }
  NM_INLINE float getRootSupportPointImportance() const { return m_rootSupportPointImportance; }
  NM_INLINE const float& getRootSupportPointImportanceRef() const { return m_rootSupportPointImportance; }
  NM_INLINE const NMP::Vector3& getRootSupportPoint() const { return rootSupportPoint; }

protected:

  NMP::Vector3 rootSupportPoint;  ///< Position of spine root.  (Position)

  // post-assignment stubs
  NM_INLINE void postAssignRootSupportPoint(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "rootSupportPoint");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_rootSupportPointImportance;

  friend class SpineSupport_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getRootSupportPointImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(rootSupportPoint), "rootSupportPoint");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_SPINESUPPORT_DATA_H

