#ifndef	__ETERNITY_Sphere_H
#define	__ETERNITY_Sphere_H

/// this is trivial cylinder class
//----------------------------------------------------------------
class r3dSphere
//----------------------------------------------------------------
{
public:
	r3dVector vCenter;		///< center point of sphere
	float	  fRange;		///< radius of sphere

public:
	r3dSphere (){}
	r3dSphere ( r3dVector center, float radius )
		: vCenter( center )
		, fRange ( radius )
	{}
};


#endif	// __ETERNITY_Cylinder_H

