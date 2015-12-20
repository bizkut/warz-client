/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_StringPuller_H
#define Navigation_StringPuller_H

#include "gwnavruntime/channel/bubblearray.h"

#include "gwnavruntime/channel/stringpulledbubblelist.h"
#include "gwnavruntime/channel/bubblefunnel.h"
#include "gwnavruntime/channel/bubblefunnelextremityconstraint.h"

#include "gwnavruntime/visualsystem/visualcolor.h"
#include "gwnavruntime/visualsystem/displaylistblob.h"


namespace Kaim
{

class World;

class StringPullerComputerConfig
{
public:
	StringPullerComputerConfig() { SetDefaults(); }

	void SetDefaults();

	KyFloat32 m_idealClearanceRadius;
	KyFloat32 m_minBiTangentLength;
};

/// Computes a StringPulledBubbleList from the provided array of bubbles. The order in the
/// Bubble array defines the way the string should respect within the Bubbles.
class StringPuller
{
public:
	StringPuller()
		: m_startConstraint(KY_NULL)
		, m_startIdx(2)
	{}

	void Initialize();
	void Clear() {}

	KyResult Compute(const BubbleArray& bubbleArray, StringPulledBubbleList& stringPulledBubbleList);

	void SetComputerConfig(const StringPullerComputerConfig& computerConfig)   { m_computerConfig = computerConfig; }
	void SetStartConstraint(const BubbleFunnelExtremityConstraint* constraint) { m_startConstraint = constraint; }
	void SetStartIdx(KyUInt32 startIdx)                                        { m_startIdx = startIdx; }

	KyFloat32 GetComputationDurationMks() const { return m_computationDurationMks; }

private:
	KyResult ResolveFunnelClosureCaseAndCompleteLocalBubblesCollection(const BubbleArray& bubbleArray, KyUInt32 lastAddedCornerIdx,
		BubbleFunnel& funnel, KyUInt32& closingBubbleIdx, KyUInt32& pivotBubbleIdx, KyArray<KyUInt32>& indexArray);

	KyResult SolveLocalStringPulledBubbleList(const BubbleArray& bubbleArray, KyArray<KyUInt32>& cornerIndexArray,
		const BubbleFunnel& funnel, KyUInt32 closingBubbleIdx, KyUInt32 pivotBubbleIdx,
		StringPulledBubbleList& stringPulledBubbleList, KyUInt32& nextApexIdxInBubbleArray, KyUInt32& nextApexIdxInIndexArray);

	KyResult ComputeFunnelNextStartBubbleIdx(const KyArray<KyUInt32>& indexArray, KyUInt32 nextApexIdxInBubbleArray, KyUInt32& funnelNextBubbleToIntegrateIdx);


	StringPullerComputerConfig m_computerConfig;
	const BubbleFunnelExtremityConstraint* m_startConstraint;

	KyUInt32 m_startIdx;

	// stats
	KyFloat32 m_computationDurationMks;
};


} // namespace Kaim

#endif // Navigation_StringPuller_H
