/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavMeshTypes_H
#define Navigation_NavMeshTypes_H

#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/math/box2i.h"
#include "gwnavruntime/math/box2ll.h"
#include "gwnavruntime/kernel/SF_Debug.h"

namespace Kaim
{
typedef Vec2LL CoordPos64; ///< A type that represents the position of a point within the 2D integer grid.  
typedef Box2LL CoordBox64; ///< A type that represents a bounding box in the integer 2D grid.  

typedef KyInt32 Coord;
typedef Vec2i CoordPos;                            ///< A type that represents the position of a point within the 2D integer grid.  
typedef Box2i CoordBox;                            ///< A type that represents a bounding box in the integer 2D grid.  
static const KyInt32 InvalidCoord = KyInt32MAXVAL; ///< Represents an invalidCoord object.

typedef KyInt32 PixelCoord;
typedef Vec2i PixelPos;
typedef Box2i PixelBox;
static const KyInt32 InvalidPixelCoord = KyInt32MAXVAL;///< Represents an invalidPixelCoord object.

typedef KyInt32 CellCoord;                             ///< A type that represents the placement of a cell on one axis of a 2D grid.
typedef Vec2i CellPos;                                 ///< A type that represents the position of a cell within a 2D grid.
typedef Box2i CellBox;                                 ///< A type that represents a bounding box around cells in a 2D grid.
static const KyInt32 InvalidCellCoord = KyInt32MAXVAL; ///< Represents an invalidCellCoord object.

enum PolygonWinding
{
	POLYGON_IS_CCW,
	POLYGON_IS_CW,
	POLYGON_UNKNOWN_WINDING,
};

/// Enumerates the possible types of obstacles that can be represented by a NavHalfEdge whose type is #EDGETYPE_OBSTACLE.
enum NavHalfEdgeObstacleType
{
	EDGEOBSTACLETYPE_WALL = 0, ///< Indicates that the obstacle represents a wall.
	EDGEOBSTACLETYPE_HOLE = 1, ///< Indicates that the obstacle represents a hole or a cliff.
};

/// Enumerates the possible types of boundary that can be represented by a NavHalfEdge.
enum NavHalfEdgeType
{
	EDGETYPE_CELLBOUNDARY_EAST  = CardinalDir_EAST,  ///< Indicates that this NavHalfEdge lies on the border of its NavFloor and its NavCell. 
	EDGETYPE_CELLBOUNDARY_NORTH = CardinalDir_NORTH, ///< Indicates that this NavHalfEdge lies on the border of its NavFloor and its NavCell. 
	EDGETYPE_CELLBOUNDARY_WEST  = CardinalDir_WEST,  ///< Indicates that this NavHalfEdge lies on the border of its NavFloor and its NavCell. 
	EDGETYPE_CELLBOUNDARY_SOUTH = CardinalDir_SOUTH, ///< Indicates that this NavHalfEdge lies on the border of its NavFloor and its NavCell. 
	EDGETYPE_FLOORBOUNDARY      = 4,                 ///< Indicates that this NavHalfEdge lies on the border of its NavFloor.
	EDGETYPE_OBSTACLE           = 5,                 ///< Indicates that this NavHalfEdge lies on an external border of the NavMesh.
	EDGETYPE_CONNEXBOUNDARY     = 6,                 ///< Indicates that another NavHalfEdge in the same NavFloor but in different Connex lies adjacent to the NavHalfEdge.
	EDGETYPE_PAIRED             = 7,                 ///< Indicates that another NavHalfEdge in the same Connex lies adjacent to the NavHalfEdge.
};

KY_INLINE bool IsHalfEdgeACellBoundary(NavHalfEdgeType type)        { return type < EDGETYPE_FLOORBOUNDARY;  }
KY_INLINE bool IsHalfEdgeAFloorOrCellBoundary(NavHalfEdgeType type) { return type < EDGETYPE_OBSTACLE;       }
KY_INLINE bool IsHalfEdgeOnNavFloorBorder(NavHalfEdgeType type)     { return type < EDGETYPE_CONNEXBOUNDARY; }
KY_INLINE bool IsHalfEdgeOnConnexBorder(NavHalfEdgeType type)       { return type < EDGETYPE_PAIRED;         }


// --- vertex Index in the Floor ---

typedef KyUInt16 CompactNavVertexIdx;
static const CompactNavVertexIdx CompactNavVertexIdx_MAXVAL = 0x0FFF; // vertexIndex encoded en 12 bits in NavHalfEdge

typedef KyUInt32 NavVertexIdx;                                                             ///< An index that uniquely identifies a single vertex of a triangle within the set of vertices owned by a NavFloor.  
static const NavVertexIdx NavVertexIdx_Invalid = (NavVertexIdx)CompactNavVertexIdx_MAXVAL; ///< Represents an invalid #NavVertexIdx.  

///< Indicates whether or not the specified NavVertexIdx is valid.
KY_INLINE bool IsNavVertexIdxValid(const NavVertexIdx idx) { return idx < NavVertexIdx_Invalid; }


// --- HalfEdge Index in the Floor ---

typedef KyUInt16 CompactNavHalfEdgeIdx;
static const CompactNavHalfEdgeIdx CompactNavHalfEdgeIdx_MAXVAL = 0x3FFF;  // NavHalfEdgeIdx coded en 14 bits in NavHalfEdge

typedef KyUInt32 NavHalfEdgeIdx; ///< An index that uniquely identifies a single edge of a triangle within the set of edges owned by a NavFloor.  
static const NavHalfEdgeIdx NavHalfEdgeIdx_Invalid = (NavHalfEdgeIdx)CompactNavHalfEdgeIdx_MAXVAL; ///< Represents an invalid #NavHalfEdgeIdx.  

/// Indicates whether or not the specified NavHalfEdgeIdx is valid.  
KY_INLINE bool IsNavHalfEdgeIdxValid(const NavHalfEdgeIdx idx) { return idx < NavHalfEdgeIdx_Invalid; }


// --- Triangle Index in the Floor ---

//WARNING, we call it Idx opposed to NavTrianglePtr, but it is not an Idx in an array
typedef KyUInt16 CompactNavTriangleIdx;
static const CompactNavTriangleIdx CompactNavTriangleIdx_MAXVAL = 0xFFFF;

typedef KyUInt32 NavTriangleIdx; ///< An index that uniquely identifies a single triangle within the set of triangles owned by a NavFloor.  
static const NavTriangleIdx NavTriangleIdx_Invalid = (NavTriangleIdx)CompactNavVertexIdx_MAXVAL; ///< Represents an invalid #NavTriangleIdx.  

/// Indicates whether or not the specified NavTriangleIdx is valid.  
KY_INLINE bool IsNavTriangleIdxValid(const NavTriangleIdx idx) { return idx < NavTriangleIdx_Invalid; }

//WARNING, we call it Idx opposed to NavConnexPtr, but it is not an Idx in an array
typedef KyUInt16 CompactNavConnexIdx;
static const CompactNavConnexIdx CompactNavConnexIdx_MAXVAL = 0xFFFF;

typedef KyUInt32 NavConnexIdx; ///< An index that uniquely identifies a single triangle within the set of triangles owned by a NavFloor.  
static const NavConnexIdx NavConnexIdx_Invalid = (NavConnexIdx)CompactNavVertexIdx_MAXVAL; ///< Represents an invalid #NavConnexIdx.  

// --- Floor Index in the Cell ---

typedef KyUInt32 NavFloorIdx; ///< An index that uniquely identifies a single NavFloor within the set of NavFloors owned by a NavCell.  
static const NavFloorIdx NavFloorIdx_Invalid = KyUInt32MAXVAL; ///< Represents an invalid #NavFloorIdx.  

/// Indicates whether or not the specified NavFloorIdx is valid.  
KY_INLINE bool IsNavFloorIdxValid(const NavFloorIdx idx) { return idx < NavFloorIdx_Invalid; }

// --- NavCellBlob Index in a NavMeshElement ---

typedef KyUInt32 NavCellIdx; ///< An index that uniquely identifies a single NavCell within the set of NavCells owned by a NavMesh.  
static const NavCellIdx NavCellIdx_Invalid = KyUInt32MAXVAL; ///< Represents an invalid #NavCellIdx.  

/// Indicates whether or not the specified NavCellIdx is valid.  
KY_INLINE bool IsNavCellIdxValid(const NavCellIdx idx) { return idx < NavCellIdx_Invalid; }

// --- index of a Cell in the buffer of ActiveData ---

typedef KyUInt32 CellIdxInActiveData;
static const CellIdxInActiveData CellIdxInActiveData_Invalid = 0x0FFFFFFF; // limit NavFloorIdx to 0x0FFFFFFF to be able to store IdxInActiveData and ActiveCellIdx on 32 bits

// --- Id of connected component ---

typedef KyUInt32 ConnectedComponentId;
static const ConnectedComponentId ConnectedComponent_UNDEFINED = KyUInt32MAXVAL;
static const ConnectedComponentId ConnectedComponent_TRAVERSALOPEN = KyUInt32MAXVAL - 1;

#define KY_DEFINE_TEMPLATE_MASK_MANIPULATOR(integerType)                                                                   \
template<int BitIdx> KY_INLINE void SetBitInMask(integerType& mask)   { mask |=  (1 << (integerType)BitIdx); }             \
template<int BitIdx> KY_INLINE void UnSetBitInMask(integerType& mask) { mask &= ~(1 << (integerType)BitIdx); }             \
template<int BitIdx> KY_INLINE bool IsBitSetInMask(integerType mask)  { return (mask & (1 << (integerType)BitIdx)) != 0; } \

KY_DEFINE_TEMPLATE_MASK_MANIPULATOR(KyUInt8)
KY_DEFINE_TEMPLATE_MASK_MANIPULATOR(KyUInt16)
KY_DEFINE_TEMPLATE_MASK_MANIPULATOR(KyUInt32)

}

#endif

