/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_Channel_H
#define Navigation_Channel_H

#include "gwnavruntime/visualsystem/displaylistblob.h"
#include "gwnavruntime/kernel/SF_RefCount.h"


namespace Kaim
{

class World;
class Bubble;


/// Enumerates the different kind of Gates.
enum GateType
{
	UndefinedGateType,     ///< Indicates the Gate is not defined.

	StartGate,             ///< Indicates the Gate is a Channel start Gate.
	EndGate,               ///< Indicates the Gate is a Channel end Gate.

	SmallLeftTurn,         ///< Indicates the Gate is a small turn to the left (counterclockwise).
	LeftTurnStart,         ///< Indicates the Gate is the start of a sampled turn to the left (counterclockwise). The next Gate must have either LeftTurnIntermediary or LeftTurnEnd type value.
	LeftTurnIntermediary,  ///< Indicates the Gate is a sampled left turn intermediary Gate.
	LeftTurnEnd,           ///< Indicates the Gate is a left turn end.

	SmallRightTurn,        ///< Indicates the Gate is a small turn to the right (clockwise).
	RightTurnStart,        ///< Indicates the Gate is the start of a sampled turn to the right (clockwise). The next Gate must have either RightTurnIntermediary or RightTurnEnd type value.
	RightTurnIntermediary, ///< Indicates the Gate is a sampled right turn intermediary Gate.
	RightTurnEnd,          ///< Indicates the Gate is a right turn end.

	WidthAdjustment,       ///< Indicates the Gate is a Channel width adjustment Gate. There is no direction modification there.
	ClampingAdjustment,    ///< Indicates the Gate is a Channel altitude clamping adjustment Gate. There is no direction modification there.

	GATE_TYPE_COUNT
};

KY_INLINE bool IsALeftTurnGate(GateType gateType)
{
	switch (gateType)
	{
	case SmallLeftTurn        :
	case LeftTurnStart        :
	case LeftTurnIntermediary :
	case LeftTurnEnd          :
		return true;

	default:
		return false;
	}
}

KY_INLINE bool IsARightTurnGate(GateType gateType)
{
	switch (gateType)
	{
	case SmallRightTurn       :
	case RightTurnStart       :
	case RightTurnIntermediary:
	case RightTurnEnd         :
		return true;

	default:
		return false;
	}
}

KY_INLINE bool IsATurnGate(GateType gateType) { return (IsALeftTurnGate(gateType) || IsARightTurnGate(gateType)); }


/// Enumerates the Channel::IsPositionInSection relative position flags.
enum RelativePositionToChannelSectionFlag
{
	RelativePositionToChannelSection_Inside  = 0, ///< Position is inside the section.

	RelativePositionToChannelSection_Before  = 1, ///< Position is outside the section: it lays before the section start Gate.
	RelativePositionToChannelSection_After   = 2, ///< Position is outside the section: it lays after the section end Gate.
	RelativePositionToChannelSection_OnLeft  = 4, ///< Position is outside the section: it lays on the left of the left Border.
	RelativePositionToChannelSection_OnRight = 8, ///< Position is outside the section: it lays on the right of the right Border.
};


/// Classes that configures the Channel visual debug display.
class ChannelDisplayConfig
{
public:
	ChannelDisplayConfig() { SetDefaults(); }
	void SetDefaults();

	bool m_displayOutline;
	bool m_displayGates;
	bool m_displayPath;
	bool m_displayIndices;
	bool m_displayGateType;

	VisualColor m_gateIndexColor;
	VisualColor m_pathColor;
	VisualColor m_leftBorderColor;
	VisualColor m_rightBorderColor;
	VisualShapeColor m_nodeColor;

	VisualColor m_gateColor[GATE_TYPE_COUNT];

	static const char* GetGateTypeText(GateType gateType);

