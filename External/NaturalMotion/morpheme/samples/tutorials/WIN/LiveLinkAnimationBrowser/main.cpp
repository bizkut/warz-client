// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMPrioritiesLogger.h"
#include "NMPlatform/NMSocket.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrDispatcher.h"
#include "morpheme/mrNetwork.h"
#include "comms/commsServer.h"
#include "comms/simpleAnimBrowserManager.h"
#include "comms/simpleConnectionManager.h"

#include <windows.h>    // Sleep()
#include <conio.h>      // _getch(), _kbhit()
//----------------------------------------------------------------------------------------------------------------------

#define MAX_NUM_CHARACTERS 5
#define MAX_NUM_CHARACTER_DEFS 5

#define MAX_PATH_LENGTH 512

namespace 
{

//----------------------------------------------------------------------------------------------------------------------
void initMorphemeRegistration()
{
  //----------------------------
  // Dispatchers
  uint32_t numDispatchers = 0;
  MR::Dispatcher* dispatchers[2];

  dispatchers[numDispatchers] = MR::DispatcherBasic::createAndInit();
  ++numDispatchers;

  //----------------------------
  // Register the dispatchers we just created
  for (uint32_t i = 0; i < numDispatchers; ++i)
    MR::CoreTaskIDs::registerNMCoreTasks(dispatchers[i]);

  //----------------------------
  // Release the dispatcher instances since we have finished initialising the static function tables
  for (uint32_t i = 0; i < numDispatchers; ++i)
    dispatchers[i]->releaseAndDestroy();
}

//----------------------------------------------------------------------------------------------------------------------
int32_t animationBrowser()
{
  //----------------------------
  // Create a logger for use by the runtime target components.
  static const int32_t LOGGER_PRIORITY = 100;
  NMP::PrioritiesLogger logger;

  //----------------------------
  // Create a connection manager and animation browser.
  //
  // These are the only two components required for this tutorial while other runtime target use cases may require
  // additional components. In fact we would only really need to provide an implementation of the animation browser
  // interface for our purposes but also providing a connection manager allows us to monitor connections to decide when
  // to quit the application.
  static const uint32_t MAX_SIMULTANEOUS_CONNCETIONS = MCOMMS::CommsServer::getMaxNumberOfConnections();

  MCOMMS::SimpleConnectionManager connectionManager;
#if defined(MR_ATTRIB_DEBUG_BUFFERING)
  MCOMMS::SimpleAnimBrowserManager animationBrowser(MAX_SIMULTANEOUS_CONNCETIONS, logger, LOGGER_PRIORITY, NULL);
#endif
  //----------------------------
  // Create and register the dispatchers required for the basic animationBrowser network.
  initMorphemeRegistration();

  //----------------------------
  // Setup the runtime target interface.
  //
  // The runtime target interface is a collection of the various runtime target components a COMMS server talks to. Only
  // those implementations required for functionality provided by a specific runtime target must be set and NULL is
  // given for all others.
  MCOMMS::RuntimeTargetInterface runtimeTarget(
    &connectionManager, //< ConnectionManagementInterface
#if defined(MR_ATTRIB_DEBUG_BUFFERING)
    &animationBrowser, //< AnimationBrowserInterface
#else
    NULL,
#endif
    NULL, //< NetworkManagementInterface
    NULL, //< NetworkDefLifecycleInterface
    NULL, //< NetworkLifecycleInterface
    NULL, //< EnvironmentManagementInterface
    NULL, //< SceneObjectManagementInterface
    NULL, //< DataManagementInterface
    NULL, //< PhysicsDataManagementInterface
    NULL, //< EuphoriaDataManagementInterface
    NULL, //< PlaybackManagementInterface
    NULL); //< PhysicsManipulationInterface

  //----------------------------
  // Setup a COMMS server.
  //
  // COMMS_PORT is the TCP port the COMMS server will listen on, this must match the morpheme:connect runtime target
  // configuration. We are going with port 4567 which is the same configuration used by the default runtime targets
  // shipped with morpheme:connect.
  static const int COMMS_PORT = 4567;
  MCOMMS::CommsServer commsServer(&runtimeTarget, COMMS_PORT);
  {
    if (!commsServer.startServer(true))
    {
      NMP_STDOUT("\nerror: Failed to start COMMS server ...");
      return false;
    }

    NMP_STDOUT("\nCOMMS server listening on %s:%i ...\n", NMP::GetNetworkAddress(), COMMS_PORT);
  }

  //----------------------------
  // Keep updating the COMMS server.
  //
  // Updating the COMMS server will poll for new connections as well as service existing ones. We keep updating COMMS
  // until a key has been pressed or the number of connections has gone done to zero.
  while (!connectionManager.connectionsReducedToZero())
  {
    // Abort if a key has been pressed.
    if (_kbhit())
    {
      _getch();
      break;
    }

    // Update the COMMS server
    commsServer.update();

    // Sleep for a little to keep the thread from eating up too much cycles.
    Sleep(10);
  }

  //----------------------------
  // Shutdown the COMMS server.
  commsServer.stopServer();
  NMP_STDOUT("COMMS server stopped\n");

  //----------------------------
  // ...
  NMP_STDOUT("Press any key to exit ...");
  _getch();

  return true;
}

} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
// morphemeLiveLinkAnimationBrowser: This tutorial demonstrates how to create a simple runtime target.
int32_t main(int32_t NMP_UNUSED(argc), char** NMP_UNUSED(argv))
{
  //----------------------------
  // Initialize morpheme.
  //
  // Please keep in mind this is a simplified morpheme initialization that does not initialize animation formats,
  // dispatchers or physics support. For a morpheme setup that is suitable for in-game use please refer to
  // Game::CharacterManager in the GameManagment library.
  NMP::Memory::init();

  MR::Manager::initMorphemeLib();
  MR::Manager::getInstance().allocateRegistry();
  MR::Manager::getInstance().finaliseInitMorphemeLib();
  initMorphemeRegistration();

  //----------------------------
  // Run animation browser.
  const int result = animationBrowser();

  //----------------------------
  // shutdown morpheme.
  MR::DispatcherBasic::term();
#ifdef NM_HOST_CELL_PPU
  MR::DispatcherPS3::term();
#endif // NM_HOST_CELL_PPU
  MR::Manager::termMorphemeLib();
  NMP::Memory::shutdown();

  return result;
}
//----------------------------------------------------------------------------------------------------------------------
