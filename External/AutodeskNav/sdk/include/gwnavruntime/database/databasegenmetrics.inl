/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// ---------- Primary contact: JUBA - secondary contact: NOBODY

namespace Kaim
{

KY_INLINE DatabaseGenMetrics::DatabaseGenMetrics() { SetDefaultValues(); }
KY_INLINE DatabaseGenMetrics::~DatabaseGenMetrics() {}

/// Clears all information maintained by this object. For internal use. 
KY_INLINE void DatabaseGenMetrics::Clear() { SetDefaultValues(); }
KY_INLINE bool DatabaseGenMetrics::IsClear() const { return m_cellSizeInPixel == KyInt32MAXVAL; }

/// Clears all information maintained by this object. For internal use. 
KY_INLINE void DatabaseGenMetrics::SetDefaultValues()
{
	m_generationRadius = KyFloat32MAXVAL;
	m_generationHeight = KyFloat32MAXVAL;
	m_altitudeTolerance = KyFloat32MAXVAL;
	m_integerPrecision = KyFloat32MAXVAL;
	m_integerPrecisionInv = 0.f;
	m_cellSizeInPixel = KyInt32MAXVAL;
	m_cellSizeInCoord = KyInt32MAXVAL;
	m_pixelSize = KyFloat32MAXVAL;
}

KY_INLINE KyFloat32 DatabaseGenMetrics::GetCellSize()                    const { return m_integerPrecision * (KyFloat32)m_cellSizeInCoord; }
KY_INLINE KyFloat32 DatabaseGenMetrics::GetGenerationRadius()            const { return m_generationRadius;                                }
KY_INLINE KyFloat32 DatabaseGenMetrics::GetGenerationHeight()            const { return m_generationHeight;                                }
KY_INLINE KyFloat32 DatabaseGenMetrics::GetGenerationAltitudeTolerance() const { return m_altitudeTolerance;                               }
KY_INLINE KyFloat32 DatabaseGenMetrics::GetPixelSize()                   const { return m_pixelSize;                                       }
KY_INLINE KyFloat32 DatabaseGenMetrics::SnapFloatOnIntegerGrid(KyFloat32 inputValue) const
{
	return (inputValue * m_integerPrecisionInv) + Kaim::Fsel(inputValue, 0.5f, -0.5f);
}

KY_INLINE KyInt32 DatabaseGenMetrics::SnapFloatOnPixelGrid(KyFloat32 inputValue) const
{
	return IntCoordSystem::NearestPixelCoord((KyInt64)SnapFloatOnIntegerGrid(inputValue));
}

KY_INLINE KyInt64 DatabaseGenMetrics::ComputeCellPosXFromInteger64(KyInt64 x) const
{
	return x > 0 ? (x - 1) / (KyInt64)m_cellSizeInCoord : (x / (KyInt64)m_cellSizeInCoord) - 1;
}

KY_INLINE KyInt64 DatabaseGenMetrics::ComputeCellPosYFromInteger64(KyInt64 y) const
{
	return y >= 0 ? y / (KyInt64)m_cellSizeInCoord : ((y + 1)/ (KyInt64)m_cellSizeInCoord) - 1;
}

KY_INLINE CellPos DatabaseGenMetrics::ComputeCellPos(const Vec2f& pos) const { return ComputeCellPosFromCoordPos64(GetCoordPos64FromVec2f(pos)); }
KY_INLINE CellPos DatabaseGenMetrics::ComputeCellPos(const Vec3f& pos) const { return ComputeCellPosFromCoordPos64(GetCoordPos64FromVec3f(pos)); }

KY_INLINE void DatabaseGenMetrics::ComputeCellPos(const Vec2f& pos, CellPos& cellPos) const { ComputeCellPosFromCoordPos64(GetCoordPos64FromVec2f(pos), cellPos); }
KY_INLINE void DatabaseGenMetrics::ComputeCellPos(const Vec3f& pos, CellPos& cellPos) const { ComputeCellPosFromCoordPos64(GetCoordPos64FromVec3f(pos), cellPos); }

KY_INLINE KyInt64 DatabaseGenMetrics::GetNearestInteger64FromFloatValue(KyFloat32 inputValue) const
{
	KY_LOG_ERROR_IF(IsClear(), ("You should insert at least a valid NavMesh first"));
	return (KyInt64)SnapFloatOnIntegerGrid(inputValue);
}

KY_INLINE KyFloat32 DatabaseGenMetrics::GetFloatValueFromInteger64(KyInt64 inputValue) const
{
	KY_LOG_ERROR_IF(IsClear(), ("You should insert at least a valid NavMesh first"));
	return (KyFloat32)inputValue * m_integerPrecision;
}

KY_INLINE KyFloat32 DatabaseGenMetrics::GetFloatValueFromInteger32(KyInt32 inputValue) const
{
	KY_LOG_ERROR_IF(IsClear(), ("You should insert at least a valid NavMesh first"));
	return (KyFloat32)inputValue * m_integerPrecision;
}

KY_INLINE void DatabaseGenMetrics::GetCoordPos64FromVec3f(const Vec3f& inputPosition, CoordPos64& result) const
{
	volatile KyFloat32 resultX = SnapFloatOnIntegerGrid(inputPosition.x);
	volatile KyFloat32 resultY = SnapFloatOnIntegerGrid(inputPosition.y);
	result.Set((KyInt64)resultX, (KyInt64)resultY);
}


KY_INLINE void DatabaseGenMetrics::GetCoordPos64FromVec2f(const Vec2f& inputPosition, CoordPos64& result) const
{
	volatile KyFloat32 resultX = SnapFloatOnIntegerGrid(inputPosition.x);
	volatile KyFloat32 resultY = SnapFloatOnIntegerGrid(inputPosition.y);
	result.Set((KyInt64)resultX, (KyInt64)resultY);
}


KY_INLINE CellPos DatabaseGenMetrics::ComputeCellPosFromCoordPos64(const CoordPos64& coordPos64) const
{
	KY_LOG_ERROR_IF(IsClear(), ("You should insert at least a valid NavMesh first"));
	const KyInt32 x = (KyInt32)ComputeCellPosXFromInteger64(coordPos64.x);
	const KyInt32 y = (KyInt32)ComputeCellPosYFromInteger64(coordPos64.y);
	return CellPos(x, y);
}

KY_INLINE void DatabaseGenMetrics::ComputeCellPosFromCoordPos64(const CoordPos64& coordPos64, CellPos& cellPos) const
{
	KY_LOG_ERROR_IF(IsClear(), ("You should insert at least a valid NavMesh first"));
	const KyInt32 x = (KyInt32)ComputeCellPosXFromInteger64(coordPos64.x);
	const KyInt32 y = (KyInt32)ComputeCellPosYFromInteger64(coordPos64.y);
	cellPos.Set(x, y);
}

KY_INLINE CoordPos64 DatabaseGenMetrics::ComputeCellOrigin(const CellPos& cellPos) const
{
	KY_LOG_ERROR_IF(m_cellSizeInCoord == KyInt32MAXVAL, ("You should insert at least a valid NavMesh first"));
	return CoordPos64((KyInt64)cellPos.x * (KyInt64)m_cellSizeInCoord, (KyInt64)cellPos.y * (KyInt64)m_cellSizeInCoord);
}

KY_INLINE void DatabaseGenMetrics::GetCoordPos64FromWorldIntegerPos(const WorldIntegerPos& inputPosition, CoordPos64& result) const
{
	result.Set( ((KyInt64)inputPosition.m_cellPos.x * (KyInt64)m_cellSizeInCoord) + (KyInt64)inputPosition.m_coordPosInCell.x, 
				((KyInt64)inputPosition.m_cellPos.y * (KyInt64)m_cellSizeInCoord )+ (KyInt64)inputPosition.m_coordPosInCell.y);
}


KY_INLINE CoordPos64 DatabaseGenMetrics::GetCoordPos64FromVec3f(const Vec3f& inputPosition) const
{
	CoordPos64 result;
	GetCoordPos64FromVec3f(inputPosition, result);
	return result;
}
KY_INLINE CoordPos64 DatabaseGenMetrics::GetCoordPos64FromVec2f(const Vec2f& inputPosition) const
{
	CoordPos64 result;
	GetCoordPos64FromVec2f(inputPosition, result);
	return result;
}

KY_INLINE WorldIntegerPos DatabaseGenMetrics::GetWorldIntegerPosFromCoordPos64(const CoordPos64& inputPosition) const
{
	WorldIntegerPos result;
	GetWorldIntegerPosFromCoordPos64(inputPosition, result);
	return result;
}

KY_INLINE void DatabaseGenMetrics::GetWorldIntegerPosFromVec3f(const Vec3f& inputPosition, WorldIntegerPos& result) const
{
	GetWorldIntegerPosFromVec2f(inputPosition.Get2d(), result);
}

KY_INLINE WorldIntegerPos DatabaseGenMetrics::GetWorldIntegerPosFromVec2f(const Vec2f& inputPosition) const
{
	WorldIntegerPos result;
	GetWorldIntegerPosFromVec2f(inputPosition, result);
	return result;
}

KY_INLINE WorldIntegerPos DatabaseGenMetrics::GetWorldIntegerPosFromVec3f(const Vec3f& inputPosition) const
{
	return GetWorldIntegerPosFromVec2f(inputPosition.Get2d());
}

KY_INLINE CoordPos64 DatabaseGenMetrics::GetCoordPos64FromWorldIntegerPos(const WorldIntegerPos& inputPosition) const
{
	CoordPos64 result;
	GetCoordPos64FromWorldIntegerPos(inputPosition, result);
	return result;
}

KY_INLINE void DatabaseGenMetrics::GetCellBoxOfAnAABBox(const Box3f& aABB, CellBox& cellBox) const
{
	cellBox.Set(ComputeCellPos(aABB.m_min), ComputeCellPos(aABB.m_max));
}

KY_INLINE void DatabaseGenMetrics::GetCellBoxOfAnAABBox(const Box2f& aABB, CellBox& cellBox) const
{
	cellBox.Set(ComputeCellPos(aABB.m_min), ComputeCellPos(aABB.m_max));
}

KY_INLINE void DatabaseGenMetrics::GetCellBoxOfAnAABBox(const CoordBox64& aABB, CellBox& cellBox) const
{
	cellBox.Set(ComputeCellPosFromCoordPos64(aABB.Min()), ComputeCellPosFromCoordPos64(aABB.Max()));
}

}