	KyFloat32 m_nodeRenderRadius;
	KyFloat32 m_verticalOffset;
	KyFloat32 m_gateHeight;
};

/// This class represents Channel section limit segment. See Channel class description.
class Gate
{
public:
	Gate() : m_type(UndefinedGateType) {}
	Gate(const Vec3f& leftPos, const Vec3f& pathPos, const Vec3f& rightPos, GateType gateType)
		: m_type(gateType)
		, m_leftPos(leftPos)
		, m_pathPos(pathPos)
		, m_rightPos(rightPos)
	{}

	GateType m_type;
	Vec3f m_leftPos;
	Vec3f m_pathPos;
	Vec3f m_rightPos;
};


/// Channels define preferred navigation areas around an original Path section on
/// NavMesh. They are typically used to drive bots which inertia prevent to use
/// reactive path follower to drive it correctly along the original path itself.
/// We provide a default ChannelTrajectory that computes splines in the Channel to drive the bot.
class Channel : public RefCountBase<Channel, MemStat_Channel>
{
public:
	Channel()
		: m_firstPathNodeIdx(0)
	{}

	~Channel();


	// ------- Main API Functions --------

	KyUInt32 GetSectionCount() const; // GateCount + 1
	KyUInt32 GetGateCount() const;

	const Gate& GetGate(KyUInt32 index) const;
	const KyArray<Vec3f>& GetPreChannelPolyline() const;
	const KyArray<Vec3f>& GetPostChannelPolyline() const;

	/// Returns the channel end position, i.e. the m_pathPos of its last Gate.
	/// \pre The Channel must not be empty.
	const Vec3f& GetEndPos() const;

	/// Return PathNodeIdx of the first gate
	KyUInt32 GetFirstPathNodeIdx() const;

	/// Return PathNodeIdx of the gate at gateIndex
	KyUInt32 GetGatePathNodeIdx(KyUInt32 gateIndex) const;

	/// Checks a provided 2D position is inside a channel section. It updates positionFlags
	/// with a combination of the RelativePositionToChannelSectionFlag.
	bool IsPositionInSection(const Vec2f& position, KyUInt32 sectionIndex, KyUInt32& positionFlags) const;

	/// Just checks a provided 2D position is inside a channel section.
	bool IsPositionInSection(const Vec2f& position, KyUInt32 sectionIndex) const;

	/// Given a previous position and its section, computes the section for a new position
	/// that is supposed to be near the previous one.
	KyResult GetNextSection(const Kaim::Vec3f& previousPosition, KyUInt32 previousSectionIdx, const Kaim::Vec3f& newPosition, KyUInt32& newSectionIdx) const;

public: // internal
	KyResult GetNextSection(const Kaim::Vec3f& previousPosition, KyUInt32 previousSectionIdx, const Kaim::Vec3f& newPosition, KyUInt32& newSectionIdx, KyFloat32 maxProgress) const;
	KyResult GetNextSection_Forward(const Kaim::Vec3f& previousPosition, KyUInt32 previousGateIdx, const Kaim::Vec3f& newPosition, KyUInt32& newGateIdx, KyFloat32 maxProgress) const;
	KyResult GetNextSection_Backward(const Kaim::Vec3f& previousPosition, KyUInt32 previousGateIdx, const Kaim::Vec3f& newPosition, KyUInt32& newGateIdx, KyFloat32 maxProgress) const;
	KyResult GetNextSection_AlongBubble(const Bubble& bubble, const Kaim::Vec3f& previousPosition, KyUInt32 previousSectionIdx, const Kaim::Vec3f& newPosition, KyUInt32& newSectionIdx) const;

	bool IntersectSegmentVsPreChannelPolyline(const Vec3f& A, const Vec3f& B, Vec3f& intersection) const;
	bool IntersectSegmentVsPostChannelPolyline(const Vec3f& A, const Vec3f& B, Vec3f& intersection) const;

	void SendVisualDebug(World* navWorld, const ChannelDisplayConfig& displayConfig = ChannelDisplayConfig(), const char* listBaseName = "", const char* groupName = "Channel", KyUInt32 visualDebugId = KyUInt32MAXVAL) const;

	// --------  Creation functions ------
	/// These functions are expected to be used internally in Channel computation
	/// classes. Though you can use them when creating your own Channel.

