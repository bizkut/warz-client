#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_MATRIX_STACK__INL__
#define XM2_MATRIX_STACK__INL__
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XMatrix34Stack::XMatrix34Stack() 
  : pod_stack<XMatrix>() 
{
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XMatrix34Stack::~XMatrix34Stack() 
{
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XMatrix34Stack::empty() const
{
  return pod_stack<XMatrix>::empty();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XMatrix34Stack::size_type XMatrix34Stack::size() const
{
  return pod_stack<XMatrix>::size();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XMatrix& XMatrix34Stack::top()
{
  XM2_ASSERT( !empty() );
  return pod_stack<XMatrix>::top();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XMatrix& XMatrix34Stack::top() const
{
  XM2_ASSERT( !empty() );
  return pod_stack<XMatrix>::top();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix34Stack::push()
{
  XMatrix m = top();
  pod_stack<XMatrix>::push(m);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix34Stack::pop()
{
  XM2_ASSERT( !empty() );
  pod_stack<XMatrix>::pop();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix34Stack::clear()
{
  while(!empty())
    pop();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix34Stack::identity()
{
  top().identity();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix34Stack::multMatrix(const XMatrix& m)
{
  top() *= m;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix34Stack::loadMatrix(const XMatrix& m)
{
  top() = m;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix34Stack::rotate(const XQuaternion& q)
{
  XMatrix c = q;
  top() *= c;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix34Stack::rotateD(const XVector3& axis,const XReal degrees)
{
  XMatrix c;
  c.fromAxisAngleD(axis,degrees);
  top() *= c;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix34Stack::rotateR(const XVector3& axis,const XReal radians)
{
  XMatrix c;
  c.fromAxisAngleR(axis,radians);
  top() *= c;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix34Stack::rotateXD(const XReal degrees)
{
  top().rotXD(degrees);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix34Stack::rotateXR(const XReal radians)
{
  top().rotXR(radians);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix34Stack::rotateYD(const XReal degrees)
{
  top().rotYD(degrees);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix34Stack::rotateYR(const XReal radians)
{
  top().rotYR(radians);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix34Stack::rotateZD(const XReal degrees)
{
  top().rotZD(degrees);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix34Stack::rotateZR(const XReal radians)
{
  top().rotZR(radians);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix34Stack::scale(const XVector3& _scale)
{
  top().scale(_scale);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix34Stack::translate(const XVector3& _translate)
{
  top().translate(_translate);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix34Stack::scale(const XReal x,const XReal y,const XReal z)
{
  top().scale(x,y,z);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix34Stack::translate(const XReal x,const XReal y,const XReal z)
{
  top().translate(x,y,z);
}
}
#endif
