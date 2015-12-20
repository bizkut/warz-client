// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef NM_EUPHORIA_USER_DATA_H
#define NM_EUPHORIA_USER_DATA_H

#include "physics/mrPhysicsRig.h"
#include "NMPlatform/NMHashMap.h"
#include "erDebugDraw.h"

namespace physx
{
class PxShape;
class PxActor;
}

namespace MR
{
struct PhysicsSerialisationBuffer;
}

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
#define INITIAL_RIGPARTMAP_SIZE 256

/// This euphoria data structure is hashed to a physics rig part
struct EuphoriaRigPartUserData
{
  static void initialiseMap()
  {
    NMP_ASSERT_MSG(s_rpMapAllocator == NULL && s_rigPartToDataMap == NULL, "EuphoriaRigPartUserData map already initialised.");

    NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(NMP::HeapAllocator::getMemoryRequirements());
    s_rpMapAllocator = NMP::HeapAllocator::init(resource);

    NMP::Memory::Format mapFormat(sizeof(RigPartToDataMap));
    NMP::Memory::Resource mapResource = NMPMemoryAllocateFromFormat(mapFormat);
    s_rigPartToDataMap = RigPartToDataMap::init(mapResource, INITIAL_RIGPARTMAP_SIZE, s_rpMapAllocator);
    NMP_ASSERT(mapResource.format.size == 0);
  }

  static void destroyMap()
  {
    if (s_rigPartToDataMap)
    {
      s_rigPartToDataMap->~RigPartToDataMap();
      NMP::Memory::memFree(s_rigPartToDataMap);
      s_rigPartToDataMap = NULL;
    }

    if (s_rpMapAllocator)
    {
      s_rpMapAllocator->term();
      s_rpMapAllocator->~HeapAllocator();
      NMP::Memory::memFree(s_rpMapAllocator);
      s_rpMapAllocator = NULL;
    }
  }

  static EuphoriaRigPartUserData* create(MR::PhysicsRig::Part* part)
  {
    if (!s_rigPartToDataMap)
      return 0;
    EuphoriaRigPartUserData* data = (EuphoriaRigPartUserData*) NMPMemoryAlloc(sizeof(EuphoriaRigPartUserData));
    NMP_ASSERT(data);
    new (data) EuphoriaRigPartUserData(part);
    return data;
  }

  static void destroy(EuphoriaRigPartUserData* data, MR::PhysicsRig::Part* part)
  {
    if (!s_rigPartToDataMap)
      return;
    if (!data)
      return;
    if (part)
    {
      s_rigPartToDataMap->erase(part);
    }
    data->destroy();
    NMP::Memory::memFree(data);
  }

  static EuphoriaRigPartUserData* getFromPart(const MR::PhysicsRig::Part* const part)
  {
    if (!s_rigPartToDataMap)
      return 0;
    EuphoriaRigPartUserData* pVal = 0;
    s_rigPartToDataMap->find(part, &pVal);
    return pVal;
  }

  /// Returns the impulse-scaled average position of the last non-zero contact
  NMP::Vector3 getLastAveragePosition() const { return m_lastTotalPositionScaled / m_lastTotalImpulseMagnitude; }
  /// Returns the impulse-scaled average velocity of the last non-zero contact
  NMP::Vector3 getLastAverageVelocity() const { return m_lastTotalVelocityScaled / m_lastTotalImpulseMagnitude; }
  /// Returns the impulse-scaled average normal of the last non-zero contact
  NMP::Vector3 getLastAverageNormal() const { return m_lastTotalNormalScaled.getNormalised(); }
  /// Returns the last total impulse applied at the last non-zero contact
  const NMP::Vector3& getLastTotalImpulse() const { return m_lastTotalImpulse; }
  /// Returns the last total impulse magnitude applied at the last non-zero contact
  float getLastTotalImpulseMagnitude() const { return m_lastTotalImpulseMagnitude; }
  /// Returns the last collision ID (which persists until the next contact), which just cast from
  /// the last contacted actor. -1 if there is no info available.
  int64_t getLastCollisionID() const {return m_lastCollisionID;}
  /// Returns the number of contacts that have been recorded in the last physics step
  uint16_t getNumContacts() const {return m_numContacts;}
  /// Returns the shape for the specified contact
  physx::PxShape& getContactedShape(uint16_t contactIndex) const 
  {
    NMP_ASSERT(contactIndex < m_numContacts);
    NMP_ASSERT(m_contactedShapes[contactIndex]);
    return *m_contactedShapes[contactIndex];
  }
  /// Returns a unique ID for the specified contact
  int64_t getContactedShapeID(uint16_t contactIndex) const
  {
    NMP_ASSERT(contactIndex < m_numContacts);
    NMP_ASSERT(m_contactedShapes[contactIndex]);
    return (int64_t) (size_t) m_contactedShapes[contactIndex];
  }

