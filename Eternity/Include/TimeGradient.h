#ifndef __R3D_TIMEGRADIENT_H
#define __R3D_TIMEGRADIENT_H


//
// time gradient - strictly from [0..1]
//
#pragma pack(push)
#pragma pack(1)
class r3dTimeGradient2
{
  public:
	struct val_s {
	  float		time;
	  float		val[4];
	};

  public:
/*  
	enum type_e {
	  TYPE_Float,
	  TYPE_Point3D,
	  TYPE_Color,
	};
*/	
	// WARNING: DO NOT REMOVE IT - it's used in binary spell fx files!
	int		Type;	// not exactly used right now - application should call correct Get/Add functions instead
	
	enum { MAX_VALUES = 16, };
	int		NumValues;
	val_s	Values[MAX_VALUES];

	bool	Smooth;

/*
  private:
        // no default copying. use CopyFrom
	r3dTimeGradient2(const r3dTimeGradient2& rhs);
	r3dTimeGradient2& operator= (const r3dTimeGradient2& rhs);
*/	
  	
  public:	
	r3dTimeGradient2() {
	  Smooth = false;
	  Reset();
	}

	void		Reset(float defval = 1.0f);
	void		Reset(float left, float right);
	void		ResortAfterChange(int* curIdx);
	void		CopyFrom(const r3dTimeGradient2& rhs);

	void		LoadGradient(const char* FName, const char* PartName, const char* SectionName);
	void 		SaveGradient(const char* FName, const char* PartName, const char* SectionName);

	int		AddValue3f(float time, float v1, float v2, float v3);
	
	int		AddValue(float time, float value) {
	  return AddValue3f(time, value, 0.0f, 0.0f);
	}

	int		AddValue(float time, const r3dVector& value) {
	  return AddValue3f(time, value.x, value.y, value.z);
	}

	int		AddValue(float time, const r3dColor24& value) {
	  return AddValue3f(time, float(value.R), float(value.G), float(value.B));
  	} 

	float4	GetValue(float time) const;

	float 	GetFloatValue(float time) const { 
	  float4 val = GetValue(time);
	  return val.x;
	}
	r3dVector	GetVectorValue(float time) const {
	  float4 val = GetValue(time);
	  return r3dPoint3D(val.x, val.y, val.z);
	}
	r3dColor24	GetColorValue(float time) const {
	  float4 val = GetValue(time);
	  return r3dColor24(val.x, val.y, val.z, 255.0f);
	}
};

bool operator == (const r3dTimeGradient2 &l, const r3dTimeGradient2 &r);
bool operator != (const r3dTimeGradient2 &l, const r3dTimeGradient2 &r);

#pragma pack(pop)

#endif	// __R3D_TIMEGRADIENT_H

