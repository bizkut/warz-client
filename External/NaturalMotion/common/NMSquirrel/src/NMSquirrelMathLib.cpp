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

#include <stdio.h>

// NMSquirrel
#include "sqplus/sqplus.h"
#include "NMSquirrelMathLib.h"

#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMQuat.h"

// ---------------------------------------------------------------------------------------------------------------------
DECLARE_INSTANCE_TYPE_NAME(NMP::Vector3, Vector3);
DECLARE_INSTANCE_TYPE_NAME(NMP::Matrix34, Matrix34);
DECLARE_INSTANCE_TYPE_NAME(NMP::Quat, Quat);

DECLARE_ENUM_TYPE(NMP::Vector3::InitType);

// ---------------------------------------------------------------------------------------------------------------------
SQInteger Vector3Constructor(HSQUIRRELVM vm)
{
  sqp::StackHandler stack(vm);

  if (stack.GetParamCount() == 4)
  {
    if (!stack.IsNumber(2))
    {
      return stack.ThrowParamError(2, _SC("number"));
    }
    if (!stack.IsNumber(3))
    {
      return stack.ThrowParamError(3, _SC("number"));
    }
    if (!stack.IsNumber(4))
    {
      return stack.ThrowParamError(4, _SC("number"));
    }

    void *memory = sq_malloc(sizeof(NMP::Vector3));
    NMP::Vector3 *instance = new (memory) NMP::Vector3(
      stack.GetFloat(2),
      stack.GetFloat(3),
      stack.GetFloat(4));

    return sqp::PostConstruct<NMP::Vector3>(vm, instance);
  }

  return sqp::DefaultConstructor<NMP::Vector3>(vm);
}

// ---------------------------------------------------------------------------------------------------------------------
SQInteger Vector3Normalise(HSQUIRRELVM vm)
{
  sqp::StackHandler stack(vm);

  if (stack.GetParamCount() == 1)
  {
    NMP::Vector3* vecInst = stack.GetInstance<NMP::Vector3>(1);
    if (vecInst)
    {
      float mag = vecInst->normaliseGetLength();
      return stack.Return(mag);
    }
    else
    {
      return stack.ThrowParamError(1, "Vector3");
    }
  }

  return stack.ThrowError("Too Many Params");
}

// ---------------------------------------------------------------------------------------------------------------------
SQInteger Vector3ToString(HSQUIRRELVM vm)
{
  sqp::StackHandler stack(vm);

  if (stack.GetParamCount() == 1)
  {
    NMP::Vector3* vecInst = stack.GetInstance<NMP::Vector3>(1);
    if (vecInst)
    {
      SQChar * scratch = (SQChar*)sq_getscratchpad(vm, 128);
      NMP_SPRINTF(scratch, 128, "[%g, %g, %g]", vecInst->x, vecInst->y, vecInst->z);

      return stack.Return(scratch, strlen(scratch));
    }
    else
    {
      return stack.ThrowParamError(1, "Vector3");
    }
  }

  return stack.ThrowError("Too Many Params");
}

// ---------------------------------------------------------------------------------------------------------------------
SQInteger Matrix34Constructor(HSQUIRRELVM vm)
{
  sqp::StackHandler stack(vm);

  if (stack.GetParamCount() == 1)
  {
    void *memory = sq_malloc(sizeof(NMP::Matrix34));
    NMP::Matrix34 *instance = new (memory) NMP::Matrix34(NMP::Matrix34::kIdentity);

    return sqp::PostConstruct<NMP::Matrix34>(vm, instance);
  }

  return stack.ThrowError("Too Many Params");
}

// ---------------------------------------------------------------------------------------------------------------------
SQInteger QuatConstructor(HSQUIRRELVM vm)
{
  sqp::StackHandler stack(vm);

  if (stack.GetParamCount() == 1)
  {
    void *memory = sq_malloc(sizeof(NMP::Quat));
    NMP::Quat *instance = new (memory) NMP::Quat(NMP::Quat::kIdentity);

    return sqp::PostConstruct<NMP::Quat>(vm, instance);
  }

  return stack.ThrowError("Too Many Params");
}

// ---------------------------------------------------------------------------------------------------------------------
SQInteger QuatToString(HSQUIRRELVM vm)
{
  sqp::StackHandler stack(vm);

  if (stack.GetParamCount() == 1)
  {
    NMP::Quat* qInst = stack.GetInstance<NMP::Quat>(1);
    if (qInst)
    {
      SQChar * scratch = (SQChar*)sq_getscratchpad(vm, 128);
      NMP_SPRINTF(scratch, 128, "[%g, %g, %g, %g]", qInst->x, qInst->y, qInst->z, qInst->w);

      return stack.Return(scratch, strlen(scratch));
    }
    else
    {
      return stack.ThrowParamError(1, "Quat");
    }
  }

  return stack.ThrowError("Too Many Params");
}


