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

#include "NetworkDescriptor.h"
#include "MyNetwork.h"
#include "Behaviours/AimBehaviour.h"
#include "Behaviours/AnimateBehaviour.h"
#include "Behaviours/ArmsBraceBehaviour.h"
#include "Behaviours/ArmsPlacementBehaviour.h"
#include "Behaviours/ArmsWindmillBehaviour.h"
#include "Behaviours/BalanceBehaviour.h"
#include "Behaviours/BalancePoserBehaviour.h"
#include "Behaviours/CharacteristicsBehaviour.h"
#include "Behaviours/EuphoriaRagdollBehaviour.h"
#include "Behaviours/EyesBehaviour.h"
#include "Behaviours/FreeFallBehaviour.h"
#include "Behaviours/HazardAwarenessBehaviour.h"
#include "Behaviours/HeadAvoidBehaviour.h"
#include "Behaviours/HeadDodgeBehaviour.h"
#include "Behaviours/HoldBehaviour.h"
#include "Behaviours/HoldActionBehaviour.h"
#include "Behaviours/IdleAwakeBehaviour.h"
#include "Behaviours/LegsPedalBehaviour.h"
#include "Behaviours/LookBehaviour.h"
#include "Behaviours/ObserveBehaviour.h"
#include "Behaviours/PropertiesBehaviour.h"
#include "Behaviours/ProtectBehaviour.h"
#include "Behaviours/ReachForBodyBehaviour.h"
#include "Behaviours/ReachForWorldBehaviour.h"
#include "Behaviours/ShieldBehaviour.h"
#include "Behaviours/ShieldActionBehaviour.h"
#include "Behaviours/SitBehaviour.h"
#include "Behaviours/UserHazardBehaviour.h"
#include "Behaviours/WritheBehaviour.h"

#ifdef _WINDLL
static NM_BEHAVIOUR_LIB_NAMESPACE::NMBehaviourLibrary gNMBehaviourLibrary_BADF000D;
__declspec(dllexport) ER::NetworkInterface* getNetworkInterface() { return &gNMBehaviourLibrary_BADF000D; }
#endif // _WINDLL

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

typedef ER::BehaviourDefs<29> BehaviourDefsOut;

//----------------------------------------------------------------------------------------------------------------------
const ER::NetworkInterface::Description& NMBehaviourLibrary::getDesc() const
{
  const static ER::NetworkInterface::Description ndOut = { 0xBADF000D , NetworkMetrics::numLimbs };
  return ndOut;
}

//----------------------------------------------------------------------------------------------------------------------
void NMBehaviourLibrary::init()
{

  NMP_ASSERT(m_defs == 0);
  BehaviourDefsOut *bDefs = (BehaviourDefsOut*)NMPMemoryAllocateFromFormat(BehaviourDefsOut::getMemoryRequirements()).ptr;
  new (bDefs) BehaviourDefsOut();
  m_defs = bDefs;

  bDefs->m_defs[0] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(AimBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[0]) AimBehaviourDef();

  bDefs->m_defs[1] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(AnimateBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[1]) AnimateBehaviourDef();

  bDefs->m_defs[2] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(ArmsBraceBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[2]) ArmsBraceBehaviourDef();

  bDefs->m_defs[3] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(ArmsPlacementBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[3]) ArmsPlacementBehaviourDef();

  bDefs->m_defs[4] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(ArmsWindmillBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[4]) ArmsWindmillBehaviourDef();

  bDefs->m_defs[5] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(BalanceBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[5]) BalanceBehaviourDef();

  bDefs->m_defs[6] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(BalancePoserBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[6]) BalancePoserBehaviourDef();

  bDefs->m_defs[7] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(CharacteristicsBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[7]) CharacteristicsBehaviourDef();

  bDefs->m_defs[8] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(EuphoriaRagdollBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[8]) EuphoriaRagdollBehaviourDef();

  bDefs->m_defs[9] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(EyesBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[9]) EyesBehaviourDef();

  bDefs->m_defs[10] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(FreeFallBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[10]) FreeFallBehaviourDef();

  bDefs->m_defs[11] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(HazardAwarenessBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[11]) HazardAwarenessBehaviourDef();

  bDefs->m_defs[12] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(HeadAvoidBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[12]) HeadAvoidBehaviourDef();

  bDefs->m_defs[13] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(HeadDodgeBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[13]) HeadDodgeBehaviourDef();

  bDefs->m_defs[14] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(HoldBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[14]) HoldBehaviourDef();

  bDefs->m_defs[15] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(HoldActionBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[15]) HoldActionBehaviourDef();

  bDefs->m_defs[16] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(IdleAwakeBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[16]) IdleAwakeBehaviourDef();

  bDefs->m_defs[17] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(LegsPedalBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[17]) LegsPedalBehaviourDef();

  bDefs->m_defs[18] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(LookBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[18]) LookBehaviourDef();

  bDefs->m_defs[19] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(ObserveBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[19]) ObserveBehaviourDef();

  bDefs->m_defs[20] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(PropertiesBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[20]) PropertiesBehaviourDef();

  bDefs->m_defs[21] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(ProtectBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[21]) ProtectBehaviourDef();

  bDefs->m_defs[22] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(ReachForBodyBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[22]) ReachForBodyBehaviourDef();

  bDefs->m_defs[23] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(ReachForWorldBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[23]) ReachForWorldBehaviourDef();

  bDefs->m_defs[24] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(ShieldBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[24]) ShieldBehaviourDef();

  bDefs->m_defs[25] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(ShieldActionBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[25]) ShieldActionBehaviourDef();

  bDefs->m_defs[26] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(SitBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[26]) SitBehaviourDef();

  bDefs->m_defs[27] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(UserHazardBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[27]) UserHazardBehaviourDef();

  bDefs->m_defs[28] = (ER::BehaviourDef*)NMPMemoryAllocateFromFormat(WritheBehaviourDef::getMemoryRequirements()).ptr;
  new (bDefs->m_defs[28]) WritheBehaviourDef();

}

