public enum TalkStatus {
	STATUS_NOT_TALKING = 0,
	STATUS_TALKING = 1,
	STATUS_TALKING_WHILE_DISABLED = 2,
};

public enum SpeexCodec {
	CODEC_SPEEX_NARROWBAND = 0,   //mono, 16bit,  8kHz, bitrate dependant on the quality setting
	CODEC_SPEEX_WIDEBAND,         //mono, 16bit, 16kHz, bitrate dependant on the quality setting
	CODEC_SPEEX_ULTRAWIDEBAND,    //mono, 16bit, 32kHz, bitrate dependant on the quality setting
	CODEC_CELT_MONO,              //mono, 16bit, 48kHz, bitrate dependant on the quality setting
	CODEC_DUMMY_MONO,             //mono, 16bit, 48kHz, no compression (=> bitrate == 93.75 KiB/s!)
};

public enum TextMessageTargetMode {
	TextMessageTarget_CLIENT=1,
	TextMessageTarget_CHANNEL,
	TextMessageTarget_SERVER,
	TextMessageTarget_MAX
};

public enum MuteInputStatus {
	MUTEINPUT_NONE = 0,
	MUTEINPUT_MUTED,
};

public enum MuteOutputStatus {
	MUTEOUTPUT_NONE = 0,
	MUTEOUTPUT_MUTED,
};

public enum HardwareInputStatus {
	HARDWAREINPUT_DISABLED = 0,
	HARDWAREINPUT_ENABLED,
};

public enum HardwareOutputStatus {
	HARDWAREOUTPUT_DISABLED = 0,
	HARDWAREOUTPUT_ENABLED,
};

public enum InputDeactivationStatus {
	INPUT_ACTIVE = 0,
	INPUT_DEACTIVATED = 1,
};

public enum ReasonIdentifier {
	REASON_NONE                              = 0,  //no reason data
	REASON_MOVED                             = 1,  //{SectionInvoker}
	REASON_SUBSCRIPTION                      = 2,  //no reason data
	REASON_LOST_CONNECTION                   = 3,  //reasonmsg=reason
	REASON_KICK_CHANNEL                      = 4,  //{SectionInvoker} reasonmsg=reason               //{SectionInvoker} is only added server->client
	REASON_KICK_SERVER                       = 5,  //{SectionInvoker} reasonmsg=reason               //{SectionInvoker} is only added server->client
	REASON_KICK_SERVER_BAN                   = 6,  //{SectionInvoker} reasonmsg=reason bantime=time  //{SectionInvoker} is only added server->client
	REASON_SERVERSTOP                        = 7,  //reasonmsg=reason
	REASON_CLIENTDISCONNECT                  = 8,  //reasonmsg=reason
	REASON_CHANNELUPDATE                     = 9,  //no reason data
	REASON_CHANNELEDIT                       = 10, //{SectionInvoker}
	REASON_CLIENTDISCONNECT_SERVER_SHUTDOWN  = 11,  //reasonmsg=reason
};

public enum ChannelProperties {
	CHANNEL_NAME = 0,                       //Available for all channels that are "in view", always up-to-date
	CHANNEL_TOPIC,                          //Available for all channels that are "in view", always up-to-date
	CHANNEL_DESCRIPTION,                    //Must be requested (=> requestChannelDescription)
	CHANNEL_PASSWORD,                       //not available client side
	CHANNEL_CODEC,                          //Available for all channels that are "in view", always up-to-date
	CHANNEL_CODEC_QUALITY,                  //Available for all channels that are "in view", always up-to-date
	CHANNEL_MAXCLIENTS,                     //Available for all channels that are "in view", always up-to-date
	CHANNEL_MAXFAMILYCLIENTS,               //Available for all channels that are "in view", always up-to-date
	CHANNEL_ORDER,                          //Available for all channels that are "in view", always up-to-date
	CHANNEL_FLAG_PERMANENT,                 //Available for all channels that are "in view", always up-to-date
	CHANNEL_FLAG_SEMI_PERMANENT,            //Available for all channels that are "in view", always up-to-date
	CHANNEL_FLAG_DEFAULT,                   //Available for all channels that are "in view", always up-to-date
	CHANNEL_FLAG_PASSWORD,                  //Available for all channels that are "in view", always up-to-date
    CHANNEL_CODEC_LATENCY_FACTOR,           //Available for all channels that are "in view", always up-to-date
    CHANNEL_CODEC_IS_UNENCRYPTED,           //Available for all channels that are "in view", always up-to-date
	CHANNEL_ENDMARKER,
};

