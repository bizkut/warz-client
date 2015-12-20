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

// module def dependencies
#include "MyNetwork.h"
#include "MyNetworkPackaging.h"

// module children dependencies
#include "CharacteristicsBehaviourInterface.h"
#include "PropertiesBehaviourInterface.h"
#include "EuphoriaRagdollBehaviourInterface.h"
#include "EyesBehaviourInterface.h"
#include "ArmsBraceBehaviourInterface.h"
#include "ArmsPlacementBehaviourInterface.h"
#include "AimBehaviourInterface.h"
#include "HeadDodgeBehaviourInterface.h"
#include "ReachForBodyBehaviourInterface.h"
#include "ReachForWorldBehaviourInterface.h"
#include "AnimateBehaviourInterface.h"
#include "BalanceBehaviourInterface.h"
#include "BalancePoserBehaviourInterface.h"
#include "ProtectBehaviourInterface.h"
#include "HazardAwarenessBehaviourInterface.h"
#include "UserHazardBehaviourInterface.h"
#include "ObserveBehaviourInterface.h"
#include "IdleAwakeBehaviourInterface.h"
#include "LookBehaviourInterface.h"
#include "HeadAvoidBehaviourInterface.h"
#include "ShieldBehaviourInterface.h"
#include "HoldBehaviourInterface.h"
#include "HoldActionBehaviourInterface.h"
#include "FreeFallBehaviourInterface.h"
#include "ArmsWindmillBehaviourInterface.h"
#include "LegsPedalBehaviourInterface.h"
#include "ShieldActionBehaviourInterface.h"
#include "SitBehaviourInterface.h"
#include "WritheBehaviourInterface.h"
#include "RandomLook.h"
#include "HazardManagement.h"
#include "GrabDetection.h"
#include "Grab.h"
#include "HazardResponse.h"
#include "FreeFallManagement.h"
#include "ImpactPredictor.h"
#include "ShieldManagement.h"
#include "BalanceManagement.h"
#include "StaticBalance.h"
#include "SteppingBalance.h"
#include "BalancePoser.h"
#include "EnvironmentAwareness.h"
#include "BodyFrame.h"
#include "SupportPolygon.h"
#include "BodyBalance.h"
#include "SittingBodyBalance.h"
#include "ReachForBody.h"
#include "BalanceAssistant.h"
#include "BodySection.h"
#include "RotateCore.h"
#include "PositionCore.h"
#include "BraceChooser.h"
#include "Arm.h"
#include "ArmGrab.h"
#include "ArmAim.h"
#include "ArmHold.h"
#include "ArmBrace.h"
#include "ArmStandingSupport.h"
#include "ArmHoldingSupport.h"
#include "ArmSittingStep.h"
#include "ArmStep.h"
#include "ArmSpin.h"
#include "ArmSwing.h"
#include "ArmReachForBodyPart.h"
#include "ArmReachForWorld.h"
#include "ArmPlacement.h"
#include "ArmPose.h"
#include "ArmReachReaction.h"
#include "ArmWrithe.h"
#include "Head.h"
#include "HeadEyes.h"
#include "HeadAvoid.h"
#include "HeadDodge.h"
#include "HeadAim.h"
#include "HeadPoint.h"
#include "HeadSupport.h"
#include "HeadPose.h"
#include "Leg.h"
#include "LegBrace.h"
#include "LegStandingSupport.h"
#include "LegSittingSupport.h"
#include "LegStep.h"
#include "LegSwing.h"
#include "LegPose.h"
#include "LegSpin.h"
#include "LegReachReaction.h"
#include "LegWrithe.h"
#include "Spine.h"
#include "SpineSupport.h"
#include "SpinePose.h"
#include "SpineControl.h"
#include "SpineReachReaction.h"
#include "SpineWrithe.h"
#include "SpineAim.h"
#include "SceneProbes.h"
#include "BalanceBehaviourFeedback.h"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/LimbControl.h"

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"
#include "NetworkDescriptor.h"
#include "NetworkDataMemory.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void MyNetworkDataAllocator::init()
{
  m_mem[ER::ModuleDataAllocator::Data] = (unsigned char*)NMPMemoryAllocAligned(NMP::Memory::align(sizeof(DataMemoryData), 128), 128);
  m_offsets[ER::ModuleDataAllocator::Data] = 0;
  m_limits[ER::ModuleDataAllocator::Data] =  NMP::Memory::align(sizeof(DataMemoryData), 128);
  m_mem[ER::ModuleDataAllocator::FeedbackInputs] = (unsigned char*)NMPMemoryAllocAligned(NMP::Memory::align(sizeof(DataMemoryFeedbackInputs), 128), 128);
  m_offsets[ER::ModuleDataAllocator::FeedbackInputs] = 0;
  m_limits[ER::ModuleDataAllocator::FeedbackInputs] =  NMP::Memory::align(sizeof(DataMemoryFeedbackInputs), 128);
  m_mem[ER::ModuleDataAllocator::Inputs] = (unsigned char*)NMPMemoryAllocAligned(NMP::Memory::align(sizeof(DataMemoryInputs), 128), 128);
  m_offsets[ER::ModuleDataAllocator::Inputs] = 0;
  m_limits[ER::ModuleDataAllocator::Inputs] =  NMP::Memory::align(sizeof(DataMemoryInputs), 128);
  m_mem[ER::ModuleDataAllocator::UserInputs] = (unsigned char*)NMPMemoryAllocAligned(NMP::Memory::align(sizeof(DataMemoryUserInputs), 128), 128);
  m_offsets[ER::ModuleDataAllocator::UserInputs] = 0;
  m_limits[ER::ModuleDataAllocator::UserInputs] =  NMP::Memory::align(sizeof(DataMemoryUserInputs), 128);
  m_mem[ER::ModuleDataAllocator::FeedbackOutputs] = (unsigned char*)NMPMemoryAllocAligned(NMP::Memory::align(sizeof(DataMemoryFeedbackOutputs), 128), 128);
  m_offsets[ER::ModuleDataAllocator::FeedbackOutputs] = 0;
  m_limits[ER::ModuleDataAllocator::FeedbackOutputs] =  NMP::Memory::align(sizeof(DataMemoryFeedbackOutputs), 128);
  m_mem[ER::ModuleDataAllocator::Outputs] = (unsigned char*)NMPMemoryAllocAligned(NMP::Memory::align(sizeof(DataMemoryOutputs), 128), 128);
  m_offsets[ER::ModuleDataAllocator::Outputs] = 0;
  m_limits[ER::ModuleDataAllocator::Outputs] =  NMP::Memory::align(sizeof(DataMemoryOutputs), 128);
}
//----------------------------------------------------------------------------------------------------------------------
void* MyNetworkDataAllocator::alloc(ER::ModuleDataAllocator::Section sct, size_t size, size_t alignment)
{
  m_offsets[sct] = (m_offsets[sct] + (alignment - 1)) & ~(alignment - 1);
  unsigned char* mL = &((m_mem[sct])[m_offsets[sct]]);
  m_offsets[sct] += size;
  NMP_ASSERT(m_offsets[sct] <= m_limits[sct]);
  return (void*)mL;
}
//----------------------------------------------------------------------------------------------------------------------
void MyNetworkDataAllocator::clear(ER::ModuleDataAllocator::Section sct)
{
  memset(m_mem[sct], 0, m_limits[sct]);
}
//----------------------------------------------------------------------------------------------------------------------
void MyNetworkDataAllocator::term()
{
  NMP::Memory::memFree( m_mem[ER::ModuleDataAllocator::Data] );
  m_offsets[ER::ModuleDataAllocator::Data] = 0;
  NMP::Memory::memFree( m_mem[ER::ModuleDataAllocator::FeedbackInputs] );
  m_offsets[ER::ModuleDataAllocator::FeedbackInputs] = 0;
  NMP::Memory::memFree( m_mem[ER::ModuleDataAllocator::Inputs] );
  m_offsets[ER::ModuleDataAllocator::Inputs] = 0;
  NMP::Memory::memFree( m_mem[ER::ModuleDataAllocator::UserInputs] );
  m_offsets[ER::ModuleDataAllocator::UserInputs] = 0;
  NMP::Memory::memFree( m_mem[ER::ModuleDataAllocator::FeedbackOutputs] );
  m_offsets[ER::ModuleDataAllocator::FeedbackOutputs] = 0;
  NMP::Memory::memFree( m_mem[ER::ModuleDataAllocator::Outputs] );
  m_offsets[ER::ModuleDataAllocator::Outputs] = 0;
}

