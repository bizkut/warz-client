#include "r3dPCH.h"
#include "r3d.h"

#define ENABLE_TEAMSPEAK 1

#ifdef ENABLE_TEAMSPEAK
// team speak 3 
#include <public_definitions.h>
#include <public_errors.h>
#include <clientlib_publicdefinitions.h>
#include <clientlib.h>
#pragma comment(lib, "../external/ts3_sdk_3/lib/ts3client_win32.lib")

#include "ObjectsCode/AI/AI_Player.h"
#include "multiplayer/ClientGameLogic.h"
#include "ui/HUDDisplay.h"

#include "TeamSpeakClient.h"

extern HUDDisplay*	hudMain;

CTeamSpeakClient	gTeamSpeakClient;
static unsigned int	error;		// last error

const	float		VOICE_PROXIMITY_CHANNEL_DIST = 200.0f;

#define CHECK_ERROR(x) if((error = x) != ERROR_ok) { goto on_error; }

static int TSHandleError(const char* func, bool isError = false)
{
#ifdef FINAL_BUILD
	if(!isError)
		return 0;
#endif

	char* errormsg;
	if(ts3client_getErrorMessage(error, &errormsg) == ERROR_ok)
	{
		r3dOutToLog("VoipError: %s %d %s\n", func, error, errormsg);
		ts3client_freeMemory(errormsg);
	}
	else
	{
		r3dOutToLog("VoipError: %s %d\n", func, error);
	}
	return 0;
}


/*
 * Callback for connection status change.
 * Connection status switches through the states STATUS_DISCONNECTED, STATUS_CONNECTING, STATUS_CONNECTED and STATUS_CONNECTION_ESTABLISHED.
 *
 * Parameters:
 *   serverConnectionHandlerID - Server connection handler ID
 *   newStatus                 - New connection status, see the enum ConnectStatus in clientlib_publicdefinitions.h
 *   errorNumber               - Error code. Should be zero when connecting or actively disconnection.
 *                               Contains error state when losing connection.
 */
static void onConnectStatusChangeEvent(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber) 
{
#ifndef FINAL_BUILD
	r3dOutToLog("voip: Connect status changed: %llu %d %d\n", (unsigned long long)serverConnectionHandlerID, newStatus, errorNumber);
#endif
	
	if(newStatus == STATUS_CONNECTION_ESTABLISHED)
	{
		gTeamSpeakClient.AddEvent(-1, CTeamSpeakClient::EVT_Connected);

		// retreive my client id
		anyID myClientID;
		if((error = ts3client_getClientID(serverConnectionHandlerID, &myClientID)) != ERROR_ok)
			TSHandleError("getClientID", true);
		gTeamSpeakClient.m_myClientID = myClientID;

		/*
		char *name, *quality, *bitrate;
		ts3client_getEncodeConfigValue(gTeamSpeakClient.scHandlerID, "name", &name);
		ts3client_getEncodeConfigValue(gTeamSpeakClient.scHandlerID, "quality", &quality);
		ts3client_getEncodeConfigValue(gTeamSpeakClient.scHandlerID, "bitrate", &bitrate);
		r3dOutToLog("VoIp encoder config: name = %s, quality = %s, bitrate = %s\n", name, quality, bitrate);
		ts3client_freeMemory(name);
		ts3client_freeMemory(quality);
		ts3client_freeMemory(bitrate);
		*/

		return;
	}
	
	if(newStatus == STATUS_DISCONNECTED) 
	{
		gTeamSpeakClient.AddEvent(-1, CTeamSpeakClient::EVT_FailedToConnect);
		gTeamSpeakClient.m_myClientID = -1;
		return;
	}
}

/*
 * Callback for current channels being announced to the client after connecting to a server.
 *
 * Parameters:
 *   serverConnectionHandlerID - Server connection handler ID
 *   channelID                 - ID of the announced channel
 *    channelParentID           - ID of the parent channel
 */
static void onNewChannelEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 channelParentID)
{
}

/*
 * Callback for just created channels.
 *
 * Parameters:
 *   serverConnectionHandlerID - Server connection handler ID
 *   channelID                 - ID of the announced channel
 *   channelParentID           - ID of the parent channel
 *    invokerID                 - ID of the client who created the channel
 *   invokerName               - Name of the client who created the channel
 */
static void onNewChannelCreatedEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 channelParentID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier)
{
}

/*
 * Callback when a channel was deleted.
 *
 * Parameters:
 *   serverConnectionHandlerID - Server connection handler ID
 *   channelID                 - ID of the deleted channel
 *   invokerID                 - ID of the client who deleted the channel
 *   invokerName               - Name of the client who deleted the channel
 */
static void onDelChannelEvent(uint64 serverConnectionHandlerID, uint64 channelID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier)
{
}

/*
 * Called when a client joins, leaves or moves to another channel.
 *
 * Parameters:
 *   serverConnectionHandlerID - Server connection handler ID
 *   clientID                  - ID of the moved client
 *   oldChannelID              - ID of the old channel left by the client
 *   newChannelID              - ID of the new channel joined by the client
 *   visibility                - Visibility of the moved client. See the enum Visibility in clientlib_publicdefinitions.h
 *                               Values: ENTER_VISIBILITY, RETAIN_VISIBILITY, LEAVE_VISIBILITY
 */
