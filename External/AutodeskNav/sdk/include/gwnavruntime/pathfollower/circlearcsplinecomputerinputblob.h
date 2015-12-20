/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: NOBODY

#ifndef Navigation_CircleArcSplineComputerInputBlob_H
#define Navigation_CircleArcSplineComputerInputBlob_H

#include "gwnavruntime/world/runtimeblobcategory.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/math/vec3f.h"

#include "gwnavruntime/pathfollower/circlearcsplinecomputer.h"
#include "gwnavruntime/channel/channelarrayblob.h"
#include "gwnavruntime/pathfollower/radiusprofileblob.h"
#include "gwnavruntime/channel/bubblefunnelextremityconstraintblob.h"


namespace Kaim
{

// Internal:
// A blob containing all the necessary information to replay a CircleArcSplineComputer.
// This is typically used to replay error cases.
class CircleArcSplineComputerInputBlob
{
	KY_ROOT_BLOB_CLASS(Runtime, CircleArcSplineComputerInputBlob, 0)

public:
	CircleArcSplineComputerInputBlob() {}

public:
	ChannelBlob m_channel;

	SplineConfig m_splineConfig;
	RadiusProfileArrayBlob m_radiusProfileArray;
	BubbleFunnelExtremityConstraintBlob m_startConstraint;

	Vec3f m_startPosition;
	Vec3f m_endPosition;

	KyUInt32 m_startIndex;
	KyUInt32 m_endIndex;
};

inline void SwapEndianness(Endianness::Target e, CircleArcSplineComputerInputBlob& self)
{
	SwapEndianness(e, self.m_channel);
	SwapEndianness(e, self.m_splineConfig);
	SwapEndianness(e, self.m_radiusProfileArray);
	SwapEndianness(e, self.m_startConstraint);
	SwapEndianness(e, self.m_startPosition);
	SwapEndianness(e, self.m_endPosition);
	SwapEndianness(e, self.m_startIndex);
	SwapEndianness(e, self.m_endIndex);
}



class CircleArcSplineComputerInputBlobBuilder : public BaseBlobBuilder<CircleArcSplineComputerInputBlob>
{
	KY_CLASS_WITHOUT_COPY(CircleArcSplineComputerInputBlobBuilder)

public:
	CircleArcSplineComputerInputBlobBuilder(const CircleArcSplineComputer* computer)
		: m_computer(computer)
	{}

	virtual void DoBuild()
	{
		BLOB_BUILD(m_blob->m_channel, ChannelBlobBuilder(m_computer->m_startSection.GetChannel()));
		BLOB_SET(m_blob->m_splineConfig, m_computer->m_splineConfig);
		BLOB_BUILD(m_blob->m_radiusProfileArray, RadiusProfileArrayBlobBuilder(&m_computer->m_radiusProfileArray));
		BLOB_BUILD(m_blob->m_startConstraint, BubbleFunnelExtremityConstraintBlobBuilder(&m_computer->m_startConstraint));
		BLOB_SET(m_blob->m_startPosition, m_computer->m_startPosition);
		BLOB_SET(m_blob->m_endPosition, m_computer->m_endPosition);
		BLOB_SET(m_blob->m_startIndex, m_computer->m_startSection.GetSectionIdx());
		BLOB_SET(m_blob->m_endIndex, m_computer->m_endSection.GetSectionIdx());
	}

private:
	const CircleArcSplineComputer* m_computer;
};


} // namespace Kaim

#endif // Navigation_CircleArcSplineComputerInputBlob_H
