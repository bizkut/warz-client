#include	"r3dPCH.h"
#include	"r3d.h"

#define B_RIGHT		0
#define B_LEFT		1
#define B_MIDDLE	2
#define B_NUMDIM	3

//inline
int HitBoundingBox(float minB[B_NUMDIM], float maxB[B_NUMDIM], float origin[B_NUMDIM], float dir[B_NUMDIM], float coord[B_NUMDIM])
{
	float 	maxT[B_NUMDIM];
	float 	candidatePlane[B_NUMDIM];
	int 	inside = TRUE;
	int	quadrant[B_NUMDIM];
	int 	whichPlane;
	int 	i;

/* Find candidate planes; this loop can be avoided if
   rays cast all from the eye(assume perpsective view) */
  for(i=0; i<B_NUMDIM; i++) {
    if(origin[i] < minB[i]) {
      quadrant[i] = B_LEFT;
      candidatePlane[i] = minB[i];
      inside = FALSE;
    } else if (origin[i] > maxB[i]) {
      quadrant[i] = B_RIGHT;
      candidatePlane[i] = maxB[i];
      inside = FALSE;
    } else {
      quadrant[i] = B_MIDDLE;
    }
  }

  /* Ray origin inside bounding box */
  if(inside) {
    coord[0] = origin[0];
    coord[1] = origin[1];
    coord[2] = origin[2];
    return TRUE;
  }

  /* Calculate T distances to candidate planes */
  for(i = 0; i < B_NUMDIM; i++) {
    if(quadrant[i] != B_MIDDLE && dir[i] != 0.0f)
      maxT[i] = (candidatePlane[i] - origin[i]) / dir[i];
    else
      maxT[i] = -1.;
  }

  /* Get largest of the maxT's for final choice of intersection */
  whichPlane = 0;
  for(i = 1; i < B_NUMDIM; i++)
    if(maxT[whichPlane] < maxT[i])
      whichPlane = i;

  /* Check final candidate actually inside box */
  if(maxT[whichPlane] < 0.0f)
    return FALSE;

  for(i = 0; i < B_NUMDIM; i++) {
    if(whichPlane != i) {
      coord[i] = origin[i] + maxT[whichPlane] * dir[i];
      if(coord[i] < minB[i] || coord[i] > maxB[i])
	return FALSE;
    } else {
      coord[i] = candidatePlane[i];
    }
  }

  /* ray hits box */
  return TRUE;
}

BOOL r3dBox3D::ContainsRay(const r3dPoint3D& vStart, const r3dPoint3D& vRay, float RayLen, float *dist) const
{
	float		minB[B_NUMDIM];
	float 		maxB[B_NUMDIM];
	float 		origin[B_NUMDIM];
	float 		dir[B_NUMDIM];
	float 		coord[B_NUMDIM];

  minB[0]   = Org.X;
  minB[1]   = Org.Y;
  minB[2]   = Org.Z;
  maxB[0]   = Org.X + Size.X;
  maxB[1]   = Org.Y + Size.Y;
  maxB[2]   = Org.Z + Size.Z;
  origin[0] = vStart.X;
  origin[1] = vStart.Y;
  origin[2] = vStart.Z;
  dir[0]    = vRay.X;
  dir[1]    = vRay.Y;
  dir[2]    = vRay.Z;

  if(HitBoundingBox(minB, maxB, origin, dir, coord) == 0)
    return 0;

	r3dPoint3D coll;
  coll.X = coord[0];
  coll.Y = coord[1];
  coll.Z = coord[2];

  *dist = (coll - vStart).Length();
  if(*dist > RayLen)
    return 0;

  return 1;
}

void
r3dBox3D::InitForExpansion()
{
	Org		= r3dPoint3D( FLT_MAX, FLT_MAX, FLT_MAX );
	Size	= r3dPoint3D( 0, 0, 0 );
}

void
r3dBox3D::ExpandTo( const r3dPoint3D& vP )
{
	if( Org.x == FLT_MAX )
	{
		Org = vP;
	}
	else
	{
		r3dPoint3D newOrg;

		newOrg.x = R3D_MIN( Org.x, vP.x );
		newOrg.y = R3D_MIN( Org.y, vP.y );
		newOrg.z = R3D_MIN( Org.z, vP.z );

		Size += Org - newOrg;

		Org = newOrg;
	}

	Size.x = R3D_MAX( Size.x, vP.x - Org.x );
	Size.y = R3D_MAX( Size.y, vP.y - Org.y );
	Size.z = R3D_MAX( Size.z, vP.z - Org.z );
}

void
r3dBox3D::ExpandTo( const r3dBox3D& box )
{
	ExpandTo( box.Org );
	ExpandTo( box.Org + box.Size );
}


