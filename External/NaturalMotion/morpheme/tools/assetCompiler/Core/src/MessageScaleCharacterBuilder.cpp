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
#include "MessageScaleCharacterBuilder.h"

#include "morpheme/Nodes/mrNodeScaleCharacter.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{
  //----------------------------------------------------------------------------------------------------------------------
  // MessageScaleCharacterBuilder
  //----------------------------------------------------------------------------------------------------------------------
  NMP::Memory::Format MessageScaleCharacterBuilder::getMemoryRequirements(
    const ME::MessageExport*        NMP_UNUSED(messageExport),
    const ME::MessagePresetExport*  messagePresetExport,
    AssetProcessor*                 NMP_UNUSED(processor))
  {
    const ME::DataBlockExport* dataBlockExport = messagePresetExport->getDataBlock();
    NMP::Memory::Format result(sizeof(MR::Message), NMP_NATURAL_TYPE_ALIGNMENT);

    bool perJoint;
    bool readResult = dataBlockExport->readBool(perJoint, "PerJoint");
    NMP_VERIFY_MSG(readResult,  "Failed to find \"PerJoint\" attribute in scale message preset.");

    if(perJoint)
    {
      size_t floatArraySize = messagePresetExport->getDataBlock()->getDataBlockSize() - sizeof(bool);
      result += NMP::Memory::Format(sizeof(bool), 4);
      result += NMP::Memory::Format(floatArraySize, NMP_NATURAL_TYPE_ALIGNMENT);
    }
    else
    {
      result += NMP::Memory::Format(sizeof(bool), 4);
      result += NMP::Memory::Format(sizeof(float) * 2, NMP_NATURAL_TYPE_ALIGNMENT);
    }
    return result;
  }

  //----------------------------------------------------------------------------------------------------------------------
  MR::Message* MessageScaleCharacterBuilder::init(
    NMP::Memory::Resource&          memRes,
    const ME::MessageExport*        messageExport,
    const ME::MessagePresetExport*  messagePresetExport,
    AssetProcessor*                 NMP_USED_FOR_ASSERTS(processor))
  {
    NMP_USED_FOR_ASSERTS(const size_t initialMemResSize = memRes.format.size);

    const ME::DataBlockExport* dataBlockExport = messagePresetExport->getDataBlock();

    bool perJoint;
    bool readResult = dataBlockExport->readBool(perJoint, "PerJoint");

    NMP_VERIFY_MSG(readResult, "Failed to find \"PerJoint\" attribute in scale message preset.");

    // The data block consists of floats except for one bool.
    uint32_t numFloatsInMessage = (messagePresetExport->getDataBlock()->getDataBlockSize() - sizeof(bool)) / sizeof(float);

    NMP::Memory::Format memReqs;
    memReqs.set(sizeof(MR::Message), NMP_NATURAL_TYPE_ALIGNMENT);

    //------------------------
    // Construct the message
    MR::Message* message = (MR::Message*)memRes.alignAndIncrement(memReqs);
    message->m_dataSize = (uint32_t) NMP::Memory::align((numFloatsInMessage * sizeof(float)) + sizeof(bool), NMP_NATURAL_TYPE_ALIGNMENT);
    message->m_id = messageExport->getMessageID();
    message->m_type = MESSAGE_TYPE_SCALE_CHARACTER;
    message->m_status = true;

    //------------------------
    // Construct the message data

    // Write the per joint flag into the first byte of the message.
    memReqs.set(sizeof(bool), NMP_NATURAL_TYPE_ALIGNMENT);
    message->m_data = memRes.alignAndIncrement(memReqs);
    *(bool*)message->m_data = perJoint;

    // Write the floats into the remaining message memory
    memReqs.set(sizeof(float) * numFloatsInMessage, NMP_NATURAL_TYPE_ALIGNMENT);
    float* perJointScales = (float*)memRes.alignAndIncrement(memReqs);
    if (perJoint)
    {
      size_t outLength = 0;
      dataBlockExport->readFloatArray(perJointScales, numFloatsInMessage, outLength, "PerJointScales");
      NMP_VERIFY_MSG(outLength == numFloatsInMessage, "Unexpected source array size when reading per joint scales.");
    }
    else
    {
      readResult = dataBlockExport->readFloat(perJointScales[0], "SpeedScale");
      NMP_VERIFY_MSG(readResult, "Failed to find \"SpeedScale\" attribute in scale message preset.");

      readResult = dataBlockExport->readFloat(perJointScales[1], "CharacterScale");
      NMP_VERIFY_MSG(readResult, "Failed to find \"CharacterScale\" attribute in scale message preset.");
    }

    // Make sure all memory requested was used
    NMP_ASSERT_MSG(
      (initialMemResSize - getMemoryRequirements(messageExport, messagePresetExport, processor).size) == memRes.format.size,
      "MessageScaleCharacterBuilder didn't use the same amount of memory as requested.");

    return message;
  }

} // namespace AP
