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

// cross platform vpu matrix4 and TM layer

// note: as column-major and row-major are terms referring to array data storage,
// (perhaps with the legacy of array subscripts being read as the mathematical convention
// of [row, column] hence forcing a visualization, leading to these terms) and as C is
// row-major, there will be no use of the terms here. Furthermore as the choice of
// visualization as row or column order matrices is independent of the underlying
// mathematics we need only store the basis vectors for the matrix, with the
// understanding that the matrix functions will be operating left to right.
//
//-----------------------------------------------------------------------------------------------------------------------

#define vpuTM(v) *(NMP::vpu::Matrix*)&(v)
#define M34vpu(v)  *(NMP::Matrix34*)&(v)

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUMP
/// \class NMP::vpu::Matrix
///
/// \brief A 4*4 Matrix class with transformation matrix functionality,
/// here * functions are ordered as pre-multiply, with a *= b as a = a(b)
///
//----------------------------------------------------------------------------------------------------------------------

class Matrix
{
public:

  union
  {
    struct
    {
      float ve[4][4];  // (vector, element)
    };
    struct
    {
      vector4_t v1;
      vector4_t v2;
      vector4_t v3;
      vector4_t tt;
    };
    vector4_t r[4];
  };

  /// \brief  Default constructor
  ///
  NM_FORCEINLINE Matrix() { };

  /// \brief  Copy constructor
  ///
  NM_FORCEINLINE Matrix(const Matrix& copy);

  /// \brief  Construct a Matrix from basis vectors and a translation vector
  ///
  NM_FORCEINLINE Matrix(const vector4_t& b1, const vector4_t& b2, const vector4_t& b3, const vector4_t& t);
  NM_FORCEINLINE Matrix(const Vector3& b1, const Vector3& b2, const Vector3& b3, const Vector3& t);
  NM_FORCEINLINE Matrix(const NMP::Vector3& b1, const NMP::Vector3& b2, const NMP::Vector3& b3, const NMP::Vector3& t);

  /// \brief  Construct a Matrix from a quaternion
  ///
  NM_FORCEINLINE Matrix(const vector4_t& quat);
  NM_FORCEINLINE Matrix(const Quat& quat);
  NM_FORCEINLINE Matrix(const NMP::Quat& quat);

  /// \brief  Construct a Matrix from a quaternion and a position vector
  ///
  NM_FORCEINLINE Matrix(const vector4_t& quat, const vector4_t& pos);
  NM_FORCEINLINE Matrix(const Quat& quat, const Vector3& pos);
  NM_FORCEINLINE Matrix(const NMP::Quat& quat, const NMP::Vector3& pos);

  /// \brief  overloads for TM operations
  ///
  NM_FORCEINLINE Matrix& operator =(const Matrix& M);

  NM_FORCEINLINE Matrix operator *(const Matrix& M) const;
  NM_FORCEINLINE vector4_t operator *(const vector4_t& vec) const;
  NM_FORCEINLINE Vector3 operator *(const Vector3& vec) const;
  NM_FORCEINLINE Matrix operator *(float s) const;
  NM_FORCEINLINE void operator *=(const Matrix& M);

  NM_FORCEINLINE Matrix operator +(const Matrix& M) const;
  NM_FORCEINLINE Matrix operator -(const Matrix& M) const;

  /// \brief matrix load and store variants
  NM_FORCEINLINE void load(const float* data);
  NM_FORCEINLINE void store(float* data) const;
  NM_FORCEINLINE void fromPxMat34(const float* nm);
  NM_FORCEINLINE void toPxMat34(float* nm) const;

  /// \brief matrix initialize
  NM_FORCEINLINE void identity();
  NM_FORCEINLINE void identity(float scale);
  NM_FORCEINLINE void setRotation(const Matrix& copy);
  NM_FORCEINLINE void setTranslation(const vector4_t& vec);
  NM_FORCEINLINE void setTranslation(const Vector3& vec);

  /// \brief 4*4 matrix fx
  NM_FORCEINLINE void transpose();
  NM_FORCEINLINE void getTranspose(const Matrix& m);

  NM_FORCEINLINE void multiply(float scale);
  NM_FORCEINLINE void multiply(const Matrix& M);
  NM_FORCEINLINE void multiply(const Matrix& M1, const Matrix& M2);
  NM_FORCEINLINE void add(const Matrix& M);
  NM_FORCEINLINE void add(const Matrix& M1, Matrix& M2);
  NM_FORCEINLINE void subtract(const Matrix& M);
  NM_FORCEINLINE void subtract(const Matrix& M1, Matrix& M2);
  NM_FORCEINLINE void multiply(vector4_t& vec) const;
  NM_FORCEINLINE void multiply(Vector3& vec) const;

  /// \brief 3*3 matrix fx
  NM_FORCEINLINE void multiply33(float scale);
  NM_FORCEINLINE void multiply33(const Matrix& M);
  NM_FORCEINLINE void preMultiply33(const Matrix& M);
  NM_FORCEINLINE void multiply33(const Matrix& M1, const Matrix& M2);
  NM_FORCEINLINE void multiplyTranspose33(const Matrix& M1, const Matrix& M2);
  NM_FORCEINLINE void add33(const Matrix& M);
  NM_FORCEINLINE void add33(const Matrix& M1, Matrix& M2);
  NM_FORCEINLINE void subtract33(const Matrix& M);
  NM_FORCEINLINE void subtract33(const Matrix& M1, Matrix& M2);
  NM_FORCEINLINE bool invert33();
  NM_FORCEINLINE bool invert33(const Matrix& M);

  /// \brief TM matrix fx
  NM_FORCEINLINE void tmTranspose();
  NM_FORCEINLINE void tmGetTranspose(const Matrix& m);

