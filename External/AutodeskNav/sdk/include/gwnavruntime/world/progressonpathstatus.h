/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_ProgressOnPathStatus_H
#define Navigation_ProgressOnPathStatus_H

namespace Kaim
{

/// Enumerates the possible status values of Bot Position on path.
enum ProgressOnPathStatus
{
	/// Progress on path has not been yet initialized at all. Main reason is that
	/// no path has been set yet.
	ProgressOnPathNotInitialized = 0,

	/// Progress on path is initialized but has not been computed yet. This namely occurs when
	/// a new path has been set, the Progress on path is at Path start.
	ProgressOnPathPending,

	/// Progress on path was on a valid NavData that has been streamed out (due to
	/// explicit streaming or DynamicNavMesh) and no valid NavData has been
	/// found there.
	ProgressOnPathInInvalidNavData,

	ProgressOnPathOutsideNavMesh,

	ProgressOnPathOnNavMesh,
};

}

#endif
