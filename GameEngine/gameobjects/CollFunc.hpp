/*BOOL CheckCylinderCollision(INARG  r3dVector &vStart,
			    INARG  r3dVector &vMove,
			    INARG  float Radius,
			    INARG  float Height,
			    INARG  r3dMesh *Cell,
			    OUTARG CollisionInfo &Info)
{

  // switch to transformed object geometry, if present
 // if(Cell->m_pTransformedObj) 
 //   Cell = Cell->m_pTransformedObj;

  static r3dVector vCylAxle(0, 1, 0);

  float MoveLength  = vMove.Length();
  
  Info.Type	    = CLT_Nothing;
  Info.pMeshObj     = Cell;
  Info.ClipCoef     = 1.00001f;			// hack
  Info.Distance     = MoveLength;
  Info.NewPosition  = vStart;
  if(MoveLength == 0)
    return 0;

  // SUPER HACK. extend move range
  // thats used for correct collide if we very near object
  vStart           -= vMove.NormalizeTo() * 0.25f;
  vMove            += vMove.NormalizeTo() * 0.5f;
  MoveLength       += 0.5f;

  //---------------------------------------------------------------------
  // STAGE 1: Initialization of internal variables
  //---------------------------------------------------------------------
  r3dVector vEnd      = vStart + vMove;
  r3dVector vBotStart = vStart;
  r3dVector vTopStart = vStart + vCylAxle * Height;

  r3dVector vDir      = vMove * (1.0f / MoveLength);
  r3dVector vCylUp    = vCylAxle;
  r3dVector vCylRight = vCylUp.Cross(vDir);

  BOOL      bVerticalMoving   = R3D_ABS(vDir.Y) > 1 - R3D_EPSILON;
  BOOL      bHorizontalMoving = R3D_ABS(vDir.Y) < R3D_EPSILON;

  if(bVerticalMoving) vCylRight.Assign(1, 0, 0);
  vCylRight.Normalize();

  r3dVector vCylForw  = vCylRight.Cross(vCylUp);
  r3dVector vMoveUp   = vDir.Cross(vCylRight);

  float     vMoveAngle= vDir.Dot(vCylForw);
  float     Radius2   = Radius * Radius;
  float     dist;

  float     A1 = vStart.Z - vEnd.Z,
	    B1 = vEnd.X - vStart.X,
	    C1 = vStart.X*vEnd.Z - vEnd.X*vStart.Z;

  //
  // calc bounding box of entire move.
  //
  r3dVector	vStart1, vEnd1;
  r3dBoundBox	Move_BBox;

  vStart1.X = R3D_MIN(vStart.X, vEnd.X);
  vStart1.Y = R3D_MIN(vStart.Y, vEnd.Y);
  vStart1.Z = R3D_MIN(vStart.Z, vEnd.Z);
  vEnd1.X   = R3D_MAX(vStart.X, vEnd.X);
  vEnd1.Y   = R3D_MAX(vStart.Y, vEnd.Y);
  vEnd1.Z   = R3D_MAX(vStart.Z, vEnd.Z);

  Move_BBox.Org   = vStart1;
  Move_BBox.Size  = vEnd1 - vStart1;

  Move_BBox.Org  -= r3dPoint3D(Radius * 1.1f, Height * 0.1f, Radius * 1.1f);
  Move_BBox.Size += r3dPoint3D(Radius,        Height,        Radius);
  Move_BBox.Size += r3dPoint3D(Radius * 1.1f, Height * 0.1f, Radius * 1.1f);

  for(int i = 0; i < Cell->NumFaces; i++)
  {
    r3dFace*  pFace      = Cell->FaceList + i;
    r3dVector FaceNormal = pFace->Normal;

    // Step 1. Check "face" collision -----------------------------
    float H = FaceNormal.Dot(vMove);
    if(H >= 0) continue;		// face invisible for vMove

    if(1)
    {
      r3dPoint3D	min, max;
      r3dBoundBox	bbox;

      pFace->GetFaceBounds(min, max);
      bbox.Org  = min;
      bbox.Size = max - min;
      if(!Move_BBox.Intersect(bbox))
	continue;
    }

    r3dVector vOrg, vUp;

    if(FaceNormal.Y <= 0)
    {
      vOrg = vTopStart;
      vUp  = vCylUp;
    }
    else
    {
      vOrg = vBotStart;
      vUp  = -vCylUp;
    }

    r3dVector vTouchPos;
    r3dVector vTouchPoint;
    r3dVector vTouchNormal = FaceNormal.Project(vCylUp);

    if(vTouchNormal.IsVoid())
    {
      vTouchNormal = FaceNormal;
      vTouchPoint  = vOrg;
    }
    else
    {
      vTouchNormal.Normalize();
      vTouchPoint  = vOrg - vTouchNormal * Radius;
    }

    r3dVector vFacePoints[3];
    vFacePoints[0] = Cell->WorldVertexList[pFace->VertIndex[0]];

    float h    = FaceNormal.Dot(vFacePoints[0] - vTouchPoint);
    float k    = h / H;

    if(k >= 0 && k < Info.ClipCoef && pFace->ContainsPoint(vTouchPoint += vMove * k))
    {
//r3dOutToLog("_Face1, %f (%d)\n", MoveLength * k, Info.Type);		
      Info.Type         = CLT_Face;
      Info.ClipCoef     = k;
      Info.Distance     = MoveLength * k;
      Info.Normal       = FaceNormal;
      Info.pFace        = pFace;
      continue;
    }

    // Step 2. Check "side" collision -----------------------------
    vFacePoints[1] = Cell->WorldVertexList[pFace->VertIndex[1]];
    vFacePoints[2] = Cell->WorldVertexList[pFace->VertIndex[2]];

    for(int j = 0; j < 3; j++)
    {
      static int off[3] = { 1, 2, 0 };

      r3dVector p1 = vFacePoints[j];
      r3dVector p2 = vFacePoints[off[j]];
      r3dVector vSide = p2 - p1;
      float     SideLength = vSide.Length();

      vSide /= SideLength;

      // End test ------------------------------------------
      if(bHorizontalMoving)
      {
	if(R3D_ABS(vSide.Y) > R3D_EPSILON)
	{
	  r3dVector pp = p1 + vSide * (vOrg.Y - p1.Y) / vSide.Y;

	  if((pp - p1).Dot(pp - p2) < 0)
	  {
	    h = (pp - vOrg).Dot(vCylRight);

	    if(R3D_ABS(h) < Radius)
	    {
	      vTouchPos = pp - vCylForw * Radius - vCylRight * h;

	      if((dist = (vTouchPos - vOrg).Length()) < Info.Distance)
	      {
//r3dOutToLog("_Side1, %f, %f (%d)\n", dist, Info.Distance, Info.Type);		
		Info.Type         = CLT_Side;
		Info.ClipCoef     = dist / MoveLength;
		Info.Distance     = dist;
		Info.Normal       = (vTouchPos - pp) / Radius;
		Info.pFace        = pFace;
		continue;
	      }
	    }
	  }
	}
      }
      else
      {
	float k1 = (p1.Y - vOrg.Y) / vDir.Y;
	float k2 = (p2.Y - vOrg.Y) / vDir.Y;

	if(k1 >= 0 || k2 >= 0)
	{
	  r3dVector pp1 = p1 - vDir * k1;
	  r3dVector pp2 = p2 - vDir * k2;

	  float L2 = (pp2 - pp1).Length();
	  float A2 = (pp1.Z - pp2.Z) / L2;
	  float B2 = (pp2.X - pp1.X) / L2;

	  h = (vOrg.X - pp1.X)*A2 + (vOrg.Z - pp1.Z)*B2;

	  if(R3D_ABS(h) < Radius)
	  {
	    r3dVector pp, vs;

	    if(k1 > 0 && k1 < k2)
	      vs = ((pp = pp1) - pp2) / L2;
	    else
	      vs = ((pp = pp2) - pp1) / L2;

	    float x = (pp - vOrg).Dot(vs) - sqrtf(Radius2 - h*h);

	    if(x > 0)
	    {
	      pp -= vs * x;

	      vTouchPoint = p1 + vSide * ((pp - pp1).Length() / L2 * SideLength);

	      if((vTouchPoint - p1).Dot(vTouchPoint - p2) < 0)
	      {
// sometime, due to float innacurary we switch from correct face collision to adjusted face (side) collision
// that should fix it
#define VERTEX_SIDE_EPSILON 0.002
		if((dist = (vTouchPoint - pp).Length()) < (Info.Distance - VERTEX_SIDE_EPSILON))
		{
//r3dOutToLog("_Side2, %f, %f (%d)\n", dist, Info.Distance - VERTEX_SIDE_EPSILON, Info.Type);		
		  Info.Type         = CLT_Side;
		  Info.ClipCoef     = dist / MoveLength;
		  Info.Distance     = dist;
		  Info.Normal       = -vDir;
		  Info.pFace        = pFace;
		  continue;
		}
	      }
	    }
	  }
	}
      }

      // Body test -----------------------------------------
      if(bVerticalMoving)
	continue;			// Only vertical component of moving

      if(R3D_ABS(vSide.Y) > 1 - R3D_EPSILON)
	continue;			// this line are vertical

      if(pFace->Normal.Y < -0.2f)	//! bad and ugly hack - we won't collide with ceilings
        continue;

      float A2 = p1.Z - p2.Z, B2 = p2.X - p1.X;

      float denom = A1*B2 - A2*B1;

      if(R3D_ABS(denom) < R3D_EPSILON)
	continue;			// these lines are parallel

      float C2 = p1.X*p2.Z - p2.X*p1.Z;
      float L2 = sqrtf(A2*A2 + B2*B2);
      float M  = Radius * L2 / R3D_ABS(denom);
      float K  = (denom < 0 ? Radius : -Radius) / L2;

      vTouchPos.Assign(  (B1*C2 - B2*C1) / denom - B1 * M,
			 vStart.Y,
			 (A2*C1 - A1*C2) / denom + A1 * M);

      vTouchPoint.Assign(vTouchPos.X + A2 * K,
			 p1.Y,
			 vTouchPos.Z + B2 * K);

      vTouchPos.Y   += vDir.Y  * (vTouchPos - vStart).Length()
			      / sqrtf(1 - vDir.Y*vDir.Y);

      vTouchPoint.Y += vSide.Y * (vTouchPoint - p1).Length()
			      / sqrtf(1 - vSide.Y*vSide.Y);

      if((vTouchPoint - p1).Dot(vTouchPoint - p2) >= 0.)
	continue;			// out of side

      if(vTouchPoint.Y < vTouchPos.Y || vTouchPoint.Y > vTouchPos.Y + Height)
	continue;

      //dist = (vTouchPos - vStart).Length();

      r3dPoint3D *tri[3];
      tri[0] = pFace->Vertices[0];
      tri[1] = pFace->Vertices[1];
      tri[2] = pFace->Vertices[2];
      r3dSegment3 seg;
      seg.Origin    = vStart + vCylAxle * Height/2.0f;
      seg.Direction = vCylAxle;
      seg.Extent    = Height/2.0f;
      float zedist1 = r3dDistSegment3Triangle3_GetSquared(seg, tri);
      float zedist2 = sqrtf(zedist1) - Radius;
      if(zedist2 < 0) 
        continue;

      dist = zedist2;
      if(dist < Info.Distance)
      {
//r3dOutToLog("_Side3[%d], %f, %f (%d)\n", pFace - Cell->FaceList, dist, Info.Distance, Info.Type);
	Info.Type	  = CLT_Side;
	Info.ClipCoef     = dist / MoveLength;
	Info.Distance     = dist;
	Info.Normal       = pFace->Normal;
	Info.pFace        = pFace;
	continue;
      }
    }

    // Step 3. Check "node" collision -----------------------------
    for(int j = 0; j < 3; j++)
    {
      r3dVector vNode = vFacePoints[j] - vOrg;

      // End test ------------------------------------------
      if(!bHorizontalMoving)
      {
	float k = vNode.Y / vMove.Y;

	if(k > 0 && k < Info.ClipCoef && (vMove*k - vNode).Length() < Radius)
	{
//r3dOutToLog("_Vertex1, %f (%d)\n", MoveLength * k, Info.Type);		
	  Info.Type	    = CLT_Vertex;
	  Info.ClipCoef     = k;
	  Info.Distance     = MoveLength * k;
	  Info.Normal       = -vCylUp;
	  Info.pFace        = pFace;
	  continue;
	}
      }

      // Body test -----------------------------------------
      if(bVerticalMoving)
	continue;			// Only vertical component of moving

      float x  = vNode.Dot(vCylForw);
      if(x < 0) continue;		// Point lies behind of me

      float h = vNode.Dot(vCylRight);
      if(R3D_ABS(h) > Radius) continue;// Distance to point > Radius

      float d = sqrtf(Radius2 - h*h);

      float dist = (x - d) / vMoveAngle;

      r3dVector vTouch = vFacePoints[j] - vDir * dist;

      if((vTouch - vStart).Dot(vTouch - vTopStart) >= 0.)
	continue;			// out of side

      if(dist >= 0 && dist < Info.Distance)
      {
//r3dOutToLog("_Vertex2, %f, %f (%d)\n", dist, Info.Distance, Info.Type);		
	Info.Type         = CLT_Vertex;
	Info.ClipCoef     = dist / MoveLength;
	Info.Distance     = dist;
	Info.Normal       = (vCylRight * -h + vCylForw * -d) / Radius;
	Info.pFace        = pFace;
      }
    }
  }

  Info.NewPosition  = vStart + vDir * Info.Distance;
  return BOOL(Info.Type);
}


*/

