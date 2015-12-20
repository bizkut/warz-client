// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------

#include "MDFPrecompiled.h"
#include "LibMDF.h"
#include "Utils.h"

#include <Shlwapi.h>

//----------------------------------------------------------------------------------------------------------------------
const ModuleDef* getRootModule(const ModuleDefs& defs)
{
  const ModuleDef* rootModule = 0;

  ModuleDefs::const_iterator it = defs.begin();
  ModuleDefs::const_iterator end = defs.end();
  for (; it != end; ++it)
  {
    const ModuleDef* def = *it;
    if (def->m_moduleOwnerPtr == 0)
    {
      rootModule = def;
      break;
    }
  }

  return rootModule;
}

//----------------------------------------------------------------------------------------------------------------------
void getAttributeVariableTitle(const BehaviourDef::AttributeVariable& av, StringBuilder& sb)
{
  const ModifierOption* moTitle = hasBehaviourVariableModifier(av.m_modifiers, bvmTitle);
  if (!av.m_title.empty())
  {
    sb.appendPString(av.m_title);
  }
  else if (moTitle)
  {
    const char* title = moTitle->m_value.c_str() + 1;
    sb.appendCharBuf(title, (unsigned int)strlen(title) - 1);
  }
  else
  {
    sb.appendPString(av.m_name);
  }
}
