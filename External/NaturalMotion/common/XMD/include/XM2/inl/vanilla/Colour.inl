#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VANILLA_COLOUR__INL__
#define XM2_VANILLA_COLOUR__INL__
#ifdef XM2_USE_SSE
# error FPU routines included when set to build with SSE
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XColour::XColour(const XReal v)
{
  set(v,v,v,v);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XColour::XColour(const XReal* col)
{
  XM2_ASSERT(col);
  set(col[0],col[1],col[2],col[3]);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XColour::equal(const XColour& _b,const XReal eps) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&_b);
  return float_equal(r,_b.r,eps) && 
    float_equal(g,_b.g,eps) && 
    float_equal(b,_b.b,eps) && 
    float_equal(a,_b.a,eps);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XColour& XColour::operator = (const XColour& c)
{
  r=c.r;
  g=c.g;
  b=c.b;
  a=c.a;
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::zero()
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(this);
  memset(this,0,sizeof(XColour));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::lerp(const XColour& _a,const XColour& _b,const XReal t)
{
  r = _a.r + t*(_b.r - _a.r);
  g = _a.g + t*(_b.g - _a.g);
  b = _a.b + t*(_b.b - _a.b);
  a = _a.a + t*(_b.a - _a.a);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::bezier(const XColour& _a,const XColour& _b,const XColour& _c,const XColour& _d,const XReal t)
{
  const XReal B0 = (1.0f-t) * (1.0f-t) * (1.0f-t);
  const XReal B1 = 3.0f * (1.0f-t) * (1.0f-t) * t;
  const XReal B2 = 3.0f * (1.0f-t) * t * t;
  const XReal B3 = t * t * t;

  r = B0 * _a.r  +  B1 * _b.r  +  B2 * _c.r +  B3 * _d.r;
  g = B0 * _a.g  +  B1 * _b.g  +  B2 * _c.g +  B3 * _d.g;
  b = B0 * _a.b  +  B1 * _b.b  +  B2 * _c.b +  B3 * _d.b;
  a = B0 * _a.a  +  B1 * _b.a  +  B2 * _c.a +  B3 * _d.a;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::white()
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(this);

  r = 1.0f;
  g = 1.0f;
  b = 1.0f;
  a = 1.0f;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::set(const XReal _r,const XReal _g,const XReal _b,const XReal _a)
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(this);

  r = _r;
  g = _g;
  b = _b;
  a = _a;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::mul(const XColour& _a,const XColour& _b)
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(&_b);
  XM2_ALIGN_ASSERT(this);

  r = _a.r * _b.r;
  g = _a.g * _b.g;
  b = _a.b * _b.b;
  a = _a.a * _b.a;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::div(const XColour& _a,const XColour& _b)
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(&_b);
  XM2_ALIGN_ASSERT(this);

  #if XM2_DIVIDE_BY_ZERO_MODE
    XM2_ASSERT(XMabs(_b.r)>XM2_FLOAT_ZERO);
    XM2_ASSERT(XMabs(_b.g)>XM2_FLOAT_ZERO);
    XM2_ASSERT(XMabs(_b.b)>XM2_FLOAT_ZERO);
    XM2_ASSERT(XMabs(_b.a)>XM2_FLOAT_ZERO);
    r = _a.r / _b.r;
    g = _a.g / _b.g;
    b = _a.b / _b.b;
    a = _a.a / _b.a;
  #else
    r = (XMabs(_b.r)<XM2_FLOAT_ZERO) ? 0 : _a.r / _b.r;
    g = (XMabs(_b.g)<XM2_FLOAT_ZERO) ? 0 : _a.g / _b.g;
    b = (XMabs(_b.b)<XM2_FLOAT_ZERO) ? 0 : _a.b / _b.b;
    a = (XMabs(_b.a)<XM2_FLOAT_ZERO) ? 0 : _a.a / _b.a;
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::sub(const XColour& _a,const XColour& _b)
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(&_b);
  XM2_ALIGN_ASSERT(this);

  r = _a.r - _b.r;
  g = _a.g - _b.g;
  b = _a.b - _b.b;
  a = _a.a - _b.a;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::add(const XColour& _a,const XColour& _b)
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(&_b);
  XM2_ALIGN_ASSERT(this);

  r = _a.r + _b.r;
  g = _a.g + _b.g;
  b = _a.b + _b.b;
  a = _a.a + _b.a;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::mul(const XColour& _a,const XReal _b)
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(this);

  r = _a.r * _b;
  g = _a.g * _b;
  b = _a.b * _b;
  a = _a.a * _b;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::div(const XColour& _a,const XReal _b)
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(this);

  #if XM2_DIVIDE_BY_ZERO_MODE
  XM2_ASSERT(XMabs(_b)>XM2_FLOAT_ZERO);
  {
    const XReal f = 1.0f/_b;
    r = _a.r * f;
    g = _a.g * f;
    b = _a.b * f;
    a = _a.a * f;
  }
  #else
  {
    const XReal f = (XMabs(_b)>XM2_FLOAT_ZERO) ? 1.0f/_b : 0.0f;
    r = _a.r * f;
    g = _a.g * f;
    b = _a.b * f;
    a = _a.a * f;
  }
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::sub(const XColour& _a,const XReal _b)
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(this);

  r = _a.r - _b;
  g = _a.g - _b;
  b = _a.b - _b;
  a = _a.a - _b;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::add(const XColour& _a,const XReal _b)
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(this);

  r = _a.r + _b;
  g = _a.g + _b;
  b = _a.b + _b;
  a = _a.a + _b;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::invert(const XColour& _a)
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(this);

  r = 1.0f-_a.r;
  g = 1.0f-_a.g;
  b = 1.0f-_a.b;
  a = 1.0f-_a.a;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XColour::negate(const XColour& _a)
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(&_a);
  XM2_ALIGN_ASSERT(this);

  r = -_a.r;
  g = -_a.g;
  b = -_a.b;
  a = -_a.a;
}
}
#endif
