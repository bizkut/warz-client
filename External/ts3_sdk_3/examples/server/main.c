/*
 * TeamSpeak 3 server sample
 *
 * Copyright (c) 2007-2010 TeamSpeak-Systems
 */

#ifdef WINDOWS
#include <Windows.h>
#else
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#endif
#include <stdio.h>

#include <public_definitions.h>
#include <public_errors.h>
#include <serverlib_publicdefinitions.h>
#include <serverlib.h>
#include "id_io.h"

#define DEFAULT_VIRTUAL_SERVER_ID 1

/* Maximum number of clients allowed per virtual server */
#define MAX_CLIENTS 8

#ifdef WINDOWS
#define SLEEP(x) Sleep(x)
#else
#define SLEEP(x) usleep(x*1000)
#endif

/* Enable to use server-side voice recording */
/* #define USE_VOICEDATAEVENT */

/* Enable to use custom encryption */
/* #define USE_CUSTOM_ENCRYPTION
#define CUSTOM_CRYPT_KEY 123 */

#ifdef USE_VOICEDATAEVENT
#ifdef WINDOWS
#include <io.h>
#else  /* Unix compatibility */
#include <unistd.h>
#define _open open
#define _write write
#define _close close
#define _O_CREAT O_CREAT
#define _O_WRONLY O_WRONLY
#define _S_IREAD S_IREAD
#define _S_IWRITE S_IWRITE
#define _O_APPEND O_APPEND
#define _O_BINARY 0
#endif
#include <fcntl.h>
#include <sys/stat.h>
#endif

#define CHECK_ERROR(x) if((error = x) != ERROR_ok) { goto on_error; }

/*
 * Callback when client has connected.
 *
 * Parameter:
 *   serverID  - Virtual server ID
 *   clientID  - ID of connected client
 *   channelID - ID of channel the client joined
 */
void onClientConnected(uint64 serverID, anyID clientID, uint64 channelID, unsigned int* removeClientError) {
    char* clientName;
    unsigned int error;

    /* Query client nickname */
    if((error = ts3server_getClientVariableAsString(serverID, clientID, CLIENT_NICKNAME, &clientName)) != ERROR_ok) {
        char* errormsg;
        if(ts3server_getGlobalErrorMessage(error, &errormsg) == ERROR_ok) {
            printf("Error querying client nickname: %s\n", errormsg);
            ts3server_freeMemory(errormsg);
        }
        return;
    }

    printf("Client '%s' joined channel %llu on virtual server %llu\n", clientName, (unsigned long long) channelID, (unsigned long long)serverID);

    /* Example: Kick clients with nickname "BlockMe from server */
    if(!strcmp(clientName, "BlockMe")) {
        printf("Blocking bad client!\n");
        *removeClientError = ERROR_client_not_logged_in;  /* Give a reason */
    }
}

/*
 * Callback when client has disconnected.
 *
 * Parameter:
 *   serverID  - Virtual server ID
 *   clientID  - ID of disconnected client
 *   channelID - ID of channel the client left
 */
void onClientDisconnected(uint64 serverID, anyID clientID, uint64 channelID) {
	printf("Client %u left channel %llu on virtual server %llu\n", clientID, (unsigned long long)channelID, (unsigned long long)serverID);
}

/*
 * Callback when client has moved.
 *
 * Parameter:
 *   serverID     - Virtual server ID
 *   clientID     - ID of moved client
 *   oldChannelID - ID of old channel the client left
 *   newChannelID - ID of new channel the client joined
 */
void onClientMoved(uint64 serverID, anyID clientID, uint64 oldChannelID, uint64 newChannelID) {
	printf("Client %u moved from channel %llu to channel %llu on virtual server %llu\n", clientID, (unsigned long long)oldChannelID, (unsigned long long)newChannelID, (unsigned long long)serverID);
}

/*
 * Callback when channel has been created.
 *
 * Parameter:
 *   serverID        - Virtual server ID
 *   invokerClientID - ID of the client who created the channel
 *   channelID       - ID of the created channel
 */
