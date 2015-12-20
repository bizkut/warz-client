// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "euphoria/erHitUtils.h"
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
#include "euphoria/erSharedEnums.h"

//----------------------------------------------------------------------------------------------------------------------
namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
// HitSelectionInfo function defs

void HitSelectionInfo::invalidate()
{
  selectedActor = (PhysXActor*)0;
  limbIndex = partIndex = -1;
  limbType = ER::LimbTypeEnum::L_unknown; // no type
  partType = ER::BodyPartTypeEnum::NumPartTypes; ; // no type
}

bool HitSelectionInfo::isNotBodyHit()
{
  return (limbType >= ER::LimbTypeEnum::L_unknown);
}
bool HitSelectionInfo::isInvalid()
{
  return selectedActor == (void*)0;
}
bool HitSelectionInfo::isBodyHit()
{
  return !isNotBodyHit();
}

void HitSelectionInfo::endianSwap()
{
  NMP::endianSwap(selectedActor);
  NMP::endianSwap(limbIndex);
  NMP::endianSwap(partIndex);
  NMP::endianSwap(rigPartIndex);

  NMP::endianSwap(limbType);
  NMP::endianSwap(partType);
  NMP::endianSwap(hitType);
  NMP::endianSwap(elapsedTime);

  NMP::endianSwap(duration);
  NMP::endianSwap(priority);
  NMP::endianSwap(damage);
  NMP::endianSwap(legStrengthReduction);

  NMP::endianSwap(legStrengthReductionDuration);
  NMP::endianSwap(id);
  for (uint32_t i = 0; i < sizeof(pad) / sizeof(int32_t); ++i)
  {
    NMP::endianSwap(pad[i]);
  }

  // eg. from raycast
  NMP::endianSwap(pointLocal);
  NMP::endianSwap(normalLocal);
  NMP::endianSwap(hitDirLocal);
  NMP::endianSwap(hitDirWorld);
  NMP::endianSwap(sourcePointWorld);
}

//----------------------------------------------------------------------------------------------------------------------
// HSILog - a prioritised stack of HitSelectionInfo

// assigned to the logged item and then incremented each time an hsi is logged
int32_t HSILog::lastID = 0;

//----------------------------------------------------------------------------------------------------------------------
// HSILog function defs

HSILog::HSILog()
{
  clear();
}

//----------------------------------------------------------------------------------------------------------------------
// insert this hsi at an appropriate (priority ordered) place in the array
// returns the log index of the item if stored or -1 if dropped
// (an item may be ignored if the log is full with higher priority entries)
//
int32_t HSILog::logHSI(HitSelectionInfo& hsiToLog)
{
  // search for highest priority item that is less than this one
  int i;
  for (i = 0; i < size; ++i)
  {
    if (hsiToLog.priority >= HSIs[i].priority)
      break;
  }

  // this item is of lower priority than anything currently stored
  if (i == size)
  {
    // store it on the end of the array if we have space or just drop it
    if (size < capacity)
    {
      push_back(hsiToLog);
      return size - 1;
    }
  }
  // this item is of higher or equal priority to the ith item
  else
  {
    insert(i, hsiToLog);
    return i;
  }
  return -1;
}

//----------------------------------------------------------------------------------------------------------------------
// return the index of the item with matching id or -1 if not found
int32_t HSILog::find(int32_t idToMatch)
{
  for (int i = 0; i < size; ++i)
  {
    if (HSIs[i].id == idToMatch)
      return i;
  }
  return -1;
}

