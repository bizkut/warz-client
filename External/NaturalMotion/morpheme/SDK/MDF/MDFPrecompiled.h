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

//----------------------------------------------------------------------------------------------------------------------

// Microsoft header optimisation switches
#define WIN32_LEAN_AND_MEAN
#define NOCRYPT
#define NOGDI
#define NOIME
#define NOSERVICE
#define NOMCX
#define NOSOUND
#define NOCOMM
#define NORPC
#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>

// parser basics
#include "ParserDefs.h"
#include "ParserBase.h"

// core types
#include "MDF.h"

// grammars
#include "MDFModuleDefs.h"
#include "MDFModuleGrammar.h"

#include "MDFTypesDefs.h"
#include "MDFTypesGrammar.h"

#include "MDFBehaviourDefs.h"
#include "MDFBehaviourGrammar.h"