  NM_FORCEINLINE void combine(const Matrix& M);
  NM_FORCEINLINE void combine(const Matrix& M1, const Matrix& M2);
  NM_FORCEINLINE void rotate(vector4_t& vec) const;
  NM_FORCEINLINE void rotate(Vector3& vec) const;
  NM_FORCEINLINE void transform(vector4_t& vec) const;
  NM_FORCEINLINE void transform(Vector3& vec) const;
  NM_FORCEINLINE void inverseRotate(vector4_t& vec) const;
  NM_FORCEINLINE void inverseRotate(Vector3& vec) const;
  NM_FORCEINLINE void inverseTransform(vector4_t& vec) const;
  NM_FORCEINLINE void inverseTransform(Vector3& vec) const;
  NM_FORCEINLINE void transformX(const Matrix& M1, const Matrix& M2); // minimizing muls

  NM_FORCEINLINE float tmDet() const;
  NM_FORCEINLINE uint32_t tmIsValid(float tolerance) const;

  NM_FORCEINLINE Quat tmToQuat() const;  // platform specific variants, avoiding branching.
  NM_FORCEINLINE Quat tmToQuatX() const; // a not to be trusted curiosity
  NM_FORCEINLINE Quat tmToQuatFPU() const;  // standard algorithm, for testing

  NM_FORCEINLINE void tmInvertFast();
  NM_FORCEINLINE void tmInvertFast(const Matrix& M);
  NM_FORCEINLINE bool tmInvert();
  NM_FORCEINLINE bool tmInvert(const Matrix& M);
  NM_FORCEINLINE void tmReOrthonormalise(); // high accuracy, fast, but will not orthonormalise garbage
  NM_FORCEINLINE void tmOrthonormalise();   // not high accuracy, similar speed
  NM_FORCEINLINE void tmInterpolate(const Matrix& a, const Matrix& b, float t);

};

//-----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE Matrix& Matrix::operator =(const Matrix& M)
{
  v1 = M.v1;
  v2 = M.v2;
  v3 = M.v3;
  tt = M.tt;
  return *this;
}

NM_FORCEINLINE Matrix Matrix::operator *(const Matrix& M) const
{
  return Matrix(add4f(add4f(mul4f(v1, splatX(M.v1)), mul4f(v2, splatY(M.v1))), mul4f(v3, splatZ(M.v1))),
                add4f(add4f(mul4f(v1, splatX(M.v2)), mul4f(v2, splatY(M.v2))), mul4f(v3, splatZ(M.v2))),
                add4f(add4f(mul4f(v1, splatX(M.v3)), mul4f(v2, splatY(M.v3))), mul4f(v3, splatZ(M.v3))),
                add4f(add4f(add4f(mul4f(v1, splatX(M.tt)), mul4f(v2, splatY(M.tt))), mul4f(v3, splatZ(M.tt))), tt));
}

NM_FORCEINLINE vector4_t Matrix::operator *(const vector4_t& vec) const
{
  return add4f(add4f(add4f(mul4f(v1, splatX(vec)), mul4f(v2, splatY(vec))), mul4f(v3, splatZ(vec))), tt);
}

NM_FORCEINLINE Matrix Matrix::operator *(float s) const
{
  vector4_t vs = set4f(s);
  return Matrix(mul4f(v1, vs), mul4f(v2, vs), mul4f(v3, vs), mul4f(tt, vs));
}

NM_FORCEINLINE void Matrix::operator *=(const Matrix& M)
{
  *this = *this * M;
}

NM_FORCEINLINE Matrix Matrix::operator +(const Matrix& M) const
{
  return Matrix(add4f(v1, M.v1),
                add4f(v2, M.v2),
                add4f(v3, M.v3),
                add4f(tt, M.tt));
}

NM_FORCEINLINE Matrix Matrix::operator -(const Matrix& M) const
{
  return Matrix(sub4f(v1, M.v1),
                sub4f(v2, M.v2),
                sub4f(v3, M.v3),
                sub4f(tt, M.tt));
}

//-----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE Matrix::Matrix(const Matrix& copy)
{
  v1 = copy.v1;
  v2 = copy.v2;
  v3 = copy.v3;
  tt = copy.tt;
}

NM_FORCEINLINE Matrix::Matrix(const NMP::Vector3& b1, const NMP::Vector3& b2, const NMP::Vector3& b3, const NMP::Vector3& t)
{
  NMP_ASSERT(NMP_IS_ALIGNED(&b1, 16));
  NMP_ASSERT(NMP_IS_ALIGNED(&b2, 16));
  NMP_ASSERT(NMP_IS_ALIGNED(&b3, 16));
  NMP_ASSERT(NMP_IS_ALIGNED(&t, 16));

  v1 = load4f((float*)&b1);
  v2 = load4f((float*)&b2);
  v3 = load4f((float*)&b3);
  tt = setwf(load4f((float*)&t), one4f()); // sure?
}

NM_FORCEINLINE Matrix::Matrix(const Vector3& b1, const Vector3& b2, const Vector3& b3, const Vector3& t)
{
  *this = Matrix(b1.vec, b2.vec, b3.vec, t.vec);
}

NM_FORCEINLINE Matrix::Matrix(const vector4_t& b1, const vector4_t& b2, const vector4_t& b3, const vector4_t& t)
{
  v1 = b1;
  v2 = b2;
  v3 = b3;
  tt = t;
}

//-----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE Matrix::Matrix(const NMP::Quat& quat)
{
  NMP_ASSERT(NMP_IS_ALIGNED(&quat, 16));

  vector4_t vq = load4f((float*)&quat);

  *this = Matrix(vq);
}

NM_FORCEINLINE Matrix::Matrix(const Quat& q)
{
  *this = Matrix(q.quat);
}

NM_FORCEINLINE Matrix::Matrix(const vector4_t& quat)
{
  rotFromQuat(v1, v2, v3, quat);
  tt = quat4f(); //setwf(zero4f(), one4f());
}

//-----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE Matrix::Matrix(const NMP::Quat& quat, const NMP::Vector3& pos)
{
  NMP_ASSERT(NMP_IS_ALIGNED(&quat, 16));
  NMP_ASSERT(NMP_IS_ALIGNED(&pos, 16));

  vector4_t vq = load4f((float*)&quat);
  vector4_t vp = load4f((float*)&pos);

  *this = Matrix(vq, vp);
}