static void onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* moveMessage)
{
#ifndef FINAL_BUILD
	r3dOutToLog("voip: ClientID %u moves from channel %llu to %llu with message %s and visibility %d\n", clientID, (unsigned long long)oldChannelID, (unsigned long long)newChannelID, moveMessage, visibility);
#endif	

	// channel 0 is not existing channel, moving from/to it mean that player is connected/disconnected
	// so this is another place to check for newly connected users
	if(oldChannelID > 0 && newChannelID == 0)
	{
		gTeamSpeakClient.OnClientDissapear(false, clientID);
		return;
	}
	if(oldChannelID == 0 && newChannelID > 0)
	{
		gTeamSpeakClient.OnClientAppear(clientID);
		return;
	}
}

/*
 * Callback for other clients in current and subscribed channels being announced to the client.
 *
 * Parameters:
 *   serverConnectionHandlerID - Server connection handler ID
 *   clientID                  - ID of the announced client
 *   oldChannelID              - ID of the subscribed channel where the client left visibility
 *   newChannelID              - ID of the subscribed channel where the client entered visibility
 *   visibility                - Visibility of the announced client. See the enum Visibility in clientlib_publicdefinitions.h
 *                               Values: ENTER_VISIBILITY, RETAIN_VISIBILITY, LEAVE_VISIBILITY
 */
static void onClientMoveSubscriptionEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility)
{
#ifndef FINAL_BUILD
	r3dOutToLog("voip: onClientMoveSubscriptionEvent: %d, visibility:%d\n", clientID, visibility);
#endif
	gTeamSpeakClient.OnClientAppear(clientID);
}

/*
 * Called when a client drops his connection.
 *
 * Parameters:
 *   serverConnectionHandlerID - Server connection handler ID
 *   clientID                  - ID of the moved client
 *   oldChannelID              - ID of the channel the leaving client was previously member of
 *   newChannelID              - 0, as client is leaving
 *   visibility                - Always LEAVE_VISIBILITY
 *   timeoutMessage            - Optional message giving the reason for the timeout
 */
static void onClientMoveTimeoutEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* timeoutMessage)
{
#ifndef FINAL_BUILD
	r3dOutToLog("voip: ClientID %u timeouts with message %s\n", clientID, timeoutMessage);
#endif	
	gTeamSpeakClient.OnClientDissapear(false, clientID);
}

static void onClientKickFromChannelEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char* kickerName, const char* kickerUniqueIdentifier, const char* kickMessage)
{
	// we have no idea what to do here... maybe other clients should report to server about kick???
}

static void onClientKickFromServerEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char* kickerName, const char* kickerUniqueIdentifier, const char* kickMessage)
{
	// we have no idea what to do here... maybe other clients should report to server about kick???
}

static void onServerErrorEvent(uint64 serverConnectionHandlerID, const char* errorMessage, unsigned int error, const char* returnCode, const char* extraMessage)
{
	r3dOutToLog("voip: !!! Error for server: %s (%u) %s\n", errorMessage, error, extraMessage);
}

/*
 * This event is called when a client starts or stops talking.
 *
 * Parameters:
 *   serverConnectionHandlerID - Server connection handler ID
 *   status                    - 1 if client starts talking, 0 if client stops talking
 *   isReceivedWhisper         - 1 if this event was caused by whispering, 0 if caused by normal talking
 *   clientID                  - ID of the client who announced the talk status change
 */
static void onTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID)
{
	// make sure we have that clientid 
	// don't work for local client, that event happens after onConnectStatusChangeEvent with disconnect
	// -- r3d_assert(gTeamSpeakClient.m_ClientToPlayerIdx.find(clientID) != gTeamSpeakClient.m_ClientToPlayerIdx.end());
	
	/*
	if(status == STATUS_TALKING) {
		r3dOutToLog("Client %d, plr:%d starts talking.\n", clientID, playerIdx);
	} else {
		r3dOutToLog("Client %d, plr:%d stops talking.\n", clientID, playerIdx);
	}*/
	gTeamSpeakClient.AddEvent(clientID, status == STATUS_TALKING ? CTeamSpeakClient::EVT_StartTalking : CTeamSpeakClient::EVT_StopTalking);
}

/*
 * This event is called when another client starts whispering to own client. Own client can decide to accept or deny
 * receiving the whisper by adding the sending client to the whisper allow list. If not added, whispering is blocked.
 *
 * Parameters:
 *   serverConnectionHandlerID - Server connection handler ID
 *   clientID                  - ID of the whispering client
 */
static void onIgnoredWhisperEvent(uint64 serverConnectionHandlerID, anyID clientID)
{
	// no whispers
}

