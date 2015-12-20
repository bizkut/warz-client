//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/MathCommon.h"
#include "XM2/Matrix34.h"
#include "XM2/pod_stack.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
/// \class  XM2::XMatrix34Stack 
/// \brief  This class defines a matrix stack primitive. This works in exactly the same
///         way as the openGL matrix stack, so use push/pop as needed, and the related
///         transform functions eg scale, translate etc.
/// \code
/// XMatrix34Stack m;
/// m.push();
///   m.translate( x,y,z );
///   m.scale( sx,sy,sz );
///   m.rotateXD( 45.0f );
///   m.glMultMatrix(); // pass to openGL
/// m.pop();
/// 
/// \endcode
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
class XM2EXPORT XMatrix34Stack 
  : private pod_stack<XMatrix>
{
public:

  //--------------------------------------------------------------------------------------------------------------------
  // XMatrix34Stack construction
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  construction/destruction

  /// \brief  ctor
  XM2_INLINE XMatrix34Stack();

  /// \brief  dtor
  XM2_INLINE ~XMatrix34Stack();

  //--------------------------------------------------------------------------------------------------------------------
  // XMatrix34Stack stack manipulation
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  stack methods

  /// \brief  test if stack is empty
  /// \return true if empty
  XM2_INLINE bool empty() const;

  /// \brief  returns the size of the stack
  /// \return the size of the stack
  XM2_INLINE size_type size() const;

  /// \brief  return last element of mutable stack
  /// \return the top of the stack
  XM2_INLINE XMatrix& top();

  /// \brief  return last element of non-mutable stack
  /// \return the top of the stack
  XM2_INLINE const XMatrix& top() const;

  /// \brief  push a new matrix onto the stack
  XM2_INLINE void push();

  /// \brief  pops the top element pf the stack
  XM2_INLINE void pop();

  /// \brief  erases all elements
  XM2_INLINE void clear();

  //--------------------------------------------------------------------------------------------------------------------
  // XMatrix34Stack matrix manipulation
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  matrix manipulation

  /// \brief  sets the top to the identity matrix
  XM2_INLINE void identity();

  /// \brief  multiplies the top matrix by m
  /// \param  m - the matrix to multiply the top by
  XM2_INLINE void multMatrix(const XMatrix& m);

  /// \brief  sets the top to m
  /// \param  m - the matrix to assign to the top value
  XM2_INLINE void loadMatrix(const XMatrix& m);

  //--------------------------------------------------------------------------------------------------------------------
  // XMatrix34Stack transformations
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  transformations

  /// \brief  rotates this matrix by the specified quaternion
  /// \param  q - the quaternion rotation
  XM2_INLINE void rotate(const XQuaternion& q);
  
  /// \brief  rotates by the specified axis angle
  /// \param  axis - the axis of rotation
  /// \param  degrees - the number of degrees to rotate 
  XM2_INLINE void rotateD(const XVector3& axis, XReal degrees);
  
  /// \brief  rotates by the specified axis angle
  /// \param  axis - the axis of rotation
  /// \param  radians - the number of radians to rotate 
  XM2_INLINE void rotateR(const XVector3& axis, XReal radians);

  /// \brief  rotates this matrix in the X axis by the specifies number of degrees
  /// \param  degrees - the number of degrees to rotate 
  XM2_INLINE void rotateXD(const XReal degrees);

  /// \brief  rotates this matrix in the X axis by the specifies number of degrees
  /// \param  radians - the number of radians to rotate 
  XM2_INLINE void rotateXR(const XReal radians);

  /// \brief  rotates this matrix in the Y axis by the specifies number of degrees
  /// \param  degrees - the number of degrees to rotate 
  XM2_INLINE void rotateYD(const XReal degrees);

  /// \brief  rotates this matrix in the Y axis by the specifies number of degrees
  /// \param  radians - the number of radians to rotate 
  XM2_INLINE void rotateYR(const XReal radians);

  /// \brief  rotates this matrix in the Z axis by the specifies number of degrees
  /// \param  degrees - the number of degrees to rotate 
  XM2_INLINE void rotateZD(const XReal degrees);

  /// \brief  rotates this matrix in the Z axis by the specifies number of degrees
  /// \param  radians - the number of radians to rotate 
  XM2_INLINE void rotateZR(const XReal radians);

  /// \brief  scales the current matrix
  /// \param  _scale - the scaling to apply
  XM2_INLINE void scale(const XVector3& _scale);

  /// \brief  translates the current matrix
  /// \param  _translate - the amount of translation
  XM2_INLINE void translate(const XVector3& _translate);

  /// \brief  scales the current matrix
  /// \param  x
  /// \param  y
  /// \param  z
  XM2_INLINE void scale(XReal x, XReal y,XReal z);

  /// \brief  translates the current matrix
  /// \param  x
  /// \param  y
  /// \param  z
  XM2_INLINE void translate(XReal x, XReal y,XReal z);
};
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/inl/MatrixStack.inl"
//----------------------------------------------------------------------------------------------------------------------
