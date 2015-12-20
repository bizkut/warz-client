/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: NOBODY

#ifndef Navigation_ChannelSectionPtr_H
#define Navigation_ChannelSectionPtr_H

#include "gwnavruntime/channel/channelarray.h"
#include "gwnavruntime/kernel/SF_RefCount.h"


namespace Kaim
{

class PositionOnPath;

class ChannelSectionPtr
{
public:

	ChannelSectionPtr();
	ChannelSectionPtr(Ptr<ChannelArray> channelArray, KyUInt32 channelIdx, KyUInt32 sectionIdx);
	~ChannelSectionPtr();

	void Invalidate();

	KyResult Set(Ptr<ChannelArray> channelArray, KyUInt32 channelIdx, KyUInt32 sectionIdx);

	KyResult SetFromPositionOnPath(const PositionOnPath& positionOnPath);

	/// Updates the ChannelSectionPtr to make it match the provided newPos. prevPos
	/// is intended to be the position with which this instance has been last updated.
	/// This is typically used when updating frame to frame from a continuously moving
	/// position.
	KyResult Update(const Vec3f& prevPos, const Vec3f& newPos);

	/// Updates the ChannelSectionPtr to make it match the provided newPos. positionOnPath
	/// is intended to be a valid PositionOnPath nearby newPos to be used as reference.
	/// This is typically used when updating from scratch because newPos is far away from
	/// the position used at last update.
	KyResult Update(const PositionOnPath& positionOnPath, const Vec3f& newPos);

	bool IsValid() const;

	/// Indicate the section is the first one, i.e. the one delimited by
	/// Channel pre-channel polyline and the first Gate;
	bool IsFirstSection() const;

	/// Indicate the section is the last one, i.e. the one delimited by
	/// Channel post-channel polyline and the last Gate;
	bool IsLastSection() const;

	/// Indicate the section has a triangular shape and it turns to the left.
	/// Hence, GetPreviousGate().m_leftPos == GetNextGate().m_leftPos
	bool IsTriangularLeftTurn() const;

	/// Indicate the section has a triangular shape and it turns to the right.
	/// Hence, GetPreviousGate().m_rightPos == GetNextGate().m_rightPos
	bool IsTriangularRightTurn() const;

	/// \pre This ChannelSectionPtr must be valid.
	bool IsOnLastSectionOfCurrentChannel() const { return m_sectionIdx == GetChannel()->GetGateCount() + 1; }

	/// Checks a provided 2D position is inside this channel section. It updates positionFlags
	/// with a combination of the RelativePositionToChannelSectionFlag.
	bool IsPositionInSection(const Vec2f& position, KyUInt32& positionFlags) const;

	KyUInt32            GetChannelIdx()      const { return m_channelIdx; }
	KyUInt32            GetSectionIdx()      const { return m_sectionIdx; }

	KyUInt32            GetPreviousGateIdx() const { return ((m_sectionIdx > 1) ? (m_sectionIdx - 1) : 0); }
	KyUInt32            GetNextGateIdx()     const { return Min(m_sectionIdx, GetChannel()->GetGateCount() - 1); }
	ChannelArray*       GetChannelArray()    const { return m_channelArray; }
	const Channel*      GetChannel()         const { return (m_channelArray ? m_channelArray->GetChannel(m_channelIdx) : KY_NULL); }

	const Gate&         GetPreviousGate()    const { return GetChannel()->GetGate(GetPreviousGateIdx()); }
	const Gate&         GetNextGate()        const { return GetChannel()->GetGate(GetNextGateIdx()); }
	KyUInt32            GetPreviousGatePathNodeIdx() const { return GetChannel()->GetGatePathNodeIdx(GetPreviousGateIdx()); }
	KyUInt32            GetNextGatePathNodeIdx() const     { return GetChannel()->GetGatePathNodeIdx(GetNextGateIdx()); }
	KyUInt32            GetLastGatePathNodeIdx() const     { return GetChannel()->GetGatePathNodeIdx(GetChannel()->GetGateCount() -1); }


	bool operator==(const ChannelSectionPtr& other) const
	{
		return (
			(m_channelArray == other.m_channelArray) &&
			(m_channelIdx == other.m_channelIdx) &&
			(m_sectionIdx == other.m_sectionIdx));
	}

	bool operator!=(const ChannelSectionPtr& other) const
	{
		return !(*this==other);
	}

public: // internal
	Ptr<ChannelArray> m_channelArray;
	KyUInt32  m_channelIdx;  // The index of the Channel in the ChannelArray.
	KyUInt32  m_sectionIdx;     // The index of the section, i.e. the Gate starting the quad or triangle this position stays on.
};

KY_INLINE bool ChannelSectionPtr::IsFirstSection() const
{
	return (m_sectionIdx == 0);
}

KY_INLINE bool ChannelSectionPtr::IsLastSection() const
{
	return (m_sectionIdx == GetChannel()->GetGateCount());
}

KY_INLINE bool ChannelSectionPtr::IsTriangularRightTurn() const
{
	GateType type = GetPreviousGate().m_type; 
	return (type == RightTurnStart) || (type == RightTurnIntermediary);
}

KY_INLINE bool ChannelSectionPtr::IsTriangularLeftTurn() const
{
	GateType type = GetPreviousGate().m_type; 
	return (type == LeftTurnStart) || (type == LeftTurnIntermediary);
}

KY_INLINE bool ChannelSectionPtr::IsPositionInSection(const Vec2f& position, KyUInt32& positionFlags) const
{
	return GetChannel()->IsPositionInSection(position, m_sectionIdx, positionFlags);
}


template <class OSTREAM>
inline OSTREAM& operator<<(OSTREAM& os, const ChannelSectionPtr& channelSectionPtr)
{
	os << "{" << channelSectionPtr.m_channelIdx << ", " << channelSectionPtr.m_sectionIdx << "}";
	return os;
}

} // namespace Kaim

#endif // Navigation_ChannelSectionPtr_H
