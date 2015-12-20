/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_GeneratorIntegerCoords_H
#define GwNavGen_GeneratorIntegerCoords_H


#include "gwnavruntime/basesystem/intcoordsystem.h"


namespace Kaim
{

class GeneratorSystem;

class GeneratorIntegerCoords
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavDataGen)

public:
	GeneratorIntegerCoords();

	void Init(KyFloat32 normalizedRasterPrecision);

	bool IsTriangleTooBig(const Vec3f& A, const Vec3f& B, const Vec3f& C) const;

	bool AreTriangleCoordinatesValid(const Vec3f& A, const Vec3f& B, const Vec3f& C) const;

	bool IsCoordinateValid(const Vec3f& pos) const;

	KyFloat32 IntToMeter_Dist(KyUInt32 intValue) const
	{
		return IntCoordSystem::IntToNavigation_Dist(intValue, m_normalizedRasterPrecision);
	}

	KyUInt32 MeterToInt_Dist(KyFloat32 coordInMeter) const
	{
		return IntCoordSystem::NavigationToInt_Dist(coordInMeter, m_normalizedRasterPrecision);
	}

	void IntToMeter_Pos(const Vec2i& intPos, Vec2f& pos) const
	{
		pos.x = IntToMeter_Dist(intPos.x);
		pos.y = IntToMeter_Dist(intPos.y);
	}

	void IntToMeter_Pos(const Vec3i& intPos, Vec3f& pos) const
	{
		pos.x = IntToMeter_Dist(intPos.x);
		pos.y = IntToMeter_Dist(intPos.y);
		pos.z = IntToMeter_Dist(intPos.z);
	}

	void MeterToInt_Pos(const Vec2f& pos, Vec2i& intPos) const
	{
		intPos.x = MeterToInt_Dist(pos.x);
		intPos.y = MeterToInt_Dist(pos.y);
	}

	void MeterToInt_Pos(const Vec3f& pos, Vec3i& intPos) const
	{
		intPos.x = MeterToInt_Dist(pos.x);
		intPos.y = MeterToInt_Dist(pos.y);
		intPos.z = MeterToInt_Dist(pos.z);
	}

public:
	KyFloat32 m_normalizedRasterPrecision;
	KyFloat32 m_maxTriangleSquareLength;
};

}


#endif

