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

#ifndef NX_MODIFIER_H
#define NX_MODIFIER_H

#include "NxApex.h"
#include "NxCurve.h"
#include "NxIofxAsset.h"
#include "NxUserRenderSpriteBufferDesc.h"

#include "NxModifierDefs.h"

namespace physx
{
namespace apex
{

PX_PUSH_PACK_DEFAULT

inline physx::PxU32 ModifierUsageFromStage(ModifierStage stage)
{
	return 1 << stage;
}

class NiIosObjectData;

// Disable the unused arguments warning for this header.
#pragma warning( disable: 4100 )

/**
	NxModifier contains all of the data necessary to apply a single modifier type to a particle system.
	Generally this combines some physical transformation with parameters specified at authoring time
	to modify the look of the final effect.
*/
class NxModifier
{
public:
	/**
		getModifierType returns the enumerated type associated with this class.
	*/
	virtual ModifierTypeEnum getModifierType() const = 0;

	/**
		getModifierUsage returns the usage scenarios allowed for a particular modifier.
	*/
	virtual physx::PxU32 getModifierUsage() const = 0;

	/**
		returns a bitmap that includes every sprite semantic that the modifier updates
	*/
	virtual physx::PxU32 getModifierSpriteSemantics()
	{
		return 0;
	}
	/**
		returns a bitmap that includes every mesh(instance) semantic that the modifier updates
	*/
	virtual physx::PxU32 getModifierMeshSemantics()
	{
		return 0;
	}


	virtual ~NxModifier() { }


};

/**
	NxModifierT is a helper class to handle the mapping of Type->Enum and Enum->Type.
	More importantly, this imposes some structure on the subclasses--they all now
	expect to have a const static field called ModifierType.
*/
template <typename T>
class NxModifierT : public NxModifier
{
public:
	virtual ModifierTypeEnum getModifierType() const
	{
		return T::ModifierType;
	}
	virtual physx::PxU32 getModifierUsage() const
	{
		return T::ModifierUsage;
	}
};


/**
	NxRotationModifier applies rotation to the particles using one of several rotation models.
*/
class NxRotationModifier : public NxModifierT<NxRotationModifier>
{
public:
	static const ModifierTypeEnum ModifierType = ModifierType_Rotation;
	static const physx::PxU32 ModifierUsage = ModifierUsage_Spawn | ModifierUsage_Continuous
	        | ModifierUsage_Mesh;


	/// get the roll model
	virtual NxApexMeshParticleRollType::Enum getRollType() const = 0;
	/// set the roll model
	virtual void setRollType(NxApexMeshParticleRollType::Enum rollType) = 0;
	/// get the maximum allowed settle rate per second
	virtual physx::PxF32 getMaxSettleRate() const = 0;
	/// set the maximum allowed settle rate per second
	virtual void setMaxSettleRate(physx::PxF32 settleRate) = 0;
	/// get the maximum allowed rotation rate per second
	virtual physx::PxF32 getMaxRotationRate() const = 0;
	/// set the maximum allowed rotation rate per second
	virtual void setMaxRotationRate(physx::PxF32 rotationRate) = 0;

};

/**
	NxSimpleScaleModifier just applies a simple scale factor to each of the X, Y and Z aspects of the model. Each scalefactor can be
	applied independently.
*/
class NxSimpleScaleModifier : public NxModifierT<NxSimpleScaleModifier>
{
public:
	static const ModifierTypeEnum ModifierType = ModifierType_SimpleScale;
	static const physx::PxU32 ModifierUsage = ModifierUsage_Spawn | ModifierUsage_Continuous
	        | ModifierUsage_Sprite | ModifierUsage_Mesh;


