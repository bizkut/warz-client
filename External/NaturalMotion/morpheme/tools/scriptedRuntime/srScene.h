#pragma once
// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.  
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

// ---------------------------------------------------------------------------------------------------------------------

namespace sqp
{
  class SquirrelVM;
}

class SceneData;

//----------------------------------------------------------------------------------------------------------------------
/// \class Scene
/// \brief This class manages a physical scene and exposes API to Squirrel to manipulate the scene and morpheme
///  characters within it.
class Scene
{
public:
  Scene();
  ~Scene();

  void init(const class Environment* env);

  void registerForScripting(sqp::SquirrelVM& vm);


private:
  Scene(const Scene& rhs);

  SceneData    *m_data;
};
