/*
 * This is the main header for the TeamSpeak 3 Server SDK. All the functions that are used
 * to communicate with the TeamSpeak 3 Server are here. Please view the documentation for
 * details. Note that some of these functions REQUIRE you to call ts3server_freeMemory()
 * after finishing with the result. This holds true for all the functions returning
 * strings (so the parameter is "char** result"). Again, the details are all in the
 * documentation.
 */

#ifndef SERVERLIB_H
#define SERVERLIB_H

#include <public_definitions.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ServerLibFunctions {
	void (*onVoiceDataEvent)           (uint64 serverID, anyID clientID, unsigned char* voiceData, unsigned int voiceDataSize, unsigned int frequency);
	void (*onClientStartTalkingEvent)  (uint64 serverID, anyID clientID);
	void (*onClientStopTalkingEvent)   (uint64 serverID, anyID clientID);
	void (*onClientConnected)          (uint64 serverID, anyID clientID, uint64 channelID, unsigned int* removeClientError);
	void (*onClientDisconnected)       (uint64 serverID, anyID clientID, uint64 channelID);
	void (*onClientMoved)              (uint64 serverID, anyID clientID, uint64 oldChannelID, uint64 newChannelID);
	void (*onChannelCreated)           (uint64 serverID, anyID invokerClientID, uint64 channelID);
	void (*onChannelEdited)            (uint64 serverID, anyID invokerClientID, uint64 channelID);
	void (*onChannelDeleted)           (uint64 serverID, anyID invokerClientID, uint64 channelID);
	void (*onServerTextMessageEvent)   (uint64 serverID, anyID invokerClientID, const char* textMessage);
	void (*onChannelTextMessageEvent)  (uint64 serverID, anyID invokerClientID, uint64 targetChannelID, const char* textMessage);
	void (*onUserLoggingMessageEvent)  (const char* logmessage, int logLevel, const char* logChannel, uint64 logID, const char* logTime, const char* completeLogString);
	void (*onAccountingErrorEvent)     (uint64 serverID, unsigned int errorCode);
	void (*onCustomPacketEncryptEvent) (char** dataToSend, unsigned int* sizeOfData);
	void (*onCustomPacketDecryptEvent) (char** dataReceived, unsigned int* dataReceivedSize);
}; //END OF ServerLibFunctions

/*Memory management*/
EXPORTDLL unsigned int ts3server_freeMemory(void* pointer);

/*Construction and Destruction*/
EXPORTDLL unsigned int ts3server_initServerLib(const struct ServerLibFunctions* functionPointers, int usedLogTypes, const char* logFileFolder);
EXPORTDLL unsigned int ts3server_destroyServerLib();
EXPORTDLL unsigned int ts3server_getServerLibVersion(char** result);
EXPORTDLL unsigned int ts3server_getServerLibVersionNumber(uint64* result);

/*logging*/
EXPORTDLL unsigned int ts3server_setLogVerbosity(enum LogLevel logVerbosity);

/*error handling*/
EXPORTDLL unsigned int ts3server_getGlobalErrorMessage(unsigned int globalErrorCode, char** result);

/*client info*/
EXPORTDLL unsigned int ts3server_getClientVariableAsInt(uint64 serverID, anyID clientID, enum ClientProperties flag, int* result);
EXPORTDLL unsigned int ts3server_getClientVariableAsString(uint64 serverID, anyID clientID, enum ClientProperties flag, char** result);
EXPORTDLL unsigned int ts3server_setClientVariableAsInt(uint64 serverID, anyID clientID, enum ClientProperties flag, int value);
EXPORTDLL unsigned int ts3server_setClientVariableAsString(uint64 serverID, anyID clientID, enum ClientProperties flag, const char* value);
EXPORTDLL unsigned int ts3server_flushClientVariable(uint64 serverID, anyID clientID);

EXPORTDLL unsigned int ts3server_setClientWhisperList(uint64 serverID, anyID clID, const uint64* channelID, const anyID* clientID);

EXPORTDLL unsigned int ts3server_getClientList(uint64 serverID, anyID** result);
EXPORTDLL unsigned int ts3server_getChannelOfClient(uint64 serverID, anyID clientID, uint64* result);
EXPORTDLL unsigned int ts3server_clientMove(uint64 serverID, uint64 newChannelID, const anyID* clientIDArray); 

/*channel info*/
EXPORTDLL unsigned int ts3server_getChannelVariableAsInt(uint64 serverID, uint64 channelID, enum ChannelProperties flag, int* result);
EXPORTDLL unsigned int ts3server_getChannelVariableAsString(uint64 serverID, uint64 channelID, enum ChannelProperties flag, char** result);
EXPORTDLL unsigned int ts3server_setChannelVariableAsInt(uint64 serverID, uint64 channelID, enum ChannelProperties flag, int value);
EXPORTDLL unsigned int ts3server_setChannelVariableAsString(uint64 serverID, uint64 channelID, enum ChannelProperties flag, const char* value);
EXPORTDLL unsigned int ts3server_flushChannelVariable(uint64 serverID, uint64 channelID);
EXPORTDLL unsigned int ts3server_flushChannelCreation(uint64 serverID, uint64 channelParentID, uint64* result);

EXPORTDLL unsigned int ts3server_getChannelList(uint64 serverID, uint64** result);
EXPORTDLL unsigned int ts3server_getChannelClientList(uint64 serverID, uint64 channelID, anyID** result);
EXPORTDLL unsigned int ts3server_getParentChannelOfChannel(uint64 serverID, uint64 channelID, uint64* result);

EXPORTDLL unsigned int ts3server_channelDelete(uint64 serverID, uint64 channelID, int force);
EXPORTDLL unsigned int ts3server_channelMove(uint64 serverID, uint64 channelID, uint64 newChannelParentID, uint64 newOrder);

/*server info*/
EXPORTDLL unsigned int ts3server_getVirtualServerVariableAsInt(uint64 serverID, enum VirtualServerProperties flag, int* result);
EXPORTDLL unsigned int ts3server_getVirtualServerVariableAsString(uint64 serverID, enum VirtualServerProperties flag, char** result);
EXPORTDLL unsigned int ts3server_setVirtualServerVariableAsInt(uint64 serverID, enum VirtualServerProperties flag, int value);
EXPORTDLL unsigned int ts3server_setVirtualServerVariableAsString(uint64 serverID, enum VirtualServerProperties flag, const char* value);
EXPORTDLL unsigned int ts3server_flushVirtualServerVariable(uint64 serverID);

EXPORTDLL unsigned int ts3server_getVirtualServerConnectionVariableAsUInt64(uint64 serverID, enum ConnectionProperties flag, uint64* result);
EXPORTDLL unsigned int ts3server_getVirtualServerConnectionVariableAsDouble(uint64 serverID, enum ConnectionProperties flag, double* result);

EXPORTDLL unsigned int ts3server_getVirtualServerList(uint64** result);
EXPORTDLL unsigned int ts3server_stopVirtualServer(uint64 serverID);
EXPORTDLL unsigned int ts3server_createVirtualServer(unsigned int serverPort, const char* serverIp, const char* serverName, const char* serverKeyPair, unsigned int serverMaxClients, uint64* result);
EXPORTDLL unsigned int ts3server_getVirtualServerKeyPair(uint64 serverID, char** result);

#ifdef __cplusplus
}
#endif

#endif