	/// get the vector of scale factors along the three axes
	virtual physx::PxVec3 getScaleFactor() const = 0;
	/// set the vector of scale factors along the three axes
	virtual void setScaleFactor(const physx::PxVec3& s) = 0;
};

/**
	NxScaleByMassModifier scales by mass of the particle.
*/
class NxScaleByMassModifier : public NxModifierT<NxScaleByMassModifier>
{
public:
	static const ModifierTypeEnum ModifierType = ModifierType_ScaleByMass;
	static const physx::PxU32 ModifierUsage = ModifierUsage_Spawn | ModifierUsage_Continuous
	        | ModifierUsage_Sprite | ModifierUsage_Mesh;
};

/**
	NxRandomScaleModifier applies a random scale uniformly to all three dimensions. Currently, the
	scale is a uniform in the range specified.
*/
class NxRandomScaleModifier : public NxModifierT<NxRandomScaleModifier>
{
public:
	static const ModifierTypeEnum ModifierType = ModifierType_RandomScale;
	static const physx::PxU32 ModifierUsage = ModifierUsage_Spawn
	        | ModifierUsage_Sprite | ModifierUsage_Mesh;


	/// get the range of scale factors along the three axes
	virtual NxRange<physx::PxF32> getScaleFactor() const = 0;
	/// set the range of scale factors along the three axes
	virtual void setScaleFactor(const NxRange<physx::PxF32>& s) = 0;
};

/**
	NxColorVsLifeModifier modifies the color constants associated with a particle
	depending on the life remaining of the particle.
*/
class NxColorVsLifeModifier : public NxModifierT<NxColorVsLifeModifier>
{
public:
	static const ModifierTypeEnum ModifierType = ModifierType_ColorVsLife;
	static const physx::PxU32 ModifierUsage = ModifierUsage_Spawn | ModifierUsage_Continuous
	        | ModifierUsage_Sprite | ModifierUsage_Mesh;


	/// get the affected color channel
	virtual ColorChannel getColorChannel() const = 0;
	/// set the affected color channel
	virtual void setColorChannel(ColorChannel colorChannel) = 0;
	/// get the curve that sets the dependency between the lifetime and the color
	virtual const NxCurve* getFunction() const = 0;
	/// set the curve that sets the dependency between the lifetime and the color
	virtual void setFunction(const NxCurve* f) = 0;
};

/**
	NxColorVsDensityModifier modifies the color constants associated with a particle
	depending on the density of the particle.
*/
class NxColorVsDensityModifier : public NxModifierT<NxColorVsDensityModifier>
{
public:
	static const ModifierTypeEnum ModifierType = ModifierType_ColorVsDensity;
	static const physx::PxU32 ModifierUsage = ModifierUsage_Continuous
	        | ModifierUsage_Sprite | ModifierUsage_Mesh;


	/// get the affected color channel
	virtual ColorChannel getColorChannel() const = 0;
	/// set the affected color channel
	virtual void setColorChannel(ColorChannel colorChannel) = 0;
	/// get the curve that sets the dependency between the density and the color
	virtual const NxCurve* getFunction() const = 0;
	/// set the curve that sets the dependency between the density and the color
	virtual void setFunction(const NxCurve* f) = 0;
};

/**
	NxSubtextureVsLifeModifier is a modifier to adjust the subtexture id versus the life remaining of a particular particle.

	Interpretation of the subtexture id over time is up to the application.
*/
class NxSubtextureVsLifeModifier : public NxModifierT<NxSubtextureVsLifeModifier>
{
public:
	static const ModifierTypeEnum ModifierType = ModifierType_SubtextureVsLife;
	static const physx::PxU32 ModifierUsage = ModifierUsage_Continuous
	        | ModifierUsage_Sprite;


	/// get the curve that sets the dependency between the life remaining and the subtexture id
	virtual const NxCurve* getFunction() const = 0;
	/// set the curve that sets the dependency between the life remaining and the subtexture id
	virtual void setFunction(const NxCurve* f) = 0;
};

/**
	NxOrientAlongVelocity is a modifier to orient a mesh so that a particular axis coincides with the velocity vector.
*/
class NxOrientAlongVelocityModifier : public NxModifierT<NxOrientAlongVelocityModifier>
{
public:
	static const ModifierTypeEnum ModifierType = ModifierType_OrientAlongVelocity;
	static const physx::PxU32 ModifierUsage = ModifierUsage_Continuous
	        | ModifierUsage_Mesh;


