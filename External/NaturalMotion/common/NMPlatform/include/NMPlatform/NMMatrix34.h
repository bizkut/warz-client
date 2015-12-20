// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef NM_MATRIX34_H
#define NM_MATRIX34_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMQuat.h"
//----------------------------------------------------------------------------------------------------------------------

// indirection alignment of formal parameter 1 (16) is
// greater than the actual argument alignment (8)
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
#endif

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionPlatformMath
/// \brief A 3x4 matrix class, formed from Vector3 elements. In physical terms this makes
///        it internally a 4x4 matrix, as Vector3s are padded out to four floats in size.
//----------------------------------------------------------------------------------------------------------------------
NMP_ALIGN_PREFIX(16) class Matrix34
{
public:

  Vector3 r[4]; // 3x3 component + xyz translation

  // enum used to pass to a Matrix34 constructor
  // so that specific initialization logic can be used
  enum MatrixConstruction
  {
    kZero,
    kIdentity,
  };

  NM_INLINE Matrix34();
  NM_INLINE Matrix34(Matrix34::MatrixConstruction _mc);
  NM_INLINE Matrix34(const NMP::Matrix34& copy);
  NM_INLINE Matrix34(const NMP::Vector3& xRow, const NMP::Vector3& yRow, const NMP::Vector3& zRow, const NMP::Vector3& trans);
  NM_INLINE Matrix34(const NMP::Quat& quat);
  NM_INLINE Matrix34(const Quat& quat, const NMP::Vector3& trans);

  /// \ingroup Maths
  /// \brief Initialise functions (removes necessity for construction of temporary).
  NM_INLINE void initialise(Matrix34::MatrixConstruction _mc);
  NM_INLINE void initialise(const NMP::Matrix34& copy);
  NM_INLINE void initialise(const NMP::Vector3& xRow, const NMP::Vector3& yRow, const NMP::Vector3& zRow, const NMP::Vector3& trans);
  NM_INLINE void initialise(const NMP::Quat& quat);
  NM_INLINE void initialise(const Quat& quat, const NMP::Vector3& trans);

  /// \ingroup Maths
  /// \brief Standard assignment operator
  NM_INLINE const Matrix34& operator = (const NMP::Matrix34& m);

  // the multiplication operators call Matrix34::multiply()
  NM_INLINE Matrix34   operator * (const NMP::Matrix34& m) const;
  NM_INLINE void       operator *= (const NMP::Matrix34& m);

  NM_INLINE float& operator()(int32_t row, int32_t column);
  NM_INLINE float operator() (int32_t row, int32_t column) const;

  NM_INLINE Vector3& xAxis() { return r[0]; }
  NM_INLINE Vector3& yAxis() { return r[1]; }
  NM_INLINE Vector3& zAxis() { return r[2]; }
  NM_INLINE Vector3& translation() { return r[3]; }

  NM_INLINE const Vector3& xAxis() const { return r[0]; }
  NM_INLINE const Vector3& yAxis() const { return r[1]; }
  NM_INLINE const Vector3& zAxis() const { return r[2]; }
  NM_INLINE const Vector3& translation() const { return r[3]; }

  NM_INLINE void setXAxis(const Vector3& v) { r[0] = v; }
  NM_INLINE void setYAxis(const Vector3& v) { r[1] = v; }
  NM_INLINE void setZAxis(const Vector3& v) { r[2] = v; }
  NM_INLINE void setTranslation(const Vector3& v) { r[3] = v; }

  NM_INLINE Matrix34   operator + (const NMP::Matrix34& m) const;
  NM_INLINE void       operator += (const NMP::Matrix34& m);
  NM_INLINE Matrix34   operator * (const float rhs) const;
  NM_INLINE void       operator *= (const float rhs);

  /// \ingroup Maths
  /// \brief This is more for unit testing and debugging than anything else, but it compares
  ///        the entire contents of one matrix with another, returning true if their differences are
  ///        within a set tolerance.
  NM_INLINE bool compare(const NMP::Matrix34& m, const float tolerance) const;

  /// \ingroup Maths
  /// \brief This is more for unit testing and debugging than anything else, but it compares
  ///        the translational contents of one matrix with another, returning true if their differences are
  ///        within a set tolerance.
  NM_INLINE bool compareTranslation(const NMP::Matrix34& m, const float tolerance) const;

  /// \ingroup Maths
  /// \brief This is more for unit testing and debugging than anything else, but it compares
  ///        the rotational contents of one matrix with another, returning true if their differences are
  ///        within a set tolerance.
  NM_INLINE bool compareRotation(const NMP::Matrix34& m, const float tolerance) const;

  /// \ingroup Maths
  /// \brief Compares matrix against another with 0 tolerance.
  NM_INLINE bool operator==(const NMP::Matrix34& rhs) const;

  /// \ingroup Maths
  /// \brief Return the specified column from the 3x3 component of the matrix as a Vector3,
  ///        <tt>index</tt> being a value between 0 and 2;
  NM_INLINE Vector3 getColumn(uint32_t index) const;

  /// \ingroup Maths
  /// \brief Sets the specified column from the 3x3 component of the matrix to the input Vector3,
  ///        <tt>index</tt> being a value between 0 and 2;
  NM_INLINE void setColumn(const NMP::Vector3& v, uint32_t index);

  /// \ingroup Maths
  /// \brief Return the determinant of the 3x3 component of the matrix
  NM_INLINE float determinant() const;

  /// \ingroup Maths
  /// \brief Test to see if this matrix's rotational component is an identity matrix,
  ///        within a given tolerance
  NM_INLINE bool isIdentity3x3(float tolerance) const;

  /// \ingroup Maths
  /// \brief Test to see if this matrix is an identity matrix, within a given tolerance
  NM_INLINE bool isIdentity(float tolerance) const;

  /// \ingroup Maths
  /// \brief Test to see if this matrix is a valid rotation matrix, i.e. the 3x3 component
  ///        must be orthonormal, with a determinant of +1, (doesn't check translation is finite).
  NM_INLINE bool isValidTM3x3(float tolerance) const;

  /// \ingroup Maths
  /// \brief Test to see if this matrix is a valid transform matrix, i.e. the 3x3 component
  ///        must be orthonormal, with a determinant of +1, and the translation is finite.
  NM_INLINE bool isValidTM(float tolerance) const;

  /// \ingroup Maths
  /// \brief Test to see if the 3x3 component is diagonal with identical values.
  NM_INLINE bool isIsotropic() const;

  /// \ingroup Maths
  /// \brief Set the 3x3 component of this matrix to the identity, translation is untouched
  NM_INLINE void identity3x3();

  /// \ingroup Maths
  /// \brief Set entire matrix to the identity
  NM_INLINE void identity();

  /// \ingroup Maths
  /// \brief Set the 3x3 component of this matrix to zero, translation is untouched
  NM_INLINE void zero3x3();

  /// \ingroup Maths
  /// \brief Set entire matrix to the zero
  NM_INLINE void zero();

  /// \ingroup Maths
  /// \brief Sets the 3x3 component of this matrix to the contents of the
  ///        first three rows of the input matrix
  NM_INLINE void set3x3(const NMP::Matrix34& m);

  /// \ingroup Maths
  /// \brief Sets the entire matrix to the contents of the input matrix
  NM_INLINE void set(const NMP::Matrix34& m);

  /// \ingroup Maths
  /// \brief Set the 3x3 component of the matrix to be a rotation of <tt>radians</tt>
  ///        around the X axis.
  NM_INLINE void set3x3ToXRotation(float angleInRadians);

  /// \ingroup Maths
  /// \brief Set the 3x3 component of the matrix to be a rotation of <tt>radians</tt>
  ///        around the Y axis.
  NM_INLINE void set3x3ToYRotation(float angleInRadians);

  /// \ingroup Maths
  /// \brief Set the 3x3 component of the matrix to be a rotation of <tt>radians</tt>
  ///        around the Z axis.
  NM_INLINE void set3x3ToZRotation(float angleInRadians);

  /// \ingroup Maths
  /// \brief Add two matrices together, place result in this matrix
  NM_INLINE void add(const NMP::Matrix34& a, const NMP::Matrix34& b);

  /// \ingroup Maths
  /// \brief Add the input matrix to current matrix, place result in this matrix
  NM_INLINE void add(const NMP::Matrix34& m);

  /// \ingroup Maths
  /// \brief Add the 3x3 component of the input matrix to current matrix, place result in this matrix
  NM_INLINE void add3x3(const NMP::Matrix34& m);

  /// \ingroup Maths
  /// \brief Subtracts two matrices, a - b, place result in this matrix
  NM_INLINE void subtract(const NMP::Matrix34& a, const NMP::Matrix34& b);

  /// \ingroup Maths
  /// \brief Subtracts the input matrix from the current matrix, place result in this matrix
  NM_INLINE void subtract(const NMP::Matrix34& m);

  /// \ingroup Maths
  /// \brief 3x4 Multiply two matrices together, place result in this matrix
  NM_INLINE void multiply(const NMP::Matrix34& a, const NMP::Matrix34& b);

  /// \ingroup Maths
  /// \brief 3x4 Multiply this matrix with the input matrix, place result in this matrix
  NM_INLINE void multiply(const NMP::Matrix34& m);

  /// \ingroup Paths
  /// \brief 4x4 Multiply two matrices together, place result in this matrix.
  /// Be aware that this function utilises the shear component of the matrix, some other functions
  /// in this class assume that these members are unused. 
  NM_INLINE void multiply4x4(const NMP::Matrix34& a, const NMP::Matrix34& b);

  /// \ingroup Maths
  /// \brief 3x3 Multiply two matrices together, place result in this matrix
  NM_INLINE void multiply3x3(const NMP::Matrix34& a, const NMP::Matrix34& b);

  /// \ingroup Maths
  /// \brief 3x3 Multiply this matrix with the input matrix, place result in this matrix
  NM_INLINE void multiply3x3(const NMP::Matrix34& m);

  /// \ingroup Maths
  /// \brief Transpose the 3x3 component of this matrix
  NM_INLINE void transpose3x3();

  /// \ingroup Maths
  /// \brief Transpose the entire of this matrix
  NM_INLINE void transpose4x4();

  /// \ingroup Maths
  /// \brief Set the 3x3 component of this matrix to be the transpose of the 3x3 component
  ///        of the input matrix
  NM_INLINE void transpose3x3(const NMP::Matrix34& m);

  /// \ingroup Maths
  /// \brief Invert the 3x3 components of this matrix,
  ///        note this transposes the rotation to invert it, will not work with matrices with scaling
  NM_INLINE void invertFast3x3();

  /// \ingroup Maths
  /// \brief Invert this matrix,
  ///        note this transposes the rotation to invert it, will not work with matrices with scaling
  NM_INLINE void invertFast();

  /// \ingroup Maths
  /// \brief Set this matrix to be the inverse of the 3x3 components of the input matrix,
  ///        note this transposes the rotation to invert it, will not work with matrices with scaling
  NM_INLINE void invertFast3x3(const NMP::Matrix34& m);

  /// \ingroup Maths
  /// \brief Set this matrix to be the inverse of the input matrix,
  ///        note this transposes the rotation to invert it, will not work with matrices with scaling
  NM_INLINE void invertFast(const NMP::Matrix34& m);

  /// \ingroup Maths
  /// \brief Invert the 3x3 components of this matrix, returns false if matrix is singular. If
  ///        the function fails and false IS returned, the original contents will not be modified.
  NM_INLINE bool invert3x3();

  /// \ingroup Maths
  /// \brief Invert this matrix, returns false if matrix is singular. If
  ///        the function fails and false IS returned, the original contents will not be modified.
  NM_INLINE bool invert();

  /// \ingroup Maths
  /// \brief Set this matrix to be the inverse of the 3x3 components of the input matrix,
  ///        returns false if matrix is singular. If the function fails and false IS returned,
  ///        the original contents will not be modified.
  NM_INLINE bool invert3x3(const NMP::Matrix34& m);

  /// \ingroup Maths
  /// \brief Set this matrix to be the inverse of the input matrix, returns false if matrix is singular. If
  ///        the function fails and false IS returned, the original contents will not be modified.
  NM_INLINE bool invert(const NMP::Matrix34& m);

  /// \ingroup Maths
  /// \brief Scale this matrix by the amount specified
  NM_INLINE void scale(float f);

  /// \ingroup Maths
  /// \brief Scale this matrix by the amount specified
  NM_INLINE void scale(const NMP::Vector3& v);

  /// \ingroup Maths
  /// \brief Scale the 3x3 components of this matrix by the amount specified
  NM_INLINE void scale3x3(float f);

  /// \ingroup Maths
  /// \brief Scale the 3x3 components of this matrix by the amount specified
  NM_INLINE void scale3x3(const NMP::Vector3& v);

  /// \ingroup Maths
    /// \brief Transform the input vector3 by the 3*3 attitude component of the matrix.
  NM_INLINE void transformVector(NMP::Vector3& v) const;

  /// \ingroup Maths
  /// \brief Transform the input vector by the full matrix and return it
  NM_INLINE Vector3 getTransformedVector(const NMP::Vector3& v) const;

  /// \ingroup Maths
  /// \brief Transform the input vector by the 3x3 rotation component of this matrix
  NM_INLINE void rotateVector(NMP::Vector3& v) const;

  /// \ingroup Maths
  /// \brief Transform the input vector by the 3x3 rotation component of this matrix and return it
  NM_INLINE Vector3 getRotatedVector(const NMP::Vector3& v) const;

  /// \ingroup Maths
  /// \brief Transform the input vector3 by the inverse of the 3*3 attitude component of the matrix.
  NM_INLINE void inverseTransformVector(NMP::Vector3& v) const;

  /// \ingroup Maths
  /// \brief Transform the input vector by the transpose of the 3x3 rotation component of this matrix
  NM_INLINE void inverseRotateVector(NMP::Vector3& v) const;

  /// \ingroup Maths
  /// \brief Transform the in vector3 by the 3*3 attitude component of the matrix and store the result in out.
  NM_INLINE void transformVector(const NMP::Vector3& in, NMP::Vector3& out) const;

  /// \ingroup Maths
  /// \brief Transform the input vector by the full 4x4 matrix.
  NM_INLINE void transformVectorFull(NMP::Vector3& in, NMP::Vector3& out) const;

  /// \ingroup Maths
  /// \brief Transform the input vector by the 3x3 rotation component of this matrix
  NM_INLINE void rotateVector(const NMP::Vector3& in, NMP::Vector3& out) const;

  /// \ingroup Maths
  /// \brief Transform the input vector by the inverse of the full matrix
  NM_INLINE void inverseTransformVector(const NMP::Vector3& in, NMP::Vector3& out) const;

  /// \ingroup Maths
  /// \brief Transform the input vector by the transpose of the 3x3 rotation component of this matrix
  NM_INLINE void inverseRotateVector(const NMP::Vector3& in, NMP::Vector3& out) const;

  /// \ingroup Maths
  /// \brief
  NM_INLINE void orthonormalise();

  /// \ingroup Maths
  /// \brief Interpolate between the two matrices provided by distance t,
  ///        t being in the range 0.0f .. 1.0f; this converts the rotation component
  ///        of the matrices into quaternions, slerps them, then linearly interpolates
  ///        the translation via NMP::Vector4::lerp
  NM_INLINE void interpolate(const NMP::Matrix34& a, const NMP::Matrix34& b, float t);

  /// \ingroup Maths
  /// \brief Interpolate between this matrix and the provided matrix, by distance t,
  ///        t being in the range 0.0f .. 1.0f; see NMP::Matrix34::interpolate(a, b, t) for notes.
  NM_INLINE void interpolate(const NMP::Matrix34& m, float t);

  /// \ingroup Maths
  /// \brief Interpolate between the two matrices provided by distance defined by
  ///        the given weights along each degree of freedom, each in the range 0.0f .. 1.0f;
  ///        wtx, wty and wtz refer to translational dofs, and the others to rotational dofs.
  ///        The implementation uses rotation vectors to do independent lerps on each
  ///        rotational dof.
  NM_INLINE void interpolate6dof(
    const NMP::Matrix34& a, const NMP::Matrix34& b,
    float wtx, float wty, float wtz,
    float wrx, float wry, float wrz);

  /// \ingroup Maths
  /// \brief Interpolate between this matrix and the provided matrix by distance
  ///        defined by the given weights along each degree of freedom, each in the range
  ///        0.0f .. 1.0f; see NM::Matrix34::interpolate(a, b, wtx, wty, wtz, wrx, wry, wrz)
  ///        for notes.
  NM_INLINE void interpolate6dof(
    const NMP::Matrix34& m,
    float wtx, float wty, float wtz,
    float wrx, float wry, float wrz);

  /// \ingroup Maths
  /// \brief Construct the 3x3 rotation component of this matrix from the euler angles
  ///        provided, applied in order X, Y, Z. Translation is untouched.
  NM_INLINE void fromEulerXYZ(const NMP::Vector3& eulers);

  /// \ingroup Maths
  /// \brief Returns the 3x3 rotation component converted to a set of euler angles, in
  ///        rotation order X, Y, Z. The results are undefined if this matrix is not orthonormal.
  NM_INLINE Vector3 toEulerXYZ() const;

  /// \ingroup Maths
  /// \brief Construct the 3x3 rotation component of this matrix from a quaternion,
  ///        translation is untouched
  NM_INLINE void fromQuat(const Quat& quat);

  /// \ingroup Maths
  /// \brief Returns the 3x3 rotation component converted to a quaternion
  NM_INLINE Quat toQuat() const;

  /// \ingroup Maths
  /// \brief Construct the 3x3 rotation component from a rotation vector
  NM_INLINE void fromRotationVector(const NMP::Vector3& r, bool tanQuarterAngle = false);

  /// \ingroup Maths
  /// \brief Convert the 3x3 rotation component to a rotation vector
  NM_INLINE Vector3 toRotationVector(bool tanQuarterAngle = false) const;

  /// \ingroup Maths
  /// \brief Set values based on contents of floating-point arrays.  Row/Column Major
  ///        Stride N means Row/Column index of the array changes every N values
  NM_INLINE void setColumnMajorStride4(float* const M);
  NM_INLINE void setRowMajorStride4(float* const M);

  /// \ingroup Maths
  /// \brief Fill contents of a floating-point array from the matrix.  Row/Column
  ///        Major Stride N means Row/Column index of the array changes every N values
  NM_INLINE void getColumnMajorStride4(float* M) const;
  NM_INLINE void getRowMajorStride4(float* M) const;

} NMP_ALIGN_SUFFIX(16);

// creation of common matrix types
NM_INLINE static Matrix34 Matrix34Identity()  { return Matrix34(Matrix34::kIdentity); }
NM_INLINE static Matrix34 Matrix34Zero()      { return Matrix34(Matrix34::kZero); }

}

#include "NMPlatform/generic/NMMatrix34.inl"

#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_MATRIX34_H
//----------------------------------------------------------------------------------------------------------------------
