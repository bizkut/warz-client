/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_GeneratorInputOutputBlob_H
#define GwNavGen_GeneratorInputOutputBlob_H

#include "gwnavgeneration/generator/generatorsectorconfigblob.h"
#include "gwnavgeneration/generator/generatorparameters.h"
#include "gwnavruntime/base/guidcompound.h"
#include "gwnavgeneration/generator/generatorguidcompound.h"

namespace Kaim
{

class GeneratorParameters;
class GeneratorSectorList;

/// used to serialize GeneratorInputOutput
class GeneratorSectorListBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Generator, GeneratorSectorListBlob, 0)

public:
	void GetSectorList(GeneratorSectorList& sectorList) const;
	void GetExclusiveGuids(KyArray<GeneratorGuidCompound>& guidCompounds) const;

public:
	BlobArray<GeneratorSectorBlob> m_sectors;
	BlobArray<GuidCompound> m_exclusiveGuids;
};

inline void SwapEndianness(Endianness::Target e, GeneratorSectorListBlob& self)
{
	SwapEndianness(e, self.m_sectors);
	SwapEndianness(e, self.m_exclusiveGuids);
}

}


#endif