//----------------------------------------------------------------------------------------------------------------------
ER::RootModule* NMBehaviourLibrary::createNetwork() const
{
  // ensure ::init() has been called to configure the memory system
  NMP_ASSERT(NMP::Memory::isInitialised());

  // construct the network; new'ing the root module will create all of its
  // children in turn
  MyNetwork *ModuleInst = (MyNetwork*)NMPMemoryAllocateFromFormat(MyNetwork::getMemoryRequirements()).ptr;
  new (ModuleInst) MyNetwork();
  ER::RootModule* rootModuleOut = ModuleInst;

  return rootModuleOut;
}

//----------------------------------------------------------------------------------------------------------------------
bool NMBehaviourLibrary::destroyNetwork(ER::Module** rootModuleOut) const
{
  // ensure ::init() has been called to configure the memory system
  NMP_ASSERT(NMP::Memory::isInitialised());

  // destroy the network; free'ing the root module will free all of its
  // children in turn
  MyNetwork* rootNetwork = (MyNetwork*)*rootModuleOut;
  rootNetwork->~MyNetwork();
  NMP::Memory::memFree(rootNetwork);
  (*rootModuleOut) = 0;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void NMBehaviourLibrary::term()
{
  NMP_ASSERT(m_defs != 0);
  BehaviourDefsOut *bDefs = (BehaviourDefsOut*)m_defs;

  ((WritheBehaviourDef*)bDefs->m_defs[28])->~WritheBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[28]);

  ((UserHazardBehaviourDef*)bDefs->m_defs[27])->~UserHazardBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[27]);

  ((SitBehaviourDef*)bDefs->m_defs[26])->~SitBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[26]);

  ((ShieldActionBehaviourDef*)bDefs->m_defs[25])->~ShieldActionBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[25]);

  ((ShieldBehaviourDef*)bDefs->m_defs[24])->~ShieldBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[24]);

  ((ReachForWorldBehaviourDef*)bDefs->m_defs[23])->~ReachForWorldBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[23]);

  ((ReachForBodyBehaviourDef*)bDefs->m_defs[22])->~ReachForBodyBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[22]);

  ((ProtectBehaviourDef*)bDefs->m_defs[21])->~ProtectBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[21]);

  ((PropertiesBehaviourDef*)bDefs->m_defs[20])->~PropertiesBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[20]);

  ((ObserveBehaviourDef*)bDefs->m_defs[19])->~ObserveBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[19]);

  ((LookBehaviourDef*)bDefs->m_defs[18])->~LookBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[18]);

  ((LegsPedalBehaviourDef*)bDefs->m_defs[17])->~LegsPedalBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[17]);

  ((IdleAwakeBehaviourDef*)bDefs->m_defs[16])->~IdleAwakeBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[16]);

  ((HoldActionBehaviourDef*)bDefs->m_defs[15])->~HoldActionBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[15]);

  ((HoldBehaviourDef*)bDefs->m_defs[14])->~HoldBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[14]);

  ((HeadDodgeBehaviourDef*)bDefs->m_defs[13])->~HeadDodgeBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[13]);

  ((HeadAvoidBehaviourDef*)bDefs->m_defs[12])->~HeadAvoidBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[12]);

  ((HazardAwarenessBehaviourDef*)bDefs->m_defs[11])->~HazardAwarenessBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[11]);

  ((FreeFallBehaviourDef*)bDefs->m_defs[10])->~FreeFallBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[10]);

  ((EyesBehaviourDef*)bDefs->m_defs[9])->~EyesBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[9]);

  ((EuphoriaRagdollBehaviourDef*)bDefs->m_defs[8])->~EuphoriaRagdollBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[8]);

  ((CharacteristicsBehaviourDef*)bDefs->m_defs[7])->~CharacteristicsBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[7]);

  ((BalancePoserBehaviourDef*)bDefs->m_defs[6])->~BalancePoserBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[6]);

  ((BalanceBehaviourDef*)bDefs->m_defs[5])->~BalanceBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[5]);

  ((ArmsWindmillBehaviourDef*)bDefs->m_defs[4])->~ArmsWindmillBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[4]);

  ((ArmsPlacementBehaviourDef*)bDefs->m_defs[3])->~ArmsPlacementBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[3]);

  ((ArmsBraceBehaviourDef*)bDefs->m_defs[2])->~ArmsBraceBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[2]);

  ((AnimateBehaviourDef*)bDefs->m_defs[1])->~AnimateBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[1]);

  ((AimBehaviourDef*)bDefs->m_defs[0])->~AimBehaviourDef();
  NMP::Memory::memFree(bDefs->m_defs[0]);

  bDefs->~BehaviourDefsOut();
  NMP::Memory::memFree(bDefs);
  m_defs = 0;

  ER::NetworkInterface::term();
}

//----------------------------------------------------------------------------------------------------------------------
const char* NetworkMetrics::getLimbName(LimbIndex index) const
{
  switch (index)
  {
    case Arm_1: return "Arm_1"; 
    case Arm_2: return "Arm_2"; 
    case Head_1: return "Head_1"; 
    case Leg_1: return "Leg_1"; 
    case Leg_2: return "Leg_2"; 
    case Spine_1: return "Spine_1"; 

  default:
    NMP_ASSERT_FAIL();
    break;
  }
  return 0;
}
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

