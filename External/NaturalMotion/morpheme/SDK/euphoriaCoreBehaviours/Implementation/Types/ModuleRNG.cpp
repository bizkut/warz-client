/*
 * Copyright (c) 2011 NaturalMotion Ltd. All rights reserved.
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

#include "Types/ModuleRNG.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

ModuleRNG::ModuleRNG()
{
  setSeed(0x5bd1e995);
}

void ModuleRNG::setSeed(uint32_t seed)
{
  NM_ASSERT_COMPILE_TIME(ModuleRNG::eMaxRNGs == 9);

  m_rng[0].setSeed(1893513180 ^ seed);
  m_rng[1].setSeed(1517746329 ^ seed);
  m_rng[2].setSeed(1447497129 ^ seed);
  m_rng[3].setSeed(1781943330 ^ seed);
  m_rng[4].setSeed(1655692410 ^ seed);
  m_rng[5].setSeed(1967773755 ^ seed);
  m_rng[6].setSeed(1606218150 ^ seed);
  m_rng[7].setSeed(1075433238 ^ seed);
  m_rng[8].setSeed(1557985959 ^ seed);
}

uint32_t ModuleRNG::genUInt32(uint32_t aIndex)
{
  NMP_ASSERT(aIndex < ModuleRNG::eMaxRNGs);
  return m_rng[aIndex].genUInt32();
}

float ModuleRNG::genFloat(uint32_t aIndex)
{
  NMP_ASSERT(aIndex < ModuleRNG::eMaxRNGs);
  return m_rng[aIndex].genFloat();
}

float ModuleRNG::genFloat(float minfv, float maxfv, uint32_t aIndex)
{
  NMP_ASSERT(aIndex < ModuleRNG::eMaxRNGs);
  return m_rng[aIndex].genFloat(minfv, maxfv);
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE
