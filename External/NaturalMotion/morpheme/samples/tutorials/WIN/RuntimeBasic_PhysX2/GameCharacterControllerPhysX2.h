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
#ifndef GAME_CHARACTER_CONTROLLER_PHYSX2_H
#define GAME_CHARACTER_CONTROLLER_PHYSX2_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMQuat.h"
#include "morpheme/mrCharacterControllerDef.h"
#include "physics/PhysX2/mrCharacterControllerInterfacePhysX2.h"
#include "morpheme/mrNetwork.h"

#include <map>

#include "mrPhysX2Includes.h"

#include "morpheme/mrCoreTaskIDs.h"
#include "physics/mrPhysicsRigDef.h"
#include "physics/mrPhysicsRigDef.h"
#include "physics/PhysX2/mrPhysicsRigPhysX2.h"
#include "physics/PhysX2/mrPhysicsScenePhysX2.h"
//----------------------------------------------------------------------------------------------------------------------

#ifdef NM_COMPILER_MSVC
#pragma warning (push)
#pragma warning (disable : 4324)
#endif

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
// Control Parameters for the character controller
struct ControllerParameters
{
  ControllerParameters() :
    m_height(0.0f),
    m_radius(0.0f),
    m_skinWidth(0.0f),
    m_stepHeight(0.0f),
    m_maxPushForce(0.0f), 
    m_maxSlopeAngle(0.0f)
  {
  };

  float m_height;
  float m_radius;
  float m_skinWidth;
  float m_stepHeight;
  float m_maxPushForce;
  float m_maxSlopeAngle;
};

typedef std::map<NxActor*, float> TouchTimes;

//----------------------------------------------------------------------------------------------------------------------
// Hit report for the character controller
//----------------------------------------------------------------------------------------------------------------------
class ControllerHitReport : public NxUserControllerHitReport
{
public:

  ControllerHitReport():
    m_maxPushForce(0),
    m_dt(0),
    m_maxTouchTime(0)
  {
  };

  ControllerHitReport(float maxTouchTime, TouchTimes& touchTimes);

  void updatePreMove(float dt, float maxPushForce);

  NxControllerAction onShapeHit(const NxControllerShapeHit& hit);
  NxControllerAction onControllerHit(const NxControllersHit& NMP_UNUSED(hit));

  float      m_maxPushForce;
  float      m_dt;
  float      m_maxTouchTime;
  TouchTimes m_touchTimes;
};

//----------------------------------------------------------------------------------------------------------------------
// Game::PhysX2CharacterController
// morpheme physics character controller example class. This class deals with all the physics related character
// controller information and takes over for the Character for updating the network instance.
//----------------------------------------------------------------------------------------------------------------------
class CharacterControllerPhysX2Basic : public MR::CharacterControllerInterfacePhysX2
{
public:

  ~CharacterControllerPhysX2Basic() {};

  //----------------------------
  // Allocate and initialise a PhysX2CharacterController instance.
  static CharacterControllerPhysX2Basic* create(  
    MR::Network* net,
    MR::PhysicsScenePhysX2* physicsScene,
    NxControllerManager* controllerManager);

  //----------------------------
  // Release a PhysX2CharacterController instance.
  static void destroy(CharacterControllerPhysX2Basic* characterController);  

  //----------------------------
  // Initialise the character controller
  bool init(
    MR::Network*                network,
    MR::PhysicsScenePhysX2*     physicsScene,
    NxControllerManager*        controllerManager);

  //----------------------------
  // Controller update stages.
  //----------------------------
  // A simple update implementation for a physical character controller.
  // For a fuller implementation see Game::CharacterControllerPhysX2 in the Game Management library.
  //----------------------------

  //----------------------------
  // Indicate that a character controller update is about to start
  //  and start setting up parameters before the actual update.
  void updatePreController();

  //----------------------------
  // Actually update the physics character controller (m_nxControllerManager).
  //
  // If the network root pos is being controlled by animation, 
  // apply the cached recommended transform delta to the physics systems character controller.
  // This function also calls the global update of the physx character controller.
  void updateController(float deltaTime);

  //----------------------------
  /// If the network root pos is being controlled by the physical rig pos,
  ///  update the character controllers position from its position.
  ///
  /// To be called after the full network update.
  /// Sets the world position of the character controller from the Networks character root transform change
  /// (driven from the physics rig position).
  void updatePostController();

  //----------------------------
  // Accessors
  bool getCollisionEnabled();
  void enableCollision(bool enable);

protected:

  CharacterControllerPhysX2Basic();

  //----------------------------
  // Release any memory allocated by this class
  bool term();

  bool isNetworkPhysical(const MR::Network* network);

protected:

  NMP::Vector3 m_positionWorld;               ///< The characters current world position from this frames update.
  NMP::Quat    m_orientationWorld;            ///< The characters current world orientation from this frames update.
  
  /// Requested movement : This can come from a user requested movement or it can be calculated directly from the Networks 
  ///  suggested delta values after updating its animation phase (up to MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER).
  NMP::Vector3 m_requestedDeltaWorldTranslation;  ///< The desired world space delta translation of this character for this frame.
                                                  ///<  The physical character controller will try to achieve this translation when it is updated,
                                                  ///<  but it may be resisted by solid objects or altered by falling under gravity.
  NMP::Quat    m_requestedDeltaWorldOrientation;  ///< The desired world space orientation of this character for this frame.
                                                  ///<  Most physical character controller implementations will not restrict rotation that does not
                                                  ///<  change the world profile of the collision capsule (i.e. rotation about the capsule vertical axis).

  /// Actual movement: After the final stage of controller update, the transformation that was actually achieved is stored here.
  ///  During controller update working values are stored here.
  NMP::Vector3 m_actualDeltaWorldTranslation;   ///< The delta translation that the character archived this frame.
  NMP::Quat    m_actualDeltaWorldOrientation;   ///< The delta orientation that the character archived this frame.

  ControllerHitReport* m_hitReport;
  bool                 m_nxControllerEnabled;   ///< Is the physical character controller object enabled.
  NxController*        m_nxController;          ///< The physics systems collidable character controller object.
  NxControllerManager* m_nxControllerManager;   ///< Reference to the PhysX2 global controller manager.

  MR::CharacterControllerDef m_controllerDef; ///< The definition of the physical character controllers properties.
                                              ///<  For example shape, size, friction etc.
  MR::Network*            m_net;            ///< That this character controller is associated with.
  MR::PhysicsRig*         m_physicsRig;     ///< The physics rig instance associated with this character controller and Network.

  TouchTimes m_touchTimes;   ///< How long we've touched different objects,

  float m_controllerOffset;
};


} // namespace Game


#ifdef NM_COMPILER_MSVC
#pragma warning (pop)
#endif

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_CHARACTER_CONTROLLER_PHYSX2_H
//----------------------------------------------------------------------------------------------------------------------
