/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_InputCellBlob_H
#define GwNavGen_InputCellBlob_H


#include "gwnavruntime/base/endianness.h"
#include "gwnavruntime/basesystem/coordsystem.h"
#include "gwnavruntime/blob/blobarray.h"
#include "gwnavgeneration/input/taggedtriangle3i.h"
#include "gwnavruntime/world/tagvolumeblob.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/base/kyguid.h"
#include "gwnavgeneration/generator/generatorblobtyperegistry.h"

namespace Kaim
{


class InputCellBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Generator, InputCellBlob, 0)
	KY_CLASS_WITHOUT_COPY(InputCellBlob)
public:
	InputCellBlob() {}

public:
	KyFloat32 m_rasterPrecision;
	KyGuid m_sectorGuid;
	CellPos m_cellPos;
	KyInt32 m_minAltitude;
	KyInt32 m_maxAltitude;
	BlobArray<TaggedTriangle3i> m_taggedTriangle3is;
	BlobArray<TagVolumeBlob> m_tagVolumeBlobs;
};

inline void SwapEndianness(Endianness::Target e, InputCellBlob& self)
{
	SwapEndianness(e, self.m_rasterPrecision);
	//SwapEndianness(e, self.m_sectorGuid); no swap on char[]
	SwapEndianness(e, self.m_cellPos);
	SwapEndianness(e, self.m_minAltitude);
	SwapEndianness(e, self.m_maxAltitude);
	SwapEndianness(e, self.m_taggedTriangle3is);
	SwapEndianness(e, self.m_tagVolumeBlobs);
}

} // namespace Kaim

#endif
