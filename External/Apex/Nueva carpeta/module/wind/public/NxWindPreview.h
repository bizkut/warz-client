// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2012 NVIDIA Corporation. All rights reserved.
#ifndef NX_WIND_PREVIEW_H
#define NX_WIND_PREVIEW_H

#include "NxApex.h"
#include "NxApexAssetPreview.h"

namespace physx
{
namespace apex
{

PX_PUSH_PACK_DEFAULT

class NxApexRenderDebug;

/**
	\brief The APEX_WIND namespace contains the defines for setting the preview detail levels.

	WIND_DRAW_NOTHING - draw nothing<BR>
	WIND_DRAW_ICON - draw the APEX Wind Icon<BR>
	WIND_DRAW_WIND_BOX - draw the wind box<BR>
	WIND_DRAW_GRIDS - draw the top and bottom grids of the wind box<BR>
	WIND_DRAW_COMPASS - draw the wind compass<BR>
	WIND_DRAW_WITH_CYLINDERS - draw the icon and the compass using cylinders so they appear BOLD<BR>
	WIND_DRAW_ASSET_INFO - draw the wind asset info in the screen space<BR>
	WIND_DRAW_FULL_DETAIL - draw all components of the wind preview<BR>
	WIND_DRAW_FULL_DETAIL_BOLD - draw all compoents of the wind preview with the icon and compas in BOLD<BR>
*/
namespace APEX_WIND
{
/**
	\def WIND_DRAW_NOTHING
	Draw no wind preview items.
*/
static const physx::PxU32 WIND_DRAW_NOTHING = 0x00;
/**
	\def WIND_DRAW_ICON
	Draw the wind icon.
*/
static const physx::PxU32 WIND_DRAW_ICON = 0x01;
/**
	\def WIND_DRAW_WIND_BOX
	Draw the wind box.  The top of the wind box should be above ground and the bottom should be below ground.
*/
static const physx::PxU32 WIND_DRAW_WIND_BOX = 0x02;
/**
	\def WIND_DRAW_GRIDS
	Draw the wind box grids.  These help to see if the entire top of the wind box is above ground and the
	bottom of the box is below ground.
*/
static const physx::PxU32 WIND_DRAW_GRIDS = 0x04;
/**
	\def WIND_DRAW_COMPASS
	Draw the compass rose and the wind heading arrows.
*/
static const physx::PxU32 WIND_DRAW_COMPASS = 0x08;
/**
	\def WIND_DRAW_WITH_CYLINDERS
	Draw the compass rose text and the icon using cylinders instead of lines.  This modifies DRAW_ICON and DRAW_COMPASS.
*/
static const physx::PxU32 WIND_DRAW_WITH_CYLINDERS = 0x10;
/**
	\def WIND_DRAW_ASSET_INFO
	Draw the Asset Info in Screen Space.  This displays the various asset members that are relevant to the current asset.
	parameters that are not relevant because they are disabled are not displayed.
*/
static const physx::PxU32 WIND_DRAW_ASSET_INFO = 0x20;
/**
	\def WIND_DRAW_ASSET_INFO
	Draw all of the preview rendering items.
*/
static const physx::PxU32 WIND_DRAW_FULL_DETAIL = (WIND_DRAW_ICON + WIND_DRAW_WIND_BOX + WIND_DRAW_GRIDS + WIND_DRAW_COMPASS + WIND_DRAW_ASSET_INFO);
/**
	\def WIND_DRAW_FULL_DETAIL_BOLD
	Draw all of the preview rendering items using cylinders instead of lines to make the text and icon look BOLD.
*/
static const physx::PxU32 WIND_DRAW_FULL_DETAIL_BOLD = (WIND_DRAW_FULL_DETAIL + WIND_DRAW_WITH_CYLINDERS);
}

/**
\brief This class provides the asset preview for APEX Wind Assets.  The class provides multiple levels of prevew
detail that can be selected individually.
*/
class NxWindPreview : public NxApexAssetPreview
{
public:
	/**
	\brief Set the half length dimensions of the actor thus resizing the wind box.
	*/
	virtual void	setHalfLengthDimensions(physx::PxVec3 halfLengthDimensions) = 0;
	/**
	\brief Set the spacing between the lines in the wind box grid.
	*/
	virtual void	setGridSpacing(physx::PxF32 gridSpacing) = 0;
	/**
	Set the scale of the Compass Rose and the wind direction arrows.<BR>
	The unscaled compass has a diameter (size) of 1.0 game units.<BR>
	By default the scale of the Compass Rose is 1.0. (unscaled)
	*/
	virtual void	setCompassScale(physx::PxF32 scale) = 0;
	/**
	Set the scale of the icon.<BR>
	The unscaled icon has is 1.0x1.0 game units.<BR>
	By default the scale of the icon is 1.0. (unscaled)<BR>
	*/
	virtual void	setIconScale(physx::PxF32 scale) = 0;
	/**
	Set the detail level of the preview rendering by selecting which features to enable.<BR>
	Any, all, or none of the following masks may be added together to select what should be drawn.<BR>
	The defines for the individual items are:<br>
		WIND_DRAW_NOTHING - draw nothing<BR>
		WIND_DRAW_ICON - draw the APEX Wind Icon<BR>
		WIND_DRAW_WIND_BOX - draw the wind box<BR>
		WIND_DRAW_GRIDS - draw the top and bottom grids of the wind box<BR>
		WIND_DRAW_COMPASS - draw the wind compass<BR>
		WIND_DRAW_WITH_CYLINDERS - draw the icon and the compass using cylinders so they appear BOLD<BR>
		WIND_DRAW_ASSET_INFO - draw the wind asset info in the screen space<BR>
		WIND_DRAW_FULL_DETAIL - draw all components of the wind preview<BR>
		WIND_DRAW_FULL_DETAIL_BOLD - draw all compoents of the wind preview with the icon and compas in BOLD<BR>
	All items may be drawn using the macro DRAW_FULL_DETAIL.
	*/
	virtual void	setDetailLevel(physx::PxU32 detail) = 0;
};


PX_POP_PACK

}
} // namespace physx::apex

#endif // NX_WIND_PREVIEW_H
