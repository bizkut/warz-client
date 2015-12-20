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
#ifndef NM_UNIFORM_QUANTISATION_H
#define NM_UNIFORM_QUANTISATION_H

//----------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMathUtils.h"
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMBitStreamCoder.h"
//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class  UniformQuantisationInfo
/// \brief  Class that stores information about how a sub-signal is quantised
//----------------------------------------------------------------------------------------------------------------------
class UniformQuantisationInfo
{
public:
  UniformQuantisationInfo() {}
  ~UniformQuantisationInfo() {}

  void clear();

public:
  uint32_t  m_precision;
  float     m_qMin;
  float     m_qMax;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class UniformQuantisationInfoSet
/// \brief A class to maintains the quantisation channel information during compounding
//----------------------------------------------------------------------------------------------------------------------
class UniformQuantisationInfoSet
{
public:
  //------------------------------
  // Construct a quantisation set with the required buffer size
  UniformQuantisationInfoSet(
    uint32_t numChannelIndices);

  // Construct a quantisation set by concatenating both input sets
  UniformQuantisationInfoSet(
    const UniformQuantisationInfoSet* qSetA,
    const UniformQuantisationInfoSet* qSetB);

  // Destroy the quantisation set
  ~UniformQuantisationInfoSet();

  // Compute the mean and covariance of the quantisation set distribution
  void computeDistribution(
    uint32_t numCoefSets,
    const UniformQuantisationInfo* quantisationInfo
    );

  // Split the quantisation set into two sub-sets given the current distribution
  bool splitDistribution(
    uint32_t numCoefSets,
    const UniformQuantisationInfo* quantisationInfo,
    uint32_t* channelIndexBuffer,
    UniformQuantisationInfoSet*& qSubSetA,
    UniformQuantisationInfoSet*& qSubSetB) const;

  // Redistribute the quantisation set into two sub-sets
  void redistribute(
    UniformQuantisationInfoSet*& qSubSetA,
    UniformQuantisationInfoSet*& qSubSetB) const;

  // Compute the [qMin, qMax] bracket for the channels
  void computeQMinMax(
    uint32_t numCoefSets,
    const UniformQuantisationInfo* quantisationInfo
    );

  // Compute the merge cost by combining the [qMin, qMax] bracket with another set
  NM_INLINE float mergeCost(
    const UniformQuantisationInfoSet* qSet
    ) const;

  //------------------------------
  // Accessors and modifiers
  NM_INLINE float getQVar() const;

  NM_INLINE uint32_t getNumChannelIndices() const;

  NM_INLINE uint32_t getChannelIndex(uint32_t i) const;

  NM_INLINE void setChannelIndex(
    uint32_t i,
    uint32_t channelIndex);

  NM_INLINE float getQMin() const;
  NM_INLINE float getQMax() const;

private:
  // Channel indices that are contained within the bounding rectangle
  uint32_t              m_numChannelIndices;
  uint32_t*             m_channelIndices;

  // The quantisation set distribution
  float                 m_qBar[2];  // The mean of the distribution  
  float                 m_qVp[2];   // Principle direction (most variant)
  float                 m_qVar;     // Variance of the principal direction

