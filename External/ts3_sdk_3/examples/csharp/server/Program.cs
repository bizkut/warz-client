/*
 * TeamSpeak 3 server minimal sample C#
 *
 * Copyright (c) 2007-2010 TeamSpeak-Systems
 */

using System;
using System.Runtime.InteropServices;
using System.IO;

using anyID = System.UInt16;
using uint64 = System.UInt64;

namespace ts3_server_minimal_sample
{
	class ts3server {
		/*
		 * Read server key from file
		*/
		public static int readKeyPairFromFile(string fileName, out string keyPair) {
			try {
				keyPair = System.IO.File.ReadAllText(fileName);
			} catch (System.Exception /*ex*/) {
				Console.WriteLine("Could not open file '{0}' for reading keypair", fileName);
				keyPair = "";
				return -1;
			}
			Console.WriteLine("Read keypair '{0}' from file '{1}'.", keyPair, fileName);
			return 0;
		}

		/*
		 * Write server key to file
		*/
		public static int writeKeyPairToFile(string fileName, string keyPair) {
			try {
				File.WriteAllText(fileName, keyPair);
			} catch (System.Exception /*ex*/) {
				return -1;
			}
			return 0;
		}

		#if x64
			[DllImport("ts3server_win64.dll", EntryPoint = "ts3server_initServerLib")]
			public static extern uint ts3server_initServerLib(ref server_callback_struct arg0, LogTypes arg1, string arg2);

			[DllImport("ts3server_win64.dll", EntryPoint = "ts3server_getServerLibVersion")]
			public static extern uint ts3server_getServerLibVersion(out IntPtr arg0);

			[DllImport("ts3server_win64.dll", EntryPoint = "ts3server_freeMemory")]
			public static extern uint ts3server_freeMemory(IntPtr arg0);

			[DllImport("ts3server_win64.dll", EntryPoint = "ts3server_destroyServerLib")]
			public static extern uint ts3server_destroyServerLib();

			[DllImport("ts3server_win64.dll", EntryPoint = "ts3server_createVirtualServer")]
            public static extern uint ts3server_createVirtualServer(int serverPort, string ip, string serverName, string serverKeyPair, uint serverMaxClients, out uint64 serverID);

			[DllImport("ts3server_win64.dll", EntryPoint = "ts3server_getGlobalErrorMessage")]
			public static extern uint ts3server_getGlobalErrorMessage(uint errorcode, out IntPtr errormessage);

			[DllImport("ts3server_win64.dll", EntryPoint = "ts3server_getVirtualServerKeyPair")]
            public static extern uint ts3server_getVirtualServerKeyPair(uint64 serverID, out IntPtr result);

			[DllImport("ts3server_win64.dll", EntryPoint = "ts3server_setVirtualServerVariableAsString")]
            public static extern uint ts3server_setVirtualServerVariableAsString(uint64 serverID, VirtualServerProperties flag, string result);

			[DllImport("ts3server_win64.dll", EntryPoint = "ts3server_flushVirtualServerVariable")]
            public static extern uint ts3server_flushVirtualServerVariable(uint64 serverID);

			[DllImport("ts3server_win64.dll", EntryPoint = "ts3server_stopVirtualServer")]
            public static extern uint ts3server_stopVirtualServer(uint64 serverID);
		#else
			[DllImport("ts3server_win32.dll", EntryPoint = "ts3server_initServerLib")]
			public static extern uint ts3server_initServerLib(ref server_callback_struct arg0, LogTypes arg1, string arg2);

			[DllImport("ts3server_win32.dll", EntryPoint = "ts3server_getServerLibVersion")]
			public static extern uint ts3server_getServerLibVersion(out IntPtr arg0);

			[DllImport("ts3server_win32.dll", EntryPoint = "ts3server_freeMemory")]
			public static extern uint ts3server_freeMemory(IntPtr arg0);

			[DllImport("ts3server_win32.dll", EntryPoint = "ts3server_destroyServerLib")]
			public static extern uint ts3server_destroyServerLib();

			[DllImport("ts3server_win32.dll", EntryPoint = "ts3server_createVirtualServer")]
			public static extern uint ts3server_createVirtualServer(int serverPort, string ip, string serverName, string serverKeyPair, uint serverMaxClients, out uint64 serverID);

			[DllImport("ts3server_win32.dll", EntryPoint = "ts3server_getGlobalErrorMessage")]
			public static extern uint ts3server_getGlobalErrorMessage(uint errorcode, out IntPtr errormessage);

			[DllImport("ts3server_win32.dll", EntryPoint = "ts3server_getVirtualServerKeyPair")]
			public static extern uint ts3server_getVirtualServerKeyPair(uint64 serverID, out IntPtr result);

			[DllImport("ts3server_win32.dll", EntryPoint = "ts3server_setVirtualServerVariableAsString")]
			public static extern uint ts3server_setVirtualServerVariableAsString(uint64 serverID, VirtualServerProperties flag, string result);

			[DllImport("ts3server_win32.dll", EntryPoint = "ts3server_flushVirtualServerVariable")]
			public static extern uint ts3server_flushVirtualServerVariable(uint64 serverID);

			[DllImport("ts3server_win32.dll", EntryPoint = "ts3server_stopVirtualServer")]
			public static extern uint ts3server_stopVirtualServer(uint64 serverID);
#endif
    }

