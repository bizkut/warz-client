#ifndef MESH_ISLAND_GENERATION_H

#define MESH_ISLAND_GENERATION_H

#include "Ps.h"
#include "foundation/PxSimpleTypes.h"
#include "PsUserAllocated.h"

namespace physx
{
	namespace general_meshutils2
	{

class MeshIslandGeneration
{
public:

  virtual PxU32 islandGenerate(PxU32 tcount,const PxU32 *indices,const PxF32 *vertices) = 0;
  virtual PxU32 islandGenerate(PxU32 tcount,const PxU32 *indices,const PxF64 *vertices) = 0;

  // sometimes island generation can produce co-planar islands.  Slivers if you will.  If you are passing these islands into a geometric system
  // that wants to turn them into physical objects, they may not be acceptable.  In this case it may be preferable to merge the co-planar islands with
  // other islands that it 'touches'.
  virtual PxU32 mergeCoplanarIslands(const PxF32 *vertices) = 0;
  virtual PxU32 mergeCoplanarIslands(const PxF64 *vertices) = 0;

  virtual PxU32 mergeTouchingIslands(const PxF32 *vertices) = 0;
  virtual PxU32 mergeTouchingIslands(const PxF64 *vertices) = 0;

  virtual PxU32 *   getIsland(PxU32 index,PxU32 &tcount) = 0;


};

MeshIslandGeneration * createMeshIslandGeneration(void);
void                   releaseMeshIslandGeneration(MeshIslandGeneration *cm);

}; // end of namespace
	using namespace general_meshutils2;
};

#endif