//----------------------------------------------------------------------------------------------------------------------
MyNetwork::MyNetwork() : ER::RootModule()
{
  m_mdAllocator.init();
  ER::ModuleDataAllocator* mdAllocator = &m_mdAllocator;

  data = (MyNetworkData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(MyNetworkData), __alignof(MyNetworkData));

  m_childModules[0] = characteristicsBehaviourInterface = (CharacteristicsBehaviourInterface*)NMPMemoryAllocateFromFormat(CharacteristicsBehaviourInterface::getMemoryRequirements()).ptr;
  new (characteristicsBehaviourInterface) CharacteristicsBehaviourInterface(mdAllocator, NULL);

  m_childModules[1] = propertiesBehaviourInterface = (PropertiesBehaviourInterface*)NMPMemoryAllocateFromFormat(PropertiesBehaviourInterface::getMemoryRequirements()).ptr;
  new (propertiesBehaviourInterface) PropertiesBehaviourInterface(mdAllocator, NULL);

  m_childModules[2] = euphoriaRagdollBehaviourInterface = (EuphoriaRagdollBehaviourInterface*)NMPMemoryAllocateFromFormat(EuphoriaRagdollBehaviourInterface::getMemoryRequirements()).ptr;
  new (euphoriaRagdollBehaviourInterface) EuphoriaRagdollBehaviourInterface(mdAllocator, NULL);

  EyesBehaviourInterface_Con* con3 = (EyesBehaviourInterface_Con*)NMPMemoryAllocateFromFormat(EyesBehaviourInterface_Con::getMemoryRequirements()).ptr;
  new(con3) EyesBehaviourInterface_Con(); // set up vtable
  m_childModules[3] = eyesBehaviourInterface = (EyesBehaviourInterface*)NMPMemoryAllocateFromFormat(EyesBehaviourInterface::getMemoryRequirements()).ptr;
  new (eyesBehaviourInterface) EyesBehaviourInterface(mdAllocator, con3);

  m_childModules[4] = armsBraceBehaviourInterface = (ArmsBraceBehaviourInterface*)NMPMemoryAllocateFromFormat(ArmsBraceBehaviourInterface::getMemoryRequirements()).ptr;
  new (armsBraceBehaviourInterface) ArmsBraceBehaviourInterface(mdAllocator, NULL);

  m_childModules[5] = armsPlacementBehaviourInterface = (ArmsPlacementBehaviourInterface*)NMPMemoryAllocateFromFormat(ArmsPlacementBehaviourInterface::getMemoryRequirements()).ptr;
  new (armsPlacementBehaviourInterface) ArmsPlacementBehaviourInterface(mdAllocator, NULL);

  AimBehaviourInterface_Con* con6 = (AimBehaviourInterface_Con*)NMPMemoryAllocateFromFormat(AimBehaviourInterface_Con::getMemoryRequirements()).ptr;
  new(con6) AimBehaviourInterface_Con(); // set up vtable
  m_childModules[6] = aimBehaviourInterface = (AimBehaviourInterface*)NMPMemoryAllocateFromFormat(AimBehaviourInterface::getMemoryRequirements()).ptr;
  new (aimBehaviourInterface) AimBehaviourInterface(mdAllocator, con6);

  m_childModules[7] = headDodgeBehaviourInterface = (HeadDodgeBehaviourInterface*)NMPMemoryAllocateFromFormat(HeadDodgeBehaviourInterface::getMemoryRequirements()).ptr;
  new (headDodgeBehaviourInterface) HeadDodgeBehaviourInterface(mdAllocator, NULL);

  ReachForBodyBehaviourInterface_Con* con8 = (ReachForBodyBehaviourInterface_Con*)NMPMemoryAllocateFromFormat(ReachForBodyBehaviourInterface_Con::getMemoryRequirements()).ptr;
  new(con8) ReachForBodyBehaviourInterface_Con(); // set up vtable
  m_childModules[8] = reachForBodyBehaviourInterface = (ReachForBodyBehaviourInterface*)NMPMemoryAllocateFromFormat(ReachForBodyBehaviourInterface::getMemoryRequirements()).ptr;
  new (reachForBodyBehaviourInterface) ReachForBodyBehaviourInterface(mdAllocator, con8);

  ReachForWorldBehaviourInterface_Con* con9 = (ReachForWorldBehaviourInterface_Con*)NMPMemoryAllocateFromFormat(ReachForWorldBehaviourInterface_Con::getMemoryRequirements()).ptr;
  new(con9) ReachForWorldBehaviourInterface_Con(); // set up vtable
  m_childModules[9] = reachForWorldBehaviourInterface = (ReachForWorldBehaviourInterface*)NMPMemoryAllocateFromFormat(ReachForWorldBehaviourInterface::getMemoryRequirements()).ptr;
  new (reachForWorldBehaviourInterface) ReachForWorldBehaviourInterface(mdAllocator, con9);

  m_childModules[10] = animateBehaviourInterface = (AnimateBehaviourInterface*)NMPMemoryAllocateFromFormat(AnimateBehaviourInterface::getMemoryRequirements()).ptr;
  new (animateBehaviourInterface) AnimateBehaviourInterface(mdAllocator, NULL);

  BalanceBehaviourInterface_Con* con11 = (BalanceBehaviourInterface_Con*)NMPMemoryAllocateFromFormat(BalanceBehaviourInterface_Con::getMemoryRequirements()).ptr;
  new(con11) BalanceBehaviourInterface_Con(); // set up vtable
  m_childModules[11] = balanceBehaviourInterface = (BalanceBehaviourInterface*)NMPMemoryAllocateFromFormat(BalanceBehaviourInterface::getMemoryRequirements()).ptr;
  new (balanceBehaviourInterface) BalanceBehaviourInterface(mdAllocator, con11);

  BalancePoserBehaviourInterface_Con* con12 = (BalancePoserBehaviourInterface_Con*)NMPMemoryAllocateFromFormat(BalancePoserBehaviourInterface_Con::getMemoryRequirements()).ptr;
  new(con12) BalancePoserBehaviourInterface_Con(); // set up vtable
  m_childModules[12] = balancePoserBehaviourInterface = (BalancePoserBehaviourInterface*)NMPMemoryAllocateFromFormat(BalancePoserBehaviourInterface::getMemoryRequirements()).ptr;
  new (balancePoserBehaviourInterface) BalancePoserBehaviourInterface(mdAllocator, con12);

  m_childModules[13] = protectBehaviourInterface = (ProtectBehaviourInterface*)NMPMemoryAllocateFromFormat(ProtectBehaviourInterface::getMemoryRequirements()).ptr;
  new (protectBehaviourInterface) ProtectBehaviourInterface(mdAllocator, NULL);

  HazardAwarenessBehaviourInterface_Con* con14 = (HazardAwarenessBehaviourInterface_Con*)NMPMemoryAllocateFromFormat(HazardAwarenessBehaviourInterface_Con::getMemoryRequirements()).ptr;
  new(con14) HazardAwarenessBehaviourInterface_Con(); // set up vtable
  m_childModules[14] = hazardAwarenessBehaviourInterface = (HazardAwarenessBehaviourInterface*)NMPMemoryAllocateFromFormat(HazardAwarenessBehaviourInterface::getMemoryRequirements()).ptr;
  new (hazardAwarenessBehaviourInterface) HazardAwarenessBehaviourInterface(mdAllocator, con14);

  UserHazardBehaviourInterface_Con* con15 = (UserHazardBehaviourInterface_Con*)NMPMemoryAllocateFromFormat(UserHazardBehaviourInterface_Con::getMemoryRequirements()).ptr;
  new(con15) UserHazardBehaviourInterface_Con(); // set up vtable
  m_childModules[15] = userHazardBehaviourInterface = (UserHazardBehaviourInterface*)NMPMemoryAllocateFromFormat(UserHazardBehaviourInterface::getMemoryRequirements()).ptr;
  new (userHazardBehaviourInterface) UserHazardBehaviourInterface(mdAllocator, con15);

  ObserveBehaviourInterface_Con* con16 = (ObserveBehaviourInterface_Con*)NMPMemoryAllocateFromFormat(ObserveBehaviourInterface_Con::getMemoryRequirements()).ptr;
  new(con16) ObserveBehaviourInterface_Con(); // set up vtable
  m_childModules[16] = observeBehaviourInterface = (ObserveBehaviourInterface*)NMPMemoryAllocateFromFormat(ObserveBehaviourInterface::getMemoryRequirements()).ptr;
  new (observeBehaviourInterface) ObserveBehaviourInterface(mdAllocator, con16);

  IdleAwakeBehaviourInterface_Con* con17 = (IdleAwakeBehaviourInterface_Con*)NMPMemoryAllocateFromFormat(IdleAwakeBehaviourInterface_Con::getMemoryRequirements()).ptr;
  new(con17) IdleAwakeBehaviourInterface_Con(); // set up vtable
  m_childModules[17] = idleAwakeBehaviourInterface = (IdleAwakeBehaviourInterface*)NMPMemoryAllocateFromFormat(IdleAwakeBehaviourInterface::getMemoryRequirements()).ptr;
  new (idleAwakeBehaviourInterface) IdleAwakeBehaviourInterface(mdAllocator, con17);

  m_childModules[18] = lookBehaviourInterface = (LookBehaviourInterface*)NMPMemoryAllocateFromFormat(LookBehaviourInterface::getMemoryRequirements()).ptr;
  new (lookBehaviourInterface) LookBehaviourInterface(mdAllocator, NULL);

  m_childModules[19] = headAvoidBehaviourInterface = (HeadAvoidBehaviourInterface*)NMPMemoryAllocateFromFormat(HeadAvoidBehaviourInterface::getMemoryRequirements()).ptr;
  new (headAvoidBehaviourInterface) HeadAvoidBehaviourInterface(mdAllocator, NULL);

  ShieldBehaviourInterface_Con* con20 = (ShieldBehaviourInterface_Con*)NMPMemoryAllocateFromFormat(ShieldBehaviourInterface_Con::getMemoryRequirements()).ptr;
  new(con20) ShieldBehaviourInterface_Con(); // set up vtable
  m_childModules[20] = shieldBehaviourInterface = (ShieldBehaviourInterface*)NMPMemoryAllocateFromFormat(ShieldBehaviourInterface::getMemoryRequirements()).ptr;
  new (shieldBehaviourInterface) ShieldBehaviourInterface(mdAllocator, con20);

  HoldBehaviourInterface_Con* con21 = (HoldBehaviourInterface_Con*)NMPMemoryAllocateFromFormat(HoldBehaviourInterface_Con::getMemoryRequirements()).ptr;
  new(con21) HoldBehaviourInterface_Con(); // set up vtable
  m_childModules[21] = holdBehaviourInterface = (HoldBehaviourInterface*)NMPMemoryAllocateFromFormat(HoldBehaviourInterface::getMemoryRequirements()).ptr;
  new (holdBehaviourInterface) HoldBehaviourInterface(mdAllocator, con21);

  HoldActionBehaviourInterface_Con* con22 = (HoldActionBehaviourInterface_Con*)NMPMemoryAllocateFromFormat(HoldActionBehaviourInterface_Con::getMemoryRequirements()).ptr;
  new(con22) HoldActionBehaviourInterface_Con(); // set up vtable
  m_childModules[22] = holdActionBehaviourInterface = (HoldActionBehaviourInterface*)NMPMemoryAllocateFromFormat(HoldActionBehaviourInterface::getMemoryRequirements()).ptr;
  new (holdActionBehaviourInterface) HoldActionBehaviourInterface(mdAllocator, con22);

  FreeFallBehaviourInterface_Con* con23 = (FreeFallBehaviourInterface_Con*)NMPMemoryAllocateFromFormat(FreeFallBehaviourInterface_Con::getMemoryRequirements()).ptr;
  new(con23) FreeFallBehaviourInterface_Con(); // set up vtable
  m_childModules[23] = freeFallBehaviourInterface = (FreeFallBehaviourInterface*)NMPMemoryAllocateFromFormat(FreeFallBehaviourInterface::getMemoryRequirements()).ptr;
  new (freeFallBehaviourInterface) FreeFallBehaviourInterface(mdAllocator, con23);

  m_childModules[24] = armsWindmillBehaviourInterface = (ArmsWindmillBehaviourInterface*)NMPMemoryAllocateFromFormat(ArmsWindmillBehaviourInterface::getMemoryRequirements()).ptr;
  new (armsWindmillBehaviourInterface) ArmsWindmillBehaviourInterface(mdAllocator, NULL);

  m_childModules[25] = legsPedalBehaviourInterface = (LegsPedalBehaviourInterface*)NMPMemoryAllocateFromFormat(LegsPedalBehaviourInterface::getMemoryRequirements()).ptr;
  new (legsPedalBehaviourInterface) LegsPedalBehaviourInterface(mdAllocator, NULL);

  ShieldActionBehaviourInterface_Con* con26 = (ShieldActionBehaviourInterface_Con*)NMPMemoryAllocateFromFormat(ShieldActionBehaviourInterface_Con::getMemoryRequirements()).ptr;
  new(con26) ShieldActionBehaviourInterface_Con(); // set up vtable
  m_childModules[26] = shieldActionBehaviourInterface = (ShieldActionBehaviourInterface*)NMPMemoryAllocateFromFormat(ShieldActionBehaviourInterface::getMemoryRequirements()).ptr;
  new (shieldActionBehaviourInterface) ShieldActionBehaviourInterface(mdAllocator, con26);

  SitBehaviourInterface_Con* con27 = (SitBehaviourInterface_Con*)NMPMemoryAllocateFromFormat(SitBehaviourInterface_Con::getMemoryRequirements()).ptr;
  new(con27) SitBehaviourInterface_Con(); // set up vtable
  m_childModules[27] = sitBehaviourInterface = (SitBehaviourInterface*)NMPMemoryAllocateFromFormat(SitBehaviourInterface::getMemoryRequirements()).ptr;
  new (sitBehaviourInterface) SitBehaviourInterface(mdAllocator, con27);

  m_childModules[28] = writheBehaviourInterface = (WritheBehaviourInterface*)NMPMemoryAllocateFromFormat(WritheBehaviourInterface::getMemoryRequirements()).ptr;
  new (writheBehaviourInterface) WritheBehaviourInterface(mdAllocator, NULL);

  RandomLook_Con* con29 = (RandomLook_Con*)NMPMemoryAllocateFromFormat(RandomLook_Con::getMemoryRequirements()).ptr;
  new(con29) RandomLook_Con(); // set up vtable
  m_childModules[29] = randomLook = (RandomLook*)NMPMemoryAllocateFromFormat(RandomLook::getMemoryRequirements()).ptr;
  new (randomLook) RandomLook(mdAllocator, con29);

  HazardManagement_Con* con30 = (HazardManagement_Con*)NMPMemoryAllocateFromFormat(HazardManagement_Con::getMemoryRequirements()).ptr;
  new(con30) HazardManagement_Con(); // set up vtable
  m_childModules[30] = hazardManagement = (HazardManagement*)NMPMemoryAllocateFromFormat(HazardManagement::getMemoryRequirements()).ptr;
  new (hazardManagement) HazardManagement(mdAllocator, con30);

  BalanceManagement_Con* con31 = (BalanceManagement_Con*)NMPMemoryAllocateFromFormat(BalanceManagement_Con::getMemoryRequirements()).ptr;
  new(con31) BalanceManagement_Con(); // set up vtable
  m_childModules[31] = balanceManagement = (BalanceManagement*)NMPMemoryAllocateFromFormat(BalanceManagement::getMemoryRequirements()).ptr;
  new (balanceManagement) BalanceManagement(mdAllocator, con31);

  EnvironmentAwareness_Con* con32 = (EnvironmentAwareness_Con*)NMPMemoryAllocateFromFormat(EnvironmentAwareness_Con::getMemoryRequirements()).ptr;
  new(con32) EnvironmentAwareness_Con(); // set up vtable
  m_childModules[32] = environmentAwareness = (EnvironmentAwareness*)NMPMemoryAllocateFromFormat(EnvironmentAwareness::getMemoryRequirements()).ptr;
  new (environmentAwareness) EnvironmentAwareness(mdAllocator, con32);

  BodyFrame_Con* con33 = (BodyFrame_Con*)NMPMemoryAllocateFromFormat(BodyFrame_Con::getMemoryRequirements()).ptr;
  new(con33) BodyFrame_Con(); // set up vtable
  m_childModules[33] = bodyFrame = (BodyFrame*)NMPMemoryAllocateFromFormat(BodyFrame::getMemoryRequirements()).ptr;
  new (bodyFrame) BodyFrame(mdAllocator, con33);

  BodySection_upperCon* con34 = (BodySection_upperCon*)NMPMemoryAllocateFromFormat(BodySection_upperCon::getMemoryRequirements()).ptr;
  new(con34) BodySection_upperCon(); // set up vtable
  m_childModules[34] = upperBody = (BodySection*)NMPMemoryAllocateFromFormat(BodySection::getMemoryRequirements()).ptr;
  new (upperBody) BodySection(mdAllocator, con34);

  BodySection_lowerCon* con35 = (BodySection_lowerCon*)NMPMemoryAllocateFromFormat(BodySection_lowerCon::getMemoryRequirements()).ptr;
  new(con35) BodySection_lowerCon(); // set up vtable
  m_childModules[35] = lowerBody = (BodySection*)NMPMemoryAllocateFromFormat(BodySection::getMemoryRequirements()).ptr;
  new (lowerBody) BodySection(mdAllocator, con35);

  Arm_Con* con36 = (Arm_Con*)NMPMemoryAllocateFromFormat(Arm_Con::getMemoryRequirements()).ptr;
  new(con36) Arm_Con(); // set up vtable
  m_childModules[36] = arms[0] = (Arm*)NMPMemoryAllocateFromFormat(Arm::getMemoryRequirements()).ptr;
  new (arms[0]) Arm(mdAllocator, con36);
  Arm_Con* con37 = (Arm_Con*)NMPMemoryAllocateFromFormat(Arm_Con::getMemoryRequirements()).ptr;
  new(con37) Arm_Con(); // set up vtable
  m_childModules[37] = arms[1] = (Arm*)NMPMemoryAllocateFromFormat(Arm::getMemoryRequirements()).ptr;
  new (arms[1]) Arm(mdAllocator, con37);

  Head_Con* con38 = (Head_Con*)NMPMemoryAllocateFromFormat(Head_Con::getMemoryRequirements()).ptr;
  new(con38) Head_Con(); // set up vtable
  m_childModules[38] = heads[0] = (Head*)NMPMemoryAllocateFromFormat(Head::getMemoryRequirements()).ptr;
  new (heads[0]) Head(mdAllocator, con38);

  Leg_Con* con39 = (Leg_Con*)NMPMemoryAllocateFromFormat(Leg_Con::getMemoryRequirements()).ptr;
  new(con39) Leg_Con(); // set up vtable
  m_childModules[39] = legs[0] = (Leg*)NMPMemoryAllocateFromFormat(Leg::getMemoryRequirements()).ptr;
  new (legs[0]) Leg(mdAllocator, con39);
  Leg_Con* con40 = (Leg_Con*)NMPMemoryAllocateFromFormat(Leg_Con::getMemoryRequirements()).ptr;
  new(con40) Leg_Con(); // set up vtable
  m_childModules[40] = legs[1] = (Leg*)NMPMemoryAllocateFromFormat(Leg::getMemoryRequirements()).ptr;
  new (legs[1]) Leg(mdAllocator, con40);

  Spine_Con* con41 = (Spine_Con*)NMPMemoryAllocateFromFormat(Spine_Con::getMemoryRequirements()).ptr;
  new(con41) Spine_Con(); // set up vtable
  m_childModules[41] = spines[0] = (Spine*)NMPMemoryAllocateFromFormat(Spine::getMemoryRequirements()).ptr;
  new (spines[0]) Spine(mdAllocator, con41);

  SceneProbes_Con* con42 = (SceneProbes_Con*)NMPMemoryAllocateFromFormat(SceneProbes_Con::getMemoryRequirements()).ptr;
  new(con42) SceneProbes_Con(); // set up vtable
  m_childModules[42] = sceneProbes = (SceneProbes*)NMPMemoryAllocateFromFormat(SceneProbes::getMemoryRequirements()).ptr;
  new (sceneProbes) SceneProbes(mdAllocator, con42);

  m_childModules[43] = balanceBehaviourFeedback = (BalanceBehaviourFeedback*)NMPMemoryAllocateFromFormat(BalanceBehaviourFeedback::getMemoryRequirements()).ptr;
  new (balanceBehaviourFeedback) BalanceBehaviourFeedback(mdAllocator, NULL);


  m_numModules = NetworkManifest::NumModules;
  m_allModules = (ER::Module**)NMPMemoryAlloc(sizeof(ER::Module*) * m_numModules);
  m_allModules[NetworkManifest::rootModule] = this; 
  m_allModules[NetworkManifest::rootModule]->assignManifestIndex(NetworkManifest::rootModule);
  m_allModules[NetworkManifest::characteristicsBehaviourInterface] = characteristicsBehaviourInterface;
  m_allModules[NetworkManifest::characteristicsBehaviourInterface]->assignManifestIndex(NetworkManifest::characteristicsBehaviourInterface);
  m_allModules[NetworkManifest::propertiesBehaviourInterface] = propertiesBehaviourInterface;
  m_allModules[NetworkManifest::propertiesBehaviourInterface]->assignManifestIndex(NetworkManifest::propertiesBehaviourInterface);
  m_allModules[NetworkManifest::euphoriaRagdollBehaviourInterface] = euphoriaRagdollBehaviourInterface;
  m_allModules[NetworkManifest::euphoriaRagdollBehaviourInterface]->assignManifestIndex(NetworkManifest::euphoriaRagdollBehaviourInterface);
  m_allModules[NetworkManifest::eyesBehaviourInterface] = eyesBehaviourInterface;
  m_allModules[NetworkManifest::eyesBehaviourInterface]->assignManifestIndex(NetworkManifest::eyesBehaviourInterface);
  m_allModules[NetworkManifest::armsBraceBehaviourInterface] = armsBraceBehaviourInterface;
  m_allModules[NetworkManifest::armsBraceBehaviourInterface]->assignManifestIndex(NetworkManifest::armsBraceBehaviourInterface);
  m_allModules[NetworkManifest::armsPlacementBehaviourInterface] = armsPlacementBehaviourInterface;
  m_allModules[NetworkManifest::armsPlacementBehaviourInterface]->assignManifestIndex(NetworkManifest::armsPlacementBehaviourInterface);
  m_allModules[NetworkManifest::aimBehaviourInterface] = aimBehaviourInterface;
  m_allModules[NetworkManifest::aimBehaviourInterface]->assignManifestIndex(NetworkManifest::aimBehaviourInterface);
  m_allModules[NetworkManifest::headDodgeBehaviourInterface] = headDodgeBehaviourInterface;
  m_allModules[NetworkManifest::headDodgeBehaviourInterface]->assignManifestIndex(NetworkManifest::headDodgeBehaviourInterface);
  m_allModules[NetworkManifest::reachForBodyBehaviourInterface] = reachForBodyBehaviourInterface;
  m_allModules[NetworkManifest::reachForBodyBehaviourInterface]->assignManifestIndex(NetworkManifest::reachForBodyBehaviourInterface);
  m_allModules[NetworkManifest::reachForWorldBehaviourInterface] = reachForWorldBehaviourInterface;
  m_allModules[NetworkManifest::reachForWorldBehaviourInterface]->assignManifestIndex(NetworkManifest::reachForWorldBehaviourInterface);
  m_allModules[NetworkManifest::animateBehaviourInterface] = animateBehaviourInterface;
  m_allModules[NetworkManifest::animateBehaviourInterface]->assignManifestIndex(NetworkManifest::animateBehaviourInterface);
  m_allModules[NetworkManifest::balanceBehaviourInterface] = balanceBehaviourInterface;
  m_allModules[NetworkManifest::balanceBehaviourInterface]->assignManifestIndex(NetworkManifest::balanceBehaviourInterface);
  m_allModules[NetworkManifest::balancePoserBehaviourInterface] = balancePoserBehaviourInterface;
  m_allModules[NetworkManifest::balancePoserBehaviourInterface]->assignManifestIndex(NetworkManifest::balancePoserBehaviourInterface);
  m_allModules[NetworkManifest::protectBehaviourInterface] = protectBehaviourInterface;
  m_allModules[NetworkManifest::protectBehaviourInterface]->assignManifestIndex(NetworkManifest::protectBehaviourInterface);
  m_allModules[NetworkManifest::hazardAwarenessBehaviourInterface] = hazardAwarenessBehaviourInterface;
  m_allModules[NetworkManifest::hazardAwarenessBehaviourInterface]->assignManifestIndex(NetworkManifest::hazardAwarenessBehaviourInterface);
  m_allModules[NetworkManifest::userHazardBehaviourInterface] = userHazardBehaviourInterface;
  m_allModules[NetworkManifest::userHazardBehaviourInterface]->assignManifestIndex(NetworkManifest::userHazardBehaviourInterface);
  m_allModules[NetworkManifest::observeBehaviourInterface] = observeBehaviourInterface;
  m_allModules[NetworkManifest::observeBehaviourInterface]->assignManifestIndex(NetworkManifest::observeBehaviourInterface);
  m_allModules[NetworkManifest::idleAwakeBehaviourInterface] = idleAwakeBehaviourInterface;
  m_allModules[NetworkManifest::idleAwakeBehaviourInterface]->assignManifestIndex(NetworkManifest::idleAwakeBehaviourInterface);
  m_allModules[NetworkManifest::lookBehaviourInterface] = lookBehaviourInterface;
  m_allModules[NetworkManifest::lookBehaviourInterface]->assignManifestIndex(NetworkManifest::lookBehaviourInterface);
  m_allModules[NetworkManifest::headAvoidBehaviourInterface] = headAvoidBehaviourInterface;
  m_allModules[NetworkManifest::headAvoidBehaviourInterface]->assignManifestIndex(NetworkManifest::headAvoidBehaviourInterface);
  m_allModules[NetworkManifest::shieldBehaviourInterface] = shieldBehaviourInterface;
  m_allModules[NetworkManifest::shieldBehaviourInterface]->assignManifestIndex(NetworkManifest::shieldBehaviourInterface);
  m_allModules[NetworkManifest::holdBehaviourInterface] = holdBehaviourInterface;
  m_allModules[NetworkManifest::holdBehaviourInterface]->assignManifestIndex(NetworkManifest::holdBehaviourInterface);
  m_allModules[NetworkManifest::holdActionBehaviourInterface] = holdActionBehaviourInterface;
  m_allModules[NetworkManifest::holdActionBehaviourInterface]->assignManifestIndex(NetworkManifest::holdActionBehaviourInterface);
  m_allModules[NetworkManifest::freeFallBehaviourInterface] = freeFallBehaviourInterface;
  m_allModules[NetworkManifest::freeFallBehaviourInterface]->assignManifestIndex(NetworkManifest::freeFallBehaviourInterface);
  m_allModules[NetworkManifest::armsWindmillBehaviourInterface] = armsWindmillBehaviourInterface;
  m_allModules[NetworkManifest::armsWindmillBehaviourInterface]->assignManifestIndex(NetworkManifest::armsWindmillBehaviourInterface);
  m_allModules[NetworkManifest::legsPedalBehaviourInterface] = legsPedalBehaviourInterface;
  m_allModules[NetworkManifest::legsPedalBehaviourInterface]->assignManifestIndex(NetworkManifest::legsPedalBehaviourInterface);
  m_allModules[NetworkManifest::shieldActionBehaviourInterface] = shieldActionBehaviourInterface;
  m_allModules[NetworkManifest::shieldActionBehaviourInterface]->assignManifestIndex(NetworkManifest::shieldActionBehaviourInterface);
  m_allModules[NetworkManifest::sitBehaviourInterface] = sitBehaviourInterface;
  m_allModules[NetworkManifest::sitBehaviourInterface]->assignManifestIndex(NetworkManifest::sitBehaviourInterface);
  m_allModules[NetworkManifest::writheBehaviourInterface] = writheBehaviourInterface;
  m_allModules[NetworkManifest::writheBehaviourInterface]->assignManifestIndex(NetworkManifest::writheBehaviourInterface);
  m_allModules[NetworkManifest::randomLook] = randomLook;
  m_allModules[NetworkManifest::randomLook]->assignManifestIndex(NetworkManifest::randomLook);
  m_allModules[NetworkManifest::hazardManagement] = hazardManagement;
  m_allModules[NetworkManifest::hazardManagement]->assignManifestIndex(NetworkManifest::hazardManagement);
  m_allModules[NetworkManifest::hazardManagement_grabDetection] = hazardManagement->getChild(0);
  m_allModules[NetworkManifest::hazardManagement_grabDetection]->assignManifestIndex(NetworkManifest::hazardManagement_grabDetection);
  m_allModules[NetworkManifest::hazardManagement_grab] = hazardManagement->getChild(1);
  m_allModules[NetworkManifest::hazardManagement_grab]->assignManifestIndex(NetworkManifest::hazardManagement_grab);
  m_allModules[NetworkManifest::hazardManagement_hazardResponse] = hazardManagement->getChild(2);
  m_allModules[NetworkManifest::hazardManagement_hazardResponse]->assignManifestIndex(NetworkManifest::hazardManagement_hazardResponse);
  m_allModules[NetworkManifest::hazardManagement_freeFallManagement] = hazardManagement->getChild(3);
  m_allModules[NetworkManifest::hazardManagement_freeFallManagement]->assignManifestIndex(NetworkManifest::hazardManagement_freeFallManagement);
  m_allModules[NetworkManifest::hazardManagement_chestImpactPredictor] = hazardManagement->getChild(4);
  m_allModules[NetworkManifest::hazardManagement_chestImpactPredictor]->assignManifestIndex(NetworkManifest::hazardManagement_chestImpactPredictor);
  m_allModules[NetworkManifest::hazardManagement_shieldManagement] = hazardManagement->getChild(5);
  m_allModules[NetworkManifest::hazardManagement_shieldManagement]->assignManifestIndex(NetworkManifest::hazardManagement_shieldManagement);
  m_allModules[NetworkManifest::balanceManagement] = balanceManagement;
  m_allModules[NetworkManifest::balanceManagement]->assignManifestIndex(NetworkManifest::balanceManagement);
  m_allModules[NetworkManifest::balanceManagement_staticBalance] = balanceManagement->getChild(0);
  m_allModules[NetworkManifest::balanceManagement_staticBalance]->assignManifestIndex(NetworkManifest::balanceManagement_staticBalance);
  m_allModules[NetworkManifest::balanceManagement_steppingBalance] = balanceManagement->getChild(1);
  m_allModules[NetworkManifest::balanceManagement_steppingBalance]->assignManifestIndex(NetworkManifest::balanceManagement_steppingBalance);
  m_allModules[NetworkManifest::balanceManagement_balancePoser] = balanceManagement->getChild(2);
  m_allModules[NetworkManifest::balanceManagement_balancePoser]->assignManifestIndex(NetworkManifest::balanceManagement_balancePoser);
  m_allModules[NetworkManifest::environmentAwareness] = environmentAwareness;
  m_allModules[NetworkManifest::environmentAwareness]->assignManifestIndex(NetworkManifest::environmentAwareness);
  m_allModules[NetworkManifest::bodyFrame] = bodyFrame;
  m_allModules[NetworkManifest::bodyFrame]->assignManifestIndex(NetworkManifest::bodyFrame);
  m_allModules[NetworkManifest::bodyFrame_supportPolygon] = bodyFrame->getChild(0);
  m_allModules[NetworkManifest::bodyFrame_supportPolygon]->assignManifestIndex(NetworkManifest::bodyFrame_supportPolygon);
  m_allModules[NetworkManifest::bodyFrame_sittingSupportPolygon] = bodyFrame->getChild(1);
  m_allModules[NetworkManifest::bodyFrame_sittingSupportPolygon]->assignManifestIndex(NetworkManifest::bodyFrame_sittingSupportPolygon);
  m_allModules[NetworkManifest::bodyFrame_bodyBalance] = bodyFrame->getChild(2);
  m_allModules[NetworkManifest::bodyFrame_bodyBalance]->assignManifestIndex(NetworkManifest::bodyFrame_bodyBalance);
  m_allModules[NetworkManifest::bodyFrame_sittingBodyBalance] = bodyFrame->getChild(3);
  m_allModules[NetworkManifest::bodyFrame_sittingBodyBalance]->assignManifestIndex(NetworkManifest::bodyFrame_sittingBodyBalance);
  m_allModules[NetworkManifest::bodyFrame_reachForBody] = bodyFrame->getChild(4);
  m_allModules[NetworkManifest::bodyFrame_reachForBody]->assignManifestIndex(NetworkManifest::bodyFrame_reachForBody);
  m_allModules[NetworkManifest::bodyFrame_balanceAssistant] = bodyFrame->getChild(5);
  m_allModules[NetworkManifest::bodyFrame_balanceAssistant]->assignManifestIndex(NetworkManifest::bodyFrame_balanceAssistant);
  m_allModules[NetworkManifest::upperBody] = upperBody;
  m_allModules[NetworkManifest::upperBody]->assignManifestIndex(NetworkManifest::upperBody);
  m_allModules[NetworkManifest::upperBody_rotate] = upperBody->getChild(0);
  m_allModules[NetworkManifest::upperBody_rotate]->assignManifestIndex(NetworkManifest::upperBody_rotate);
  m_allModules[NetworkManifest::upperBody_position] = upperBody->getChild(1);
  m_allModules[NetworkManifest::upperBody_position]->assignManifestIndex(NetworkManifest::upperBody_position);
  m_allModules[NetworkManifest::upperBody_braceChooser] = upperBody->getChild(2);
  m_allModules[NetworkManifest::upperBody_braceChooser]->assignManifestIndex(NetworkManifest::upperBody_braceChooser);
  m_allModules[NetworkManifest::lowerBody] = lowerBody;
  m_allModules[NetworkManifest::lowerBody]->assignManifestIndex(NetworkManifest::lowerBody);
  m_allModules[NetworkManifest::lowerBody_rotate] = lowerBody->getChild(0);
  m_allModules[NetworkManifest::lowerBody_rotate]->assignManifestIndex(NetworkManifest::lowerBody_rotate);
  m_allModules[NetworkManifest::lowerBody_position] = lowerBody->getChild(1);
  m_allModules[NetworkManifest::lowerBody_position]->assignManifestIndex(NetworkManifest::lowerBody_position);
  m_allModules[NetworkManifest::lowerBody_braceChooser] = lowerBody->getChild(2);
  m_allModules[NetworkManifest::lowerBody_braceChooser]->assignManifestIndex(NetworkManifest::lowerBody_braceChooser);
  m_allModules[NetworkManifest::arms_0] = arms[0];
  m_allModules[NetworkManifest::arms_0]->assignManifestIndex(NetworkManifest::arms_0);
  m_allModules[NetworkManifest::arms_0_grab] = arms[0]->getChild(0);
  m_allModules[NetworkManifest::arms_0_grab]->assignManifestIndex(NetworkManifest::arms_0_grab);
  m_allModules[NetworkManifest::arms_0_aim] = arms[0]->getChild(1);
  m_allModules[NetworkManifest::arms_0_aim]->assignManifestIndex(NetworkManifest::arms_0_aim);
  m_allModules[NetworkManifest::arms_0_hold] = arms[0]->getChild(2);
  m_allModules[NetworkManifest::arms_0_hold]->assignManifestIndex(NetworkManifest::arms_0_hold);
  m_allModules[NetworkManifest::arms_0_brace] = arms[0]->getChild(3);
  m_allModules[NetworkManifest::arms_0_brace]->assignManifestIndex(NetworkManifest::arms_0_brace);
  m_allModules[NetworkManifest::arms_0_standingSupport] = arms[0]->getChild(4);
  m_allModules[NetworkManifest::arms_0_standingSupport]->assignManifestIndex(NetworkManifest::arms_0_standingSupport);
  m_allModules[NetworkManifest::arms_0_holdingSupport] = arms[0]->getChild(5);
  m_allModules[NetworkManifest::arms_0_holdingSupport]->assignManifestIndex(NetworkManifest::arms_0_holdingSupport);
  m_allModules[NetworkManifest::arms_0_sittingStep] = arms[0]->getChild(6);
  m_allModules[NetworkManifest::arms_0_sittingStep]->assignManifestIndex(NetworkManifest::arms_0_sittingStep);
  m_allModules[NetworkManifest::arms_0_step] = arms[0]->getChild(7);
  m_allModules[NetworkManifest::arms_0_step]->assignManifestIndex(NetworkManifest::arms_0_step);
  m_allModules[NetworkManifest::arms_0_spin] = arms[0]->getChild(8);
  m_allModules[NetworkManifest::arms_0_spin]->assignManifestIndex(NetworkManifest::arms_0_spin);
  m_allModules[NetworkManifest::arms_0_swing] = arms[0]->getChild(9);
  m_allModules[NetworkManifest::arms_0_swing]->assignManifestIndex(NetworkManifest::arms_0_swing);
  m_allModules[NetworkManifest::arms_0_reachForBodyPart] = arms[0]->getChild(10);
  m_allModules[NetworkManifest::arms_0_reachForBodyPart]->assignManifestIndex(NetworkManifest::arms_0_reachForBodyPart);
  m_allModules[NetworkManifest::arms_0_reachForWorld] = arms[0]->getChild(11);
  m_allModules[NetworkManifest::arms_0_reachForWorld]->assignManifestIndex(NetworkManifest::arms_0_reachForWorld);
  m_allModules[NetworkManifest::arms_0_placement] = arms[0]->getChild(12);
  m_allModules[NetworkManifest::arms_0_placement]->assignManifestIndex(NetworkManifest::arms_0_placement);
  m_allModules[NetworkManifest::arms_0_pose] = arms[0]->getChild(13);
  m_allModules[NetworkManifest::arms_0_pose]->assignManifestIndex(NetworkManifest::arms_0_pose);
  m_allModules[NetworkManifest::arms_0_reachReaction] = arms[0]->getChild(14);
  m_allModules[NetworkManifest::arms_0_reachReaction]->assignManifestIndex(NetworkManifest::arms_0_reachReaction);
  m_allModules[NetworkManifest::arms_0_writhe] = arms[0]->getChild(15);
  m_allModules[NetworkManifest::arms_0_writhe]->assignManifestIndex(NetworkManifest::arms_0_writhe);
  m_allModules[NetworkManifest::arms_1] = arms[1];
  m_allModules[NetworkManifest::arms_1]->assignManifestIndex(NetworkManifest::arms_1);
  m_allModules[NetworkManifest::arms_1_grab] = arms[1]->getChild(0);
  m_allModules[NetworkManifest::arms_1_grab]->assignManifestIndex(NetworkManifest::arms_1_grab);
  m_allModules[NetworkManifest::arms_1_aim] = arms[1]->getChild(1);
  m_allModules[NetworkManifest::arms_1_aim]->assignManifestIndex(NetworkManifest::arms_1_aim);
  m_allModules[NetworkManifest::arms_1_hold] = arms[1]->getChild(2);
  m_allModules[NetworkManifest::arms_1_hold]->assignManifestIndex(NetworkManifest::arms_1_hold);
  m_allModules[NetworkManifest::arms_1_brace] = arms[1]->getChild(3);
  m_allModules[NetworkManifest::arms_1_brace]->assignManifestIndex(NetworkManifest::arms_1_brace);
  m_allModules[NetworkManifest::arms_1_standingSupport] = arms[1]->getChild(4);
  m_allModules[NetworkManifest::arms_1_standingSupport]->assignManifestIndex(NetworkManifest::arms_1_standingSupport);
  m_allModules[NetworkManifest::arms_1_holdingSupport] = arms[1]->getChild(5);
  m_allModules[NetworkManifest::arms_1_holdingSupport]->assignManifestIndex(NetworkManifest::arms_1_holdingSupport);
  m_allModules[NetworkManifest::arms_1_sittingStep] = arms[1]->getChild(6);
  m_allModules[NetworkManifest::arms_1_sittingStep]->assignManifestIndex(NetworkManifest::arms_1_sittingStep);
  m_allModules[NetworkManifest::arms_1_step] = arms[1]->getChild(7);
  m_allModules[NetworkManifest::arms_1_step]->assignManifestIndex(NetworkManifest::arms_1_step);
  m_allModules[NetworkManifest::arms_1_spin] = arms[1]->getChild(8);
  m_allModules[NetworkManifest::arms_1_spin]->assignManifestIndex(NetworkManifest::arms_1_spin);
  m_allModules[NetworkManifest::arms_1_swing] = arms[1]->getChild(9);
  m_allModules[NetworkManifest::arms_1_swing]->assignManifestIndex(NetworkManifest::arms_1_swing);
  m_allModules[NetworkManifest::arms_1_reachForBodyPart] = arms[1]->getChild(10);
  m_allModules[NetworkManifest::arms_1_reachForBodyPart]->assignManifestIndex(NetworkManifest::arms_1_reachForBodyPart);
  m_allModules[NetworkManifest::arms_1_reachForWorld] = arms[1]->getChild(11);
  m_allModules[NetworkManifest::arms_1_reachForWorld]->assignManifestIndex(NetworkManifest::arms_1_reachForWorld);
  m_allModules[NetworkManifest::arms_1_placement] = arms[1]->getChild(12);
  m_allModules[NetworkManifest::arms_1_placement]->assignManifestIndex(NetworkManifest::arms_1_placement);
  m_allModules[NetworkManifest::arms_1_pose] = arms[1]->getChild(13);
  m_allModules[NetworkManifest::arms_1_pose]->assignManifestIndex(NetworkManifest::arms_1_pose);
  m_allModules[NetworkManifest::arms_1_reachReaction] = arms[1]->getChild(14);
  m_allModules[NetworkManifest::arms_1_reachReaction]->assignManifestIndex(NetworkManifest::arms_1_reachReaction);
  m_allModules[NetworkManifest::arms_1_writhe] = arms[1]->getChild(15);
  m_allModules[NetworkManifest::arms_1_writhe]->assignManifestIndex(NetworkManifest::arms_1_writhe);
  m_allModules[NetworkManifest::heads_0] = heads[0];
  m_allModules[NetworkManifest::heads_0]->assignManifestIndex(NetworkManifest::heads_0);
  m_allModules[NetworkManifest::heads_0_eyes] = heads[0]->getChild(0);
  m_allModules[NetworkManifest::heads_0_eyes]->assignManifestIndex(NetworkManifest::heads_0_eyes);
  m_allModules[NetworkManifest::heads_0_avoid] = heads[0]->getChild(1);
  m_allModules[NetworkManifest::heads_0_avoid]->assignManifestIndex(NetworkManifest::heads_0_avoid);
  m_allModules[NetworkManifest::heads_0_dodge] = heads[0]->getChild(2);
  m_allModules[NetworkManifest::heads_0_dodge]->assignManifestIndex(NetworkManifest::heads_0_dodge);
  m_allModules[NetworkManifest::heads_0_aim] = heads[0]->getChild(3);
  m_allModules[NetworkManifest::heads_0_aim]->assignManifestIndex(NetworkManifest::heads_0_aim);
  m_allModules[NetworkManifest::heads_0_point] = heads[0]->getChild(4);
  m_allModules[NetworkManifest::heads_0_point]->assignManifestIndex(NetworkManifest::heads_0_point);
  m_allModules[NetworkManifest::heads_0_support] = heads[0]->getChild(5);
  m_allModules[NetworkManifest::heads_0_support]->assignManifestIndex(NetworkManifest::heads_0_support);
  m_allModules[NetworkManifest::heads_0_pose] = heads[0]->getChild(6);
  m_allModules[NetworkManifest::heads_0_pose]->assignManifestIndex(NetworkManifest::heads_0_pose);
  m_allModules[NetworkManifest::legs_0] = legs[0];
  m_allModules[NetworkManifest::legs_0]->assignManifestIndex(NetworkManifest::legs_0);
  m_allModules[NetworkManifest::legs_0_brace] = legs[0]->getChild(0);
  m_allModules[NetworkManifest::legs_0_brace]->assignManifestIndex(NetworkManifest::legs_0_brace);
  m_allModules[NetworkManifest::legs_0_standingSupport] = legs[0]->getChild(1);
  m_allModules[NetworkManifest::legs_0_standingSupport]->assignManifestIndex(NetworkManifest::legs_0_standingSupport);
  m_allModules[NetworkManifest::legs_0_sittingSupport] = legs[0]->getChild(2);
  m_allModules[NetworkManifest::legs_0_sittingSupport]->assignManifestIndex(NetworkManifest::legs_0_sittingSupport);
  m_allModules[NetworkManifest::legs_0_step] = legs[0]->getChild(3);
  m_allModules[NetworkManifest::legs_0_step]->assignManifestIndex(NetworkManifest::legs_0_step);
  m_allModules[NetworkManifest::legs_0_swing] = legs[0]->getChild(4);
  m_allModules[NetworkManifest::legs_0_swing]->assignManifestIndex(NetworkManifest::legs_0_swing);
  m_allModules[NetworkManifest::legs_0_pose] = legs[0]->getChild(5);
  m_allModules[NetworkManifest::legs_0_pose]->assignManifestIndex(NetworkManifest::legs_0_pose);
  m_allModules[NetworkManifest::legs_0_spin] = legs[0]->getChild(6);
  m_allModules[NetworkManifest::legs_0_spin]->assignManifestIndex(NetworkManifest::legs_0_spin);
  m_allModules[NetworkManifest::legs_0_reachReaction] = legs[0]->getChild(7);
  m_allModules[NetworkManifest::legs_0_reachReaction]->assignManifestIndex(NetworkManifest::legs_0_reachReaction);
  m_allModules[NetworkManifest::legs_0_writhe] = legs[0]->getChild(8);
  m_allModules[NetworkManifest::legs_0_writhe]->assignManifestIndex(NetworkManifest::legs_0_writhe);
  m_allModules[NetworkManifest::legs_1] = legs[1];
  m_allModules[NetworkManifest::legs_1]->assignManifestIndex(NetworkManifest::legs_1);
  m_allModules[NetworkManifest::legs_1_brace] = legs[1]->getChild(0);
  m_allModules[NetworkManifest::legs_1_brace]->assignManifestIndex(NetworkManifest::legs_1_brace);
  m_allModules[NetworkManifest::legs_1_standingSupport] = legs[1]->getChild(1);
  m_allModules[NetworkManifest::legs_1_standingSupport]->assignManifestIndex(NetworkManifest::legs_1_standingSupport);
  m_allModules[NetworkManifest::legs_1_sittingSupport] = legs[1]->getChild(2);
  m_allModules[NetworkManifest::legs_1_sittingSupport]->assignManifestIndex(NetworkManifest::legs_1_sittingSupport);
  m_allModules[NetworkManifest::legs_1_step] = legs[1]->getChild(3);
  m_allModules[NetworkManifest::legs_1_step]->assignManifestIndex(NetworkManifest::legs_1_step);
  m_allModules[NetworkManifest::legs_1_swing] = legs[1]->getChild(4);
  m_allModules[NetworkManifest::legs_1_swing]->assignManifestIndex(NetworkManifest::legs_1_swing);
  m_allModules[NetworkManifest::legs_1_pose] = legs[1]->getChild(5);
  m_allModules[NetworkManifest::legs_1_pose]->assignManifestIndex(NetworkManifest::legs_1_pose);
  m_allModules[NetworkManifest::legs_1_spin] = legs[1]->getChild(6);
  m_allModules[NetworkManifest::legs_1_spin]->assignManifestIndex(NetworkManifest::legs_1_spin);
  m_allModules[NetworkManifest::legs_1_reachReaction] = legs[1]->getChild(7);
  m_allModules[NetworkManifest::legs_1_reachReaction]->assignManifestIndex(NetworkManifest::legs_1_reachReaction);
  m_allModules[NetworkManifest::legs_1_writhe] = legs[1]->getChild(8);
  m_allModules[NetworkManifest::legs_1_writhe]->assignManifestIndex(NetworkManifest::legs_1_writhe);
  m_allModules[NetworkManifest::spines_0] = spines[0];
  m_allModules[NetworkManifest::spines_0]->assignManifestIndex(NetworkManifest::spines_0);
  m_allModules[NetworkManifest::spines_0_support] = spines[0]->getChild(0);
  m_allModules[NetworkManifest::spines_0_support]->assignManifestIndex(NetworkManifest::spines_0_support);
  m_allModules[NetworkManifest::spines_0_pose] = spines[0]->getChild(1);
  m_allModules[NetworkManifest::spines_0_pose]->assignManifestIndex(NetworkManifest::spines_0_pose);
  m_allModules[NetworkManifest::spines_0_control] = spines[0]->getChild(2);
  m_allModules[NetworkManifest::spines_0_control]->assignManifestIndex(NetworkManifest::spines_0_control);
  m_allModules[NetworkManifest::spines_0_reachReaction] = spines[0]->getChild(3);
  m_allModules[NetworkManifest::spines_0_reachReaction]->assignManifestIndex(NetworkManifest::spines_0_reachReaction);
  m_allModules[NetworkManifest::spines_0_writhe] = spines[0]->getChild(4);
  m_allModules[NetworkManifest::spines_0_writhe]->assignManifestIndex(NetworkManifest::spines_0_writhe);
  m_allModules[NetworkManifest::spines_0_aim] = spines[0]->getChild(5);
  m_allModules[NetworkManifest::spines_0_aim]->assignManifestIndex(NetworkManifest::spines_0_aim);
  m_allModules[NetworkManifest::sceneProbes] = sceneProbes;
  m_allModules[NetworkManifest::sceneProbes]->assignManifestIndex(NetworkManifest::sceneProbes);
  m_allModules[NetworkManifest::balanceBehaviourFeedback] = balanceBehaviourFeedback;
  m_allModules[NetworkManifest::balanceBehaviourFeedback]->assignManifestIndex(NetworkManifest::balanceBehaviourFeedback);

  m_moduleEnableStates = (ER::RootModule::ModuleEnableState*)NMPMemoryAlloc(sizeof(ER::RootModule::ModuleEnableState) * m_numModules);
  for (unsigned int cm = 0; cm < m_numModules; cm ++)
  {
    m_moduleEnableStates[cm].m_enabled = 0;
    m_moduleEnableStates[cm].m_enabledByOwner = 1;
  }

  m_apiBase = (MyNetworkAPIBase*)NMPMemoryAllocAligned(sizeof(MyNetworkAPIBase), 16);
}

