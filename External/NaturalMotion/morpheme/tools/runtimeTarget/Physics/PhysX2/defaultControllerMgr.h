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
#include "morpheme/mrCharacterControllerDef.h"
#include "mrCharacterControllerInterfacePhysX2.h"
#include "morpheme/mrNetwork.h"

#include "comms/mcomms.h"
#include "comms/sceneObject.h"

#include "../../iControllerMgr.h"
#include "../../runtimeTargetContext.h"
#include <map>
#include "mrPhysX2Includes.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{
class Connection;
}

typedef std::map<PhysXActor*, float> TouchTimes;

//----------------------------------------------------------------------------------------------------------------------
/// \class ControllerRecord
/// \brief Contains information describing the physical character controller associated with a character.
/// \ingroup LiveLinkRuntimeModule
///
/// This structure can be seen as a game character.
/// It contains everything that is required animate and move a game character in the world.
//----------------------------------------------------------------------------------------------------------------------
class ControllerRecord : public MR::CharacterControllerInterfacePhysX2
{
public:

  ControllerRecord();
  ~ControllerRecord();

  class ControllerHitReport* m_hitReport;
  NxController* m_nxController;
  void*         pad[2];

  /// Requested movement: the position change calculated from the ANIMATION STREAM.  Update controllers
  ///  tries to achieve this movement (but it may be resisted by solid objects or altered by falling
  ///  under gravity)
  NMP::Vector3  m_requestedMovement;              ///< In physics units.
  NMP::Vector3  m_requestedHorizontalMovement;
  NMP::Vector3  m_characterPosition;              ///< In physics units.
  NMP::Quat     m_characterOrientation;

  bool          m_controllerEnabled;
  bool          m_onGround;
  bool          m_onGroundOld;

  NMP::Vector3  m_extraVerticalMove;              ///< Used to keep the character in contact with the ground when
                                                  ///<  going down hill or stepping down. In physics units. 
  MR::CharacterControllerDef m_controllerParams;  ///< A copy of the character controller def in the MR::Network, which has
                                                  ///<  been scaled as required by any node-specific data.
  NMP::Vector3  m_originOffset;                   ///< An offset from the origin of the character controller shape.
                                                  ///<  Physics engine character controller origins are often in the centre of their shape
                                                  ///<  but when placing a character in game we will generally use a ground position.
                                                  ///<  So in order to place the controller correctly in the world we need to know how
                                                  ///<  we should offset the character controller position from the game characters position.
                                                  ///<  Defined in characters local space.
  NMP::Vector3  m_graveyardPos;

  MR::Network*  m_network;
  NMP::Vector3  m_velocityInGravityDirection;
  float         m_velocityInGravityDirectionDt;
  NMP::Quat     m_characterOrientationOld;
  NMP::Vector3  m_characterPositionOld;
  NMP::Vector3  m_deltaTranslation;               ///< In local space in animation units.
  NMP::Quat     m_deltaOrientation;               ///< In local space.
  
  TouchTimes    m_touchTimes;                     ///< How long we've touched different objects, for ramping up forces
  
  MCOMMS::SceneObject* m_connectRepresentation;   ///< The graphical object reflecting this CC in connect.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class DefaultCharacterControllerMgr
/// \brief Implements the IControllerMgr interface
/// \ingroup LiveLinkRuntimeModule
//----------------------------------------------------------------------------------------------------------------------
class DefaultControllerMgr : public IControllerMgr
{
public:

  //--------------------------------------------------------------------------------------------------------------------
  // Functions inherited from IControllerMgr
  //--------------------------------------------------------------------------------------------------------------------

  /// Create a new controller.
  virtual void createControllerRecord(
    MCOMMS::InstanceID  instanceID,
    MR::Network*        const network,
    const NMP::Vector3& characterStartPosition,
    const NMP::Quat&    characterStartRotation);

  /// \brief Update the character controller after an animation set change.
  virtual void updateControllerRecord(MCOMMS::InstanceID instanceID);

  /// Remove controller (and physics rig) from a network.
  virtual void destroyControllerRecord(MCOMMS::InstanceID instanceID);

  /// Set controller position given network instance ID.
  void setPosition(
    MCOMMS::InstanceID  instanceID,
    const NMP::Vector3& pos);

  /// Set controller orientation given network instance ID.
  virtual void setOrientation(
    MCOMMS::InstanceID  instanceID,
    const NMP::Quat&    ori);

  /// Get controller position given network instance ID.
  virtual NMP::Vector3 getPosition(MCOMMS::InstanceID instanceID) const;

  /// Get controller orientation given network instance ID.
  virtual NMP::Quat getOrientation(MCOMMS::InstanceID instanceID) const;

  /// Get change in controller position given network instance ID.
  virtual NMP::Vector3 getPositionDelta(MCOMMS::InstanceID instanceID) const;

  /// Get change in controller orientation given network instance ID.
  virtual NMP::Quat getOrientationDelta(MCOMMS::InstanceID instanceID) const;

  /// Teleport the character to specified transform.
  virtual void teleport(MCOMMS::InstanceID instanceID, const NMP::PosQuat& rootTransform);

