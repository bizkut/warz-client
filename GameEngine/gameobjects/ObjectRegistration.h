//=========================================================================
//	Module: ObjectRegistration.h
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

// I dont know
// if it is in obj_Dummy.cpp, it is not working ( class not registered )
#include "obj_Dummy.h"
IMPLEMENT_CLASS(DummyObject, "DummyObject", "Object");
AUTOREGISTER_CLASS(DummyObject);

#ifndef WO_SERVER
#if APEX_ENABLED
#include "obj_Apex.hpp"
IMPLEMENT_CLASS(obj_ApexDestructible, "obj_ApexDestructible", "Object");
AUTOREGISTER_CLASS(obj_ApexDestructible);
#endif //APEX_ENABLED
#endif //WO_SERVER


#ifndef WO_SERVER
#include "obj_Sprite.h"
IMPLEMENT_CLASS(obj_Sprite, "obj_Sprite", "Object");
AUTOREGISTER_CLASS(obj_Sprite);
#endif // WO_SERVER
