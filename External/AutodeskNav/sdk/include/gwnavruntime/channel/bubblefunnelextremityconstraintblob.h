/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_BubbleFunnelExtremityConstraintBlob_H
#define Navigation_BubbleFunnelExtremityConstraintBlob_H

#include "gwnavruntime/channel/bubblefunnelextremityconstraint.h"
#include "gwnavruntime/channel/bubbleblob.h"



namespace Kaim
{

class BubbleFunnelExtremityConstraintBlob
{
	KY_ROOT_BLOB_CLASS(Runtime, BubbleFunnelExtremityConstraintBlob, 0)

public:
	BubbleFunnelExtremityConstraintBlob() {}

public:
	BubbleBlob m_leftBubble;
	BubbleBlob m_rightBubble;
	KyUInt32 m_constraintType;
	KyUInt32 m_firstGateIndex;
	KyUInt32 m_lastGateIndex;
};

inline void SwapEndianness(Endianness::Target e, BubbleFunnelExtremityConstraintBlob& self)
{
	SwapEndianness(e, self.m_leftBubble);
	SwapEndianness(e, self.m_rightBubble);
	SwapEndianness(e, self.m_constraintType);
	SwapEndianness(e, self.m_firstGateIndex);
	SwapEndianness(e, self.m_lastGateIndex);
}


class BubbleFunnelExtremityConstraintBlobBuilder : public BaseBlobBuilder<BubbleFunnelExtremityConstraintBlob>
{
	KY_CLASS_WITHOUT_COPY(BubbleFunnelExtremityConstraintBlobBuilder)

public:
	BubbleFunnelExtremityConstraintBlobBuilder(const BubbleFunnelExtremityConstraint* bubbleFunnelExtremityConstraint)
		: m_bubbleFunnelExtremityConstraint(bubbleFunnelExtremityConstraint)
	{}

	virtual void DoBuild()
	{
		BLOB_BUILD(m_blob->m_leftBubble,     BubbleBlobBuilder(&m_bubbleFunnelExtremityConstraint->m_leftBubble));
		BLOB_BUILD(m_blob->m_rightBubble,    BubbleBlobBuilder(&m_bubbleFunnelExtremityConstraint->m_rightBubble));
		BLOB_SET(m_blob->m_constraintType, KyUInt32(m_bubbleFunnelExtremityConstraint->m_constraintType));
		BLOB_SET(m_blob->m_firstGateIndex, m_bubbleFunnelExtremityConstraint->m_firstGateIndex);
		BLOB_SET(m_blob->m_lastGateIndex,  m_bubbleFunnelExtremityConstraint->m_lastGateIndex);
	}

private:
	const BubbleFunnelExtremityConstraint* m_bubbleFunnelExtremityConstraint;
};

} // namespace Kaim

#endif // Navigation_BubbleFunnelExtremityConstraint_H
