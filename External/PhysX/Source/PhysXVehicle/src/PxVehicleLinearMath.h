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
// Copyright (c) 2008-2013 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#ifndef PX_VEHICLE_LINEAR_MATH_H
#define PX_VEHICLE_LINEAR_MATH_H
/** \addtogroup vehicle
  @{
*/

#include "PxVehicleSDK.h"

#ifndef PX_DOXYGEN
namespace physx
{
#endif

#define MAX_VECTORN_SIZE (PX_MAX_NUM_WHEELS+1)

class VectorN
{
public:

	VectorN(const PxU32 size)
		: mSize(size)
	{
		PX_ASSERT(mSize<MAX_VECTORN_SIZE);
	}
	~VectorN()
	{
	}

	VectorN(const VectorN& src)
	{
		for(PxU32 i=0;i<src.mSize;i++)
		{
			mValues[i]=src.mValues[i];
		}
		mSize=src.mSize;
	}

	VectorN& operator=(const VectorN& src)
	{
		for(PxU32 i=0;i<src.mSize;i++)
		{
			mValues[i]=src.mValues[i];
		}
		mSize=src.mSize;
		return *this;
	}

	PX_FORCE_INLINE PxF32& operator[] (const PxU32 i)
	{
		PX_ASSERT(i<mSize);
		return (mValues[i]);
	}

	PX_FORCE_INLINE const PxF32& operator[] (const PxU32 i) const
	{
		PX_ASSERT(i<mSize);
		return (mValues[i]);
	}

	PX_FORCE_INLINE PxU32 getSize() const {return mSize;}

private:

	PxF32 mValues[MAX_VECTORN_SIZE];
	PxU32 mSize;
};

class MatrixNN
{
public:

	MatrixNN()
		: mSize(0)
	{
	}
	MatrixNN(const PxU32 size)
		: mSize(size)
	{
		PX_ASSERT(mSize<MAX_VECTORN_SIZE);
	}
	MatrixNN(const MatrixNN& src)
	{
		for(PxU32 i=0;i<src.mSize;i++)
		{
			for(PxU32 j=0;j<src.mSize;j++)
			{
				mValues[i][j]=src.mValues[i][j];
			}
		}
		mSize=src.mSize;
	}
	~MatrixNN()
	{
	}

	MatrixNN& operator=(const MatrixNN& src)
	{
		for(PxU32 i=0;i<src.mSize;i++)
		{
			for(PxU32 j=0;j<src.mSize;j++)
			{
				mValues[i][j]=src.mValues[i][j];
			}
		}
		mSize=src.mSize;
		return *this;
	}

	PX_FORCE_INLINE PxF32 get(const PxU32 i, const PxU32 j) const
	{
		PX_ASSERT(i<mSize);
		PX_ASSERT(j<mSize);
		return mValues[i][j];
	}
	PX_FORCE_INLINE void set(const PxU32 i, const PxU32 j, const PxF32 val)
	{
		PX_ASSERT(i<mSize);
		PX_ASSERT(j<mSize);
		mValues[i][j]=val;
	}

	PX_FORCE_INLINE PxU32 getSize() const {return mSize;}

public:

	PxF32 mValues[MAX_VECTORN_SIZE][MAX_VECTORN_SIZE];
	PxU32 mSize;
};

class MatrixNNLUSolver
{
private:

	PxU32 mIndex[MAX_VECTORN_SIZE];
	MatrixNN mLU;

public:

	MatrixNNLUSolver(){}
	~MatrixNNLUSolver(){}

	//Algorithm taken from Numerical Recipes in Fortran 77, page 38

	void decomposeLU(const MatrixNN& a)
	{
#define TINY (1.0e-20f)

		const PxU32 size=a.mSize;

		//Copy a into result then work exclusively on the result matrix.
		MatrixNN LU=a;

		//Initialise index swapping values.
		for(PxU32 i=0;i<size;i++)
		{
			mIndex[i]=0xffffffff;
		}

		PxU32 imax=0;
		PxF32 big,dum,sum;
		PxF32 vv[MAX_VECTORN_SIZE];
		PxF32 d=1.0f;

		for(PxU32 i=0;i<=(size-1);i++)
		{
			big=0.0f;
			for(PxU32 j=0;j<=(size-1);j++)
			{
				const PxF32 temp=PxAbs(LU.get(i,j));
				big = temp>big ? temp : big;
			}
			PX_ASSERT(big!=0.0f);
			vv[i]=1.0f/big;
		}

		for(PxU32 j=0;j<=(size-1);j++)
		{
			for(PxU32 i=0;i<j;i++)
			{
				PxF32 sum=LU.get(i,j);
				for(PxU32 k=0;k<i;k++)
				{
					sum-=LU.get(i,k)*LU.get(k,j);
				}
				LU.set(i,j,sum);
			}

			big=0.0f;
			for(PxU32 i=j;i<=(size-1);i++)
			{
				sum=LU.get(i,j);
				for(PxU32 k=0;k<j;k++)
				{
					sum-=LU.get(i,k)*LU.get(k,j);
				}
				LU.set(i,j,sum);
				dum=vv[i]*PxAbs(sum);
				if(dum>=big)
				{
					big=dum;
					imax=i;
				}
			}

			if(j!=imax)
			{
				for(PxU32 k=0;k<size;k++)
				{
					dum=LU.get(imax,k);
					LU.set(imax,k,LU.get(j,k));
					LU.set(j,k,dum);
				}
				d=-d;
				vv[imax]=vv[j];
			}
			mIndex[j]=imax;

			if(LU.get(j,j)==0)
			{
				LU.set(j,j,TINY);
			}

			if(j!=(size-1))
			{
				dum=1.0f/LU.get(j,j);
				for(PxU32 i=j+1;i<=(size-1);i++)
				{
					LU.set(i,j,LU.get(i,j)*dum);
				}
			}
		}

		//Store the result.
		mLU=LU;
	}

	void solve(const VectorN& input, VectorN& result) const
	{
		const PxU32 size=input.getSize();

		result=input;

		PxU32 ip;
		PxU32 ii=0xffffffff;
		PxF32 sum;

		for(PxU32 i=0;i<size;i++)
		{
			ip=mIndex[i];
			sum=result[ip];
			result[ip]=result[i];
			if(ii!=-1)
			{
				for(PxU32 j=ii;j<=(i-1);j++)
				{
					sum-=mLU.get(i,j)*result[j];
				}
			}
			else if(sum!=0)
			{
				ii=i;
			}
			result[i]=sum;
		}
		for(PxI32 i=size-1;i>=0;i--)
		{
			sum=result[i];
			for(PxU32 j=i+1;j<=(size-1);j++)
			{
				sum-=mLU.get(i,j)*result[j];
			}
			result[i]=sum/mLU.get(i,i);
		}
	}
};


#ifndef PX_DOXYGEN
} // namespace physx
#endif

#endif //PX_VEHICLE_LINEAR_MATH_H