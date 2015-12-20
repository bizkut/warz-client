// Copyright (C) 2001-2003 Even Balance, Inc.
//
//
// pbsdk.h
//
// EVEN BALANCE - T.RAY
//

#ifndef __PBSDK_H__
#define __PBSDK_H__


#define _cplusplus
#include "pbcommon.h"
#include "pbcl.h"
#include "pbsv.h"



#ifdef __WITH_PB__



//
// PB Integration Points - Typedefs
//
typedef char *(*PBtd_getBasePath) ( char *path , int maxlen ) ;
typedef char *(*PBtd_getHomePath) ( char *path , int maxlen ) ;
typedef void  (*PBtd_CvarSet) ( const char *varName , const char *value ) ;
typedef void  (*PBtd_SetClPunkBuster) ( char *value ) ;
typedef void  (*PBtd_ExecCmd) ( const char *cmd ) ;
typedef void *(*PBtd_DllHandle) ( const char *modname ) ;
typedef char *(*PBtd_CvarValidate) ( char *buf ) ;
typedef int   (*PBtd_CvarWalk) ( char **name , char **string , int *flags , char **resetString ) ;
typedef void  (*PBtd_GetFileMD5) ( int index, char *fn, unsigned char *md5, long *sz ) ;
typedef char *(*PBtd_GetKeyName) ( int keynum ) ;
typedef char *(*PBtd_GetKeyBinding) ( int keynum ) ;
typedef int   (*PBtd_GetMaxKeys) ( void ) ;
typedef char *(*PBtd_GetServerAddr) ( void ) ;
typedef char *(*PBtd_GetKeyValue) ( char *s , char *k ) ;
typedef char *(*PBtd_GetServerInfo) ( void ) ;
typedef char *(*PBtd_GetCvarValue) ( char *var_name , int origination ) ;
typedef void  (*PBtd_Out) ( char *msg , unsigned long color ) ;
typedef void  (*PBtd_SendClPacket) ( int datalen , char *data ) ;
typedef void  (*PBtd_SendUdpPacket) ( char *addr , unsigned short port , int datalen , char *data, bool fromServer ) ;
typedef char *(*PBtd_GlQuery) ( int queryType ) ;
typedef void  (*PBtd_SetSvPunkBuster) ( char *val ) ;
typedef void  (*PBtd_DropClient) ( int clientIndex , char *reason ) ;
typedef void  (*PBtd_ForceTeam) ( int clientIndex , int team) ;
typedef int   (*PBtd_GetMaxClients) ( void ) ;
typedef int   (*PBtd_GetClientInfo) ( int index , stPb_Sv_Client *c ) ;
typedef int   (*PBtd_GetClientStats) ( int index , char *data ) ;
typedef void  (*PBtd_SendSvPacket) ( int datalen , char *data , int index ) ;



#define CONST_PBSDKID 0xFF80FF1E
#define PBNULLFUNC "NULL Function Pointer"

class stPbSdk {
public:
	unsigned long PBSDKID ;
	stPbCl pbcl ;
	stPbSv pbsv ;
	unsigned long flags ;
	void *exeInstance ;
	char *ConsoleCaptureBuf ;
	long ConsoleCaptureBufLen ;

