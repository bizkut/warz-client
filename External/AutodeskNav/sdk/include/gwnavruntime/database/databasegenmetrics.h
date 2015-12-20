/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_DatabaseGenMetrics_H
#define Navigation_DatabaseGenMetrics_H

#include "gwnavruntime/navmesh/worldintegerpos.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/math/box2f.h"
#include "gwnavruntime/basesystem/intcoordsystem.h"
#include "gwnavruntime/basesystem/logger.h"

namespace Kaim
{

/// This class gathers a set of generation parameters of one Database and manages the conversion
///  between the floating-point and integer coordinate systems used internally by NavData.
/// Each Database object maintains and provides access to an instance of this class
class DatabaseGenMetrics
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_CLASS_WITHOUT_COPY(DatabaseGenMetrics)
public:
	DatabaseGenMetrics();
	~DatabaseGenMetrics();

	// Clears all information maintained by this object. For internal use. 
	void Clear();
	bool IsClear() const;

	// ---------------------------------- Public Member Functions ----------------------------------

	KyFloat32 GetCellSize()                    const; ///< Retrieves the size (length and width) of theNavCells in this Database. 
	KyFloat32 GetGenerationRadius()            const; ///< Retrieves the radius of the character that NavMeshes in this Database were generated for. 
	KyFloat32 GetGenerationHeight()            const; ///< Retrieves the height of the character that the NavMeshes in this Database were generated for. 
	KyFloat32 GetGenerationAltitudeTolerance() const; ///< Retrieves the altitude tolerance parameters that the NavMeshes in this Database were generated with. 
	KyFloat32 GetPixelSize()                   const; ///< Retrieves the size of pixel (raster precision) that the NavMeshes in this Database were generated with. 

	KyInt64   GetNearestInteger64FromFloatValue(KyFloat32 inputValue) const; ///< Retrieves the 64-bit integer nearest to the specified float.
	KyFloat32 GetFloatValueFromInteger64(KyInt64 inputValue)          const; ///< Retrieves the float nearest to the specified 64-bit integer.
	KyFloat32 GetFloatValueFromInteger32(KyInt32 inputValue)          const; ///< Retrieves the float nearest to the specified 32-bit integer.

	CoordPos64 ComputeCellOrigin(const CellPos& cellPos) const; ///< Retrieves the 64-bit integer coordinates that mark the origin point of theNavCell at the specified position

	// ---------------------------------- Functions to compute CellPos ----------------------------------

	/// Retrieves the position of theNavCell in the grid that is responsible for the specified (X,Y) coordinates. 
	CellPos ComputeCellPos(const Vec2f& pos) const;

	/// Retrieves the position of theNavCell in the grid that is responsible for the specified (X,Y) coordinates.
	/// \param pos             The coordinates you want to find the whichNavCell is responsible for.
	/// \param[out] cellPos    The position of the NavCell responsible for pos. 
	void ComputeCellPos(const Vec2f& pos, CellPos& cellPos) const;

	/// Retrieves the position within the grid of theNavCell that is responsible for the specified coordinates. 
	CellPos ComputeCellPos(const Vec3f& pos) const;

	/// Retrieves the position of theNavCell in the grid that is responsible for the specified coordinates.
	/// \param pos             The coordinates you want to find the whichNavCell is responsible for.
	/// \param[out] cellPos    The position of the NavCell responsible for pos. 
	void ComputeCellPos(const Vec3f& pos, CellPos& cellPos) const;

	/// Retrieves the position of theNavCell in the grid that is responsible for the specified (X,Y) coordinates. 
	CellPos ComputeCellPosFromCoordPos64(const CoordPos64& coordPos64) const;

	/// Retrieves the position of theNavCell in the grid that is responsible for the specified (X,Y) coordinates.
	/// \param coordPos64      The coordinates you want to find the whichNavCell is responsible for.
	/// \param[out] cellPos    The position of the NavCell responsible for pos. 
	void ComputeCellPosFromCoordPos64(const CoordPos64& coordPos64, CellPos& cellPos) const;

	// ---------------------------------- Conversion from floating-point coordinates to 64 bits integer coordinates ----------------------------------

	CoordPos64 GetCoordPos64FromVec3f(const Vec3f& inputPosition)               const; ///< Retrieves the 64-bit integer coordinates that correspond to the specified floating-point coordinates.
	void GetCoordPos64FromVec3f(const Vec3f& inputPosition, CoordPos64& result) const; ///< Retrieves the 64-bit integer coordinates that correspond to the specified floating-point coordinates.

