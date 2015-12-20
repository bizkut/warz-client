/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_InputCellInfo_H
#define GwNavGen_InputCellInfo_H


#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/blob/blobhandler.h"


namespace Kaim
{

class InputCellBlob;
class GeneratorSectorBuilder;


// Small wrapper around m_inputCellBlobHandler that can be NULL.
// Allows to retrieve sectorBuilder from which the m_inputCellBlobHandler comes from.
// Allows to retrieve cellPos even when m_inputCellBlobHandler is KY_NULL (when IsInputTilingEnabled() is true).
class InputCellInfo
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavDataGen)

public:
	InputCellInfo() // default Ctor for KyArray<InputCellInfo>
		: m_sectorBuilder(KY_NULL)
	{}

	InputCellInfo(GeneratorSectorBuilder* sectorBuilder, const CellPos& cellpos, Ptr<BlobHandler<InputCellBlob> > inputCellBlobHandler)
		: m_sectorBuilder(sectorBuilder), m_cellPos(cellpos), m_inputCellBlobHandler(inputCellBlobHandler)
	{}

	bool operator==(const InputCellInfo& other) const
	{
		return m_sectorBuilder == other.m_sectorBuilder
			&& m_cellPos       == other.m_cellPos;
			// we do not compare m_inputCellBlob on purpose because m_inputCellBlobHandler may be in memory (!= KY_NULL) or not (== KY_NULL)
	}

	bool operator!=(const InputCellInfo& other) const { return !operator==(other); }

public:
	GeneratorSectorBuilder* m_sectorBuilder; // the sector(Builder) from which the InputCell comes from
	CellPos m_cellPos;
	Ptr<BlobHandler<InputCellBlob> > m_inputCellBlobHandler; // NULL is sys->InputTilingEnabled()
};


class InputCellInfoArrayAtPos : public RefCountBaseNTS<InputCellInfoArrayAtPos, MemStat_NavDataGen>
{
public:
	InputCellInfoArrayAtPos(KyInt32 x, KyInt32 y) : m_cellPos(x, y) {}

	void PushBack(const InputCellInfo& inputCellInfo) { m_inputCellInfos.PushBack(inputCellInfo); }

	KyUInt32 GetCount() const { return m_inputCellInfos.GetCount(); }

	const InputCellInfo& Get(KyUInt32 index) const { return m_inputCellInfos[index]; }
	      InputCellInfo& Get(KyUInt32 index)       { return m_inputCellInfos[index]; }

	bool DoesContain(const InputCellInfo& inputCellInfo) { return m_inputCellInfos.DoesContain(inputCellInfo); }

	// m_inputCells from sectors with m_doGenerateNavData==true are at the beginning of the m_inputCellInfos
	void Sort();

public:
	CellPos m_cellPos;
	KyArray<InputCellInfo> m_inputCellInfos;
};


} // namespace Kaim

#endif
