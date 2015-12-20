/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_BubbleFunnelExtremityConstraint_H
#define Navigation_BubbleFunnelExtremityConstraint_H

#include "gwnavruntime/channel/bubble.h"



namespace Kaim
{

class BubbleFunnelExtremityConstraintBlob;


/// Class defining a Bubble couple constraining string puller at start or end
/// extremity. Bubbles must be:
///   - rotating in opposite directions: left bubble CCW & right bubble CW
///   - exactly tangent in one point that is the bubble array first or last position
//    - compatible with the diagonal strip implicitly defined in the bubble array.
class BubbleFunnelExtremityConstraint
{
public:
	enum ConstraintType
	{
		UninitializedConstraint,
		LeftTurnConstraint,
		RightTurnConstraint,
		FacingDirectionConstraint
	};

	BubbleFunnelExtremityConstraint() : m_constraintType(UninitializedConstraint) {}

	KyResult Initialize(const Bubble& leftBubble, const Bubble& rightBubble, ConstraintType constraintType, KyUInt32 firstGateIndex, KyUInt32 lastGateIndex);
	void InitFromBlob(const BubbleFunnelExtremityConstraintBlob& blob);

	void Clear() { m_constraintType = UninitializedConstraint; }
	bool IsInitialized() const { return (m_constraintType != UninitializedConstraint); }

	const Bubble&  GetLeftBubble()        const { return m_leftBubble; }
	const Bubble&  GetRightBubble()       const { return m_rightBubble; }
	const Vec3f&   GetTangencyPoint()     const { return m_tangencyPositionBubble.GetCenter(); }
	const Vec3f&   GetTangencyDircetion() const { return m_tangencyDirection; }
	ConstraintType GetConstraintType()    const { return m_constraintType; }
	KyUInt32       GetFirstGateIndex()    const { return m_firstGateIndex; }
	KyUInt32       GetLastGateIndex()     const { return m_lastGateIndex; }

	/// Returns the Bubble on which the bi-tangent segment to arrivalBubble will lay on.
	const Bubble& GetConstraintBubble(const Bubble& arrivalBubble) const;

	/// Returns the Bubble on which the bi-tangent segment will lay on when going in the provided direction.
	const Bubble& GetConstraintBubble(const Vec2f& direction) const;

	/// Calls GetConstraintBubble to retrieve the constraining bubble and computes the bi-tangent segment.
	KyResult ComputeBiTangent(const Bubble& arrivalBubble, Vec3f& biTangentStart, Vec3f& biTangentEnd) const;

public: // internal
	Bubble m_leftBubble;
	Bubble m_rightBubble;
	Bubble m_tangencyPositionBubble;
	Vec3f m_tangencyDirection;
	ConstraintType m_constraintType;
	KyUInt32 m_firstGateIndex;
	KyUInt32 m_lastGateIndex;
};

} // namespace Kaim

#endif // Navigation_BubbleFunnelExtremityConstraint_H
