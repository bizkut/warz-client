/*
 * TeamSpeak 3 client minimal sample with custom 
 * capture and record
 *
 * Copyright (c) 2007-2010 TeamSpeak-Systems
 */

/* This example connects to a server and plays a wave file, while 
   recording incomming sound to output.wav */

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#endif
#include <stdio.h>

#include <public_definitions.h>
#include <public_errors.h>
#include <clientlib_publicdefinitions.h>
#include <clientlib.h>

#ifdef _WIN32
#define SLEEP(x) Sleep(x)
#else
#define SLEEP(x) usleep(x*1000)
#endif

#include "wave.h"

/*The client lib works at 48Khz internally. 
  It is therefore advisable to use the same for your project */
#define PLAYBACK_FREQUENCY 48000
#define PLAYBACK_CHANNELS 2

#define AUDIO_PROCESS_SECONDS 10

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
void onConnectStatusChangeEvent(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber) {
	printf("Connect status changed: %llu %d %u\n", (unsigned long long)serverConnectionHandlerID, newStatus, errorNumber);
	/* Failed to connect ? */
	if(newStatus == STATUS_DISCONNECTED && errorNumber == ERROR_failed_connection_initialisation) {
		printf("Looks like there is no server running, terminate!\n");
		getchar();
		exit(-1);
	}
}

/*
 * Callback for current channels being announced to the client after connecting to a server.
 *
 * Parameters:
 *   serverConnectionHandlerID - Server connection handler ID
 *   channelID                 - ID of the announced channel
 *   channelParentID           - ID of the parent channel
 */
void onNewChannelEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 channelParentID) {
    /* Query channel name from channel ID */
    char* name;
    unsigned int error;

    printf("onNewChannelEvent: %llu %llu %llu\n", (unsigned long long)serverConnectionHandlerID, (unsigned long long)channelID, (unsigned long long)channelParentID);
    if((error = ts3client_getChannelVariableAsString(serverConnectionHandlerID, channelID, CHANNEL_NAME, &name)) == ERROR_ok) {
        printf("New channel: %llu %s \n", (unsigned long long)channelID, name);
        ts3client_freeMemory(name);  /* Release dynamically allocated memory only if function succeeded */
    } else {
        char* errormsg;
        if(ts3client_getErrorMessage(error, &errormsg) == ERROR_ok) {
            printf("Error getting channel name in onNewChannelEvent: %s\n", errormsg);
            ts3client_freeMemory(errormsg);
        }
    }
}

/*
 * Callback for just created channels.
 *
 * Parameters:
 *   serverConnectionHandlerID - Server connection handler ID
 *   channelID                 - ID of the announced channel
 *   channelParentID           - ID of the parent channel
 *   invokerID                 - ID of the client who created the channel
 *   invokerName               - Name of the client who created the channel
 */
void onNewChannelCreatedEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 channelParentID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier) {
    char* name;

    /* Query channel name from channel ID */
    if(ts3client_getChannelVariableAsString(serverConnectionHandlerID, channelID, CHANNEL_NAME, &name) != ERROR_ok)
        return;
    printf("New channel created: %s\n", name);
    ts3client_freeMemory(name);  /* Release dynamically allocated memory only if function succeeded */
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
void onDelChannelEvent(uint64 serverConnectionHandlerID, uint64 channelID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier) {
	printf("Channel ID %llu deleted by %s (%u)\n", (unsigned long long)channelID, invokerName, invokerID);
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
void onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* moveMessage) {
	printf("ClientID %u moves from channel %llu to %llu with message %s\n", clientID, (unsigned long long)oldChannelID, (unsigned long long)newChannelID, moveMessage);
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
void onClientMoveSubscriptionEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility) {
    char* name;

    /* Query client nickname from ID */
    if(ts3client_getClientVariableAsString(serverConnectionHandlerID, clientID, CLIENT_NICKNAME, &name) != ERROR_ok)
        return;
    printf("New client: %s\n", name);
    ts3client_freeMemory(name);  /* Release dynamically allocated memory only if function succeeded */
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
void onClientMoveTimeoutEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* timeoutMessage) {
	printf("ClientID %u timeouts with message %s\n",clientID, timeoutMessage);
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
void onTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID) {
    char* name;

    /* Query client nickname from ID */
    if(ts3client_getClientVariableAsString(serverConnectionHandlerID, clientID, CLIENT_NICKNAME, &name) != ERROR_ok)
        return;
    if(status == STATUS_TALKING) {
        printf("Client \"%s\" starts talking.\n", name);
    } else {
        printf("Client \"%s\" stops talking.\n", name);
    }
    ts3client_freeMemory(name);  /* Release dynamically allocated memory only if function succeeded */
}

void onServerErrorEvent(uint64 serverConnectionHandlerID, const char* errorMessage, unsigned int error, const char* returnCode, const char* extraMessage) {
	printf("Error for server %llu: %s %s\n", (unsigned long long)serverConnectionHandlerID, errorMessage, extraMessage);
}

int main() {
	uint64 scHandlerID;
	unsigned int error;
	char *version;
	char *identity;

	int    captureFrequency;
	int    captureChannels;
	short* captureBuffer;
	int    captureBufferSamples;

	int    audioPeriodCounter;
	int    captureAudioOffset;
	int    capturePeriodSize;

	short* playbackBuffer;
	int    playbackAudioOffset;
	int    playbackPeriodSize;

	/* Create struct for callback function pointers */
	struct ClientUIFunctions funcs;

	/* Initialize all callbacks with NULL */
	memset(&funcs, 0, sizeof(struct ClientUIFunctions));

	/* Now assign the used callback function pointers */
	funcs.onConnectStatusChangeEvent    = onConnectStatusChangeEvent;
	funcs.onNewChannelEvent             = onNewChannelEvent;
	funcs.onNewChannelCreatedEvent      = onNewChannelCreatedEvent;
	funcs.onDelChannelEvent             = onDelChannelEvent;
	funcs.onClientMoveEvent             = onClientMoveEvent;
	funcs.onClientMoveSubscriptionEvent = onClientMoveSubscriptionEvent;
	funcs.onClientMoveTimeoutEvent      = onClientMoveTimeoutEvent;
	funcs.onTalkStatusChangeEvent       = onTalkStatusChangeEvent;
	funcs.onServerErrorEvent            = onServerErrorEvent;

	/* Read in the wave we are going to stream to the server */
	if (!readWave("welcome_to_teamspeak.wav", &captureFrequency, &captureChannels, &captureBuffer, &captureBufferSamples)) 
		return 1;
	
	/* allocate AUDIO_PROCESS_SECONDS seconds worth of PLAYBACK_FREQUENCY 16bit PLAYBACK_CHANNELS channels */
	playbackBuffer = (short*) malloc(AUDIO_PROCESS_SECONDS * PLAYBACK_FREQUENCY * sizeof(short) * PLAYBACK_CHANNELS);
	if (!playbackBuffer){
		printf("error: could not allocate memory for output wave\n");
		return 1;
	}

	/* Initialize client lib with callbacks */
	if((error = ts3client_initClientLib(&funcs, NULL, LogType_FILE | LogType_CONSOLE, NULL, "")) != ERROR_ok) {
		char* errormsg;
		if(ts3client_getErrorMessage(error, &errormsg) == ERROR_ok) {
			printf("Error initialzing serverlib: %s\n", errormsg);
			ts3client_freeMemory(errormsg);
		}
		return 1;
	}

	/* register a new custom sound device, that captures at read wave freq+channels and plays PLAYBACK_CHANNELS channels at PLAYBACK_FREQUENCY */
	if ((error = ts3client_registerCustomDevice("customWaveDeviceId", "Nice displayable wave device name", captureFrequency, captureChannels, PLAYBACK_FREQUENCY, PLAYBACK_CHANNELS)) != ERROR_ok) {
		char* errormsg;
		if(ts3client_getErrorMessage(error, &errormsg) == ERROR_ok) {
			printf("Error registering custom sound device: %s\n", errormsg);
			ts3client_freeMemory(errormsg);
		}
	}

    /* Spawn a new server connection handler using the default port and store the server ID */
    if((error = ts3client_spawnNewServerConnectionHandler(0, &scHandlerID)) != ERROR_ok) {
        printf("Error spawning server connection handler: %d\n", error);
        return 1;
    }

    /* Open capture device we created earlier */
    if((error = ts3client_openCaptureDevice(scHandlerID, "custom", "customWaveDeviceId")) != ERROR_ok) {
        printf("Error opening capture device: %d\n", error);
    }

    /* Open playback device we created earlier */
    if((error = ts3client_openPlaybackDevice(scHandlerID, "custom", "customWaveDeviceId")) != ERROR_ok) {
        printf("Error opening playback device: %d\n", error);
    }

    /* Create a new client identity */
    /* In your real application you should do this only once, store the assigned identity locally and then reuse it. */
    if((error = ts3client_createIdentity(&identity)) != ERROR_ok) {
        printf("Error creating identity: %d\n", error);
        return 1;
    }

    /* Connect to server on localhost:9987 with nickname "client", no default channel, no default channel password and server password "secret" */
    if((error = ts3client_startConnection(scHandlerID, identity, "localhost", 9987, "client", NULL, "", "secret")) != ERROR_ok) {
        printf("Error connecting to server: %d\n", error);
        return 1;
    }

	ts3client_freeMemory(identity);  /* Release dynamically allocated memory */
	identity = NULL;

	printf("Client lib initialized and running\n");

    /* Query and print client lib version */
    if((error = ts3client_getClientLibVersion(&version)) != ERROR_ok) {
        printf("Failed to get clientlib version: %d\n", error);
        return 1;
    }
    printf("Client lib version: %s\n", version);
    ts3client_freeMemory(version);  /* Release dynamically allocated memory */
    version = NULL;

    SLEEP(500);

    printf("\n--- processing audio for %d seconds ---\n", AUDIO_PROCESS_SECONDS);
	
	/*the clientlib works with 20ms packets internaly. 
	  So the best is to feed is 20ms worth of sound at a time */
	capturePeriodSize = (captureFrequency*20)/1000; 
	playbackPeriodSize = (PLAYBACK_FREQUENCY*20)/1000;

	captureAudioOffset = 0;
	playbackAudioOffset = 0;
	for(audioPeriodCounter = 0; audioPeriodCounter < 50*AUDIO_PROCESS_SECONDS; ++audioPeriodCounter){ /*50*20=1000*/
		/* wait 20 ms */
		SLEEP(20);

		/* make sure we dont stream past the end of our wave sample */
		if (captureAudioOffset + capturePeriodSize > captureBufferSamples)
			captureAudioOffset = 0;

		/* stream capture data to the client lib */
		if((error = ts3client_processCustomCaptureData("customWaveDeviceId", captureBuffer + captureAudioOffset*captureChannels, capturePeriodSize)) != ERROR_ok){
			printf("Failed to get stream capture data: %d\n", error);
			return 1;
		}

		/* get playback data from the client lib */
		if((error = ts3client_acquireCustomPlaybackData("customWaveDeviceId", playbackBuffer + playbackAudioOffset*PLAYBACK_CHANNELS, playbackPeriodSize))!= ERROR_ok){
			if(error != ERROR_sound_no_data) { //this error signals us to play silence
				printf("Failed to get acquire playback data: %d\n", error);
				return 1;
			}
			memset(playbackBuffer + playbackAudioOffset * PLAYBACK_CHANNELS, 0, playbackPeriodSize*2);
		}

		/*update buffer offsets */
		captureAudioOffset += capturePeriodSize;
		playbackAudioOffset += playbackPeriodSize; 
	}

    /* Disconnect from server */
    if((error = ts3client_stopConnection(scHandlerID, "leaving")) != ERROR_ok) {
        printf("Error stopping connection: %d\n", error);
        return 1;
    }

    /* Destroy server connection handler */
    if((error = ts3client_destroyServerConnectionHandler(scHandlerID)) != ERROR_ok) {
        printf("Error destroying ServerConnectionHandler: %d\n", error);
        return 1;
    }

	/* unregister the custom sound device */
	if ((error = ts3client_unregisterCustomDevice("customWaveDeviceId")) != ERROR_ok){
        printf("Error unregisterring custom sound device: %d\n", error);
        return 1;
	}


    /* Shutdown client lib */
    if((error = ts3client_destroyClientLib()) != ERROR_ok) {
        printf("Failed to destroy clientlib: %d\n", error);
        return 1;
    }

	/* save the playback data */
	writeWave("output.wav", PLAYBACK_FREQUENCY, PLAYBACK_CHANNELS, playbackBuffer, PLAYBACK_FREQUENCY*AUDIO_PROCESS_SECONDS);

	/* release allocated memory */
	free(captureBuffer);
	free(playbackBuffer);

	return 0;
}
