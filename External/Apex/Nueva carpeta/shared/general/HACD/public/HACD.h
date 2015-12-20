#ifndef HACD_H

#define HACD_H

#include "PlatformConfigHACD.h"
#include <stdlib.h>

namespace HACD
{


class HACD_API
{
public:
	class UserCallback
	{
	public:
		virtual bool hacdProgressUpdate(const char *message, hacd::HaF32 progress, hacd::HaF32 concavity, hacd::HaU32 nVertices) = 0;
	};

	class Constraint
	{
	public:
		hacd::HaU32	mFrom;			// The 'parent' hull
		hacd::HaU32	mTo;			// The 'child' hull
		hacd::HaF32	mNormal[3];		// the surface normal where the two hulls are joined
		hacd::HaF32	mIntersect[3];	// the world space intersection point where the two hulls are joined.
	};

	class Desc
	{
	public:
		Desc(void)
		{
			init();
		}

		UserCallback		*mCallback;
		hacd::HaU32		mTriangleCount;
		hacd::HaU32		mVertexCount;
		const hacd::HaF32	*mVertices;
		const hacd::HaU32	*mIndices;
		hacd::HaU32		mMinHullCount;
		hacd::HaF32		mConcavity;
		hacd::HaU32		mMaxHullVertices;
		hacd::HaF32		mConnectDistance;
		void init(void)
		{
			mCallback = NULL;
			mTriangleCount = 0;
			mVertexCount = 0;
			mVertices = NULL;
			mIndices = NULL;
			mMinHullCount = 10;
			mConcavity = 100;
			mMaxHullVertices = 64;
			mConnectDistance = 0;
		}
	};

	class Hull
	{
	public:
		hacd::HaU32			mTriangleCount;
		hacd::HaU32			mVertexCount;
		const hacd::HaF32		*mVertices;
		const hacd::HaU32		*mIndices;
	};

	virtual hacd::HaU32	performHACD(const Desc &desc) = 0;
	virtual hacd::HaU32	getHullCount(void) = 0;
	virtual const Hull		*getHull(hacd::HaU32 index) const = 0;
	virtual void			releaseHACD(void) = 0; // release memory associated with the last HACD request
	virtual hacd::HaU32	generateConstraints(void) = 0;
	virtual hacd::HaU32	getConstraintCount(void) = 0;

	virtual const Constraint		*getConstraint(hacd::HaU32 index)const  = 0;


	virtual void release(void) = 0; // release the HACD_API interface
protected:
	virtual ~HACD_API(void)
	{

	}
};

HACD_API * createHACD_API(void);

};

#endif
