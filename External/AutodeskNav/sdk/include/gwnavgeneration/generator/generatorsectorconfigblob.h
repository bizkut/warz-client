/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_GeneratorSectorBlob_H
#define GwNavGen_GeneratorSectorBlob_H


#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/blob/blobfieldarray.h"
#include "gwnavruntime/base/kyguid.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavgeneration/generator/generatorblobtyperegistry.h"
#include "gwnavgeneration/generator/generatorsector.h"

namespace Kaim
{

class BlobAggregate;
class GeneratorSectorList;
class GeneratorSector;

/// Used to serialize GeneratorInputOutput. Corresponds to GeneratorSector.
class GeneratorSectorBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Generator, GeneratorSectorBlob, 0)
	KY_CLASS_WITHOUT_COPY(GeneratorSectorBlob)
public:
	GeneratorSectorBlob() {}

	BlobFieldArray                      m_fields;
	CellBox                             m_navDataCellBox;
	BlobArray<BlobArray<char> >         m_inputFilenames;
	BlobArray<Vec3f>                    m_inputSeedPoints;
	BlobArray<ClientInputTagVolumeBlob> m_inputTagVolumes;
	BlobArray<CellPos>                  m_previousPatchCellPosList;
	CellBox                             m_previousInputCellBox;
	BlobArray<Vec3f>                    m_previousSeedPoints;
};

inline void SwapEndianness(Endianness::Target e, GeneratorSectorBlob& self)
{
	SwapEndianness(e, self.m_fields);
	SwapEndianness(e, self.m_navDataCellBox);
	SwapEndianness(e, self.m_inputFilenames);
	SwapEndianness(e, self.m_inputSeedPoints);
	SwapEndianness(e, self.m_inputTagVolumes);
	SwapEndianness(e, self.m_previousPatchCellPosList);
	SwapEndianness(e, self.m_previousInputCellBox);
	SwapEndianness(e, self.m_previousSeedPoints);
}

/// Used to serialize GeneratorInputOutput
class GeneratorSectorBlobBuilder : public BaseBlobBuilder<GeneratorSectorBlob>
{
public:
	GeneratorSectorBlobBuilder(Ptr<GeneratorSector> sector) : m_sector(sector) {}
private:
	virtual void DoBuild();
private:
	Ptr<GeneratorSector> m_sector;
};

}


#endif

