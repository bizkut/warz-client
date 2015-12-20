/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: GUAL - secondary contact: JUBA
#ifndef Navigation_PathProgressConfig_H
#define Navigation_PathProgressConfig_H

#include "gwnavruntime/base/memory.h"

namespace Kaim
{

/// Class that aggregates parameters that allow to configure the progress of a Bot on its path.
class PathProgressConfig
	{
public:
	PathProgressConfig() {  SetDefaults(); }

	void SetDefaults()
	{
		m_pathValidationDistanceBackward = 30.0f;
		m_pathValidationDistanceForward  = 50.0f;
		m_checkPointRadius               = 0.2f;
		m_navTagDiscoveryDistance        = 3.0f;
	}

public:
	/// The distance the Path is checked backward each time a new Path is
	/// computed. The Path is never checked backward farther than this distance
	/// from the current target on path.
	KyFloat32 m_pathValidationDistanceBackward;

	/// The distance the Path is checked forward each time a new Path is
	/// computed. The Path validation is then done each frame progressing for
	/// this distance until the whole Path is validated or the validation
	/// fails somewhere forward.
	KyFloat32 m_pathValidationDistanceForward;

	/// The radius around check points that trigger their validation.
	KyFloat32 m_checkPointRadius;

	/// Distance along the path from the ProgressOnPath used to discover and list all the navtags along the way.
	KyFloat32 m_navTagDiscoveryDistance;
};

inline void SwapEndianness(Endianness::Target e, PathProgressConfig& self)
{
	SwapEndianness(e, self.m_pathValidationDistanceBackward);
	SwapEndianness(e, self.m_pathValidationDistanceForward);
	SwapEndianness(e, self.m_checkPointRadius);
	SwapEndianness(e, self.m_navTagDiscoveryDistance);
}

} // namespace Kaim

#endif // Navigation_PathProgressConfig_H