NM_FORCEINLINE Matrix::Matrix(const Quat& q, const Vector3& p)
{
  *this = Matrix(q.quat, p.vec);
}

NM_FORCEINLINE Matrix::Matrix(const vector4_t& quat, const vector4_t& pos)
{
  rotFromQuat(v1, v2, v3, quat);
  tt = setwf(pos, one4f());
}

//-----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void Matrix::load(const float* data)
{
  NMP_ASSERT(NMP_IS_ALIGNED(data, 16));
  load4v(data, v1, v2, v3, tt);
}

NM_FORCEINLINE void Matrix::store(float* data) const
{
  NMP_ASSERT(NMP_IS_ALIGNED(data, 16));
  store4v(data, v1, v2, v3, tt);
}

//-----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void Matrix::fromPxMat34(const float* nm)
{
  NMP_ASSERT(NMP_IS_ALIGNED(nm, 16));

  // read
  v1 = load4f(nm);
  v2 = load4f(nm + 4);
  v3 = load4f(nm + 8);

  vector4_t vZero = zero4f();

  // unpack
  v3 = rotL4(v3);
  tt = setwf(v3, one4f());

  v2 = rotL4(v2);
  v3 = setwf(v2, v3);
  v3 = rotL4(v3);

  v2 = rotR4(v2);
  v2 = setwf(v2, v1);
  v2 = rotR4(v2);

  // transpose basis
  transpose4v(v1, v2, v3, vZero);
}

NM_FORCEINLINE void Matrix::toPxMat34(float* nm) const
{
  NMP_ASSERT(NMP_IS_ALIGNED(nm, 16));

  vector4_t m1, m2, m3, mt;

  // transpose basis
  transpose4vConst(m1, m2, m3, mt,
                   v1, v2, v3, zero4f());

  // pack
  m2 = rotL4(m2);
  m3 = swapLoHi(m3);
  m1 = setwf(m1, m2);
  m2 = mixLoHi(m2, m3);
  mt = rotR4(tt);
  m3 = setxf(mt, m3);

  // write
  store4f(nm, m1);
  store4f(nm + 4, m2);
  store4f(nm + 8, m3);
}

//-----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void Matrix::identity()
{
  const vector4_t vZero = zero4f();
  const vector4_t vOne = one4f();

  v1 = setxf(vZero, vOne);
  v2 = setyf(vZero, vOne);
  v3 = setzf(vZero, vOne);
  tt = setwf(vZero, vOne);
}

NM_FORCEINLINE void Matrix::identity(float scale)
{
  const vector4_t vScale = set4f(scale);
  const vector4_t vZero = zero4f();

  v1 = setxf(vZero, vScale);
  v2 = setyf(vZero, vScale);
  v3 = setzf(vZero, vScale);
  tt = setwf(vZero, vScale);
}

NM_FORCEINLINE void Matrix::setRotation(const Matrix& copy)
{
  v1 = copy.v1;
  v2 = copy.v2;
  v3 = copy.v3;
}

NM_FORCEINLINE void Matrix::setTranslation(const Vector3& v)
{
  tt = v.vec;
}

NM_FORCEINLINE void Matrix::setTranslation(const vector4_t& vec)
{
  tt = vec;
}

//-----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void Matrix::transpose()
{
  transpose4v(v1, v2, v3, tt);
}

NM_FORCEINLINE void Matrix::getTranspose(const Matrix& m)
{
  transpose4vConst(v1, v2, v3, tt, m.v1, m.v2, m.v3, m.tt);
}

NM_FORCEINLINE void Matrix::tmTranspose()
{
  // just transpose the rotation component
  vector4_t vZero = zero4f();
  transpose4v(v1, v2, v3, vZero);
}

NM_FORCEINLINE void Matrix::tmGetTranspose(const Matrix& m)
{
  // just transpose the rotation component
  vector4_t vZero = zero4f();
  transpose4vConst(v1, v2, v3, tt, m.v1, m.v2, m.v3, vZero);
  tt = m.tt;
}

//-----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void Matrix::multiply(float scale)
{
  vector4_t vs = set4f(scale);
  v1 = mul4f(v1, vs);
  v2 = mul4f(v2, vs);
  v3 = mul4f(v3, vs);
  tt = mul4f(tt, vs);
}

NM_FORCEINLINE void Matrix::multiply(const Matrix& M)
{
  *this = Matrix(add4f(add4f(add4f(mul4f(v1, splatX(M.v1)), mul4f(v2, splatY(M.v1))), mul4f(v3, splatZ(M.v1))), mul4f(tt, splatW(M.v1))),
                 add4f(add4f(add4f(mul4f(v1, splatX(M.v2)), mul4f(v2, splatY(M.v2))), mul4f(v3, splatZ(M.v2))), mul4f(tt, splatW(M.v2))),
                 add4f(add4f(add4f(mul4f(v1, splatX(M.v3)), mul4f(v2, splatY(M.v3))), mul4f(v3, splatZ(M.v3))), mul4f(tt, splatW(M.v3))),
                 add4f(add4f(add4f(mul4f(v1, splatX(M.tt)), mul4f(v2, splatY(M.tt))), mul4f(v3, splatZ(M.tt))), mul4f(tt, splatW(M.tt))));
}

NM_FORCEINLINE void Matrix::multiply(const Matrix& M1, const Matrix& M2)
{
  *this = Matrix(add4f(add4f(add4f(mul4f(M1.v1, splatX(M2.v1)), mul4f(M1.v2, splatY(M2.v1))), mul4f(M1.v3, splatZ(M2.v1))), mul4f(M1.tt, splatW(M2.v1))),
                 add4f(add4f(add4f(mul4f(M1.v1, splatX(M2.v2)), mul4f(M1.v2, splatY(M2.v2))), mul4f(M1.v3, splatZ(M2.v2))), mul4f(M1.tt, splatW(M2.v2))),
                 add4f(add4f(add4f(mul4f(M1.v1, splatX(M2.v3)), mul4f(M1.v2, splatY(M2.v3))), mul4f(M1.v3, splatZ(M2.v3))), mul4f(M1.tt, splatW(M2.v3))),
                 add4f(add4f(add4f(mul4f(M1.v1, splatX(M2.tt)), mul4f(M1.v2, splatY(M2.tt))), mul4f(M1.v3, splatZ(M2.tt))), mul4f(M1.tt, splatW(M2.tt))));
}