//----------------------------------------------------------------------------------------------------------------------
MyNetwork::~MyNetwork()
{
  ER::ModuleCon* balanceBehaviourFeedbackConnections = balanceBehaviourFeedback->getConnections();
  if (balanceBehaviourFeedbackConnections)
  {
    balanceBehaviourFeedbackConnections->~ModuleCon();
    NMP::Memory::memFree(balanceBehaviourFeedbackConnections);
  }
  balanceBehaviourFeedback->~BalanceBehaviourFeedback();
  NMP::Memory::memFree(balanceBehaviourFeedback);

  ER::ModuleCon* sceneProbesConnections = sceneProbes->getConnections();
  if (sceneProbesConnections)
  {
    sceneProbesConnections->~ModuleCon();
    NMP::Memory::memFree(sceneProbesConnections);
  }
  sceneProbes->~SceneProbes();
  NMP::Memory::memFree(sceneProbes);

  ER::ModuleCon* spines0Connections = spines[0]->getConnections();
  spines0Connections->~ModuleCon();
  NMP::Memory::memFree(spines0Connections);
  spines[0]->~Spine();
  NMP::Memory::memFree(spines[0]);


  ER::ModuleCon* legs0Connections = legs[0]->getConnections();
  legs0Connections->~ModuleCon();
  NMP::Memory::memFree(legs0Connections);
  legs[0]->~Leg();
  NMP::Memory::memFree(legs[0]);

  ER::ModuleCon* legs1Connections = legs[1]->getConnections();
  legs1Connections->~ModuleCon();
  NMP::Memory::memFree(legs1Connections);
  legs[1]->~Leg();
  NMP::Memory::memFree(legs[1]);


  ER::ModuleCon* heads0Connections = heads[0]->getConnections();
  heads0Connections->~ModuleCon();
  NMP::Memory::memFree(heads0Connections);
  heads[0]->~Head();
  NMP::Memory::memFree(heads[0]);


  ER::ModuleCon* arms0Connections = arms[0]->getConnections();
  arms0Connections->~ModuleCon();
  NMP::Memory::memFree(arms0Connections);
  arms[0]->~Arm();
  NMP::Memory::memFree(arms[0]);

  ER::ModuleCon* arms1Connections = arms[1]->getConnections();
  arms1Connections->~ModuleCon();
  NMP::Memory::memFree(arms1Connections);
  arms[1]->~Arm();
  NMP::Memory::memFree(arms[1]);


  ER::ModuleCon* lowerBodyConnections = lowerBody->getConnections();
  if (lowerBodyConnections)
  {
    lowerBodyConnections->~ModuleCon();
    NMP::Memory::memFree(lowerBodyConnections);
  }
  lowerBody->~BodySection();
  NMP::Memory::memFree(lowerBody);

  ER::ModuleCon* upperBodyConnections = upperBody->getConnections();
  if (upperBodyConnections)
  {
    upperBodyConnections->~ModuleCon();
    NMP::Memory::memFree(upperBodyConnections);
  }
  upperBody->~BodySection();
  NMP::Memory::memFree(upperBody);

  ER::ModuleCon* bodyFrameConnections = bodyFrame->getConnections();
  if (bodyFrameConnections)
  {
    bodyFrameConnections->~ModuleCon();
    NMP::Memory::memFree(bodyFrameConnections);
  }
  bodyFrame->~BodyFrame();
  NMP::Memory::memFree(bodyFrame);

  ER::ModuleCon* environmentAwarenessConnections = environmentAwareness->getConnections();
  if (environmentAwarenessConnections)
  {
    environmentAwarenessConnections->~ModuleCon();
    NMP::Memory::memFree(environmentAwarenessConnections);
  }
  environmentAwareness->~EnvironmentAwareness();
  NMP::Memory::memFree(environmentAwareness);

  ER::ModuleCon* balanceManagementConnections = balanceManagement->getConnections();
  if (balanceManagementConnections)
  {
    balanceManagementConnections->~ModuleCon();
    NMP::Memory::memFree(balanceManagementConnections);
  }
  balanceManagement->~BalanceManagement();
  NMP::Memory::memFree(balanceManagement);

  ER::ModuleCon* hazardManagementConnections = hazardManagement->getConnections();
  if (hazardManagementConnections)
  {
    hazardManagementConnections->~ModuleCon();
    NMP::Memory::memFree(hazardManagementConnections);
  }
  hazardManagement->~HazardManagement();
  NMP::Memory::memFree(hazardManagement);

  ER::ModuleCon* randomLookConnections = randomLook->getConnections();
  if (randomLookConnections)
  {
    randomLookConnections->~ModuleCon();
    NMP::Memory::memFree(randomLookConnections);
  }
  randomLook->~RandomLook();
  NMP::Memory::memFree(randomLook);

  ER::ModuleCon* writheBehaviourInterfaceConnections = writheBehaviourInterface->getConnections();
  if (writheBehaviourInterfaceConnections)
  {
    writheBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(writheBehaviourInterfaceConnections);
  }
  writheBehaviourInterface->~WritheBehaviourInterface();
  NMP::Memory::memFree(writheBehaviourInterface);

  ER::ModuleCon* sitBehaviourInterfaceConnections = sitBehaviourInterface->getConnections();
  if (sitBehaviourInterfaceConnections)
  {
    sitBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(sitBehaviourInterfaceConnections);
  }
  sitBehaviourInterface->~SitBehaviourInterface();
  NMP::Memory::memFree(sitBehaviourInterface);

  ER::ModuleCon* shieldActionBehaviourInterfaceConnections = shieldActionBehaviourInterface->getConnections();
  if (shieldActionBehaviourInterfaceConnections)
  {
    shieldActionBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(shieldActionBehaviourInterfaceConnections);
  }
  shieldActionBehaviourInterface->~ShieldActionBehaviourInterface();
  NMP::Memory::memFree(shieldActionBehaviourInterface);

  ER::ModuleCon* legsPedalBehaviourInterfaceConnections = legsPedalBehaviourInterface->getConnections();
  if (legsPedalBehaviourInterfaceConnections)
  {
    legsPedalBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(legsPedalBehaviourInterfaceConnections);
  }
  legsPedalBehaviourInterface->~LegsPedalBehaviourInterface();
  NMP::Memory::memFree(legsPedalBehaviourInterface);

  ER::ModuleCon* armsWindmillBehaviourInterfaceConnections = armsWindmillBehaviourInterface->getConnections();
  if (armsWindmillBehaviourInterfaceConnections)
  {
    armsWindmillBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(armsWindmillBehaviourInterfaceConnections);
  }
  armsWindmillBehaviourInterface->~ArmsWindmillBehaviourInterface();
  NMP::Memory::memFree(armsWindmillBehaviourInterface);

  ER::ModuleCon* freeFallBehaviourInterfaceConnections = freeFallBehaviourInterface->getConnections();
  if (freeFallBehaviourInterfaceConnections)
  {
    freeFallBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(freeFallBehaviourInterfaceConnections);
  }
  freeFallBehaviourInterface->~FreeFallBehaviourInterface();
  NMP::Memory::memFree(freeFallBehaviourInterface);

  ER::ModuleCon* holdActionBehaviourInterfaceConnections = holdActionBehaviourInterface->getConnections();
  if (holdActionBehaviourInterfaceConnections)
  {
    holdActionBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(holdActionBehaviourInterfaceConnections);
  }
  holdActionBehaviourInterface->~HoldActionBehaviourInterface();
  NMP::Memory::memFree(holdActionBehaviourInterface);

  ER::ModuleCon* holdBehaviourInterfaceConnections = holdBehaviourInterface->getConnections();
  if (holdBehaviourInterfaceConnections)
  {
    holdBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(holdBehaviourInterfaceConnections);
  }
  holdBehaviourInterface->~HoldBehaviourInterface();
  NMP::Memory::memFree(holdBehaviourInterface);

  ER::ModuleCon* shieldBehaviourInterfaceConnections = shieldBehaviourInterface->getConnections();
  if (shieldBehaviourInterfaceConnections)
  {
    shieldBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(shieldBehaviourInterfaceConnections);
  }
  shieldBehaviourInterface->~ShieldBehaviourInterface();
  NMP::Memory::memFree(shieldBehaviourInterface);

  ER::ModuleCon* headAvoidBehaviourInterfaceConnections = headAvoidBehaviourInterface->getConnections();
  if (headAvoidBehaviourInterfaceConnections)
  {
    headAvoidBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(headAvoidBehaviourInterfaceConnections);
  }
  headAvoidBehaviourInterface->~HeadAvoidBehaviourInterface();
  NMP::Memory::memFree(headAvoidBehaviourInterface);

  ER::ModuleCon* lookBehaviourInterfaceConnections = lookBehaviourInterface->getConnections();
  if (lookBehaviourInterfaceConnections)
  {
    lookBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(lookBehaviourInterfaceConnections);
  }
  lookBehaviourInterface->~LookBehaviourInterface();
  NMP::Memory::memFree(lookBehaviourInterface);

  ER::ModuleCon* idleAwakeBehaviourInterfaceConnections = idleAwakeBehaviourInterface->getConnections();
  if (idleAwakeBehaviourInterfaceConnections)
  {
    idleAwakeBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(idleAwakeBehaviourInterfaceConnections);
  }
  idleAwakeBehaviourInterface->~IdleAwakeBehaviourInterface();
  NMP::Memory::memFree(idleAwakeBehaviourInterface);

  ER::ModuleCon* observeBehaviourInterfaceConnections = observeBehaviourInterface->getConnections();
  if (observeBehaviourInterfaceConnections)
  {
    observeBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(observeBehaviourInterfaceConnections);
  }
  observeBehaviourInterface->~ObserveBehaviourInterface();
  NMP::Memory::memFree(observeBehaviourInterface);

  ER::ModuleCon* userHazardBehaviourInterfaceConnections = userHazardBehaviourInterface->getConnections();
  if (userHazardBehaviourInterfaceConnections)
  {
    userHazardBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(userHazardBehaviourInterfaceConnections);
  }
  userHazardBehaviourInterface->~UserHazardBehaviourInterface();
  NMP::Memory::memFree(userHazardBehaviourInterface);

  ER::ModuleCon* hazardAwarenessBehaviourInterfaceConnections = hazardAwarenessBehaviourInterface->getConnections();
  if (hazardAwarenessBehaviourInterfaceConnections)
  {
    hazardAwarenessBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(hazardAwarenessBehaviourInterfaceConnections);
  }
  hazardAwarenessBehaviourInterface->~HazardAwarenessBehaviourInterface();
  NMP::Memory::memFree(hazardAwarenessBehaviourInterface);

  ER::ModuleCon* protectBehaviourInterfaceConnections = protectBehaviourInterface->getConnections();
  if (protectBehaviourInterfaceConnections)
  {
    protectBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(protectBehaviourInterfaceConnections);
  }
  protectBehaviourInterface->~ProtectBehaviourInterface();
  NMP::Memory::memFree(protectBehaviourInterface);

  ER::ModuleCon* balancePoserBehaviourInterfaceConnections = balancePoserBehaviourInterface->getConnections();
  if (balancePoserBehaviourInterfaceConnections)
  {
    balancePoserBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(balancePoserBehaviourInterfaceConnections);
  }
  balancePoserBehaviourInterface->~BalancePoserBehaviourInterface();
  NMP::Memory::memFree(balancePoserBehaviourInterface);

  ER::ModuleCon* balanceBehaviourInterfaceConnections = balanceBehaviourInterface->getConnections();
  if (balanceBehaviourInterfaceConnections)
  {
    balanceBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(balanceBehaviourInterfaceConnections);
  }
  balanceBehaviourInterface->~BalanceBehaviourInterface();
  NMP::Memory::memFree(balanceBehaviourInterface);

  ER::ModuleCon* animateBehaviourInterfaceConnections = animateBehaviourInterface->getConnections();
  if (animateBehaviourInterfaceConnections)
  {
    animateBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(animateBehaviourInterfaceConnections);
  }
  animateBehaviourInterface->~AnimateBehaviourInterface();
  NMP::Memory::memFree(animateBehaviourInterface);

  ER::ModuleCon* reachForWorldBehaviourInterfaceConnections = reachForWorldBehaviourInterface->getConnections();
  if (reachForWorldBehaviourInterfaceConnections)
  {
    reachForWorldBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(reachForWorldBehaviourInterfaceConnections);
  }
  reachForWorldBehaviourInterface->~ReachForWorldBehaviourInterface();
  NMP::Memory::memFree(reachForWorldBehaviourInterface);

  ER::ModuleCon* reachForBodyBehaviourInterfaceConnections = reachForBodyBehaviourInterface->getConnections();
  if (reachForBodyBehaviourInterfaceConnections)
  {
    reachForBodyBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(reachForBodyBehaviourInterfaceConnections);
  }
  reachForBodyBehaviourInterface->~ReachForBodyBehaviourInterface();
  NMP::Memory::memFree(reachForBodyBehaviourInterface);

  ER::ModuleCon* headDodgeBehaviourInterfaceConnections = headDodgeBehaviourInterface->getConnections();
  if (headDodgeBehaviourInterfaceConnections)
  {
    headDodgeBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(headDodgeBehaviourInterfaceConnections);
  }
  headDodgeBehaviourInterface->~HeadDodgeBehaviourInterface();
  NMP::Memory::memFree(headDodgeBehaviourInterface);

  ER::ModuleCon* aimBehaviourInterfaceConnections = aimBehaviourInterface->getConnections();
  if (aimBehaviourInterfaceConnections)
  {
    aimBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(aimBehaviourInterfaceConnections);
  }
  aimBehaviourInterface->~AimBehaviourInterface();
  NMP::Memory::memFree(aimBehaviourInterface);

  ER::ModuleCon* armsPlacementBehaviourInterfaceConnections = armsPlacementBehaviourInterface->getConnections();
  if (armsPlacementBehaviourInterfaceConnections)
  {
    armsPlacementBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(armsPlacementBehaviourInterfaceConnections);
  }
  armsPlacementBehaviourInterface->~ArmsPlacementBehaviourInterface();
  NMP::Memory::memFree(armsPlacementBehaviourInterface);

  ER::ModuleCon* armsBraceBehaviourInterfaceConnections = armsBraceBehaviourInterface->getConnections();
  if (armsBraceBehaviourInterfaceConnections)
  {
    armsBraceBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(armsBraceBehaviourInterfaceConnections);
  }
  armsBraceBehaviourInterface->~ArmsBraceBehaviourInterface();
  NMP::Memory::memFree(armsBraceBehaviourInterface);

  ER::ModuleCon* eyesBehaviourInterfaceConnections = eyesBehaviourInterface->getConnections();
  if (eyesBehaviourInterfaceConnections)
  {
    eyesBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(eyesBehaviourInterfaceConnections);
  }
  eyesBehaviourInterface->~EyesBehaviourInterface();
  NMP::Memory::memFree(eyesBehaviourInterface);

  ER::ModuleCon* euphoriaRagdollBehaviourInterfaceConnections = euphoriaRagdollBehaviourInterface->getConnections();
  if (euphoriaRagdollBehaviourInterfaceConnections)
  {
    euphoriaRagdollBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(euphoriaRagdollBehaviourInterfaceConnections);
  }
  euphoriaRagdollBehaviourInterface->~EuphoriaRagdollBehaviourInterface();
  NMP::Memory::memFree(euphoriaRagdollBehaviourInterface);

  ER::ModuleCon* propertiesBehaviourInterfaceConnections = propertiesBehaviourInterface->getConnections();
  if (propertiesBehaviourInterfaceConnections)
  {
    propertiesBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(propertiesBehaviourInterfaceConnections);
  }
  propertiesBehaviourInterface->~PropertiesBehaviourInterface();
  NMP::Memory::memFree(propertiesBehaviourInterface);

  ER::ModuleCon* characteristicsBehaviourInterfaceConnections = characteristicsBehaviourInterface->getConnections();
  if (characteristicsBehaviourInterfaceConnections)
  {
    characteristicsBehaviourInterfaceConnections->~ModuleCon();
    NMP::Memory::memFree(characteristicsBehaviourInterfaceConnections);
  }
  characteristicsBehaviourInterface->~CharacteristicsBehaviourInterface();
  NMP::Memory::memFree(characteristicsBehaviourInterface);

  memset(m_childModules, 0, sizeof(ER::Module*) * getNumChildren());

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_allModules);
  m_allModules = 0;
  m_numModules = 0;
  NMP::Memory::memFree(m_moduleEnableStates);
  m_moduleEnableStates = 0;
  m_mdAllocator.term();
}

