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
class TestBehaviourBase
{
  m_testCounter = 0;
  m_numTests = 0;
  m_frameCount = 0;
  m_testTime = 0;
  m_character = null;
}

//----------------------------------------------------------------------------------------------------------------------
// Test the ability of the balancer to recover from horizontal prods
class Stepping extends TestBehaviourBase
{
  // Parameters
  m_frameCounterForProd = 60; // frames after the restore
  m_maxSteppingTime = 10.0;
  m_totalTimeStanding = 0.0;
};

//----------------------------------------------------------------------------------------------------------------------
function Stepping::startTest()
{
}

//----------------------------------------------------------------------------------------------------------------------
function Stepping::startPass()
{
  m_character.broadcastMessage("Stagger");
}

//----------------------------------------------------------------------------------------------------------------------
function Stepping::hasPassFinished()
{
  return m_testTime > m_maxSteppingTime;
}

//----------------------------------------------------------------------------------------------------------------------
function Stepping::endPass()
{
  if (m_character.isBalanced())
  {
    local timeSinceLastStep = m_character.getTimeSinceLastStep();
    m_totalTimeStanding += timeSinceLastStep;
    print("End of test pass: time since last step = " + timeSinceLastStep);
  }
  else
  {
    print("End of test pass: not balanced");
  }
  m_character.broadcastMessage("SK");
}

//----------------------------------------------------------------------------------------------------------------------
// The percentage of time balanced
function Stepping::getFitness()
{
  return 100.0 * m_totalTimeStanding / (m_numTests * m_maxSteppingTime);
} 

//----------------------------------------------------------------------------------------------------------------------
function Stepping::update(timeStep)
{
  if (m_frameCount == m_frameCounterForProd)
  {
    // Test a range of strengths
    local impulseMag = 90.0;
    impulseMag += (m_testCounter % 3) * 30.0
    local impulse = Vector3(impulseMag, 0.0, 0.0);

    local quat = Quat();
    local prodBearing = (2.0*3.1415 * m_testCounter) / m_numTests;
    quat.fromAxisAngle(Vector3(0,1,0), prodBearing);
    impulse = quat.rotateVector(impulse);
    m_character.addImpulseToChar(impulse);

    m_testTime = 0.0;
  }
}

scene.setCurrentDirectory(NETWORKS_DIR);

// Load the network specified on the command line
local myDef = scene.loadDefaultNetworkDef();

// Create a character
local charTM = Matrix34();
charTM.setTranslation( Vector3(0, 0, 0) );
local behaviour = Stepping();
behaviour.m_numTests = 360;
behaviour.m_character = myDef.createInstance("Fred", charTM);
// Fixed ground box
scene.physics().createStaticBox( Vector3(0, -1, 0), Vector3(20, 0.95, 20), Quat(), 1.0, 0.0, 1.0);
behaviour.startTest();

// Run for a while and then terminate
print("phase #3");
local frameCounter = 0;
local relaxTime = 50;

local maxFrames = 1000 + (behaviour.m_numTests * (behaviour.m_maxSteppingTime + relaxTime) / UPDATE_TIMESTEP)
local totalFrames = 0

for (totalFrames=0; totalFrames < maxFrames; totalFrames++) 
{
  if (frameCounter == relaxTime)
  {
    behaviour.m_character.storeState();
    behaviour.startPass();
  }
  else if (frameCounter > relaxTime)
  {
    behaviour.update(UPDATE_TIMESTEP);
  }
  scene.updateAll(UPDATE_TIMESTEP);
  
  if (frameCounter > relaxTime && behaviour.hasPassFinished())
  {
    behaviour.endPass();
    // Move character
    
    behaviour.m_testCounter++;
    behaviour.m_testTime = 0.0;
    behaviour.m_frameCount = 0;
    if (behaviour.m_testCounter == behaviour.m_numTests)
    {
      print("Unit test complete. Fitness = " + behaviour.getFitness());
      SetReturnValue(1000 * behaviour.getFitness());
      break;
    }

    behaviour.m_character.restoreState();
    frameCounter = 0;
  }

  frameCounter++;
  behaviour.m_frameCount++;
  behaviour.m_testTime += UPDATE_TIMESTEP;
}

print("End after " + totalFrames + " frames");