float r3dDistVector3Triangle3_GetSquared(const r3dPoint3D& m_rkVector, r3dPoint3D *m_rkTriangle_V[3])
{
    r3dPoint3D kDiff  = *m_rkTriangle_V[0] - m_rkVector;
    r3dPoint3D kEdge0 = *m_rkTriangle_V[1] - *m_rkTriangle_V[0];
    r3dPoint3D kEdge1 = *m_rkTriangle_V[2] - *m_rkTriangle_V[0];
    float fA00 = kEdge0.x * kEdge0.x + kEdge0.y * kEdge0.y + kEdge0.z * kEdge0.z;
    float fA01 = kEdge0.Dot(kEdge1);
    float fA11 = kEdge1.x * kEdge1.x + kEdge1.y * kEdge1.y + kEdge1.z * kEdge1.z;
    float fB0 = kDiff.Dot(kEdge0);
    float fB1 = kDiff.Dot(kEdge1);
    float fC   = kDiff.x * kDiff.x + kDiff.y * kDiff.y + kDiff.z * kDiff.z;
    float fDet = fabs(fA00*fA11-fA01*fA01);
    float fS = fA01*fB1-fA11*fB0;
    float fT = fA01*fB0-fA00*fB1;
    float fSqrDistance;

    if(fS + fT <= fDet)
    {
        if(fS < 0.0f)
        {
            if(fT < 0.0f)  // region 4
            {
                if(fB0 < 0.0f)
                {
                    fT = 0.0f;
                    if(-fB0 >= fA00)
                    {
                        fS = 1.0f;
                        fSqrDistance = fA00+(2.0f)*fB0+fC;
                    }
                    else
                    {
                        fS = -fB0/fA00;
                        fSqrDistance = fB0*fS+fC;
                    }
                }
                else
                {
                    fS = 0.0f;
                    if(fB1 >= 0.0f)
                    {
                        fT = 0.0f;
                        fSqrDistance = fC;
                    }
                    else if(-fB1 >= fA11)
                    {
                        fT = 1.0f;
                        fSqrDistance = fA11+(2.0f)*fB1+fC;
                    }
                    else
                    {
                        fT = -fB1/fA11;
                        fSqrDistance = fB1*fT+fC;
                    }
                }
            }
            else  // region 3
            {
                fS = 0.0f;
                if(fB1 >= 0.0f)
                {
                    fT = 0.0f;
                    fSqrDistance = fC;
                }
                else if(-fB1 >= fA11)
                {
                    fT = 1.0f;
                    fSqrDistance = fA11+(2.0f)*fB1+fC;
                }
                else
                {
                    fT = -fB1/fA11;
                    fSqrDistance = fB1*fT+fC;
                }
            }
        }
        else if(fT < 0.0f)  // region 5
        {
            fT = 0.0f;
            if(fB0 >= 0.0f)
            {
                fS = 0.0f;
                fSqrDistance = fC;
            }
            else if(-fB0 >= fA00)
            {
                fS = 1.0f;
                fSqrDistance = fA00+(2.0f)*fB0+fC;
            }
            else
            {
                fS = -fB0/fA00;
                fSqrDistance = fB0*fS+fC;
            }
        }
        else  // region 0
        {
            // minimum at interior point
            float fInvDet = (1.0f)/fDet;
            fS *= fInvDet;
            fT *= fInvDet;
            fSqrDistance = fS*(fA00*fS+fA01*fT+(2.0f)*fB0) +
                fT*(fA01*fS+fA11*fT+(2.0f)*fB1)+fC;
        }
    }
    else
    {
        float fTmp0, fTmp1, fNumer, fDenom;

        if(fS < 0.0f)  // region 2
        {
            fTmp0 = fA01 + fB0;
            fTmp1 = fA11 + fB1;
            if(fTmp1 > fTmp0)
            {
                fNumer = fTmp1 - fTmp0;
                fDenom = fA00-2.0f*fA01+fA11;
                if(fNumer >= fDenom)
                {
                    fS = 1.0f;
                    fT = 0.0f;
                    fSqrDistance = fA00+(2.0f)*fB0+fC;
                }
                else
                {
                    fS = fNumer/fDenom;
                    fT = 1.0f - fS;
                    fSqrDistance = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
                        fT*(fA01*fS+fA11*fT+(2.0f)*fB1)+fC;
                }
            }
            else
            {
                fS = 0.0f;
                if(fTmp1 <= 0.0f)
                {
                    fT = 1.0f;
                    fSqrDistance = fA11+(2.0f)*fB1+fC;
                }
                else if(fB1 >= 0.0f)
                {
                    fT = 0.0f;
                    fSqrDistance = fC;
                }
                else
                {
                    fT = -fB1/fA11;
                    fSqrDistance = fB1*fT+fC;
                }
            }
        }
        else if(fT < 0.0f)  // region 6
        {
            fTmp0 = fA01 + fB1;
            fTmp1 = fA00 + fB0;
            if(fTmp1 > fTmp0)
            {
                fNumer = fTmp1 - fTmp0;
                fDenom = fA00-(2.0f)*fA01+fA11;
                if(fNumer >= fDenom)
                {
                    fT = 1.0f;
                    fS = 0.0f;
                    fSqrDistance = fA11+(2.0f)*fB1+fC;
                }
                else
                {
                    fT = fNumer/fDenom;
                    fS = 1.0f - fT;
                    fSqrDistance = fS*(fA00*fS+fA01*fT+(2.0f)*fB0) +
                        fT*(fA01*fS+fA11*fT+(2.0f)*fB1)+fC;
                }
            }
            else
            {
                fT = 0.0f;
                if(fTmp1 <= 0.0f)
                {
                    fS = 1.0f;
                    fSqrDistance = fA00+(2.0f)*fB0+fC;
                }
                else if(fB0 >= 0.0f)
                {
                    fS = 0.0f;
                    fSqrDistance = fC;
                }
                else
                {
                    fS = -fB0/fA00;
                    fSqrDistance = fB0*fS+fC;
                }
            }
        }
        else  // region 1
        {
            fNumer = fA11 + fB1 - fA01 - fB0;
            if(fNumer <= 0.0f)
            {
                fS = 0.0f;
                fT = 1.0f;
                fSqrDistance = fA11+(2.0f)*fB1+fC;
            }
            else
            {
                fDenom = fA00-2.0f*fA01+fA11;
                if(fNumer >= fDenom)
                {
                    fS = 1.0f;
                    fT = 0.0f;
                    fSqrDistance = fA00+(2.0f)*fB0+fC;
                }
                else
                {
                    fS = fNumer/fDenom;
                    fT = 1.0f - fS;
                    fSqrDistance = fS*(fA00*fS+fA01*fT+(2.0f)*fB0) +
                        fT*(fA01*fS+fA11*fT+(2.0f)*fB1)+fC;
                }
            }
        }
    }

    // account for numerical round-off error
    if(fSqrDistance < 0.0f)
        fSqrDistance = 0.0f;

    //m_kClosestPoint0 = m_rkVector;
    //m_kClosestPoint1 = m_rkTriangle_V[0] + fS*kEdge0 + fT*kEdge1;

    return fSqrDistance;
}