/*
 * Callback allowing access to voice data after it has been mixed by TeamSpeak 3
 * This event can be used to alter/add to the voice data being played by TeamSpeak 3.
 * But here we use it to record the voice data to a wave file.
 *
 * Parameters:
 *   serverConnectionHandlerID - Server connection handler ID
 *   samples - Pointer to a buffer containg 16 bit voice data samples at 48000 Hz. Channels are interleaved.
 *   sampleCount - The number of samples 1 channel of sample data contains. 
 *   channels - The number of channels the sample data contains.
 *   channelSpeakerArray - A bitmask of the speakers for each channel.
 *   channelFillMask - A bitmask of channels that actually have valid data.
 *
 * -The size of the data "samples" points to is: sizeof(short)*sampleCount*channels
 * -channelSpeakerArray uses SPEAKER_ defined in public_definitions.h
 * -In the interrest of optimizations, a channel only contains data, if there is sound data for it. For example:
 * in 5.1 or 7.1 we (almost) never have data for the subwoofer. Teamspeak then leaves the data in this channel
 * undefined. This is more efficient for mixing.
 * This implementation will record sound to a 2 channel (stereo) wave file. This sample assumes there is only
 * 1 connection to a server
 * Hint: Normally you would want to defer the writing to an other thread because this callback is very time sensitive
 */
static void onEditMixedPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask)
{
}

static void onEditCapturedVoiceDataEvent(uint64 serverConnectionHandlerID, short* samples, int sampleCount, int channels, int* edited)
{
	// store to wav?
}

static void onCustom3dRolloffCalculationClientEvent(uint64 serverConnectionHandlerID, anyID clientID, float distance, float* volume)
{
	float prevVolume = *volume;
	// linear rolloff
	if(distance > VOICE_PROXIMITY_CHANNEL_DIST)
		*volume = 0.0f;
	else
		*volume = 1.0f - (distance / VOICE_PROXIMITY_CHANNEL_DIST);

	//r3dOutToLog("3dsound: client:%d dist:%f, volume:%f -> %f\n", clientID, distance, prevVolume, *volume);
}


/*
 * Callback for user-defined logging.
 *
 * Parameter:
 *   logMessage        - Log message text
 *   logLevel          - Severity of log message
 *   logChannel        - Custom text to categorize the message channel
 *   logID             - Virtual server ID giving the virtual server source of the log event
 *   logTime           - String with the date and time the log entry occured
 *   completeLogString - Verbose log message including all previous parameters for convinience
 */
static void onUserLoggingMessageEvent(const char* logMessage, int logLevel, const char* logChannel, uint64 logID, const char* logTime, const char* completeLogString)
{
#ifdef FINAL_BUILD
	// ignore not important errors in final client
	if(logLevel < LogLevel_ERROR)
		return;
#endif
	r3dOutToLog("VoipLog:[%d] %s\n", logLevel, completeLogString);
}

/*
 * Callback allowing to apply custom encryption to outgoing packets.
 * Using this function is optional. Do not implement if you want to handle the TeamSpeak 3 SDK encryption.
 *
 * Parameters:
 *   dataToSend - Pointer to an array with the outgoing data to be encrypted
 *   sizeOfData - Pointer to an integer value containing the size of the data array
 *
 * Apply your custom encryption to the data array. If the encrypted data is smaller than sizeOfData, write
 * your encrypted data into the existing memory of dataToSend. If your encrypted data is larger, you need
 * to allocate memory and redirect the pointer dataToSend. You need to take care of freeing your own allocated
 * memory yourself. The memory allocated by the SDK, to which dataToSend is originally pointing to, must not
 * be freed.
 * 
 */
static void onCustomPacketEncryptEvent(char** dataToSend, unsigned int* sizeOfData)
{
#ifdef USE_CUSTOM_ENCRYPTION
	for(unsigned int i = 0; i < *sizeOfData; i++) {
		(*dataToSend)[i] ^= CUSTOM_CRYPT_KEY;
	}
#endif
}

/*
 * Callback allowing to apply custom encryption to incoming packets
 * Using this function is optional. Do not implement if you want to handle the TeamSpeak 3 SDK encryption.
 *
 * Parameters:
 *   dataToSend - Pointer to an array with the received data to be decrypted
 *   sizeOfData - Pointer to an integer value containing the size of the data array
 *
 * Apply your custom decryption to the data array. If the decrypted data is smaller than dataReceivedSize, write
 * your decrypted data into the existing memory of dataReceived. If your decrypted data is larger, you need
 * to allocate memory and redirect the pointer dataReceived. You need to take care of freeing your own allocated
 * memory yourself. The memory allocated by the SDK, to which dataReceived is originally pointing to, must not
 * be freed.
 */
static void onCustomPacketDecryptEvent(char** dataReceived, unsigned int* dataReceivedSize)
{
#ifdef USE_CUSTOM_ENCRYPTION
	for(unsigned int i = 0; i < *dataReceivedSize; i++) {
		(*dataReceived)[i] ^= CUSTOM_CRYPT_KEY;
	}
#endif
}

static bool readIdentity(char* buf, DWORD size)
{
	// query for game registry node
	HKEY hKey;
	int hr;
	hr = RegOpenKeyEx(HKEY_CURRENT_USER, 
		"Software\\Arktos Entertainment Group\\WarZ", 
		0, 
		KEY_ALL_ACCESS, 
		&hKey);
	if(hr != ERROR_SUCCESS)
		return false;

	hr = RegQueryValueEx(hKey, "TSIdentity", NULL, NULL, (BYTE*)buf, &size);
	if(hr != ERROR_SUCCESS)
		return false;
		
	return true;
}

