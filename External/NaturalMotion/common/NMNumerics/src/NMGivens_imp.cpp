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
// Specializes template implementations for the type ScalarType.
#ifndef ScalarType
  #error This file should not be compiled directly
#endif

// Check if the tolerances have been defined
#ifndef NMGIVENS_ETOL
  #error Machine precision tolerance undefined
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// Givens
//----------------------------------------------------------------------------------------------------------------------
template <>
Givens<ScalarType>::~Givens()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Givens<ScalarType>::Givens(const Matrix<ScalarType>& A) :
  m_d(A.numColumns()), m_s(A.numColumns()),
  m_x(A.numColumns()), m_v(A.numColumns()), m_z(A.numColumns())
{
  setT(A);
  setETol(NMGIVENS_ETOL);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Givens<ScalarType>::Givens(const Vector<ScalarType>& d, const Vector<ScalarType>& s) :
  m_d(d.numElements()), m_s(d.numElements()),
  m_x(d.numElements()), m_v(d.numElements()), m_z(d.numElements())
{
  setT(d, s);
  setETol(NMGIVENS_ETOL);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Givens<ScalarType>::Givens(uint32_t n) :
  m_d(n), m_s(n), m_x(n), m_v(n), m_z(n)
{
  setETol(NMGIVENS_ETOL);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Givens<ScalarType>::givens(ScalarType a, ScalarType b, ScalarType& c, ScalarType& s)
{
  if (b == 0)
  {
    c = 1; s = 0;
    return;
  }

  if (fabs(b) >= fabs(a))
  {
    ScalarType tau = -a / b;
    s = 1 / sqrt(1 + tau * tau);
    c = s * tau;
  }
  else
  {
    ScalarType tau = -b / a;
    c = 1 / sqrt(1 + tau * tau);
    s = c * tau;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Givens<ScalarType>::jacobi(ScalarType a11, ScalarType a12, ScalarType a22, ScalarType& c, ScalarType& s)
{
  if (a12 == 0)
  {
    c = 1; s = 0;
    return;
  }

  ScalarType t, tau = (a22 - a11) / (2 * a12);
  if (tau >= 0)
  {
    t = 1 / (tau + sqrt(1 + tau * tau));
  }
  else
  {
    t = -1 / (-tau + sqrt(1 + tau * tau));
  }
  c = 1 / sqrt(1 + t * t);
  s = t * c;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Givens<ScalarType>::implicitQRStep(uint32_t p, uint32_t q, Matrix<ScalarType>* pV)
{
  uint32_t n = m_d.numElements();
  NMP_ASSERT(q > p && q <= n);

  // Index adjust: start index of the tail 2 by 2 matrix block
  q -= 2;

  // Compute the Wilkinson shift Eigenvalue
  ScalarType mu, d = m_d[q] - m_d[q+1];
  if (d == 0)
  {
    // Jacobi rotation: c = s = 1/sqrt(2)
    mu = m_d[q] + m_s[q];
  }
  else
  {
    d *= 0.5;

    // T(2,2) - T(2,1)^2 / (d + sign(d)*sqrt(d^2 + T(2,1)^2));
    ScalarType k = m_s[q] / d;
    mu = m_d[q+1] - k * (m_s[q] / (1 + sqrt(1 + k * k)));
  }

  // Compute the initial implicit shift parameters
  ScalarType x = m_d[p] - mu;
  ScalarType z = m_s[p];

  // Chase the unwanted entry down the diagonal. Note that x is
  // updated to be the next super-diagonal element, while z is
  // updated to be the unwanted element on the 2^nd super-diagonal
  for (uint32_t k = p; k <= q; ++k)
  {
    uint32_t kp1 = k + 1;

    // Compute the Givens rotation G_k that zeros z
    ScalarType c, s;
    givens(x, z, c, s);

    // Apply the Givens rotation T' = G_k^T * T * G_k
    if (k > p) m_s[k-1] = c * m_s[k-1] - s * z; // Modifies m_s[k-1]
    ScalarType M[4];
    M[0] = m_d[k] * c - m_s[k] * s; M[1] = m_d[k] * s + m_s[k] * c;
    M[2] = m_s[k] * c - m_d[kp1] * s; M[3] = m_s[k] * s + m_d[kp1] * c;
    m_d[k] = c * M[0] - s * M[2];
    m_s[k] = c * M[1] - s * M[3];
    m_d[kp1] = s * M[1] + c * M[3];
    if (k < q)
    {
      z = -s * m_s[kp1]; // Uses m_s[k+1]
      m_s[kp1] *= c; // Modifies m_s[k+1]
      x = m_s[k];
    }

    // Update the orthogonal transform V
    if (pV)
    {
      Matrix<ScalarType>& V = *pV;
      NMP_ASSERT(V.isSize(n, n));

      // Get the k and k+1 ^th columns of V
      V.getColumnVector(k, m_x);
      V.getColumnVector(kp1, m_v);

      // Update V(:,k)
      for (uint32_t i = 0; i < n; ++i) m_z[i] = c * m_x[i] - s * m_v[i];
      V.setColumnVector(k, m_z);

      // Update V(:,k+1)
      for (uint32_t i = 0; i < n; ++i) m_z[i] = s * m_x[i] + c * m_v[i];
      V.setColumnVector(kp1, m_z);
    }
  }

}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Givens<ScalarType>::implicitSVDStep(uint32_t p, uint32_t q, Matrix<ScalarType>* pU, Matrix<ScalarType>* pV)
{
  uint32_t n = m_d.numElements();
  NMP_ASSERT(q > p && q <= n);

  // Index adjust: start index of the tail 2 by 2 matrix block
  q -= 2;

  // Compute the tri-diagonal matrices T = B^T * B for the head
  // and tail 2 by 2 matrix blocks

  ScalarType t[5]; // [d_1, s_1, d_{n-1}, s_{n-1}, d_n]
  t[0] = m_d[p]; t[1] = m_s[p];
  t[2] = m_d[q]; t[3] = m_s[q]; t[4] = m_d[q+1];

  // Find largest absolute scale of the elements
  ScalarType scale = fabs(t[0]);
  for (uint32_t i = 1; i < 5; ++i)
  {
    ScalarType s = fabs(t[i]);
    if (s > scale) scale = s;
  }

  // Normalize the bi-diagonal elements
  for (uint32_t i = 0; i < 5; ++i) t[i] /= scale;

  // Compute T = B^T * B for head block (first row vector only)
  ScalarType h11 = t[0] * t[0];
  ScalarType h12 = t[0] * t[1];

  // Compute T = B^T * B for tail block
  ScalarType t11 = t[2] * t[2];
  ScalarType t12 = t[2] * t[3];
  ScalarType t22 = t[3] * t[3] + t[4] * t[4];

  // Compute the Wilkinson shift Eigenvalue of tail T
  ScalarType mu, d = t11 - t22;
  if (d == 0)
  {
    // Jacobi rotation: c = s = 1/sqrt(2)
    mu = t11 + t12;
  }
  else
  {
    d *= 0.5;

    // T(2,2) - T(2,1)^2 / (d + sign(d)*sqrt(d^2 + T(2,1)^2));
    ScalarType k = t12 / d;
    mu = t22 - k * (t12 / (1 + sqrt(1 + k * k)));
  }

  // Compute the initial implicit shift parameters
  ScalarType x = h11 - mu;
  ScalarType z = h12;

  // Chase the unwanted entry down the diagonal. Note that x is
  // updated to be the next super-diagonal element, while z is
  // updated to be the unwanted element
  for (uint32_t k = p; k <= q; ++k)
  {
    uint32_t kp1 = k + 1;

    // Compute the right Givens rotation G_k that zeros z
    // [x z] * [c s; -s c] = [* 0]
    ScalarType c, s;
    givens(x, z, c, s);

    // Apply the right Givens rotation T' = T * G_k
    if (k > p) m_s[k-1] = m_s[k-1] * c - z * s;
    ScalarType a = m_d[k];
    ScalarType b = m_s[k];
    m_d[k] = a * c - b * s;
    m_s[k] = a * s + b * c;

    z = -m_d[kp1] * s;
    m_d[kp1] *= c;

    // Update the orthogonal transform V
    if (pV)
    {
      Matrix<ScalarType>& V = *pV;
      NMP_ASSERT(V.isSize(n, n));

      // Get the k and k+1 ^th columns of V
      V.getColumnVector(k, m_x);
      V.getColumnVector(kp1, m_v);

      // Update V(:, k)
      for (uint32_t i = 0; i < n; ++i) m_z[i] = c * m_x[i] - s * m_v[i];
      V.setColumnVector(k, m_z);

      // Update V(:, k+1)
      for (uint32_t i = 0; i < n; ++i) m_z[i] = s * m_x[i] + c * m_v[i];
      V.setColumnVector(kp1, m_z);
    }

    // Compute the left Givens rotation G_k that zeros z
    // [c s; -s c]^T * [x; z]= [*; 0]
    givens(m_d[k], z, c, s);

    // Apply the left Givens rotation T' = G_k * T
    m_d[k] = c * m_d[k] - s * z;
    a = m_s[k];
    b = m_d[kp1];
    m_s[k] = c * a - s * b;
    m_d[kp1] = s * a + c * b;

    if (k < q)
    {
      x = m_s[k];
      z = -s * m_s[kp1];
      m_s[kp1] *= c;
    }

    // Update the orthogonal transform U
    if (pU)
    {
      Matrix<ScalarType>& U = *pU;
      NMP_ASSERT(U.isSize(n, n));

      // Get the k and k+1 ^th columns of U
      U.getColumnVector(k, m_x);
      U.getColumnVector(kp1, m_v);

      // Update U(:, k)
      for (uint32_t i = 0; i < n; ++i) m_z[i] = c * m_x[i] - s * m_v[i];
      U.setColumnVector(k, m_z);

      // Update U(:, k+1)
      for (uint32_t i = 0; i < n; ++i) m_z[i] = s * m_x[i] + c * m_v[i];
      U.setColumnVector(kp1, m_z);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Givens<ScalarType>::chaseLeft(uint32_t i, uint32_t q, Matrix<ScalarType>* pU)
{
  uint32_t n = m_d.numElements();
  NMP_ASSERT(q > i && q <= n);

  // Index adjust: index of last element in unreduced block
  q--;

  // Get the initial off diagonal element in row i
  ScalarType z = m_s[i];

  // Zero the super diagonal element
  m_s[i] = 0;

  for (uint32_t k = i + 1; k <= q; ++k)
  {
    ScalarType c, s;
    givens(-m_d[k], z, c, s);

    m_d[k] = s * z + c * m_d[k];
    if (k < q)
    {
      z = -s * m_s[k];
      m_s[k] *= c;
    }

    // Update the orthogonal transform U
    if (pU)
    {
      Matrix<ScalarType>& U = *pU;
      NMP_ASSERT(U.isSize(n, n));

      // Get the i^th and k^th columns of U
      U.getColumnVector(i, m_x);
      U.getColumnVector(k, m_v);

      // Update U(:, i)
      for (uint32_t j = 0; j < n; ++j) m_z[j] = c * m_x[j] - s * m_v[j];
      U.setColumnVector(i, m_z);

      // Update U(:, k)
      for (uint32_t j = 0; j < n; ++j) m_z[j] = s * m_x[j] + c * m_v[j];
      U.setColumnVector(k, m_z);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Givens<ScalarType>::chaseRight(uint32_t i, uint32_t p, Matrix<ScalarType>* pV)
{
  uint32_t n = m_d.numElements();
  NMP_ASSERT(i > p);

  // Get the initial off diagonal element in row i-1
  int32_t k = i - 1;
  ScalarType z = m_s[k];

  // Zero the super diagonal element
  m_s[k] = 0;

  for (; k >= (int32_t)p; --k)
  {
    ScalarType c, s;
    givens(m_d[k], z, c, s);

    m_d[k] = m_d[k] * c - z * s;
    if (k > (int32_t)p)
    {
      z = m_s[k-1] * s;
      m_s[k-1] *= c;
    }

    // Update the orthogonal transform V
    if (pV)
    {
      Matrix<ScalarType>& V = *pV;
      NMP_ASSERT(V.isSize(n, n));

      // Get the i^th and k^th columns of V
      V.getColumnVector(k, m_x);
      V.getColumnVector(i, m_v);

      // Update V(:, k)
      for (uint32_t j = 0; j < n; ++j) m_z[j] = c * m_x[j] - s * m_v[j];
      V.setColumnVector(k, m_z);

      // Update V(:, i)
      for (uint32_t j = 0; j < n; ++j) m_z[j] = s * m_x[j] + c * m_v[j];
      V.setColumnVector(i, m_z);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool Givens<ScalarType>::diagonalizeT(uint32_t max_its, Matrix<ScalarType>* pV)
{
  uint32_t n = m_d.numElements();
  uint32_t its = 0;
  int32_t p, q = n;

  bool status = true;
  while (q > 1)
  {
    // Test for too many iterations
    if (its >= max_its)
    {
      status = false;
      q--; // Move on to next Eigen-value
      its = 0;
      continue;
    }

    // Zero negligible super diagonal elements
    for (p = q - 2; p >= 0; p--)
    {
      // Criteria for smallness of the super-diagonal element:
      // |T(p,p+1)| <= eps*(|T(p, p)| + |T(p, p+1)|)
      if (fabs(m_s[p]) <= m_eTol*(fabs(m_d[p]) + fabs(m_d[p+1])))
      {
        m_s[p] = static_cast<ScalarType>(0);
        break;
      }
    }
    p++; // start index of valid block

    // Check for convergence of the tail element
    if (p == q - 1)
    {
      // Decrement the start index of the diagonalized block
      q--;
      its = 0;
    }
    else
    {
      // Perform a QR step on the p:q block
      implicitQRStep(p, q, pV);

      // Update the iteration count
      its++;
    }
  }

  return status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool Givens<ScalarType>::diagonalizeB(uint32_t max_its, Matrix<ScalarType>* pU, Matrix<ScalarType>* pV)
{
  uint32_t n = m_d.numElements();

  uint32_t its = 0;
  int32_t q = n; // Start index of diagonalized block
  int32_t p; // Start index of the unreduced block

  bool status = true;
  while (q > 0)
  {
    // Test for too many iterations
    if (its >= max_its)
    {
      status = false;
      q--; // Move on to next singular value
      its = 0;
      continue;
    }

    // Zero negligible super diagonal elements
    for (p = q - 2; p >= 0; p--)
    {
      // Criteria for smallness of the super-diagonal element:
      // |B(p,p+1)| <= eps*(|B(p, p)| + |B(p, p+1)|)
      if (fabs(m_s[p]) <= m_eTol*(fabs(m_d[p]) + fabs(m_d[p+1])))
      {
        m_s[p] = static_cast<ScalarType>(0);
        break;
      }
    }
    p++; // start index of valid block

    // Check for convergence of the tail element
    int32_t qm1 = q - 1;
    if (p == qm1)
    {
      // Make the singular value positive
      if (m_d[p] < 0)
      {
        m_d[p] = -m_d[p];
        if (pV) pV->multiplyColumn(p, static_cast<ScalarType>(-1));
      }

      // Order the singular values
      ScalarType s = m_d[p];
      uint32_t k;
      for (k = p + 1; k < n; ++k)
      {
        if (m_d[k] <= s) break;
      }
      k--; // index to shift singular value to

      if (k > (uint32_t)p)
      {
        // Shift singular values
        for (uint32_t i = p + 1; i <= k; ++i) m_d[i-1] = m_d[i];
        m_d[k] = s;

        // Shift columns of U
        if (pU)
        {
          pU->getColumnVector(p, &m_x[0]);
          for (uint32_t i = p + 1; i <= k; ++i)
          {
            pU->getColumnVector(i, &m_v[0]);
            pU->setColumnVector(i - 1, &m_v[0]);
          }
          pU->setColumnVector(k, &m_x[0]);
        }

        // Shift columns of V
        if (pV)
        {
          pV->getColumnVector(p, &m_x[0]);
          for (uint32_t i = p + 1; i <= k; ++i)
          {
            pV->getColumnVector(i, &m_v[0]);
            pV->setColumnVector(i - 1, &m_v[0]);
          }
          pV->setColumnVector(k, &m_x[0]);
        }
      }

      // Decrement the start index of the diagonalized block
      q--;

      // Zero the iteration count
      its = 0;
    }
    else
    {
      // Index of negligible diagonal in initial unreduced block
      int32_t r;

      // Zero negligible diagonals within the *putative* unreduced block
      for (r = qm1; r >= p; r--)
      {
        // Simple norm of the current and previous super-diagonal elements
        // |B(r-1, r)| + |B(r, r+1)|
        ScalarType t = static_cast<ScalarType>(0);
        if (r > p) t += fabs(m_s[r-1]);
        if (r < qm1) t += fabs(m_s[r]);

        // Criteria for smallness of the diagonal element:
        // |B(r, r)| <= eps*(|B(r-1, r)| + |B(r, r+1)|)
        if (fabs(m_d[r]) <= m_eTol * t)
        {
          m_d[r] = static_cast<ScalarType>(0);
          break;
        }
      }

      // Check if no negligible diagonals were found i.e. the unreduced
      // block is indeed unreduced in p:q (perform QR step on the block)
      if (r < p)
      {
        // Perform a QR step on the p:q block
        implicitSVDStep(p, q, pU, pV);
      }
      else
      {
        // We have found a zero diagonal element. We can *pre-multiply* by
        // a sequence of Givens rotations to chase the super-diagonal
        // element out to the right (update to U).
        if (r < q - 1)
        {
          chaseLeft(r, q, pU);
        }
        else
        {
          // In the case where the zero diagonal corresponds to the tail
          // element we can *post-multiply* by a sequence of Givens rotations
          // to chase the super-diagonal element upward (update to V).
          chaseRight(r, p, pV);
        }
      }
    }

    // Update the iteration count
    its++;
  }

  return status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType>& Givens<ScalarType>::getD()
{
  return m_d;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Vector<ScalarType>& Givens<ScalarType>::getD() const
{
  return m_d;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType>& Givens<ScalarType>::getS()
{
  return m_s;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Vector<ScalarType>& Givens<ScalarType>::getS() const
{
  return m_s;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Givens<ScalarType>::getT(Matrix<ScalarType>& T) const
{
  NMP_ASSERT(T.isSize(m_d.numElements(), m_d.numElements()));

  T.set(static_cast<ScalarType>(0));
  T.setDiagVector(0, &m_d[0]);
  T.setDiagVector(1, &m_s[0]);
  T.setDiagVector(-1, &m_s[0]); // Symmetric side
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Givens<ScalarType>::setT(const Matrix<ScalarType>& T)
{
  uint32_t n = m_d.numElements();

  T.getDiagVector(0, &m_d[0]);
  T.getDiagVector(1, &m_s[0]);
  m_s[n-1] = static_cast<ScalarType>(0);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Givens<ScalarType>::setT(const Vector<ScalarType>& d, const Vector<ScalarType>& s)
{
  uint32_t n = m_d.numElements();
  m_d.setSubVector(0, n, &d[0]);
  m_s.setSubVector(0, n - 1, &s[0]);
  m_s[n-1] = static_cast<ScalarType>(0);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType Givens<ScalarType>::getETol() const
{
  return m_eTol;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Givens<ScalarType>::setETol(ScalarType tol)
{
  m_eTol = tol;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
