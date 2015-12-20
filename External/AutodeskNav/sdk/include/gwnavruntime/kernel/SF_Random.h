/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

/**************************************************************************

PublicHeader:   Kernel
Filename    :   KY_Random.h
Content     :   Pseudo-random number generator
Created     :   June 27, 2005
Authors     :   

**************************************************************************/

#ifndef INC_KY_Kernel__Random_H
#define INC_KY_Kernel__Random_H

#include "gwnavruntime/kernel/SF_Types.h"

namespace Kaim { namespace Alg {

class Random
{
public:
    
    // Global generator.
    static UInt32   KY_STDCALL NextRandom();
    static void     KY_STDCALL SeedRandom(UInt32 seed);
    static float    KY_STDCALL GetUnitFloat();

    // In case you need independent generators.  The global
    // generator is just an instance of this.
    enum RandomConstants {
        Random_SeedCount = 8
    };

    class Generator
    {
    public:
        UInt32  Q[Random_SeedCount];
        UInt32  C;
        UInt32  I;
        
    public:
        Generator();
        void    SeedRandom(UInt32 seed);    // not necessary
        UInt32  NextRandom();
        float   GetUnitFloat();
    };

};

}}

#endif // INC_KY_Kernel__Random_H