  // The quantisation set bracket
  float                 m_qMin;
  float                 m_qMax;
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float UniformQuantisationInfoSet::mergeCost(
  const UniformQuantisationInfoSet* qSet) const
{
  return NMP::nmfabs(m_qMin - qSet->m_qMin) + NMP::nmfabs(m_qMax - qSet->m_qMax);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float UniformQuantisationInfoSet::getQVar() const
{
  return m_qVar;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t UniformQuantisationInfoSet::getNumChannelIndices() const
{
  return m_numChannelIndices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t UniformQuantisationInfoSet::getChannelIndex(uint32_t i) const
{
  NMP_VERIFY(i < m_numChannelIndices);
  return m_channelIndices[i];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void UniformQuantisationInfoSet::setChannelIndex(
  uint32_t i,
  uint32_t channelIndex)
{
  NMP_VERIFY(i < m_numChannelIndices);
  m_channelIndices[i] = channelIndex;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float UniformQuantisationInfoSet::getQMin() const
{
  return m_qMin;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float UniformQuantisationInfoSet::getQMax() const
{
  return m_qMax;
}

//----------------------------------------------------------------------------------------------------------------------
/// \class  UniformQuantisationInfoCompounder
/// \brief  A class that compounds the input qMin, qMax quantisation set information
///         into a smaller number of sets by optimally combining the range information
///         of the closest matches until the desired number of sets remain. A greedy
///         method of combining quantisation information is used, so for large sets
///         this algorithm is likely to be quite slow.
//----------------------------------------------------------------------------------------------------------------------
class UniformQuantisationInfoCompounder
{
public:
  //---------------------------------------------------------------------
  UniformQuantisationInfoCompounder(uint32_t numCoefSets);
  ~UniformQuantisationInfoCompounder();

  /// \brief Main function to compound the input set of quantisation
  /// [qMin, qMax] information into a desired number of coefficient sets.
  /// The algorithm partitions the [qMin, qMax] space along directions of
  /// greatest variance. It is possible that fewer quantisation sets will
  /// be required than the desired number.
  ///
  /// \param redistribute If true then the algorithm will enforce that
  /// there are precisely desiredNumCoefSets by adding additional sets
  /// when required. Set assignments are redistributed amongst the
  /// additional sets.
  void compoundQuantisationInfo(
    uint32_t desiredNumCoefSets,
    bool redistribute = true);

  //---------------------------------------------------------------------
  // Input quantisation info
  uint32_t getNumCoefSets() const;
  void getQuantisationInfo(uint32_t coefSetIndex, float& qMin, float& qMax) const;
  void setQuantisationInfo(uint32_t coefSetIndex, float qMin, float qMax);

  const uint32_t* getMapFromInputToCompounded() const;
  uint32_t getMapFromInputToCompounded(uint32_t coefSetIndex) const;

  //---------------------------------------------------------------------
  // Compounded quantisation info
  uint32_t getNumCoefSetsCompounded() const;
  void getQuantisationInfoCompounded(uint32_t compoundedCoefSetIndex, float& qMin, float& qMax) const;

protected:
  // Input quantisation set information
  uint32_t                          m_numCoefSets;
  UniformQuantisationInfo*          m_quantisationInfo;
  uint32_t*                         m_mapFromInputToCompounded;

  // Compounded quantisation set information
  uint32_t                          m_numCoefSetsCompounded;
  UniformQuantisationInfo*          m_quantisationInfoCompounded;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class  UniformQuantisationCoefSetBase
/// \brief  A base class that maintains information about coefficient set
///         sample data.
//----------------------------------------------------------------------------------------------------------------------
class UniformQuantisationCoefSetBase
{
public:
  //---------------------------------------------------------------------
  UniformQuantisationCoefSetBase(uint32_t maxBitsOfPrecision);
  virtual ~UniformQuantisationCoefSetBase();

  //---------------------------------------------------------------------
  /// \name   Rate and distortion characteristics
  /// \brief  Functions to compute the bit rate and distortion characteristics
  ///         of a coefficient set of data samples.
  //---------------------------------------------------------------------
  //@{

  /// \brief Function to clear the rate and distortion characteristics
  virtual void clearRateAndDistortion();

  /// \brief Main function to compute the quantisation data range and compute
  /// the tables of bit rate and distortion characteristics over the specified
  /// range of quantisation precisions.
  ///
  /// Bit rate - number of bits required to represent the quantised data. We
  /// do not perform further entropy encoding on the quantised data post
  /// quantisation, therefore the bit rate for a quantised sample corresponds
  /// exactly with the bit precision level.
  ///
  /// Distortion - the sum of squared residual errors between the dequantised
  /// and original data samples. Lower quantisation precision generates higher
  /// distortion errors.
  virtual void computeRateAndDistortion() = 0;

  /// \brief After allocating the bits to a coefficient set we adjust the
  /// quantisation information for any coefficient sets given zero precision.
  virtual void updateQuantisationInfo();

  /// \brief Function to print out the rate and distortion characteristics
  void printRateAndDistortion(FILE* filePointer);

  /// \brief Function to modify the distortion errors to be strictly decreasing
  /// as the precision of bits increases
  void makeDistortionConvex();

  /// \brief Function to determine if the distortion error strictly decreases
  /// as the precision of bits increases
  bool isDistortionConvex(float tol = 0.0f) const;

  /// \brief Function to determine if the coefficient set was allocated the
  /// full precision of bits
  bool isMaxPrecision() const;
  //@}

  //---------------------------------------------------------------------
  /// \name   Quantisation functions
  /// \brief  Functions for quantising / dequantising data
  //---------------------------------------------------------------------
  //@{

  /// \brief Function to quantise a set of data samples with the
  /// specified quantisation range and precision.
  virtual void quantise(BitStreamEncoder& encoder) = 0;

  /// \brief Function to dequantise a set of data samples with the
  /// specified quantisation range and precision.
  virtual void dequantise(BitStreamDecoder& decoder) = 0;
  //@}

  //---------------------------------------------------------------------
  /// \name   Data management functions
  /// \brief  Accessors for managing the class data
  //---------------------------------------------------------------------
  //@{
  uint32_t getCoefSetId() const;
  void setCoefSetId(uint32_t id);

  uint32_t getUserID() const;
  void setUserID(uint32_t id);

  uint32_t getUserValue() const;
  void setUserValue(uint32_t userValue);

  void* getUserData() const;
  void setUserData(void* userData);

  const UniformQuantisationInfo& getQuantisationInfo() const;
  UniformQuantisationInfo& getQuantisationInfo();

  float getWeight() const;
  void setWeight(float w);

  uint32_t getNumQuantisers() const;
  const float* getRates() const;
  float* getRates();
  float getRate() const;

  const float* getDistortions() const;
  float* getDistortions();
  float getDistortion() const;

  // Quantisation rate and distortion
  bool getRateDistComputed() const;
  void resetRateDistComputed();
  //@}

protected:
  // Information
  uint32_t                          m_coefSetId;          ///< Index of this coef set by the uniform quanitser

  // User data
  uint32_t                          m_userID;             ///< Id for this coef set determined by the user
  uint32_t                          m_userValue;          ///< Value for this coef set determined by the user
  void*                             m_userData;           ///< User data pointer set by the user

  // Quantisation rate and distortion
  UniformQuantisationInfo           m_qInfo;
  float                             m_weight;

  bool                              m_rateDistComputed;
  uint32_t                          m_numQuantisers;
  float*                            m_rates;
  float*                            m_dists;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class  UniformQuantisationCoefSet
/// \brief  A class that maintains information about coefficient set
///         sample data.
//----------------------------------------------------------------------------------------------------------------------
class UniformQuantisationCoefSet : public UniformQuantisationCoefSetBase
{
public:
  //---------------------------------------------------------------------
  UniformQuantisationCoefSet(uint32_t numSamples, uint32_t maxBitsOfPrecision);
  virtual ~UniformQuantisationCoefSet();

  //---------------------------------------------------------------------
  /// \name   Rate and distortion characteristics
  /// \brief  Functions to compute the bit rate and distortion characteristics
  ///         of a coefficient set of data samples.
  //---------------------------------------------------------------------
  //@{

  /// \brief A function to compute the mean and variance of the coefficient
  /// set data samples.
  void computeStatistics();

  /// \brief Main function to compute the quantisation data range and compute
  /// the tables of bit rate and distortion characteristics over the specified
  /// range of quantisation precisions.
  ///
  /// Bit rate - number of bits required to represent the quantised data. We
  /// do not perform further entropy encoding on the quantised data post
  /// quantisation, therefore the bit rate for a quantised sample corresponds
  /// exactly with the bit precision level.
  ///
  /// Distortion - the sum of squared residual errors between the dequantised
  /// and original data samples. Lower quantisation precision generates higher
  /// distortion errors.
  virtual void computeRateAndDistortion();

  /// \brief A callback function to calculate the rate and distortion errors
  /// generated by quantising the coefficient set data values with the
  /// required precision level.
  virtual void computeRateAndDistortion(uint32_t prec, float& rate, float& dist);

  /// \brief After allocating the bits to a coefficient set we adjust the
  /// quantisation information for any coefficient sets given zero precision.
  /// We adjust the information to represent the mean of the sample data.
  virtual void updateQuantisationInfo();
  //@}

  //---------------------------------------------------------------------
  /// \name   Quantisation functions
  /// \brief  Functions for quantising / dequantising data
  //---------------------------------------------------------------------
  //@{

  /// \brief Function to quantise a set of data samples with the
  /// specified quantisation range and precision.
  virtual void quantise(BitStreamEncoder& encoder);

  /// \brief Function to dequantise a set of data samples with the
  /// specified quantisation range and precision.
  virtual void dequantise(BitStreamDecoder& decoder);
  //@}

  //---------------------------------------------------------------------
  /// \name   Sample data functions
  /// \brief  Functions for managing the sample data
  //---------------------------------------------------------------------
  //@{
  uint32_t getNumSamples() const;
  const float* getSamples() const;
  float* getSamples();

  void setZeroMeanDistribution(bool enable);
  bool getZeroMeanDistribution() const;

  float getCoefMean() const;
  float getCoefVar() const;
  //@}

protected:
  // Sample data
  uint32_t                          m_numSamples;
  float*                            m_data;

  // Statistics of sample data
  bool                              m_zeroMean;
  float                             m_coefMin;
  float                             m_coefMax;
  float                             m_coefMean;
  float                             m_coefVar;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class UniformQuantisation
/// \brief Given a signal x of size N that is divided into n sub-signals x_i
/// of size N_i, find the most efficient distribution of a given bit budget R
/// among a number of quantisers with differing levels of bit precision. An
/// example use would be for image compression, where the representation of an
/// image is first transformed by applying a Discrete Wavelet Transform, or
/// Discrete Cosine Transform. The resulting sub-bands or macro-blocks are
/// then quantised into a user specified number of bytes.
///
/// This class implements the bit optimisation algorithm described in:
/// @article{ shoham1988,
///   author = "Y. Shoham and A. Gersho",
///   title = "Efficient bit allocation for an arbitrary set of quantizers",
///   journal = "IEEE Transactions on Acoustics, Speech, and Signal Processing",
///   volume = "36",
///   number = "9",
///   pages = "1445-1453",
///   year = "September 1988" }
//----------------------------------------------------------------------------------------------------------------------
class UniformQuantisation
{
public:
  //---------------------------------------------------------------------
  /// \name   Quantisation utility functions
  /// \brief  Low level utility functions for qunatising / dequantising data
  //---------------------------------------------------------------------
  //@{

  /// \brief Function for uniformly quantising a value within the range [qMin, qMax]
  /// with a specified precision of allocated bits.
  static uint32_t quantise(
    float    qMin,
    float    qMax,
    uint32_t prec,
    float    val);

  static uint32_t quantiseFloor(
    float    qMin,
    float    qMax,
    uint32_t prec,
    float    val);

  static uint32_t quantiseCeil(
    float    qMin,
    float    qMax,
    uint32_t prec,
    float    val);

  /// \brief Function for uniformly quantising a value within the range [qMin, qMax]
  /// with a specified reciprocal step size.
  static uint32_t quantise(
    float    qMin,
    float    qMax,
    float    recipStepSize,
    float    val);

  static uint32_t quantiseFloor(
    float    qMin,
    float    qMax,
    float    recipStepSize,
    float    val);

  static uint32_t quantiseCeil(
    float    qMin,
    float    qMax,
    float    recipStepSize,
    float    val);

  /// \brief Function to uniformly dequantise a value within the range [qMin, qMax]
  /// with a specified precision of allocated bits.
  static float dequantise(
    float    qMin,
    float    qMax,
    uint32_t prec,
    uint32_t val);

  /// \brief Function to uniformly dequantise a value within the range [qMin, qMax]
  static float dequantise(
    float    qStepSize,
    float    qMin,
    uint32_t val);

  /// \brief Function to compute the quantisation step size for the range [qMin, qMax]
  static float stepSize(
    float    qMin,
    float    qMax,
    uint32_t prec);

  /// \brief Function to compute the reciprocal of the quantisation step size
  /// for the range [qMin, qMax]
  static float recipStepSize(
    float    qMin,
    float    qMax,
    uint32_t prec);

  /// \brief Function to quantise a set of data samples with the
  /// specified quantisation range and precision.
  static void quantise(
    BitStreamEncoder&              encoder,
    const UniformQuantisationInfo& qInfo,
    uint32_t                       numSamples,
    const float*                   data);

  static void quantiseDebug(
    FILE*                          filePointer,
    const UniformQuantisationInfo& qInfo,
    uint32_t                       numSamples,
    const float*                   data);

  /// \brief Function to dequantise a set of data samples with the
  /// specified quantisation range and precision.
  static void dequantise(
    BitStreamDecoder&              decoder,
    const UniformQuantisationInfo& qInfo,
    uint32_t                       numSamples,
    float*                         data);
  //@}

public:
  //---------------------------------------------------------------------
  UniformQuantisation(uint32_t numCoefSets);
  ~UniformQuantisation();

  /// \brief Main function to efficiently distribute a given byte budget R
  /// among a number of coefficient sets
  bool distributeBits(size_t byteBudget);

  //---------------------------------------------------------------------
  /// \name   Quantisation functions
  /// \brief  Functions for quantising / dequantising data
  //---------------------------------------------------------------------
  //@{

  /// \brief Function to quantise a set of data samples with the
  /// specified quantisation range and precision.
  void quantise(BitStreamEncoder& encoder);

  /// \brief Function to dequantise a set of data samples with the
  /// specified quantisation range and precision.
  void dequantise(BitStreamDecoder& decoder);
  //@}

  //---------------------------------------------------------------------
  /// \name   Coefficient set functions
  /// \brief  Functions for managing coefficient sets
  //---------------------------------------------------------------------
  //@{

  /// \brief Function to get the number of coefficient sets
  uint32_t getNumCoefSets() const;

  /// \brief Function to recover a specified coefficient set
  const UniformQuantisationCoefSetBase* getCoefficientSet(uint32_t i) const;
  UniformQuantisationCoefSetBase* getCoefficientSet(uint32_t i);
  void setCoefficientSet(uint32_t i, UniformQuantisationCoefSetBase* coefSet);

  /// \brief Function to get the number of coefficient sets with the specified user id
  uint32_t getNumCoefSetsWithUserId(uint32_t userId) const;

  /// \brief Function to get the sum of the precisions for coefficient sets
  /// with the specified user id
  uint32_t getSumCoefSetPrecisionsWithUserId(uint32_t userId) const;

  /// \brief Function to print out the rate and distortion characteristics
  void printRateAndDistortion(FILE* filePointer);
  //@}

  //---------------------------------------------------------------------
  /// \name   Data management functions
  /// \brief  Accessors for managing the class data
  //---------------------------------------------------------------------
  //@{

  /// \brief Function to get the actual byte budget for the optimal
  /// distribution of bits over the coefficient sets
  size_t getActualByteBudget() const;

  /// \brief Function to recover the rate for the optimal distribution
  /// of bits over the coefficient sets
  float getRate() const;

  /// \brief Function to recover the distortion for the optimal
  /// distribution of bits over the coefficient sets
  float getDistortion() const;

  /// \brief Function to determine if all coefficient sets were allocated the
  /// full precision of bits
  bool isMaxPrecision() const;

  /// \brief Function to reset the rate and distortion computed flags
  /// on every coefficient set in the uniform quantiser
  void resetRateDistComputed();

  /// \brief Function to save the coefficient set quantisation information state
  void saveInfoState();

  /// \brief Function to restore a saved coefficient set quantisation information state
  void restoreInfoState();

  /// \brief Function to get the saved quantisation information state
  const UniformQuantisationInfo* getInfoState() const;
  //@}

protected:
  //---------------------------------------------------------------------
  /// \brief The optimal bit allocation problem can be formulated as a constrained
  /// optimization problem which aims to minimize the overall distortion while remaining
  /// within an upper limit on the bit budget.
  ///
  /// argmin_{r_i}( \sum_i d_i(r_i) ), subject to \sum_i r_i <= R
  /// where r_i are the bit rates.
  /// d_i are the distortions generated by these bit rates r_i.
  /// R is the total bit budget available for quantisation.
  ///
  /// An optimal solution to this problem can be found by solving an equivalent unconstrained
  /// Lagrangian optimization problem: argmin_{r_i}( \sum_i (d_i(r_i) + lambda*r_i) )
  bool optimiseBitAllocations(size_t byteBudget);

  /// \brief Function to compute the unconstrained Lagrangian optimization step for a
  /// specified lambda parameter: argmin_{r_i}( \sum_i (d_i(r_i) + lambda*r_i) )
  void optimiseBitAllocationsLambdaStep(
    float  lambda,
    float& optimalRate,
    float& optimalDist);

protected:
  // Byte budget to quantise the coefficient set into
  size_t                            m_byteBudget;

  // Coefficient sets
  uint32_t                          m_numCoefSets;
  UniformQuantisationCoefSetBase**  m_coeffSets;

  // Saved quantisation information state
  UniformQuantisationInfo*          m_qInfoState;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_UNIFORM_QUANTISATION_H
