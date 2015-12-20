#ifndef	__ETERNITY_Cylinder_H
#define	__ETERNITY_Cylinder_H

/// this is trivial cylinder class
//----------------------------------------------------------------
class r3dCylinder
//----------------------------------------------------------------
{
public:
	r3dVector vBase;		///< base point of cylinder
	r3dVector vAltitude;	///< altitude vector of cylinder, relative to base point
	float	  fRadius;		///< radius of cylinder

public:
	r3dCylinder (){}
	r3dCylinder ( r3dVector base, r3dVector altitude, float radius )
		: vBase		( base )
		, vAltitude ( altitude )
		, fRadius	( radius )
	{}
};


#endif	// __ETERNITY_Cylinder_H

