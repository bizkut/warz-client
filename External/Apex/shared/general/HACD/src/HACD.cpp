#include "HACD.h"
#include "hacdHACD.h"
#include <stdlib.h>
#include <string.h>
#include "PlatformConfigHACD.h"

#if USE_CONSTRAINT_BUILDER
#include "ConstraintBuilder.h"
#include "FloatMath.h"
#endif

#pragma warning(disable:4100 4996)


namespace HACD
{

class MyHACD_API : public HACD_API, public UANS::UserAllocated, public HACD::CallBackFunction
{
public:
	MyHACD_API(void)
	{
		mHACD = NULL;
		mUserCallback = NULL;
	}
	virtual ~MyHACD_API(void)
	{
		releaseHACD();
	}

	virtual hacd::HaU32	performHACD(const Desc &desc) 
	{
		hacd::HaU32 ret = 0;
		releaseHACD();
		mUserCallback = desc.mCallback;
		if ( desc.mVertexCount )
		{
			mHACD = PX_NEW(HACD);
			// convert the input data points into doubles
			for (hacd::HaU32 i=0; i<desc.mVertexCount; i++)
			{
				Vec3<hacd::HaF64> p;
				p.X() = desc.mVertices[i*3+0];
				p.Y() = desc.mVertices[i*3+1];
				p.Z() = desc.mVertices[i*3+2];
				mPoints.push_back(p);
			}
			for (hacd::HaU32 i=0; i<desc.mTriangleCount; i++)
			{
				Vec3<hacd::HaI32> t;
				t.X() = desc.mIndices[i*3+0];
				t.Y() = desc.mIndices[i*3+1];
				t.Z() = desc.mIndices[i*3+2];
				mTriangles.push_back(t);
			}

			mHACD->SetPoints(&mPoints[0]);
			mHACD->SetNPoints(desc.mVertexCount);
			mHACD->SetTriangles(&mTriangles[0]);
			mHACD->SetNTriangles(desc.mTriangleCount);
			mHACD->SetCompacityWeight(0.1);
			mHACD->SetVolumeWeight(0);
			mHACD->SetNClusters(desc.mMinHullCount);
			mHACD->SetCallBack(this);
			mHACD->SetAddExtraDistPoints(false);
			mHACD->SetAddNeighboursDistPoints(false);
			mHACD->SetAddFacesPoints(true);
			mHACD->SetNVerticesPerCH(desc.mMaxHullVertices);		// max of 64 vertices per convex-hull
			mHACD->SetConcavity(desc.mConcavity);
			mHACD->SetConnectDist(desc.mConnectDistance);
			mHACD->Compute();

			ret = (hacd::HaU32)mHACD->GetNClusters();
			for (hacd::HaU32 i=0; i<ret; i++)
			{
				Hull h;
				getHull(i,h);
				mHulls.push_back(h);
			}
		}
		return ret;
	}

	void releaseHull(Hull &h)
	{
		PX_FREE((void *)h.mIndices);
		PX_FREE((void *)h.mVertices);
		h.mIndices = NULL;
		h.mVertices = NULL;
	}

	virtual const Hull		*getHull(hacd::HaU32 index)  const
	{
		const Hull *ret = NULL;
		if ( index < mHulls.size() )
		{
			ret = &mHulls[index];
		}
		return ret;
	}

