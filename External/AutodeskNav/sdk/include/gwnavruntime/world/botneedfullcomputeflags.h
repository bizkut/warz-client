/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: GUAL - secondary contact: NONE
#ifndef Navigation_BotNeedFullComputeFlags_H
#define Navigation_BotNeedFullComputeFlags_H

#include "gwnavruntime/base/memory.h"


namespace Kaim
{


class BotNeedFullComputeFlags
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Bot)

public:
	// ---------------------------------- Public Member Functions ----------------------------------

	BotNeedFullComputeFlags() { SetDefaults(); }

	/// Sets all members to their default value.
	void SetDefaults()
	{
		m_pathEventListNeedFullCompute      = true;
		m_colliderCollectionNeedFullCompute = true;
		m_trajectoryNeedFullCompute         = true;
		m_channelNeedFullCompute            = false;
	}

	bool GetPathEventListNeedFullCompute()      const { return m_pathEventListNeedFullCompute;      }
	bool GetColliderCollectionNeedFullCompute() const { return m_colliderCollectionNeedFullCompute; }
	bool GetTrajectoryNeedFullCompute()         const { return m_trajectoryNeedFullCompute;         }
	bool GetChannelNeedFullCompute()            const { return m_channelNeedFullCompute;            }

	void PathHasChanged()
	{
		m_channelNeedFullCompute = false;
		m_pathEventListNeedFullCompute      = true;
		m_trajectoryNeedFullCompute         = true;
	}

	void ChannelHasBeenInvalidated()
	{
		m_channelNeedFullCompute = true;
	}

	void PathEventListStartedFullCompute()
	{
		m_pathEventListNeedFullCompute = false; // ok PathEventList compute started
	}

	void PathEventListHasChanged()
	{
		m_trajectoryNeedFullCompute = true; // trajectory need to be recomputed
	}

	void TrajectoryHasBeenFullyRecomputed()
	{
		m_trajectoryNeedFullCompute = false;
	}

	void ColliderCollectionHasBeenFullyRecomputed()
	{
		m_colliderCollectionNeedFullCompute = false;
	}

	void EnableAvoidanceHasChanged()
	{
		// do nothing, avoidance is computed every frame
	}

	void PathEventListConfigHasChanged()
	{
		m_pathEventListNeedFullCompute = true;
		m_trajectoryNeedFullCompute    = true;
	}

	void ShortcutTrajectoryConfigHasChanged()
	{
		m_trajectoryNeedFullCompute = true;
	}

	void ChannelTrajectoryConfigHasChanged()
	{
		m_trajectoryNeedFullCompute = true;
	}

	void TrajectoryModeHasChanged()
	{
		m_trajectoryNeedFullCompute = true;
	}

	void MaxDesiredLinearSpeedHasChanged()
	{
		m_trajectoryNeedFullCompute = true;
	}

	void ColliderCollectorConfigHasChanged()
	{
		m_colliderCollectionNeedFullCompute = true;
		m_trajectoryNeedFullCompute         = true;
	}

	void AvoidanceConfigHasChanged()
	{
		// do nothing, avoidance is computed every frame
	}

	void DoComputeTrajectoryHasBecomeTrue()
	{
		m_trajectoryNeedFullCompute = true;
	}

public:
	// ---------------------------------- Public Data Members ----------------------------------
	bool m_pathEventListNeedFullCompute;
	bool m_colliderCollectionNeedFullCompute;
	bool m_trajectoryNeedFullCompute;
	bool m_channelNeedFullCompute;
};


} // namespace Kaim

#endif // Navigation_BotNeedFullComputeFlags_H
