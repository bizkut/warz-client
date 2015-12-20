/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_DynamicRasterCell_H
#define GwNavGen_DynamicRasterCell_H


#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavgeneration/common/boxofarrays.h"
#include "gwnavgeneration/raster/dynamicrastercolumn.h"
#include "gwnavruntime/navmesh/celldesc.h"
#include "gwnavgeneration/generator/generatorparameters.h"
#include "gwnavruntime/basesystem/intcoordsystem.h"
#include "gwnavgeneration/raster/rastercelltagvolume.h"
#include "gwnavgeneration/containers/tlsarray.h"

namespace Kaim
{

class GeneratorSystem;
class SingleStagePixel;
class InputCellBlob;
class NavTag;

enum DynamicRasterCellMergeStatus
{
	DynamicRasterCellMergeStatus_MergeEnabled,
	DynamicRasterCellMergeStatus_MergeDisabled
};

class DynamicRasterCell
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_CLASS_WITHOUT_COPY(DynamicRasterCell)

public:
	DynamicRasterCell(
		const GeneratorParameters& config, const CellDesc& cellDesc, const KyArrayPOD<const InputCellBlob*>* inputCells,
		const KyArrayTLS_POD<const NavTag*>* navTagArray,
		KyFloat32 modifiedRasterPrecision = 0.0f,
		const PixelBox* modifiedPixelBox = KY_NULL
		)
		: m_navTagArray(navTagArray)
		, m_dynamicColumns(GeneratorMemory::TlsHeap())
		, m_finalColumns(GeneratorMemory::TlsHeap())
		, m_finalNavTagIdxColumns(GeneratorMemory::TlsHeap())
		, m_areAllInputCellsOverlapping(true)
	{
		KyFloat32 rasterPrecision = config.m_normalizedParameters.m_normalizedRasterPrecision;
		if (modifiedRasterPrecision != 0.0f)
			rasterPrecision = modifiedRasterPrecision;

		const PixelBox* pixelBox = &cellDesc.m_enlargedPixelBox;
		if (modifiedPixelBox != KY_NULL)
			pixelBox = modifiedPixelBox;

		Setup(config, cellDesc, inputCells, rasterPrecision, *pixelBox);
	}

	void Setup(const GeneratorParameters& config, const CellDesc& cellDesc,
			   const KyArrayPOD<const InputCellBlob*>* inputCells, KyFloat32 rasterPrecision, const PixelBox& pixelBox);

	/* To be called before the fill of dynamicColumns: GetDynamicColumn().PushPixel(). */
	void BeginColumns();

	/* To be called between BeginColumns() and EndColumns(). */
	void PushPixel(const PixelPos& pixelPos, const SingleStagePixel& pixel, KyUInt32 navTagIdx, KyUInt32 ownerIdx = KyUInt32MAXVAL);

	/* To be called between BeginColumns() and EndColumns(). */
	void PushPixelToLocal(const PixelPos& localPixelPos, const SingleStagePixel& pixel, KyUInt32 navTagIdx);

	/* To be called between BeginColumns() and EndColumns(). */
	void PushPixel(KyInt32 rowMajorIdx, const SingleStagePixel& pixel, KyUInt32 navTagIdx, KyUInt32 ownerIdx = KyUInt32MAXVAL);

	/*
	To be called after the fill of dynamicColumns: GetDynamicColumn().PushPixel().
	Copy dynamicColumns to finalColumns and release dynamicColumns.
	If mergeStatus is DynamicRasterCellMergeStatus_MergeEnabled, merge vertically the obstacles which are
	less than entity height from one another.
	*/
	void EndColumns(DynamicRasterCellMergeStatus mergeStatus);

	/* Once EndColumns() has been called, read FinalColumns. */
	const BoxOfArrays<KyFloat32>::Column& GetFinalColumnFromLocal(const PixelPos& localPixelPos) const;

	/* Once EndColumns() has been called, read FinalColumns. */
	const BoxOfArrays<KyFloat32>::Column& GetFinalColumn(const PixelPos& pixelPos) const;

	/* Once EndColumns() has been called, read FinalColumns. */
	const BoxOfArrays<KyUInt32>::Column& GetFinalNavTagColumnFromLocal(const PixelPos& localPixelPos) const;

	/* Once EndColumns() has been called, read FinalColumns. */
	const BoxOfArrays<KyUInt32>::Column& GetFinalNavTagIndicesColumn(const PixelPos& pixelPos) const;

	/* Write Raster file and Raster 3d obj file if specified in the generatorConfig, for debug purpose. */
	KyResult WriteIntermediateRasterFile(GeneratorSystem* system) const;

