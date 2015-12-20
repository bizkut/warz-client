#include "r3dPCH.h"
#include "r3d.h"

#include "BezierGradient.h"

#include "r3dSpline.h"

template <class T> 
inline T LerpFast ( T from, T to, float weight )
{
	return from + weight * ( to - from );
}

static void GetFirstControlPoints( r3dTL::TArray < float > const & rhs, int iCnt, r3dTL::TArray < float > & pOut )
{
	int n = iCnt;
	r3dTL::TArray < float > tmp;
	tmp.Resize(n);

	float b = 2.0f;
	pOut[0] = rhs[0] / b;
	for (int i = 1; i < n; i++) // Decomposition and forward substitution.
	{
		tmp[i] = 1.0f / b;
		b = (i < n - 1 ? 4.0f : 3.5f) - tmp[i];
		pOut[i] = (rhs[i] - pOut[i - 1]) / b;
	}
	for (int i = 1; i < n; i++)
		pOut[n - i - 1] -= tmp[n - i] * pOut[n - i]; // Backsubstitution.
}

static void GetCurveControlPoints ( const r3dBezierGradient::val_s * pKnots, int iCnt, r3dBezierGradient::val_s * pFirstControlPoints, r3dBezierGradient::val_s * pSecondControlPoints )
{
	int n = iCnt - 1;
	if ( pKnots == NULL || n < 1 )
	{
		return;
	}

	if ( n == 1 )
	{ 
		// Special case: Bezier curve should be a straight line.
		// 3P1 = 2P0 + P3
		// P2 = 2P1 – P0

		for ( int i = 0; i < 4; i++ )
		{
			pFirstControlPoints[0].val[i] = (2.0f * pKnots[0].val[i] + pKnots[1].val[i]) / 3.0f;
			pSecondControlPoints[0].val[i] = 2.0f * pFirstControlPoints[0].val[i] - pKnots[0].val[i];
		}

		pFirstControlPoints[0].time = (2.0f * pKnots[0].time + pKnots[1].time) / 3.0f;
		pSecondControlPoints[0].time = 2.0f * pFirstControlPoints[0].time - pKnots[0].time;

		return;
	}

	// Calculate first Bezier control points
	// Right hand side vector

	r3dTL::TArray < float > rhs;
	rhs.Resize ( n );
	
	// Set right hand side X values
	for (int i = 1; i < n - 1; ++i)
		rhs[i] = 4.0f * pKnots[i].time + 2.0f * pKnots[i + 1].time;
	rhs[0] = pKnots[0].time + 2.0f * pKnots[1].time;
	
	rhs[n - 1] = (8.0f * pKnots[n - 1].time + pKnots[n].time) / 2.0f;
	// Get first control points X-values
	r3dTL::TArray < float > x;
	r3dTL::TArray < float > y;
	x.Resize(n);
	y.Resize(n);
	GetFirstControlPoints(rhs, n, x );

	// Set right hand side Y values
	for (int i = 1; i < n - 1; ++i)
		rhs[i] = 4.0f * pKnots[i].val[0] + 2 * pKnots[i + 1].val[0];
	rhs[0] = pKnots[0].val[0] + 2 * pKnots[1].val[0];
	rhs[n - 1] = (8.0f * pKnots[n - 1].val[0] + pKnots[n].val[0]) / 2.0f;
	// Get first control points Y-values
	GetFirstControlPoints(rhs, n, y );

	// Fill output arrays.
	//firstControlPoints = new (ALLOC_TYPE_LOGIC) Point[n];
	//secondControlPoints = new (ALLOC_TYPE_LOGIC) Point[n];
	for (int i = 0; i < n; ++i)
	{
		// First control point
		pFirstControlPoints[i].time = x[i];
		pFirstControlPoints[i].val[0] = y[i];
		// Second control point
		if (i < n - 1)
		{
			pSecondControlPoints[i].time = 2 * pKnots[i + 1].time - x[i + 1];
			pSecondControlPoints[i].val[0] = 2 * pKnots[i + 1].val[0] - y[i + 1];
		}
		else
		{
			pSecondControlPoints[i].time = (pKnots[n].time + x[n - 1]) / 2.0f;
			pSecondControlPoints[i].val[0] = (pKnots[n].val[0] + y[n - 1]) / 2.0f;
		}

		//std::swap ( pSecondControlPoints[i].val[0],  pSecondControlPoints[i].time );
		//std::swap ( pFirstControlPoints[i].val[0],  pFirstControlPoints[i].time );
	}
}

static int grad2SortByTime(const void *P1, const void *P2)
{
  const r3dBezierGradient::val_s& g1 = *(const r3dBezierGradient::val_s*)P1;
  const r3dBezierGradient::val_s& g2 = *(const r3dBezierGradient::val_s*)P2;
  
  if(g1.time >= g2.time) return 1;
  if(g1.time <  g2.time) return -1;
  return 0;
}

void r3dBezierGradient::UpdateControlPoints ()
{
	GetCurveControlPoints ( Values, NumValues, FirstControlPoints, SecondControlPoints );
}

void r3dBezierGradient::Reset(float defval)
{ 
  NumValues        = 2; 
  Values[0].time   = 0.0f;
  Values[0].val[0] = defval;
  Values[0].val[1] = defval;
  Values[0].val[2] = defval;
  Values[1].time   = 1.0f;
  Values[1].val[0] = defval;
  Values[1].val[1] = defval;
  Values[1].val[2] = defval;
  
  UpdateControlPoints ();

  return;
}

void r3dBezierGradient::CopyFrom(const r3dBezierGradient& rhs)
{
  NumValues = rhs.NumValues;

  for(int i=0;i<NumValues;i++) {
    Values[i] = rhs.Values[i];
  }

  UpdateControlPoints ();
  
  return;
}


