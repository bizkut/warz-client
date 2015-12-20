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
 
//#include "safestdio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AutoGeometry.h"
#include "StanHull.h"
#include "PsUserAllocated.h"
#include "foundation/PxAssert.h"
#include "PsArray.h"
#include "PsString.h"

#pragma warning(disable:4100 4996)


namespace AUTO_GEOMETRY_PXSHARE
{

	void  fm_transform(const float matrix[16],const float v[3],float t[3]) // rotate and translate this point
	{
		if ( matrix )
		{
			float tx = (matrix[0*4+0] * v[0]) +  (matrix[1*4+0] * v[1]) + (matrix[2*4+0] * v[2]) + matrix[3*4+0];
			float ty = (matrix[0*4+1] * v[0]) +  (matrix[1*4+1] * v[1]) + (matrix[2*4+1] * v[2]) + matrix[3*4+1];
			float tz = (matrix[0*4+2] * v[0]) +  (matrix[1*4+2] * v[1]) + (matrix[2*4+2] * v[2]) + matrix[3*4+2];
			t[0] = tx;
			t[1] = ty;
			t[2] = tz;
		}
		else
		{
			t[0] = v[0];
			t[1] = v[1];
			t[2] = v[2];
		}
	}
	inline float det(const float *p1,const float *p2,const float *p3)
	{
		return  p1[0]*p2[1]*p3[2] + p2[0]*p3[1]*p1[2] + p3[0]*p1[1]*p2[2] -p1[0]*p3[1]*p2[2] - p2[0]*p1[1]*p3[2] - p3[0]*p2[1]*p1[2];
	}


	float  fm_computeMeshVolume(const float *vertices,size_t tcount,const unsigned int *indices)
	{
		float volume = 0;

		for (unsigned int i=0; i<tcount; i++,indices+=3)
		{
			const float *p1 = &vertices[ indices[0]*3 ];
			const float *p2 = &vertices[ indices[1]*3 ];
			const float *p3 = &vertices[ indices[2]*3 ];
			volume+=det(p1,p2,p3); // compute the volume of the tetrahedran relative to the origin.
		}

		volume*=(1.0f/6.0f);
		if ( volume < 0 )
			volume*=-1;
		return volume;
	}



	class Vec3
	{
	public:
		Vec3(const float *pos)
		{
			x = pos[0];
			y = pos[1];
			z = pos[2];
		}
		float x;
		float y;
		float z;
	};

	typedef physx::Array< Vec3 > Vec3Vector;

	class MyHull : public physx::SimpleHull, public physx::UserAllocated
	{
	public:
		MyHull(void)
		{
			mValidHull = false;
		}

		~MyHull(void)
		{
			release();
		}

		void release(void)
		{
			PX_FREE(mIndices);
			PX_FREE(mVertices);
			mIndices = 0;
			mVertices = 0;
			mTriCount = 0;
			mVertexCount = 0;
		}

		void addPos(const float *p)
		{
			Vec3 v(p);
			mPoints.pushBack(v);
		}

		float generateHull(void)
		{
			release();
			if ( mPoints.size() >= 3 ) // must have at least 3 vertices to create a hull.
			{
				// now generate the convex hull.
				physx::stanhull::HullDesc desc((physx::PxU32)(physx::stanhull::QF_TRIANGLES | physx::stanhull::QF_SKIN_WIDTH), (physx::PxU32)mPoints.size(),&mPoints[0].x, sizeof(float)*3);
				desc.mMaxVertices = 32;
				desc.mSkinWidth = 0.001f;

				physx::stanhull::HullLibrary h;
				physx::stanhull::HullResult result;
				physx::stanhull::HullError e = h.CreateConvexHull(desc,result);
				if ( e == physx::stanhull::QE_OK )
				{
					mTriCount = result.mNumFaces;
					mIndices  = (unsigned int *)PX_ALLOC(sizeof(unsigned int)*mTriCount*3, PX_DEBUG_EXP("SimpleHull::generateHull"));
					memcpy(mIndices,result.mIndices,sizeof(unsigned int)*mTriCount*3);
					mVertexCount = result.mNumOutputVertices;
					mVertices = (float *)PX_ALLOC(sizeof(float)*mVertexCount*3, PX_DEBUG_EXP("SimpleHull::generateHull"));
					memcpy(mVertices,result.mOutputVertices,sizeof(float)*mVertexCount*3);
					mValidHull = true;
					mMeshVolume = fm_computeMeshVolume( mVertices, mTriCount, mIndices ); // compute the volume of this mesh.
					h.ReleaseResult(result);
				}
			}
			return mMeshVolume;
		}

