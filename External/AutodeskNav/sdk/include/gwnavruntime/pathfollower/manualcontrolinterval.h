/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_ManualControlInterval_H
#define Navigation_ManualControlInterval_H

#include "gwnavruntime/path/positiononlivepath.h"

namespace Kaim
{


enum ManualControlStatus
{
	ManualControlStatus_NotInitialized,  ///< not initialized
	ManualControlStatus_PositionInvalid, ///< binded to a bot, but PositionPath is invalid
	ManualControlStatus_EventInvalid,    ///< PositionOnPath is valid, but event information in PositionOnLivePath are not valid
	ManualControlStatus_Pending,         ///< PositionOnLivePath needs validation, PositionOnPath and event information can change!
	ManualControlStatus_Valid,           ///< PositionOnLivePath is valid
};

// Beware that ManualControlInterval should not be maintain over several frames
// due to PositionOnLivePath members whose validity must be maintained by the Bot
class ManualControlInterval
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Bot)

public:
	ManualControlInterval()	{ Clear(); }
	
	// Find the ManualControl entry and exit points based on the given NavTag
	ManualControlStatus Compute(const NavTag* navTag, const PositionOnLivePath& positionOnLivePath);
	ManualControlStatus Compute(const DynamicNavTag* navTag, const PositionOnLivePath& positionOnLivePath);

private:
	ManualControlStatus GetStatus(const PositionOnLivePath& positionOnLivePath) const;

	// Move to NavTag Entry
	// if already on the NavTag it will give the entry of the _current_ EventInterval that is backward from positionOnLivePath!
	// if not on the NavTag it will give the entry of the _next_ EventInterval that is forward from positionOnLivePath!
	template<class TNavTag> void MovePositionOnPathToCurrentOrNextNavTagEntry(const TNavTag* navTag, PositionOnLivePath& positionOnLivePath);

	// Move to NavTag Exit
	// if already on the NavTag it will give the exit of the _current_ EventInterval that is forward from positionOnLivePath!
	// if not on the NavTag it will give the exit of the _previous_ EventInterval that is backward from positionOnLivePath!
	template<class TNavTag> void MovePositionOnPathToCurrentOrPreviousNavTagExit(const TNavTag* navTag, PositionOnLivePath& positionOnLivePath);

	// Find the ManualControl entry and exit points based on the given NavTag
	// if m_progressOnLivePath already on the NavTag, it gives the previous entry and current exit
	// if m_progressOnLivePath not on the NavTag, it gives the next entry and next exit
	// Beware that ManualControlInterval should not be kept over several frames.
	template<class TNavTag> ManualControlStatus GetCurrentOrNextManualControlInterval(const TNavTag* navTag, const PositionOnLivePath& positionOnLivePath);

public: // internal

	ManualControlStatus GetEnterStatus() const { return GetStatus(m_enterPos); }
	ManualControlStatus GetExitStatus() const;

	// Be sure to check GetEnterStatus() and GetExitStatus() first
	const PositionOnLivePath& GetEnterPositionOnLivePath() const { return m_enterPos; }
	const PositionOnLivePath& GetExitPositionOnLivePath() const { return m_exitPos; }
	const PositionOnPath& GetEnterPositionOnPath() const { return m_enterPos.GetPositionOnPath(); }
	const PositionOnPath& GetExitPositionOnPath() const { return m_exitPos.GetPositionOnPath(); }
	const Vec3f& GetEnterPosition() const { return m_enterPos.GetPosition(); }
	const Vec3f& GetExitPosition() const { return m_exitPos.GetPosition(); }

	// Called at Compute
	void BindToBot(Bot* bot)
	{
		m_enterPos.BindToBot(bot);
		m_exitPos.BindToBot(bot);
		Clear();
	}

	void Clear()
	{
		m_enterPos.Clear();
		m_exitPos.Clear();
	}

	void FindLocationOnEventListFromScratch()
	{
		ManualControlStatus enterStatus = GetEnterStatus();
		if (enterStatus <= ManualControlStatus_PositionInvalid) // in that case it is also true for exitStatus
			return;

		m_enterPos.FindLocationOnEventListFromScratch();

		ManualControlStatus exitStatus = GetExitStatus();
		if (exitStatus <= ManualControlStatus_PositionInvalid)
			return;

		m_exitPos.FindLocationOnEventListFromScratch();
	}

	// Should be called only if ExitPosition was pending by the PathProgressComputer
	void UpdatePendingExitPosition();

	// event information are invalidated, but PositionOnPath are maintained
	void ClearEventListLocation()
	{
		m_enterPos.ClearEventListLocation();
		m_exitPos.ClearEventListLocation();
	}

public: // internal
	PositionOnLivePath m_enterPos;
	PositionOnLivePath m_exitPos;
};

}

#endif