int CTeamSpeakClient::InitializeIdentity()
{
	// try to load from registry
	char buf[2048];
	if(readIdentity(buf, sizeof(buf)))
	{
		m_identity = buf;
		return 1;
	}

	// create new
	char* id;
	if((error = ts3client_createIdentity(&id)) != ERROR_ok) {
		r3dOutToLog("Error creating identity: %d\n", error);
		return 0;
	}
	
	// store it
	m_identity = id;
	
	// and save it
	HKEY hKey;
	int hr;
	hr = RegCreateKeyEx(HKEY_CURRENT_USER, 
		"Software\\Arktos Entertainment Group\\WarZ", 
		0, 
		NULL,
		REG_OPTION_NON_VOLATILE, 
		KEY_ALL_ACCESS,
		NULL,
		&hKey,
		NULL);
	if(hr == ERROR_SUCCESS)
	{
		hr = RegSetValueEx(hKey, "TSIdentity", NULL, REG_SZ, (BYTE*)id, strlen(id) + 1);
		RegCloseKey(hKey);
	}
	
	ts3client_freeMemory(id);
	return 1;
}

CTeamSpeakClient::CTeamSpeakClient()
{
	scHandlerID  = 0;
	m_myClientID = -1;

	m_captureOpened  = false;
	m_playbackOpened = false;
	
	m_usingPTT   = true;
	m_pttActive  = false;
	
	m_nextUpdate = -1;
	
	InitializeCriticalSection(&m_csEvents);
	InitializeCriticalSection(&m_csClients);
}

CTeamSpeakClient::~CTeamSpeakClient()
{
	DeleteCriticalSection(&m_csEvents);
	DeleteCriticalSection(&m_csClients);
}

int CTeamSpeakClient::Startup()
{
	r3dOutToLog("Starting voice\n"); CLOG_INDENT;

	// Query and print client lib version
#ifndef FINAL_BUILD	
	char* version;
	if((error = ts3client_getClientLibVersion(&version)) != ERROR_ok)
		return TSHandleError("getClientLibVersion", true);
	r3dOutToLog("Client lib version: %s\n", version);
	ts3client_freeMemory(version);
	version = NULL;
#endif	
	
	// Create struct for callback function pointers
	struct ClientUIFunctions funcs;
	memset(&funcs, 0, sizeof(struct ClientUIFunctions));

	// Callback function pointers
	// It is sufficient to only assign those callback functions you are using. When adding more callbacks, add those function pointers here.
	funcs.onConnectStatusChangeEvent        = onConnectStatusChangeEvent;
	funcs.onNewChannelEvent                 = onNewChannelEvent;
	funcs.onNewChannelCreatedEvent          = onNewChannelCreatedEvent;
	funcs.onDelChannelEvent                 = onDelChannelEvent;
	funcs.onClientMoveEvent                 = onClientMoveEvent;
	funcs.onClientMoveSubscriptionEvent     = onClientMoveSubscriptionEvent;
	funcs.onClientMoveTimeoutEvent          = onClientMoveTimeoutEvent;
	funcs.onClientKickFromChannelEvent      = onClientKickFromChannelEvent;
	funcs.onClientKickFromServerEvent       = onClientKickFromServerEvent;
	funcs.onServerErrorEvent                = onServerErrorEvent;
	funcs.onTalkStatusChangeEvent           = onTalkStatusChangeEvent;
	funcs.onIgnoredWhisperEvent             = onIgnoredWhisperEvent;
	funcs.onEditMixedPlaybackVoiceDataEvent = onEditMixedPlaybackVoiceDataEvent;
	funcs.onEditCapturedVoiceDataEvent      = onEditCapturedVoiceDataEvent;
	funcs.onCustom3dRolloffCalculationClientEvent = onCustom3dRolloffCalculationClientEvent;
	funcs.onUserLoggingMessageEvent         = onUserLoggingMessageEvent;
	funcs.onCustomPacketEncryptEvent        = onCustomPacketEncryptEvent;
	funcs.onCustomPacketDecryptEvent        = onCustomPacketDecryptEvent;
	funcs.onProvisioningSlotRequestResultEvent = NULL;

	// Initialize client lib with callbacks
	// Resource path points to the SDK\bin directory to locate the soundbackends folder when running from Visual Studio.
	// If you want to run directly from the SDK\bin directory, use an empty string instead to locate the soundbackends folder in the current directory.
	if((error = ts3client_initClientLib(&funcs, NULL, LogType_USERLOGGING, NULL, "./")) != ERROR_ok)
		return TSHandleError("initClientLib", true);
	
	// Spawn a new server connection handler using the default port and store the server ID
	if((error = ts3client_spawnNewServerConnectionHandler(0, &scHandlerID)) != ERROR_ok)
		return TSHandleError("spawnNewServerConnectionHandler", true);

	// identity
	InitializeIdentity();
	//r3dOutToLog("Using identity: %s\n", identity);


/*	// list of capture/playback modes (directsound vs windows audio)
	char** list;
	if(ts3client_getPlaybackModeList(&list) == ERROR_ok)
	{
		for(int i=0; list[i] != NULL; ++i)
		{
			r3dOutToLog("PlaybackMode: %s\n", list[i]);
			ts3client_freeMemory(list[i]);  // Free C-string
		}
		ts3client_freeMemory(list);  // Free the array
	}

	if(ts3client_getCaptureModeList(&list) == ERROR_ok)
	{
		for(int i=0; list[i] != NULL; ++i)
		{
			r3dOutToLog("CaptureMode: %s\n", list[i]);
			ts3client_freeMemory(list[i]);  // Free C-string
		}
		ts3client_freeMemory(list);  // Free the array
	}
*/	

	// Get default capture mode - note that those modes MUST exist even if we don't have audio card
	char* mode;
	if((error = ts3client_getDefaultCaptureMode(&mode)) != ERROR_ok)
		return TSHandleError("getDefaultCaptureMode");
	m_captureMode = mode;
	ts3client_freeMemory(mode);
	
	// Get default playback mode
	if((error = ts3client_getDefaultPlayBackMode(&mode)) != ERROR_ok)
		return TSHandleError("getDefaultPlayBackMode");
	m_playbackMode = mode;
	ts3client_freeMemory(mode);
	
	EnumeratePlaybackDevices(m_playbackMode.c_str());
	EnumerateCaptureDevices(m_captureMode.c_str());
	
	return 1;
}

