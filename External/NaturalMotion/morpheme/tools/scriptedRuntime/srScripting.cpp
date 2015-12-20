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

#include "scriptedRuntimeApp.h"
#include "NMPlatform/NMRNG.h"
#include "srScripting.h"
#include "NMSquirrelMathLib.h"
#include "srScene.h"
#include "srEnvironment.h"

// ---------------------------------------------------------------------------------------------------------------------
static void SqPrintFunc(HSQUIRRELVM SQUIRREL_UNUSED(v), const SQChar *s, ...)
{
  char buffer[8192];
  buffer[8191] = 0;
  NMP_VSPRINTF(buffer, 8191, s, (char *)(&s+1));

  printf("%s\n", buffer);
}

// ---------------------------------------------------------------------------------------------------------------------
void SqErrFunc(const SQChar *s, ...)
{
  char buffer[8192];
  buffer[8191] = 0;
  NMP_VSPRINTF(buffer, 8191, s, (char *)(&s+1));

  printf("! %s\n", buffer);
}

// ---------------------------------------------------------------------------------------------------------------------
static void SqCompilerErrorHandler(HSQUIRRELVM NMP_UNUSED(v), const SQChar *sErr,const SQChar *sSource,SQInteger line,SQInteger NMP_UNUSED(column))
{
  SqErrFunc("%s(%d) - error: %s", sSource, line, sErr);
}

// ---------------------------------------------------------------------------------------------------------------------
SQInteger SqErrorHandler(HSQUIRRELVM v)
{
  sqp::StackHandler stack(v);
  const SQChar *errorString = stack.GetString(2);

  SQStackInfos stackinfos;
  sq_stackinfos(v, 1, &stackinfos);

  SqErrFunc("%s(%d) - error: %s", stackinfos.source, stackinfos.line, errorString);

  return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
struct SqRNG
{
  SqRNG();
  void setSeed(uint32_t seed);
  float genFloat();
  float genFloatInRange(float minfv, float maxfv);
  uint32_t genUInt32();
  bool genBool() { return (genUInt32() & 1) == 1; }

  NM_INLINE static void registerForScripting(sqp::SquirrelVM& vm)
  {
    sqp::ClassDef<SqRNG>(vm, "RNG")
      .Func(&SqRNG::setSeed, "setSeed")
      .Func(&SqRNG::genFloat, "genFloat")
      .Func(&SqRNG::genFloatInRange, "genFloatInRange")
      .Func(&SqRNG::genUInt32, "genUInt32")
      .Func(&SqRNG::genBool, "genBool")
      ;
  }

private:

  NMP::RNG  m_rng;
};

DECLARE_INSTANCE_TYPE_NAME(SqRNG, "RNG");


// ---------------------------------------------------------------------------------------------------------------------
class ScriptEngineData
{
public:

  enum { eStringBufLen = 260 };

  ScriptEngineData() {}

  sqp::SquirrelVM     m_vm;
  Environment        *m_env;
  Scene               m_scene;

  char                m_nutFilename[eStringBufLen];
};

int32_t ScriptedScenario::m_returnValue = 0;

// ---------------------------------------------------------------------------------------------------------------------
void SetReturnValue(float value)
{
  ScriptedScenario::m_returnValue = (int32_t) value;
}

// ---------------------------------------------------------------------------------------------------------------------
ScriptedScenario::ScriptedScenario(Environment* env) :
  m_data(new ScriptEngineData)
{
  m_data->m_env = env;
  NMP_STRNCPY_S(m_data->m_nutFilename, ScriptEngineData::eStringBufLen, m_data->m_env->getScriptDir());
  NMP_STRNCAT_S(m_data->m_nutFilename, ScriptEngineData::eStringBufLen, m_data->m_env->getScenarioName());
  NMP_STRNCAT_S(m_data->m_nutFilename, ScriptEngineData::eStringBufLen, ".nut");

  m_data->m_vm.Initialise();
  m_data->m_vm.PushRootTable();
  m_data->m_vm.RegisterStdLibs();

  // setup the 3 different error reporting functions
  if (env->isLogOutputEnabled())
    m_data->m_vm.SetPrintFunc(SqPrintFunc);
  sq_setcompilererrorhandler(m_data->m_vm.GetVMPtr(), SqCompilerErrorHandler);
  sq_newclosure(m_data->m_vm.GetVMPtr(), &SqErrorHandler, 0);
  sq_seterrorhandler(m_data->m_vm.GetVMPtr());

  m_data->m_vm.GetConstTable().SetValue("UPDATE_TIMESTEP", (1.0f / m_data->m_env->getSimulationRate()));
  m_data->m_vm.GetConstTable().SetValue("SCENARIO_NAME", m_data->m_env->getScenarioName());
  m_data->m_vm.GetConstTable().SetValue("NETWORKS_DIR", m_data->m_env->getNetworksDir());

  // create the environment to work with; initializing morpheme, euphoria and a PhysX 3.0 scene
  m_data->m_scene.init(env);

  // bind extensions
  m_data->m_vm.CreateFunction(SetReturnValue, "SetReturnValue", false);

  MathLib::registerForScripting(m_data->m_vm);
  SqRNG::registerForScripting(m_data->m_vm);
  m_data->m_scene.registerForScripting(m_data->m_vm);
  m_data->m_env->registerForScripting(m_data->m_vm);
}

// ---------------------------------------------------------------------------------------------------------------------
ScriptedScenario::~ScriptedScenario()
{
  delete m_data;
}

// ---------------------------------------------------------------------------------------------------------------------
bool ScriptedScenario::run()
{
  // check to see that our generated path is valid
  FILE *doesExist;
  fopen_s(&doesExist, m_data->m_nutFilename, "rt");

  if (!doesExist)
  {
    SqErrFunc("Could not open scenario file (%s)", m_data->m_nutFilename);
    return false;
  }
  fclose(doesExist);

  // tag the output with environmental config stats
  if (m_data->m_env->isLogOutputEnabled())
  {
    NMP_MSG("Executing : %s", m_data->m_env->getScenarioName());
    NMP_MSG("------------------------------------------------");
  }

  // compile and run the script
  SQRESULT sqResult = 0;
  sqp::SquirrelObject compiledScript = m_data->m_vm.CompileScript(m_data->m_nutFilename, true, &sqResult);

  if (SQ_SUCCEEDED(sqResult) && compiledScript.GetType() != OT_NULL)
  {
    sqp::SquirrelObject retObj = m_data->m_vm.RunScript(compiledScript, 0, true, &sqResult);

    return SQ_SUCCEEDED(sqResult);
  }

  return false;
}


// ---------------------------------------------------------------------------------------------------------------------
SqRNG::SqRNG()
{
}

// ---------------------------------------------------------------------------------------------------------------------
void SqRNG::setSeed(uint32_t seed)
{
  m_rng.setSeed(seed);
}

// ---------------------------------------------------------------------------------------------------------------------
uint32_t SqRNG::genUInt32()
{
  return m_rng.genUInt32();
}

// ---------------------------------------------------------------------------------------------------------------------
float SqRNG::genFloat()
{
  return m_rng.genFloat();
}

// ---------------------------------------------------------------------------------------------------------------------
float SqRNG::genFloatInRange(float minfv, float maxfv)
{
  return m_rng.genFloat(minfv, maxfv);
}
