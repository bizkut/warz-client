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
#include "morpheme/mrMessageDistributor.h"
#include "morpheme/mrManager.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
//----------------------------------------------------------------------------------------------------------------------
// Message functions.
//----------------------------------------------------------------------------------------------------------------------
void Message::locate()
{
  NMP::endianSwap(m_dataSize);
  NMP::endianSwap(m_id);
  NMP::endianSwap(m_type);

  if (m_dataSize > 0)
  {
    REFIX_SWAP_PTR(void*, m_data);

    MessageDataLocateFn locateData = MR::Manager::getInstance().getMessageDataLocateFn(m_type);
    NMP_ASSERT(locateData);

#if defined(NMP_ENABLE_ASSERTS)
    bool result =
#endif
      locateData(this);
    NMP_ASSERT(result);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Message::dislocate()
{
  if (m_dataSize > 0)
  {
    MessageDataDislocateFn dislocateData = MR::Manager::getInstance().getMessageDataDislocateFn(m_type);
    NMP_ASSERT(dislocateData);

#if defined(NMP_ENABLE_ASSERTS)
    bool result =
#endif
      dislocateData(this);
    NMP_ASSERT(result);

    UNFIX_SWAP_PTR(void*, m_data);
  }
  NMP::endianSwap(m_dataSize);

  NMP::endianSwap(m_id);
  NMP::endianSwap(m_type);
}

//----------------------------------------------------------------------------------------------------------------------
// MR::MessageDistributor functions.
//----------------------------------------------------------------------------------------------------------------------
uint32_t MessageDistributor::getNumMessagePresets() const
{
  return m_numMessagePresets;
}

//----------------------------------------------------------------------------------------------------------------------
const Message* MessageDistributor::getMessagePreset(uint32_t index) const
{
  NMP_ASSERT(index < m_numMessagePresets);
  return m_messagePresets[index];
}

//----------------------------------------------------------------------------------------------------------------------
void MessageDistributor::locate()
{
  NMP::endianSwap(m_messageID);
  NMP::endianSwap(m_numNodeIDs);

  if (m_nodeIDs)
  {
    REFIX_SWAP_PTR(NodeID, m_nodeIDs);
    NMP::endianSwapArray(m_nodeIDs, m_numNodeIDs);
  }

  NMP::endianSwap(m_numMessagePresets);

  //-------------------------
  // it is possible for a message to have no presets
  if (m_messagePresets)
  {
    REFIX_SWAP_PTR(Message*, m_messagePresets);
    for (uint32_t i = 0; i != m_numMessagePresets; ++i)
    {
      REFIX_SWAP_PTR(Message, m_messagePresets[i]);
      m_messagePresets[i]->locate();
    }
  }

  //-------------------------
  // string table is not needed for final builds
  if (m_messagePresetIndexNamesTable)
  {
    REFIX_SWAP_PTR(NMP::OrderedStringTable, m_messagePresetIndexNamesTable);
    m_messagePresetIndexNamesTable->locate();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void MessageDistributor::dislocate()
{
  //-------------------------
  // string table is not needed for final builds
  if (m_messagePresetIndexNamesTable)
  {
    m_messagePresetIndexNamesTable->dislocate();
    UNFIX_SWAP_PTR(NMP::OrderedStringTable, m_messagePresetIndexNamesTable);
  }

  //-------------------------
  // it is possible for a message to have no presets
  if (m_messagePresets)
  {
    for (uint32_t i = 0; i != m_numMessagePresets; ++i)
    {
      m_messagePresets[i]->dislocate();
      UNFIX_SWAP_PTR(Message, m_messagePresets[i]);
    }
    UNFIX_SWAP_PTR(Message*, m_messagePresets);
  }

  NMP::endianSwap(m_numMessagePresets);

  if (m_nodeIDs)
  {
    NMP::endianSwapArray(m_nodeIDs, m_numNodeIDs);
    UNFIX_SWAP_PTR(NodeID, m_nodeIDs);
  }

  NMP::endianSwap(m_numNodeIDs);
  NMP::endianSwap(m_messageID);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
