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

#ifndef NM_MDF_BRACECHOOSER_DATA_H
#define NM_MDF_BRACECHOOSER_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/BraceChooser.module"

// known types
#include "Types/BraceState.h"

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
 * \class BraceChooserOutputs
 * \ingroup BraceChooser
 * \brief Outputs this module takes care of choosing which leg(s) to brace with
 *  based on which one is closer. It is designed to work on n legs and the result depends on the masses 
 *  a character with heavy legs and a small body mass will brace with just the closest leg
 *  a character with light legs and a heavy body will usually brace with all its legs
 *  This module could also be used for arms, and for deciding which legs to step with
 *  Lastly, there is scope to improve this by shanging how the incoming data is calculated
 *
 * Data packet definition (10 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BraceChooserOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BraceChooserOutputs));
  }

  enum { maxShouldBrace = 2 };
  NM_INLINE unsigned int getMaxShouldBrace() const { return maxShouldBrace; }
  NM_INLINE void setShouldBrace(unsigned int number, const bool* _shouldBrace, float shouldBrace_importance = 1.f)
  {
    NMP_ASSERT(number <= maxShouldBrace);
    NMP_ASSERT(shouldBrace_importance >= 0.0f && shouldBrace_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      shouldBrace[i] = _shouldBrace[i];
      m_shouldBraceImportance[i] = shouldBrace_importance;
    }
  }
  NM_INLINE void setShouldBraceAt(unsigned int index, const bool& _shouldBrace, float shouldBrace_importance = 1.f)
  {
    NMP_ASSERT(index < maxShouldBrace);
    shouldBrace[index] = _shouldBrace;
    NMP_ASSERT(shouldBrace_importance >= 0.0f && shouldBrace_importance <= 1.0f);
    m_shouldBraceImportance[index] = shouldBrace_importance;
  }
  NM_INLINE float getShouldBraceImportance(int index) const { return m_shouldBraceImportance[index]; }
  NM_INLINE const float& getShouldBraceImportanceRef(int index) const { return m_shouldBraceImportance[index]; }
  NM_INLINE unsigned int calculateNumShouldBrace() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_shouldBraceImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const bool& getShouldBrace(unsigned int index) const { NMP_ASSERT(index <= maxShouldBrace); return shouldBrace[index]; }

protected:

  bool shouldBrace[NetworkConstants::networkMaxNumLegs];  ///< Outputs whether each limb should brace

  // importance values
  float
     m_shouldBraceImportance[NetworkConstants::networkMaxNumLegs];

  friend class BraceChooser_Con;

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
 * \class BraceChooserFeedbackInputs
 * \ingroup BraceChooser
 * \brief FeedbackInputs this module takes care of choosing which leg(s) to brace with
 *  based on which one is closer. It is designed to work on n legs and the result depends on the masses 
 *  a character with heavy legs and a small body mass will brace with just the closest leg
 *  a character with light legs and a heavy body will usually brace with all its legs
 *  This module could also be used for arms, and for deciding which legs to step with
 *  Lastly, there is scope to improve this by shanging how the incoming data is calculated
 *
 * Data packet definition (72 bytes, 96 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BraceChooserFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BraceChooserFeedbackInputs));
  }

  enum { maxBraceStates = 2 };
  NM_INLINE unsigned int getMaxBraceStates() const { return maxBraceStates; }
  NM_INLINE void setBraceStates(unsigned int number, const BraceState* _braceStates, float braceStates_importance = 1.f)
  {
    NMP_ASSERT(number <= maxBraceStates);
    NMP_ASSERT(braceStates_importance >= 0.0f && braceStates_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      braceStates[i] = _braceStates[i];
      postAssignBraceStates(braceStates[i]);
      m_braceStatesImportance[i] = braceStates_importance;
    }
  }
  NM_INLINE void setBraceStatesAt(unsigned int index, const BraceState& _braceStates, float braceStates_importance = 1.f)
  {
    NMP_ASSERT(index < maxBraceStates);
    braceStates[index] = _braceStates;
    NMP_ASSERT(braceStates_importance >= 0.0f && braceStates_importance <= 1.0f);
    postAssignBraceStates(braceStates[index]);
    m_braceStatesImportance[index] = braceStates_importance;
  }
  NM_INLINE float getBraceStatesImportance(int index) const { return m_braceStatesImportance[index]; }
  NM_INLINE const float& getBraceStatesImportanceRef(int index) const { return m_braceStatesImportance[index]; }
  NM_INLINE unsigned int calculateNumBraceStates() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_braceStatesImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const BraceState& getBraceStates(unsigned int index) const { NMP_ASSERT(index <= maxBraceStates); return braceStates[index]; }

protected:

  BraceState braceStates[NetworkConstants::networkMaxNumLegs];  ///< Pose of each limb which determines its fitness for bracing

  // post-assignment stubs
  NM_INLINE void postAssignBraceStates(const BraceState& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_braceStatesImportance[NetworkConstants::networkMaxNumLegs];

  friend class BraceChooser_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    {
      uint32_t numBraceStates = calculateNumBraceStates();
      for (uint32_t i=0; i<numBraceStates; i++)
      {
        if (getBraceStatesImportance(i) > 0.0f)
          braceStates[i].validate();
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

#endif // NM_MDF_BRACECHOOSER_DATA_H