	bool IsEmpty()             const { return m_isEmpty;             }
	bool AreAllInputCellsOverlapping() const {return m_inputCells->GetCount() == 1 || m_areAllInputCellsOverlapping; }
	bool HasMultipleNavTags() const { return m_hasMultipleNavTags; }

	void SetHasMultipleNavTags(bool hasMultipleNavTags) { m_hasMultipleNavTags = hasMultipleNavTags; }
	
	const PixelBox& GetPixelBox()                   const { return m_pixelBox;     }
	const CellDesc& GetCellDesc()                   const { return m_cellDesc;     }

	const BoxOfArrays<KyFloat32>& GetFinalColumns() const { return m_finalColumns; }
	      BoxOfArrays<KyFloat32>& GetFinalColumns()       { return m_finalColumns; }

	const BoxOfArrays<KyUInt32>& GetFinalColorColumns() const { return m_finalNavTagIdxColumns; }
	      BoxOfArrays<KyUInt32>& GetFinalColorColumns()       { return m_finalNavTagIdxColumns; }

	/* Copies the final columns from  another raster. Fails if celldesc is not the same. */
	KyResult Copy(const DynamicRasterCell& from);

	KyInt32 GetPixelSize()         const { return IntCoordSystem::IntPixelSize(); }
	KyFloat32 GetRasterPrecision() const { return m_rasterPrecision;              }

	const KyArrayTLS_POD<const NavTag*>& GetNavTagArray() const { return *m_navTagArray; }
private:
	void ComputeUniqueTagVolumes(KyArray<RasterCellTagVolume>& rasterVolumes);
	void MakeFinalColumns(DynamicRasterCellMergeStatus mergeStatus);
	void ReleaseDynamicColumns();

private:
	KyFloat32 m_rasterPrecision;
	GeneratorParameters m_config;
	CellDesc m_cellDesc;
	PixelBox m_pixelBox;
	const KyArrayPOD<const InputCellBlob*>* m_inputCells;
	const KyArrayTLS_POD<const NavTag*>* m_navTagArray;

	// dynamic columns
	DynamicRasterColumnPool m_dynamicColumns;

	// final columns
	BoxOfArrays<KyFloat32> m_finalColumns;

	// final navTag
	BoxOfArrays<KyUInt32> m_finalNavTagIdxColumns;
	
	bool m_isEmpty;
	bool m_hasMultipleNavTags; //extracted by the CellRasterizerFromPdgInput
	bool m_areAllInputCellsOverlapping; //this flag is set and computed during MakeFinalColumns()

	friend class DynamicRasterCellBlobBuilder;
};



// --------------------------------- inline implementation ---------------------------------

inline void DynamicRasterCell::PushPixelToLocal(const PixelPos& localPixelPos, const SingleStagePixel& pixel, KyUInt32 navTagIdx)
{
	KyInt32 idx = m_pixelBox.GetRowMajorIndexFromLocalPos(localPixelPos);
	PushPixel(idx, pixel, navTagIdx);
}

inline void DynamicRasterCell::PushPixel(const PixelPos& pixelPos, const SingleStagePixel& pixel, KyUInt32 navTagIdx, KyUInt32 ownerIdx)
{
	KyInt32 idx = m_pixelBox.GetRowMajorIndex(pixelPos);
	PushPixel(idx, pixel, navTagIdx, ownerIdx);
}

inline const BoxOfArrays<KyFloat32>::Column& DynamicRasterCell::GetFinalColumnFromLocal(const PixelPos& localPixelPos) const
{
	return m_finalColumns.GetColumn(localPixelPos.x, localPixelPos.y);
}

inline const BoxOfArrays<KyFloat32>::Column& DynamicRasterCell::GetFinalColumn(const PixelPos& pixelPos) const
{
	KyInt32 idx = m_pixelBox.GetRowMajorIndex(pixelPos);
	return m_finalColumns.GetColumn(idx);
}

inline const BoxOfArrays<KyUInt32>::Column& DynamicRasterCell::GetFinalNavTagColumnFromLocal(const PixelPos& localPixelPos) const
{
	return m_finalNavTagIdxColumns.GetColumn(localPixelPos.x, localPixelPos.y);
}

inline const BoxOfArrays<KyUInt32>::Column& DynamicRasterCell::GetFinalNavTagIndicesColumn(const PixelPos& pixelPos) const
{
	KyInt32 idx = m_pixelBox.GetRowMajorIndex(pixelPos);
	return m_finalNavTagIdxColumns.GetColumn(idx);
}


}


#endif