NM_FORCEINLINE void Matrix::add(const Matrix& M)
{
  v1 = add4f(v1, M.v1);
  v2 = add4f(v2, M.v2);
  v3 = add4f(v3, M.v3);
  tt = add4f(tt, M.tt);
}

NM_FORCEINLINE void Matrix::add(const Matrix& M1, Matrix& M2)
{
  v1 = add4f(M1.v1, M2.v1);
  v2 = add4f(M1.v2, M2.v2);
  v3 = add4f(M1.v3, M2.v3);
  tt = add4f(M1.tt, M2.tt);
}

NM_FORCEINLINE void Matrix::subtract(const Matrix& M)
{
  v1 = sub4f(v1, M.v1);
  v2 = sub4f(v2, M.v2);
  v3 = sub4f(v3, M.v3);
  tt = sub4f(tt, M.tt);
}

NM_FORCEINLINE void Matrix::subtract(const Matrix& M1, Matrix& M2)
{
  v1 = sub4f(M1.v1, M2.v1);
  v2 = sub4f(M1.v2, M2.v2);
  v3 = sub4f(M1.v3, M2.v3);
  tt = sub4f(M1.tt, M2.tt);
}

NM_FORCEINLINE void Matrix::multiply(Vector3& v) const
{
  multiply(v.vec);
}

NM_FORCEINLINE void Matrix::multiply(vector4_t& vec) const
{
  vec = add4f(add4f(add4f(mul4f(v1, splatX(vec)), mul4f(v2, splatY(vec))), mul4f(v3, splatZ(vec))), mul4f(tt, splatW(vec)));
}

//-----------------------------------------------------------------------------------------------------------------------

/*
// standard matrix multiply with M1 in row order and M2 transposed to column order (as vectors).
NM_FORCEINLINE void matrix4premul(matrix4_t &M12, const matrix4_t &M1, matrix4_t &M2)
{
transpose4v(M2.r[0], M2.r[1], M2.r[2], M2.r[3]);
M12.r[0] = horzadd4(mul4f(M1.r1, M2.r1), mul4f(M1.r1, M2.r2), mul4f(M1.r1, M2.r3), mul4f(M1.r1, M2.r4));
M12.r[1] = horzadd4(mul4f(M1.r2, M2.r1), mul4f(M1.r2, M2.r2), mul4f(M1.r2, M2.r3), mul4f(M1.r2, M2.r4));
M12.r[2] = horzadd4(mul4f(M1.r3, M2.r1), mul4f(M1.r3, M2.r2), mul4f(M1.r3, M2.r3), mul4f(M1.r3, M2.r4));
M12.r[3] = horzadd4(mul4f(M1.r4, M2.r1), mul4f(M1.r4, M2.r2), mul4f(M1.r4, M2.r3), mul4f(M1.r4, M2.r4));
}
*/

//-----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void Matrix::multiply33(float scale)
{
  vector4_t vs = set4f(scale);
  v1 = mul4f(v1, vs);
  v2 = mul4f(v2, vs);
  v3 = mul4f(v3, vs);
}

NM_FORCEINLINE void Matrix::multiply33(const Matrix& M)
{
  vector4_t vv1 = add4f(add4f(mul4f(v1, splatX(M.v1)), mul4f(v2, splatY(M.v1))), mul4f(v3, splatZ(M.v1)));
  vector4_t vv2 = add4f(add4f(mul4f(v1, splatX(M.v2)), mul4f(v2, splatY(M.v2))), mul4f(v3, splatZ(M.v2)));
  vector4_t vv3 = add4f(add4f(mul4f(v1, splatX(M.v3)), mul4f(v2, splatY(M.v3))), mul4f(v3, splatZ(M.v3)));

  *this = Matrix(vv1, vv2, vv3, tt);
}

NM_FORCEINLINE void Matrix::preMultiply33(const Matrix& M)
{
  vector4_t vv1 = add4f(add4f(mul4f(M.v1, splatX(v1)), mul4f(M.v2, splatY(v1))), mul4f(M.v3, splatZ(v1)));
  vector4_t vv2 = add4f(add4f(mul4f(M.v1, splatX(v2)), mul4f(M.v2, splatY(v2))), mul4f(M.v3, splatZ(v2)));
  vector4_t vv3 = add4f(add4f(mul4f(M.v1, splatX(v3)), mul4f(M.v2, splatY(v3))), mul4f(M.v3, splatZ(v3)));

  *this = Matrix(vv1, vv2, vv3, tt);
}

NM_FORCEINLINE void Matrix::multiply33(const Matrix& M1, const Matrix& M2)
{
  // temp copy for a = a * b etc.
  vector4_t vv1 = add4f(add4f(mul4f(M1.v1, splatX(M2.v1)), mul4f(M1.v2, splatY(M2.v1))), mul4f(M1.v3, splatZ(M2.v1)));
  vector4_t vv2 = add4f(add4f(mul4f(M1.v1, splatX(M2.v2)), mul4f(M1.v2, splatY(M2.v2))), mul4f(M1.v3, splatZ(M2.v2)));
  vector4_t vv3 = add4f(add4f(mul4f(M1.v1, splatX(M2.v3)), mul4f(M1.v2, splatY(M2.v3))), mul4f(M1.v3, splatZ(M2.v3)));

  *this = Matrix(vv1, vv2, vv3, tt);
}