float r3dDistSegment3Segment3_GetSquared(r3dSegment3 &m_rkSegment0, r3dSegment3 &m_rkSegment1)
{
    r3dPoint3D kDiff = m_rkSegment0.Origin - m_rkSegment1.Origin;
    float fA01 = -m_rkSegment0.Direction.Dot(m_rkSegment1.Direction);
    float fB0 = kDiff.Dot(m_rkSegment0.Direction);
    float fB1 = -kDiff.Dot(m_rkSegment1.Direction);
    float fC = (kDiff.x * kDiff.x + kDiff.y * kDiff.y + kDiff.z * kDiff.z);
    float fDet = fabs(1.0f - fA01*fA01);
    float fS0, fS1, fSqrDist, fExtDet0, fExtDet1, fTmpS0, fTmpS1;

#define ZERO_TOLERANCE 0.00001f
    if (fDet >= ZERO_TOLERANCE)
    {
        // segments are not parallel
        fS0 = fA01*fB1-fB0;
        fS1 = fA01*fB0-fB1;
        fExtDet0 = m_rkSegment0.Extent*fDet;
        fExtDet1 = m_rkSegment1.Extent*fDet;

        if (fS0 >= -fExtDet0)
        {
            if (fS0 <= fExtDet0)
            {
                if (fS1 >= -fExtDet1)
                {
                    if (fS1 <= fExtDet1)  // region 0 (interior)
                    {
                        // minimum at two interior points of 3D lines
                        float fInvDet = (1.0f)/fDet;
                        fS0 *= fInvDet;
                        fS1 *= fInvDet;
                        fSqrDist = fS0*(fS0+fA01*fS1+(2.0f)*fB0) +
                            fS1*(fA01*fS0+fS1+(2.0f)*fB1)+fC;
                    }
                    else  // region 3 (side)
                    {
                        fS1 = m_rkSegment1.Extent;
                        fTmpS0 = -(fA01*fS1+fB0);
                        if (fTmpS0 < -m_rkSegment0.Extent)
                        {
                            fS0 = -m_rkSegment0.Extent;
                            fSqrDist = fS0*(fS0-(2.0f)*fTmpS0) +
                                fS1*(fS1+(2.0f)*fB1)+fC;
                        }
                        else if (fTmpS0 <= m_rkSegment0.Extent)
                        {
                            fS0 = fTmpS0;
                            fSqrDist = -fS0*fS0+fS1*(fS1+(2.0f)*fB1)+fC;
                        }
                        else
                        {
                            fS0 = m_rkSegment0.Extent;
                            fSqrDist = fS0*(fS0-(2.0f)*fTmpS0) +
                                fS1*(fS1+(2.0f)*fB1)+fC;
                        }
                    }
                }
                else  // region 7 (side)
                {
                    fS1 = -m_rkSegment1.Extent;
                    fTmpS0 = -(fA01*fS1+fB0);
                    if (fTmpS0 < -m_rkSegment0.Extent)
                    {
                        fS0 = -m_rkSegment0.Extent;
                        fSqrDist = fS0*(fS0-(2.0f)*fTmpS0) +
                            fS1*(fS1+(2.0f)*fB1)+fC;
                    }
                    else if (fTmpS0 <= m_rkSegment0.Extent)
                    {
                        fS0 = fTmpS0;
                        fSqrDist = -fS0*fS0+fS1*(fS1+(2.0f)*fB1)+fC;
                    }
                    else
                    {
                        fS0 = m_rkSegment0.Extent;
                        fSqrDist = fS0*(fS0-(2.0f)*fTmpS0) +
                            fS1*(fS1+(2.0f)*fB1)+fC;
                    }
                }
            }
            else
            {
                if (fS1 >= -fExtDet1)
                {
                    if (fS1 <= fExtDet1)  // region 1 (side)
                    {
                        fS0 = m_rkSegment0.Extent;
                        fTmpS1 = -(fA01*fS0+fB1);
                        if (fTmpS1 < -m_rkSegment1.Extent)
                        {
                            fS1 = -m_rkSegment1.Extent;
                            fSqrDist = fS1*(fS1-(2.0f)*fTmpS1) +
                                fS0*(fS0+(2.0f)*fB0)+fC;
                        }
                        else if (fTmpS1 <= m_rkSegment1.Extent)
                        {
                            fS1 = fTmpS1;
                            fSqrDist = -fS1*fS1+fS0*(fS0+(2.0f)*fB0)+fC;
                        }
                        else
                        {
                            fS1 = m_rkSegment1.Extent;
                            fSqrDist = fS1*(fS1-(2.0f)*fTmpS1) +
                                fS0*(fS0+(2.0f)*fB0)+fC;
                        }
                    }
                    else  // region 2 (corner)
                    {
                        fS1 = m_rkSegment1.Extent;
                        fTmpS0 = -(fA01*fS1+fB0);
                        if (fTmpS0 < -m_rkSegment0.Extent)
                        {
                            fS0 = -m_rkSegment0.Extent;
                            fSqrDist = fS0*(fS0-(2.0f)*fTmpS0) +
                                fS1*(fS1+(2.0f)*fB1)+fC;
                        }
                        else if (fTmpS0 <= m_rkSegment0.Extent)
                        {
                            fS0 = fTmpS0;
                            fSqrDist = -fS0*fS0+fS1*(fS1+(2.0f)*fB1)+fC;
                        }
                        else
                        {
                            fS0 = m_rkSegment0.Extent;
                            fTmpS1 = -(fA01*fS0+fB1);
                            if (fTmpS1 < -m_rkSegment1.Extent)
                            {
                                fS1 = -m_rkSegment1.Extent;
                                fSqrDist = fS1*(fS1-(2.0f)*fTmpS1) +
                                    fS0*(fS0+(2.0f)*fB0)+fC;
                            }
                            else if (fTmpS1 <= m_rkSegment1.Extent)
                            {
                                fS1 = fTmpS1;
                                fSqrDist = -fS1*fS1+fS0*(fS0+(2.0f)*fB0)
                                    + fC;
                            }
                            else
                            {
                                fS1 = m_rkSegment1.Extent;
                                fSqrDist = fS1*(fS1-(2.0f)*fTmpS1) +
                                    fS0*(fS0+(2.0f)*fB0)+fC;
                            }
                        }
                    }
                }
                else  // region 8 (corner)
                {
                    fS1 = -m_rkSegment1.Extent;
                    fTmpS0 = -(fA01*fS1+fB0);
                    if (fTmpS0 < -m_rkSegment0.Extent)
                    {
                        fS0 = -m_rkSegment0.Extent;
                        fSqrDist = fS0*(fS0-(2.0f)*fTmpS0) +
                            fS1*(fS1+(2.0f)*fB1)+fC;
                    }
                    else if (fTmpS0 <= m_rkSegment0.Extent)
                    {
                        fS0 = fTmpS0;
                        fSqrDist = -fS0*fS0+fS1*(fS1+(2.0f)*fB1)+fC;
                    }
                    else
                    {
                        fS0 = m_rkSegment0.Extent;
                        fTmpS1 = -(fA01*fS0+fB1);
                        if (fTmpS1 > m_rkSegment1.Extent)
                        {
                            fS1 = m_rkSegment1.Extent;
                            fSqrDist = fS1*(fS1-(2.0f)*fTmpS1) +
                                fS0*(fS0+(2.0f)*fB0)+fC;
                        }
                        else if (fTmpS1 >= -m_rkSegment1.Extent)
                        {
                            fS1 = fTmpS1;
                            fSqrDist = -fS1*fS1+fS0*(fS0+(2.0f)*fB0)
                                + fC;
                        }
                        else
                        {
                            fS1 = -m_rkSegment1.Extent;
                            fSqrDist = fS1*(fS1-(2.0f)*fTmpS1) +
                                fS0*(fS0+(2.0f)*fB0)+fC;
                        }
                    }
                }
            }
        }
        else 
        {
            if (fS1 >= -fExtDet1)
            {
                if (fS1 <= fExtDet1)  // region 5 (side)
                {
                    fS0 = -m_rkSegment0.Extent;
                    fTmpS1 = -(fA01*fS0+fB1);
                    if (fTmpS1 < -m_rkSegment1.Extent)
                    {
                        fS1 = -m_rkSegment1.Extent;
                        fSqrDist = fS1*(fS1-(2.0f)*fTmpS1) +
                            fS0*(fS0+(2.0f)*fB0)+fC;
                    }
                    else if (fTmpS1 <= m_rkSegment1.Extent)
                    {
                        fS1 = fTmpS1;
                        fSqrDist = -fS1*fS1+fS0*(fS0+(2.0f)*fB0)+fC;
                    }
                    else
                    {
                        fS1 = m_rkSegment1.Extent;
                        fSqrDist = fS1*(fS1-(2.0f)*fTmpS1) +
                            fS0*(fS0+(2.0f)*fB0)+fC;
                    }
                }
                else  // region 4 (corner)
                {
                    fS1 = m_rkSegment1.Extent;
                    fTmpS0 = -(fA01*fS1+fB0);
                    if (fTmpS0 > m_rkSegment0.Extent)
                    {
                        fS0 = m_rkSegment0.Extent;
                        fSqrDist = fS0*(fS0-(2.0f)*fTmpS0) +
                            fS1*(fS1+(2.0f)*fB1)+fC;
                    }
                    else if (fTmpS0 >= -m_rkSegment0.Extent)
                    {
                        fS0 = fTmpS0;
                        fSqrDist = -fS0*fS0+fS1*(fS1+(2.0f)*fB1)+fC;
                    }
                    else
                    {
                        fS0 = -m_rkSegment0.Extent;
                        fTmpS1 = -(fA01*fS0+fB1);
                        if (fTmpS1 < -m_rkSegment1.Extent)
                        {
                            fS1 = -m_rkSegment1.Extent;
                            fSqrDist = fS1*(fS1-(2.0f)*fTmpS1) +
                                fS0*(fS0+(2.0f)*fB0)+fC;
                        }
                        else if (fTmpS1 <= m_rkSegment1.Extent)
                        {
                            fS1 = fTmpS1;
                            fSqrDist = -fS1*fS1+fS0*(fS0+(2.0f)*fB0)
                                + fC;
                        }
                        else
                        {
                            fS1 = m_rkSegment1.Extent;
                            fSqrDist = fS1*(fS1-(2.0f)*fTmpS1) +
                                fS0*(fS0+(2.0f)*fB0)+fC;
                        }
                    }
                }
            }
            else   // region 6 (corner)
            {
                fS1 = -m_rkSegment1.Extent;
                fTmpS0 = -(fA01*fS1+fB0);
                if (fTmpS0 > m_rkSegment0.Extent)
                {
                    fS0 = m_rkSegment0.Extent;
                    fSqrDist = fS0*(fS0-(2.0f)*fTmpS0) +
                        fS1*(fS1+(2.0f)*fB1)+fC;
                }
                else if (fTmpS0 >= -m_rkSegment0.Extent)
                {
                    fS0 = fTmpS0;
                    fSqrDist = -fS0*fS0+fS1*(fS1+(2.0f)*fB1)+fC;
                }
                else
                {
                    fS0 = -m_rkSegment0.Extent;
                    fTmpS1 = -(fA01*fS0+fB1);
                    if (fTmpS1 < -m_rkSegment1.Extent)
                    {
                        fS1 = -m_rkSegment1.Extent;
                        fSqrDist = fS1*(fS1-(2.0f)*fTmpS1) +
                            fS0*(fS0+(2.0f)*fB0)+fC;
                    }
                    else if (fTmpS1 <= m_rkSegment1.Extent)
                    {
                        fS1 = fTmpS1;
                        fSqrDist = -fS1*fS1+fS0*(fS0+(2.0f)*fB0)
                            + fC;
                    }
                    else
                    {
                        fS1 = m_rkSegment1.Extent;
                        fSqrDist = fS1*(fS1-(2.0f)*fTmpS1) +
                            fS0*(fS0+(2.0f)*fB0)+fC;
                    }
                }
            }
        }
    }
    else
    {
        // segments are parallel
        float fE0pE1 = m_rkSegment0.Extent + m_rkSegment1.Extent;
        float fSign = (fA01 > 0.0f ? -1.0f : 1.0f);
        float fLambda = -fB0;
        if (fLambda < -fE0pE1)
        {
            fLambda = -fE0pE1;
        }
        else if (fLambda > fE0pE1)
        {
            fLambda = fE0pE1;
        }

        fS1 = fSign*fB0*m_rkSegment1.Extent/fE0pE1;
        fS0 = fLambda + fSign*fS1;
        fSqrDist = fLambda*(fLambda + (2.0f)*fB0) + fC;
    }

    //r3dPoint3D m_kClosestPoint0 = m_rkSegment0.Origin + fS0*m_rkSegment0.Direction;
    //r3dPoint3D m_kClosestPoint1 = m_rkSegment1.Origin + fS1*m_rkSegment1.Direction;
    return fabs(fSqrDist);
}


