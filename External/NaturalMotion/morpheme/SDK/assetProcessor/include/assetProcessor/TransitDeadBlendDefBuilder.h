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
#ifndef MR_TRANSIT_DEADBLEND_BUILDER_H
#define MR_TRANSIT_DEADBLEND_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "AssetProcessor.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::TransitDeadBlendDefBuilder
/// \brief For construction of TransitDeadBlend in the asset compiler - pure virtual base class
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class TransitDeadBlendDefBuilder
{
public:
  virtual NMP::Memory::Format getNodeDefMemoryRequirements(
    const ME::NodeExport*        nodeDefExport,
    const ME::NetworkDefExport*  netDefExport,
    AssetProcessor*              processor) = 0;

  /// \brief Initialise the dead blend attribute data from connects xml data.
  virtual MR::AttribData* initAttribData(
    NMP::Memory::Resource&       memRes,
    const ME::NodeExport*        nodeDefExport,
    const ME::NetworkDefExport*  netDefExport,
    AssetProcessor*              processor) = 0;

private:
};

//----------------------------------------------------------------------------------------------------------------------
bool autoAddDeadBlendDef(ME::DataBlockExport* datablock, uint32_t sourceNodeID);

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format getAttribDataDeadBlendDefMemoryRequirements(
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor);

//----------------------------------------------------------------------------------------------------------------------
MR::AttribData* initAttribDataDeadBlendDef(
  NMP::Memory::Resource&      memRes,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor);

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRANSIT_DEADBLEND_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
