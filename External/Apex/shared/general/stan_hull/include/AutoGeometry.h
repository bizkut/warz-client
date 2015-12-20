#ifndef AUTO_GEOMETRY_H

#define AUTO_GEOMETRY_H
/*
 * Copyright 2009-2011 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */
#include "PsShare.h"

namespace physx
{
	namespace general_stan_hull2
	{
		using namespace shdfnd;

	class SimpleHull
	{
	public:
		SimpleHull(void)
		{
			mBoneIndex = 0;
			mVertexCount = 0;
			mVertices  = 0;
			mTriCount = 0;
			mIndices = 0;
			mMeshVolume = 0;
			mParentIndex = -1;
			mBoneName = 0;
		}
		int           mBoneIndex;
		int           mParentIndex;
		const char   *mBoneName;
		float         mTransform[16];
		unsigned int  mVertexCount;
		float        *mVertices;
		unsigned int  mTriCount;
		unsigned int *mIndices;
		float         mMeshVolume;
	};

	enum BoneOption
	{
		BO_INCLUDE,
		BO_EXCLUDE,
		BO_COLLAPSE
	};

	class SimpleBone
	{
	public:
		SimpleBone(void)
		{
			mOption = BO_INCLUDE;
			mParentIndex = -1;
			mBoneName = 0;
			mBoneMinimalWeight = 0.4f;
		}
		BoneOption   mOption;
		const char  *mBoneName;
		int          mParentIndex;
		float        mBoneMinimalWeight;
		float        mTransform[16];
		float        mInverseTransform[16];
	};

	class SimpleSkinnedVertex
	{
	public:
		float          mPos[3];
		unsigned short mBone[4];
		float          mWeight[4];
	};

	class SimpleSkinnedMesh
	{
	public:
		unsigned int         mVertexCount;
		SimpleSkinnedVertex *mVertices;

	};

	class AutoGeometry
	{
	public:

		virtual SimpleHull ** createCollisionVolumes(float collapse_percentage,          // percentage volume to collapse a child into a parent
			unsigned int bone_count,
			const SimpleBone *bones,
			const SimpleSkinnedMesh *mesh,
			unsigned int &geom_count) = 0;


		virtual SimpleHull ** createCollisionVolumes(float collapse_percentage,unsigned int &geom_count) = 0;

		virtual void addSimpleSkinnedTriangle(const SimpleSkinnedVertex &v1,
			const SimpleSkinnedVertex &v2,
			const SimpleSkinnedVertex &v3) = 0;

		virtual void addSimpleBone(const SimpleBone &b) = 0;

		virtual const char * stristr(const char *str,const char *match) = 0; // case insensitive ststr

	};

	AutoGeometry * createAutoGeometry();
	void           releaseAutoGeometry(AutoGeometry *g);

}; // end of namespace
using namespace general_stan_hull2;
};


#endif