  /// Process contact (erContactFeedback.cpp)
  void processData(
    physx::PxActor* contactedActor, 
    physx::PxShape* contactedShape, 
    const NMP::Vector3& point, 
    const NMP::Vector3& normal, 
    float impulseMagnitude);

  /// This prepares the contact info for a new update, without wiping out the old stored data
  void reset()
  {
    m_accumulating = false;
    m_numContacts = 0;
    // Note that we don't clear the last contacted etc data
  }

  /// This is called when a non-zero contact is found, and the contact data needs to be reset to
  /// accumulate it.
  void startNewContact()
  {
    m_lastTotalPositionScaled.setToZero();
    m_lastTotalNormalScaled.setToZero();
    m_lastTotalVelocityScaled.setToZero();
    m_lastTotalImpulse.setToZero();
    m_lastTotalImpulseMagnitude = 1e-10f; // avoid div by zero
    m_lastCollisionID = -1;

    m_accumulating = false;
    m_numContacts = 0;
  }

  /// Free the shape list
  void destroy()
  {
    NMP::Memory::memFree(m_contactedShapes);
  }

  // Data
private:
  // Once set these remain until the next non-zero-impulse contact
  NMP::Vector3  m_lastTotalPositionScaled; // scaled by m_lastTotalImpulseMagnitude
  NMP::Vector3  m_lastTotalNormalScaled;   // m_lastTotalImpulseMagnitude
  NMP::Vector3  m_lastTotalVelocityScaled; // m_lastTotalImpulseMagnitude
  NMP::Vector3  m_lastTotalImpulse;
  float         m_lastTotalImpulseMagnitude;
  int64_t       m_lastCollisionID;

  // These refer to the current update only
  physx::PxShape** m_contactedShapes;
  uint16_t         m_numContacts;
  uint16_t         m_maxNumContacts;
  bool             m_accumulating;

public:
  void*            m_appUserData;

#if defined(MR_OUTPUT_DEBUGGING)
  float           m_red;
  float           m_green;
  float           m_blue;
  float           m_alpha;
#endif // defined(MR_OUTPUT_DEBUGGING)

  // Hash Map
private:
  typedef NMP::hash_map<const MR::PhysicsRig::Part*, EuphoriaRigPartUserData*>  RigPartToDataMap;

  EuphoriaRigPartUserData(MR::PhysicsRig::Part* part) : m_maxNumContacts(1), m_appUserData(0)
  {
    NMP_ASSERT(getFromPart(part) == 0);
    s_rigPartToDataMap->insert(part, this);

    m_contactedShapes = (physx::PxShape**) NMPMemoryAlloc(sizeof(physx::PxShape*)*m_maxNumContacts);
    startNewContact();

#if defined(MR_OUTPUT_DEBUGGING)
    m_red = 1.0f;
    m_green = 1.0f;
    m_blue = 1.0f;
    m_alpha = 0.3f;
#endif
  }

  static RigPartToDataMap*    s_rigPartToDataMap;
  static NMP::HeapAllocator*  s_rpMapAllocator;
};

}

#endif // NM_EUPHORIA_USER_DATA_H
