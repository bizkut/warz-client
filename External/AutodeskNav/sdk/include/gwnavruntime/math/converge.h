/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_Converge_H
#define Navigation_Converge_H


#include "gwnavruntime/math/vec2f.h"


namespace Kaim
{

/// This class represents a three-dimensional 6 freedom degrees unit transform.
class Converge
{
public:
	/// Returns a float that is as close as possible to 'desired' but respecting |returned_value - current| < maxDelta
	///                   desired-maxDelta   desired    desired+maxDelta
	/// -------------------------[--------------|--------------]---------------------------
	///  output=current+maxDelta          output=desired           output=desired-maxDelta
	static KyFloat32 ConvergeToFloat32(KyFloat32 desired, KyFloat32 maxDelta, KyFloat32 current);

	/*             maxDeltaAngle    */
	/*            +                 */
	/*           /                  */
	/*          /                   */
	/*  +------+------+             */
	/*          \                   */
	/*           \                  */
	/*            +                 */
	/// Returns a 2D direction that is as close as possible to 'desiredDir2D' but respecting |angle(returned_dir2D, currentDir2D) | < maxDeltaAngleInRadian
	static Vec2f ConvergeToDir2d(const Vec2f& desiredDir2D, KyFloat32 maxDeltaAngleInRadian, const Vec2f& currentDir2D);
};


}

#endif // Navigation_Converge_H
