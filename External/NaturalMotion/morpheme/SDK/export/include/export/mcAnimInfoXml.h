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
#ifndef MC_ANIMINFOXML_H
#define MC_ANIMINFOXML_H
//----------------------------------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include <map>
#include "mcAnimInfo.h"
#include "mcXML.h"

//----------------------------------------------------------------------------------------------------------------------
/// XML implementation of AnimInfo classes
//----------------------------------------------------------------------------------------------------------------------
namespace ME
{

//----------------------------------------------------------------------------------------------------------------------
class AnimInfoListXML;

//----------------------------------------------------------------------------------------------------------------------
/// AnimTakeXML
//----------------------------------------------------------------------------------------------------------------------
class AnimTakeXML : public AnimTake
{
  friend class AnimInfoXML;

public:
  virtual ~AnimTakeXML();
  AnimTakeXML(XMLElement* xmlElement);
  AnimTakeXML(float duration, float fps, const char* name);

  virtual float getDuration() const;
  virtual const char* getName() const;
  virtual float getFps() const;

private:
  XMLElement* m_xmlElement;
};

//----------------------------------------------------------------------------------------------------------------------
/// AnimInfoXML
//----------------------------------------------------------------------------------------------------------------------
class AnimInfoXML : public AnimInfo
{
  friend class AnimInfoListXML;

public:
  virtual ~AnimInfoXML();

  virtual int64_t getValidDate() const;
  virtual uint32_t getNumTakes() const;
  virtual const char* getName() const;
  virtual const AnimTake* getTake(uint32_t index) const;

  /// Modifying the anim info
  virtual void setValidDate(int64_t validDate) const;
  virtual void clearTakes();
  virtual void addTake(const char* name, float duration, float fps);

  virtual bool write();

protected:
  AnimInfoXML(XMLElement* xmlElement);
  AnimInfoXML(const char* filename);

private:
  typedef std::vector<AnimTakeXML*> TakeNameList;

  XMLElement*            m_xmlElement;
  TakeNameList           m_takes;
  const AnimInfoListXML* m_eventTrackList;
  mutable char*          m_cachedname; // a lowercase version of the name with backslashes converted to the "normal" windows form
};

//----------------------------------------------------------------------------------------------------------------------
/// AnimInfoListXML
//----------------------------------------------------------------------------------------------------------------------
class AnimInfoListXML : public AnimInfoList
{
  friend class AnimInfoFactoryXML;

public:
  virtual ~AnimInfoListXML();

  virtual AnimInfoXML* createAnimInfo(const char* filename);

  virtual uint32_t getNumAnimInfos() const;
  virtual AnimInfo* getAnimInfo(unsigned int index);
  virtual const AnimInfo* getAnimInfo(unsigned int index) const;
  virtual AnimInfo* getAnimInfo(const char* animName);
  virtual const AnimInfo* getAnimInfo(const char* animName) const;

  virtual void removeAnimInfo(AnimInfo* animInfo);
  virtual void replaceAnimInfo(const AnimInfo* animInfo);

  virtual bool write(const char* destFile, int* error = NULL);

protected:
  AnimInfoListXML(XMLElement* xmlElement);
  AnimInfoListXML();

private:
  struct compareString
  {
    NM_INLINE bool operator()(const char* x, const char* y) const
    {
      return strcmp(x, y) < 0;
    }
  };

  typedef std::vector<AnimInfoXML*> AnimInfoXMLList;
  typedef std::map<const char*, AnimInfoXML*, compareString> AnimInfoXMLMap;

  XMLElement*             m_xmlElement;
  AnimInfoXMLMap          m_animInfos;
  mutable AnimInfoXMLList m_animInfosCache; ///< A cache allowing an inxexed access to animInfo. It is rebuild when needed.
};

//----------------------------------------------------------------------------------------------------------------------
/// AnimInfoFactoryXML
//----------------------------------------------------------------------------------------------------------------------
class AnimInfoFactoryXML : public AnimInfoFactory
{
public:
  AnimInfoFactoryXML();
  ~AnimInfoFactoryXML();

  virtual AnimInfoListXML* createAnimInfoList();
  virtual void destroyAnimInfoList(AnimInfoListXML* animList);

  /// Loads a AnimInfoList.  It calls into the
  /// doLoadAnimInfoList function to actually get the xml text (the default implementation
  /// simply loads the file from disk)
  virtual AnimInfoList* loadAnimInfoList(const char* srcFilename);

  /// unloads a previously loaded anim info list and removes it from the cache.
  virtual bool unloadAnimInfoList(const char* srcFileName);

protected:

  /// This function handles bringing the data (xml text in this case) into memory.
  /// The default implementation simply loads data from a file, but could
  /// easily be overloaded to lookup xml text that already resides in memory.
  /// dataSize should be set to the length of the returned xml data.
  virtual const char* doLoadAnimInfoList(const char* srcFilename, size_t& dataSize);

  /// Frees asset source data (xml text) that was allocated via the
  /// doLoadAnimInfoList() function.  This is called once the AnimInfoList has been
  /// created.  Naturally, this can be overloaded to do nothing if the xml source
  /// is to persist in memory.
  virtual void doFreeAnimInfoList(const char* animList);

private:

  struct XMLAnimInfoListAsset
  {
    NM::TiXmlDocument* m_document;
    AnimInfoList*      m_asset;
  };

  std::map<std::string, XMLAnimInfoListAsset> m_fileAssetMap;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace ME

#endif // MC_ANIMINFOXML_H
