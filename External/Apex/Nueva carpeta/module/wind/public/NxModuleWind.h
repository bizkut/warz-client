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

#ifndef NX_MODULE_WIND_H
#define NX_MODULE_WIND_H

#include "NxApex.h"

namespace physx
{
namespace apex
{

PX_PUSH_PACK_DEFAULT

/*! \mainpage APEX Wind Module Description
 * \image html WindIconNoBorder.JPG
 *
 * \section intro_sec Introduction
 * APEX Wind simulates the various components of wind using force fields.
 * These include the prevailing wind, gusts, and the drag and flutter force fields.
 * Drag and Flutter while not strictly part of wind normally allow objects such as
 * leaves and paper to behave in a manner consistent with light falling objects that flutter
 * as they fall.<br>
 *
 * \section compat_sec PhysX SDK compatibility
 * APEX Wind is compatible with PhysX SDK version 2.8.4 only.<br>
 *
 * \section dependencies_sec APEX Module Dependencies
 * APEX Wind Assets can optionally specify one or more APEX Field Boundary (Force Field module) assets that are
 * attached as exclude groups in every force field created by APEX wind.  If a Field Boundary Asset is specified
 * in an APEX Wind Asset the Force Field Module (which creates the Field Boundary actors) is force loaded.
 * Field Boundary actors that are created at module load time as well as those that are dynimacally created are
 * added to each force field when they are created and are removed when they are released.<br>
 *
 * \section physX_dependencies_sec PhysX Dependencies
 * For an APEX Wind force field to interact with a PhysX actor such as various types of particles, rigid bodies and
 * cloth the actors and the force field have to be in the same PhysX collision group.  APEX modules set their
 * collision group symbolically.  The actual collision group used is looked up in the Named Resource Provider (NRP).
 * For APEX Wind and PhysX particles to interact in the desired manner manner (I.E. look "real") many particle
 * parameters must be set proportionally to the APEX Wind forces such as the particle mass, the coefficient of
 * static friction (stiction) and particle parameters such as the Motion Limit Multiplier.  If these parameters
 * are not balanced the wind could be blowing but nothing could move or at the other extreme the wind could blow
 * and every actor is blown far away in a single frame.  Due to the time required to tune these parameters it is
 * recommended that assets be derived from working samples and then tuned to achieve the desired behavior.<br>
 *
 */

/**
\brief The APEX Wind Module class.  This class provides the ability to create NxWindAsset instances.

This class provides the ability to create NxWindAsset instances.
*/
class NxModuleWind : public NxModule
{
public:
	/**
	\brief The module ID value of the wind.
	*/
	virtual physx::PxU32					getModuleValue() const = 0;

protected:
	virtual ~NxModuleWind() {}
};

#if !defined(_USRDLL)
/* If this module is distributed as a static library, the user must call this
 * function before calling NxApexSDK::createModule("Wind")
 */
void instantiateModuleWind();
#endif

PX_POP_PACK

}
} // namespace physx::apex

#endif // NX_MODULE_WIND_H
