#pragma once

int GetXMLVal( const char* Name, pugi::xml_node& node, float* oVal );
int GetXMLVal( const char* Name, pugi::xml_node& node, int* oVal );
int GetXMLVal( const char* Name, pugi::xml_node& node, unsigned int* oVal );
int GetXMLVal( const char* Name, pugi::xml_node& node, r3dSTLString* oVal );
int GetXMLVal( const char* Name, pugi::xml_node& node, r3dString* oVal );
int GetXMLVal( const char* Name, pugi::xml_node& node, char* oVal );
int GetXMLVal( const char* Name, pugi::xml_node& node, r3dColor* oVal );
int GetXMLVal( const char* Name, pugi::xml_node& node, r3dPoint3D* oVal );
int GetXMLVal( const char* Name, pugi::xml_node& node, r3dPoint2D* oVal );

int GetXMLCmdVarF( const char* Name, pugi::xml_node& node, CmdVar * oVar );
int GetXMLCmdVarI( const char* Name, pugi::xml_node& node, CmdVar * oVar );

int GetXMLCurve3f( const char* Name, pugi::xml_node& node, class r3dTimeGradient2* oTimeGradient );
int GetXMLCurve3f( const char* Name, pugi::xml_node& node, class r3dBezierGradient* oBezGradient );

int SetXMLVal( const char* Name, pugi::xml_node& node, const float* val );
int SetXMLVal( const char* Name, pugi::xml_node& node, const int* val );
int SetXMLVal( const char* Name, pugi::xml_node& node, const unsigned int* val );
int SetXMLVal( const char* Name, pugi::xml_node& node, const r3dString* val );
int SetXMLVal( const char* Name, pugi::xml_node& node, const r3dSTLString* val );
int SetXMLVal( const char* Name, pugi::xml_node& node, const char* val );
int SetXMLVal( const char* Name, pugi::xml_node& node, const r3dColor* val );
int SetXMLVal( const char* Name, pugi::xml_node& node, const r3dPoint3D* val );
int SetXMLVal( const char* Name, pugi::xml_node& node, const r3dPoint2D* val );

int SetXMLCmdVarF( const char* Name, pugi::xml_node& node, CmdVar * var );
int SetXMLCmdVarI( const char* Name, pugi::xml_node& node, CmdVar * var );

int SetXMLCurve3f( const char* Name, pugi::xml_node& node, class r3dTimeGradient2* timeGradient );
int SetXMLCurve3f( const char* Name, pugi::xml_node& node, class r3dBezierGradient* bezGradient );

template < bool Write > R3D_FORCEINLINE int SerializeXMLVal( const char* Name, pugi::xml_node& node, float* ioVal );
template < bool Write > R3D_FORCEINLINE int SerializeXMLVal( const char* Name, pugi::xml_node& node, int* ioVal );
template < bool Write > R3D_FORCEINLINE int SerializeXMLVal( const char* Name, pugi::xml_node& node, r3dSTLString* ioVal );
template < bool Write > R3D_FORCEINLINE int SerializeXMLVal( const char* Name, pugi::xml_node& node, r3dString* ioVal );
template < bool Write > R3D_FORCEINLINE int SerializeXMLVal( const char* Name, pugi::xml_node& node, char* ioVal );
template < bool Write > R3D_FORCEINLINE int SerializeXMLVal( const char* Name, pugi::xml_node& node, r3dColor* ioVal );
template < bool Write > R3D_FORCEINLINE int SerializeXMLVal( const char* Name, pugi::xml_node& node, r3dPoint3D* ioVal );

template <> R3D_FORCEINLINE int SerializeXMLVal< true > ( const char* Name, pugi::xml_node& node, float* ioVal ){ return SetXMLVal( Name, node, ioVal );	}
template <> R3D_FORCEINLINE int SerializeXMLVal< true > ( const char* Name, pugi::xml_node& node, int* ioVal ){ return SetXMLVal( Name, node, ioVal ); }
template <> R3D_FORCEINLINE int SerializeXMLVal< true > ( const char* Name, pugi::xml_node& node, r3dSTLString* ioVal ){ return SetXMLVal( Name, node, ioVal ); }
template <> R3D_FORCEINLINE int SerializeXMLVal< true > ( const char* Name, pugi::xml_node& node, r3dString* ioVal ){ return SetXMLVal( Name, node, ioVal ); }
template <> R3D_FORCEINLINE int SerializeXMLVal< true > ( const char* Name, pugi::xml_node& node, char* ioVal ){ return SetXMLVal( Name, node, ioVal ); }
template <> R3D_FORCEINLINE int SerializeXMLVal< true > ( const char* Name, pugi::xml_node& node, r3dColor* ioVal ) { return SetXMLVal( Name, node, ioVal ); }
template <> R3D_FORCEINLINE int SerializeXMLVal< true > ( const char* Name, pugi::xml_node& node, r3dPoint3D* ioVal ) { return SetXMLVal( Name, node, ioVal ); }