	PBtd_getBasePath m_getBasePath ;
	PBtd_getHomePath m_getHomePath ;
	PBtd_CvarSet m_CvarSet ;
	PBtd_SetClPunkBuster m_SetClPunkBuster ;
	PBtd_ExecCmd m_ExecCmd ;
	PBtd_DllHandle m_DllHandle ;
	PBtd_CvarValidate m_CvarValidate ;
	PBtd_CvarWalk m_CvarWalk ;
	PBtd_GetFileMD5 m_GetFileMD5 ;
	PBtd_GetKeyName m_GetKeyName ;
	PBtd_GetKeyBinding m_GetKeyBinding ;
	PBtd_GetMaxKeys m_GetMaxKeys ;
	PBtd_GetServerAddr m_GetServerAddr ;
	PBtd_GetKeyValue m_GetKeyValue ;
	PBtd_GetServerInfo m_GetServerInfo ;
	PBtd_GetCvarValue m_GetCvarValue ;
	PBtd_Out m_Out ;
	PBtd_SendClPacket m_SendClPacket ;
	PBtd_SendUdpPacket m_SendUdpPacket ;
	PBtd_GlQuery m_GlQuery ;
	PBtd_SetSvPunkBuster m_SetSvPunkBuster ;
	PBtd_DropClient m_DropClient ;
	PBtd_ForceTeam m_ForceTeam ;
	PBtd_GetMaxClients m_GetMaxClients ;
	PBtd_GetClientInfo m_GetClientInfo ;
	PBtd_GetClientStats m_GetClientStats ;
	PBtd_SendSvPacket m_SendSvPacket ;

