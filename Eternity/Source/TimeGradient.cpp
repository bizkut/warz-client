#include "r3dPCH.h"
#include "r3d.h"

#include "TimeGradient.h"


static int grad2SortByTime(const void *P1, const void *P2)
{
  const r3dTimeGradient2::val_s& g1 = *(const r3dTimeGradient2::val_s*)P1;
  const r3dTimeGradient2::val_s& g2 = *(const r3dTimeGradient2::val_s*)P2;
  
  if(g1.time >= g2.time) return 1;
  if(g1.time <  g2.time) return -1;
  return 0;
}

void r3dTimeGradient2::Reset(float defval)
{ 
  //Type             = TYPE_Float;
  NumValues        = 2; 
  Values[0].time   = 0.0f;
  Values[0].val[0] = defval;
  Values[0].val[1] = defval;
  Values[0].val[2] = defval;
  Values[1].time   = 1.0f;
  Values[1].val[0] = defval;
  Values[1].val[1] = defval;
  Values[1].val[2] = defval;
  
  return;
}

void r3dTimeGradient2::Reset(float left, float right)
{ 
	//Type             = TYPE_Float;
	NumValues        = 2; 
	Values[0].time   = 0.0f;
	Values[0].val[0] = left;
	Values[0].val[1] = left;
	Values[0].val[2] = left;
	Values[1].time   = 1.0f;
	Values[1].val[0] = right;
	Values[1].val[1] = right;
	Values[1].val[2] = right;

	return;
}


void r3dTimeGradient2::CopyFrom(const r3dTimeGradient2& rhs)
{
  //Type      = rhs.Type;
  NumValues = rhs.NumValues;

  for(int i=0;i<NumValues;i++) {
    Values[i] = rhs.Values[i];
  }
  
  return;
}


int r3dTimeGradient2::AddValue3f(float time, float v1, float v2, float v3)
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

  // find new added value
  for(int i=0; i<NumValues; i++) {
    if(Values[i].time == time) {
      return i;
    }
  }
  
  assert(0);
  return 0;
}

void r3dTimeGradient2::ResortAfterChange(int* curIdx)
{
  val_s saved;
  if(curIdx) {
    saved = Values[*curIdx];
  }

  qsort((void *)Values, NumValues, sizeof(Values[0]), grad2SortByTime);
  
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

inline float CalcSmooth( float y0, float y1, float t )
{
	return ( 2 * t*t*t - 3 * t*t + 1 ) * y0 + ( -2 * t*t*t + 3 * t*t ) * y1;
}

float4 r3dTimeGradient2::GetValue(float time) const
{ 
  float4 res;

  assert(NumValues >= 2);
  //assert(time >= 0.0f && time <= 1.1f);
  if(time <= 0.0f)
	  return *(float4*)Values[0].val;

  if(time >= 1.0f)
	  return *(float4*)Values[NumValues-1].val;

  // FIXME: Binary search here?
  int i;
  for(i=0; i<NumValues; i++) {
	  if(Values[i].time >= time)
		  break;
  }

  assert(i);

  const val_s& v1 = Values[i-1];
  const val_s& v2 = Values[i];
  float delta     = (time - v1.time) / (v2.time - v1.time);

  if( Smooth )
  {
	  res.x = CalcSmooth( v1.val[0], v2.val[0], delta );
	  res.y = CalcSmooth( v1.val[1], v2.val[1], delta );
	  res.z = CalcSmooth( v1.val[2], v2.val[2], delta );
  }
  else
  {
	  res.x = v1.val[0] + (v2.val[0] - v1.val[0]) * delta;
	  res.y = v1.val[1] + (v2.val[1] - v1.val[1]) * delta;
	  res.z = v1.val[2] + (v2.val[2] - v1.val[2]) * delta;
  }
  
  return res;
}



void r3dTimeGradient2::LoadGradient(const char* FName, const char* PartName, const char* SectionName)
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

  sprintf( varName, "%sSmooth", SectionName );
  Smooth = r3dReadCFG_I(FName, PartName, varName, 0 ) ? true : false;

  return;
}

void r3dTimeGradient2::SaveGradient(const char* FName, const char* PartName, const char* SectionName)
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

  sprintf( varName, "%sSmooth", SectionName );
  r3dWriteCFG_I(FName, PartName, varName, Smooth );
  
  return;
}

bool operator == (const r3dTimeGradient2 &l, const r3dTimeGradient2 &r)
{
	if (l.Smooth != r.Smooth)
		return false;

	if (l.NumValues != r.NumValues)
		return false;

	return memcmp(l.Values, r.Values, sizeof(r.Values[0]) * r.NumValues) == 0;
}

bool operator != (const r3dTimeGradient2 &l, const r3dTimeGradient2 &r)
{
	return !(l == r);
}
