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

#ifndef NM_MDF_TYPE_MODULERNG_H
#define NM_MDF_TYPE_MODULERNG_H

// include definition file to create project dependency
#include "./Definition/Types/Externs.types"

// external types
#include "NMPlatform/NMRNG.h"

// for combiners
#include "euphoria/erJunction.h"

// constants
#include "NetworkConstants.h"

// hook functions
#include "euphoria/erValueValidators.h"
#include "euphoria/erValuePostCombiners.h"

#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// Declaration from 'Externs.types'
// Data Payload: 72 Bytes
// Modifiers: __no_combiner__  

// Multiply-with-carry (MWCG) RNG for module usage

struct ModuleRNG
{


  // functions

  ModuleRNG();
  void setSeed(uint32_t seed);
  float genFloat(uint32_t aIndex=0);
  float genFloat(float minfv, float maxfv, uint32_t aIndex=0);
  uint32_t genUInt32(uint32_t aIndex=0);

private:


  enum Constants
  {
    /*  9 */ eMaxRNGs = 9,  
  };

  NMP::RNG m_rng[ModuleRNG::eMaxRNGs];


public:


}; // struct ModuleRNG


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_MODULERNG_H

