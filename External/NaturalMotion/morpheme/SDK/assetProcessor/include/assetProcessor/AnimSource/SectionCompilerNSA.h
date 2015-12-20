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
#ifndef SECTION_COMPILER_NSA_H
#define SECTION_COMPILER_NSA_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMVector3.h"
#include "morpheme/AnimSource/mrAnimSourceNSA.h"
#include "assetProcessor/AnimSource/AnimSourceUncompressed.h"
#include "assetProcessor/AnimSource/AnimSourceBuilderUtils.h"
#include "assetProcessor/AnimSource/RotVecTableBuilder.h"
#include "assetProcessor/AnimSource/Vector3QuantisationTableBuilder.h"
#include "assetProcessor/AnimSource/IntVector3TableBuilder.h"
#include "assetProcessor/AnimSource/AnimSourceBuilderNSA.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// ChannelCompressionOptionsNSA
//----------------------------------------------------------------------------------------------------------------------
class ChannelCompressionOptionsNSA
{
public:
  enum ChannelCompressionType
  {
    Unchanging,
    Sampled,
    NumChanMethods
  };

public:
  ChannelCompressionOptionsNSA();
  ~ChannelCompressionOptionsNSA();

  /// \brief Function for parsing the input options string.
  void parseOptions(
    const char* options,
    NMP::BasicLogger* errorLogger);

  void initChannelMethods(const AnimSourceUncompressed* animSource);

  bool hasSampledChannelMethods() const;

  // Accessors
  NM_INLINE float getQuantisationSetQuality() const { return m_quantisationSetQuality; }
  NM_INLINE size_t getByteBudgetPerSection() const { return m_byteBudgetPerSection; }
  NM_INLINE uint32_t getMaxFramesPerSection() const { return m_maxFramesPerSection; }

  NM_INLINE uint32_t getNumAnimChannels() const { return (uint32_t)m_posChannelMethods.size(); }
  NM_INLINE const std::vector<ChannelCompressionType>& getPosChannelMethods() const { return m_posChannelMethods; }
  NM_INLINE const std::vector<ChannelCompressionType>& getQuatChannelMethods() const { return m_quatChannelMethods; }

  NM_INLINE ChannelCompressionType getPosChannelMethod(uint32_t channel) const;
  NM_INLINE void setPosChannelMethod(uint32_t channel, ChannelCompressionType chanType);
  NM_INLINE ChannelCompressionType getQuatChannelMethod(uint32_t channel) const;
  NM_INLINE void setQuatChannelMethod(uint32_t channel, ChannelCompressionType chanType);

protected:
  float                                 m_quantisationSetQuality;
  size_t                                m_byteBudgetPerSection;
  uint32_t                              m_maxFramesPerSection;