  //--------------------------------------------------------------------------------------------------------------------
  // Our own functions
  //--------------------------------------------------------------------------------------------------------------------

  DefaultControllerMgr(class DefaultPhysicsMgr* physicsManager, RuntimeTargetContext* context);
  ~DefaultControllerMgr();

  /// \brief This custom assignment operator is required as the compiler cannot generate the default assignment operator
  ///  due to const members.
  DefaultControllerMgr & operator= (const DefaultControllerMgr & other) {*this = other; return *this;}

  /// Switch off collision for controller, given network instance record.
  void disableCollision(MCOMMS::InstanceID instanceID);

  /// Switch on collision for controller, given network instance record.
  void enableCollision(MCOMMS::InstanceID instanceID);

  /// Get whether collision is enabled for controller.
  bool getCollisionEnabled(MCOMMS::InstanceID instanceID) const;

  /// Rescale properties of controller (for instance if a character kneels down, it needs a shorter capsule).
  void scaleControllerProperties(
    MCOMMS::InstanceID instanceID,
    float              horizontalFraction,
    float              verticalFraction);

  /// Make link between physics rig and controller.
  void makeLinkBetweenPhysicsRigAndController(
    MCOMMS::InstanceID instanceID,
    MR::PhysicsRig*    physicalBody);

  /// Get the PhysX actor associated with controller.
  PhysXActor* getCharacterControllerActor(MCOMMS::InstanceID instanceID);

  /// Get whether controller is on ground, given network instance ID.
  bool getOnGround(MCOMMS::InstanceID instanceID) const;

  /// Get whether controller achieved its requested movement, given network instance ID.
  bool getAchievedRequestedMovement(MCOMMS::InstanceID instanceID) const;

  /// Set requested controller movement (i.e. from animation stream) given network instance ID.
  void setRequestedMovement(
    MCOMMS::InstanceID  instanceID,
    const NMP::Vector3& delta);

  /// Update all controllers.
  void updateControllers(float dt);

  void updateInstanceNetworksBegin(float dt);
  void continueNetworkUpdate(MR::Network* network, MR::TaskID taskID);
  void updateInstanceNetworksContinue(MR::TaskID taskID);
  void updateInstanceNetworksEnd();

  /// Update all controller instances connections.
  void updateInstancesConnections(float dt);

  /// Update all controller instances pre physics controller update.
  void updateInstancesPreController(float delta);

  /// Update all controller instances pre physics update.
  void updateInstancesPrePhysics(float delta);

  /// Update all controller instances post physics update.
  void updateInstancesPostPhysicsInit(float delta);
  void updateInstancesPostPhysics(float delta);

  /// Transmit back any debug output timings
  void sendInstanceProfileTimingDebugOutput();

  // Get controller record for a given instance ID.
  ControllerRecord* getControllerRecord(MCOMMS::InstanceID instanceID);

  /// Create a new controller representation given the objectID that represents the character itself.
  MCOMMS::SceneObject* addNewControllerRepresentation(MCOMMS::SceneObjectID objectID, MCOMMS::Connection* owner);

  /// Update representations of character controllers.
  void updateControllerRepresentations();

  void spaceGraveyard(NMP::Vector3& graveyardPos, float factor);

protected:

  /// \brief Calculate the offset from the origin of the character controller shape.
  ///
  /// In setting the position of the character controller in PhysX you are setting the center of the capsule or box,
  /// but when placing a character in game we will generally use a ground position.
  /// So in order to place the controller correctly in the world we need to know how
  /// we should offset the character controller position from the game characters position.
  ///
  /// With this code we are assuming that the origin of the capsule is at its center and that we want to offset the origin to its base.
  /// Note: We have to calculate this value dynamically rather than using an offset value generated by connect when using a capsule.
  /// This is because if we dynamically scale the height of the character controller we cannot just scale an offset. 
  /// The offset would include the radius of the bottom sphere of the capsule, but this is not actually scaled when scaling height.
  NMP::Vector3 calculateOriginOffset(
    ControllerRecord*   ccRecord,
    const NMP::Vector3& worldUpDirection);

  /// \brief Applying any overridden character controller properties that have been set this frame.
  void applyOverriddenProperties(
    MCOMMS::InstanceID instanceID,
    ControllerRecord*  ccRecord);

  /// \brief Compute the controllers offset from the character root.
  ///
  /// The local space offset from the character root + the local space externally set offset
  ///  all multiplied by the characters current orientation.
  NMP::Vector3 computeWorldSpaceCCOriginOffset(ControllerRecord* ccRecord);


  typedef std::map<MCOMMS::InstanceID, ControllerRecord*> Records;

  class DefaultPhysicsMgr* m_physicsManager;
  RuntimeTargetContext*    const m_context;
  NxControllerManager*     m_manager;
  Records                  m_ccRecords;  ///< Map of character controllers to instance IDs.

  uint32_t                 m_graveSeed;

  uint32_t                 m_numCompleted;
  uint32_t                 m_numBlocked;

}; // class DefaultControllerMgr

#endif // #ifdef DEFAULTCCM_H

//----------------------------------------------------------------------------------------------------------------------
