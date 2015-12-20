#ifndef __R3D_BEZIERGRADIENT_H
#define __R3D_BEZIERGRADIENT_H

//
// bezier gradient - strictly from [0..1]
//

class r3dBezierGradient
{
  public:
	struct val_s {
	  float		time;
	  float		val[4];
	};

  public:
	
	enum { MAX_VALUES = 32, };
	int		NumValues;
	val_s	Values[MAX_VALUES];
	val_s	FirstControlPoints[MAX_VALUES];
	val_s	SecondControlPoints[MAX_VALUES];

	float	ClampMin;
	float	ClampMax;

  public:	
	r3dBezierGradient() {
	  Reset();

	  ClampMin = -FLT_MAX;
	  ClampMax = FLT_MAX;
	}

	void		UpdateControlPoints ();
	void		Reset(float defval = 1.0f);
	void		ResortAfterChange(int* curIdx);
	void		CopyFrom(const r3dBezierGradient& rhs);

	void		LoadGradient(const char* FName, const char* PartName, const char* SectionName);
	void 		SaveGradient(const char* FName, const char* PartName, const char* SectionName);

	int			AddValue3f(float time, float v1, float v2, float v3);
	
	int			AddValue(float time, float value) {
	  return AddValue3f(time, value, 0.0f, 0.0f);
	}

	int			AddValue(float time, const r3dVector& value) {
	  return AddValue3f(time, value.x, value.y, value.z);
	}

	int			AddValue(float time, const r3dColor24& value) {
	  return AddValue3f(time, float(value.R), float(value.G), float(value.B));
  	} 

	const float*GetValue(float time) const;

	float 		GetFloatValue(float time) const { 
	  const float* val = GetValue(time);
	  return val[0];
	}
	r3dVector	GetVectorValue(float time) const {
	  const float* val = GetValue(time);
	  return r3dPoint3D(val[0], val[1], val[2]);
	}
	r3dColor24	GetColorValue(float time) const {
	  const float* val = GetValue(time);
	  return r3dColor24(val[0], val[1], val[2], 255.0f);
	}
};

#endif	// __R3D_BEZIERGRADIENT_H