int CTeamSpeakClient::Shutdown()
{
	if(scHandlerID == 0)
		return 1;
		
	r3dOutToLog("Shutting down voice\n"); CLOG_INDENT;
	
	Disconnect();
	
	// Destroy server connection handle
	if((error = ts3client_destroyServerConnectionHandler(scHandlerID)) != ERROR_ok)
		TSHandleError("destroyServerConnectionHandler");

	// Shutdown client lib
	if((error = ts3client_destroyClientLib()) != ERROR_ok)
		TSHandleError("destroyClientLib");

	scHandlerID = 0;
	
	return 1;
}

int CTeamSpeakClient::EnumeratePlaybackDevices(const char* mode)
{
	m_playbackDevices.clear();
	
	// Get default playback device
	m_defPlaybackId = "";
	{
		char** device;
		if((error = ts3client_getDefaultPlaybackDevice(mode, &device)) != ERROR_ok)
			return TSHandleError("getDefaultPlaybackDevice", true);
		m_defPlaybackId = device[1];

		ts3client_freeMemory(device[0]);
		ts3client_freeMemory(device[1]);
		ts3client_freeMemory(device);
	}
	
	char*** list;
	if(ts3client_getPlaybackDeviceList(mode, &list) == ERROR_ok)
	{
		for(int i=0; list[i] != NULL; ++i) 
		{
			dev_s dev;
			dev.name      = list[i][0];
			dev.id        = list[i][1];
			dev.isDefault = strcmp(list[i][1], m_defPlaybackId.c_str()) == 0;
			m_playbackDevices.push_back(dev);
			r3dOutToLog("Playback device: %s %s\n", list[i][0], dev.isDefault ? "(default)" : "");

			/* Free element */
			ts3client_freeMemory(list[i][0]);
			ts3client_freeMemory(list[i][1]);
			ts3client_freeMemory(list[i]);
		}
		ts3client_freeMemory(list);  /* Free complete array */
		return 1;
	}

	return TSHandleError("getPlaybackDeviceList");
}

int CTeamSpeakClient::EnumerateCaptureDevices(const char* mode)
{
	m_captureDevices.clear();
	
	// Get default capture device
	m_defCaptureId = "";
	{
		char** device;
		if((error = ts3client_getDefaultCaptureDevice(mode, &device)) != ERROR_ok)
			return TSHandleError("getDefaultCaptureDevice", true);
		m_defCaptureId = device[1];

		ts3client_freeMemory(device[0]);
		ts3client_freeMemory(device[1]);
		ts3client_freeMemory(device);
	}

	char*** list;
	if(ts3client_getCaptureDeviceList(mode, &list) == ERROR_ok)
	{
		for(int i=0; list[i] != NULL; ++i) 
		{
			dev_s dev;
			dev.name      = list[i][0];
			dev.id        = list[i][1];
			dev.isDefault = strcmp(list[i][1], m_defCaptureId.c_str()) == 0;
			m_captureDevices.push_back(dev);
			r3dOutToLog("Capture device: %s %s\n", list[i][0], dev.isDefault ? "(default)" : "");

			/* Free element */
			ts3client_freeMemory(list[i][0]);
			ts3client_freeMemory(list[i][1]);
			ts3client_freeMemory(list[i]);
		}
		ts3client_freeMemory(list);  /* Free complete array */
		return 1;
	}

	return TSHandleError("getCaptureDeviceList");
}

