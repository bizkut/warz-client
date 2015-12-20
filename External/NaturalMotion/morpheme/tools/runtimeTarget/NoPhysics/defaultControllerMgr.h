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
  #pragma warning (disable: 4324)
#endif
#ifndef NM_DEFAULTCCM_H
#define NM_DEFAULTCCM_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVector3.h"
#include "morpheme/mrCharacterControllerInterfaceBase.h"
#include "morpheme/mrCharacterControllerDef.h"
#include "../iControllerMgr.h"
#include "../runtimeTargetContext.h"
#include "morpheme/mrNetwork.h"
#include <map>
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{
class Connection;
}

//----------------------------------------------------------------------------------------------------------------------
/// \class CCMRecord
/// \brief Contains information describing the physical character controller associated with a character
/// \ingroup RuntimeTarget
//----------------------------------------------------------------------------------------------------------------------
class ControllerRecord : public MR::CharacterControllerInterfaceBase
{
public:
  ControllerRecord();
  virtual ~ControllerRecord();

  NMP::Vector3 m_requestedMovement; // in physics units
  NMP::Vector3 m_characterPosition; // in physics units
  NMP::Vector3 m_characterPositionOld;
  NMP::Quat    m_characterOrientation;

  bool         m_controllerEnabled;
  bool         m_onGround;
  bool         m_onGroundOld;
  float        m_groundContactTime;

  MR::CharacterControllerDef m_controllerParams; ///< This is a copy of the character controller def in the MR::Network, which has
                                                 ///   been scaled as required by any node-specific data.
  NMP::Vector3               m_controllerOffset;

  // The following were taken from PhysicsRecord
  MR::Network*       m_network;
  float              m_characterVerticalVel;
  float              m_characterVerticalVelDt;
  NMP::Quat          m_characterOrientationOld;
  NMP::Vector3       m_deltaTranslation; // in local space in animation units
  NMP::Quat          m_deltaOrientation; // in local space

  MCOMMS::SceneObject* m_connectRepresentation; // The graphical object for this CC in connect.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class DefaultCharacterControllerMgr
/// \brief Implements the IControllerMgr interface
/// \ingroup RuntimeTarget
//----------------------------------------------------------------------------------------------------------------------
class DefaultControllerMgr : public IControllerMgr
{
public:

  DefaultControllerMgr(class DefaultPhysicsMgr* physicsManager, RuntimeTargetContext* context);
  virtual ~DefaultControllerMgr();

  /// \brief This custom assignment operator is required as the compiler cannot generate the default assignment operator
  ///  due to const members.
  DefaultControllerMgr & operator= (const DefaultControllerMgr & other) {*this = other; return *this;}

  /// Create a new controller, with an optional default set of parameters
  void createControllerRecord(
    MCOMMS::InstanceID  instanceID,
    MR::Network*        const network,
    const NMP::Vector3& characterStartPosition,
    const NMP::Quat&    characterStartRotation);

  void updateControllerRecord(MCOMMS::InstanceID instanceID);

  /// Remove controller (and physics rig) from a network
  void destroyControllerRecord(MCOMMS::InstanceID instanceID);

  /// Switch off collision for controller, given network instance record
  void disableCollision(MCOMMS::InstanceID instanceID);

  /// Switch on collision for controller, given network instance record
  void enableCollision(MCOMMS::InstanceID instanceID);

  /// Rescale properties of controller (for instance if a character kneels down, it needs a shorter capsule)
  void scaleControllerProperties(
    MCOMMS::InstanceID instanceID,
    float              radiusFraction,
    float              heightFraction);

  /// Get whether collision is enabled for controller
  bool getCollisionEnabled(MCOMMS::InstanceID instanceID) const;

  /// Get whether controller is on ground, given network instance ID
  bool getOnGround(MCOMMS::InstanceID instanceID) const;

  /// Set requested controller movement (i.e. from animation stream) given network instance ID
  void setRequestedMovement(MCOMMS::InstanceID instanceID, const NMP::Vector3& delta);

  /// Set controller position given network instance ID
  void setPosition(MCOMMS::InstanceID instanceID, const NMP::Vector3& pos);

  /// Get controller position given network instance ID
  NMP::Vector3 getPosition(MCOMMS::InstanceID instanceID) const;

  /// Get change in controller position given network instance ID
  NMP::Vector3 getPositionDelta(MCOMMS::InstanceID instanceID) const;

  /// Get change in controller orientation given network instance ID
  NMP::Quat getOrientationDelta(MCOMMS::InstanceID instanceID) const;

  /// Set controller height given network instance ID
  void setOrientation(MCOMMS::InstanceID instanceID, const NMP::Quat& ori);

  /// Get controller orientation given network instance ID
  NMP::Quat getOrientation(MCOMMS::InstanceID instanceID) const;

  /// Update all controllers
  void updateControllers(float dt);

  void updateInstanceNetworksBegin(float dt);
  void continueNetworkUpdate(MR::Network* network, MR::TaskID taskID);
  void updateInstanceNetworksContinue(MR::TaskID taskID);
  void updateInstanceNetworksEnd();

  /// Update all controller instances connectivity
  void updateInstancesConnectivity(float dt);

  /// Update all controller instances pre-controller
  void updateInstancesPreController(float delta);

  /// Update all controller instances pre-physics
  void updateInstancesPrePhysics(float delta);

  /// Update all controller instances post physics
  void updateInstancesPostPhysics(float delta);

#if defined(MR_OUTPUT_DEBUGGING)
  /// Transmit back any debug output timings
  void sendInstanceProfileTimingDebugOutput();
#endif

  /// Get the controller record given the instanceID of the associated network
  ControllerRecord* getControllerRecord(MCOMMS::InstanceID instanceID);

  /// Create a new controller representation given the objectID that represents the character itself.
  MCOMMS::SceneObject* addNewControllerRepresentation(MCOMMS::SceneObjectID objectID, MCOMMS::Connection* owner);

  /// Update representations of character controllers
  void updateControllerRepresentations();

  ///
  void teleport(MCOMMS::InstanceID instanceID, const NMP::PosQuat& rootTransform);

protected:

  typedef std::map<MCOMMS::InstanceID, ControllerRecord*> Records;

  class DefaultPhysicsMgr*    m_physicsManager;
  RuntimeTargetContext*       const m_context;

  Records                     m_records;

  uint32_t                    m_numCompleted;
  uint32_t                    m_numBlocked;

}; // class DefaultCharacterControllerMgr

#endif // #ifdef DEFAULTCCM_H