//----------------------------------------------------------------------------------------------------------------------
void MyNetwork::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  if (getConnections())
    getConnections()->create(this, 0);

  characteristicsBehaviourInterface->create(this);
  propertiesBehaviourInterface->create(this);
  euphoriaRagdollBehaviourInterface->create(this);
  eyesBehaviourInterface->create(this);
  armsBraceBehaviourInterface->create(this);
  armsPlacementBehaviourInterface->create(this);
  aimBehaviourInterface->create(this);
  headDodgeBehaviourInterface->create(this);
  reachForBodyBehaviourInterface->create(this);
  reachForWorldBehaviourInterface->create(this);
  animateBehaviourInterface->create(this);
  balanceBehaviourInterface->create(this);
  balancePoserBehaviourInterface->create(this);
  protectBehaviourInterface->create(this);
  hazardAwarenessBehaviourInterface->create(this);
  userHazardBehaviourInterface->create(this);
  observeBehaviourInterface->create(this);
  idleAwakeBehaviourInterface->create(this);
  lookBehaviourInterface->create(this);
  headAvoidBehaviourInterface->create(this);
  shieldBehaviourInterface->create(this);
  holdBehaviourInterface->create(this);
  holdActionBehaviourInterface->create(this);
  freeFallBehaviourInterface->create(this);
  armsWindmillBehaviourInterface->create(this);
  legsPedalBehaviourInterface->create(this);
  shieldActionBehaviourInterface->create(this);
  sitBehaviourInterface->create(this);
  writheBehaviourInterface->create(this);
  randomLook->create(this);
  hazardManagement->create(this);
  balanceManagement->create(this);
  environmentAwareness->create(this);
  bodyFrame->create(this);
  upperBody->create(this);
  lowerBody->create(this);
  for (int i=0; i<2; i++)
    arms[i]->create(this, i);
  for (int i=0; i<1; i++)
    heads[i]->create(this, i);
  for (int i=0; i<2; i++)
    legs[i]->create(this, i);
  for (int i=0; i<1; i++)
    spines[i]->create(this, i);
  sceneProbes->create(this);
  balanceBehaviourFeedback->create(this);

  new(m_apiBase) MyNetworkAPIBase( data);
}