	void Initialize();
	void Clear();
	void AddGate(const Gate& gate);
	Gate& GetGate(KyUInt32 index);
	void SetPreChannelPolyline(const KyArray<Vec3f>& polyline);
	void SetPostChannelPolyline(const KyArray<Vec3f>& polyline);

	void SetFirstPathNodeIdx(KyUInt32 firstPathNodeIdx) { m_firstPathNodeIdx = firstPathNodeIdx; }

public: // internal : to be used with lot of precaution
	// Computes if the given position is inside the polyline, with polyline being
	// a function on its closure segment (for each point P along the polyline, its
	// orthogonal projection M on the closure segment is so that [MP[ doesn't
	// intersect the polyline.
	bool IsInsidePolyline(const Vec2f& position, const KyArray<Vec3f>& polyline) const;

	bool IntersectSegmentVsPolyline(const Vec3f& A, const Vec3f& B, Vec3f& intersection, const KyArray<Vec3f>& polyline) const;
	bool IsPositionWithinSectionBorders(const Vec2f& newPos2d, const Vec2f& A, const Vec2f& B, const Vec2f& C, const Vec2f& D) const;

public: // internal
	KyArray<Gate> m_gates;
	KyArray<Vec3f> m_preChannelPolyline;
	KyArray<Vec3f> m_postChannelPolyline;
	KyUInt32 m_firstPathNodeIdx;
};

KY_INLINE             Channel::~Channel()                    { Clear(); }
KY_INLINE KyUInt32    Channel::GetSectionCount()       const { KyUInt32 count = GetGateCount(); return (count ? (count + 1) : 0); }
KY_INLINE KyUInt32    Channel::GetGateCount()          const { return m_gates.GetCount(); }
KY_INLINE const Gate& Channel::GetGate(KyUInt32 index) const { return m_gates[index]; }

KY_INLINE const KyArray<Vec3f>& Channel::GetPreChannelPolyline() const { return m_preChannelPolyline; }
KY_INLINE const KyArray<Vec3f>& Channel::GetPostChannelPolyline() const { return m_postChannelPolyline; }

KY_INLINE const Vec3f& Channel::GetEndPos() const { return m_gates[GetGateCount() - 1].m_pathPos; }

KY_INLINE bool        Channel::IntersectSegmentVsPreChannelPolyline(const Vec3f& A, const Vec3f& B, Vec3f& intersection)  const { return IntersectSegmentVsPolyline(A, B, intersection, m_preChannelPolyline);  }
KY_INLINE bool        Channel::IntersectSegmentVsPostChannelPolyline(const Vec3f& A, const Vec3f& B, Vec3f& intersection) const { return IntersectSegmentVsPolyline(A, B, intersection, m_postChannelPolyline); }

KY_INLINE void        Channel::Initialize()                  { Clear(); }
KY_INLINE void        Channel::Clear()                       { m_gates.Clear(); }
KY_INLINE void        Channel::AddGate(const Gate& gate)     { m_gates.PushBack(gate); }
KY_INLINE Gate&       Channel::GetGate(KyUInt32 index)       { return m_gates[index]; }

KY_INLINE void        Channel::SetPreChannelPolyline(const KyArray<Vec3f>& polyline) { m_preChannelPolyline = polyline; }
KY_INLINE void        Channel::SetPostChannelPolyline(const KyArray<Vec3f>& polyline) { m_postChannelPolyline = polyline; }

KY_INLINE KyUInt32    Channel::GetFirstPathNodeIdx() const                  { return m_firstPathNodeIdx; }
KY_INLINE KyUInt32    Channel::GetGatePathNodeIdx(KyUInt32 gateIndex) const { return m_firstPathNodeIdx + gateIndex; }

KY_INLINE bool Channel::IsPositionInSection(const Vec2f& position, KyUInt32 sectionIndex) const
{
	KyUInt32 unusedPositionFlags;
	return IsPositionInSection(position, sectionIndex, unusedPositionFlags);
}

} // namespace Kaim

#endif // Navigation_Channel_H