/*
//-----------------------------------------------------------------------------------
BOOL CheckSphereCollision(INARG r3dVector &vStart,
			  INARG r3dVector &vMove,
			  INARG float Radius,
			  INARG float Height,
			  INARG r3dMesh *Cell,
			  OUTARG CollisionInfo &Info)
//-----------------------------------------------------------------------------------
{
 // if(Cell->m_pTransformedObj) 
 //   Cell = Cell->m_pTransformedObj;

  float MoveLength  = vMove.Length();

  r3dVector vEnd    = vStart + vMove;

  //---------------------------------------------------------------------
  // STAGE 1: Initialization of internal variables
  //---------------------------------------------------------------------
  Info.Type	    = CLT_Nothing;
  Info.pMeshObj     = Cell;
  Info.ClipCoef     = 1.00001f;			// hack
  Info.Distance     = MoveLength;

  r3dVector vDir = vMove * (1.0f / MoveLength);

  float Radius2  = Radius * Radius;

  //---------------------------------------------------------------------
  // STAGE 2: Perform checks in each cell we where going
  //---------------------------------------------------------------------
  r3dVector vMoveRight(vStart.Z - vEnd.Z, 0, vEnd.X - vStart.X);
  if(!vMoveRight.IsVoid())
    vMoveRight.Normalize();

      for(int i = 0 ; i < Cell->NumFaces; i++)
      {
	r3dFace* pFace = Cell->FaceList + i; //Cell->FaceArray[i];
	r3dVector FaceNormal = pFace->Normal;

	// Step 1. Check "face" collision -----------------------------
	float H = FaceNormal.Dot(vMove);
	if(H >= 0) continue;		// face invisible for vMove

	r3dVector vFacePoints[3];
	vFacePoints[0] = Cell->WorldVertexList[pFace->VertIndex[0]];
        float h = FaceNormal.Dot(vFacePoints[0] - vStart);
        if(h >= 0) continue;		// the face is behind of vMove

        float k = (h + Radius) / H;

        if(k > 1.0) continue;		// the face is too far for vMove

	if(k > 0 && k < Info.ClipCoef && pFace->ContainsPoint(vStart + vMove * k - FaceNormal * Radius))
	{
	  Info.Type	    = CLT_Face;
	  Info.ClipCoef     = k;
	  Info.Distance     = MoveLength * k;
	  Info.Normal       = FaceNormal;
	  Info.pFace        = pFace;
	  continue;
        }

        // Step 2. Check "side" collision -----------------------------
	vFacePoints[1] = Cell->WorldVertexList[pFace->VertIndex[1]];
	vFacePoints[2] = Cell->WorldVertexList[pFace->VertIndex[2]];

        for(int j = 0; j < 3; j++)
        {
          static int off[3] = { 1, 2, 0 };

          r3dVector p1 = vFacePoints[j];
          r3dVector p2 = vFacePoints[off[j]];
	  r3dVector vSide = (p2 - p1).NormalizeTo();

	  r3dMatrix M;		// Equation's matrix for 3D-line intersection

          M[1] = vDir.Cross(vSide);

          float sine = M[1].Length();		// sine of angle between vDir & vSide
	  if(sine < R3D_EPSILON) continue;	// the vectors are almost parallel

	  M[1] *= (1.0f/sine);			// Normalization for M.vUp

	  float h = (p1 - vStart).Dot(M[1]);
          if(fabs(h) >= Radius) continue;       // The sphere don't touched this rib

          float r_sin = sqrtf(Radius2 - h*h) / sine;	// r / sin(a)

                                    		// normalization
          M[0] = vSide.Cross(M[1]); 		// is not needed
          M[2] = M[1].Cross(vDir);  		// for M[0] and M[2]
                                                        
	  // --------------------------------------------------------------
          // The cross point we'll search as solution of equation
	  // by Kramer's rule
          // --------------------------------------------------------------
 	  // Free members of equation
          r3dVector D(p1.Dot(M[0]), p1.Dot(M[1]), vStart.Dot(M[2]));

	  // Determinant of solution's matrix
          float Det = M.Det();

          // Point of 3D-lines crossing
	  r3dVector vCross;

          // Solution is . . .
          r3dMatrix T;
          T = M; T.SetColumn(0, D); vCross[0] = T.Det() / Det;
          T = M; T.SetColumn(1, D); vCross[1] = T.Det() / Det;
          T = M; T.SetColumn(2, D); vCross[2] = T.Det() / Det;

          float cose = vDir.Dot(vSide);
	  r3dVector vTouch = vCross - vSide * (r_sin * cose);

	  if((vTouch - p1).Dot(vTouch - p2) >= 0.)
            continue;			// out of side

          float dist = vDir.Dot(vCross - vDir * r_sin - vStart);

	  if(dist >= 0 && dist < Info.Distance)
	  {
//r3dOutToLog("_Side4, %f, %f (%d)\n", dist, Info.Distance, Info.Type);		
	    Info.Type	      = CLT_Side;
	    Info.pFace        = pFace;
	    Info.ClipCoef     = dist / MoveLength;
	    Info.Distance     = dist;
	    Info.Normal       = (vStart + vDir * dist - vTouch).NormalizeTo();
          }
        }

        // Step 3. Check "node" collision -----------------------------
        for(int j = 0; j < 3; j++)
        {
          r3dVector vNode = vFacePoints[j] - vStart;

	  float d  = vNode.Dot(vDir);
          if(d < 0) continue;		// Point lies behind of me

          float x2 = Radius2 - vNode.Dot(vNode) + d*d;
          if(x2 < 0) continue;		// Distance to point > Radius

	  float dist = d - sqrtf(x2);

	  if(dist >= 0 && dist < Info.Distance)
	  {
	    Info.Type	      = CLT_Vertex;
	    Info.ClipCoef     = dist / MoveLength;
	    Info.Distance     = dist;
	    Info.Normal       = (vDir * dist - vNode).NormalizeTo();
	    Info.pFace        = pFace;
	  }
	}
      }

  if(Info.Type)
  {
    Info.Distance  -= 1;			// hack
    if(Info.Distance < 0) Info.Distance = 0;	//
  }

  Info.ClipCoef     = Info.Distance / MoveLength;
  Info.NewPosition  = vStart + vDir * Info.Distance;

  return BOOL(Info.Type);
}

*/