//----------------------------------------------------------------------------------------------------------------------
void MyNetwork::clearAllData()
{
  data->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool MyNetwork::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool MyNetwork::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<MyNetworkData>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* MyNetwork::getChildName(int32_t index) const
{
  switch (index)
  {
    case 0: return "characteristicsBehaviourInterface";
    case 1: return "propertiesBehaviourInterface";
    case 2: return "euphoriaRagdollBehaviourInterface";
    case 3: return "eyesBehaviourInterface";
    case 4: return "armsBraceBehaviourInterface";
    case 5: return "armsPlacementBehaviourInterface";
    case 6: return "aimBehaviourInterface";
    case 7: return "headDodgeBehaviourInterface";
    case 8: return "reachForBodyBehaviourInterface";
    case 9: return "reachForWorldBehaviourInterface";
    case 10: return "animateBehaviourInterface";
    case 11: return "balanceBehaviourInterface";
    case 12: return "balancePoserBehaviourInterface";
    case 13: return "protectBehaviourInterface";
    case 14: return "hazardAwarenessBehaviourInterface";
    case 15: return "userHazardBehaviourInterface";
    case 16: return "observeBehaviourInterface";
    case 17: return "idleAwakeBehaviourInterface";
    case 18: return "lookBehaviourInterface";
    case 19: return "headAvoidBehaviourInterface";
    case 20: return "shieldBehaviourInterface";
    case 21: return "holdBehaviourInterface";
    case 22: return "holdActionBehaviourInterface";
    case 23: return "freeFallBehaviourInterface";
    case 24: return "armsWindmillBehaviourInterface";
    case 25: return "legsPedalBehaviourInterface";
    case 26: return "shieldActionBehaviourInterface";
    case 27: return "sitBehaviourInterface";
    case 28: return "writheBehaviourInterface";
    case 29: return "randomLook";
    case 30: return "hazardManagement";
    case 31: return "balanceManagement";
    case 32: return "environmentAwareness";
    case 33: return "bodyFrame";
    case 34: return "upperBody";
    case 35: return "lowerBody";
    case 36: return "arms[0]";
    case 37: return "arms[1]";
    case 38: return "heads[0]";
    case 39: return "legs[0]";
    case 40: return "legs[1]";
    case 41: return "spines[0]";
    case 42: return "sceneProbes";
    case 43: return "balanceBehaviourFeedback";
    default: NMP_ASSERT_FAIL();
  }
  return 0;
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