float r3dDistSegment3Segment3_GetSquared1(r3dSegment3 &m_rkSegment0, r3dSegment3 &m_rkSegment1)
{
    r3dPoint3D kDiff = m_rkSegment0.Origin - m_rkSegment1.Origin;
    float fA01 = -m_rkSegment0.Direction.Dot(m_rkSegment1.Direction);
    float fB0 = kDiff.Dot(m_rkSegment0.Direction);
    float fB1 = -kDiff.Dot(m_rkSegment1.Direction);
    float fC  = (kDiff.x * kDiff.x + kDiff.y * kDiff.y + kDiff.z * kDiff.z);
    float fDet = fabs(1.0f - fA01*fA01);
    float fS0, fS1, fSqrDist = 99999999.0f, fExtDet0, fExtDet1, fTmpS0, fTmpS1;

#define ZERO_TOLERANCE 0.00001f
    if(fDet >= ZERO_TOLERANCE)
    {
        // segments are not parallel
        fS0 = fA01*fB1-fB0;
        fS1 = fA01*fB0-fB1;
        fExtDet0 = m_rkSegment0.Extent*fDet;
        fExtDet1 = m_rkSegment1.Extent*fDet;

        if(fS0 >= -fExtDet0)
        {
            if(fS0 <= fExtDet0)
            {
                if(fS1 >= -fExtDet1)
                {
                    if(fS1 <= fExtDet1)  // region 0 (interior)
                    {
                        // minimum at two interior points of 3D lines
                        float fInvDet = (1.0f)/fDet;
                        fS0 *= fInvDet;
                        fS1 *= fInvDet;
                        fSqrDist = fS0*(fS0+fA01*fS1+(2.0f)*fB0) +
                            fS1*(fA01*fS0+fS1+(2.0f)*fB1)+fC;
                    }
                    else  // region 3 (side)
                    {
                        fS1 = m_rkSegment1.Extent;
                        fTmpS0 = -(fA01*fS1+fB0);
                        if(fTmpS0 < -m_rkSegment0.Extent)
                        {
                            fS0 = -m_rkSegment0.Extent;
                            fSqrDist = fS0*(fS0-(2.0f)*fTmpS0) +
                                fS1*(fS1+(2.0f)*fB1)+fC;
                        }
                        else if(fTmpS0 <= m_rkSegment0.Extent)
                        {
                            fS0 = fTmpS0;
                            fSqrDist = -fS0*fS0+fS1*(fS1+(2.0f)*fB1)+fC;
                        }
                        else
                        {
                            fS0 = m_rkSegment0.Extent;
                            fSqrDist = fS0*(fS0-(2.0f)*fTmpS0) +
                                fS1*(fS1+(2.0f)*fB1)+fC;
                        }
                    }
                }
                else  // region 7 (side)
                {
                    fS1 = -m_rkSegment1.Extent;
                    fTmpS0 = -(fA01*fS1+fB0);
                    if(fTmpS0 < -m_rkSegment0.Extent)
                    {
                        fS0 = -m_rkSegment0.Extent;
                        fSqrDist = fS0*(fS0-(2.0f)*fTmpS0) +
                            fS1*(fS1+(2.0f)*fB1)+fC;
                    }
                    else if(fTmpS0 <= m_rkSegment0.Extent)
                    {
                        fS0 = fTmpS0;
                        fSqrDist = -fS0*fS0+fS1*(fS1+(2.0f)*fB1)+fC;
                    }
                    else
                    {
                        fS0 = m_rkSegment0.Extent;
                        fSqrDist = fS0*(fS0-(2.0f)*fTmpS0) +
                            fS1*(fS1+(2.0f)*fB1)+fC;
                    }
                }
            }
            else
            {
                if(fS1 >= -fExtDet1)
                {
                    if(fS1 <= fExtDet1)  // region 1 (side)
                    {
                        fS0 = m_rkSegment0.Extent;
                        fTmpS1 = -(fA01*fS0+fB1);
                        if(fTmpS1 < -m_rkSegment1.Extent)
                        {
                            fS1 = -m_rkSegment1.Extent;
                            fSqrDist = fS1*(fS1-(2.0f)*fTmpS1) +
                                fS0*(fS0+(2.0f)*fB0)+fC;
                        }
                        else if(fTmpS1 <= m_rkSegment1.Extent)
                        {
                            fS1 = fTmpS1;
                            fSqrDist = -fS1*fS1+fS0*(fS0+(2.0f)*fB0)+fC;
                        }
                        else
                        {
                            fS1 = m_rkSegment1.Extent;
                            fSqrDist = fS1*(fS1-(2.0f)*fTmpS1) +
                                fS0*(fS0+(2.0f)*fB0)+fC;
                        }
                    }
                    else  // region 2 (corner)
                    {
                        fS1 = m_rkSegment1.Extent;
                        fTmpS0 = -(fA01*fS1+fB0);
                        if(fTmpS0 < -m_rkSegment0.Extent)
                        {
                            fS0 = -m_rkSegment0.Extent;
                            fSqrDist = fS0*(fS0-(2.0f)*fTmpS0) +
                                fS1*(fS1+(2.0f)*fB1)+fC;
                        }
                        else if(fTmpS0 <= m_rkSegment0.Extent)
                        {
                            fS0 = fTmpS0;
                            fSqrDist = -fS0*fS0+fS1*(fS1+(2.0f)*fB1)+fC;
                        }
                        else
                        {
                            fS0 = m_rkSegment0.Extent;
                            fTmpS1 = -(fA01*fS0+fB1);
                            if(fTmpS1 < -m_rkSegment1.Extent)
                            {
                                fS1 = -m_rkSegment1.Extent;
                                fSqrDist = fS1*(fS1-(2.0f)*fTmpS1) +
                                    fS0*(fS0+(2.0f)*fB0)+fC;
                            }
                            else if(fTmpS1 <= m_rkSegment1.Extent)
                            {
                                fS1 = fTmpS1;
                                fSqrDist = -fS1*fS1+fS0*(fS0+(2.0f)*fB0)
                                    + fC;
                            }
                            else
                            {
                                fS1 = m_rkSegment1.Extent;
                                fSqrDist = fS1*(fS1-(2.0f)*fTmpS1) +
                                    fS0*(fS0+(2.0f)*fB0)+fC;
                            }
                        }
                    }
                }
                else  // region 8 (corner)
                {
                    fS1 = -m_rkSegment1.Extent;
                    fTmpS0 = -(fA01*fS1+fB0);
                    if(fTmpS0 < -m_rkSegment0.Extent)
                    {
                        fS0 = -m_rkSegment0.Extent;
                        fSqrDist = fS0*(fS0-(2.0f)*fTmpS0) +
                            fS1*(fS1+(2.0f)*fB1)+fC;
                    }
                    else if(fTmpS0 <= m_rkSegment0.Extent)
                    {
                        fS0 = fTmpS0;
                        fSqrDist = -fS0*fS0+fS1*(fS1+(2.0f)*fB1)+fC;
                    }
                    else
                    {
                        fS0 = m_rkSegment0.Extent;
                        fTmpS1 = -(fA01*fS0+fB1);
                        if(fTmpS1 > m_rkSegment1.Extent)
                        {
                            fS1 = m_rkSegment1.Extent;
                            fSqrDist = fS1*(fS1-(2.0f)*fTmpS1) +
                                fS0*(fS0+(2.0f)*fB0)+fC;
                        }
                        else if(fTmpS1 >= -m_rkSegment1.Extent)
                        {
                            fS1 = fTmpS1;
                            fSqrDist = -fS1*fS1+fS0*(fS0+(2.0f)*fB0)
                                + fC;
                        }
                        else
                        {
                            fS1 = -m_rkSegment1.Extent;
                            fSqrDist = fS1*(fS1-(2.0f)*fTmpS1) +
                                fS0*(fS0+(2.0f)*fB0)+fC;
                        }
                    }
                }
            }
        }
        else 
        {
            if(fS1 >= -fExtDet1)
            {
                if(fS1 <= fExtDet1)  // region 5 (side)
                {
                    fS0 = -m_rkSegment0.Extent;
                    fTmpS1 = -(fA01*fS0+fB1);
                    if(fTmpS1 < -m_rkSegment1.Extent)
                    {
                        fS1 = -m_rkSegment1.Extent;
                        fSqrDist = fS1*(fS1-(2.0f)*fTmpS1) +
                            fS0*(fS0+(2.0f)*fB0)+fC;
                    }
                    else if(fTmpS1 <= m_rkSegment1.Extent)
                    {
                        fS1 = fTmpS1;
                        fSqrDist = -fS1*fS1+fS0*(fS0+(2.0f)*fB0)+fC;
                    }
                    else
                    {
                        fS1 = m_rkSegment1.Extent;
                        fSqrDist = fS1*(fS1-(2.0f)*fTmpS1) +
                            fS0*(fS0+(2.0f)*fB0)+fC;
                    }
                }
                else  // region 4 (corner)
                {
                    fS1 = m_rkSegment1.Extent;
                    fTmpS0 = -(fA01*fS1+fB0);
                    if(fTmpS0 > m_rkSegment0.Extent)
                    {
                        fS0 = m_rkSegment0.Extent;
                        fSqrDist = fS0*(fS0-(2.0f)*fTmpS0) +
                            fS1*(fS1+(2.0f)*fB1)+fC;
                    }
                    else if(fTmpS0 >= -m_rkSegment0.Extent)
                    {
                        fS0 = fTmpS0;
                        fSqrDist = -fS0*fS0+fS1*(fS1+(2.0f)*fB1)+fC;
                    }
                    else
                    {
                        fS0 = -m_rkSegment0.Extent;
                        fTmpS1 = -(fA01*fS0+fB1);
                        if(fTmpS1 < -m_rkSegment1.Extent)
                        {
                            fS1 = -m_rkSegment1.Extent;
                            fSqrDist = fS1*(fS1-(2.0f)*fTmpS1) +
                                fS0*(fS0+(2.0f)*fB0)+fC;
                        }
                        else if(fTmpS1 <= m_rkSegment1.Extent)
                        {
                            fS1 = fTmpS1;
                            fSqrDist = -fS1*fS1+fS0*(fS0+(2.0f)*fB0)
                                + fC;
                        }
                        else
                        {
                            fS1 = m_rkSegment1.Extent;
                            fSqrDist = fS1*(fS1-(2.0f)*fTmpS1) +
                                fS0*(fS0+(2.0f)*fB0)+fC;
                        }
                    }
                }
            }
            else   // region 6 (corner)
            {
                fS1 = -m_rkSegment1.Extent;
                fTmpS0 = -(fA01*fS1+fB0);
                if(fTmpS0 > m_rkSegment0.Extent)
                {
                    fS0 = m_rkSegment0.Extent;
                    fSqrDist = fS0*(fS0-(2.0f)*fTmpS0) +
                        fS1*(fS1+(2.0f)*fB1)+fC;
                }
                else if(fTmpS0 >= -m_rkSegment0.Extent)
                {
                    fS0 = fTmpS0;
                    fSqrDist = -fS0*fS0+fS1*(fS1+(2.0f)*fB1)+fC;
                }
                else
                {
                    fS0 = -m_rkSegment0.Extent;
                    fTmpS1 = -(fA01*fS0+fB1);
                    if(fTmpS1 < -m_rkSegment1.Extent)
                    {
                        fS1 = -m_rkSegment1.Extent;
                        fSqrDist = fS1*(fS1-(2.0f)*fTmpS1) +
                            fS0*(fS0+(2.0f)*fB0)+fC;
                    }
                    else if(fTmpS1 <= m_rkSegment1.Extent)
                    {
                        fS1 = fTmpS1;
                        fSqrDist = -fS1*fS1+fS0*(fS0+(2.0f)*fB0)
                            + fC;
                    }
                    else
                    {
                        fS1 = m_rkSegment1.Extent;
                        fSqrDist = fS1*(fS1-(2.0f)*fTmpS1) +
                            fS0*(fS0+(2.0f)*fB0)+fC;
                    }
                }
            }
        }
    }
    else
    {
        // segments are parallel
        float fE0pE1 = m_rkSegment0.Extent + m_rkSegment1.Extent;
        float fSign = (fA01 > 0.0f ? -1.0f : 1.0f);
        float fLambda = -fB0;
        if(fLambda < -fE0pE1)
            fLambda = -fE0pE1;
        else if(fLambda > fE0pE1)
            fLambda = fE0pE1;

        fS1 = fSign*fB0*m_rkSegment1.Extent/fE0pE1;
        fS0 = fLambda + fSign*fS1;
        fSqrDist = fLambda*(fLambda + (2.0f)*fB0) + fC;
    }

    r3dPoint3D m_kClosestPoint0 = m_rkSegment0.Origin + fS0*m_rkSegment0.Direction;
    r3dPoint3D m_kClosestPoint1 = m_rkSegment1.Origin + fS1*m_rkSegment1.Direction;
    return fabs(fSqrDist);
}

