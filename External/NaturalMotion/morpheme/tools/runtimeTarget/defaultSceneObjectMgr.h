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
#ifndef NM_DEFAULTSCENEOBJECTMGR_H
#define NM_DEFAULTSCENEOBJECTMGR_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/runtimeTargetInterface.h"
//----------------------------------------------------------------------------------------------------------------------

class DefaultPhysicsMgr;
class NetworkInstanceRecordManager;
class RuntimeTargetContext;
class SceneObjectRecordManager;

//----------------------------------------------------------------------------------------------------------------------
/// \class DefaultSceneObjectMgr
/// \brief Implements MCOMMS::SceneObjectManagementInterface.
/// \ingroup RuntimeTarget
//----------------------------------------------------------------------------------------------------------------------
class DefaultSceneObjectMgr :
  public MCOMMS::SceneObjectManagementInterface
{
public:

  DefaultSceneObjectMgr(RuntimeTargetContext* context, DefaultPhysicsMgr* physicsManager);
  virtual ~DefaultSceneObjectMgr() {}

  /// \brief This custom assignment operator is required as the compiler cannot generate the default assignment operator
  ///  due to const members.
  DefaultSceneObjectMgr & operator= (const DefaultSceneObjectMgr & other) {*this = other; return *this;}

  MCOMMS::SceneObjectID getUnusedSceneObjectID();

  // [SceneObjectManagementInterface]
public:

  virtual bool canCreateSceneObjects() const NM_OVERRIDE { return true; }
  virtual bool canEditSceneObjects() const NM_OVERRIDE { return true; }

  virtual uint32_t getNumSceneObjects() const NM_OVERRIDE;

  virtual MCOMMS::SceneObject* createSceneObject(
    uint32_t numAttributes,
    const MCOMMS::Attribute* const* attributes,
    MCOMMS::Connection* owner,
    MCOMMS::SceneObject *&createdCharacterController) NM_OVERRIDE;

  virtual bool destroySceneObject(MCOMMS::SceneObjectID objectID, MCOMMS::SceneObjectID& destroyedCharacterController) NM_OVERRIDE;

  virtual uint32_t getSceneObjects(MCOMMS::SceneObject** objects, uint32_t maxObjects) const NM_OVERRIDE;
  virtual MCOMMS::SceneObject* getSceneObject(MCOMMS::SceneObjectID objectID) const NM_OVERRIDE;

  virtual void onConnectionClosed(MCOMMS::Connection* connection) NM_OVERRIDE;

private:

  SceneObjectRecordManager* const             m_sceneObjectManager;
  const NetworkInstanceRecordManager* const   m_networkInstanceManager;
  DefaultPhysicsMgr* const                    m_physicsManager;
};

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_DEFAULTSCENEOBJECTMGR_H
//----------------------------------------------------------------------------------------------------------------------
