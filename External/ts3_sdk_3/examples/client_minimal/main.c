/*
 * TeamSpeak 3 client minimal sample
 *
 * Copyright (c) 2007-2010 TeamSpeak-Systems
 */

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

	/* Create struct for callback function pointers */
	struct ClientUIFunctions funcs;

	/* Initialize all callbacks with NULL */
	memset(&funcs, 0, sizeof(struct ClientUIFunctions));

	/* Callback function pointers */
	/* It is sufficient to only assign those callback functions you are using. When adding more callbacks, add those function pointers here. */
	funcs.onConnectStatusChangeEvent    = onConnectStatusChangeEvent;
	funcs.onNewChannelEvent             = onNewChannelEvent;
	funcs.onNewChannelCreatedEvent      = onNewChannelCreatedEvent;
	funcs.onDelChannelEvent             = onDelChannelEvent;
	funcs.onClientMoveEvent             = onClientMoveEvent;
	funcs.onClientMoveSubscriptionEvent = onClientMoveSubscriptionEvent;
	funcs.onClientMoveTimeoutEvent      = onClientMoveTimeoutEvent;
	funcs.onTalkStatusChangeEvent       = onTalkStatusChangeEvent;
	funcs.onServerErrorEvent            = onServerErrorEvent;

	/* Initialize client lib with callbacks */
	/* Resource path points to the SDK\bin directory to locate the soundbackends folder when running from Visual Studio. */
	/* If you want to run directly from the SDK\bin directory, use an empty string instead to locate the soundbackends folder in the current directory. */
	if((error = ts3client_initClientLib(&funcs, NULL, LogType_FILE | LogType_CONSOLE | LogType_USERLOGGING, NULL, "../../bin/")) != ERROR_ok) {
		char* errormsg;
		if(ts3client_getErrorMessage(error, &errormsg) == ERROR_ok) {
			printf("Error initialzing serverlib: %s\n", errormsg);
			ts3client_freeMemory(errormsg);
		}
		return 1;
	}

    /* Spawn a new server connection handler using the default port and store the server ID */
    if((error = ts3client_spawnNewServerConnectionHandler(0, &scHandlerID)) != ERROR_ok) {
        printf("Error spawning server connection handler: %d\n", error);
        return 1;
    }

    /* Open default capture device */
	/* Passing empty string for mode and NULL or empty string for device will open the default device */
    if((error = ts3client_openCaptureDevice(scHandlerID, "", NULL)) != ERROR_ok) {
        printf("Error opening capture device: %d\n", error);
    }

    /* Open default playback device */
    /* Passing empty string for mode and NULL or empty string for device will open the default device */
	if((error = ts3client_openPlaybackDevice(scHandlerID, "", NULL)) != ERROR_ok) {
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

    /* Wait for user input */
    printf("\n--- Press Return to disconnect from server and exit ---\n");
    getchar();

    /* Disconnect from server */
    if((error = ts3client_stopConnection(scHandlerID, "leaving")) != ERROR_ok) {
        printf("Error stopping connection: %d\n", error);
        return 1;
    }

    SLEEP(200);

    /* Destroy server connection handler */
    if((error = ts3client_destroyServerConnectionHandler(scHandlerID)) != ERROR_ok) {
        printf("Error destroying clientlib: %d\n", error);
        return 1;
    }

    /* Shutdown client lib */
    if((error = ts3client_destroyClientLib()) != ERROR_ok) {
        printf("Failed to destroy clientlib: %d\n", error);
        return 1;
    }

	return 0;
}
