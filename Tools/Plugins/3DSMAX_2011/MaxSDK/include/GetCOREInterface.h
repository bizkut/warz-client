//**************************************************************************/
// Copyright 2009 Autodesk, Inc.  All rights reserved.
// Use of this software is subject to the terms of the Autodesk license 
// agreement provided at the time of installation or download, or which 
// otherwise accompanies this software in either electronic or hard copy form. 
//**************************************************************************/

#pragma once
#include "CoreExport.h"
#include "maxtypes.h"

class Interface;
class Interface7;
class Interface8;
class Interface9;
class Interface10;
class Interface11;
class Interface12;
class Interface13;
class FPInterface;

CoreExport Interface*   GetCOREInterface();
CoreExport Interface7*  GetCOREInterface7();
CoreExport Interface8*  GetCOREInterface8();
CoreExport Interface9*  GetCOREInterface9();
CoreExport Interface10* GetCOREInterface10();
CoreExport Interface11* GetCOREInterface11();
CoreExport Interface12* GetCOREInterface12();
CoreExport Interface13* GetCOREInterface13();

// get ID'd CORE interface
CoreExport FPInterface* GetCOREInterface(Interface_ID id);
// register CORE interface
CoreExport void RegisterCOREInterface(FPInterface* fpi);
// indexed access for MAXScript enumaration, etc...
CoreExport int NumCOREInterfaces();	
CoreExport FPInterface* GetCOREInterfaceAt(int i);
// get ID'd interface from ClassDesc for given class/sclass
CoreExport FPInterface* GetInterface(SClass_ID super, Class_ID cls, Interface_ID id);
// unregister CORE interface
CoreExport void UnregisterCOREInterface(FPInterface* fpi);