		void inherit(MyHull &c)
		{
			Vec3Vector::Iterator i;
			for (i=c.mPoints.begin(); i!=c.mPoints.end(); ++i)
			{
				mPoints.pushBack( (*i) );
			}
			c.mPoints.clear();
			c.mValidHull = false;
			generateHull();
		}

		void setTransform(const physx::SimpleBone &b,int bone_index)
		{
			mBoneName    = b.mBoneName;
			mBoneIndex   = bone_index;
			mParentIndex = b.mParentIndex;
			memcpy(mTransform,b.mTransform,sizeof(float)*16);
			if ( mVertexCount )
			{
				for (unsigned int i=0; i<mVertexCount; i++)
				{
					float *vtx = &mVertices[i*3];
					fm_transform(b.mInverseTransform,vtx,vtx); // inverse transform the point into bone relative object space
				}
			}

		}

		bool        mValidHull;
		Vec3Vector  mPoints;
	};

	class MyAutoGeometry : public physx::AutoGeometry, public physx::UserAllocated
	{
	public:
		MyAutoGeometry()
		{
			mHulls = 0;
			mSimpleHulls = 0;
		}

		virtual ~MyAutoGeometry(void)
		{
			release();
		}

		void release(void)
		{
			delete []mHulls;
			mHulls = 0;
			PX_FREE(mSimpleHulls);
			mSimpleHulls = 0;
		}

#define MAX_BONE_COUNT 8

		void addBone(unsigned int bone,unsigned int *bones,unsigned int &bcount)
		{
			PX_ASSERT(bcount < MAX_BONE_COUNT);
			if ( bcount < MAX_BONE_COUNT )
			{
				bool found = false;

				for (unsigned int i=0; i<bcount; i++)
				{
					if ( bones[i] == bone )
					{
						found = true;
						break;
					}
				}
				if ( !found )
				{
					bones[bcount] = bone;
					bcount++;
				}
			}
		}

		void addBones(const physx::SimpleSkinnedVertex &v,unsigned int *bones,unsigned int &bcount, const physx::SimpleBone *sbones)
		{
			if ( v.mWeight[0] >= sbones[v.mBone[0]].mBoneMinimalWeight) addBone(v.mBone[0],bones,bcount);
			if ( v.mWeight[1] >= sbones[v.mBone[1]].mBoneMinimalWeight) addBone(v.mBone[1],bones,bcount);
			if ( v.mWeight[2] >= sbones[v.mBone[2]].mBoneMinimalWeight) addBone(v.mBone[2],bones,bcount);
			if ( v.mWeight[3] >= sbones[v.mBone[3]].mBoneMinimalWeight) addBone(v.mBone[3],bones,bcount);
		}

		void addTri(const physx::SimpleSkinnedVertex &v1,const physx::SimpleSkinnedVertex &v2,const physx::SimpleSkinnedVertex &v3,const physx::SimpleBone *sbones)
		{
			unsigned int bcount = 0;
			unsigned int bones[MAX_BONE_COUNT];
			addBones(v1,bones,bcount, sbones);
			addBones(v2,bones,bcount, sbones);
			addBones(v3,bones,bcount, sbones);
			for (unsigned int i=0; i<bcount; i++)
			{
				addPos(v1.mPos, bones[i], sbones );
				addPos(v2.mPos, bones[i], sbones );
				addPos(v3.mPos, bones[i], sbones );
			}
		}

		virtual physx::SimpleHull ** createCollisionVolumes(float collapse_percentage,
			unsigned int bone_count,
			const physx::SimpleBone *bones,
			const physx::SimpleSkinnedMesh *mesh,
			unsigned int &geom_count)
		{
			release();
			geom_count = 0;

			mHulls = PX_NEW(MyHull)[bone_count];

			for (unsigned int i=0; i<bone_count; i++)
			{
				const physx::SimpleBone &b = bones[i];
				mHulls[i].setTransform(b,i);
			}

			unsigned int tcount = mesh->mVertexCount/3;

			for (unsigned int i=0; i<tcount; i++)
			{
				const physx::SimpleSkinnedVertex &v1 = mesh->mVertices[i*3+0];
				const physx::SimpleSkinnedVertex &v2 = mesh->mVertices[i*3+0];
				const physx::SimpleSkinnedVertex &v3 = mesh->mVertices[i*3+0];
				addTri(v1,v2,v3,bones);
			}

			float totalVolume = 0;
			for (unsigned int i=0; i<bone_count; i++)
			{
				totalVolume+=mHulls[i].generateHull();
			}

			// ok.. now do auto-collapse of hulls...
#if 1
			if ( collapse_percentage > 0 )
			{
				float ratio = collapse_percentage / 100.0f;
				for (int i=(int)(bone_count-1); i>=0; i--)
				{
					MyHull &h = mHulls[i];
					const physx::SimpleBone &b = bones[i];
					if ( b.mParentIndex >= 0 )
					{
						MyHull &parent_hull = mHulls[b.mParentIndex];
						if ( h.mValidHull && parent_hull.mValidHull )
						{
							if ( h.mMeshVolume < (parent_hull.mMeshVolume*ratio) ) // if we are less than 1/3 the volume of our parent, copy our vertices to the parent..
							{
								parent_hull.inherit(h);
							}
						}
					}
				}
			}
#endif
			for (int i=0; i<(int)bone_count; i++)
			{
				MyHull &h = mHulls[i];
				if ( h.mValidHull )
					geom_count++;
			}

			if ( geom_count )
			{
				mSimpleHulls = (physx::SimpleHull **)PX_ALLOC(sizeof(physx::SimpleHull *)*geom_count, PX_DEBUG_EXP("SimpleHull::createCollisionVolumes"));
				int index = 0;
				for (int i=0; i<(int)bone_count; i++)
				{
					MyHull *hull = &mHulls[i];
					if ( hull->mValidHull )
					{
						const physx::SimpleBone &b = bones[i];
						hull->setTransform(b,i);
						mSimpleHulls[index] = hull;
						index++;
					}
				}
			}

			return mSimpleHulls;
		}

