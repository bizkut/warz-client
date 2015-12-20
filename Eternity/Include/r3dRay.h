#ifndef	__ETERNITY_Ray_H
#define	__ETERNITY_Ray_H

class r3dSphere;
//----------------------------------------------------------------
class r3dRay
//----------------------------------------------------------------
{
  public:

	r3dPoint3D	org;
	r3dVector   dir;

	r3dRay( r3dPoint3D _org, r3dVector _dir ){ org = _org; dir = _dir;}
	r3dRay(){}

    BOOL	IsIntersect(const r3dBoundBox & p);
	
	/**	dir member should be normalized! */
	BOOL	IsIntersect(const r3dSphere & s, r3dPoint3D *outIntersectionPoint = 0) const;
};

#endif	// __ETERNITY_Ray_H

