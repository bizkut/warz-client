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

#ifndef NX_CLOTHING_ASSET_H
#define NX_CLOTHING_ASSET_H

#include "NxApexAsset.h"

namespace physx
{
namespace apex
{

PX_PUSH_PACK_DEFAULT

class NxClothingActor;
class NxClothingPreview;

#define NX_CLOTHING_AUTHORING_TYPE_NAME "NxClothingAsset"


/**
\brief Describes the solver that is used to simulate the clothing actor.
*/
struct ClothSolverMode
{
	/** \brief the enum type */
	enum Enum
	{
		/**
		\brief The actor is simulated using the 2.8.x cloth solver.
		*/
		v2,

		/**
		\brief The actor is simulated using the 3.x cloth solver.
		*/
		v3,
	};
};


/**
\brief A clothing asset. It contains all the static and shared data for a given piece of clothing.
*/
class NxClothingAsset : public NxApexAsset
{
protected:
	// do not manually delete this object
	virtual ~NxClothingAsset() {}
public:
	/**
	\brief Returns the number of NxClothingActors this asset has created.
	*/
	virtual physx::PxU32 getNumActors() const = 0;

	/**
	\brief Returns the index'th NxClothingActor
	*/
	virtual NxClothingActor* getActor(physx::PxU32 index) = 0;

	/**
	\brief Returns the bounding box for the asset.
	*/
	virtual physx::PxBounds3 getBoundingBox() const = 0;

	/**
	\brief returns the number of LOD levels present in this asset
	*/
	virtual physx::PxU32 getNumGraphicalLodLevels() const = 0;

	/**
	\brief returns the actual LOD value of any particular level, normally this is just the identity map
	*/
	virtual physx::PxU32 getGraphicalLodValue(physx::PxU32 lodLevel) const = 0;

	/**
	\brief returns the biggest max distance of any vertex in any physical mesh
	*/
	virtual physx::PxF32 getBiggestMaxDistance() const = 0;

	/**
	\brief returns for which solver the asset has been cooked for.
	*/
	virtual ClothSolverMode::Enum getClothSolverMode() const = 0;

	/**
	\brief remaps bone with given name to a new index for updateState calls

	This is needed when the order of bones differs from authoring tool to runtime

	\note must be called after asset is deserialized. Changing this any later, especially after the first NxClothingActor::updateState call
	will lead to ill-defined behavior.
	*/
	virtual bool remapBoneIndex(const char* name, physx::PxU32 newIndex) = 0;

	/**
	\brief Returns the number of bones
	*/
	virtual physx::PxU32 getNumBones() const = 0;

	/**
	\brief Returns the number of bones that are actually used. They are the first ones internally.
	*/
	virtual physx::PxU32 getNumUsedBones() const = 0;

	/**
	\brief Returns the name of the bone at the given internal index.
	*/
	virtual const char* getBoneName(physx::PxU32 internalIndex) const = 0;

	/**
	\brief Returns the bind pose transform for this bone.
	*/
	virtual bool getBoneBasePose(physx::PxU32 internalIndex, physx::PxMat44& result) const = 0;

	/**
	\brief returns the mapping from internal to external indices for a given asset.

	Use this map to transform all the boneIndices returned from the ClothingPhysicalMesh into the order you specified initially
	*/
	virtual void getBoneMapping(physx::PxU32* internal2externalMap) const = 0;

	/**
	\brief Gets the NxRenderMeshAsset associated with this asset.

	\param [in] lodLevel	The LoD level of the render mesh asset
	\return NULL if lodLevel is not valid
	\see NxClothingAsset::getNumGraphicalLodLevels()
	*/
	virtual const NxRenderMeshAsset*		getRenderMeshAsset(physx::PxU32 lodLevel) const = 0;

	/**
	\brief Prepares the asset for use with morph target.

	When setting morph target displacements, the asset needs to create a mapping of external to internal vertex order.
	These positions will be matched with the internal mesh positions with a smallest distance criterion.

	\param [in] originalPositions	Array of positions before the morphing is applied.
	\param [in] numPositions		Length of the Array
	\param [in] epsilon				Difference two vertices can have before being reported
	\return							Number of vertices for which the distance to the next originalPosition was larger than epsilon
	*/
	virtual PxU32 prepareMorphTargetMapping(const physx::PxVec3* originalPositions, physx::PxU32 numPositions, physx::PxF32 epsilon) = 0;
};

PX_POP_PACK

} // namespace apex
} // namespace physx

#endif // NX_CLOTHING_ASSET_H
