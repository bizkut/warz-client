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
#ifndef MC_ANIMINFO_H
#define MC_ANIMINFO_H

//----------------------------------------------------------------------------------------------------------------------
#include "mcExport.h"

//----------------------------------------------------------------------------------------------------------------------
namespace ME
{

//----------------------------------------------------------------------------------------------------------------------
/// NamedItem
//----------------------------------------------------------------------------------------------------------------------
class NamedInfo
{
public:
  virtual ~NamedInfo() { }
  virtual const char* getName() const = 0;
};

//----------------------------------------------------------------------------------------------------------------------
/// AnimTake
//----------------------------------------------------------------------------------------------------------------------
class AnimTake : public NamedInfo
{
public:
  virtual ~AnimTake() { }

  virtual float getDuration() const = 0;
  virtual float getFps() const = 0 ;
};

//----------------------------------------------------------------------------------------------------------------------
/// AnimInfo
//----------------------------------------------------------------------------------------------------------------------
class AnimInfo : public NamedInfo
{
public:
  virtual ~AnimInfo() { }

  /// Reading the anim info
  virtual int64_t getValidDate() const = 0;
  virtual uint32_t getNumTakes() const = 0;
  virtual const AnimTake* getTake(uint32_t index) const = 0;

  /// Modifying the anim info
  virtual void setValidDate(int64_t validDate) const = 0;
  virtual void clearTakes() = 0;
  virtual void addTake(const char* name, float duration, float fps) = 0;

protected:
  AnimInfo() { }
};

//----------------------------------------------------------------------------------------------------------------------
/// AnimInfoList
//----------------------------------------------------------------------------------------------------------------------
class AnimInfoList
{
public:
  virtual ~AnimInfoList() { }

  virtual uint32_t getNumAnimInfos() const = 0;
  virtual AnimInfo* getAnimInfo(uint32_t index) = 0;
  virtual const AnimInfo* getAnimInfo(unsigned int index) const = 0;
  virtual AnimInfo* getAnimInfo(const char* animName) = 0;
  virtual const AnimInfo* getAnimInfo(const char* animName) const = 0;

  virtual void removeAnimInfo(AnimInfo* animInfo) = 0;
  virtual void replaceAnimInfo(const AnimInfo* animInfo) = 0;
  virtual bool write(const char* /*destFile*/, int* /*error*/ = NULL) { return false; }

protected:
  AnimInfoList() { }
};

//----------------------------------------------------------------------------------------------------------------------
/// AnimInfoFactory
//----------------------------------------------------------------------------------------------------------------------
class AnimInfoFactory
{
public:
  virtual ~AnimInfoFactory() { }

  virtual AnimInfoList* createAnimInfoList() = 0;

  /// Loads an AnimInfoList.
  virtual AnimInfoList* loadAnimInfoList(const char* srcFilename) = 0;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace ME

#endif // MC_ANIMINFO_H