NM_FORCEINLINE void Matrix::multiplyTranspose33(const Matrix& M1, const Matrix& M2)
{
  // temp copy for a = a * b etc.
  vector4_t vv1 = add4f(add4f(mul4f(M1.v1, splatX(M2.v1)), mul4f(M1.v2, splatX(M2.v2))), mul4f(M1.v3, splatX(M2.v3)));
  vector4_t vv2 = add4f(add4f(mul4f(M1.v1, splatY(M2.v1)), mul4f(M1.v2, splatY(M2.v2))), mul4f(M1.v3, splatY(M2.v3)));
  vector4_t vv3 = add4f(add4f(mul4f(M1.v1, splatZ(M2.v1)), mul4f(M1.v2, splatZ(M2.v2))), mul4f(M1.v3, splatZ(M2.v3)));

  *this = Matrix(vv1, vv2, vv3, tt);
}

NM_FORCEINLINE void Matrix::add33(const Matrix& M)
{
  v1 = add4f(v1, M.v1);
  v2 = add4f(v2, M.v2);
  v3 = add4f(v3, M.v3);
}

NM_FORCEINLINE void Matrix::add33(const Matrix& M1, Matrix& M2)
{
  v1 = add4f(M1.v1, M2.v1);
  v2 = add4f(M1.v2, M2.v2);
  v3 = add4f(M1.v3, M2.v3);
}

NM_FORCEINLINE void Matrix::subtract33(const Matrix& M)
{
  v1 = sub4f(v1, M.v1);
  v2 = sub4f(v2, M.v2);
  v3 = sub4f(v3, M.v3);
}

NM_FORCEINLINE void Matrix::subtract33(const Matrix& M1, Matrix& M2)
{
  v1 = sub4f(M1.v1, M2.v1);
  v2 = sub4f(M1.v2, M2.v2);
  v3 = sub4f(M1.v3, M2.v3);
}

NM_FORCEINLINE bool Matrix::invert33()
{
  vector4_t sp2 = swapXZ(v2);
  vector4_t sp3 = swapXZ(v3);

  vector4_t pc1 = swapXY(sp2);
  vector4_t pc2 = swapYZ(sp3);
  vector4_t nc1 = swapYZ(sp2);
  vector4_t nc2 = swapXY(sp3);

  vector4_t vd0 = sub4f(mul4f(pc1, pc2), mul4f(nc1, nc2));

  vector4_t vdt = dot3f(v1, vd0);
  vector4_t rvdt = sel4cmpEQ(vdt, zero4f(), one4f(), rcp4f(vdt));

  sp2 = swapXZ(v3);
  sp3 = swapXZ(v1);

  pc1 = swapXY(sp2);
  pc2 = swapYZ(sp3);
  nc1 = swapYZ(sp2);
  nc2 = swapXY(sp3);

  vector4_t vd1 = sub4f(mul4f(pc1, pc2), mul4f(nc1, nc2));

  sp2 = swapXZ(v1);
  sp3 = swapXZ(v2);

  pc1 = swapXY(sp2);
  pc2 = swapYZ(sp3);
  nc1 = swapYZ(sp2);
  nc2 = swapXY(sp3);

  vector4_t vd2 = sub4f(mul4f(pc1, pc2), mul4f(nc1, nc2));

  v1 = mul4f(vd0, rvdt);
  v2 = mul4f(vd1, rvdt);
  v3 = mul4f(vd2, rvdt);

  tmTranspose();

  return (floatX(vdt) != 0.0f);
}

NM_FORCEINLINE bool Matrix::invert33(const Matrix& M)
{
  *this = M;
  return invert33();
}

//-----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void Matrix::combine(const Matrix& M)
{
  *this = Matrix(add4f(add4f(mul4f(v1, splatX(M.v1)), mul4f(v2, splatY(M.v1))), mul4f(v3, splatZ(M.v1))),
                 add4f(add4f(mul4f(v1, splatX(M.v2)), mul4f(v2, splatY(M.v2))), mul4f(v3, splatZ(M.v2))),
                 add4f(add4f(mul4f(v1, splatX(M.v3)), mul4f(v2, splatY(M.v3))), mul4f(v3, splatZ(M.v3))),
                 add4f(add4f(add4f(mul4f(v1, splatX(M.tt)), mul4f(v2, splatY(M.tt))), mul4f(v3, splatZ(M.tt))), tt));
}

NM_FORCEINLINE void Matrix::combine(const Matrix& M1, const Matrix& M2)
{
  *this = Matrix(add4f(add4f(mul4f(M1.v1, splatX(M2.v1)), mul4f(M1.v2, splatY(M2.v1))), mul4f(M1.v3, splatZ(M2.v1))),
                 add4f(add4f(mul4f(M1.v1, splatX(M2.v2)), mul4f(M1.v2, splatY(M2.v2))), mul4f(M1.v3, splatZ(M2.v2))),
                 add4f(add4f(mul4f(M1.v1, splatX(M2.v3)), mul4f(M1.v2, splatY(M2.v3))), mul4f(M1.v3, splatZ(M2.v3))),
                 add4f(add4f(add4f(mul4f(M1.v1, splatX(M2.tt)), mul4f(M1.v2, splatY(M2.tt))), mul4f(M1.v3, splatZ(M2.tt))), M1.tt));
}

NM_FORCEINLINE void Matrix::rotate(Vector3& v) const
{
  rotate(v.vec);
}

NM_FORCEINLINE void Matrix::rotate(vector4_t& vec) const
{
  vec = add4f(add4f(mul4f(v1, splatX(vec)), mul4f(v2, splatY(vec))), mul4f(v3, splatZ(vec)));
}

NM_FORCEINLINE void Matrix::transform(Vector3& v) const
{
  transform(v.vec);
}

NM_FORCEINLINE void Matrix::transform(vector4_t& vec) const
{
  vec = add4f(add4f(add4f(mul4f(v1, splatX(vec)), mul4f(v2, splatY(vec))), mul4f(v3, splatZ(vec))), tt);
}

NM_FORCEINLINE void Matrix::inverseRotate(Vector3& v) const
{
  inverseRotate(v.vec);
}

NM_FORCEINLINE void Matrix::inverseRotate(vector4_t& vec) const
{
  vec = horzadd3(mul4f(vec, v1), mul4f(vec, v2), mul4f(vec, v3));
}

NM_FORCEINLINE void Matrix::inverseTransform(Vector3& v) const
{
  inverseTransform(v.vec);
}

