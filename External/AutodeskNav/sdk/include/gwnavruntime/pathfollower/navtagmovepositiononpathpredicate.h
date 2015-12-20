/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_NavTagMovePositionOnPathPredicate_H
#define Navigation_NavTagMovePositionOnPathPredicate_H

#include "gwnavruntime/pathfollower/imovepositiononpathpredicate.h"

namespace Kaim
{

class NavTag;
class DynamicNavTag;

/// Default implementation of IMovePositionOnPathPredicate interface based on NavTag.
class NavTagMovePositionOnPathPredicate : public IMovePositionOnPathPredicate
{
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	NavTagMovePositionOnPathPredicate(const NavTag* navTag) : m_navTag(navTag) {}

	/// \returns true if positionOnPath is on or at start of a PathEventInterval
	/// with the same NavTag value than #m_navTag.
	virtual bool CanMoveForward(Bot* bot, PositionOnLivePath& positionOnLivePath);
	virtual bool CanMoveBackward(Bot* bot, PositionOnLivePath& positionOnLivePath);


	// ---------------------------------- Public Data Members ----------------------------------

	/// The NavTag the PositionOnPath is allowed to move on.
	const NavTag* m_navTag;
};


/// This predicates allows Move forward to next entry or backward to previous entry of the given NavTag.
/// When moving forward, if already on a NavTag, it will move forward to the next entry of the same NavTag
/// TNavTag must either be NavTag or DynamicNavTag, this class is explicitly instantiated in .cpp file
template<class TNavTag>
class MoveToNavTagEntryPositionOnPathPredicate : public IMovePositionOnPathPredicate
{
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	MoveToNavTagEntryPositionOnPathPredicate(const TNavTag* navTag) : m_navTag(navTag) {}

	virtual bool CanMoveForward(Bot* bot, PositionOnLivePath& positionOnLivePath);

	virtual bool CanMoveBackward(Bot* bot, PositionOnLivePath& positionOnLivePath);

	bool IsAtNavTagEntry(Bot* bot, PositionOnLivePath& positionOnLivePath);

	// ---------------------------------- Public Data Members ----------------------------------

	/// The NavTag the PositionOnPath is allowed to move on.
	const TNavTag* m_navTag;
};

/// This predicates allows Move forward to next entry or backward to previous entry of the given NavTag.
/// When moving forward, if already on a NavTag, it will move forward to the next entry of the same NavTag
/// TNavTag must either be NavTag or DynamicNavTag, this class is explicitly instantiated in .cpp file
template<class TNavTag>
class MoveToNavTagExitPositionOnPathPredicate : public IMovePositionOnPathPredicate
{
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	MoveToNavTagExitPositionOnPathPredicate(const TNavTag* navTag) : m_navTag(navTag) {}

	virtual bool CanMoveForward(Bot* bot, PositionOnLivePath& positionOnLivePath);
	virtual bool CanMoveBackward(Bot* bot, PositionOnLivePath& positionOnLivePath);

	bool IsAtNavTagExit(Bot* bot, PositionOnLivePath& positionOnLivePath);

	// ---------------------------------- Public Data Members ----------------------------------

	/// The NavTag the PositionOnPath is allowed to move on.
	const TNavTag* m_navTag;
};

} // namespace Kaim

#endif // Navigation_NavTagMovePositionOnPathPredicate_H