int r3dBezierGradient::AddValue3f(float time, float v1, float v2, float v3)
{
  if(NumValues >= MAX_VALUES)
    return 0;

  assert(NumValues >= 2);
  assert(NumValues < MAX_VALUES);

  if(time <= 0.0f) {
    val_s& v = Values[0];
    v.val[0] = v1;
    v.val[1] = v2;
    v.val[2] = v3;
    return 0;
  } 

  if(time >= 1.0f) {
    val_s& v = Values[NumValues-1];
    v.val[0] = v1;
    v.val[1] = v2;
    v.val[2] = v3;
    return NumValues-1;
  }
  
  val_s& v = Values[NumValues];
  NumValues++;

  v.time   = time;
  v.val[0] = v1;
  v.val[1] = v2;
  v.val[2] = v3;

  // sort on time
  qsort((void *)Values, NumValues, sizeof(Values[0]), grad2SortByTime);

  UpdateControlPoints ();

  // find new added value
  for(int i=0; i<NumValues; i++) {
    if(Values[i].time == time) {
      return i;
    }
  }
  
  assert(0);
  return 0;
}

void r3dBezierGradient::ResortAfterChange(int* curIdx)
{
  val_s saved;
  if(curIdx) {
    saved = Values[*curIdx];
  }

  qsort((void *)Values, NumValues, sizeof(Values[0]), grad2SortByTime);

  UpdateControlPoints ();
  
  if(curIdx) {
    for(int i=0; i<NumValues; i++) {
      const val_s& v = Values[i];
      if(v.time == saved.time && v.val[0] == saved.val[0] && v.val[1] == saved.val[1] && v.val[2] == saved.val[2]) {
        *curIdx = i;
        break;
      }
    }
  }

  return;  
}

const float* r3dBezierGradient::GetValue(float time) const
{ 
	r3dPoint3D dPoints[MAX_VALUES];
	for ( int i = 0; i < NumValues; i++ )
	{
		dPoints[i] = r3dPoint3D ( Values[i].val[0], Values[i].val[1], Values[i].val[2] );
	}

	static float outf[4];

	r3dPoint3D res;

	if ( NumValues < 3 )
	{
		if ( NumValues == 1 )
			res = dPoints[0];
		else if ( NumValues == 2 )
			res = LerpFast ( dPoints[0], dPoints[1], time );
	}
	else
	{
		int iStartIdx = 0;
		while ( iStartIdx + 1 < NumValues && time > Values[iStartIdx + 1].time )
			iStartIdx++;
		
		if ( iStartIdx > NumValues - 2 )
			iStartIdx = NumValues - 2;

		if ( time < 0.99f )
		{
			float fTimeNew = LerpFast ( 0.0f, 1.0f, ( time - Values[iStartIdx].time ) / (Values[iStartIdx+1].time - Values[iStartIdx].time) );

			r3dPoint3D vFirstPoint;
			r3dPoint3D vSecondPoint;
			memcpy ( vFirstPoint.d3dx(), FirstControlPoints[iStartIdx].val, sizeof ( FirstControlPoints[iStartIdx].val[0] ) * 3 ); //-V512
			memcpy ( vSecondPoint.d3dx(), SecondControlPoints[iStartIdx].val, sizeof ( SecondControlPoints[iStartIdx].val[0] ) * 3 ); //-V512

			res = r3dSpline3D::Bezier4(dPoints[iStartIdx], vFirstPoint, vSecondPoint, dPoints[iStartIdx + 1], fTimeNew );
			//res = r3dSpline3D::Bezier(dPoints, NumValues - 1, time );
		}
		else
			res = dPoints[NumValues - 1];
	}

	outf[0] = R3D_CLAMP ( res.x, ClampMin, ClampMax );
	outf[1] = R3D_CLAMP ( res.y, ClampMin, ClampMax );
	outf[2] = R3D_CLAMP ( res.z, ClampMin, ClampMax );

	outf[3] = 0.0f;

	return outf;
}



void r3dBezierGradient::LoadGradient(const char* FName, const char* PartName, const char* SectionName)
{
  // ok, this is tricky part - if gradient wasn't loaded (it wasn't saved in the first place), we should NOT reset it.
  bool wasResetted = false;
  
  char varName[256];

  sprintf(varName, "%sNum", SectionName);
  int maxVars = r3dReadCFG_I(FName, PartName, varName, 8);

  for(int i=0;i<maxVars;i++) {
    sprintf(varName, "%s%d", SectionName, i+1);

    float time = -100;
    float v0   = 0;
    float v1   = 0;
    float v2   = 0;
    sscanf(r3dReadCFG_S(FName, PartName, varName, "-100 0 0 0"), "%f %f %f %f", &time, &v0, &v1, &v2);
    if(time < 0) continue;
    
    if(!wasResetted) {
      Reset();
      wasResetted = true;
    }
    AddValue3f(time, v0, v1, v2);
  }

  return;
}

void r3dBezierGradient::SaveGradient(const char* FName, const char* PartName, const char* SectionName)
{
  char varName[256];
  char data[256];

  sprintf(varName, "%sNum", SectionName);
  r3dWriteCFG_I(FName, PartName, varName, NumValues);

  for(int i=0; i<NumValues; i++) {
    sprintf(varName, "%s%d", SectionName, i+1);
    sprintf(data, "%.2f %.2f %.2f %.2f", Values[i].time, Values[i].val[0], Values[i].val[1], Values[i].val[2]);
    r3dWriteCFG_S(FName, PartName, varName, data);
  }
  
  return;
}