	char *pb_getBasePath ( char *path , int maxlen ) { 
		if ( m_getBasePath == NULL ) {
			*path = 0 ;
			return PBNULLFUNC ;
		}
		return m_getBasePath ( path , maxlen ) ;
	}
	char *pb_getHomePath ( char *path , int maxlen ) {
		if ( m_getHomePath == NULL ) {
			*path = 0 ;
			return PBNULLFUNC ;
		}
		return m_getHomePath ( path , maxlen ) ;
	}
	void  pb_CvarSet ( const char *varName , const char *value ) {
		if ( m_CvarSet == NULL ) return ;
		m_CvarSet ( varName , value ) ;
	}
	void  pb_SetClPunkBuster ( char *value ) {
		if ( m_SetClPunkBuster == NULL ) return ;
		m_SetClPunkBuster ( value ) ;
	}
	void  pb_ExecCmd ( const char *cmd ) {
		if ( m_ExecCmd == NULL ) return ;
		m_ExecCmd ( cmd ) ;
	}
	void *pb_DllHandle ( const char *modname ) {
		if ( m_DllHandle == NULL ) return NULL ;
		return m_DllHandle ( modname ) ;
	}
	char *pb_CvarValidate ( char *buf ) {
		if ( m_CvarValidate == NULL ) {
			*buf = 0 ;
			return buf ;
		}
		return m_CvarValidate ( buf ) ;
	}
	int   pb_CvarWalk ( char **name , char **string , int *flags , char **resetString ) {
		if ( m_CvarWalk == NULL ) return 0 ;
		return m_CvarWalk ( name , string , flags , resetString ) ;
	}
	void pb_GetFileMD5 ( int index , char *fn , unsigned char *md5 , long *sz ) {
		if ( m_GetFileMD5 == NULL ) {
			*sz = -1 ;
			return ;
		}
		m_GetFileMD5 ( index , fn , md5 , sz ) ;
	}
	char *pb_GetKeyName ( int keynum ) {
		if ( m_GetKeyName == NULL ) return "" ;
		return m_GetKeyName ( keynum ) ;
	}
	char *pb_GetKeyBinding ( int keynum ) {
		if ( m_GetKeyBinding == NULL ) return "" ;
		return m_GetKeyBinding ( keynum ) ;
	}
	int   pb_GetMaxKeys ( void ) {
		if ( m_GetMaxKeys == NULL ) return 0 ;
		return m_GetMaxKeys() ;
	}
	char *pb_GetServerAddr ( void ) {
		if ( m_GetServerAddr == NULL ) return "" ;
		return m_GetServerAddr() ;
	}
	char *pb_GetKeyValue ( char *s , char *k ) {
		if ( m_GetKeyValue == NULL ) return "" ;
		return m_GetKeyValue ( s , k ) ;
	}
	char *pb_GetServerInfo ( void ) {
		if ( m_GetServerInfo == NULL ) return "" ;
		return m_GetServerInfo() ;
	}
	char *pb_GetCvarValue ( char *var_name , int origination ) {
		if ( m_GetCvarValue == NULL ) return "" ;
		return m_GetCvarValue ( var_name , origination ) ;
	}
	void  pb_Outf ( unsigned long color , char *msg , ... ) {
		if ( m_Out == NULL ) return ;

		char buf[4150] ;
		va_list va ;
		va_start ( va , msg ) ;
		if ( strlen ( msg ) > 2048 ) {
			strncpy ( buf , msg , 4096 ) ;
			buf[4096] = 0 ;
#ifdef __PBWIN32__
		} else _vsnprintf ( buf , 4149 , msg , va ) ;
#else
   	        } else  vsnprintf ( buf , 4149 , msg , va ) ;
#endif
		m_Out ( buf , color ) ;
	}
	void  pb_SendClPacket ( int datalen , char *data ) {
		if ( m_SendClPacket == NULL ) return ;
		m_SendClPacket ( datalen , data ) ;
	}
	void  pb_SendUdpPacket ( char *addr , unsigned short port , int datalen , char *data, bool fromServer ) {
		if ( m_SendUdpPacket == NULL ) return ;
		m_SendUdpPacket ( addr , port , datalen , data, fromServer ) ;
	}
	char *pb_GlQuery ( int queryType ) {
		if ( m_GlQuery == NULL ) return NULL ;
		return m_GlQuery ( queryType ) ;
	}
	void  pb_SetSvPunkBuster ( char *val ) {
		if ( m_SetSvPunkBuster == NULL ) return ;
		m_SetSvPunkBuster ( val ) ;
	}
	void  pb_DropClient ( int clientIndex , char *reason ) {
		if ( m_DropClient == NULL ) return ;
		m_DropClient ( clientIndex , reason ) ;
	}
	void  pb_ForceTeam ( int clientIndex , int team) {
		if ( m_ForceTeam == NULL ) return ;
		m_ForceTeam ( clientIndex , team ) ;
	}
	int   pb_GetMaxClients ( void ) {
		if ( m_GetMaxClients == NULL ) return 0 ;
		return m_GetMaxClients() ;
	}
	int   pb_GetClientInfo ( int index , stPb_Sv_Client *c ) {
		if ( m_GetClientInfo == NULL ) return 0 ;
		return m_GetClientInfo ( index , c ) ;
	}
	int   pb_GetClientStats ( int index , char *data ) {
		if ( m_GetClientStats == NULL ) return 0 ;
		return m_GetClientStats ( index , data ) ;
	}
	void  pb_SendSvPacket ( int datalen , char *data , int index ) {
		if ( m_SendSvPacket == NULL ) return ;
		m_SendSvPacket ( datalen , data , index ) ;
	}

	inline void init ( void ) {
		memset ( this , 0 , sizeof ( stPbSdk ) ) ;
	}

	//constructor
	stPbSdk() { 
		init() ;
		PBSDKID = CONST_PBSDKID ;
		pbcl.m_GlQuery = m_GlQuery ;
		pbcl.init() ;
		pbsv.init() ;
	}
	
	void Shutdown( void ) {
		pbsv.Shutdown();
		pbcl.Shutdown();
		pbsdk = NULL;
	}

	//destructor
	~stPbSdk() {
		Shutdown();
	}
} ;



extern class stPbSdk *pbsdk ;



//One game source module should "#define DEFINE_PbSdk" before including this file so that only one 
// instance of the following struct and function are each defined in the game project.
//Usually, the source module containing the main() or WinMain() function will serve in that role.
#ifdef DEFINE_PbSdk
#ifndef PbSdk_DEFINED

//single instance of stPbSdk
stPbSdk PbSdkInstance ;
stPbSdk *pbsdk = NULL ;
#define PbSdk_DEFINED

#endif //#ifndef PbSdk_DEFINED
#endif //#ifdef DEFINE_PbSdk

#endif //#ifdef __WITH_PB__

#endif //#ifdef __PBSDK_H__