	/// get the object-space vector that will coincide with the velocity vector
	virtual physx::PxVec3 getModelForward() const = 0;
	/// set the object-space vector that will coincide with the velocity vector
	virtual void setModelForward(const physx::PxVec3& s) = 0;
};


/**
	NxScaleAlongVelocityModifier is a modifier to apply a scale factor along the current velocity vector.

	Note that without applying an OrientAlongVelocity modifier first, the results for this will be 'odd.'
*/
class NxScaleAlongVelocityModifier : public NxModifierT<NxScaleAlongVelocityModifier>
{
public:
	static const ModifierTypeEnum ModifierType = ModifierType_ScaleAlongVelocity;
	static const physx::PxU32 ModifierUsage = ModifierUsage_Continuous
	        | ModifierUsage_Mesh;


	/// get the scale factor
	virtual physx::PxF32 getScaleFactor() const = 0;
	/// set the scale factor
	virtual void setScaleFactor(const physx::PxF32& s) = 0;
};

/**
	NxRandomSubtextureModifier generates a random subtexture ID and places it in the subTextureId field.
*/
class NxRandomSubtextureModifier : public NxModifierT<NxRandomSubtextureModifier>
{
public:
	static const ModifierTypeEnum ModifierType = ModifierType_RandomSubtexture;
	static const physx::PxU32 ModifierUsage = ModifierUsage_Spawn
	        | ModifierUsage_Sprite;


	///get the range for subtexture values
	virtual NxRange<physx::PxF32> getSubtextureRange() const = 0;
	///set the range for subtexture values
	virtual void setSubtextureRange(const NxRange<physx::PxF32>& s) = 0;
};

/**
	NxRandomRotationModifier will choose a random orientation for a sprite particle within the range as specified below.

	The values in the range are interpreted as radians. Please keep in mind that all the sprites are coplanar to the screen.
*/
class NxRandomRotationModifier : public NxModifierT<NxRandomRotationModifier>
{
public:
	static const ModifierTypeEnum ModifierType = ModifierType_RandomRotation;
	static const physx::PxU32 ModifierUsage = ModifierUsage_Spawn
	        | ModifierUsage_Sprite;


	///get the range of orientations, in radians.
	virtual NxRange<physx::PxF32> getRotationRange() const = 0;
	///set the range of orientations, in radians.
	virtual void setRotationRange(const NxRange<physx::PxF32>& s) = 0;
};

/**
	NxScaleVsLifeModifier applies a scale factor function against a single axis versus the life remaining.
*/
class NxScaleVsLifeModifier : public NxModifierT<NxScaleVsLifeModifier>
{
public:
	static const ModifierTypeEnum ModifierType = ModifierType_ScaleVsLife;
	static const physx::PxU32 ModifierUsage = ModifierUsage_Continuous
	        | ModifierUsage_Sprite | ModifierUsage_Mesh;


	/// get the axis along which the scale factor will be applied
	virtual ScaleAxis getScaleAxis() const = 0;
	/// set the axis along which the scale factor will be applied
	virtual void setScaleAxis(ScaleAxis a) = 0;
	/// get the the curve that defines the dependency between the life remaining and the scale factor
	virtual const NxCurve* getFunction() const = 0;
	/// set the the curve that defines the dependency between the life remaining and the scale factor
	virtual void setFunction(const NxCurve* f) = 0;
};

/**
	NxScaleVsDensityModifier applies a scale factor function against a single axis versus the density of the particle.
*/
class NxScaleVsDensityModifier : public NxModifierT<NxScaleVsDensityModifier>
{
public:
	static const ModifierTypeEnum ModifierType = ModifierType_ScaleVsDensity;
	static const physx::PxU32 ModifierUsage = ModifierUsage_Continuous
	        | ModifierUsage_Sprite | ModifierUsage_Mesh;