int CTeamSpeakClient::OpenDevices(int playbackIdx, int captureIdx)
{
	// close current devices
	ts3client_closePlaybackDevice(scHandlerID);
	ts3client_closeCaptureDevice(scHandlerID);
	
	m_captureOpened  = false;
	m_playbackOpened = false;

	if(m_playbackDevices.size() > 0)
	{
		const char* playbackDev;
		if(playbackIdx < 0)
			playbackDev = m_defPlaybackId.c_str();
		else
			playbackDev = m_playbackDevices[playbackIdx].id.c_str();

		// Open playback device
		if((error = ts3client_openPlaybackDevice(scHandlerID, m_playbackMode.c_str(), playbackDev)) != ERROR_ok)
			TSHandleError("openPlaybackDevice", true);
		else
			m_playbackOpened = true;
	}
	
	if(m_captureDevices.size() > 0)
	{
		const char* captureDev; 
		if(captureIdx < 0)
			captureDev = m_defCaptureId.c_str();
		else
			captureDev = m_captureDevices[captureIdx].id.c_str();

		// Open capture device
		if((error = ts3client_openCaptureDevice(scHandlerID, m_captureMode.c_str(), captureDev)) != ERROR_ok)
			TSHandleError("openCaptureDevice", true);
		else
			m_captureOpened = true;
	}

	SetPushToTalk(true);
	
	return 1;
}

void CTeamSpeakClient::StartConnect(const char* host, int port, DWORD voicePwd, DWORD voiceId)
{
	// all clients should be disconnected by that time. 
	// NOPE! sometime TS dont send proper disconnect events for clients
	// r3d_assert(m_ClientToPlayerIdx.size() == 0);

	// so just clear player map and hope that we're properly disconnected
	r3d_assert(m_myClientID == -1);
	m_ClientToPlayerIdx.clear();
	
	// there might be leftovers from previous session
	m_events = std::queue<evt_s>();

	char pwd[32];   sprintf(pwd,  "%u", voicePwd);
	char nick[32];  sprintf(nick, "%u", voiceId);

	if((error = ts3client_startConnection(scHandlerID, m_identity.c_str(), host, port, nick, NULL, "", pwd)) != ERROR_ok)
		TSHandleError("startConnection", true);
}

void CTeamSpeakClient::Disconnect()
{
	// Disconnect from server
	if((error = ts3client_stopConnection(scHandlerID, "leaving")) != ERROR_ok)
		TSHandleError("stopConnection");

	// we should not clear m_ClientToPlayerIdx - disconnect events will be fired after sucessful disconnect
}

void CTeamSpeakClient::Tick()
{
	if(m_usingPTT && m_captureOpened)
	{
		// push-to-talk logic
		bool shouldTalk = InputMappingMngr->isPressed(r3dInputMappingMngr::KS_VOICE_PTT);
		if(!(!win::bSuspended && !hudMain->isChatInputActive())) 
			shouldTalk = false; // do not allow to talk if you are in chat or window is minimized

		if(m_pttActive != shouldTalk)
		{
			if((error = ts3client_setClientSelfVariableAsInt(scHandlerID, CLIENT_INPUT_DEACTIVATED, shouldTalk ? INPUT_ACTIVE : INPUT_DEACTIVATED)) != ERROR_ok)
				TSHandleError("CLIENT_INPUT_DEACTIVATED");
			if((error = ts3client_flushClientSelfUpdates(scHandlerID, NULL)) != ERROR_ok)
				1==1; //by some reason we're getting ERROR_ok_no_update TSHandleError("flushClientSelfUpdates");

			m_pttActive = shouldTalk;
		}
	}
	
	// update 3d position
	if(m_playbackOpened)
	{
		TS3_VECTOR pos, fwd, up;
		pos.x = gCam.x;
		pos.y = gCam.y;
		pos.z = gCam.z;
		
		r3dPoint3D camFwd = r3dPoint3D(gCam.vPointTo.x, 0, gCam.vPointTo.z).NormalizeTo();
		fwd.x = camFwd.x;
		fwd.y = camFwd.y;
		fwd.z = camFwd.z;
		
		up.x  = 0;
		up.y  = 1;
		up.z  = 0;

		if((error = ts3client_systemset3DListenerAttributes(scHandlerID, &pos, &fwd, &up)) != ERROR_ok)
			TSHandleError("systemset3DListenerAttributes");
	}
	
	// update clients
	ClientGameLogic& CGL = gClientLogic();
	if(r3dGetTime() > m_nextUpdate)
	{
		r3dCSHolder csC(m_csClients);
		m_nextUpdate = r3dGetTime() + 0.1f;
	
		for(stdext::hash_map<int, int>::iterator it = m_ClientToPlayerIdx.begin(); it != m_ClientToPlayerIdx.end(); ++it)
		{
			// check if we have actual player for connected client
			int playerIdx = it->second;
			anyID clientID = it->first;
			obj_Player* plr = CGL.GetPlayer(playerIdx);

			// set 3d position 
			TS3_VECTOR pos;
			if(plr == NULL)
			{
				pos.x = gCam.x;
				pos.y = gCam.y + 10000;	// player doesn't exist in our client, move it out of our reach
				pos.z = gCam.z;
			}
			else
			{
				pos.x = plr->GetPosition().x;
				pos.y = plr->GetPosition().y;
				pos.z = plr->GetPosition().z;
			}
			if((error = ts3client_channelset3DAttributes(scHandlerID, clientID, &pos)) != ERROR_ok)
				TSHandleError("channelset3DAttributes");
				
			// mute/unmute based on distance + mute dead players
			bool isVisible = plr && (gCam - plr->GetPosition()).Length() < (VOICE_PROXIMITY_CHANNEL_DIST * 1.2f) && plr->bDead==0;
			if(CGL.playerNames[playerIdx].voiceEnabled != isVisible)
			{
				if(!CGL.playerNames[playerIdx].voiceIsMuted)
				{
					if(isVisible)
						UnmuteClient(clientID);
					else
						MuteClient(clientID);
				}
					
				CGL.playerNames[playerIdx].voiceEnabled = isVisible;
			}
		}
	}
	
	// process events
	ProcessEvents();
}