	virtual void getHull(hacd::HaU32 index,Hull &h)
	{
		h.mVertexCount = (hacd::HaU32)mHACD->GetNPointsCH(index);
		h.mTriangleCount = (hacd::HaU32)mHACD->GetNTrianglesCH(index);
		h.mVertices = (hacd::HaF32 *)PX_ALLOC(sizeof(hacd::HaF32)*3*h.mVertexCount, PX_DEBUG_EXP("HACD::getHull"));
		h.mIndices = h.mTriangleCount ? (hacd::HaU32 *)PX_ALLOC(sizeof(hacd::HaU32)*3*h.mTriangleCount, PX_DEBUG_EXP("HACD::getHull")) : NULL;

		Vec3<hacd::HaF64> * pointsCH = (Vec3<hacd::HaF64> *)PX_ALLOC(sizeof(Vec3<hacd::HaF64>)*h.mVertexCount, PX_DEBUG_EXP("HACD::getHull"));
		Vec3<hacd::HaI32> * trianglesCH = (Vec3<hacd::HaI32> *)PX_ALLOC(sizeof(Vec3<hacd::HaI32>)*h.mTriangleCount, PX_DEBUG_EXP("HACD::getHull"));

		mHACD->GetCH(index, pointsCH, trianglesCH);
		hacd::HaF32 *hullVertices = (hacd::HaF32 *)h.mVertices;
		for (hacd::HaU32 i=0; i<h.mVertexCount; i++)
		{
			hullVertices[i*3+0] = (hacd::HaF32)pointsCH[i].X();
			hullVertices[i*3+1] = (hacd::HaF32)pointsCH[i].Y();
			hullVertices[i*3+2] = (hacd::HaF32)pointsCH[i].Z();
		}
		hacd::HaU32 *hullIndices = (hacd::HaU32 *)h.mIndices;
		for (hacd::HaU32 i=0; i<h.mTriangleCount; i++)
		{
			hullIndices[i*3+0] = trianglesCH[i].X();
			hullIndices[i*3+1] = trianglesCH[i].Y();
			hullIndices[i*3+2] = trianglesCH[i].Z();
		}
		PX_FREE(pointsCH);
		PX_FREE(trianglesCH);
	}

	virtual void	releaseHACD(void) // release memory associated with the last HACD request
	{
		if ( mHACD )
		{
			delete mHACD;
			mHACD = NULL;
		}
		mPoints.clear();
		mTriangles.clear();
		for (hacd::HaU32 i=0; i<mHulls.size(); i++)
		{
			releaseHull(mHulls[i]);
		}
		mHulls.clear();
		mUserCallback = NULL;
	}


	virtual void release(void) // release the HACD_API interface
	{
		delete this;
	}

	virtual bool progressUpdate(const char * msg, hacd::HaF64 progress, hacd::HaF64 concavity, hacd::HaU32 nVertices)
	{
		bool ret = true;
		if ( mUserCallback )
		{
			ret = mUserCallback->hacdProgressUpdate(msg,(hacd::HaF32)progress,(hacd::HaF32)concavity,nVertices);
		}
		else
		{
			printf("%s", msg );
		}
		return ret;
	}

	virtual hacd::HaU32	generateConstraints(void)
	{
		hacd::HaU32 ret = 0;
#if USE_CONSTRAINT_BUILDER
		if ( mHulls.size() != 0 )
		{
			hacd::ConstraintBuilder *cb = hacd::createConstraintBuilder();
			for (hacd::HaU32 i=0; i<(hacd::HaU32)mHulls.size(); i++)
			{
				Hull &h = mHulls[i];
				hacd::HaF32 volume = hacd::fm_computeMeshVolume(h.mVertices,h.mTriangleCount,h.mIndices);
				hacd::addConvexHull(cb,h.mVertexCount,h.mVertices,h.mTriangleCount,h.mIndices,volume,i);
			}
			ret = hacd::buildConstraints(cb);
			for (hacd::HaU32 i=0; i<ret; i++)
			{
				HACD_API::Constraint result;
				hacd::Constraint *c = hacd::getConstraint(cb,i,result.mFrom,result.mTo);
				result.mIntersect[0] = c->mIntersect[0];
				result.mIntersect[1] = c->mIntersect[1];
				result.mIntersect[2] = c->mIntersect[2];
				mConstraints.push_back(result);
			}
			hacd::releaseConstraintBuilder(cb);
		}
#endif
		return ret;
	}

	virtual const Constraint * getConstraint(hacd::HaU32 index) const
	{
		const Constraint *ret = NULL;
		if ( index < (hacd::HaU32)mConstraints.size() )
		{
			ret = &mConstraints[index];
		}
		return ret;
	}

	virtual hacd::HaU32	getHullCount(void)
	{
		return (hacd::HaU32) mHulls.size(); 
	}

	virtual hacd::HaU32	getConstraintCount(void)
	{
		return (hacd::HaU32) mConstraints.size(); 
	}


private:
	HACD					*mHACD;
	Vec3Vector				mPoints;
	TriVector				mTriangles;
	UserCallback			*mUserCallback;
	STDNAME::vector< Hull >	mHulls;
	STDNAME::vector< Constraint > mConstraints;

};

HACD_API * createHACD_API(void)
{
	MyHACD_API *m = PX_NEW(MyHACD_API);
	return static_cast<HACD_API *>(m);
}


};