  std::vector<ChannelCompressionType>   m_posChannelMethods;
  std::vector<ChannelCompressionType>   m_quatChannelMethods;
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE ChannelCompressionOptionsNSA::ChannelCompressionType
ChannelCompressionOptionsNSA::getPosChannelMethod(uint32_t channel) const
{
  NMP_VERIFY(channel < m_posChannelMethods.size());
  return m_posChannelMethods[channel];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void ChannelCompressionOptionsNSA::setPosChannelMethod(
  uint32_t channel,
  ChannelCompressionType chanType)
{
  NMP_VERIFY(channel < m_posChannelMethods.size());
  m_posChannelMethods[channel] = chanType;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE ChannelCompressionOptionsNSA::ChannelCompressionType
ChannelCompressionOptionsNSA::getQuatChannelMethod(uint32_t channel) const
{
  NMP_VERIFY(channel < m_quatChannelMethods.size());
  return m_quatChannelMethods[channel];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void ChannelCompressionOptionsNSA::setQuatChannelMethod(
  uint32_t channel,
  ChannelCompressionType chanType)
{
  NMP_VERIFY(channel < m_quatChannelMethods.size());
  m_quatChannelMethods[channel] = chanType;
}

//----------------------------------------------------------------------------------------------------------------------
// UnchangingDataLayoutNSA
//----------------------------------------------------------------------------------------------------------------------
class UnchangingDataLayoutNSA
{  
public:
  UnchangingDataLayoutNSA();
  ~UnchangingDataLayoutNSA();

  void tidy();
  
  void computeLayout(
    const AnimSourceUncompressed* animSource,
    const ChannelCompressionOptionsNSA* channelOptions);
    
  NM_INLINE const CompToAnimChannelMapLayout* getUnchangingPosCompToAnimMap() const { return m_unchangingPosCompToAnimMap; }
  NM_INLINE const CompToAnimChannelMapLayout* getUnchangingQuatCompToAnimMap() const { return m_unchangingQuatCompToAnimMap; }

public:
  CompToAnimChannelMapLayout* m_unchangingPosCompToAnimMap;
  CompToAnimChannelMapLayout* m_unchangingQuatCompToAnimMap;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class SampledDataIntermediateNSA
/// \brief For intermediate data used to compile the binary components of the unchanging data.
//----------------------------------------------------------------------------------------------------------------------
class UnchangingDataIntermediateNSA
{
public:
  UnchangingDataIntermediateNSA();
  ~UnchangingDataIntermediateNSA();

  void tidy();

  void computeUnchangingData(
    const AnimSourceUncompressed* animSource,
    const UnchangingDataLayoutNSA* unchangingDataLayout);
    
  NM_INLINE const Vector3Table* getUnchangingPosTable() const { return m_unchangingPosTable; }
  NM_INLINE const Vector3QuantisationTable* getUnchangingPosQuantisation() const { return m_unchangingPosQuantisation; }
  NM_INLINE const IntVector3Table* getUnchangingPosQuantisedTable() const { return m_unchangingPosQuantisedTable; }
  
  NM_INLINE const RotVecTable* getUnchangingQuatTable() const { return m_unchangingQuatTable; }
  NM_INLINE const Vector3QuantisationTable* getUnchangingQuatQuantisation() const { return m_unchangingQuatQuantisation; }
  NM_INLINE const IntVector3Table* getUnchangingQuatQuantisedTable() const { return m_unchangingQuatQuantisedTable; }
    
protected:
  void computeUnchangingPosQuantisedData(
    const AnimSourceUncompressed* animSource,
    const UnchangingDataLayoutNSA* unchangingDataLayout);
    
  void computeUnchangingQuatQuantisedData(
    const AnimSourceUncompressed* animSource,
    const UnchangingDataLayoutNSA* unchangingDataLayout);
    
protected:
  // Pos
  Vector3Table*               m_unchangingPosTable;
  Vector3QuantisationTable*   m_unchangingPosQuantisation;
  IntVector3Table*            m_unchangingPosQuantisedTable;
  
  // Quat
  RotVecTable*                m_unchangingQuatTable;
  Vector3QuantisationTable*   m_unchangingQuatQuantisation;
  IntVector3Table*            m_unchangingQuatQuantisedTable;
};

//----------------------------------------------------------------------------------------------------------------------
// UnchangingDataCompiledNSA
//----------------------------------------------------------------------------------------------------------------------
class UnchangingDataCompiledNSA
{
public:
  UnchangingDataCompiledNSA();
  ~UnchangingDataCompiledNSA();

  void tidy();

  void compileUnchangingData(
    const AnimSourceUncompressed* animSource,
    const UnchangingDataLayoutNSA* unchangingDataLayout);

  void compileCompToAnimMaps(const UnchangingDataLayoutNSA* unchangingDataLayout);
  
  // Accessors
  NM_INLINE const MR::CompToAnimChannelMap* getUnchangingPosCompToAnimMap() const { return m_unchangingPosCompToAnimMap; }
  NM_INLINE const MR::CompToAnimChannelMap* getUnchangingQuatCompToAnimMap() const { return m_unchangingQuatCompToAnimMap; }
  NM_INLINE const MR::UnchangingDataNSA* getUnchangingData() const { return m_unchangingData; }
  
protected:
  MR::CompToAnimChannelMap*   m_unchangingPosCompToAnimMap;
  MR::CompToAnimChannelMap*   m_unchangingQuatCompToAnimMap;
  MR::UnchangingDataNSA*      m_unchangingData;
};

//----------------------------------------------------------------------------------------------------------------------
// SampledDataLayoutNSA
//----------------------------------------------------------------------------------------------------------------------
class SampledDataLayoutNSA
{
public:
  static NMP::Memory::Format getLayoutMemoryRequirements(
    const std::vector<uint32_t>& sectionStartFrames,
    const std::vector<uint32_t>& sectionEndFrames,
    const SectionDataChannelLayout* sectionDataLayout);

public:
  // Constructor / Destructor
  SampledDataLayoutNSA();
  ~SampledDataLayoutNSA();

  void tidy();

  // Layout generation
  void computeLayout(
    const AnimSourceUncompressed* animSource,
    const ChannelCompressionOptionsNSA* channelOptions);

  void setLayout(
    const std::vector<uint32_t>& sectionStartFrames,
    const std::vector<uint32_t>& sectionEndFrames,
    const SectionDataChannelLayout* sectionDataLayout);

  // Sectioning
  NM_INLINE uint32_t getNumFrameSections() const { return m_numFrameSections; }
  NM_INLINE uint32_t getNumChannelSections() const { return m_numChannelSections; }

  // Quantisation set basis vectors
  NM_INLINE uint32_t getSampledPosNumQuantisationSets() const { return m_sampledPosNumQuantisationSets; }
  NM_INLINE uint32_t getSampledQuatNumQuantisationSets() const { return m_sampledQuatNumQuantisationSets; }

  // Section frame divisions
  NM_INLINE const std::vector<uint32_t>& getSectionStartFrames() const { return m_sectionStartFrames; }
  NM_INLINE const std::vector<uint32_t>& getSectionEndFrames() const { return m_sectionEndFrames; }
  uint32_t getSectionStartFrame(uint32_t iFrameSection) const;
  uint32_t getSectionEndFrame(uint32_t iFrameSection) const;

  // Section channel divisions
  uint32_t getNumSampledPosChannels() const;
  NM_INLINE const std::vector<CompToAnimChannelMapLayout*>& getSampledPosCompToAnimMaps() const { return m_sampledPosCompToAnimMaps; }
  const CompToAnimChannelMapLayout* getSampledPosCompToAnimMap(uint32_t iChannelSection) const;

  uint32_t getNumSampledQuatChannels() const;
  NM_INLINE const std::vector<CompToAnimChannelMapLayout*>& getSampledQuatCompToAnimMaps() const { return m_sampledQuatCompToAnimMaps; }
  const CompToAnimChannelMapLayout* getSampledQuatCompToAnimMap(uint32_t iChannelSection) const;

protected:
    
  bool computeLayoutSingleChannelSection(
    const AnimSourceUncompressed* animSource,
    const ChannelCompressionOptionsNSA* channelOptions,
    const std::vector<uint32_t>& sampledPosAnimChans,
    const std::vector<uint32_t>& sampledQuatAnimChans);
    
  bool computeLayoutMultipleChannelSections(
    const AnimSourceUncompressed* animSource,
    const ChannelCompressionOptionsNSA* channelOptions,
    const std::vector<uint32_t>& sampledPosAnimChans,
    const std::vector<uint32_t>& sampledQuatAnimChans);

protected:
  // Sectioning
  uint32_t                            m_numFrameSections;
  uint32_t                            m_numChannelSections;

  // Quantisation set basis vectors
  uint32_t                            m_sampledPosNumQuantisationSets;
  uint32_t                            m_sampledQuatNumQuantisationSets;

  // Division of section frames
  std::vector<uint32_t>               m_sectionStartFrames;
  std::vector<uint32_t>               m_sectionEndFrames;

  // Division of compression channels (channel-wise) and their maps to animation channels
  std::vector<CompToAnimChannelMapLayout*>  m_sampledPosCompToAnimMaps;
  std::vector<CompToAnimChannelMapLayout*>  m_sampledQuatCompToAnimMaps;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class SubSectionIntermediateNSA
//----------------------------------------------------------------------------------------------------------------------
class SubSectionIntermediateNSA
{
public:
  SubSectionIntermediateNSA();
  ~SubSectionIntermediateNSA();

  void tidy();

  // Pos
  void computeSampledPosRelativeTransforms(
    const AnimSourceUncompressed* animSource,
    const SampledDataLayoutNSA* sampledDataLayout,
    uint32_t iFrameSection,
    uint32_t iChannelSection,
    const Vector3QuantisationTable* quantPosMeans);

  void computeSampledPosCompToQSetMaps(
    const uint32_t* sectionChanToQSetMapX,
    const uint32_t* sectionChanToQSetMapY,
    const uint32_t* sectionChanToQSetMapZ);

  void computeSampledPosQuantisedData(
    const AnimSourceUncompressed* animSource,
    const SampledDataLayoutNSA* sampledDataLayout,
    uint32_t iFrameSection,
    uint32_t iChannelSection,
    const NMP::Vector3* qSetPosMin,
    const NMP::Vector3* qSetPosMax);

  NM_INLINE const uint32_t* getSampledPosMeansX() const { return m_sampledPosMeansX; }
  NM_INLINE const uint32_t* getSampledPosMeansY() const { return m_sampledPosMeansY; }
  NM_INLINE const uint32_t* getSampledPosMeansZ() const { return m_sampledPosMeansZ; }
  NM_INLINE const Vector3Table* getSampledPosRelTable() const { return m_sampledPosRelTable; }
  NM_INLINE const uint32_t* getSampledPosCompToQSetMapX() const { return m_sampledPosCompToQSetMapX; }
  NM_INLINE const uint32_t* getSampledPosCompToQSetMapY() const { return m_sampledPosCompToQSetMapY; }
  NM_INLINE const uint32_t* getSampledPosCompToQSetMapZ() const { return m_sampledPosCompToQSetMapZ; }
  NM_INLINE const IntVector3Table* getSampledPosRelQuantisedTable() const { return m_sampledPosRelQuantisedTable; }
  
  // Quat
  void computeSampledQuatRelativeTransforms(
    const AnimSourceUncompressed* animSource,
    const SampledDataLayoutNSA* sampledDataLayout,
    uint32_t iFrameSection,
    uint32_t iChannelSection,
    const Vector3QuantisationTable* quantQuatMeans);

  void computeSampledQuatCompToQSetMaps(
    const uint32_t* sectionChanToQSetMapX,
    const uint32_t* sectionChanToQSetMapY,
    const uint32_t* sectionChanToQSetMapZ);

  void computeSampledQuatQuantisedData(
    const AnimSourceUncompressed* animSource,
    const SampledDataLayoutNSA* sampledDataLayout,
    uint32_t iFrameSection,
    uint32_t iChannelSection,
    const NMP::Vector3* qSetQuatMin,
    const NMP::Vector3* qSetQuatMax);

  NM_INLINE const uint32_t* getSampledQuatMeansX() const { return m_sampledQuatMeansX; }
  NM_INLINE const uint32_t* getSampledQuatMeansY() const { return m_sampledQuatMeansY; }
  NM_INLINE const uint32_t* getSampledQuatMeansZ() const { return m_sampledQuatMeansZ; }
  NM_INLINE const RotVecTable* getSampledQuatRelTable() const { return m_sampledQuatRelTable; }
  NM_INLINE const uint32_t* getSampledQuatCompToQSetMapX() const { return m_sampledQuatCompToQSetMapX; }
  NM_INLINE const uint32_t* getSampledQuatCompToQSetMapY() const { return m_sampledQuatCompToQSetMapY; }
  NM_INLINE const uint32_t* getSampledQuatCompToQSetMapZ() const { return m_sampledQuatCompToQSetMapZ; }
  NM_INLINE const IntVector3Table* getSampledQuatRelQuantisedTable() const { return m_sampledQuatRelQuantisedTable; }

protected:
  // Pos
  uint32_t*         m_sampledPosMeansX;
  uint32_t*         m_sampledPosMeansY;
  uint32_t*         m_sampledPosMeansZ;
  Vector3Table*     m_sampledPosRelTable;
  uint32_t*         m_sampledPosCompToQSetMapX;
  uint32_t*         m_sampledPosCompToQSetMapY;
  uint32_t*         m_sampledPosCompToQSetMapZ;
  IntVector3Table*  m_sampledPosRelQuantisedTable;
  
  // Quat
  uint32_t*         m_sampledQuatMeansX;
  uint32_t*         m_sampledQuatMeansY;
  uint32_t*         m_sampledQuatMeansZ;
  RotVecTable*      m_sampledQuatRelTable;
  uint32_t*         m_sampledQuatCompToQSetMapX;
  uint32_t*         m_sampledQuatCompToQSetMapY;
  uint32_t*         m_sampledQuatCompToQSetMapZ;
  IntVector3Table*  m_sampledQuatRelQuantisedTable;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class SampledDataIntermediateNSA
/// \brief For intermediate data used to compile the binary components of the sampled data.
//----------------------------------------------------------------------------------------------------------------------
class SampledDataIntermediateNSA
{
public:
  SampledDataIntermediateNSA();
  ~SampledDataIntermediateNSA();

  void tidy();

  void computeSampledData(
    const AnimSourceUncompressed* animSource,
    const SampledDataLayoutNSA* sampledDataLayout);

  const SubSectionIntermediateNSA* getSubSection(uint32_t iFrameSection, uint32_t iChannelSection) const;
  SubSectionIntermediateNSA* getSubSection(uint32_t iFrameSection, uint32_t iChannelSection);

  NM_INLINE const Vector3QuantisationTable* getQuantisationPosMeans() const { return m_quantPosMeans; }
  NM_INLINE const Vector3QuantisationTable* getQuantisationQuatMeans() const { return m_quantQuatMeans; }
  NM_INLINE const NMP::Vector3* getQuantisationSetPosMin() const { return m_qSetPosMin; }
  NM_INLINE const NMP::Vector3* getQuantisationSetPosMax() const { return m_qSetPosMax; }
  NM_INLINE const NMP::Vector3* getQuantisationSetQuatMin() const { return m_qSetQuatMin; }
  NM_INLINE const NMP::Vector3* getQuantisationSetQuatMax() const { return m_qSetQuatMax; }

protected:
  // Pos
  void computeSampledPosMeansQuantisationInfo(
    const AnimSourceUncompressed* animSource,
    const SampledDataLayoutNSA* sampledDataLayout);

  void computeSampledPosRelativeTransforms(
    const AnimSourceUncompressed* animSource,
    const SampledDataLayoutNSA* sampledDataLayout);

  // Reduce the number of quantisation sets used to represent the data by using the similarity
  // in the sample distributions to compound the channel sets. This process computes the basis
  // direction vectors and min, max offsets for each of the compounded quantisation sets. This
  // function does not use the bit-wise channel precisions to match quantisation sets.
  void computeSampledPosQuantisationSetBasisVectors(
    const AnimSourceUncompressed* animSource,
    const SampledDataLayoutNSA* sampledDataLayout);

  // Compute the scale, offset quantisation set basis vectors for the desired bit-wise channel
  // precisions, then quantisae the channel set data accordingly.
  void computeSampledPosQuantisedData(
    const AnimSourceUncompressed* animSource,
    const SampledDataLayoutNSA* sampledDataLayout);
    
  // Quat
  void computeSampledQuatMeansQuantisationInfo(
    const AnimSourceUncompressed* animSource,
    const SampledDataLayoutNSA* sampledDataLayout);

  void computeSampledQuatRelativeTransforms(
    const AnimSourceUncompressed* animSource,
    const SampledDataLayoutNSA* sampledDataLayout);

  // Reduce the number of quantisation sets used to represent the data by using the similarity
  // in the sample distributions to compound the channel sets. This process computes the basis
  // direction vectors and min, max offsets for each of the compounded quantisation sets. This
  // function does not use the bit-wise channel precisions to match quantisation sets.
  void computeSampledQuatQuantisationSetBasisVectors(
    const AnimSourceUncompressed* animSource,
    const SampledDataLayoutNSA* sampledDataLayout);

  // Compute the scale, offset quantisation set basis vectors for the desired bit-wise channel
  // precisions, then quantisae the channel set data accordingly.
  void computeSampledQuatQuantisedData(
    const AnimSourceUncompressed* animSource,
    const SampledDataLayoutNSA* sampledDataLayout);

public:
  // Section data
  uint32_t                            m_numFrameSections;
  uint32_t                            m_numChannelSections;
  SubSectionIntermediateNSA**         m_subSections;

  // Quantistion set means
  Vector3QuantisationTable*           m_quantPosMeans;
  Vector3QuantisationTable*           m_quantQuatMeans;

  // Quantisation set basis vectors
  NMP::Vector3*                       m_qSetPosMin;
  NMP::Vector3*                       m_qSetPosMax;
  NMP::Vector3*                       m_qSetQuatMin;
  NMP::Vector3*                       m_qSetQuatMax;
};

//----------------------------------------------------------------------------------------------------------------------
// SubSectionNSACompiled
//----------------------------------------------------------------------------------------------------------------------
class SubSectionNSACompiled
{
public:
  SubSectionNSACompiled();
  ~SubSectionNSACompiled();

  void tidy();

  void compileSectionData(
    const SampledDataIntermediateNSA* sampledDataIntermediate,
    const SampledDataLayoutNSA* sampledDataLayout,
    uint32_t iFrameSection,
    uint32_t iChannelSection);
    
  NM_INLINE const MR::SectionDataNSA* getSectionData() const { return m_sectionData; }
  
protected:
  MR::SectionDataNSA* m_sectionData;
};

//----------------------------------------------------------------------------------------------------------------------
// SampledDataCompiledNSA
//----------------------------------------------------------------------------------------------------------------------
class SampledDataCompiledNSA
{  
public:
  SampledDataCompiledNSA();
  ~SampledDataCompiledNSA();

  void tidy();

  void compileSampledData(
    const AnimSourceUncompressed* animSource,
    const SampledDataLayoutNSA* sampledDataLayout);

  bool checkForUnchangingChannels(
    const SampledDataLayoutNSA* sampledDataLayout,
    ChannelCompressionOptionsNSA* channelCompressionOptions) const;

  void compileCompToAnimMaps(const SampledDataLayoutNSA* sampledDataLayout);


  // Accessors
  NM_INLINE uint32_t getNumFrameSections() const { return m_numFrameSections; }
  NM_INLINE uint32_t getNumChannelSections() const { return m_numChannelSections; }
  
  NM_INLINE const uint32_t* getSectionStartFrames() const { return m_sectionStartFrames; }
  
  NM_INLINE const MR::CompToAnimChannelMap* const* getSampledPosCompToAnimMaps() const { return m_sampledPosCompToAnimMaps; }
  NM_INLINE const MR::CompToAnimChannelMap* const* getSampledQuatCompToAnimMaps() const { return m_sampledQuatCompToAnimMaps; }

  NM_INLINE const MR::QuantisationScaleAndOffsetVec3& getPosMeansQuantisationInfo() const { return m_posMeansQuantisationInfo; }
  NM_INLINE uint32_t getSampledPosNumQuantisationSets() const { return m_sampledPosNumQuantisationSets; }
  NM_INLINE uint32_t getSampledQuatNumQuantisationSets() const { return m_sampledQuatNumQuantisationSets; }

  NM_INLINE const MR::QuantisationScaleAndOffsetVec3* getSampledPosQuantisationInfo() const { return m_sampledPosQuantisationInfo; }
  NM_INLINE const MR::QuantisationScaleAndOffsetVec3* getSampledQuatQuantisationInfo() const { return m_sampledQuatQuantisationInfo; }
  
  const SubSectionNSACompiled* getSubSection(uint32_t iFrameSection, uint32_t iChannelSection) const;
  SubSectionNSACompiled* getSubSection(uint32_t iFrameSection, uint32_t iChannelSection);

protected:
  void compileGlobalData(
    const SampledDataIntermediateNSA* sampledDataIntermediate,
    const SampledDataLayoutNSA* sampledDataLayout);

  void compileSectionData(
    const SampledDataIntermediateNSA* sampledDataIntermediate,
    const SampledDataLayoutNSA* sampledDataLayout);

  uint32_t getTotalNumAnimSamples() const;

  void getUnpackedSampledPosQuantisationData(
    uint32_t iChannelSection,
    uint32_t iChan,
    IntVector3Table* qMeanTable,
    IntVector3Table* qSetTable) const;

  void getUnpackedSampledPosData(
    uint32_t iChannelSection,
    uint32_t iChan,
    IntVector3Table* qDataTable) const;

  void getUnpackedSampledQuatQuantisationData(
    uint32_t iChannelSection,
    uint32_t iChan,
    IntVector3Table* qMeanTable,
    IntVector3Table* qSetTable) const;

  void getUnpackedSampledQuatData(
    uint32_t iChannelSection,
    uint32_t iChan,
    IntVector3Table* qDataTable) const;

protected:
  // Comp to anim maps
  MR::CompToAnimChannelMap**          m_sampledPosCompToAnimMaps;
  MR::CompToAnimChannelMap**          m_sampledQuatCompToAnimMaps;

  // Global data
  uint32_t*                           m_sectionStartFrames;
  MR::QuantisationScaleAndOffsetVec3  m_posMeansQuantisationInfo;
  uint32_t                            m_sampledPosNumQuantisationSets;
  uint32_t                            m_sampledQuatNumQuantisationSets;
  MR::QuantisationScaleAndOffsetVec3* m_sampledPosQuantisationInfo;
  MR::QuantisationScaleAndOffsetVec3* m_sampledQuatQuantisationInfo;
  
  // Section data
  uint32_t                            m_numFrameSections;
  uint32_t                            m_numChannelSections;
  SubSectionNSACompiled**             m_subSections;
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // SECTION_COMPILER_NSA_H
//----------------------------------------------------------------------------------------------------------------------
