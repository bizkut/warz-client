/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_PositionSpatializationRange_H
#define Navigation_PositionSpatializationRange_H

#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/base/endianness.h"


namespace Kaim
{

class PositionSpatializationRange
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_WorldFwk)

public:
	PositionSpatializationRange()
		: m_rangeAbovePosition(0.0f)
		, m_rangeBelowPosition(0.0f)
	{}

	PositionSpatializationRange(KyFloat32 rangeAbovePosition, KyFloat32 rangeBelowPosition)
		: m_rangeAbovePosition(rangeAbovePosition)
		, m_rangeBelowPosition(rangeBelowPosition)
	{}

	KyFloat32 m_rangeAbovePosition; // Range of altitude above positions that will be searched for triangles.
	KyFloat32 m_rangeBelowPosition; // Range of altitude below positions that will be searched for triangles.
};

KY_INLINE void SwapEndianness(Endianness::Target e, PositionSpatializationRange& self)
{
	SwapEndianness(e, self.m_rangeAbovePosition);
	SwapEndianness(e, self.m_rangeAbovePosition);
}


class DatabaseGenMetrics;

class DatabasePositionSpatializationRange
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_WorldFwk)
	KY_CLASS_WITHOUT_COPY(DatabasePositionSpatializationRange)

public:
	DatabasePositionSpatializationRange()
		: m_genMetrics(KY_NULL)
		, m_customized(false)
	{}

	void InitFromGenerationMetrics();
	KyResult SetSpatializationRange(KyFloat32 abovePosition, KyFloat32 belowPosition);

	const PositionSpatializationRange& GetPositionSpatializationRange() const { return m_positionSpatializationRange; }
	bool IsCustomized() const { return m_customized; }

private:
	friend class Database;
	PositionSpatializationRange m_positionSpatializationRange;
	DatabaseGenMetrics* m_genMetrics;
	bool m_customized;
};

} // namespace Kaim

#endif // Navigation_PositionSpatializationRange_H
