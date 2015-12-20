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

#ifndef NM_MDF_CHARACTERISTICSBEHAVIOURINTERFACE_DATA_H
#define NM_MDF_CHARACTERISTICSBEHAVIOURINTERFACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/CharacteristicsBehaviourInterface.module"

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
 * \class CharacteristicsBehaviourInterfaceOutputs
 * \ingroup CharacteristicsBehaviourInterface
 * \brief Outputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (280 bytes, 288 aligned)
 */
NMP_ALIGN_PREFIX(32) struct CharacteristicsBehaviourInterfaceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(CharacteristicsBehaviourInterfaceOutputs));
  }

  NM_INLINE void setWholeBodyStrengthReduction(const float& _wholeBodyStrengthReduction, float wholeBodyStrengthReduction_importance = 1.f)
  {
    wholeBodyStrengthReduction = _wholeBodyStrengthReduction;
    postAssignWholeBodyStrengthReduction(wholeBodyStrengthReduction);
    NMP_ASSERT(wholeBodyStrengthReduction_importance >= 0.0f && wholeBodyStrengthReduction_importance <= 1.0f);
    m_wholeBodyStrengthReductionImportance = wholeBodyStrengthReduction_importance;
  }
  NM_INLINE float getWholeBodyStrengthReductionImportance() const { return m_wholeBodyStrengthReductionImportance; }
  NM_INLINE const float& getWholeBodyStrengthReductionImportanceRef() const { return m_wholeBodyStrengthReductionImportance; }
  NM_INLINE const float& getWholeBodyStrengthReduction() const { return wholeBodyStrengthReduction; }

  enum { maxArmStrengthReduction = 2 };
  NM_INLINE unsigned int getMaxArmStrengthReduction() const { return maxArmStrengthReduction; }
  NM_INLINE void setArmStrengthReduction(unsigned int number, const float* _armStrengthReduction, float armStrengthReduction_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmStrengthReduction);
    NMP_ASSERT(armStrengthReduction_importance >= 0.0f && armStrengthReduction_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armStrengthReduction[i] = _armStrengthReduction[i];
      postAssignArmStrengthReduction(armStrengthReduction[i]);
      m_armStrengthReductionImportance[i] = armStrengthReduction_importance;
    }
  }
  NM_INLINE void setArmStrengthReductionAt(unsigned int index, const float& _armStrengthReduction, float armStrengthReduction_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmStrengthReduction);
    armStrengthReduction[index] = _armStrengthReduction;
    NMP_ASSERT(armStrengthReduction_importance >= 0.0f && armStrengthReduction_importance <= 1.0f);
    postAssignArmStrengthReduction(armStrengthReduction[index]);
    m_armStrengthReductionImportance[index] = armStrengthReduction_importance;
  }
  NM_INLINE float getArmStrengthReductionImportance(int index) const { return m_armStrengthReductionImportance[index]; }
  NM_INLINE const float& getArmStrengthReductionImportanceRef(int index) const { return m_armStrengthReductionImportance[index]; }
  NM_INLINE unsigned int calculateNumArmStrengthReduction() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armStrengthReductionImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getArmStrengthReduction(unsigned int index) const { NMP_ASSERT(index <= maxArmStrengthReduction); return armStrengthReduction[index]; }

  enum { maxHeadStrengthReduction = 1 };
  NM_INLINE unsigned int getMaxHeadStrengthReduction() const { return maxHeadStrengthReduction; }
  NM_INLINE void setHeadStrengthReduction(unsigned int number, const float* _headStrengthReduction, float headStrengthReduction_importance = 1.f)
  {
    NMP_ASSERT(number <= maxHeadStrengthReduction);
    NMP_ASSERT(headStrengthReduction_importance >= 0.0f && headStrengthReduction_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      headStrengthReduction[i] = _headStrengthReduction[i];
      postAssignHeadStrengthReduction(headStrengthReduction[i]);
      m_headStrengthReductionImportance[i] = headStrengthReduction_importance;
    }
  }
  NM_INLINE void setHeadStrengthReductionAt(unsigned int index, const float& _headStrengthReduction, float headStrengthReduction_importance = 1.f)
  {
    NMP_ASSERT(index < maxHeadStrengthReduction);
    headStrengthReduction[index] = _headStrengthReduction;
    NMP_ASSERT(headStrengthReduction_importance >= 0.0f && headStrengthReduction_importance <= 1.0f);
    postAssignHeadStrengthReduction(headStrengthReduction[index]);
    m_headStrengthReductionImportance[index] = headStrengthReduction_importance;
  }
  NM_INLINE float getHeadStrengthReductionImportance(int index) const { return m_headStrengthReductionImportance[index]; }
  NM_INLINE const float& getHeadStrengthReductionImportanceRef(int index) const { return m_headStrengthReductionImportance[index]; }
  NM_INLINE unsigned int calculateNumHeadStrengthReduction() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_headStrengthReductionImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getHeadStrengthReduction(unsigned int index) const { NMP_ASSERT(index <= maxHeadStrengthReduction); return headStrengthReduction[index]; }

  enum { maxLegStrengthReduction = 2 };
  NM_INLINE unsigned int getMaxLegStrengthReduction() const { return maxLegStrengthReduction; }
  NM_INLINE void setLegStrengthReduction(unsigned int number, const float* _legStrengthReduction, float legStrengthReduction_importance = 1.f)
  {
    NMP_ASSERT(number <= maxLegStrengthReduction);
    NMP_ASSERT(legStrengthReduction_importance >= 0.0f && legStrengthReduction_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      legStrengthReduction[i] = _legStrengthReduction[i];
      postAssignLegStrengthReduction(legStrengthReduction[i]);
      m_legStrengthReductionImportance[i] = legStrengthReduction_importance;
    }
  }
  NM_INLINE void setLegStrengthReductionAt(unsigned int index, const float& _legStrengthReduction, float legStrengthReduction_importance = 1.f)
  {
    NMP_ASSERT(index < maxLegStrengthReduction);
    legStrengthReduction[index] = _legStrengthReduction;
    NMP_ASSERT(legStrengthReduction_importance >= 0.0f && legStrengthReduction_importance <= 1.0f);
    postAssignLegStrengthReduction(legStrengthReduction[index]);
    m_legStrengthReductionImportance[index] = legStrengthReduction_importance;
  }
  NM_INLINE float getLegStrengthReductionImportance(int index) const { return m_legStrengthReductionImportance[index]; }
  NM_INLINE const float& getLegStrengthReductionImportanceRef(int index) const { return m_legStrengthReductionImportance[index]; }
  NM_INLINE unsigned int calculateNumLegStrengthReduction() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_legStrengthReductionImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getLegStrengthReduction(unsigned int index) const { NMP_ASSERT(index <= maxLegStrengthReduction); return legStrengthReduction[index]; }

  enum { maxSpineStrengthReduction = 1 };
  NM_INLINE unsigned int getMaxSpineStrengthReduction() const { return maxSpineStrengthReduction; }
  NM_INLINE void setSpineStrengthReduction(unsigned int number, const float* _spineStrengthReduction, float spineStrengthReduction_importance = 1.f)
  {
    NMP_ASSERT(number <= maxSpineStrengthReduction);
    NMP_ASSERT(spineStrengthReduction_importance >= 0.0f && spineStrengthReduction_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      spineStrengthReduction[i] = _spineStrengthReduction[i];
      postAssignSpineStrengthReduction(spineStrengthReduction[i]);
      m_spineStrengthReductionImportance[i] = spineStrengthReduction_importance;
    }
  }
  NM_INLINE void setSpineStrengthReductionAt(unsigned int index, const float& _spineStrengthReduction, float spineStrengthReduction_importance = 1.f)
  {
    NMP_ASSERT(index < maxSpineStrengthReduction);
    spineStrengthReduction[index] = _spineStrengthReduction;
    NMP_ASSERT(spineStrengthReduction_importance >= 0.0f && spineStrengthReduction_importance <= 1.0f);
    postAssignSpineStrengthReduction(spineStrengthReduction[index]);
    m_spineStrengthReductionImportance[index] = spineStrengthReduction_importance;
  }
  NM_INLINE float getSpineStrengthReductionImportance(int index) const { return m_spineStrengthReductionImportance[index]; }
  NM_INLINE const float& getSpineStrengthReductionImportanceRef(int index) const { return m_spineStrengthReductionImportance[index]; }
  NM_INLINE unsigned int calculateNumSpineStrengthReduction() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_spineStrengthReductionImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getSpineStrengthReduction(unsigned int index) const { NMP_ASSERT(index <= maxSpineStrengthReduction); return spineStrengthReduction[index]; }

  NM_INLINE void setWholeBodyControlCompensationReduction(const float& _wholeBodyControlCompensationReduction, float wholeBodyControlCompensationReduction_importance = 1.f)
  {
    wholeBodyControlCompensationReduction = _wholeBodyControlCompensationReduction;
    postAssignWholeBodyControlCompensationReduction(wholeBodyControlCompensationReduction);
    NMP_ASSERT(wholeBodyControlCompensationReduction_importance >= 0.0f && wholeBodyControlCompensationReduction_importance <= 1.0f);
    m_wholeBodyControlCompensationReductionImportance = wholeBodyControlCompensationReduction_importance;
  }
  NM_INLINE float getWholeBodyControlCompensationReductionImportance() const { return m_wholeBodyControlCompensationReductionImportance; }
  NM_INLINE const float& getWholeBodyControlCompensationReductionImportanceRef() const { return m_wholeBodyControlCompensationReductionImportance; }
  NM_INLINE const float& getWholeBodyControlCompensationReduction() const { return wholeBodyControlCompensationReduction; }

  enum { maxArmControlCompensationReduction = 2 };
  NM_INLINE unsigned int getMaxArmControlCompensationReduction() const { return maxArmControlCompensationReduction; }
  NM_INLINE void setArmControlCompensationReduction(unsigned int number, const float* _armControlCompensationReduction, float armControlCompensationReduction_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmControlCompensationReduction);
    NMP_ASSERT(armControlCompensationReduction_importance >= 0.0f && armControlCompensationReduction_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armControlCompensationReduction[i] = _armControlCompensationReduction[i];
      postAssignArmControlCompensationReduction(armControlCompensationReduction[i]);
      m_armControlCompensationReductionImportance[i] = armControlCompensationReduction_importance;
    }
  }
  NM_INLINE void setArmControlCompensationReductionAt(unsigned int index, const float& _armControlCompensationReduction, float armControlCompensationReduction_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmControlCompensationReduction);
    armControlCompensationReduction[index] = _armControlCompensationReduction;
    NMP_ASSERT(armControlCompensationReduction_importance >= 0.0f && armControlCompensationReduction_importance <= 1.0f);
    postAssignArmControlCompensationReduction(armControlCompensationReduction[index]);
    m_armControlCompensationReductionImportance[index] = armControlCompensationReduction_importance;
  }
  NM_INLINE float getArmControlCompensationReductionImportance(int index) const { return m_armControlCompensationReductionImportance[index]; }
  NM_INLINE const float& getArmControlCompensationReductionImportanceRef(int index) const { return m_armControlCompensationReductionImportance[index]; }
  NM_INLINE unsigned int calculateNumArmControlCompensationReduction() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armControlCompensationReductionImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getArmControlCompensationReduction(unsigned int index) const { NMP_ASSERT(index <= maxArmControlCompensationReduction); return armControlCompensationReduction[index]; }

  enum { maxHeadControlCompensationReduction = 1 };
  NM_INLINE unsigned int getMaxHeadControlCompensationReduction() const { return maxHeadControlCompensationReduction; }
  NM_INLINE void setHeadControlCompensationReduction(unsigned int number, const float* _headControlCompensationReduction, float headControlCompensationReduction_importance = 1.f)
  {
    NMP_ASSERT(number <= maxHeadControlCompensationReduction);
    NMP_ASSERT(headControlCompensationReduction_importance >= 0.0f && headControlCompensationReduction_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      headControlCompensationReduction[i] = _headControlCompensationReduction[i];
      postAssignHeadControlCompensationReduction(headControlCompensationReduction[i]);
      m_headControlCompensationReductionImportance[i] = headControlCompensationReduction_importance;
    }
  }
  NM_INLINE void setHeadControlCompensationReductionAt(unsigned int index, const float& _headControlCompensationReduction, float headControlCompensationReduction_importance = 1.f)
  {
    NMP_ASSERT(index < maxHeadControlCompensationReduction);
    headControlCompensationReduction[index] = _headControlCompensationReduction;
    NMP_ASSERT(headControlCompensationReduction_importance >= 0.0f && headControlCompensationReduction_importance <= 1.0f);
    postAssignHeadControlCompensationReduction(headControlCompensationReduction[index]);
    m_headControlCompensationReductionImportance[index] = headControlCompensationReduction_importance;
  }
  NM_INLINE float getHeadControlCompensationReductionImportance(int index) const { return m_headControlCompensationReductionImportance[index]; }
  NM_INLINE const float& getHeadControlCompensationReductionImportanceRef(int index) const { return m_headControlCompensationReductionImportance[index]; }
  NM_INLINE unsigned int calculateNumHeadControlCompensationReduction() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_headControlCompensationReductionImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getHeadControlCompensationReduction(unsigned int index) const { NMP_ASSERT(index <= maxHeadControlCompensationReduction); return headControlCompensationReduction[index]; }

  enum { maxLegControlCompensationReduction = 2 };
  NM_INLINE unsigned int getMaxLegControlCompensationReduction() const { return maxLegControlCompensationReduction; }
  NM_INLINE void setLegControlCompensationReduction(unsigned int number, const float* _legControlCompensationReduction, float legControlCompensationReduction_importance = 1.f)
  {
    NMP_ASSERT(number <= maxLegControlCompensationReduction);
    NMP_ASSERT(legControlCompensationReduction_importance >= 0.0f && legControlCompensationReduction_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      legControlCompensationReduction[i] = _legControlCompensationReduction[i];
      postAssignLegControlCompensationReduction(legControlCompensationReduction[i]);
      m_legControlCompensationReductionImportance[i] = legControlCompensationReduction_importance;
    }
  }
  NM_INLINE void setLegControlCompensationReductionAt(unsigned int index, const float& _legControlCompensationReduction, float legControlCompensationReduction_importance = 1.f)
  {
    NMP_ASSERT(index < maxLegControlCompensationReduction);
    legControlCompensationReduction[index] = _legControlCompensationReduction;
    NMP_ASSERT(legControlCompensationReduction_importance >= 0.0f && legControlCompensationReduction_importance <= 1.0f);
    postAssignLegControlCompensationReduction(legControlCompensationReduction[index]);
    m_legControlCompensationReductionImportance[index] = legControlCompensationReduction_importance;
  }
  NM_INLINE float getLegControlCompensationReductionImportance(int index) const { return m_legControlCompensationReductionImportance[index]; }
  NM_INLINE const float& getLegControlCompensationReductionImportanceRef(int index) const { return m_legControlCompensationReductionImportance[index]; }
  NM_INLINE unsigned int calculateNumLegControlCompensationReduction() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_legControlCompensationReductionImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getLegControlCompensationReduction(unsigned int index) const { NMP_ASSERT(index <= maxLegControlCompensationReduction); return legControlCompensationReduction[index]; }

  enum { maxSpineControlCompensationReduction = 1 };
  NM_INLINE unsigned int getMaxSpineControlCompensationReduction() const { return maxSpineControlCompensationReduction; }
  NM_INLINE void setSpineControlCompensationReduction(unsigned int number, const float* _spineControlCompensationReduction, float spineControlCompensationReduction_importance = 1.f)
  {
    NMP_ASSERT(number <= maxSpineControlCompensationReduction);
    NMP_ASSERT(spineControlCompensationReduction_importance >= 0.0f && spineControlCompensationReduction_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      spineControlCompensationReduction[i] = _spineControlCompensationReduction[i];
      postAssignSpineControlCompensationReduction(spineControlCompensationReduction[i]);
      m_spineControlCompensationReductionImportance[i] = spineControlCompensationReduction_importance;
    }
  }
  NM_INLINE void setSpineControlCompensationReductionAt(unsigned int index, const float& _spineControlCompensationReduction, float spineControlCompensationReduction_importance = 1.f)
  {
    NMP_ASSERT(index < maxSpineControlCompensationReduction);
    spineControlCompensationReduction[index] = _spineControlCompensationReduction;
    NMP_ASSERT(spineControlCompensationReduction_importance >= 0.0f && spineControlCompensationReduction_importance <= 1.0f);
    postAssignSpineControlCompensationReduction(spineControlCompensationReduction[index]);
    m_spineControlCompensationReductionImportance[index] = spineControlCompensationReduction_importance;
  }
  NM_INLINE float getSpineControlCompensationReductionImportance(int index) const { return m_spineControlCompensationReductionImportance[index]; }
  NM_INLINE const float& getSpineControlCompensationReductionImportanceRef(int index) const { return m_spineControlCompensationReductionImportance[index]; }
  NM_INLINE unsigned int calculateNumSpineControlCompensationReduction() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_spineControlCompensationReductionImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getSpineControlCompensationReduction(unsigned int index) const { NMP_ASSERT(index <= maxSpineControlCompensationReduction); return spineControlCompensationReduction[index]; }

  NM_INLINE void setWholeBodyExternalComplianceReduction(const float& _wholeBodyExternalComplianceReduction, float wholeBodyExternalComplianceReduction_importance = 1.f)
  {
    wholeBodyExternalComplianceReduction = _wholeBodyExternalComplianceReduction;
    postAssignWholeBodyExternalComplianceReduction(wholeBodyExternalComplianceReduction);
    NMP_ASSERT(wholeBodyExternalComplianceReduction_importance >= 0.0f && wholeBodyExternalComplianceReduction_importance <= 1.0f);
    m_wholeBodyExternalComplianceReductionImportance = wholeBodyExternalComplianceReduction_importance;
  }
  NM_INLINE float getWholeBodyExternalComplianceReductionImportance() const { return m_wholeBodyExternalComplianceReductionImportance; }
  NM_INLINE const float& getWholeBodyExternalComplianceReductionImportanceRef() const { return m_wholeBodyExternalComplianceReductionImportance; }
  NM_INLINE const float& getWholeBodyExternalComplianceReduction() const { return wholeBodyExternalComplianceReduction; }

  enum { maxArmExternalComplianceReduction = 2 };
  NM_INLINE unsigned int getMaxArmExternalComplianceReduction() const { return maxArmExternalComplianceReduction; }
  NM_INLINE void setArmExternalComplianceReduction(unsigned int number, const float* _armExternalComplianceReduction, float armExternalComplianceReduction_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmExternalComplianceReduction);
    NMP_ASSERT(armExternalComplianceReduction_importance >= 0.0f && armExternalComplianceReduction_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armExternalComplianceReduction[i] = _armExternalComplianceReduction[i];
      postAssignArmExternalComplianceReduction(armExternalComplianceReduction[i]);
      m_armExternalComplianceReductionImportance[i] = armExternalComplianceReduction_importance;
    }
  }
  NM_INLINE void setArmExternalComplianceReductionAt(unsigned int index, const float& _armExternalComplianceReduction, float armExternalComplianceReduction_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmExternalComplianceReduction);
    armExternalComplianceReduction[index] = _armExternalComplianceReduction;
    NMP_ASSERT(armExternalComplianceReduction_importance >= 0.0f && armExternalComplianceReduction_importance <= 1.0f);
    postAssignArmExternalComplianceReduction(armExternalComplianceReduction[index]);
    m_armExternalComplianceReductionImportance[index] = armExternalComplianceReduction_importance;
  }
  NM_INLINE float getArmExternalComplianceReductionImportance(int index) const { return m_armExternalComplianceReductionImportance[index]; }
  NM_INLINE const float& getArmExternalComplianceReductionImportanceRef(int index) const { return m_armExternalComplianceReductionImportance[index]; }
  NM_INLINE unsigned int calculateNumArmExternalComplianceReduction() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armExternalComplianceReductionImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getArmExternalComplianceReduction(unsigned int index) const { NMP_ASSERT(index <= maxArmExternalComplianceReduction); return armExternalComplianceReduction[index]; }

  enum { maxHeadExternalComplianceReduction = 1 };
  NM_INLINE unsigned int getMaxHeadExternalComplianceReduction() const { return maxHeadExternalComplianceReduction; }
  NM_INLINE void setHeadExternalComplianceReduction(unsigned int number, const float* _headExternalComplianceReduction, float headExternalComplianceReduction_importance = 1.f)
  {
    NMP_ASSERT(number <= maxHeadExternalComplianceReduction);
    NMP_ASSERT(headExternalComplianceReduction_importance >= 0.0f && headExternalComplianceReduction_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      headExternalComplianceReduction[i] = _headExternalComplianceReduction[i];
      postAssignHeadExternalComplianceReduction(headExternalComplianceReduction[i]);
      m_headExternalComplianceReductionImportance[i] = headExternalComplianceReduction_importance;
    }
  }
  NM_INLINE void setHeadExternalComplianceReductionAt(unsigned int index, const float& _headExternalComplianceReduction, float headExternalComplianceReduction_importance = 1.f)
  {
    NMP_ASSERT(index < maxHeadExternalComplianceReduction);
    headExternalComplianceReduction[index] = _headExternalComplianceReduction;
    NMP_ASSERT(headExternalComplianceReduction_importance >= 0.0f && headExternalComplianceReduction_importance <= 1.0f);
    postAssignHeadExternalComplianceReduction(headExternalComplianceReduction[index]);
    m_headExternalComplianceReductionImportance[index] = headExternalComplianceReduction_importance;
  }
  NM_INLINE float getHeadExternalComplianceReductionImportance(int index) const { return m_headExternalComplianceReductionImportance[index]; }
  NM_INLINE const float& getHeadExternalComplianceReductionImportanceRef(int index) const { return m_headExternalComplianceReductionImportance[index]; }
  NM_INLINE unsigned int calculateNumHeadExternalComplianceReduction() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_headExternalComplianceReductionImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getHeadExternalComplianceReduction(unsigned int index) const { NMP_ASSERT(index <= maxHeadExternalComplianceReduction); return headExternalComplianceReduction[index]; }

  enum { maxLegExternalComplianceReduction = 2 };
  NM_INLINE unsigned int getMaxLegExternalComplianceReduction() const { return maxLegExternalComplianceReduction; }
  NM_INLINE void setLegExternalComplianceReduction(unsigned int number, const float* _legExternalComplianceReduction, float legExternalComplianceReduction_importance = 1.f)
  {
    NMP_ASSERT(number <= maxLegExternalComplianceReduction);
    NMP_ASSERT(legExternalComplianceReduction_importance >= 0.0f && legExternalComplianceReduction_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      legExternalComplianceReduction[i] = _legExternalComplianceReduction[i];
      postAssignLegExternalComplianceReduction(legExternalComplianceReduction[i]);
      m_legExternalComplianceReductionImportance[i] = legExternalComplianceReduction_importance;
    }
  }
  NM_INLINE void setLegExternalComplianceReductionAt(unsigned int index, const float& _legExternalComplianceReduction, float legExternalComplianceReduction_importance = 1.f)
  {
    NMP_ASSERT(index < maxLegExternalComplianceReduction);
    legExternalComplianceReduction[index] = _legExternalComplianceReduction;
    NMP_ASSERT(legExternalComplianceReduction_importance >= 0.0f && legExternalComplianceReduction_importance <= 1.0f);
    postAssignLegExternalComplianceReduction(legExternalComplianceReduction[index]);
    m_legExternalComplianceReductionImportance[index] = legExternalComplianceReduction_importance;
  }
  NM_INLINE float getLegExternalComplianceReductionImportance(int index) const { return m_legExternalComplianceReductionImportance[index]; }
  NM_INLINE const float& getLegExternalComplianceReductionImportanceRef(int index) const { return m_legExternalComplianceReductionImportance[index]; }
  NM_INLINE unsigned int calculateNumLegExternalComplianceReduction() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_legExternalComplianceReductionImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getLegExternalComplianceReduction(unsigned int index) const { NMP_ASSERT(index <= maxLegExternalComplianceReduction); return legExternalComplianceReduction[index]; }

  enum { maxSpineExternalComplianceReduction = 1 };
  NM_INLINE unsigned int getMaxSpineExternalComplianceReduction() const { return maxSpineExternalComplianceReduction; }
  NM_INLINE void setSpineExternalComplianceReduction(unsigned int number, const float* _spineExternalComplianceReduction, float spineExternalComplianceReduction_importance = 1.f)
  {
    NMP_ASSERT(number <= maxSpineExternalComplianceReduction);
    NMP_ASSERT(spineExternalComplianceReduction_importance >= 0.0f && spineExternalComplianceReduction_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      spineExternalComplianceReduction[i] = _spineExternalComplianceReduction[i];
      postAssignSpineExternalComplianceReduction(spineExternalComplianceReduction[i]);
      m_spineExternalComplianceReductionImportance[i] = spineExternalComplianceReduction_importance;
    }
  }
  NM_INLINE void setSpineExternalComplianceReductionAt(unsigned int index, const float& _spineExternalComplianceReduction, float spineExternalComplianceReduction_importance = 1.f)
  {
    NMP_ASSERT(index < maxSpineExternalComplianceReduction);
    spineExternalComplianceReduction[index] = _spineExternalComplianceReduction;
    NMP_ASSERT(spineExternalComplianceReduction_importance >= 0.0f && spineExternalComplianceReduction_importance <= 1.0f);
    postAssignSpineExternalComplianceReduction(spineExternalComplianceReduction[index]);
    m_spineExternalComplianceReductionImportance[index] = spineExternalComplianceReduction_importance;
  }
  NM_INLINE float getSpineExternalComplianceReductionImportance(int index) const { return m_spineExternalComplianceReductionImportance[index]; }
  NM_INLINE const float& getSpineExternalComplianceReductionImportanceRef(int index) const { return m_spineExternalComplianceReductionImportance[index]; }
  NM_INLINE unsigned int calculateNumSpineExternalComplianceReduction() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_spineExternalComplianceReductionImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getSpineExternalComplianceReduction(unsigned int index) const { NMP_ASSERT(index <= maxSpineExternalComplianceReduction); return spineExternalComplianceReduction[index]; }

  NM_INLINE void setWholeBodyDampingRatioReduction(const float& _wholeBodyDampingRatioReduction, float wholeBodyDampingRatioReduction_importance = 1.f)
  {
    wholeBodyDampingRatioReduction = _wholeBodyDampingRatioReduction;
    postAssignWholeBodyDampingRatioReduction(wholeBodyDampingRatioReduction);
    NMP_ASSERT(wholeBodyDampingRatioReduction_importance >= 0.0f && wholeBodyDampingRatioReduction_importance <= 1.0f);
    m_wholeBodyDampingRatioReductionImportance = wholeBodyDampingRatioReduction_importance;
  }
  NM_INLINE float getWholeBodyDampingRatioReductionImportance() const { return m_wholeBodyDampingRatioReductionImportance; }
  NM_INLINE const float& getWholeBodyDampingRatioReductionImportanceRef() const { return m_wholeBodyDampingRatioReductionImportance; }
  NM_INLINE const float& getWholeBodyDampingRatioReduction() const { return wholeBodyDampingRatioReduction; }

  enum { maxArmDampingRatioReduction = 2 };
  NM_INLINE unsigned int getMaxArmDampingRatioReduction() const { return maxArmDampingRatioReduction; }
  NM_INLINE void setArmDampingRatioReduction(unsigned int number, const float* _armDampingRatioReduction, float armDampingRatioReduction_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmDampingRatioReduction);
    NMP_ASSERT(armDampingRatioReduction_importance >= 0.0f && armDampingRatioReduction_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armDampingRatioReduction[i] = _armDampingRatioReduction[i];
      postAssignArmDampingRatioReduction(armDampingRatioReduction[i]);
      m_armDampingRatioReductionImportance[i] = armDampingRatioReduction_importance;
    }
  }
  NM_INLINE void setArmDampingRatioReductionAt(unsigned int index, const float& _armDampingRatioReduction, float armDampingRatioReduction_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmDampingRatioReduction);
    armDampingRatioReduction[index] = _armDampingRatioReduction;
    NMP_ASSERT(armDampingRatioReduction_importance >= 0.0f && armDampingRatioReduction_importance <= 1.0f);
    postAssignArmDampingRatioReduction(armDampingRatioReduction[index]);
    m_armDampingRatioReductionImportance[index] = armDampingRatioReduction_importance;
  }
  NM_INLINE float getArmDampingRatioReductionImportance(int index) const { return m_armDampingRatioReductionImportance[index]; }
  NM_INLINE const float& getArmDampingRatioReductionImportanceRef(int index) const { return m_armDampingRatioReductionImportance[index]; }
  NM_INLINE unsigned int calculateNumArmDampingRatioReduction() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armDampingRatioReductionImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getArmDampingRatioReduction(unsigned int index) const { NMP_ASSERT(index <= maxArmDampingRatioReduction); return armDampingRatioReduction[index]; }

  enum { maxHeadDampingRatioReduction = 1 };
  NM_INLINE unsigned int getMaxHeadDampingRatioReduction() const { return maxHeadDampingRatioReduction; }
  NM_INLINE void setHeadDampingRatioReduction(unsigned int number, const float* _headDampingRatioReduction, float headDampingRatioReduction_importance = 1.f)
  {
    NMP_ASSERT(number <= maxHeadDampingRatioReduction);
    NMP_ASSERT(headDampingRatioReduction_importance >= 0.0f && headDampingRatioReduction_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      headDampingRatioReduction[i] = _headDampingRatioReduction[i];
      postAssignHeadDampingRatioReduction(headDampingRatioReduction[i]);
      m_headDampingRatioReductionImportance[i] = headDampingRatioReduction_importance;
    }
  }
  NM_INLINE void setHeadDampingRatioReductionAt(unsigned int index, const float& _headDampingRatioReduction, float headDampingRatioReduction_importance = 1.f)
  {
    NMP_ASSERT(index < maxHeadDampingRatioReduction);
    headDampingRatioReduction[index] = _headDampingRatioReduction;
    NMP_ASSERT(headDampingRatioReduction_importance >= 0.0f && headDampingRatioReduction_importance <= 1.0f);
    postAssignHeadDampingRatioReduction(headDampingRatioReduction[index]);
    m_headDampingRatioReductionImportance[index] = headDampingRatioReduction_importance;
  }
  NM_INLINE float getHeadDampingRatioReductionImportance(int index) const { return m_headDampingRatioReductionImportance[index]; }
  NM_INLINE const float& getHeadDampingRatioReductionImportanceRef(int index) const { return m_headDampingRatioReductionImportance[index]; }
  NM_INLINE unsigned int calculateNumHeadDampingRatioReduction() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_headDampingRatioReductionImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getHeadDampingRatioReduction(unsigned int index) const { NMP_ASSERT(index <= maxHeadDampingRatioReduction); return headDampingRatioReduction[index]; }

  enum { maxLegDampingRatioReduction = 2 };
  NM_INLINE unsigned int getMaxLegDampingRatioReduction() const { return maxLegDampingRatioReduction; }
  NM_INLINE void setLegDampingRatioReduction(unsigned int number, const float* _legDampingRatioReduction, float legDampingRatioReduction_importance = 1.f)
  {
    NMP_ASSERT(number <= maxLegDampingRatioReduction);
    NMP_ASSERT(legDampingRatioReduction_importance >= 0.0f && legDampingRatioReduction_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      legDampingRatioReduction[i] = _legDampingRatioReduction[i];
      postAssignLegDampingRatioReduction(legDampingRatioReduction[i]);
      m_legDampingRatioReductionImportance[i] = legDampingRatioReduction_importance;
    }
  }
  NM_INLINE void setLegDampingRatioReductionAt(unsigned int index, const float& _legDampingRatioReduction, float legDampingRatioReduction_importance = 1.f)
  {
    NMP_ASSERT(index < maxLegDampingRatioReduction);
    legDampingRatioReduction[index] = _legDampingRatioReduction;
    NMP_ASSERT(legDampingRatioReduction_importance >= 0.0f && legDampingRatioReduction_importance <= 1.0f);
    postAssignLegDampingRatioReduction(legDampingRatioReduction[index]);
    m_legDampingRatioReductionImportance[index] = legDampingRatioReduction_importance;
  }
  NM_INLINE float getLegDampingRatioReductionImportance(int index) const { return m_legDampingRatioReductionImportance[index]; }
  NM_INLINE const float& getLegDampingRatioReductionImportanceRef(int index) const { return m_legDampingRatioReductionImportance[index]; }
  NM_INLINE unsigned int calculateNumLegDampingRatioReduction() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_legDampingRatioReductionImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getLegDampingRatioReduction(unsigned int index) const { NMP_ASSERT(index <= maxLegDampingRatioReduction); return legDampingRatioReduction[index]; }

  enum { maxSpineDampingRatioReduction = 1 };
  NM_INLINE unsigned int getMaxSpineDampingRatioReduction() const { return maxSpineDampingRatioReduction; }
  NM_INLINE void setSpineDampingRatioReduction(unsigned int number, const float* _spineDampingRatioReduction, float spineDampingRatioReduction_importance = 1.f)
  {
    NMP_ASSERT(number <= maxSpineDampingRatioReduction);
    NMP_ASSERT(spineDampingRatioReduction_importance >= 0.0f && spineDampingRatioReduction_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      spineDampingRatioReduction[i] = _spineDampingRatioReduction[i];
      postAssignSpineDampingRatioReduction(spineDampingRatioReduction[i]);
      m_spineDampingRatioReductionImportance[i] = spineDampingRatioReduction_importance;
    }
  }
  NM_INLINE void setSpineDampingRatioReductionAt(unsigned int index, const float& _spineDampingRatioReduction, float spineDampingRatioReduction_importance = 1.f)
  {
    NMP_ASSERT(index < maxSpineDampingRatioReduction);
    spineDampingRatioReduction[index] = _spineDampingRatioReduction;
    NMP_ASSERT(spineDampingRatioReduction_importance >= 0.0f && spineDampingRatioReduction_importance <= 1.0f);
    postAssignSpineDampingRatioReduction(spineDampingRatioReduction[index]);
    m_spineDampingRatioReductionImportance[index] = spineDampingRatioReduction_importance;
  }
  NM_INLINE float getSpineDampingRatioReductionImportance(int index) const { return m_spineDampingRatioReductionImportance[index]; }
  NM_INLINE const float& getSpineDampingRatioReductionImportanceRef(int index) const { return m_spineDampingRatioReductionImportance[index]; }
  NM_INLINE unsigned int calculateNumSpineDampingRatioReduction() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_spineDampingRatioReductionImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getSpineDampingRatioReduction(unsigned int index) const { NMP_ASSERT(index <= maxSpineDampingRatioReduction); return spineDampingRatioReduction[index]; }

  NM_INLINE void setWholeBodySoftLimitStiffnessReduction(const float& _wholeBodySoftLimitStiffnessReduction, float wholeBodySoftLimitStiffnessReduction_importance = 1.f)
  {
    wholeBodySoftLimitStiffnessReduction = _wholeBodySoftLimitStiffnessReduction;
    postAssignWholeBodySoftLimitStiffnessReduction(wholeBodySoftLimitStiffnessReduction);
    NMP_ASSERT(wholeBodySoftLimitStiffnessReduction_importance >= 0.0f && wholeBodySoftLimitStiffnessReduction_importance <= 1.0f);
    m_wholeBodySoftLimitStiffnessReductionImportance = wholeBodySoftLimitStiffnessReduction_importance;
  }
  NM_INLINE float getWholeBodySoftLimitStiffnessReductionImportance() const { return m_wholeBodySoftLimitStiffnessReductionImportance; }
  NM_INLINE const float& getWholeBodySoftLimitStiffnessReductionImportanceRef() const { return m_wholeBodySoftLimitStiffnessReductionImportance; }
  NM_INLINE const float& getWholeBodySoftLimitStiffnessReduction() const { return wholeBodySoftLimitStiffnessReduction; }

  enum { maxArmSoftLimitStiffnessReduction = 2 };
  NM_INLINE unsigned int getMaxArmSoftLimitStiffnessReduction() const { return maxArmSoftLimitStiffnessReduction; }
  NM_INLINE void setArmSoftLimitStiffnessReduction(unsigned int number, const float* _armSoftLimitStiffnessReduction, float armSoftLimitStiffnessReduction_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmSoftLimitStiffnessReduction);
    NMP_ASSERT(armSoftLimitStiffnessReduction_importance >= 0.0f && armSoftLimitStiffnessReduction_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armSoftLimitStiffnessReduction[i] = _armSoftLimitStiffnessReduction[i];
      postAssignArmSoftLimitStiffnessReduction(armSoftLimitStiffnessReduction[i]);
      m_armSoftLimitStiffnessReductionImportance[i] = armSoftLimitStiffnessReduction_importance;
    }
  }
  NM_INLINE void setArmSoftLimitStiffnessReductionAt(unsigned int index, const float& _armSoftLimitStiffnessReduction, float armSoftLimitStiffnessReduction_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmSoftLimitStiffnessReduction);
    armSoftLimitStiffnessReduction[index] = _armSoftLimitStiffnessReduction;
    NMP_ASSERT(armSoftLimitStiffnessReduction_importance >= 0.0f && armSoftLimitStiffnessReduction_importance <= 1.0f);
    postAssignArmSoftLimitStiffnessReduction(armSoftLimitStiffnessReduction[index]);
    m_armSoftLimitStiffnessReductionImportance[index] = armSoftLimitStiffnessReduction_importance;
  }
  NM_INLINE float getArmSoftLimitStiffnessReductionImportance(int index) const { return m_armSoftLimitStiffnessReductionImportance[index]; }
  NM_INLINE const float& getArmSoftLimitStiffnessReductionImportanceRef(int index) const { return m_armSoftLimitStiffnessReductionImportance[index]; }
  NM_INLINE unsigned int calculateNumArmSoftLimitStiffnessReduction() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armSoftLimitStiffnessReductionImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getArmSoftLimitStiffnessReduction(unsigned int index) const { NMP_ASSERT(index <= maxArmSoftLimitStiffnessReduction); return armSoftLimitStiffnessReduction[index]; }

  enum { maxHeadSoftLimitStiffnessReduction = 1 };
  NM_INLINE unsigned int getMaxHeadSoftLimitStiffnessReduction() const { return maxHeadSoftLimitStiffnessReduction; }
  NM_INLINE void setHeadSoftLimitStiffnessReduction(unsigned int number, const float* _headSoftLimitStiffnessReduction, float headSoftLimitStiffnessReduction_importance = 1.f)
  {
    NMP_ASSERT(number <= maxHeadSoftLimitStiffnessReduction);
    NMP_ASSERT(headSoftLimitStiffnessReduction_importance >= 0.0f && headSoftLimitStiffnessReduction_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      headSoftLimitStiffnessReduction[i] = _headSoftLimitStiffnessReduction[i];
      postAssignHeadSoftLimitStiffnessReduction(headSoftLimitStiffnessReduction[i]);
      m_headSoftLimitStiffnessReductionImportance[i] = headSoftLimitStiffnessReduction_importance;
    }
  }
  NM_INLINE void setHeadSoftLimitStiffnessReductionAt(unsigned int index, const float& _headSoftLimitStiffnessReduction, float headSoftLimitStiffnessReduction_importance = 1.f)
  {
    NMP_ASSERT(index < maxHeadSoftLimitStiffnessReduction);
    headSoftLimitStiffnessReduction[index] = _headSoftLimitStiffnessReduction;
    NMP_ASSERT(headSoftLimitStiffnessReduction_importance >= 0.0f && headSoftLimitStiffnessReduction_importance <= 1.0f);
    postAssignHeadSoftLimitStiffnessReduction(headSoftLimitStiffnessReduction[index]);
    m_headSoftLimitStiffnessReductionImportance[index] = headSoftLimitStiffnessReduction_importance;
  }
  NM_INLINE float getHeadSoftLimitStiffnessReductionImportance(int index) const { return m_headSoftLimitStiffnessReductionImportance[index]; }
  NM_INLINE const float& getHeadSoftLimitStiffnessReductionImportanceRef(int index) const { return m_headSoftLimitStiffnessReductionImportance[index]; }
  NM_INLINE unsigned int calculateNumHeadSoftLimitStiffnessReduction() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_headSoftLimitStiffnessReductionImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getHeadSoftLimitStiffnessReduction(unsigned int index) const { NMP_ASSERT(index <= maxHeadSoftLimitStiffnessReduction); return headSoftLimitStiffnessReduction[index]; }

  enum { maxLegSoftLimitStiffnessReduction = 2 };
  NM_INLINE unsigned int getMaxLegSoftLimitStiffnessReduction() const { return maxLegSoftLimitStiffnessReduction; }
  NM_INLINE void setLegSoftLimitStiffnessReduction(unsigned int number, const float* _legSoftLimitStiffnessReduction, float legSoftLimitStiffnessReduction_importance = 1.f)
  {
    NMP_ASSERT(number <= maxLegSoftLimitStiffnessReduction);
    NMP_ASSERT(legSoftLimitStiffnessReduction_importance >= 0.0f && legSoftLimitStiffnessReduction_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      legSoftLimitStiffnessReduction[i] = _legSoftLimitStiffnessReduction[i];
      postAssignLegSoftLimitStiffnessReduction(legSoftLimitStiffnessReduction[i]);
      m_legSoftLimitStiffnessReductionImportance[i] = legSoftLimitStiffnessReduction_importance;
    }
  }
  NM_INLINE void setLegSoftLimitStiffnessReductionAt(unsigned int index, const float& _legSoftLimitStiffnessReduction, float legSoftLimitStiffnessReduction_importance = 1.f)
  {
    NMP_ASSERT(index < maxLegSoftLimitStiffnessReduction);
    legSoftLimitStiffnessReduction[index] = _legSoftLimitStiffnessReduction;
    NMP_ASSERT(legSoftLimitStiffnessReduction_importance >= 0.0f && legSoftLimitStiffnessReduction_importance <= 1.0f);
    postAssignLegSoftLimitStiffnessReduction(legSoftLimitStiffnessReduction[index]);
    m_legSoftLimitStiffnessReductionImportance[index] = legSoftLimitStiffnessReduction_importance;
  }
  NM_INLINE float getLegSoftLimitStiffnessReductionImportance(int index) const { return m_legSoftLimitStiffnessReductionImportance[index]; }
  NM_INLINE const float& getLegSoftLimitStiffnessReductionImportanceRef(int index) const { return m_legSoftLimitStiffnessReductionImportance[index]; }
  NM_INLINE unsigned int calculateNumLegSoftLimitStiffnessReduction() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_legSoftLimitStiffnessReductionImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getLegSoftLimitStiffnessReduction(unsigned int index) const { NMP_ASSERT(index <= maxLegSoftLimitStiffnessReduction); return legSoftLimitStiffnessReduction[index]; }

  enum { maxSpineSoftLimitStiffnessReduction = 1 };
  NM_INLINE unsigned int getMaxSpineSoftLimitStiffnessReduction() const { return maxSpineSoftLimitStiffnessReduction; }
  NM_INLINE void setSpineSoftLimitStiffnessReduction(unsigned int number, const float* _spineSoftLimitStiffnessReduction, float spineSoftLimitStiffnessReduction_importance = 1.f)
  {
    NMP_ASSERT(number <= maxSpineSoftLimitStiffnessReduction);
    NMP_ASSERT(spineSoftLimitStiffnessReduction_importance >= 0.0f && spineSoftLimitStiffnessReduction_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      spineSoftLimitStiffnessReduction[i] = _spineSoftLimitStiffnessReduction[i];
      postAssignSpineSoftLimitStiffnessReduction(spineSoftLimitStiffnessReduction[i]);
      m_spineSoftLimitStiffnessReductionImportance[i] = spineSoftLimitStiffnessReduction_importance;
    }
  }
  NM_INLINE void setSpineSoftLimitStiffnessReductionAt(unsigned int index, const float& _spineSoftLimitStiffnessReduction, float spineSoftLimitStiffnessReduction_importance = 1.f)
  {
    NMP_ASSERT(index < maxSpineSoftLimitStiffnessReduction);
    spineSoftLimitStiffnessReduction[index] = _spineSoftLimitStiffnessReduction;
    NMP_ASSERT(spineSoftLimitStiffnessReduction_importance >= 0.0f && spineSoftLimitStiffnessReduction_importance <= 1.0f);
    postAssignSpineSoftLimitStiffnessReduction(spineSoftLimitStiffnessReduction[index]);
    m_spineSoftLimitStiffnessReductionImportance[index] = spineSoftLimitStiffnessReduction_importance;
  }
  NM_INLINE float getSpineSoftLimitStiffnessReductionImportance(int index) const { return m_spineSoftLimitStiffnessReductionImportance[index]; }
  NM_INLINE const float& getSpineSoftLimitStiffnessReductionImportanceRef(int index) const { return m_spineSoftLimitStiffnessReductionImportance[index]; }
  NM_INLINE unsigned int calculateNumSpineSoftLimitStiffnessReduction() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_spineSoftLimitStiffnessReductionImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getSpineSoftLimitStiffnessReduction(unsigned int index) const { NMP_ASSERT(index <= maxSpineSoftLimitStiffnessReduction); return spineSoftLimitStiffnessReduction[index]; }

