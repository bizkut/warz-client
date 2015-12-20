/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: MUBI
#ifndef Navigation_SplineConfig_H
#define Navigation_SplineConfig_H

#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/base/endianness.h"


namespace Kaim
{

/// Class that configures Spline geometric parameters.
class SplineConfig
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Bot)

public:
	SplineConfig() { SetDefaults(); }

	void SetDefaults();
	bool operator==(const SplineConfig& other) const;
	bool operator!=(const SplineConfig& other) const { return !operator==(other); }


public:
	// -------------------- Spline geometry parameters --------------------

	/// Desired spline length. This is the distance looked ahead on the channel
	/// when computing the spline. To get optimal results when updating the spline
	/// this is highly recommended to set a value greater than the longest straight
	/// line the bot should encounter on its path to avoid adding artificial turns
	/// due to some kind of myopia.
	/// \defaultvalue 100.0f.
	KyFloat32 m_length;

	/// Maximal angle to be ran around a CircleArc. Must be strictly less than
	/// twice pi (we don't manage spires, our turns are in the plane).
	/// \defaultvalue 4.18f (~ 1.33*pi = 240 degrees = 2/3 of full turn)
	KyFloat32 m_maxCircleArcAngleRad;


	// -------------------- Ideal distances to Channel borders --------------------

	/// Spline distance to Channel borders. If there is not enough room to reach these
	/// distances, for instance through a door, the spline will stay at the center of
	/// the channel. You can set different values to induce natural preference for the
	/// left or the right side of the Channel.

	/// Distance to left borders.
	/// \defaultvalue 0.2f.
	KyFloat32 m_distanceToLeftBorders;

	/// Distance to right borders.
	/// \defaultvalue 0.2f.
	KyFloat32 m_distanceToRightBorders;


public: // internal
	// Computed by CircleArcSplineComputer when starting the computation to call only once cosf(m_maxCircleArcAngleRad).
	KyFloat32 m_cosineOfMaxCircleArcAngle;
};

inline void SwapEndianness(Endianness::Target e, SplineConfig& self)
{
	SwapEndianness(e, self.m_length);
	SwapEndianness(e, self.m_maxCircleArcAngleRad);
	SwapEndianness(e, self.m_distanceToLeftBorders);
	SwapEndianness(e, self.m_distanceToRightBorders);

	SwapEndianness(e, self.m_cosineOfMaxCircleArcAngle);
}

} // namespace Kaim

#endif // Navigation_SplineConfig_H