// ---------------------------------------------------------------------------------------------------------------------
namespace MathLib
{

void registerForScripting(sqp::SquirrelVM& vm)
{
  sqp::ClassDefNoConstructor<NMP::Vector3>(vm, "Vector3")
    .SquirrelFunc(&Vector3Constructor, "constructor")
    .SquirrelFunc(&Vector3ToString, "_tostring", 1, "x")
    .Func<void (NMP::Vector3::*)(float, float, float)>(&NMP::Vector3::set, "set")
    .Func(&NMP::Vector3::magnitude, "magnitude")
    .Func(&NMP::Vector3::dot, "dot")
    .Func(&NMP::Vector3::transform, "transform")
    .Func(&NMP::Vector3::rotate, "rotate")
    .Func(&NMP::Vector3::inverseTransform, "inverseTransform")
    .Func(&NMP::Vector3::inverseRotate, "inverseRotate")
    .Func(&NMP::Vector3::operator+, "_add")
    .Func<NMP::Vector3 (NMP::Vector3::*)() const>(&NMP::Vector3::operator-, "_unm")
    .Func<NMP::Vector3 (NMP::Vector3::*)(const NMP::Vector3&) const>(&NMP::Vector3::operator-, "_sub")
    .Func<NMP::Vector3 (NMP::Vector3::*)(float) const>(&NMP::Vector3::operator*, "_mul")
    .Func(&NMP::Vector3::operator/, "_div")
    .SquirrelFunc(&Vector3Normalise, "normalise", 1, "x")
    .Var(&NMP::Vector3::x, "x")
    .Var(&NMP::Vector3::y, "y")
    .Var(&NMP::Vector3::z, "z")
    ;

  sqp::ClassDefNoConstructor<NMP::Matrix34>(vm, "Matrix34")
    .SquirrelFunc(&Matrix34Constructor, "constructor")
    .Func(&NMP::Matrix34::set3x3ToXRotation, "set3x3ToXRotation")
    .Func(&NMP::Matrix34::set3x3ToYRotation, "set3x3ToYRotation")
    .Func(&NMP::Matrix34::set3x3ToZRotation, "set3x3ToZRotation")
    .Func(&NMP::Matrix34::orthonormalise, "orthonormalise")
    .Func(&NMP::Matrix34::fromEulerXYZ, "fromEulerXYZ")
    .Func(&NMP::Matrix34::toEulerXYZ, "toEulerXYZ")
    .Func(&NMP::Matrix34::fromQuat, "fromQuat")
    .Func(&NMP::Matrix34::toQuat, "toQuat")
    .Func<NMP::Matrix34 (NMP::Matrix34::*)(const NMP::Matrix34&) const>(&NMP::Matrix34::operator*, "_mul")
    .Func<void (NMP::Matrix34::*)(NMP::Vector3&) const> (&NMP::Matrix34::transformVector, "transformVector")
    .Func<void (NMP::Matrix34::*)(NMP::Vector3&) const> (&NMP::Matrix34::rotateVector, "rotateVector")
    .Func<void (NMP::Matrix34::*)(NMP::Vector3&) const> (&NMP::Matrix34::inverseTransformVector, "inverseTransformVector")
    .Func<void (NMP::Matrix34::*)(NMP::Vector3&) const> (&NMP::Matrix34::inverseRotateVector, "inverseRotateVector")    
    .Func<void (NMP::Matrix34::*)(const NMP::Matrix34&)> (&NMP::Matrix34::multiply, "multiply")
    .Func<bool (NMP::Matrix34::*)()> (&NMP::Matrix34::invert, "invert")
    .Func<NMP::Vector3& (NMP::Matrix34::*)()> (&NMP::Matrix34::xAxis, "xAxis")
    .Func<NMP::Vector3& (NMP::Matrix34::*)()> (&NMP::Matrix34::yAxis, "yAxis")
    .Func<NMP::Vector3& (NMP::Matrix34::*)()> (&NMP::Matrix34::zAxis, "zAxis")
    .Func<NMP::Vector3& (NMP::Matrix34::*)()> (&NMP::Matrix34::translation, "translation")
    .Func(&NMP::Matrix34::setXAxis, "setXAxis")
    .Func(&NMP::Matrix34::setYAxis, "setYAxis")
    .Func(&NMP::Matrix34::setZAxis, "setZAxis")
    .Func(&NMP::Matrix34::setTranslation, "setTranslation")
    ;

  sqp::ClassDefNoConstructor<NMP::Quat>(vm, "Quat")
    .SquirrelFunc(&QuatConstructor, "constructor")
    .SquirrelFunc(&QuatToString, "_tostring", 1, "x")
    .Func(&NMP::Quat::conjugate, "conjugate")
    .Func<float (NMP::Quat::*)()>(&NMP::Quat::normaliseGetLength, "normaliseGetLength")
    .Func<void (NMP::Quat::*)(const NMP::Quat&)>(&NMP::Quat::multiply, "multiply")
    .Func<NMP::Vector3 (NMP::Quat::*)(const NMP::Vector3&) const>(&NMP::Quat::rotateVector, "rotateVector")
    .Func<NMP::Vector3 (NMP::Quat::*)(const NMP::Vector3&) const>(&NMP::Quat::inverseRotateVector, "inverseRotateVector")
    .Func(&NMP::Quat::fromEulerXYZ, "fromEulerXYZ")
    .Func(&NMP::Quat::toEulerXYZ, "toEulerXYZ")
    .Func(&NMP::Quat::fromAxisAngle, "fromAxisAngle")
    ;
}

} // namespace MathLib
