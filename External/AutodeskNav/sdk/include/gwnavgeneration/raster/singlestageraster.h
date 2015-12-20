/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_SingleStageRaster_H
#define GwNavGen_SingleStageRaster_H


#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavgeneration/common/bitarray2d_1024.h"
#include "gwnavgeneration/raster/singlestagepixel.h"
#include "gwnavgeneration/containers/tlsarray.h"

namespace Kaim
{

/*
Contains the memory that receives the rasterization of one triangle.
Given a clippingPixelBox, from a trianglePixelBox, make {rasterPixelBox, rasterPixelsMemory} available.
Has the ability to restore its initial state after a given geometry rasterization.

+-----------------------+
|clippingPixelBox       |
|                       |
|                       |
|                       |
|                       |
|                       |
|     +-----------------+---------+
|     |                 |         |
|     |rasterPixelBox   |         |
|     |                 |         |
+-----+-----------------+         |
      |                           |
      |           trianglePixelBox|
      +---------------------------+
*/
class SingleStageRaster
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	SingleStageRaster(MemoryHeap* heap);

	KyResult Init(const PixelBox& clippingPixelBox);

	bool SetupRasterPixelBox(const PixelBox& geometryPixelBox);

	void ClearPixels();

	void PushPixelValue(const PixelPos& pixelPos, KyFloat32 value);
	void FlagPixel(const PixelPos& pixelPos);

	const SingleStagePixel& GetPixel(const PixelPos& pixelPos) const;
	const SingleStagePixel& GetPixelFromLocalPos(const PixelPos& localPixelPos) const;

	const PixelBox& GetRasterPixelBox() const;

	PixelCoord GetFirstLine() const;
	PixelCoord GetLastLine() const;
	PixelCoord GetNoLineIdx() const;

	PixelCoord GetFirstPixelOnLine(PixelCoord py) const;
	PixelCoord GetLastPixelOnLine(PixelCoord py) const;
	PixelCoord GetNoPixelIdx() const;

	// unit test usage
	bool IsPixelFlaggedFromLocalPos(const PixelPos& localPixelPos) const;

	void SetNavTagIdx(KyUInt32 navTagIdx);
	KyUInt32 GetNavTagIdx() const;

	void SetOwnerIdx(KyUInt32 ownerIdx);
	KyUInt32 GetOwnerIdx() const;

public:
	PixelBox m_clippingPixelBox;
	PixelBox m_rasterPixelBox;
	KyArrayTLS<SingleStagePixel> m_pixels; // row major
	BitArray2d_1024 m_bitArray2d;
	KyUInt32 m_navTagIdx;
	KyUInt32 m_ownerIndex;
	MemoryHeap* m_heap;
};


KY_INLINE SingleStageRaster::SingleStageRaster(MemoryHeap* heap) : m_bitArray2d(heap), m_navTagIdx(KyUInt32MAXVAL), m_heap(heap) {}

KY_INLINE void SingleStageRaster::PushPixelValue(const PixelPos& pixelPos, KyFloat32 value)
{
	KY_DEBUG_ASSERTN(m_rasterPixelBox.IsInside(pixelPos),("Pixel is outside rasterPixelBox"));

	PixelPos localPixelPos = pixelPos - m_rasterPixelBox.Min();
	KyInt32 pixelIdx = m_rasterPixelBox.GetRowMajorIndexFromLocalPos(localPixelPos);
	m_pixels[pixelIdx].AddValue(value);
	m_bitArray2d.SetPixel(localPixelPos.x, localPixelPos.y);
}

KY_INLINE void SingleStageRaster::FlagPixel(const PixelPos& pixelPos)
{
	KY_DEBUG_ASSERTN(m_rasterPixelBox.IsInside(pixelPos),("Pixel is outside rasterPixelBox"));

	PixelPos localPixelPos = pixelPos - m_rasterPixelBox.Min();
	m_bitArray2d.SetPixel(localPixelPos.x, localPixelPos.y);
}

KY_INLINE const SingleStagePixel& SingleStageRaster::GetPixel(const PixelPos& pixelPos) const
{
	return m_pixels[m_rasterPixelBox.GetRowMajorIndex(pixelPos)];
}

KY_INLINE const SingleStagePixel& SingleStageRaster::GetPixelFromLocalPos(const PixelPos& localPixelPos) const
{
	return m_pixels[m_rasterPixelBox.GetRowMajorIndexFromLocalPos(localPixelPos)];
}

KY_INLINE const PixelBox& SingleStageRaster::GetRasterPixelBox()                const { return m_rasterPixelBox; }
KY_INLINE PixelCoord      SingleStageRaster::GetFirstLine()                     const { return m_bitArray2d.GetFirstY() + m_rasterPixelBox.Min().y; }
KY_INLINE PixelCoord      SingleStageRaster::GetLastLine()                      const { return m_bitArray2d.GetLastY()  + m_rasterPixelBox.Min().y; }
KY_INLINE PixelCoord      SingleStageRaster::GetNoLineIdx()                     const { return m_rasterPixelBox.Min().y - 1; }
KY_INLINE PixelCoord      SingleStageRaster::GetNoPixelIdx()                    const { return m_rasterPixelBox.Min().x - 1; }
KY_INLINE PixelCoord      SingleStageRaster::GetFirstPixelOnLine(PixelCoord py) const { return m_bitArray2d.GetFirstX(py - m_rasterPixelBox.Min().y) + m_rasterPixelBox.Min().x; }
KY_INLINE PixelCoord      SingleStageRaster::GetLastPixelOnLine(PixelCoord py)  const { return m_bitArray2d.GetLastX( py - m_rasterPixelBox.Min().y) + m_rasterPixelBox.Min().x; }

// unit test usage
KY_INLINE bool SingleStageRaster::IsPixelFlaggedFromLocalPos(const PixelPos& localPixelPos) const
{
	return m_bitArray2d.GetPixel(localPixelPos.x, localPixelPos.y);
}

KY_INLINE void SingleStageRaster::SetNavTagIdx(KyUInt32 navTagIdx) { m_navTagIdx = navTagIdx; }
KY_INLINE KyUInt32 SingleStageRaster::GetNavTagIdx() const { return m_navTagIdx; }

KY_INLINE void SingleStageRaster::SetOwnerIdx(KyUInt32 ownerIdx) { m_ownerIndex = ownerIdx; }
KY_INLINE KyUInt32 SingleStageRaster::GetOwnerIdx() const { return m_ownerIndex; }

}


#endif
