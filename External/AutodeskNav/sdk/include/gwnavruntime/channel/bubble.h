/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_Bubble_H
#define Navigation_Bubble_H


#include "gwnavruntime/math/vec3f.h"


namespace Kaim
{

class ScopedDisplayList;
class BubbleBlob;


/// Defines the 4 possible cases of possibly constrained rotation in the horizontal plane for a given element.
enum RotationDirection
{
	UninitializedRotationDirection, // Default RotationDirection value when not initialized.
	UndefinedRotationDirection,     // The element has no rotation constraints and can turn in both direction.
	Clockwise,                      // The element can turn only clockwise.
	CounterClockwise,               // The element can turn only counter clockwise.
	NoRotation                      // The element is fixed and could not turn.
};


/// This class represents a circle with potential rotation limitation.
class Bubble
{
public:
	Bubble()
		: m_rotationDirection(UninitializedRotationDirection)
		, m_userData(KY_NULL)
	{}

	Bubble(const Vec3f& center, KyFloat32 radius, RotationDirection rotationDirection)
		: m_center(center)
		, m_radius(radius)
		, m_rotationDirection(rotationDirection)
		, m_userData(KY_NULL)
		, m_referencePosition(center)
	{}

	void Reset()
	{
		m_center = Vec3f::Zero();
		m_radius = 0.0f;
		m_rotationDirection = UninitializedRotationDirection;
		m_userData = KY_NULL;
	}

	void Set(const Vec3f& center, KyFloat32 radius, RotationDirection rotationDirection)
	{
		SetCenter(center);
		SetRadius(radius);
		SetRotationDirection(rotationDirection);
		SetReferencePosition(center);
	}

	void InitFromBlob(const BubbleBlob& blob);

	void SetUserData(void* userData) { m_userData = userData; }

	void SetCenter(const Vec3f& center)                            { m_center = center; }
	void SetRadius(KyFloat32 radius)                               { m_radius = radius; }
	void SetRotationDirection(RotationDirection rotationDirection) { m_rotationDirection = rotationDirection; }

	void SetCenterAltitude(KyFloat32 newCenterAltitude)            { m_center.z = newCenterAltitude; }
	void SetReferencePosition(const Vec3f& referencePosition)      { m_referencePosition = referencePosition; }

	void*             GetUserData()          const { return m_userData;          }
	const Vec3f&      GetCenter()            const { return m_center;            }
	KyFloat32         GetRadius()            const { return m_radius;            }
	RotationDirection GetRotationDirection() const { return m_rotationDirection; }
	const Vec3f&      GetReferencePosition() const { return m_referencePosition; }

	bool              IsInitialized()        const { return (m_rotationDirection != UninitializedRotationDirection); }

	/// Computes the segment starting from this Bubble and arriving on \arrivalBubble, tangent to
	/// both Bubbles and respecting Bubbles rotation directions.
	KyResult ComputeBiTangent(const Bubble& arrivalBubble, Vec2f& biTangentStart, Vec2f& biTangentEnd) const;

	/// Computes the segment starting from this Bubble and arriving on \arrivalBubble, tangent to
	/// both Bubbles and respecting Bubbles rotation directions. Bi-tangent segment extremities altitude
	/// is set to respective Bubble center altitudes.
	KyResult ComputeBiTangent(const Bubble& arrivalBubble, Vec3f& biTangentStart, Vec3f& biTangentEnd) const;

	/// Computes the Bubble tangent to this Bubble at the provided position and tangent to arrivalBubble.
	KyResult ComputeBiTangentBubble(const Vec3f& tangencyPosition, const Bubble& secondBubble, Vec3f& tangencyPositionOnSecondBubble, Bubble& biTangentBubble) const;

	/// Computes the Bubble tangent to this Bubble at the provided tangencyPosition and passing through an arbitrary position.
	KyResult ComputeTangentBubble(const Vec3f& tangencyPosition, const Vec3f& position, Bubble& tangentBubble) const;

	void Display(ScopedDisplayList& displayList) const;

	bool operator==(const Bubble& other) const
	{
		return (
			(m_center            == other.m_center           ) &&
			(m_radius            == other.m_radius           ) &&
			(m_rotationDirection == other.m_rotationDirection)
			);
	}

private:
	Vec3f m_center;
	KyFloat32 m_radius;
	RotationDirection m_rotationDirection;
	void* m_userData;

	// Reference position is used to detect BubbleFunnelIgnoreRadius closures when
	// string-pulling. It is initialized at Bubble center, which is fine for Bubbles
	// representing a real obstacle (corner, obstacle...). But, when dealing with
	// abstract Bubbles such as departure or approach constraining Bubbles, the center
	// may be completely decorrelated from BubbleArray strip, leading to 2D closure
	// errors. In such cases, this reference position should be overridden to a
	// position with concrete meaning (e.g. actual start position for departure
	// constraining Bubbles).
	Vec3f m_referencePosition;
};


template <class OSTREAM>
inline OSTREAM& operator<<(OSTREAM& os, const Bubble& bubble)
{
	os << "center=" << bubble.GetCenter() << "refPos=" << bubble.GetReferencePosition() << " radius=" << bubble.GetRadius() << " direction=";

	switch (bubble.GetRotationDirection())
	{
	case UninitializedRotationDirection :    os << "!!!  Not initialized  !!! ";   break;
	case UndefinedRotationDirection     :    os << "UndefinedRotationDirection";   break;
	case Clockwise                      :    os << "Clockwise                 ";   break;
	case CounterClockwise               :    os << "CounterClockwise          ";   break;
	case NoRotation                     :    os << "NoRotation                ";   break;

	default:
		break;
	}

	return os;
}


} // namespace Kaim

#endif // Navigation_Bubble_H