float r3dDistSegment3Triangle3_GetSquared(r3dSegment3 &m_rkSegment, r3dPoint3D *m_rkTriangle_V[3])
{    
    // compare segment to all three edges of the triangle
    float fSqrDist = 99999999999.9f, fSqrDistTmp;
    for (int i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
    {
        r3dSegment3 kEdge;
        kEdge.Origin = (0.5f)*(*m_rkTriangle_V[i0] + *m_rkTriangle_V[i1]);
        kEdge.Direction = *m_rkTriangle_V[i1] - *m_rkTriangle_V[i0];
        kEdge.Extent = (0.5f)*kEdge.Direction.Length();
        kEdge.Direction.Normalize();
        fSqrDistTmp = r3dDistSegment3Segment3_GetSquared(m_rkSegment, kEdge);
        if(fSqrDistTmp < fSqrDist)
        {
            //m_kClosestPoint0 = kSSDist.GetClosestPoint0();
            //m_kClosestPoint1 = kSSDist.GetClosestPoint1();
            fSqrDist = fSqrDistTmp;
        }
    }

    // compare segment end points to triangle interior
    r3dPoint3D kPosEnd = m_rkSegment.GetPosEnd();
    fSqrDistTmp = r3dDistVector3Triangle3_GetSquared(kPosEnd, m_rkTriangle_V);
    if(fSqrDistTmp < fSqrDist)
    {
        //m_kClosestPoint0 = kPosDist.GetClosestPoint0();
        //m_kClosestPoint1 = kPosDist.GetClosestPoint1();
        fSqrDist = fSqrDistTmp;
    }

    r3dPoint3D kNegEnd = m_rkSegment.GetNegEnd();
    fSqrDistTmp = r3dDistVector3Triangle3_GetSquared(kNegEnd, m_rkTriangle_V);
    if(fSqrDistTmp < fSqrDist)
    {
        //m_kClosestPoint0 = kNegDist.GetClosestPoint0();
        //m_kClosestPoint1 = kNegDist.GetClosestPoint1();
        fSqrDist = fSqrDistTmp;
    }

    return fSqrDist;
}


float r3dDistPoint3DSegment3D(const r3dPoint3D& p, const r3dPoint3D& s1, const r3dPoint3D& s2)
{
  const r3dPoint3D v = s2 - s1;	// segment direction

  float c1 = (p - s1).Dot(v);
  if(c1 < 0) return -1;

  float c2 = v.Dot(v);
  if(c2 <= c1) return -1;

  float t = c1 / c2;
  r3dPoint3D Pb = s1 + t * v;
  return (p - Pb).Length();
}

float r3dDistPoint3DLine3D(const r3dPoint3D& p, const r3dPoint3D& s1, const r3dPoint3D& s2)
{
  const r3dPoint3D v = s2 - s1;	// segment direction

  float c1 = (p - s1).Dot(v);
  float c2 = v.Dot(v);

  float t = c1 / c2;
  r3dPoint3D Pb = s1 + t * v;
  return (p - Pb).Length();
}


float r3dDistancePointLine(const r3dPoint3D& p, const r3dPoint3D& l1, const r3dPoint3D& l2, float* dist_on_line)
{
  float LineMag = (l2 - l1).Length();
  float U       = (
		((p.X - l1.X) * (l2.X - l1.X)) +
		((p.Y - l1.Y) * (l2.Y - l1.Y)) +
		((p.Z - l1.Z) * (l2.Z - l1.Z))) /
		(LineMag * LineMag);

  //point of intersection
  r3dPoint3D v    = (l1 + U * (l2-l1));
  float      dist = (p-v).Length();

  if(dist_on_line) *dist_on_line = U;

  return dist;
}

float r3dDistancePointSegment(const r3dPoint3D& p, const r3dPoint3D& l1, const r3dPoint3D& l2)
{
  float U;
  float dist = r3dDistancePointLine(p, l1, l2, &U);

  if(U < 0.0f || U > 1.0f)
    return -1;   // negative value means, which will indicate that point does not fall into
    
  return dist;
}




//--------------------------------------------------------------------------------------------------------
void r3dBox3D::Transform( const r3dMatrix * pM )
{
	r3dPoint3D dPoints[8];

	r3dPoint3D vMin;
	r3dPoint3D vMax;

	vMin = r3dPoint3D( FLT_MAX, FLT_MAX, FLT_MAX );
	vMax = r3dPoint3D( -FLT_MAX, -FLT_MAX, -FLT_MAX );

	
	dPoints[ 0 ] = r3dPoint3D( Org.x, Org.y, Org.z ), 
	dPoints[ 1 ] = r3dPoint3D( Org.x, Org.y + Size.y, Org.z );
	dPoints[ 2 ] = r3dPoint3D( Org.x, Org.y + Size.y, Org.z + Size.z );
	dPoints[ 3 ] = r3dPoint3D( Org.x, Org.y, Org.z + Size.z );

	dPoints[ 4 ] = r3dPoint3D( Org.x + Size.x, Org.y, Org.z );
	dPoints[ 5 ] = r3dPoint3D( Org.x + Size.x, Org.y + Size.y, Org.z );
	dPoints[ 6 ] = r3dPoint3D( Org.x + Size.x, Org.y + Size.y, Org.z + Size.z );
	dPoints[ 7 ] = r3dPoint3D( Org.x + Size.x, Org.y, Org.z + Size.z );

	for ( int i = 0; i < 8; i++ )
	{
		r3dPoint3D vPos;
		D3DXVec3TransformCoord( ( D3DXVECTOR3 * ) &vPos, ( D3DXVECTOR3 * ) &dPoints[ i ], ( D3DXMATRIX *) pM );

		vMin = r3dPoint3D( R3D_MIN( vMin.x, vPos.x ), R3D_MIN( vMin.y, vPos.y ), R3D_MIN( vMin.z, vPos.z ) );
		vMax = r3dPoint3D( R3D_MAX( vMax.x, vPos.x ), R3D_MAX( vMax.y, vPos.y ), R3D_MAX( vMax.z, vPos.z ) );
	}

	Org = vMin;
	Size = vMax - Org;
}