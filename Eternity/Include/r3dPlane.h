//=========================================================================
//	Module: r3dPlane.h
//	Copyright (C) 2012.
//=========================================================================

#pragma once

/*	r3dPlane
------------------------------------------------------------------------------------------
	
	A r3dPlane in 3D Space represented in point-normal form (Ax + By + Cz + D = 0).

	The convention for the distanceant D is:

	D = -(A, B, C) dot (X, Y, Z)

------------------------------------------------------------------------------------------
*/

class r3dPlane
{
public:

	// ----- CREATORS ---------------------

    r3dPlane();
    r3dPlane(r3dPlane& r3dPlane);
    r3dPlane(const r3dPoint3D& Point0, const r3dPoint3D& Point1, const r3dPoint3D& Point2);
    ~r3dPlane();

	// ----- OPERATORS --------------------

	r3dPlane& operator=( r3dPlane& Src );


	// tests if data is identical
	friend bool operator==( r3dPlane& r3dPlaneA, r3dPlane& r3dPlaneB )
	{
		return (r3dPlaneA.m_Normal == r3dPlaneB.m_Normal && r3dPlaneA.m_Point==r3dPlaneB.m_Point);
	}

	// ----- MUTATORS ---------------------

    void Set(const r3dPoint3D& Point0, const r3dPoint3D& Point1, const r3dPoint3D& Point2);

	float SolveForX(float Y, float Z);
	float SolveForY(float X, float Z);
	float SolveForZ(float X, float Y);

	// ----- ACCESSORS --------------------

    r3dPoint3D& Normal();
    r3dPoint3D& Point();
    float& Distance();
                     
    float SignedDistance( const r3dPoint3D& Point);

private:

	// ----- DATA -------------------------

    r3dPoint3D     m_Normal;
    r3dPoint3D     m_Point;
    float       m_Distance;

	// ----- HELPER FUNCTIONS -------------

	// ----- UNIMPLEMENTED FUNCTIONS ------

};

//- Inline Functions ---------------------------------------------------------------------

//= CREATORS =============================================================================

/*	r3dPlane
------------------------------------------------------------------------------------------
	
	Default Constructor
	
------------------------------------------------------------------------------------------
*/
inline r3dPlane::r3dPlane()
{
}

/*	r3dPlane
------------------------------------------------------------------------------------------
	
	Default Copy Constructor
	
------------------------------------------------------------------------------------------
*/
inline r3dPlane::r3dPlane(r3dPlane& Rhs)
{
    m_Normal = Rhs.m_Normal;
    m_Point = Rhs.m_Point;
    m_Distance = Rhs.m_Distance;
}


inline r3dPlane::r3dPlane(const r3dPoint3D& Point0, const r3dPoint3D& Point1, const r3dPoint3D& Point2)
{
    Set(Point0, Point1, Point2);
}

inline r3dPlane::~r3dPlane()
{
}

//= OPERATORS ============================================================================


inline r3dPlane& r3dPlane::operator=( r3dPlane& Src )
{
    m_Normal = Src.m_Normal;
    m_Point = Src.m_Point;
    m_Distance = Src.m_Distance;

	return (*this);
}

//= MUTATORS =============================================================================

//:	SolveForX
//----------------------------------------------------------------------------------------
//
//	Given Z and Y, Solve for X on the r3dPlane 
//
//-------------------------------------------------------------------------------------://
inline float r3dPlane::SolveForX(float Y, float Z)
{
	//Ax + By + Cz + D = 0
	// Ax = -(By + Cz + D)
	// x = -(By + Cz + D)/A

	if (m_Normal[0])
	{
		return ( -(m_Normal[1]*Y + m_Normal[2]*Z + m_Distance) / m_Normal[0] );
	}

	return (0.0f);
}

//:	SolveForY
//----------------------------------------------------------------------------------------
//
//	Given X and Z, Solve for Y on the r3dPlane 
//
//-------------------------------------------------------------------------------------://
inline float r3dPlane::SolveForY(float X, float Z)
{
	//Ax + By + Cz + D = 0
	// By = -(Ax + Cz + D)
	// y = -(Ax + Cz + D)/B

	if (m_Normal[1])
	{
		return ( -(m_Normal[0]*X + m_Normal[2]*Z + m_Distance) / m_Normal[1] );
	}

	return (0.0f);
}

//:	SolveForZ
//----------------------------------------------------------------------------------------
//
//	Given X and Y, Solve for Z on the r3dPlane 
//
//-------------------------------------------------------------------------------------://
inline float r3dPlane::SolveForZ(float X, float Y)
{
	//Ax + By + Cz + D = 0
	// Cz = -(Ax + By + D)
	// z = -(Ax + By + D)/C

	if (m_Normal[2])
	{
		return ( -(m_Normal[0]*X + m_Normal[1]*Y + m_Distance) / m_Normal[2] );
	}

	return (0.0f);
}


//:	Set
//----------------------------------------------------------------------------------------
//
//	Setup r3dPlane object given a clockwise ordering of 3D points 
//
//-------------------------------------------------------------------------------------://
inline void r3dPlane::Set(const r3dPoint3D& Point0, const r3dPoint3D& Point1, const r3dPoint3D& Point2)
{
    m_Normal = (Point1 - Point0).Cross(Point2 - Point0);
    m_Normal.Normalize();
    m_Point = Point0;

    m_Distance = -m_Point.Dot(m_Normal);
}

//= ACCESSORS ============================================================================


inline r3dPoint3D& r3dPlane::Normal()
{
    return(m_Normal);
}

inline r3dPoint3D& r3dPlane::Point()
{
    return(m_Point);
}

inline float& r3dPlane::Distance()
{
    return(m_Distance);
}

inline float r3dPlane::SignedDistance( const r3dPoint3D& newPoint )
{

	r3dVector distance = newPoint - Point();
	return m_Normal.Dot( distance );

}