template <> R3D_FORCEINLINE int SerializeXMLVal< false > ( const char* Name, pugi::xml_node& node, float* ioVal ){ return GetXMLVal( Name, node, ioVal );	}
template <> R3D_FORCEINLINE int SerializeXMLVal< false > ( const char* Name, pugi::xml_node& node, int* ioVal ){ return GetXMLVal( Name, node, ioVal ); }
template <> R3D_FORCEINLINE int SerializeXMLVal< false > ( const char* Name, pugi::xml_node& node, r3dSTLString* ioVal ){ return GetXMLVal( Name, node, ioVal ); }
template <> R3D_FORCEINLINE int SerializeXMLVal< false > ( const char* Name, pugi::xml_node& node, r3dString* ioVal ){ return GetXMLVal( Name, node, ioVal ); }
template <> R3D_FORCEINLINE int SerializeXMLVal< false > ( const char* Name, pugi::xml_node& node, char* ioVal ){ return GetXMLVal( Name, node, ioVal ); }
template <> R3D_FORCEINLINE int SerializeXMLVal< false > ( const char* Name, pugi::xml_node& node, r3dColor* ioVal ){ return GetXMLVal( Name, node, ioVal ); }
template <> R3D_FORCEINLINE int SerializeXMLVal< false > ( const char* Name, pugi::xml_node& node, r3dPoint3D* ioVal ){ return GetXMLVal( Name, node, ioVal ); }

template < bool Write > R3D_FORCEINLINE int SerializeXMLCurve3f( const char* Name, pugi::xml_node& node, class r3dTimeGradient2* ioTimeGradient );
template < bool Write > R3D_FORCEINLINE int SerializeXMLCurve3f( const char* Name, pugi::xml_node& node, class r3dBezierGradient* ioBezGradient );

template <> R3D_FORCEINLINE int SerializeXMLCurve3f< true >( const char* Name, pugi::xml_node& node, class r3dTimeGradient2* ioTimeGradient ) { return SetXMLCurve3f( Name, node, ioTimeGradient ); } ;
template <> R3D_FORCEINLINE int SerializeXMLCurve3f< false >( const char* Name, pugi::xml_node& node, class r3dTimeGradient2* ioTimeGradient ) { return GetXMLCurve3f( Name, node, ioTimeGradient ); } ;

template <> R3D_FORCEINLINE int SerializeXMLCurve3f< true >( const char* Name, pugi::xml_node& node, class r3dBezierGradient* ioBezGradient ) { return SetXMLCurve3f( Name, node, ioBezGradient ); } ;
template <> R3D_FORCEINLINE int SerializeXMLCurve3f< false >( const char* Name, pugi::xml_node& node, class r3dBezierGradient* ioBezGradient ) { return GetXMLCurve3f( Name, node, ioBezGradient ); } ;

template < bool Write > R3D_FORCEINLINE int SerializeXMLCmdVarF( const char* Name, pugi::xml_node& node, CmdVar * var );
template < bool Write > R3D_FORCEINLINE int SerializeXMLCmdVarI( const char* Name, pugi::xml_node& node, CmdVar * var );

template <> R3D_FORCEINLINE int SerializeXMLCmdVarF<true>( const char* Name, pugi::xml_node& node, CmdVar * var ) { return SetXMLCmdVarF( Name, node, var ); };
template <> R3D_FORCEINLINE int SerializeXMLCmdVarI<true>( const char* Name, pugi::xml_node& node, CmdVar * var ) { return SetXMLCmdVarI( Name, node, var ); };
	
template <> R3D_FORCEINLINE int SerializeXMLCmdVarF<false>( const char* Name, pugi::xml_node& node, CmdVar * var ) { return GetXMLCmdVarF( Name, node, var ); };
template <> R3D_FORCEINLINE int SerializeXMLCmdVarI<false>( const char* Name, pugi::xml_node& node, CmdVar * var ) { return GetXMLCmdVarI( Name, node, var ); };

template <bool Write> pugi::xml_node SerializeXMLNode( pugi::xml_node& parent, const char* Name ) ;

typedef r3dTL::TArray< char > Bytes ;

int ParseXMLInMemory( r3dFile* f,  Bytes * xmlFileBuffer, pugi::xml_document* doc );
bool LoadAndParseXMLFile(const char *filePath, pugi::xml_document &outDoc, Bytes &xmlFileBuffer);