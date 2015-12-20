/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavVertex_H
#define Navigation_NavVertex_H

#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/base/endianness.h"
#include "gwnavruntime/basesystem/intcoordsystem.h"

namespace Kaim
{

/// Represents a single vertex that forms the corner of at least one NavMesh triangle.
/// NavVertex coordinates are stored on 8 bits and are local to its NavCell.
class NavVertex
{
public:
	NavVertex();

	KyInt64 GetX64() const; ///< Returns the X coordinate of the vertex. 
	KyInt64 GetY64() const; ///< Returns the Y coordinate of the vertex. 
	KyInt32 GetX32() const; ///< Returns the X coordinate of the vertex. 
	KyInt32 GetY32() const; ///< Returns the Y coordinate of the vertex. 

	CoordPos   GetCoordPos()   const; ///< Returns the position of the vertex in an integer grid. For internal use. 
	CoordPos64 GetCoordPos64() const; ///< Returns the position of the vertex in an integer grid. For internal use. 

	/// Retrieves the size of the vector around one of its axes. Use [0] for the X axis, or [1] for
	/// the Y axis. 
	KY_INLINE KyUInt8 operator[](KyInt32 idx) const { return (&x)[idx]; }

	bool operator==(const NavVertex& v) const;
	bool operator!=(const NavVertex& v) const;
	bool operator< (const NavVertex& v) const;
	bool operator> (const NavVertex& v) const;
public:
	KyUInt8 x; ///< Stores the X coordinate of the vertex. Do not modify. 
	KyUInt8 y; ///< Stores the Y coordinate of the vertex. Do not modify. 
};

/// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework.
/// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped. 
KY_INLINE void SwapEndianness(Endianness::Target e, NavVertex& self)
{
	SwapEndianness(e, self.x);
	SwapEndianness(e, self.y);
}

KY_INLINE NavVertex::NavVertex() : x(0xFF), y(0xFF) {}

KY_INLINE KyInt64    NavVertex::GetX64()        const { return (KyInt64)GetX32();                           }
KY_INLINE KyInt64    NavVertex::GetY64()        const { return (KyInt64)GetY32();                           }
KY_INLINE KyInt32    NavVertex::GetX32()        const { return IntCoordSystem::PixelCoordToInt((KyInt32)x); }
KY_INLINE KyInt32    NavVertex::GetY32()        const { return IntCoordSystem::PixelCoordToInt((KyInt32)y); }
KY_INLINE CoordPos   NavVertex::GetCoordPos()   const { return CoordPos(GetX32(), GetY32());                }
KY_INLINE CoordPos64 NavVertex::GetCoordPos64() const { return CoordPos64(GetX64(), GetY64());              }

KY_INLINE bool NavVertex::operator==(const NavVertex& v) const { return x == v.x && y == v.y;                 }
KY_INLINE bool NavVertex::operator!=(const NavVertex& v) const { return x != v.x || y != v.y;                 }
KY_INLINE bool NavVertex::operator< (const NavVertex& v) const { return (x != v.x) ? (x < v.x) : (y < v.y);   }
KY_INLINE bool NavVertex::operator> (const NavVertex& v) const { return !operator<(v) && operator!=(v);       }

}

#endif //Navigation_NavVertex_H

