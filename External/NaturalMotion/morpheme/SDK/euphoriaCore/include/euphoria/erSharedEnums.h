// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef ER_SHARED_ENUMS_H
#define ER_SHARED_ENUMS_H

namespace ER
{

enum ReachSwivelModes
{
  REACH_SWIVEL_NONE      = -1,
  REACH_SWIVEL_AUTOMATIC =  0,
  REACH_SWIVEL_SPECIFIED =  1
};

enum BodyReachTypes
{
  BODY_REACH_TYPE_REACH_WITH_LEFT,
  BODY_REACH_TYPE_REACH_WITH_RIGHT,
  BODY_REACH_TYPE_REACH_WITH_RIGHT_AND_LEFT,
  BODY_REACH_TYPE_HEURISTIC,
  BODY_REACH_TYPE_REACH_FOR_ARM,
  BODY_REACH_TYPE_NUM_TYPES
};

struct LimbTypeEnum
{
  enum Type { L_arm, L_leg, L_spine, L_head, L_unknown };
  Type m_type;
};

struct BodyPartTypeEnum
{
  enum Type
  {
    /*  0 */ ChestFront,
    /*  1 */ TorsoFront,
    /*  2 */ PelvisFront,
    /*  3 */ ChestBack,
    /*  4 */ TorsoBack,
    /*  5 */ PelvisBack,
    /*  6 */ Head,
    /*  7 */ Neck,
    /*  8 */ Leg,
    /*  9 */ Foot,
    /* 10 */ Shoulder,
    /* 11 */ Arm,
    /* 12 */ Hand,
    /* 13 */ NumPartTypes,
  };
  Type m_type;
};

}

#endif // ER_SHARED_ENUMS_H