		void addPos(const float *p,int bone,const physx::SimpleBone *bones)
		{
			switch ( bones[bone].mOption )
			{
			case physx::BO_INCLUDE:
				mHulls[bone].addPos(p);
				break;
			case physx::BO_EXCLUDE:
				break;
			case physx::BO_COLLAPSE:
				{
					while ( bone >= 0 )
					{
						bone = bones[bone].mParentIndex;
						if ( bones[bone].mOption == physx::BO_INCLUDE )
							break;
						else if ( bones[bone].mOption == physx::BO_EXCLUDE )
						{
							bone = -1;
							break;
						}
					}
					if ( bone >= 0 )
					{
						mHulls[bone].addPos(p); // collapse into the parent
					}
				}
				break;
			}
		}

		virtual physx::SimpleHull ** createCollisionVolumes(float collapse_percentage,unsigned int &geom_count)
		{
			physx::SimpleHull **ret = 0;

			if ( !mVertices.empty() && !mBones.empty() )
			{
				physx::SimpleSkinnedMesh mesh;
				mesh.mVertexCount = (physx::PxU32)mVertices.size();
				mesh.mVertices    = &mVertices[0];
				ret = createCollisionVolumes(collapse_percentage,(physx::PxU32)mBones.size(),&mBones[0],&mesh,geom_count);
				mVertices.clear();
				mBones.clear();
			}

			return ret;
		}

		virtual void addSimpleSkinnedTriangle(const physx::SimpleSkinnedVertex &v1,const physx::SimpleSkinnedVertex &v2,const physx::SimpleSkinnedVertex &v3)
		{
			mVertices.pushBack(v1);
			mVertices.pushBack(v2);
			mVertices.pushBack(v3);
		}

		virtual void addSimpleBone(const physx::SimpleBone &_b)
		{
			physx::SimpleBone b = _b;
			mBones.pushBack(b);
		}

		virtual const char * stristr(const char *str,const char *key)  // case insensitive ststr
		{
			PX_ASSERT( strlen(str) < 2048 );
			PX_ASSERT( strlen(key) < 2048 );

			char istr[2048];
			char ikey[2048];

			strncpy(istr,str,2048);
			strncpy(ikey,key,2048);
			physx::string::strlwr(istr);
			physx::string::strlwr(ikey);

			char *foo = strstr(istr,ikey);
			if ( foo )
			{
				unsigned int loc = (unsigned int)(foo - istr);
				foo = (char *)str+loc;
			}

			return foo;
		}

	private:
		typedef physx::Array< physx::SimpleBone > SimpleBoneVector;
		typedef physx::Array< physx::SimpleSkinnedVertex > SimpleSkinnedVertexVector;
		SimpleBoneVector mBones;
		SimpleSkinnedVertexVector mVertices;

		MyHull      *mHulls;
		physx::SimpleHull **mSimpleHulls;
	};

}; // end of namespace

namespace physx
{
	namespace general_stan_hull2
	{
	physx::AutoGeometry * createAutoGeometry()
	{
		AUTO_GEOMETRY_PXSHARE::MyAutoGeometry *g = PX_NEW(AUTO_GEOMETRY_PXSHARE::MyAutoGeometry());
		return static_cast< physx::AutoGeometry *>(g);
	}

	void           releaseAutoGeometry(physx::AutoGeometry *g)
	{
		AUTO_GEOMETRY_PXSHARE::MyAutoGeometry * m = static_cast<AUTO_GEOMETRY_PXSHARE::MyAutoGeometry *>(g);
		delete m;
	}

}; // end of namespace
};
