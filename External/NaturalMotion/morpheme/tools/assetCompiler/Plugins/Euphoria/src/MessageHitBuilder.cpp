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
#include "MessageHitBuilder.h"

#include "euphoria/Nodes/erNodeOperatorHit.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{
//----------------------------------------------------------------------------------------------------------------------
// MessageHitBuilder
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format MessageHitBuilder::getMemoryRequirements(
  const ME::MessageExport*        NMP_UNUSED(messageExport),
  const ME::MessagePresetExport*  NMP_UNUSED(messagePresetExport),
  AssetProcessor*                 NMP_UNUSED(processor))
{
  NMP::Memory::Format result(sizeof(MR::Message), NMP_NATURAL_TYPE_ALIGNMENT);
  result += NMP::Memory::Format(sizeof(ER::HitMessageData), NMP_NATURAL_TYPE_ALIGNMENT);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::Message* MessageHitBuilder::init(
  NMP::Memory::Resource&          memRes,
  const ME::MessageExport*        messageExport,
  const ME::MessagePresetExport*  messagePresetExport,
  AssetProcessor*                 NMP_UNUSED(processor))
{
  NMP::Memory::Format memReqs;

  memReqs.set(sizeof(MR::Message), NMP_NATURAL_TYPE_ALIGNMENT);
  MR::Message* message = (MR::Message*)memRes.alignAndIncrement(memReqs);

  memReqs.set(sizeof(ER::HitMessageData), NMP_NATURAL_TYPE_ALIGNMENT);
  message->m_data = memRes.alignAndIncrement(memReqs);
  message->m_dataSize = (uint32_t)memReqs.size;

  message->m_id = messageExport->getMessageID();
  message->m_type = MESSAGE_TYPE_HIT;
  message->m_status = true;

  ER::HitMessageData* messageData = (ER::HitMessageData*)message->m_data;
  messageData->reset();

  if (messagePresetExport)
  {
    const ME::DataBlockExport* dataBlockExport = messagePresetExport->getDataBlock();

    dataBlockExport->readInt(messageData->m_rigPartIndex, "RigPartIndex");

    dataBlockExport->readFloat(messageData->m_hitPointLocalX, "HitPointLocalX");
    dataBlockExport->readFloat(messageData->m_hitPointLocalY, "HitPointLocalY");
    dataBlockExport->readFloat(messageData->m_hitPointLocalZ, "HitPointLocalZ");
    dataBlockExport->readFloat(messageData->m_hitNormalLocalX, "HitNormalLocalX");
    dataBlockExport->readFloat(messageData->m_hitNormalLocalY, "HitNormalLocalY");
    dataBlockExport->readFloat(messageData->m_hitNormalLocalZ, "HitNormalLocalZ");
    dataBlockExport->readFloat(messageData->m_hitDirectionLocalX, "HitDirectionLocalX");
    dataBlockExport->readFloat(messageData->m_hitDirectionLocalY, "HitDirectionLocalY");
    dataBlockExport->readFloat(messageData->m_hitDirectionLocalZ, "HitDirectionLocalZ");
    dataBlockExport->readFloat(messageData->m_hitDirectionWorldX, "HitDirectionWorldX");
    dataBlockExport->readFloat(messageData->m_hitDirectionLocalY, "HitDirectionWorldY");
    dataBlockExport->readFloat(messageData->m_hitDirectionLocalZ, "HitDirectionWorldZ");

    dataBlockExport->readFloat(messageData->m_sourcePointWorldX, "SourcePointWorldX");
    dataBlockExport->readFloat(messageData->m_sourcePointWorldY, "SourcePointWorldY");
    dataBlockExport->readFloat(messageData->m_sourcePointWorldZ, "SourcePointWorldZ");

    dataBlockExport->readInt(messageData->m_priority, "Priority");

    dataBlockExport->readFloat(messageData->m_reachDelay, "ReachDelay");
    dataBlockExport->readFloat(messageData->m_reachDuration, "ReachDuration");
    dataBlockExport->readFloat(messageData->m_maxReachSpeed, "MaxReachSpeed");
    dataBlockExport->readFloat(messageData->m_reflexAnimStart, "ReflexAnimStart");
    dataBlockExport->readFloat(messageData->m_reflexAnimRampDuration, "ReflexAnimRampDuration");
    dataBlockExport->readFloat(messageData->m_reflexAnimFullDuration, "ReflexAnimFullDuration");
    dataBlockExport->readFloat(messageData->m_reflexAnimDerampDuration, "ReflexAnimDerampDuration");
    dataBlockExport->readFloat(messageData->m_reflexAnimMaxWeight, "ReflexAnimMaxWeight");
    dataBlockExport->readFloat(messageData->m_reflexLookDuration, "ReflexLookDuration");
    dataBlockExport->readFloat(messageData->m_deathTriggerDelay, "DeathTriggerDelay");
    dataBlockExport->readFloat(messageData->m_deathRelaxDuration, "DeathRelaxDuration");

    dataBlockExport->readFloat(messageData->m_expandLimitsFullDuration, "ExpandLimitsMaxDuration");
    dataBlockExport->readFloat(messageData->m_expandLimitsDerampDuration, "ExpandLimitsDerampDuration");
    dataBlockExport->readFloat(messageData->m_expandLimitsMaxExpansion, "ExpandLimitsMaxExpansion");

    dataBlockExport->readFloat(messageData->m_partImpactMagnitude, "ShotPartImpactMagnitude");
    dataBlockExport->readFloat(messageData->m_partResponseRatio, "PartResponseRatio");
    dataBlockExport->readFloat(messageData->m_bodyImpactMagnitude, "ShotBodyImpactMagnitude");
    dataBlockExport->readFloat(messageData->m_bodyResponseRatio, "BodyResponseRatio");
    dataBlockExport->readFloat(messageData->m_torqueMultiplier, "ShotTorqueMultiplier");
    dataBlockExport->readFloat(messageData->m_desiredMinimumLift, "ShotLiftBoost");
    dataBlockExport->readFloat(messageData->m_liftResponseRatio, "BodyLiftResponseRatio");
    dataBlockExport->readFloat(messageData->m_impulseYield, "ShotImpulseYield");
    dataBlockExport->readFloat(messageData->m_impulseYieldDuration, "ImpulseYieldDuration");

    dataBlockExport->readFloat(messageData->m_impulseTargetStepSpeed, "ImpulseBalanceErrorMagnitude");
    dataBlockExport->readFloat(messageData->m_impulseTargetStepDuration, "ImpulseBalanceErrorDuration");
    dataBlockExport->readFloat(messageData->m_balanceAssistance, "BalanceAssistance");
    dataBlockExport->readFloat(messageData->m_impulseLegStrengthReduction, "ImpulseLegStrengthReduction");
    dataBlockExport->readFloat(messageData->m_impulseLegStrengthReductionDuration, "ImpulseLegStrengthReductionDuration");
    dataBlockExport->readFloat(messageData->m_deathTargetStepSpeed, "ShotDeathBalanceErrorMagnitude");
    dataBlockExport->readFloat(messageData->m_deathBalanceStrength, "ShotDeathBalanceStrength");

    dataBlockExport->readInt(messageData->m_reachSku, "ReachSku");
    dataBlockExport->readInt(messageData->m_reflexAnim, "ReflexAnim");

    dataBlockExport->readBool(messageData->m_reachSwitch, "ReachSwitch");
    dataBlockExport->readBool(messageData->m_reflexAnimSwitch, "ReflexAnimSwitch");
    dataBlockExport->readBool(messageData->m_reflexLookSwitch, "ReflexLookSwitch");
    dataBlockExport->readBool(messageData->m_forcedFalldownSwitch, "ForcedFalldownSwitch");
    dataBlockExport->readFloat(messageData->m_targetTimeBeforeFalldown, "TargetTimeBeforeFalldown");
    dataBlockExport->readInt(messageData->m_targetNumberOfStepsBeforeFalldown, "TargetNumberOfStepsBeforeFalldown");
    dataBlockExport->readBool(messageData->m_impulseDirWorldOrLocal, "ImpulseDirWorldOrLocal");
    dataBlockExport->readBool(messageData->m_lookAtWoundOrHitSource, "LookAtWoundOrShotSource");
    dataBlockExport->readBool(messageData->m_deathTrigger, "DeathTrigger");
    dataBlockExport->readBool(messageData->m_deathTriggerOnHeadhit, "DeathTriggerOnHeadshot");
  }

  return message;
}

} // namespace AP
