#ifndef	__ETERNITY_Cone_H
#define	__ETERNITY_Cone_H

/// Our simple cone
//----------------------------------------------------------------
class r3dCone
//----------------------------------------------------------------
{
public:
	float			fFOV;			///< cone angle
	r3dVector		vCenter;		///< center vertex
	r3dVector		vDir;			///< direction vector
public:
	r3dCone (){}
	r3dCone ( float _fFOV, r3dVector _vCenter, r3dVector _vDir )
		: fFOV		( _fFOV )
		, vCenter	( _vCenter )
		, vDir		( _vDir )
	{}
};


#endif	// __ETERNITY_Cone_H