void CTeamSpeakClient::AddEvent(int clientID, int code)
{
	r3dCSHolder cs1(m_csEvents);
	r3dCSHolder csC(m_csClients);
	
	stdext::hash_map<int, int>::iterator it = m_ClientToPlayerIdx.find(clientID);
	int playerIdx = (it != m_ClientToPlayerIdx.end()) ? it->second : -1;
		
	evt_s evt;
	evt.code      = code;
	evt.playerIdx = playerIdx;
	m_events.push(evt);
}

void CTeamSpeakClient::ProcessEvents()
{
	r3dCSHolder cs1(m_csEvents);
	
	const ClientGameLogic& CGL = gClientLogic();
	while(!m_events.empty())
	{
		evt_s evt = m_events.front();
		m_events.pop();
		
		if(evt.code == EVT_ClientDisconnected)
		{
			OnClientDissapear(true, evt.playerIdx);
			continue;
		}

		if(!hudMain)
			continue;
		
		switch(evt.code)
		{
			case EVT_Connected:
			{
				hudMain->addChatMessage(0, "<voice>", "Connected to voice server", 0);
				break;
			}

			case EVT_FailedToConnect:
			{
				hudMain->addChatMessage(0, "<voice>", "Disconnected from voice server", 0);
				break;
			}
			
			case EVT_StartTalking:
			case EVT_StopTalking:
			{
				// ignore fucked up onTalkStatusChangeEvent's that was fired after onConnectStatusChangeEvent with disconnect
				if(evt.playerIdx == -1)
					break;

				// player talking icon
				obj_Player* plr = gClientLogic().GetPlayer(evt.playerIdx);
				if(plr)
				{
					float dist = (gCam - plr->GetPosition()).Length();
					plr->setVoiceActive((dist < VOICE_PROXIMITY_CHANNEL_DIST) && evt.code == EVT_StartTalking);
					hudMain->setPlayerInListVoipIconVisible(evt.playerIdx, plr->isVoiceActive(), CGL.playerNames[evt.playerIdx].voiceIsMuted);
				}
				else
				{
					hudMain->setPlayerInListVoipIconVisible(evt.playerIdx, false, CGL.playerNames[evt.playerIdx].voiceIsMuted);
				}
				
				// local player talking icon
				if(evt.playerIdx == CGL.localPlayerIdx_)
				{
					hudMain->showVOIPIconTalking(evt.code == EVT_StartTalking);
				}
				
				break;
			}
		}
	}
}

int CTeamSpeakClient::OnClientAppear(int clientID)
{
	r3dCSHolder csC(m_csClients);

	stdext::hash_map<int, int>::iterator it = m_ClientToPlayerIdx.find(clientID);
	if(it != m_ClientToPlayerIdx.end())
		return it->second;

	// Query client nickname from ID
	char* name;
	if((error = ts3client_getClientVariableAsString(scHandlerID, clientID, CLIENT_NICKNAME, &name)) != ERROR_ok)
	{
		TSHandleError("nickname in onTalkStatusChangeEvent");
		return -1;
	}
	// first word of server voiceId (stored in nickname) is playerIdx
	DWORD voiceId = 0;
	sscanf(name, "%u", &voiceId);
	int playerIdx = (voiceId & 0xFFFF);
	if(playerIdx >= R3D_ARRAYSIZE(gClientLogic().playerNames))
		r3dError("bad playerIdx: %d\n", playerIdx);
#ifndef FINAL_BUILD	
	r3dOutToLog("VoIp: Client %d %s mapped to playerIdx %d\n", clientID, name, playerIdx);
#endif	
	ts3client_freeMemory(name);
	
	// set voice client id to list of players
	gClientLogic().playerNames[playerIdx].voiceClientID = clientID;
	gClientLogic().playerNames[playerIdx].voiceIsMuted  = false;
	gClientLogic().playerNames[playerIdx].voiceEnabled  = true;
	
	// teleport it out of the way so if client was talking before updating his 3d position he won't be heard
	TS3_VECTOR pos;
	pos.x = gCam.x;
	pos.y = gCam.y + 10000;
	pos.z = gCam.z;
	if((error = ts3client_channelset3DAttributes(scHandlerID, clientID, &pos)) != ERROR_ok)
		TSHandleError("channelset3DAttributes");
		
	m_ClientToPlayerIdx.insert(std::pair<int, int>(clientID, playerIdx));
	return playerIdx;
}