protected:

  float wholeBodyStrengthReduction;          ///< (Multiplier)
  float armStrengthReduction[NetworkConstants::networkMaxNumArms];  ///< (Multiplier)
  float headStrengthReduction[NetworkConstants::networkMaxNumHeads];  ///< (Multiplier)
  float legStrengthReduction[NetworkConstants::networkMaxNumLegs];  ///< (Multiplier)
  float spineStrengthReduction[NetworkConstants::networkMaxNumSpines];  ///< (Multiplier)
  float wholeBodyControlCompensationReduction;  ///< (Multiplier)
  float armControlCompensationReduction[NetworkConstants::networkMaxNumArms];  ///< (Multiplier)
  float headControlCompensationReduction[NetworkConstants::networkMaxNumHeads];  ///< (Multiplier)
  float legControlCompensationReduction[NetworkConstants::networkMaxNumLegs];  ///< (Multiplier)
  float spineControlCompensationReduction[NetworkConstants::networkMaxNumSpines];  ///< (Multiplier)
  float wholeBodyExternalComplianceReduction;  ///< (Multiplier)
  float armExternalComplianceReduction[NetworkConstants::networkMaxNumArms];  ///< (Multiplier)
  float headExternalComplianceReduction[NetworkConstants::networkMaxNumHeads];  ///< (Multiplier)
  float legExternalComplianceReduction[NetworkConstants::networkMaxNumLegs];  ///< (Multiplier)
  float spineExternalComplianceReduction[NetworkConstants::networkMaxNumSpines];  ///< (Multiplier)
  float wholeBodyDampingRatioReduction;      ///< (Multiplier)
  float armDampingRatioReduction[NetworkConstants::networkMaxNumArms];  ///< (Multiplier)
  float headDampingRatioReduction[NetworkConstants::networkMaxNumHeads];  ///< (Multiplier)
  float legDampingRatioReduction[NetworkConstants::networkMaxNumLegs];  ///< (Multiplier)
  float spineDampingRatioReduction[NetworkConstants::networkMaxNumSpines];  ///< (Multiplier)
  float wholeBodySoftLimitStiffnessReduction;  ///< (Multiplier)
  float armSoftLimitStiffnessReduction[NetworkConstants::networkMaxNumArms];  ///< (Multiplier)
  float headSoftLimitStiffnessReduction[NetworkConstants::networkMaxNumHeads];  ///< (Multiplier)
  float legSoftLimitStiffnessReduction[NetworkConstants::networkMaxNumLegs];  ///< (Multiplier)
  float spineSoftLimitStiffnessReduction[NetworkConstants::networkMaxNumSpines];  ///< (Multiplier)

  // post-assignment stubs
  NM_INLINE void postAssignWholeBodyStrengthReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "wholeBodyStrengthReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmStrengthReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "armStrengthReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHeadStrengthReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "headStrengthReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLegStrengthReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "legStrengthReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSpineStrengthReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "spineStrengthReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignWholeBodyControlCompensationReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "wholeBodyControlCompensationReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmControlCompensationReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "armControlCompensationReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHeadControlCompensationReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "headControlCompensationReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLegControlCompensationReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "legControlCompensationReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSpineControlCompensationReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "spineControlCompensationReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignWholeBodyExternalComplianceReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "wholeBodyExternalComplianceReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmExternalComplianceReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "armExternalComplianceReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHeadExternalComplianceReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "headExternalComplianceReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLegExternalComplianceReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "legExternalComplianceReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSpineExternalComplianceReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "spineExternalComplianceReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignWholeBodyDampingRatioReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "wholeBodyDampingRatioReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmDampingRatioReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "armDampingRatioReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHeadDampingRatioReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "headDampingRatioReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLegDampingRatioReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "legDampingRatioReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSpineDampingRatioReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "spineDampingRatioReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignWholeBodySoftLimitStiffnessReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "wholeBodySoftLimitStiffnessReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmSoftLimitStiffnessReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "armSoftLimitStiffnessReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHeadSoftLimitStiffnessReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "headSoftLimitStiffnessReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLegSoftLimitStiffnessReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "legSoftLimitStiffnessReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSpineSoftLimitStiffnessReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "spineSoftLimitStiffnessReduction");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_wholeBodyStrengthReductionImportance, 
     m_armStrengthReductionImportance[NetworkConstants::networkMaxNumArms], 
     m_headStrengthReductionImportance[NetworkConstants::networkMaxNumHeads], 
     m_legStrengthReductionImportance[NetworkConstants::networkMaxNumLegs], 
     m_spineStrengthReductionImportance[NetworkConstants::networkMaxNumSpines], 
     m_wholeBodyControlCompensationReductionImportance, 
     m_armControlCompensationReductionImportance[NetworkConstants::networkMaxNumArms], 
     m_headControlCompensationReductionImportance[NetworkConstants::networkMaxNumHeads], 
     m_legControlCompensationReductionImportance[NetworkConstants::networkMaxNumLegs], 
     m_spineControlCompensationReductionImportance[NetworkConstants::networkMaxNumSpines], 
     m_wholeBodyExternalComplianceReductionImportance, 
     m_armExternalComplianceReductionImportance[NetworkConstants::networkMaxNumArms], 
     m_headExternalComplianceReductionImportance[NetworkConstants::networkMaxNumHeads], 
     m_legExternalComplianceReductionImportance[NetworkConstants::networkMaxNumLegs], 
     m_spineExternalComplianceReductionImportance[NetworkConstants::networkMaxNumSpines], 
     m_wholeBodyDampingRatioReductionImportance, 
     m_armDampingRatioReductionImportance[NetworkConstants::networkMaxNumArms], 
     m_headDampingRatioReductionImportance[NetworkConstants::networkMaxNumHeads], 
     m_legDampingRatioReductionImportance[NetworkConstants::networkMaxNumLegs], 
     m_spineDampingRatioReductionImportance[NetworkConstants::networkMaxNumSpines], 
     m_wholeBodySoftLimitStiffnessReductionImportance, 
     m_armSoftLimitStiffnessReductionImportance[NetworkConstants::networkMaxNumArms], 
     m_headSoftLimitStiffnessReductionImportance[NetworkConstants::networkMaxNumHeads], 
     m_legSoftLimitStiffnessReductionImportance[NetworkConstants::networkMaxNumLegs], 
     m_spineSoftLimitStiffnessReductionImportance[NetworkConstants::networkMaxNumSpines];