	class Program {
		static void Main(string[] args) {
			/* Assign the used callback function pointers */
			server_callback_struct cbs = new server_callback_struct();
			cbs.onClientConnected_delegate = new onClientConnected_type(callback.onClientConnected);
			cbs.onClientDisconnected_delegate = new onClientDisconnected_type(callback.onClientDisconnected);
            cbs.onClientMoved_delegate = new onClientMoved_type(callback.onClientMoved);
			cbs.onChannelCreated_delegate = new onChannelCreated_type(callback.onChannelCreated);
			cbs.onChannelEdited_delegate = new onChannelEdited_type(callback.onChannelEdited);
			cbs.onChannelDeleted_delegate = new onChannelDeleted_type(callback.onChannelDeleted);
            cbs.onServerTextMessageEvent_delegate = new onServerTextMessageEvent_type(callback.onServerTextMessageEvent);
            cbs.onChannelTextMessageEvent_delegate = new onChannelTextMessageEvent_type(callback.onChannelTextMessageEvent);
			cbs.onUserLoggingMessageEvent_delegate = new onUserLoggingMessageEvent_type(callback.onUserLoggingMessageEvent);
			cbs.onClientStartTalkingEvent_delegate = new onClientStartTalkingEvent_type(callback.onClientStartTalkingEvent);
			cbs.onClientStopTalkingEvent_delegate = new onClientStopTalkingEvent_type(callback.onClientStopTalkingEvent);
            cbs.onAccountingErrorEvent_delegate = new onAccountingErrorEvent_type(callback.onAccountingErrorEvent);

			/* Initialize server lib with callbacks */
			uint error = ts3server.ts3server_initServerLib(ref cbs, LogTypes.LogType_FILE | LogTypes.LogType_CONSOLE | LogTypes.LogType_USERLOGGING, null);
            if (error != public_errors.ERROR_ok) {
                Console.WriteLine("Failed to initialize serverlib: {0}.", error);
                return;
            }

			/* Query and print client lib version */
			IntPtr versionPtr = IntPtr.Zero;
			error = ts3server.ts3server_getServerLibVersion(out versionPtr);
			if (error != public_errors.ERROR_ok) {
				Console.WriteLine("Failed to get clientlib version: {0}.", error);
				return;
			}
			string version = Marshal.PtrToStringAnsi(versionPtr);
			Console.WriteLine(version);
			ts3server.ts3server_freeMemory(versionPtr); /* Release dynamically allocated memory */

			string filename = string.Format("keypair_{0}.txt", 9987); // 9987 = default port
			string keyPair;
			if (ts3server.readKeyPairFromFile(filename,out keyPair) != 0) {
				keyPair = "";
			}

			/* Create virtual server using default port 9987 with max 10 slots */

			/* Create the virtual server with specified port, name, keyPair and max clients */
            uint64 serverID = 0;
			Console.WriteLine("Create virtual server using keypair '{0}'", keyPair);
			IntPtr pServerID = IntPtr.Zero;
			error = ts3server.ts3server_createVirtualServer(9987, "0.0.0.0", "TeamSpeak 3 SDK Testserver", keyPair, 10, out serverID);
			if (error != public_errors.ERROR_ok) {
				IntPtr errormsgPtr = IntPtr.Zero;
				ts3server.ts3server_getGlobalErrorMessage(error, out errormsgPtr);
				if (error == public_errors.ERROR_ok) {
					string errormsg = Marshal.PtrToStringAnsi(errormsgPtr);
					Console.WriteLine("Error creating virtual server: {0} ({1})", errormsg, error);
					ts3server.ts3server_freeMemory(errormsgPtr);
				}
				return;
			}
			
			/* If we didn't load the keyPair before, query it from virtual server and save to file */
			if (keyPair == null) {
				IntPtr keyPairPtr = IntPtr.Zero;
				error = ts3server.ts3server_getVirtualServerKeyPair(serverID, out keyPairPtr);
				if (error != public_errors.ERROR_ok) {
					IntPtr errormsgPtr = IntPtr.Zero;
					ts3server.ts3server_getGlobalErrorMessage(error, out errormsgPtr);
					if (error == public_errors.ERROR_ok) {
						string errormsg = Marshal.PtrToStringAnsi(errormsgPtr);
						Console.WriteLine("Error querying keyPair: %s\n", errormsg);
						ts3server.ts3server_freeMemory(errormsgPtr);
					}
					return;
				}
				keyPair = Marshal.PtrToStringAnsi(keyPairPtr);

				/* Save keyPair to file "keypair_<port>.txt"*/
				if (ts3server.writeKeyPairToFile(filename, keyPair) != 0) {
					return;
				}
			}

			/* Set welcome message */
			error = ts3server.ts3server_setVirtualServerVariableAsString(serverID, VirtualServerProperties.VIRTUALSERVER_WELCOMEMESSAGE, "Hello TeamSpeak 3");
			if (error != public_errors.ERROR_ok) {
				Console.WriteLine("Error setting server welcomemessage: {0}", error);
				return;
			}

			/* Set server password */
			error = ts3server.ts3server_setVirtualServerVariableAsString(serverID, VirtualServerProperties.VIRTUALSERVER_PASSWORD, "secret");
			if (error != public_errors.ERROR_ok) {
				Console.WriteLine("Error setting server password: {0}", error);
				return;
			}

			/* Flush above two changes to server */
			error = ts3server.ts3server_flushVirtualServerVariable(serverID);
			if (error != public_errors.ERROR_ok) {
				Console.WriteLine("Error flushing server variables: {0}", error);
				return;
			}

			/* Wait for user input */
			Console.WriteLine("\n--- Press Return to shutdown server and exit ---");
			Console.ReadLine();

			/* Stop virtual server */
			error = ts3server.ts3server_stopVirtualServer(serverID);
			if (error != public_errors.ERROR_ok) {
				Console.WriteLine("Error stopping virtual server: {0}", error);
				return;
			}

			/* Shutdown server lib */
			error = ts3server.ts3server_destroyServerLib();
			if (error != public_errors.ERROR_ok) {
				Console.WriteLine("Error destroying server lib: {0}", error);
				return;
			}

		}
	}
}