void CTeamSpeakClient::OnClientDissapear(bool bMainThread, int clientID)
{
	if(!bMainThread)
	{
		// NOTE: we can't remove client in not main thread, teamspeak will deadlock in some functions that is called in Tick() for that client.
		// so we add a special event and remove client in main thread
		r3dCSHolder cs1(m_csEvents);

		CTeamSpeakClient::evt_s evt;
		evt.code      = CTeamSpeakClient::EVT_ClientDisconnected;
		evt.playerIdx = clientID;
		m_events.push(evt);
		return;
	}

	r3dCSHolder csC(m_csClients);

	stdext::hash_map<int, int>::iterator it = m_ClientToPlayerIdx.find(clientID);
	if(it != m_ClientToPlayerIdx.end())
	{
		// remove voice client id from list of players
		// (note that disconnect event is dissynched with game logic - so there might be already different client)
		int playerIdx = it->second;
		if(gClientLogic().playerNames[playerIdx].voiceClientID == clientID)
		{
			gClientLogic().playerNames[playerIdx].voiceClientID = 0;
			if(gClientLogic().GetPlayer(playerIdx)) // ptumik: this should be safe, as if other player already connected on the same playerIdx, then voiceClientID wouldn't match
				gClientLogic().GetPlayer(playerIdx)->setVoiceActive(false);
		}

#ifndef FINAL_BUILD	
		r3dOutToLog("VoIp: Client %d, plr: %d disconnected\n", clientID, it->second);
#endif		
		m_ClientToPlayerIdx.erase(it);
	}
#ifndef FINAL_BUILD	
	else
	{
		r3dOutToLog("!!! VOIP: Client %d disconnected and does not exist\n", clientID);
		r3d_assert(false && "send r3dLog.txt");
	}
#endif
}

void CTeamSpeakClient::SetPushToTalk(bool enable)
{
	m_usingPTT  = enable;
	m_pttActive = false;
	
	if(m_usingPTT)
	{
		if((error = ts3client_setPreProcessorConfigValue(scHandlerID, "vad", "false")) != ERROR_ok)
			TSHandleError("vad=false");

		ts3client_setClientSelfVariableAsInt(scHandlerID, CLIENT_INPUT_DEACTIVATED, INPUT_DEACTIVATED);
		ts3client_flushClientSelfUpdates(scHandlerID, NULL);
	}
	else
	{
		if((error = ts3client_setPreProcessorConfigValue(scHandlerID, "vad", "true")) != ERROR_ok)
			TSHandleError("vad=true");

		ts3client_setClientSelfVariableAsInt(scHandlerID, CLIENT_INPUT_DEACTIVATED, INPUT_ACTIVE);
		ts3client_flushClientSelfUpdates(scHandlerID, NULL);
	}
}

void CTeamSpeakClient::SetVolume(float volume)
{
	float db = 0.0f;
	if(volume >= 0.99f)
		db = 0;
	if(volume <= 0.01f)
		db = -120;
	else
		db = 10.0f * log(volume) / log(2.0f);	// loudness radio in dB is 10*log2(x)
	
	char strdb[128]; sprintf(strdb, "%.2f", db);
	ts3client_setPlaybackConfigValue(scHandlerID, "volume_modifier", strdb);

	//float value;
	//if((error = ts3client_getPlaybackConfigValueAsFloat(scHandlerID, "volume_modifier", &value)) != ERROR_ok)
	//	TSHandleError("getVolume");
	//r3dOutToLog("vol: %f, db:%f/%f\n", volume, value, db);
}

void CTeamSpeakClient::MuteClient(int clientID)
{
	anyID ids[2];
	ids[0] = (anyID)clientID;
	ids[1] = 0;
	if((error = ts3client_requestMuteClients(scHandlerID, ids, NULL)) != ERROR_ok)
		TSHandleError("requestUnmuteClient");
}

void CTeamSpeakClient::UnmuteClient(int clientID)
{
	anyID ids[2];
	ids[0] = (anyID)clientID;
	ids[1] = 0;
	if((error = ts3client_requestUnmuteClients(scHandlerID, ids, NULL)) != ERROR_ok)
		TSHandleError("requestUnmuteClient");
}


#endif // #ifdef ENABLE_TEAMSPEAK

#ifndef ENABLE_TEAMSPEAK
#include "TeamSpeakClient.h"
CTeamSpeakClient	gTeamSpeakClient;
CTeamSpeakClient::CTeamSpeakClient() {} 
CTeamSpeakClient::~CTeamSpeakClient() {} 
int CTeamSpeakClient::Startup() { return 1; }
int CTeamSpeakClient::Shutdown() { return 1; }
int CTeamSpeakClient::OpenDevices(int playbackIdx, int captureIdx) { return 1; }
void CTeamSpeakClient::StartConnect(const char* host, int port, DWORD voicePwd, DWORD voiceId) {}
void CTeamSpeakClient::Disconnect() {}
void CTeamSpeakClient::Tick() {}
#endif
