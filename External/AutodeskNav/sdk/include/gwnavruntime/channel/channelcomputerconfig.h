/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_ChannelComputerConfig_H
#define Navigation_ChannelComputerConfig_H


#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/base/endianness.h"


namespace Kaim
{

/// Class that aggregates parameters that allow to configure the channel computation of a Bot.
class ChannelComputerConfig
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Bot)

public:
	ChannelComputerConfig() { SetDefaults(); }

	void SetDefaults()
	{
		m_idealClearanceRadius     = 5.0f;
		m_minTurnAngleDeg          = 15.0f;
		m_turnSamplingAngleDeg     = 45.0f; // 1/8 round
		m_channelSmoothingAngleDeg = 30.0f;
		m_channelIntermediaryGateCollapseDistance = 0.01f;
	}

	bool operator==(const ChannelComputerConfig& other) const
	{
		return m_idealClearanceRadius     == other.m_idealClearanceRadius
			&& m_minTurnAngleDeg          == other.m_minTurnAngleDeg
			&& m_turnSamplingAngleDeg     == other.m_turnSamplingAngleDeg
			&& m_channelSmoothingAngleDeg == other.m_channelSmoothingAngleDeg
			&& m_channelIntermediaryGateCollapseDistance == other.m_channelIntermediaryGateCollapseDistance;
	}

	bool operator!=(const ChannelComputerConfig& other) const
	{
		return !operator==(other);
	}

public:
	/// The ideal distance between the path and the Channel borders. In meters.
	/// \defaultvalue 5.0f
	KyFloat32 m_idealClearanceRadius;

	/// Turns bellow this value will be treated as single Gate. In degrees.
	/// \defaultvalue 15.0f
	KyFloat32 m_minTurnAngleDeg;

	/// Turns over this value will be split into equal fans. In degrees.
	/// \defaultvalue 45.0f
	KyFloat32 m_turnSamplingAngleDeg; 

	/// The Channel maximal angle, taken with path edge direction as reference,
	/// when entering or quiting open space. Range: [1, 89]. In degrees.
	/// \defaultvalue 30.0f
	KyFloat32 m_channelSmoothingAngleDeg;

	/// Technical.
	/// This parameter has to be customized with lot of precaution. It defines
	/// the distance below which close border points will be collapsed when
	/// computing intermediary gates. It must stay small.
	/// \defaultvalue 0.01f
	KyFloat32 m_channelIntermediaryGateCollapseDistance;
};

inline void SwapEndianness(Endianness::Target e, ChannelComputerConfig& self)
{
	SwapEndianness(e, self.m_idealClearanceRadius);
	SwapEndianness(e, self.m_minTurnAngleDeg);
	SwapEndianness(e, self.m_turnSamplingAngleDeg);
	SwapEndianness(e, self.m_channelSmoothingAngleDeg);
	SwapEndianness(e, self.m_channelIntermediaryGateCollapseDistance);
}


} // namespace Kaim

#endif // Navigation_ChannelComputerConfig_H