public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getWholeBodyStrengthReductionImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(wholeBodyStrengthReduction), "wholeBodyStrengthReduction");
    }
    {
      uint32_t numArmStrengthReduction = calculateNumArmStrengthReduction();
      for (uint32_t i=0; i<numArmStrengthReduction; i++)
      {
        if (getArmStrengthReductionImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(armStrengthReduction[i]), "armStrengthReduction");
        }
      }
    }
    {
      uint32_t numHeadStrengthReduction = calculateNumHeadStrengthReduction();
      for (uint32_t i=0; i<numHeadStrengthReduction; i++)
      {
        if (getHeadStrengthReductionImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(headStrengthReduction[i]), "headStrengthReduction");
        }
      }
    }
    {
      uint32_t numLegStrengthReduction = calculateNumLegStrengthReduction();
      for (uint32_t i=0; i<numLegStrengthReduction; i++)
      {
        if (getLegStrengthReductionImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(legStrengthReduction[i]), "legStrengthReduction");
        }
      }
    }
    {
      uint32_t numSpineStrengthReduction = calculateNumSpineStrengthReduction();
      for (uint32_t i=0; i<numSpineStrengthReduction; i++)
      {
        if (getSpineStrengthReductionImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(spineStrengthReduction[i]), "spineStrengthReduction");
        }
      }
    }
    if (getWholeBodyControlCompensationReductionImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(wholeBodyControlCompensationReduction), "wholeBodyControlCompensationReduction");
    }
    {
      uint32_t numArmControlCompensationReduction = calculateNumArmControlCompensationReduction();
      for (uint32_t i=0; i<numArmControlCompensationReduction; i++)
      {
        if (getArmControlCompensationReductionImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(armControlCompensationReduction[i]), "armControlCompensationReduction");
        }
      }
    }
    {
      uint32_t numHeadControlCompensationReduction = calculateNumHeadControlCompensationReduction();
      for (uint32_t i=0; i<numHeadControlCompensationReduction; i++)
      {
        if (getHeadControlCompensationReductionImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(headControlCompensationReduction[i]), "headControlCompensationReduction");
        }
      }
    }
    {
      uint32_t numLegControlCompensationReduction = calculateNumLegControlCompensationReduction();
      for (uint32_t i=0; i<numLegControlCompensationReduction; i++)
      {
        if (getLegControlCompensationReductionImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(legControlCompensationReduction[i]), "legControlCompensationReduction");
        }
      }
    }
    {
      uint32_t numSpineControlCompensationReduction = calculateNumSpineControlCompensationReduction();
      for (uint32_t i=0; i<numSpineControlCompensationReduction; i++)
      {
        if (getSpineControlCompensationReductionImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(spineControlCompensationReduction[i]), "spineControlCompensationReduction");
        }
      }
    }
    if (getWholeBodyExternalComplianceReductionImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(wholeBodyExternalComplianceReduction), "wholeBodyExternalComplianceReduction");
    }
    {
      uint32_t numArmExternalComplianceReduction = calculateNumArmExternalComplianceReduction();
      for (uint32_t i=0; i<numArmExternalComplianceReduction; i++)
      {
        if (getArmExternalComplianceReductionImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(armExternalComplianceReduction[i]), "armExternalComplianceReduction");
        }
      }
    }
    {
      uint32_t numHeadExternalComplianceReduction = calculateNumHeadExternalComplianceReduction();
      for (uint32_t i=0; i<numHeadExternalComplianceReduction; i++)
      {
        if (getHeadExternalComplianceReductionImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(headExternalComplianceReduction[i]), "headExternalComplianceReduction");
        }
      }
    }
    {
      uint32_t numLegExternalComplianceReduction = calculateNumLegExternalComplianceReduction();
      for (uint32_t i=0; i<numLegExternalComplianceReduction; i++)
      {
        if (getLegExternalComplianceReductionImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(legExternalComplianceReduction[i]), "legExternalComplianceReduction");
        }
      }
    }
    {
      uint32_t numSpineExternalComplianceReduction = calculateNumSpineExternalComplianceReduction();
      for (uint32_t i=0; i<numSpineExternalComplianceReduction; i++)
      {
        if (getSpineExternalComplianceReductionImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(spineExternalComplianceReduction[i]), "spineExternalComplianceReduction");
        }
      }
    }
    if (getWholeBodyDampingRatioReductionImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(wholeBodyDampingRatioReduction), "wholeBodyDampingRatioReduction");
    }
    {
      uint32_t numArmDampingRatioReduction = calculateNumArmDampingRatioReduction();
      for (uint32_t i=0; i<numArmDampingRatioReduction; i++)
      {
        if (getArmDampingRatioReductionImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(armDampingRatioReduction[i]), "armDampingRatioReduction");
        }
      }
    }
    {
      uint32_t numHeadDampingRatioReduction = calculateNumHeadDampingRatioReduction();
      for (uint32_t i=0; i<numHeadDampingRatioReduction; i++)
      {
        if (getHeadDampingRatioReductionImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(headDampingRatioReduction[i]), "headDampingRatioReduction");
        }
      }
    }
    {
      uint32_t numLegDampingRatioReduction = calculateNumLegDampingRatioReduction();
      for (uint32_t i=0; i<numLegDampingRatioReduction; i++)
      {
        if (getLegDampingRatioReductionImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(legDampingRatioReduction[i]), "legDampingRatioReduction");
        }
      }
    }
    {
      uint32_t numSpineDampingRatioReduction = calculateNumSpineDampingRatioReduction();
      for (uint32_t i=0; i<numSpineDampingRatioReduction; i++)
      {
        if (getSpineDampingRatioReductionImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(spineDampingRatioReduction[i]), "spineDampingRatioReduction");
        }
      }
    }
    if (getWholeBodySoftLimitStiffnessReductionImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(wholeBodySoftLimitStiffnessReduction), "wholeBodySoftLimitStiffnessReduction");
    }
    {
      uint32_t numArmSoftLimitStiffnessReduction = calculateNumArmSoftLimitStiffnessReduction();
      for (uint32_t i=0; i<numArmSoftLimitStiffnessReduction; i++)
      {
        if (getArmSoftLimitStiffnessReductionImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(armSoftLimitStiffnessReduction[i]), "armSoftLimitStiffnessReduction");
        }
      }
    }
    {
      uint32_t numHeadSoftLimitStiffnessReduction = calculateNumHeadSoftLimitStiffnessReduction();
      for (uint32_t i=0; i<numHeadSoftLimitStiffnessReduction; i++)
      {
        if (getHeadSoftLimitStiffnessReductionImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(headSoftLimitStiffnessReduction[i]), "headSoftLimitStiffnessReduction");
        }
      }
    }
    {
      uint32_t numLegSoftLimitStiffnessReduction = calculateNumLegSoftLimitStiffnessReduction();
      for (uint32_t i=0; i<numLegSoftLimitStiffnessReduction; i++)
      {
        if (getLegSoftLimitStiffnessReductionImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(legSoftLimitStiffnessReduction[i]), "legSoftLimitStiffnessReduction");
        }
      }
    }
    {
      uint32_t numSpineSoftLimitStiffnessReduction = calculateNumSpineSoftLimitStiffnessReduction();
      for (uint32_t i=0; i<numSpineSoftLimitStiffnessReduction; i++)
      {
        if (getSpineSoftLimitStiffnessReductionImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(spineSoftLimitStiffnessReduction[i]), "spineSoftLimitStiffnessReduction");
        }
      }
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_CHARACTERISTICSBEHAVIOURINTERFACE_DATA_H