	/// get the axis along which the scale factor will be applied
	virtual ScaleAxis getScaleAxis() const = 0;
	/// set the axis along which the scale factor will be applied
	virtual void setScaleAxis(ScaleAxis a) = 0;
	/// get the the curve that defines the dependency between the density and the scale factor
	virtual const NxCurve* getFunction() const = 0;
	/// set the the curve that defines the dependency between the density and the scale factor
	virtual void setFunction(const NxCurve* f) = 0;
};

/**
	NxScaleVsCameraDistance applies a scale factor against a specific axis based on distance from the camera to the particle.
*/
class NxScaleVsCameraDistanceModifier : public NxModifierT<NxScaleVsCameraDistanceModifier>
{
public:
	static const ModifierTypeEnum ModifierType = ModifierType_ScaleVsCameraDistance;
	static const physx::PxU32 ModifierUsage = ModifierUsage_Continuous
	        | ModifierUsage_Sprite | ModifierUsage_Mesh;


	/// get the axis along which the scale factor will be applied
	virtual ScaleAxis getScaleAxis() const = 0;
	/// set the axis along which the scale factor will be applied
	virtual void setScaleAxis(ScaleAxis a) = 0;
	/// get the the curve that defines the dependency between the camera distance and the scale factor
	virtual const NxCurve* getFunction() const = 0;
	/// set the the curve that defines the dependency between the camera distance and the scale factor
	virtual void setFunction(const NxCurve* f) = 0;
};

/**
	NxViewDirectionSortingModifier sorts sprite particles along view direction back to front.
*/
class NxViewDirectionSortingModifier : public NxModifierT<NxViewDirectionSortingModifier>
{
public:
	static const ModifierTypeEnum ModifierType = ModifierType_ViewDirectionSorting;
	static const physx::PxU32 ModifierUsage = ModifierUsage_Continuous
	        | ModifierUsage_Sprite;

	// Access to expected data members
};

/**
	NxRotationRateModifier is a modifier to apply a continuous rotation for sprites.
*/
class NxRotationRateModifier : public NxModifierT<NxRotationRateModifier>
{
public:
	static const ModifierTypeEnum ModifierType = ModifierType_RotationRate;
	static const physx::PxU32 ModifierUsage = ModifierUsage_Continuous
	        | ModifierUsage_Sprite;


	/// set the rotation rate
	virtual physx::PxF32 getRotationRate() const = 0;
	/// get the rotation rate
	virtual void setRotationRate(const physx::PxF32& rate) = 0;
};

/**
	NxRotationRateVsLifeModifier is a modifier to adjust the rotation rate versus the life remaining of a particular particle.
*/
class NxRotationRateVsLifeModifier : public NxModifierT<NxRotationRateVsLifeModifier>
{
public:
	static const ModifierTypeEnum ModifierType = ModifierType_RotationRateVsLife;
	static const physx::PxU32 ModifierUsage = ModifierUsage_Continuous
	        | ModifierUsage_Sprite;


	/// get the curve that sets the dependency between the life remaining and the rotation rate
	virtual const NxCurve* getFunction() const = 0;
	/// set the curve that sets the dependency between the life remaining and the rotation rate
	virtual void setFunction(const NxCurve* f) = 0;
};

/**
	NxOrientScaleAlongScreenVelocityModifier is a modifier to orient & scale sprites along the current screen velocity vector.
*/
class NxOrientScaleAlongScreenVelocityModifier : public NxModifierT<NxOrientScaleAlongScreenVelocityModifier>
{
public:
	static const ModifierTypeEnum ModifierType = ModifierType_OrientScaleAlongScreenVelocity;
	static const physx::PxU32 ModifierUsage = ModifierUsage_Continuous
	        | ModifierUsage_Sprite;


	/// set the scale per velocity
	virtual physx::PxF32 getScalePerVelocity() const = 0;
	/// get the scale per velocity
	virtual void setScalePerVelocity(const physx::PxF32& s) = 0;
};


#pragma warning( default: 4100 )

PX_POP_PACK

}
} // namespace apex

#endif // NX_MODIFIER_H
