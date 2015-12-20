// Include Game specific funtions/definitions
#include "r3dPCH.h"
#include "r3d.h"

#include "r3dNetwork.h"

#include "../../../Eternity/SF/CmdProcessor/CmdProcessor.h"

#include "../../EclipseStudio/Sources/multiplayer/ClientGameLogic.h"
#include "../../EclipseStudio/Sources/multiplayer/P2PMessages.h"

#include "../../EclipseStudio/Sources/UI/HUDDisplay.h"

#include "../../EclipseStudio/Sources/ObjectsCode/AI/AI_Player.h"

extern bool r3dOutToLog(const char* Str, ...);

extern CommandProcessor * g_pCmdProc ;
extern HUDDisplay*	hudMain ;