public enum ClientProperties {
	CLIENT_UNIQUE_IDENTIFIER = 0,           //automatically up-to-date for any client "in view", can be used to identify this particular client installation
	CLIENT_NICKNAME,                        //automatically up-to-date for any client "in view"
	CLIENT_VERSION,                         //for other clients than ourself, this needs to be requested (=> requestClientVariables)
	CLIENT_PLATFORM,                        //for other clients than ourself, this needs to be requested (=> requestClientVariables)
	CLIENT_FLAG_TALKING,                    //automatically up-to-date for any client that can be heard (in room / whisper)
	CLIENT_INPUT_MUTED,                     //automatically up-to-date for any client "in view", this clients microphone mute status
	CLIENT_OUTPUT_MUTED,                    //automatically up-to-date for any client "in view", this clients headphones/speakers mute status
    CLIENT_OUTPUTONLY_MUTED,                //automatically up-to-date for any client "in view", this clients headphones/speakers only mute status
	CLIENT_INPUT_HARDWARE,                  //automatically up-to-date for any client "in view", this clients microphone hardware status (is the capture device opened?)
	CLIENT_OUTPUT_HARDWARE,                 //automatically up-to-date for any client "in view", this clients headphone/speakers hardware status (is the playback device opened?)
	CLIENT_INPUT_DEACTIVATED,               //only usable for ourself, not propagated to the network
	CLIENT_IDLE_TIME,                       //internal use
	CLIENT_DEFAULT_CHANNEL,                 //only usable for ourself, the default channel we used to connect on our last connection attempt
	CLIENT_DEFAULT_CHANNEL_PASSWORD,        //internal use
	CLIENT_SERVER_PASSWORD,                 //internal use
	CLIENT_META_DATA,                       //automatically up-to-date for any client "in view", not used by TeamSpeak, free storage for sdk users
	CLIENT_IS_MUTED,                        //only make sense on the client side locally, "1" if this client is currently muted by us, "0" if he is not
	CLIENT_IS_RECORDING,                    //automatically up-to-date for any client "in view"
    CLIENT_VOLUME_MODIFICATOR,				//internal use
	CLIENT_ENDMARKER,
};

public enum VirtualServerProperties {
	VIRTUALSERVER_UNIQUE_IDENTIFIER = 0,    //available when connected, can be used to identify this particular server installation
	VIRTUALSERVER_NAME,                     //available and always up-to-date when connected
	VIRTUALSERVER_WELCOMEMESSAGE,           //available when connected, not updated while connected
	VIRTUALSERVER_PLATFORM,                 //available when connected
	VIRTUALSERVER_VERSION,                  //available when connected
	VIRTUALSERVER_MAXCLIENTS,               //only available on request (=> requestServerVariables), stores the maximum number of clients that may currently join the server
	VIRTUALSERVER_PASSWORD,                 //not available to clients, the server password
	VIRTUALSERVER_CLIENTS_ONLINE,           //only available on request (=> requestServerVariables),
	VIRTUALSERVER_CHANNELS_ONLINE,          //only available on request (=> requestServerVariables),
	VIRTUALSERVER_CREATED,                  //available when connected, stores the time when the server was created
	VIRTUALSERVER_UPTIME,                   //only available on request (=> requestServerVariables), the time since the server was started
    VIRTUALSERVER_CODEC_ENCRYPTION_MODE,    //available and always up-to-date when connected
	VIRTUALSERVER_ENDMARKER,
};

public enum LogTypes {
	LogType_NONE          = 0x0000,
	LogType_FILE          = 0x0001,
	LogType_CONSOLE       = 0x0002,
	LogType_USERLOGGING   = 0x0004,
	LogType_NO_NETLOGGING = 0x0008,
	LogType_DATABASE      = 0x0010,
};

public enum LogLevel {
	LogLevel_CRITICAL = 0, //these messages stop the program
	LogLevel_ERROR,        //everything that is really bad, but not so bad we need to shut down
	LogLevel_WARNING,      //everything that *might* be bad
	LogLevel_DEBUG,        //output that might help find a problem
	LogLevel_INFO,         //informational output, like "starting database version x.y.z"
	LogLevel_DEVEL         //developer only output (will not be displayed in release mode)
};