void onChannelCreated(uint64 serverID, anyID invokerClientID, uint64 channelID) {
	printf("Channel %llu created by %u on virtual server %llu\n", (unsigned long long)channelID, invokerClientID, (unsigned long long)serverID);
}

/*
 * Callback when channel has been edited.
 *
 * Parameter:
 *   serverID        - Virtual server ID
 *   invokerClientID - ID of the client who edited the channel
 *   channelID       - ID of the edited channel
 */
void onChannelEdited(uint64 serverID, anyID invokerClientID, uint64 channelID) {
	printf("Channel %llu edited by %u on virtual server %llu\n", (unsigned long long)channelID, invokerClientID, (unsigned long long)serverID);
}

/*
 * Callback when channel has been deleted.
 *
 * Parameter:
 *   serverID        - Virtual server ID
 *   invokerClientID - ID of the client who deleted the channel
 *   channelID       - ID of the deleted channel
 */
void onChannelDeleted(uint64 serverID, anyID invokerClientID, uint64 channelID) {
	printf("Channel %llu deleted by %u on virtual server %llu\n", (unsigned long long)channelID, invokerClientID, (unsigned long long)serverID);
}

/*
 * Callback when a server text message has been received.
 * Note that only server and channel chats are received, private client messages are not caught due to privacy reasons.
 *
 * Parameter:
 *   serverID        - Virtual server ID
 *   invokerClientID - ID of the client who sent the text message
 *   textMessage     - Message text
 */
void onServerTextMessageEvent(uint64 serverID, anyID invokerClientID, const char* textMessage) {
	char* invokerNickname;
	unsigned int error;

	/* Get invoker nickname */
	if((error = ts3server_getClientVariableAsString(serverID, invokerClientID, CLIENT_NICKNAME, &invokerNickname)) != ERROR_ok) {
		printf("Error getting client nickname: %d\n", error);
		return;
	}

	printf("Text message in server chat by %s: %s\n", invokerNickname, textMessage);

	ts3server_freeMemory(invokerNickname);  /* Release previously allocated memory */
}

/*
 * Callback when a channel text message has been received.
 * Note that only server and channel chats are received, private client messages are not caught due to privacy reasons.
 *
 * Parameter:
 *   serverID        - Virtual server ID
 *   invokerClientID - ID of the client who sent the text message
 *   targetChannelID - ID of the channel in which the chat was sent
 *   textMessage     - Message text
 */
