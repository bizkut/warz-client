/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LASI - secondary contact: GUAL
#ifndef Navigation_IntCoordSystem_H
#define Navigation_IntCoordSystem_H


#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/math/vec3i.h"
#include "gwnavruntime/math/fastmath.h"


namespace Kaim
{

#define KY_IntToPixelBitShift 7
#define KY_PixelSizeInIntCoord 128
#define KY_HalfPixelSizeInIntCoord 64
#define KY_PixelSizeInIntCoord_Float 128.0f

class IntCoordSystem
{
public:
	// [-320;-193] is snapped to -2
	// [-192; -65] is snapped to -1
	// [ -64;  64] is snapped to  0
	// [  65; 192] is snapped to  1
	// [ 193; 320] is snapped to  2
	// [ 321; 448] is snapped to  3
	// ...
	static inline KyInt32 NearestPixelCoord(KyInt32 x) { return IntToPixelCoord(x + Kaim::Isel(x, KY_HalfPixelSizeInIntCoord-1, KY_HalfPixelSizeInIntCoord)); }
	static inline KyInt32 NearestPixelCoord(KyInt64 x) { return IntToPixelCoord(x + Kaim::Lsel(x, KY_HalfPixelSizeInIntCoord-1, KY_HalfPixelSizeInIntCoord)); }

	static inline KyInt32 IntPixelSize() { return KY_PixelSizeInIntCoord; }

	static inline KyInt32 IntToPixelCoord(KyInt32 intCoord)   { return intCoord   >> KY_IntToPixelBitShift; }
	static inline KyInt32 IntToPixelCoord(KyInt64 intCoord)   { return (KyInt32)(intCoord >> KY_IntToPixelBitShift); }
	static inline KyInt32 PixelCoordToInt(KyInt32 pixelCoord) { return pixelCoord << KY_IntToPixelBitShift; }

	// one unit of Int in meter
	static inline KyFloat32 IntegerPrecision(KyFloat32 rasterPrecision) { return rasterPrecision / KY_PixelSizeInIntCoord_Float; }
	static inline KyFloat32 InvIntegerPrecision(KyFloat32 rasterPrecision) { return KY_PixelSizeInIntCoord_Float / rasterPrecision; }
	
	//PERF WARNING: Int->Float cast = LHS on PPC
	static inline KyFloat32 IntToNavigation_Dist(KyInt32 intDist, KyFloat32 rasterPrecision)
	{
		return IntToNavigation(intDist, IntegerPrecision(rasterPrecision));
	}

	//PERF WARNING: Int->Float cast = LHS on PPC
	static inline KyInt32 NavigationToInt_Dist(KyFloat32 dist, KyFloat32 rasterPrecision)
	{
		return NavigationToInt(dist, InvIntegerPrecision(rasterPrecision));
	}

	static inline void IntToNavigation_Pos(const Vec2i& intPos, Vec2f& pos, KyFloat32 rasterPrecision)
	{
		KyFloat32 integerPrecision = IntegerPrecision(rasterPrecision);
		pos.x = IntToNavigation(intPos.x, integerPrecision);
		pos.y = IntToNavigation(intPos.y, integerPrecision);
	}

	//PERF WARNING: Int->Float cast = LHS on PPC
	static inline void IntToNavigation_Pos(const Vec3i& intPos, Vec3f& pos, KyFloat32 rasterPrecision)
	{
		KyFloat32 integerPrecision = IntegerPrecision(rasterPrecision);
		pos.x = IntToNavigation(intPos.x, integerPrecision);
		pos.y = IntToNavigation(intPos.y, integerPrecision);
		pos.z = IntToNavigation(intPos.z, integerPrecision);
	}

	//PERF WARNING: Int->Float cast = LHS on PPC
	static inline void PixelToNavigation_Pos(const Vec3i& intPos, Vec3f& pos, KyFloat32 integerPrecision)
	{
		pos.x = (KyFloat32)(PixelCoordToInt(intPos.x) * integerPrecision);
		pos.y = (KyFloat32)(PixelCoordToInt(intPos.y) * integerPrecision);
		pos.z = (KyFloat32)(PixelCoordToInt(intPos.z) * integerPrecision);
	}

	//PERF WARNING: Int->Float cast = LHS on PPC
	static inline void NavigationToInt_Pos(const Vec2f& pos, Vec2i& intPos, KyFloat32 rasterPrecision)
	{
		KyFloat32 invIntegerPrecision = InvIntegerPrecision(rasterPrecision);
		intPos.x = NavigationToInt(pos.x, invIntegerPrecision);
		intPos.y = NavigationToInt(pos.y, invIntegerPrecision);
	}

	//PERF WARNING: Int->Float cast = LHS on PPC
	static inline void NavigationToInt_Pos(const Vec3f& pos, Vec3i& intPos, KyFloat32 rasterPrecision)
	{
		KyFloat32 invIntegerPrecision = InvIntegerPrecision(rasterPrecision);
		intPos.x = NavigationToInt(pos.x, invIntegerPrecision);
		intPos.y = NavigationToInt(pos.y, invIntegerPrecision);
		intPos.z = NavigationToInt(pos.z, invIntegerPrecision);
	}

private:
	//PERF WARNING: Int->Float cast = LHS on PPC
	static inline KyFloat32 IntToNavigation(KyInt32 intDist, KyFloat32 integerPrecision)
	{
		return ((KyFloat32)intDist) * integerPrecision;
	}
	//PERF WARNING: Int->Float cast = LHS on PPC
	static inline KyInt32 NavigationToInt(KyFloat32 dist, KyFloat32 invIntegerPrecision)
	{
		return NearestInt(dist * invIntegerPrecision);
	}
};

}

#endif

