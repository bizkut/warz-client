//
//
//#include "utils.h"
/*
#include "CollMain.h"
#include "CollFunc.hpp"

#define FILL_DEF_TRACE_PARAMS(); \
  trace.ClipCoef    = trace.Distance / move.RayLen; \
  trace.NewPosition = move.vStart + move.vMove * trace.ClipCoef;

#define HACK_COLLIDE_AS_BOXES(); \
  trace.Type        = CLT_Side; \
  trace.pMeshObj    = NULL; \
  trace.Material       = NULL; \
  trace.Distance    = 0; \
  trace.Normal      = r3dPoint3D(0, 1, 0); \
  FILL_DEF_TRACE_PARAMS();


	//
	// coll_Point
	//

DEF_COL_PROC(Point, Point)
{
  // points always not intersects.
  return FALSE;
}

DEF_COL_PROC(Point, Cylinder)
{
	float		dst;

  if(obj2->GetBBoxWorld().ContainsRay(move.vStart, move.vRay, move.RayLen, &dst) == 0)
    return FALSE;

  trace.Type        = CLT_Side;
  trace.Distance    = dst;
  FILL_DEF_TRACE_PARAMS();

  return TRUE;
}

	//
	// coll_Cylinder
	//

DEF_COL_PROC(Cylinder, Point)
{
  HACK_COLLIDE_AS_BOXES();
  trace.Distance    = move.RayLen;

  return TRUE;
}


DEF_COL_PROC(Cylinder, Cylinder)
{
	return FALSE;
  // solve cylinders in 2D space

}


	//
	// coll_Mesh


	//
	// additionals swapper functions
	// simulate inverse collision.



//DEF_COL_PROC(Cylinder, Point)  	{ CALL_INVERSE( coll_Point_Cylinder);	}
//DEF_COL_PROC(Cylinder, Point)  	{ return FALSE;	}
//DEF_COL_PROC(Mesh, Point)     	{ CALL_INVERSE( coll_Point_Mesh    );	}
//DEF_COL_PROC(Mesh, Cylinder) 	{ CALL_INVERSE( coll_Cylinder_Mesh );	}
//#undef CALL_INVERSE

	//
	// table for collision functions
	//

#define ext_Coll(x, y)	&coll_##x##_##y
fn_CollisionProc coll_ObjTable[CLM_EnumSize][CLM_EnumSize] =
{
  // Point
  {
    ext_Coll(Point, 	Point),		// Point
    ext_Coll(Point, 	Cylinder),	// Cylinder
  //  ext_Coll(Point, 	Mesh),		// Mesh
  },
  // Cylinder
  {
    ext_Coll(Cylinder,	Point),		// Point
    ext_Coll(Cylinder, 	Cylinder),	// Cylinder
  //  ext_Coll(Cylinder, 	Mesh),		// Mesh
  },
  // Mesh
//  {
 //   ext_Coll(Mesh,	Point),		// Point
//    ext_Coll(Mesh, 	Cylinder),	// Cylinder
 //   ext_Coll(Mesh, 	Mesh),		// Mesh
//  },
};
#undef ext_Coll

DEF_COL_PROC(Default, Default)
{
  return coll_ObjTable[obj->CollisionType][obj2->CollisionType](obj, obj2, move, trace);
}
*/