NM_FORCEINLINE void Matrix::inverseTransform(vector4_t& vec) const
{
  vector4_t ivec = sub4f(vec, tt);
  vec = horzadd3(mul4f(ivec, v1), mul4f(ivec, v2), mul4f(ivec, v3));
}

// minimizing muls
NM_FORCEINLINE void Matrix::transformX(const Matrix& M1, const Matrix& M2)
{
  transpose4v(v1, v2, v3, tt);
  tt = zero4f();
  vector4_t cv1 = add4f(add4f(mul4f(M1.v1, splatX(M2.v1)), mul4f(M1.v2, splatX(M2.v2))), mul4f(M1.v3, splatX(M2.v3)));
  vector4_t cv2 = add4f(add4f(mul4f(M1.v1, splatY(M2.v1)), mul4f(M1.v2, splatY(M2.v2))), mul4f(M1.v3, splatY(M2.v3)));
  vector4_t cv3 = add4f(add4f(mul4f(M1.v1, splatZ(M2.v1)), mul4f(M1.v2, splatZ(M2.v2))), mul4f(M1.v3, splatZ(M2.v3)));
  transpose4v(cv1, cv2, cv3, tt);
  tt = add4f(tt, M2.tt); v1 = cv1; v2 = cv2; v3 = cv3;
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void Matrix::tmReOrthonormalise()
{
  // M -> C*M : C = (3I-(M * M'))/2

  Matrix I; I.identity(); I.multiply33(3.0f);
  Matrix C; C.multiplyTranspose33(*this, *this);
  Matrix T; T.subtract33(I, C);

  C.multiply33(T, *this); C.multiply33(0.5f);

  setRotation(C); // * 0.5f);

  NMP_ASSERT(tmIsValid(0.001f));
}

NM_FORCEINLINE void Matrix::tmOrthonormalise()
{
  vector4_t mZero = zero4f();
  vector4_t mOne = one4f();

  vector4_t defvec = setxf(mZero, mOne);
  v1 = normalise3OrDef(v1, defvec);
  v1 = setwf(v1, mZero);

  //v2 += (v1 * -dot);
  v2 = sub4f(v2, mul4f(v1, dot3f(v1, v2)));

  defvec = setyf(mZero, mOne);
  v2 = normalise3OrDef(v2, defvec);
  v2 = setwf(v2, mZero);

  v3 = setwf(cross3f(v1, v2), mZero);
  tt = setwf(tt, mOne);
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void Matrix::tmInvertFast()
{
  tmTranspose();
  rotate(tt);
  tt = setwf(neg4f(tt), one4f());
}

NM_FORCEINLINE void Matrix::tmInvertFast(const Matrix& M)
{
  tmGetTranspose(M);
  rotate(tt);
  tt = setwf(neg4f(tt), one4f());
}

NM_FORCEINLINE bool Matrix::tmInvert()
{
  inverseRotate(tt);
  tt = setwf(neg4f(tt), one4f());
  return invert33();
}

NM_FORCEINLINE bool Matrix::tmInvert(const Matrix& M)
{
  if (invert33(M))
  {
    //tt = M.tt;
    rotate(tt);
    tt = setwf(neg4f(tt), one4f());
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------------------------------------------------

// vector return, with dot4?
NM_FORCEINLINE float Matrix::tmDet() const
{
  vector4_t sp2 = swapXZ(v2);
  vector4_t sp3 = swapXZ(v3);

  vector4_t pc1 = swapXY(sp2);
  vector4_t pc2 = swapYZ(sp3);
  vector4_t nc1 = swapYZ(sp2);
  vector4_t nc2 = swapXY(sp3);

  vector4_t vdt = dot3f(v1, sub4f(mul4f(pc1, pc2), mul4f(nc1, nc2)));
  return floatX(vdt);
}

//-----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE uint32_t Matrix::tmIsValid(float tolerance) const
{
  vector4_t vtol = set4f(tolerance);

  // get basis lengths
  vector4_t unit1 = mul4f(v1, v1);
  vector4_t unit2 = mul4f(v2, v2);
  vector4_t unit3 = mul4f(v3, v3);

  // and get determinant, pre-horzadd
  vector4_t sp2 = swapXZ(v2);
  vector4_t sp3 = swapXZ(v3);

  vector4_t pc1 = swapXY(sp2);
  vector4_t pc2 = swapYZ(sp3);
  vector4_t nc1 = swapYZ(sp2);
  vector4_t nc2 = swapXY(sp3);

  vector4_t pdt = mul4f(v1, sub4f(mul4f(pc1, pc2), mul4f(nc1, nc2)));

  // prepare units, det for test with 1
  vector4_t units = abs4f(sub4f(horzadd4(unit1, unit2, unit3, pdt), one4f()));

  // and get perpendicular tests
  vector4_t prp1 = mul4f(v1, v2);
  vector4_t prp2 = mul4f(v2, v3);
  vector4_t prp3 = mul4f(v1, v3);
  vector4_t perps = abs4f(horzadd3(prp1, prp2, prp3));

  return elemX(isBound4f(add4f(units, perps), vtol));
}

//-----------------------------------------------------------------------------------------------------------------------

#if defined(NM_HOST_X360) && !defined(NMP_VPU_EMULATION)

NM_FORCEINLINE Quat Matrix::tmToQuat() const
{
  //static const __vector4i signMask = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 };
  vector4_t signMask = __vspltisw(-1); signMask = __vslw(signMask, signMask);

  const vector4_t pvecw = set4i(VPERM_ELEM(0), VPERM_ELEM(1), VPERM_ELEM(2), VPERM_ELEM(7));
  const vector4_t pvecx = set4i(VPERM_ELEM(4), VPERM_ELEM(2), VPERM_ELEM(1), VPERM_ELEM(0));
  const vector4_t pvecy = set4i(VPERM_ELEM(2), VPERM_ELEM(5), VPERM_ELEM(0), VPERM_ELEM(1));
  const vector4_t pvecz = set4i(VPERM_ELEM(1), VPERM_ELEM(0), VPERM_ELEM(6), VPERM_ELEM(2));

  const vector4_t mask = swapXZ(mixLoHi(signMask, zero4f()));  // 0, 1, 1, 0
  //const vector4_t mask = __vrlimi(zero4f(), signMask, 6, 0);
  const vector4_t mhalf = __vcfsx(__vspltisw(1), 1);

  const vector4_t r0 = splatX(v1);
  const vector4_t r1 = splatY(v2);
  const vector4_t r2 = splatZ(v3);

  vector4_t tr = add4f(one4f(), __vxor(r0, mask));
  tr = add4f(tr, __vxor(r1, swapXY(mask)));
  tr = add4f(tr, __vxor(r2, swapXZ(mask)));

  const vector4_t smask = mask4cmpLT(one4f(), splatW(tr));
  const vector4_t shuffle = sel4cmp2LT(r1, r2, r0, pvecx, sel4cmpLT(r2, r1, pvecy, pvecz));
  const vector4_t shuffleQuat = sel4cmpMask(smask, pvecw, shuffle);

  vector4_t cs1 = splatZ(v2);
  cs1 = __vrlimi(cs1, v3, 4, 3);
  cs1 = __vrlimi(cs1, v1, 2, 3);
  vector4_t cs2 = splatY(v3);
  cs2 = __vrlimi(cs2, v1, 4, 1);
  cs2 = __vrlimi(cs2, v2, 2, 2);

  const vector4_t signs = __vperm(__vand(smask, signMask), signMask, shuffle);

  vector4_t qb = add4f(cs1, __vxor(cs2, signs));

  qb = __vperm(qb, tr, shuffleQuat);
  tr = __vperm(tr, tr, splatW(shuffleQuat));

  return Quat(mul4f(qb, mul4f(mhalf, rsqrt4f(tr))));
}

#elif defined(NM_HOST_CELL_SPU) && !defined(NMP_VPU_EMULATION)

NM_FORCEINLINE Quat Matrix::tmToQuat() const
{
  const vector4_t signmask = set4i(0x80000000);

  static const vector unsigned char pvecw = SHUFFLE_XYZD;
  static const vector unsigned char pvecx = SHUFFLE_AZYX;
  static const vector unsigned char pvecy = SHUFFLE_ZBXY;
  static const vector unsigned char pvecz = SHUFFLE_YXCZ;

  const vector4_t mask = swapXZ(mixLoHi(signmask, zero4f()));  // 0, 1, 1, 0
  vector4_t mhalf = set4f(0.5f);

  const vector4_t r0 = splatX(v1);
  const vector4_t r1 = splatY(v2);
  const vector4_t r2 = splatZ(v3);

  vector4_t tr = add4f(one4f(), xor4(r0, mask));
  tr = add4f(tr, xor4(r1, swapXY(mask)));
  tr = add4f(tr, xor4(r2, swapXZ(mask)));

  const vector4_t smask = mask4cmpLT(one4f(), splatW(tr));
  const vector4_t shuffle = sel4cmp2LT(r1, r2, r0, (vector4_t) pvecx, sel4cmpLT(r2, r1, (vector4_t)pvecy, (vector4_t)pvecz));
  const vector4_t shuffleQuat = sel4cmpMask(smask, (vector4_t)pvecw, shuffle);

  vector4_t cs1 = splatZ(v2);
  cs1 = setyf(cs1, splatX(v3));
  cs1 = setzf(cs1, splatY(v1));
  vector4_t cs2 = splatY(v3);
  cs2 = setyf(cs2, splatZ(v1));
  cs2 = setzf(cs2, splatX(v2));

  const vector4_t signs = spu_shuffle(and4(smask, signmask), signmask, (vec_uchar16)shuffle);

  vector4_t qb = add4f(cs1, xor4(cs2, signs));

  qb = spu_shuffle(qb, tr, (vec_uchar16)shuffleQuat);
  tr = spu_shuffle(tr, tr, (vec_uchar16)splatW(shuffleQuat));

  return Quat(mul4f(qb, mul4f(mhalf, rsqrt4f(tr))));
}

#elif defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64) || defined(NMP_VPU_EMULATION)

NM_FORCEINLINE Quat Matrix::tmToQuat() const
{
  const vector4_t signmask = set4i(0x80000000);

  vector4_t mask = swapXZ(mixLoHi(signmask, zero4f())); // 0, 1, 1, 0
  vector4_t mhalf = set4f(0.5f);

  vector4_t r0 = splatX(v1);
  vector4_t r1 = splatY(v2);
  vector4_t r2 = splatZ(v3);

  vector4_t tr = add4f(one4f(), xor4(r0, mask));
  tr = add4f(tr, xor4(r1, swapXY(mask)));               // 1, 0, 1, 0
  tr = add4f(tr, xor4(r2, swapXZ(mask)));               // 1, 1, 0, 0

  vector4_t cs1 = splatZ(v2);
  cs1 = setyf(cs1, splatX(v3));
  cs1 = setzf(cs1, splatY(v1));
  vector4_t cs2 = splatY(v3);
  cs2 = setyf(cs2, splatZ(v1));
  cs2 = setzf(cs2, splatX(v2));

  vector4_t xtr = splatX(tr);
  vector4_t ytr = splatY(tr);
  vector4_t ztr = splatZ(tr);
  vector4_t wtr = splatW(tr);

  vector4_t b4 = setwf(zero4f(), mask4i());
  vector4_t b3 = rotL4(b4);
  vector4_t b2 = swapLoHi(b4);
  vector4_t b1 = swapLoHi(b3);

  vector4_t smask = mask4cmpLT(one4f(), wtr);
  vector4_t selector = sel4cmpMask(smask, b4, sel4cmp2LT(r1, r2, r0, b1, sel4cmpLT(r2, r1, b2, b3)));
  vector4_t signs = sel4cmpMask(smask, smask, selector);

  vector4_t qb = add4f(cs1, neg4f(cs2, signs)); // 681x

  vector4_t vx186 = reverse(qb);               // x186
  vector4_t v1x68 = swapLoHi(qb);              // 1x68
  vector4_t v86x1 = swapLoHi(vx186);           // 86x1

  vector4_t xs = splatX(selector);
  vector4_t ys = splatY(selector);
  vector4_t zs = splatZ(selector);
  vector4_t ws = splatW(selector);

  //qb = [vx186, v1x68, v86x1, qb] * selector;
  //tr = [xtr, ytr, ztr, wtr] * selector;
  tr = or4(or4(and4(xs, xtr), and4(ys, ytr)), or4(and4(zs, ztr), and4(ws, wtr)));
  qb = or4(or4(and4(xs, vx186), and4(ys, v1x68)), or4(and4(zs, v86x1), and4(ws, qb)));

  // set sqrt column x
  qb = sel4cmpMask(selector, tr, qb);

  return Quat(mul4f(qb, mul4f(mhalf, rsqrt4f(tr))));
}

#else // NM_HOST_CELL_PPU

NM_FORCEINLINE Quat Matrix::tmToQuat() const
{
  return tmToQuatFPU();
}

#endif

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE Quat Matrix::tmToQuatFPU() const
{
  float tr, s;

  tr = 1.0f + ve[0][0] + ve[1][1] + ve[2][2];

  Quat quat;

  // check the diagonals.
  if (tr > 1.0f)
  {
    s = sqrtf (tr) * 2.0f;

    quat.w = s * 0.25f;

    quat.x = (ve[1][2] - ve[2][1]) / s;
    quat.y = (ve[2][0] - ve[0][2]) / s;
    quat.z = (ve[0][1] - ve[1][0]) / s;
  }
  else
  {
    if ((ve[0][0] > ve[1][1]) && (ve[0][0] > ve[2][2]))
    { // Column 0:
      s  = sqrtf (1.0f + ve[0][0] - ve[1][1] - ve[2][2]) * 2.0f;
      quat.x = 0.25f * s;
      quat.y = (ve[0][1] + ve[1][0]) / s;
      quat.z = (ve[2][0] + ve[0][2]) / s;
      quat.w = (ve[1][2] - ve[2][1]) / s;
    }
    else if (ve[1][1] > ve[2][2])
    { // Column 1:
      s  = sqrtf (1.0f + ve[1][1] - ve[0][0] - ve[2][2]) * 2.0f;
      quat.x = (ve[0][1] + ve[1][0]) / s;
      quat.y = 0.25f * s;
      quat.z = (ve[1][2] + ve[2][1]) / s;
      quat.w = (ve[2][0] - ve[0][2]) / s;
    }
    else
    { // Column 2:
      s  = sqrtf (1.0f + ve[2][2] - ve[0][0] - ve[1][1]) * 2.0f;
      quat.x = (ve[2][0] + ve[0][2]) / s;
      quat.y = (ve[1][2] + ve[2][1]) / s;
      quat.z = 0.25f * s;
      quat.w = (ve[0][1] - ve[1][0]) / s;
    }
  }
  return quat;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Matrix::tmInterpolate(const Matrix& a, const Matrix& b, float t)
{
  // Short-cut blend
  if (t == 0.0f)
    *this = a;
  else if (t == 1.0f)
    *this = b;
  else
  {
    // create quats from the matrix positions, slerp them
    Quat aRot = a.tmToQuat();
    Quat bRot = b.tmToQuat();
    vector4_t vt = set4f(t);

    vector4_t dt = dot4f(aRot.quat, bRot.quat);
    vector4_t dta = abs4f(dt);

    bRot.quat = neg4f(bRot.quat, dt);
    vector4_t result = fastSlerp2(aRot.quat, bRot.quat, vt, dta);

    // lerp the position
    tt = lerpMP4(a.tt, b.tt, vt);
    tt = setwf(tt, one4f());

    *this = Matrix(result, tt);
  }
}

//-----------------------------------------------------------------------------------------------------------------------

/*
Because quaternions cannot represent a reflection component,
the matrix must be special orthogonal. For a special orthogonal
matrix, 1 + trace is always positive.

quaternion.x = sqrt(max(0, 1 + m00 - m11 - m22)) / 2;
quaternion.y = sqrt(max(0, 1 - m00 + m11 - m22)) / 2;
quaternion.z = sqrt(max(0, 1 - m00 - m11 + m22)) / 2;
quaternion.w = sqrt(max(0, 1 + m00 + m11 + m22)) / 2;

Q.x = _copysign(Q.x, m12 - m21)
Q.y = _copysign(Q.y, m20 - m02)
Q.z = _copysign(Q.z, m01 - m10)
*/

NM_FORCEINLINE Quat Matrix::tmToQuatX() const // unstable and poor accuracy sometimes
{
  vector4_t vq   = one4f();
  vector4_t half = set4f(0.5f);
  vector4_t mask = swapXZ(mixLoHi(zero4f(), mask4i()));  // 1, 0, 0, 1

  vector4_t r0 = splatX(v1);
  vector4_t r1 = splatY(v2);
  vector4_t r2 = splatZ(v3);

  vq = add4f(vq, sel4cmpMask(mask, r0, neg4f(r0)));
  mask = swapXY(mask); // 0, 1, 0, 1
  vq = add4f(vq, sel4cmpMask(mask, r1, neg4f(r1)));
  mask = swapYZ(mask); // 0, 0, 1, 1
  vq = add4f(vq, sel4cmpMask(mask, r2, neg4f(r2)));

  // half *= sign mask of (m12 - m21, m20 - m02, m01 - m10, +1)
  vector4_t cs1 = splatY(v3);
  cs1 = setyf(cs1, splatZ(v1));
  cs1 = setzf(cs1, splatX(v2));

  vector4_t cs2 = splatZ(v1);
  cs2 = setyf(cs2, splatX(v3));
  cs2 = setzf(cs2, splatY(v1));

  cs2 = setwf(cs2, cs1);
  half = sel4cmpLT(cs2, cs1, neg4f(half), half);

  return Quat(mul4f(sqrt4f(sel4cmpLT(vq, zero4f(), zero4f(), vq)), half));
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
