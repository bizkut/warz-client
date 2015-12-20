#ifndef REMOVE_TJUNCTIONS_H

#define REMOVE_TJUNCTIONS_H

#include "Ps.h"
#include "foundation/PxSimpleTypes.h"

namespace physx
{
	namespace general_meshutils2
	{

class RemoveTjunctionsDesc
{
public:
  RemoveTjunctionsDesc(void)
  {
    mVcount = 0;
    mVertices = 0;
    mTcount = 0;
    mIndices = 0;
    mIds = 0;
    mTcountOut = 0;
    mIndicesOut = 0;
    mIdsOut = 0;
	mEpsilon = 0.00000001f;
  }

// input
  PxF32        mEpsilon;
  PxF32        mDistanceEpsilon;
  PxU32        mVcount;  // input vertice count.
  const PxF32 *mVertices; // input vertices as PxF32s or...
  PxU32        mTcount;    // number of input triangles.
  const PxU32 *mIndices;   // triangle indices.
  const PxU32 *mIds;       // optional triangle Id numbers.
// output..
  PxU32        mTcountOut;  // number of output triangles.
  const PxU32 *mIndicesOut; // output triangle indices
  const PxU32 *mIdsOut;     // output retained id numbers.
};

// Removes t-junctions from an input mesh.  Does not generate any new data points, but may possible produce additional triangles and new indices.
class RemoveTjunctions
{
public:

   virtual PxU32 removeTjunctions(RemoveTjunctionsDesc &desc) =0; // returns number of triangles output and the descriptor is filled with the appropriate results.


};

RemoveTjunctions * createRemoveTjunctions(void);
void               releaseRemoveTjunctions(RemoveTjunctions *tj);

	};
	using namespace general_meshutils2;
};

#endif