void onChannelTextMessageEvent(uint64 serverID, anyID invokerClientID, uint64 targetChannelID, const char* textMessage) {
	char* invokerNickname;
	char* channelName;
	unsigned int error;

	/* Get invoker nickname */
	if((error = ts3server_getClientVariableAsString(serverID, invokerClientID, CLIENT_NICKNAME, &invokerNickname)) != ERROR_ok) {
		printf("Error getting client nickname: %d\n", error);
		return;
	}

	/* Get channel name */
	if((error = ts3server_getChannelVariableAsString(serverID, targetChannelID, CHANNEL_NAME, &channelName)) != ERROR_ok) {
		printf("Error getting channel name: %d\n", error);
		ts3server_freeMemory(invokerNickname);
		return;
	}

	printf("Text message in channel '%s' by %s: %s\n", channelName, invokerNickname, textMessage);

	ts3server_freeMemory(invokerNickname);
	ts3server_freeMemory(channelName);
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
void onUserLoggingMessageEvent(const char* logMessage, int logLevel, const char* logChannel, uint64 logID, const char* logTime, const char* completeLogString) {
	/* Your custom error display here... */
	/* printf("LOG: %s\n", completeLogString); */
	if(logLevel == LogLevel_CRITICAL) {
		exit(1);  /* Your custom handling of critical errors */
	}
}

#ifdef USE_VOICEDATAEVENT
/*
 * Callback triggered by the specified client sending voice data.
 *
 * Parameters:
 *   serverID      - ID of the server sending the callback
 *   clientID      - ID of the client sending the voice data
 *   voiceData     - Voice data buffer. Format is 16 bit mono. Do not free this buffer.
 *   voiceDataSize - Size of the voiceData buffer
 *   frequency     - Voice frequency
 */
void onVoiceDataEvent(uint64 serverID, anyID clientID, unsigned char* voiceData, unsigned int voiceDataSize, unsigned int frequency) {
	int fd;
	unsigned int error;
    char* name;

	/* Query client nickname as string */
    if((error = ts3server_getClientVariableAsString(serverID, clientID, CLIENT_NICKNAME, &name)) != ERROR_ok) {
        char* errormsg;
        if(ts3server_getGlobalErrorMessage(error, &errormsg) == ERROR_ok) {
            printf("Error querying client nickname: %s\n", errormsg);
            ts3server_freeMemory(errormsg);
        }
        return;
    }

	/* Open file with client nickname as filename and write voice data */
	fd =_open(name, _O_CREAT | _O_APPEND | _O_BINARY | _O_WRONLY , _S_IREAD | _S_IWRITE);
	if(fd == -1) {
		printf("failed to open file\n");
		ts3server_freeMemory(name);
		exit(-1);
	}
	_write(fd, voiceData, voiceDataSize);
	_close(fd);

	/* Release string dynamically allocated in getClientVariableAsString */
	ts3server_freeMemory(name);
}
#endif

/*
 * Callback triggered when the specified client starts talking.
 *
 * Parameters:
 *   serverID - ID of the server sending the callback
 *   clientID - ID of the client which started talking
 */
void onClientStartTalkingEvent(uint64 serverID, anyID clientID) {
	printf("onClientStartTalkingEvent serverID=%llu, clientID=%u\n", (unsigned long long)serverID, clientID);
}

/*
 * Callback triggered when the specified client stops talking.
 *
 * Parameters:
 *   serverID - ID of the server sending the callback
 *   clientID - ID of the client which stopped talking
 */
void onClientStopTalkingEvent(uint64 serverID, anyID clientID) {
	printf("onClientStopTalkingEvent serverID=%llu, clientID=%u\n", (unsigned long long)serverID, clientID);
}

/*
 * Callback triggered when a license error occurs.
 *
 * Parameters:
 *   serverID  - ID of the virtual server on which the license error occured. This virtual server will be automatically
 *               shutdown. If the parameter is zero, all virtual servers are affected and have been shutdown.
 *   errorCode - Code of the occured error. Use ts3server_getGlobalErrorMessage() to convert to a message string
 */
void onAccountingErrorEvent(uint64 serverID, unsigned int errorCode) {
	char* errorMessage;
	if(ts3server_getGlobalErrorMessage(errorCode, &errorMessage) == ERROR_ok) {
		printf("onAccountingErrorEvent serverID=%llu, errorCode=%u: %s\n", (unsigned long long)serverID, errorCode, errorMessage);
		ts3server_freeMemory(errorMessage);
	}

	/* Your custom handling here. In a real application, you wouldn't stop the whole process because the TS3 server part went down.
	 * The whole idea of this callback is to let you gracefully handle the TS3 server failing to start and to continue your application. */
	exit(1);
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
void onCustomPacketEncryptEvent(char** dataToSend, unsigned int* sizeOfData) {
#ifdef USE_CUSTOM_ENCRYPTION
	unsigned int i;
	for(i = 0; i < *sizeOfData; i++) {
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
void onCustomPacketDecryptEvent(char** dataReceived, unsigned int* dataReceivedSize) {
#ifdef USE_CUSTOM_ENCRYPTION
	unsigned int i;
	for(i = 0; i < *dataReceivedSize; i++) {
		(*dataReceived)[i] ^= CUSTOM_CRYPT_KEY;
	}
#endif
}

void showHelp() {
	printf("\n[q] - Quit\n[h] - Show this help\n[v] - List virtual servers\n[c] - Show channels of virtual server %d\n", DEFAULT_VIRTUAL_SERVER_ID);
	printf("[l] - Show clients of virtual server %d\n[n] - Create new channel on virtual server %d with generated name\n[N] - Create new channel on virtual server %d with custom name\n", DEFAULT_VIRTUAL_SERVER_ID, DEFAULT_VIRTUAL_SERVER_ID, DEFAULT_VIRTUAL_SERVER_ID);
	printf("[d] - Delete channel on virtual server %d\n[r] - Rename channel on virtual server %d\n[m] - Move client on virtual server %d\n", DEFAULT_VIRTUAL_SERVER_ID, DEFAULT_VIRTUAL_SERVER_ID, DEFAULT_VIRTUAL_SERVER_ID);
	printf("[C] - Create new virtual server\n[E] - Edit virtual server\n[S] - Stop virtual server\n\n");
}

void emptyInputBuffer() {
    int c;
    while((c = getchar()) != '\n' && c != EOF);
}

void showVirtualServers() {
	uint64* ids;
	int i;
	unsigned int error;

	printf("\nList of virtual servers:\n");
    if((error = ts3server_getVirtualServerList(&ids)) != ERROR_ok) {  /* Get array of virtual server IDs */
        printf("Error getting virtual server list: %d\n", error);
        return;
    }
	if(!ids[0]) {
		printf("No virtual servers\n\n");
		ts3server_freeMemory(ids);
		return;
	}
	for(i=0; ids[i]; i++) {
		char* name;
		int slotCount;
		char* virtualServerUniqueIdentifier;
        if((error = ts3server_getVirtualServerVariableAsString(ids[i], VIRTUALSERVER_NAME, &name)) != ERROR_ok) {  /* Query server name */
            printf("Error getting virtual server nickname: %d\n", error);
            break;
        }
		if((error = ts3server_getVirtualServerVariableAsInt(ids[i], VIRTUALSERVER_MAXCLIENTS, &slotCount)) != ERROR_ok) {
			printf("Error getting virtual server slot count: %d\n", error);
			break;
		}
		if((error = ts3server_getVirtualServerVariableAsString(ids[i], VIRTUALSERVER_UNIQUE_IDENTIFIER, &virtualServerUniqueIdentifier)) != ERROR_ok) {
			printf("Error getting virtual server unique identifier: %d\n", error);
			break;
		}
		printf("ID=%llu  NAME=\"%s\"  CAPACITY=%d  Unique Identifier=\"%s\"\n", (unsigned long long)ids[i], name, slotCount, virtualServerUniqueIdentifier);
		ts3server_freeMemory(name);  /* Do not free memory if above function returned an error */
		ts3server_freeMemory(virtualServerUniqueIdentifier);
	}
	printf("\n");

	ts3server_freeMemory(ids);  /* Release array */
}

void showChannels(uint64 serverID) {
	uint64* ids;
	int i;
	unsigned int error;

	printf("\nList of channels on virtual server %llu:\n", (unsigned long long)serverID);
    if((error = ts3server_getChannelList(serverID, &ids)) != ERROR_ok) {  /* Get array of channel IDs */
        printf("Error getting channel list: %d\n", error);
        return;
    }
	if(!ids[0]) {
		printf("No channels\n\n");
		ts3server_freeMemory(ids);
		return;
	}
	for(i=0; ids[i]; i++) {
		char* name;
        if((error = ts3server_getChannelVariableAsString(serverID, ids[i], CHANNEL_NAME, &name)) != ERROR_ok) {  /* Query channel name */
            printf("Error querying channel name: %d\n", error);
            break;
        }
		printf("%llu - %s\n", (unsigned long long)ids[i], name);
		ts3server_freeMemory(name);  /* Do not free memory if above function returned an error */
	}
	printf("\n");

	ts3server_freeMemory(ids);  /* Release array */
}

void showClients(uint64 serverID) {
	anyID* ids;
	int i;
	unsigned int error;

	printf("\nList of clients on virtual server %llu:\n", (unsigned long long)serverID);
    if((error = ts3server_getClientList(serverID, &ids)) != ERROR_ok) {  /* Get array of client IDs */
        printf("Error getting client list: %d\n", error);
        return;
    }
	if(!ids[0]) {
		printf("No clients\n\n");
		ts3server_freeMemory(ids);
		return;
	}
	for(i=0; ids[i]; i++) {
		char* name;
        if((error = ts3server_getClientVariableAsString(serverID, ids[i], CLIENT_NICKNAME, &name)) != ERROR_ok) {  /* Query client nickname */
            printf("Error querying client nickname: %d\n", error);
            break;
        }
		printf("%u - %s\n", ids[i], name);
		ts3server_freeMemory(name);  /* Do not free memory if above function returned an error */
	}
	printf("\n");

	ts3server_freeMemory(ids);  /* Release array */
}

void createDefaultChannelName(char *name) {
	static int i = 1;
	sprintf(name, "Channel_%d", i++);
}

void enterName(char *name) {
	char *s;
	printf("\nEnter name: ");
	fgets(name, BUFSIZ, stdin);
	s = strrchr(name, '\n');
	if(s) {
		*s = '\0';
	}
}

void createChannel(uint64 serverID, const char *name) {
	unsigned int error;
	uint64 newChannelID;

	/* Set data of new channel. Use channelID of 0 for creating channels. */
	CHECK_ERROR(ts3server_setChannelVariableAsString(serverID, 0, CHANNEL_NAME,                name));
	CHECK_ERROR(ts3server_setChannelVariableAsString(serverID, 0, CHANNEL_TOPIC,               "Test channel topic"));
	CHECK_ERROR(ts3server_setChannelVariableAsString(serverID, 0, CHANNEL_DESCRIPTION,         "Test channel description"));
	CHECK_ERROR(ts3server_setChannelVariableAsInt   (serverID, 0, CHANNEL_FLAG_PERMANENT,      1));
	CHECK_ERROR(ts3server_setChannelVariableAsInt   (serverID, 0, CHANNEL_FLAG_SEMI_PERMANENT, 0));

	/* Flush changes to server */
	CHECK_ERROR(ts3server_flushChannelCreation(serverID, 0, &newChannelID));  /* Create as top-level channel */

    printf("\nCreated channel: %llu\n\n", (unsigned long long)newChannelID);
    return;

on_error:
    printf("Error creating channel: %d\n", error);
}

void deleteChannel(uint64 serverID) {
	uint64 channelID;
	int n;
	unsigned int error;

	/* Query channel ID from user */
	printf("\nEnter ID of channel to delete: ");
	n = scanf("%llu", (unsigned long long*)&channelID);
	emptyInputBuffer();
	if(n == 0) {
		printf("Invalid input. Please enter a number.\n\n");
		return;
	}

	/* Delete channel */
    if((error = ts3server_channelDelete(serverID, channelID, 0)) != ERROR_ok) {
		char* errormsg;
        if(ts3server_getGlobalErrorMessage(error, &errormsg) == ERROR_ok) {
		    printf("Error deleting channel: %s\n\n", errormsg);
		    ts3server_freeMemory(errormsg);
        }
	}
}

void renameChannel(uint64 serverID) {
	uint64 channelID;
	int n;
	unsigned int error;
	char name[BUFSIZ];

	/* Query channel ID from user */
	printf("\nEnter ID of channel to rename: ");
	n = scanf("%llu", (unsigned long long*)&channelID);
	emptyInputBuffer();
	if(n == 0) {
		printf("Invalid input. Please enter a number.\n\n");
		return;
	}

	/* Query new channel name from user */
	enterName(name);

	/* Change channel name and flush changes */
	CHECK_ERROR(ts3server_setChannelVariableAsString(serverID, channelID, CHANNEL_NAME, name));
	CHECK_ERROR(ts3server_flushChannelVariable(serverID, channelID));

    printf("Renamed channel %llu\n\n", (unsigned long long)channelID);
    return;

on_error:
	printf("Error renaming channel: %d\n\n", error);
}

void moveClient(uint64 serverID) {
	anyID clientIDArray[2];  /* We only want to move one client plus terminating null end-marker */
	uint64 newChannelID;  /* ID of channel to move the client into */
	unsigned int error;
	int n;

	/* Query client ID from user */
	printf("\nEnter ID of client to move: ");
	n = scanf("%hu", &clientIDArray[0]);
	emptyInputBuffer();
	if(n == 0) {
		printf("Invalid input. Please enter a number.\n\n");
		return;
	}

	clientIDArray[1] = 0;  /* Add end-marker */

	/* Query channel ID from user */
	printf("\nEnter ID of channel to move client into: ");
	n = scanf("%llu", (unsigned long long*)&newChannelID);
	emptyInputBuffer();
	if(n == 0) {
		printf("Invalid input. Please enter a number.\n\n");
		return;
	}

	/* Move client and check for error */
	if((error = ts3server_clientMove(serverID, newChannelID, clientIDArray)) != ERROR_ok) {
		char* errormsg;
		if(ts3server_getGlobalErrorMessage(error, &errormsg) == ERROR_ok) {
			printf("Error moving client: %s\n\n", errormsg);
			ts3server_freeMemory(errormsg);
		}
		return;
	}

	printf("Client %d moved to channel %llu\n", clientIDArray[0], (unsigned long long)newChannelID);
}

uint64 createVirtualServer(const char* name, int port, unsigned int maxClients) {
    char buffer[BUFSIZ] = { 0 };
    char filename[BUFSIZ];
    char port_str[20];
    char *keyPair;
    uint64 serverID;
    unsigned int error;

    /* Assemble filename: keypair_<port>.txt */
    strcpy(filename, "keypair_");
    sprintf(port_str, "%d", port);
    strcat(filename, port_str);
    strcat(filename, ".txt");

    /* Try reading keyPair from file */
    if(readKeyPairFromFile(filename, buffer) == 0) {
        keyPair = buffer;  /* Id read from file */
    } else {
        keyPair = "";  /* No Id saved, start virtual server with empty keyPair string */
    }

    /* Create the virtual server with specified port, name, keyPair and max clients */
    printf("Create virtual server using keypair '%s'\n", keyPair);
    printf("Create virtual server with %d slots\n", maxClients);
    if((error = ts3server_createVirtualServer(port, "0.0.0.0", name, keyPair, maxClients, &serverID)) != ERROR_ok) {
		char* errormsg;
		if(ts3server_getGlobalErrorMessage(error, &errormsg) == ERROR_ok) {
			printf("Error creating virtual server: %s (%d)\n", errormsg, error);
			ts3server_freeMemory(errormsg);
		}
        return 0;
    }

    /* If we didn't load the keyPair before, query it from virtual server and save to file */
    if(!*buffer) {
        if((error = ts3server_getVirtualServerKeyPair(serverID, &keyPair)) != ERROR_ok) {
            char* errormsg;
            if(ts3server_getGlobalErrorMessage(error, &errormsg) == ERROR_ok) {
                printf("Error querying keyPair: %s\n\n", errormsg);
                ts3server_freeMemory(errormsg);
            }
            return 0;
        }

        /* Save keyPair to file "keypair_<port>.txt"*/
        if(writeKeyPairToFile(filename, keyPair) != 0) {
			ts3server_freeMemory(keyPair);
            return 0;
        }
		ts3server_freeMemory(keyPair);
    }

    return serverID;
}

uint64 startVirtualServer() {
	int n;
	int port;
	unsigned int maxClients;
	char name[BUFSIZ];

	/* Ask user for server port */
	printf("\nEnter server port (default 9987): ");
	n = scanf("%d", &port);
	emptyInputBuffer();
	if(n == 0) {
		printf("Invalid input. Please enter a number.\n\n");
		return 0;
	}

	printf("\nEnter server capacity (default %d): ", MAX_CLIENTS);
	n = scanf("%d", &maxClients);
	emptyInputBuffer();
	if(n == 0) {
		printf("Invalid input. Please enter a number.\n\n");
		return 0;
	}
	
	/* Ask user for server name */
	enterName(name);

    return createVirtualServer(name, port, maxClients);
}

void editVirtualServer() {
	int n;
	uint64 serverID;
	int currentSlotCount, newSlotCount;
	unsigned int error;

	printf("\nEnter ID of virtual server to edit: ");
	n = scanf("%llu", (unsigned long long*)&serverID);
	emptyInputBuffer();
	if(n == 0) {
		printf("Invalid input. Please enter a number.\n\n");
		return;
	}
	if((error = ts3server_getVirtualServerVariableAsInt(serverID, VIRTUALSERVER_MAXCLIENTS, &currentSlotCount)) != ERROR_ok) {
		printf("Error getting the current capcity of virtual server: %d\n\n", error);
		return;
	}
	printf("\nEnter new capacity of virtual server (currently %d): ", currentSlotCount);
	n = scanf("%d", &newSlotCount);
	emptyInputBuffer();
	if(n == 0) {
		printf("Invalid input. Please enter a number.\n\n");
		return;
	}
	if((error = ts3server_setVirtualServerVariableAsInt(serverID, VIRTUALSERVER_MAXCLIENTS, newSlotCount)) != ERROR_ok) {
		printf("Error setting the new capacity: %d\n\n", error);
		return;
	}
	if((error = ts3server_flushVirtualServerVariable(serverID)) != ERROR_ok) {
		printf("Error flushing server variable updates %d\n\n", error);
		return;
	}
}

void stopVirtualServer() {
	int n;
	uint64 serverID;
	unsigned int error;

	printf("\nEnter ID of virtual server to stop: ");
	n = scanf("%llu", (unsigned long long*)&serverID);
	emptyInputBuffer();
	if(n == 0) {
		printf("Invalid input. Please enter a number.\n\n");
		return;
	}

	if((error = ts3server_stopVirtualServer(serverID)) != ERROR_ok) {
		printf("Error stopping virtual server: %d\n\n", error);
	}
}

int main() {
	char *version;
	short abort = 0;
    uint64 serverID;
	unsigned int error;
	int unknownInput = 0;
	uint64* ids;
	int i;

	/* Create struct for callback function pointers */
	struct ServerLibFunctions funcs;

	/* Initialize all callbacks with NULL */
	memset(&funcs, 0, sizeof(struct ServerLibFunctions));

	/* Now assign the used callback function pointers */
	funcs.onClientConnected          = onClientConnected;
	funcs.onClientDisconnected       = onClientDisconnected;
	funcs.onClientMoved              = onClientMoved;
	funcs.onChannelCreated           = onChannelCreated;
	funcs.onChannelEdited            = onChannelEdited;
	funcs.onChannelDeleted           = onChannelDeleted;
	funcs.onServerTextMessageEvent   = onServerTextMessageEvent;
	funcs.onChannelTextMessageEvent  = onChannelTextMessageEvent;
	funcs.onUserLoggingMessageEvent  = onUserLoggingMessageEvent;
	funcs.onClientStartTalkingEvent  = onClientStartTalkingEvent;
	funcs.onClientStopTalkingEvent   = onClientStopTalkingEvent;
	funcs.onAccountingErrorEvent     = onAccountingErrorEvent;
	funcs.onCustomPacketEncryptEvent = onCustomPacketEncryptEvent;
	funcs.onCustomPacketDecryptEvent = onCustomPacketDecryptEvent;
#ifdef USE_VOICEDATAEVENT
	funcs.onVoiceDataEvent          = onVoiceDataEvent;
#endif

	/* Initialize server lib with callbacks */
	if((error = ts3server_initServerLib(&funcs, LogType_FILE | LogType_CONSOLE | LogType_USERLOGGING, NULL)) != ERROR_ok) {
		char* errormsg;
		if(ts3server_getGlobalErrorMessage(error, &errormsg) == ERROR_ok) {
			printf("Error initialzing serverlib: %s\n", errormsg);
			ts3server_freeMemory(errormsg);
		}
		return 1;
	}

	printf("Server running\n");

    /* Query and print server lib version */
    if((error = ts3server_getServerLibVersion(&version)) != ERROR_ok) {
        printf("Error querying server lib version: %d\n", error);
        return 1;
    }
    printf("Server lib version: %s\n", version);
    ts3server_freeMemory(version);  /* Release dynamically allocated memory */

	/* Create a virtual server on localhost using default port 9987 with max 10 slots */
    serverID = createVirtualServer("TS3 SDK Test Server", 9987, MAX_CLIENTS);

    /* Set welcome message */
    if((error = ts3server_setVirtualServerVariableAsString(serverID, VIRTUALSERVER_WELCOMEMESSAGE, "Hello TeamSpeak 3")) != ERROR_ok) {
        printf("Error setting server welcomemessage: %d\n", error);
        return 1;
    }

    /* Set server password */
    if((error = ts3server_setVirtualServerVariableAsString(serverID, VIRTUALSERVER_PASSWORD, "secret")) != ERROR_ok) {
        printf("Error setting server password: %d\n", error);
        return 1;
    }

    /* Flush above two changes to server */
    if((error = ts3server_flushVirtualServerVariable(serverID)) != ERROR_ok) {
        printf("Error flushing server variables: %d\n", error);
        return 1;
    }

	/* Simple commandline interface */
	printf("\nTeamSpeak 3 server commandline interface\n");
	showHelp();

	while(!abort) {
		int c;
		if(unknownInput == 0) {
			printf("\nEnter Command (h for help)> ");
		}
		unknownInput = 0;
		c = getchar();
		switch(c) {
			case 'q':
				printf("\nShutting down server...\n");
				abort = 1;
				break;
			case 'h':
				showHelp();
				break;
			case 'v':
				showVirtualServers();
				break;
			case 'c':
				showChannels(serverID);
				break;
			case 'l':
				showClients(serverID);
				break;
			case 'n':
			{
				char name[BUFSIZ];
				createDefaultChannelName(name);
				createChannel(serverID, name);
				break;
			}
			case 'N':
			{
				char name[BUFSIZ];
				emptyInputBuffer();
				enterName(name);
				createChannel(serverID, name);
				break;
			}
			case 'd':
				deleteChannel(serverID);
				break;
			case 'r':
				renameChannel(serverID);
				break;
			case 'm':
				moveClient(serverID);
				break;
			case 'C':
				startVirtualServer();
				break;
			case 'E':
				editVirtualServer();
				break;
			case 'S':
				stopVirtualServer();
				break;
			default:
				unknownInput = 1;
		}

		SLEEP(50);
	}

    /* Stop virtual servers to make sure connected clients are notified instead of dropped */
    if((error = ts3server_getVirtualServerList(&ids)) != ERROR_ok) {  /* Get array of virtual server IDs */
        printf("Error getting virtual server list: %d\n", error);
    } else {
		for(i=0; ids[i]; i++) {
			if((error = ts3server_stopVirtualServer(ids[i])) != ERROR_ok) {
				printf("Error stopping virtual server: %d\n", error);
		        break;
			}
	  }
      ts3server_freeMemory(ids);
	}

    /* Shutdown server lib */
    if((error = ts3server_destroyServerLib()) != ERROR_ok) {
        printf("Error destroying server lib: %d\n", error);
        return 1;
    }

	return 0;
}