	CoordPos64 GetCoordPos64FromVec2f(const Vec2f& inputPosition)               const; ///< Retrieves the 64-bit integer coordinates that correspond to the specified floating-point coordinates.
	void GetCoordPos64FromVec2f(const Vec2f& inputPosition, CoordPos64& result) const; ///< Retrieves the 64-bit integer coordinates that correspond to the specified floating-point coordinates.

	// ---------------------------------- Conversion from floating-point coordinates to WorldIntegerPos coordinates ----------------------------------

	
	WorldIntegerPos GetWorldIntegerPosFromVec2f(const Vec2f& inputPosition)               const; ///< Retrieves the WorldIntegerPos coordinates that correspond to the specified floating-point coordinates.
	void GetWorldIntegerPosFromVec2f(const Vec2f& inputPosition, WorldIntegerPos& result) const; ///< Retrieves the WorldIntegerPos coordinates that correspond to the specified floating-point coordinates.

	WorldIntegerPos GetWorldIntegerPosFromVec3f(const Vec3f& inputPosition)               const; ///< Retrieves the WorldIntegerPos coordinates that correspond to the specified floating-point coordinates.
	void GetWorldIntegerPosFromVec3f(const Vec3f& inputPosition, WorldIntegerPos& result) const; ///< Retrieves the WorldIntegerPos coordinates that correspond to the specified floating-point coordinates.

	// ---------------------------------- CoordPos64 <-> WorldIntegerPos Conversion ----------------------------------

	WorldIntegerPos GetWorldIntegerPosFromCoordPos64(const CoordPos64& inputPosition)               const; ///< Retrieves the integer coordinates that correspond to the specified 64-bit integer coordinates.
	void GetWorldIntegerPosFromCoordPos64(const CoordPos64& inputPosition, WorldIntegerPos& result) const; ///< Retrieves the integer coordinates that correspond to the specified 64-bit integer coordinates.

	CoordPos64 GetCoordPos64FromWorldIntegerPos(const WorldIntegerPos& inputPosition)               const; ///< Retrieves the 64-bit integer coordinates that correspond to the specified WorldIntegerPos coordinates. 
	void GetCoordPos64FromWorldIntegerPos(const WorldIntegerPos& inputPosition, CoordPos64& result) const; ///< Retrieves the 64-bit integer coordinates that correspond to the specified WorldIntegerPos coordinates. 

	// ---------------------------------- CellBox computation ----------------------------------

	void GetCellBoxOfAnAABBox(const Box2f& aABB     , CellBox& cellBox) const; ///< Computes aCellBox from aBox2f.
	void GetCellBoxOfAnAABBox(const Box3f& aABB     , CellBox& cellBox) const; ///< Computes aCellBox from aBox3f.
	void GetCellBoxOfAnAABBox(const CoordBox64& aABB, CellBox& cellBox) const; ///< Computes aCellBox from aCoordBox64.

public: //internal
	void SetDefaultValues();
	KyFloat32 SnapFloatOnIntegerGrid(KyFloat32 inputValue) const;
	KyInt32 SnapFloatOnPixelGrid(KyFloat32 inputValue) const;
	KyInt64 ComputeCellPosXFromInteger64(KyInt64 x) const;
	KyInt64 ComputeCellPosYFromInteger64(KyInt64 y) const;

public: // internal
	
	/// Initialize m_integerPrecisionInv, m_pixelSize and m_cellSizeInCoord,
	/// it has to be called after other members were properly initialized
	void Setup(); 

public: // internal
	
	// ---------------------------------- Members to be initialized before calling Setup() ----------------------------------

	KyFloat32 m_generationRadius;    ///< The radius of the character the NavMeshes in this DatabaseGenMetrics were generated for. Do not modify. 
	KyFloat32 m_generationHeight;    ///< The height of the character the NavMeshes in this DatabaseGenMetrics were generated for. Do not modify. 
	KyFloat32 m_altitudeTolerance;   ///< The maximum difference in altitude that may exist between the NavMesh and the original terrain mesh. Do not modify. 
	KyFloat32 m_integerPrecision;    ///< For internal use. Do not modify. 
	KyInt32 m_cellSizeInPixel;       ///< The number of raster pixels in the length and width of each cell. Do not modify. 
	
	// ---------------------------------- Initialized from other members in Setup() ----------------------------------

	KyFloat32 m_integerPrecisionInv; ///< For internal use. Do not modify. Value set in Setup() based on m_integerPrecision.
	KyFloat32 m_pixelSize;           ///< For internal use. Do not modify. Value set in Setup() based on m_integerPrecision.
	KyInt32 m_cellSizeInCoord;       ///< The number of integer steps in the length and width of each cell. Do not modify. Value set in Setup() based on m_cellSizeInPixel.
};

}

#include "gwnavruntime/database/databasegenmetrics.inl"

#endif //Navigation_DatabaseGenMetrics_H