//----------------------------------------------------------------------------------------------------------------------
// set all priorities to zero and set size to zero
void HSILog::clear()
{
  for (int i = 0; i < capacity; ++i)
  {
    HSIs[i].priority = 0;
  }
  size = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void HSILog::push_back(HitSelectionInfo& hsiToPushBack)
{
  NMP_ASSERT(size < capacity);
  hsiToPushBack.id = ++lastID;
  HSIs[size] = hsiToPushBack;
  ++size;
}

//----------------------------------------------------------------------------------------------------------------------
void HSILog::insert(int32_t at, HitSelectionInfo& hsiToInsert)
{
  NMP_ASSERT(at < size);
  int32_t last = size < capacity ? size : capacity - 1;
  for (int i = last; i > at; --i)
  {
    HSIs[i] = HSIs[i-1];
  }
  hsiToInsert.id = ++lastID;
  HSIs[at] = hsiToInsert;
  // only increment size if we are still growing
  if (size < capacity)
    ++size;
}

//----------------------------------------------------------------------------------------------------------------------
// removes an element from the log
void HSILog::remove(int32_t toRemove)
{
  for (int32_t i = toRemove; i < size - 1; ++i)
  {
    HSIs[i] = HSIs[i+1];
  }
  --size;
}

//----------------------------------------------------------------------------------------------------------------------
// special aux functions
//
// sum of damage stored in the log
float HSILog::getTotalDamage()
{
  float totalDamage = 0.0f;
  for (int32_t i = 0; i < size; ++i)
  {
    totalDamage += HSIs[i].damage;
  }
  return totalDamage;
}

//----------------------------------------------------------------------------------------------------------------------
// sum of damage to a given limb identified by the limb index
float HSILog::getLimbDamage(int32_t limbIndex)
{
  float limbDamage = 0.0f;
  for (int32_t i = 0; i < size; ++i)
  {
    if (HSIs[i].limbIndex == limbIndex)
      limbDamage += HSIs[i].damage;
  }
  return limbDamage;
}

//----------------------------------------------------------------------------------------------------------------------
// subtracts rateOfHealing * dt, from each item in the log
void HSILog::healDamage(float rateOfHealing, float dt)
{
  for (int32_t i = 0; i < size; ++i)
  {
    HSIs[i].damage -= rateOfHealing * dt;
  }
}

// increments elapsedTime by dt, from each item in the log
void HSILog::updateTimers(float dt)
{
  for (int32_t i = 0; i < size; ++i)
  {
    HSIs[i].elapsedTime += dt;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// removes all items with damage <= 0
void HSILog::removeZeroDamageItems()
{
  for (int32_t i = size - 1; i >= 0; --i)
  {
    if (HSIs[i].damage <= 0)
      remove(i);
  }
}

// removes all items with elapsedTime > duration
void HSILog::removeExpiredItems()
{
  for (int32_t i = size - 1; i >= 0; --i)
  {
    if (HSIs[i].elapsedTime > HSIs[i].duration)
      remove(i);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// helper function used to calculate a ramped weight somewhere on a trapezoidal profile
//
float calcRampedWeight(
  float t, float tStart, float maxWeight,
  float rampDuration, float fullDuration, float derampDuration)
{
  // all over or not started
  if (t < tStart || t >= (tStart + rampDuration + fullDuration + derampDuration))
  {
    return 0.0f;
  }

  // tDerampStart is the end of full duration ie. start of deramp
  float tDerampStart = tStart + rampDuration + fullDuration;

  // deramp i.e. decay to zero
  if (derampDuration > 0.0f && t >= tDerampStart)
  {
    return maxWeight * (1.0f  + NMP::clampValue<float>((tDerampStart - t) / derampDuration, -1.0f, 0.0f));
  }

  // tRampEnd is the end of the ramp and the start of full
  float tRampEnd = tStart + rampDuration;

  // full i.e. return weight == 1
  if (t >= tRampEnd)
  {
    return maxWeight;
  }

  // ramping starts at tStart if we got here then we should be ramping
  NMP_ASSERT(t >= tStart);
  if (rampDuration > 0)
  {
    return NMP::clampValue<float>((t - tStart) / rampDuration, 0.0f, 1.0f) * maxWeight;
  }
  else
  {
    return maxWeight;
  }

}

} // namespace ER
