#include "r3dPCH.h"
#include "r3d.h"

#include "TimeGradient.h"
#include "BezierGradient.h"

#include "XMLHelpers.h"

int GetXMLVal( const char* Name, pugi::xml_node& node, float* oVal )
{
	if( node.empty() )
		return  0;

	pugi::xml_node valNode = node.child( Name ) ;
	if( valNode.empty() )
		return 0 ;

	pugi::xml_attribute valAtt = valNode.attribute( "val" );

	if( valAtt.empty() )
		return 0 ;

	*oVal = valAtt.as_float() ;

	return 1 ;
}

//------------------------------------------------------------------------

int GetXMLVal( const char* Name, pugi::xml_node& node, int* oVal )
{
	if( node.empty() )
		return  0;

	pugi::xml_node valNode = node.child( Name ) ;
	if( valNode.empty() )
		return 0 ;

	pugi::xml_attribute valAtt = valNode.attribute( "val" );

	if( valAtt.empty() )
		return 0 ;

	*oVal = valAtt.as_int() ;

	return 1 ;
}

//------------------------------------------------------------------------

int GetXMLVal( const char* Name, pugi::xml_node& node, unsigned int* oVal )
{
	if( node.empty() )
		return  0;

	pugi::xml_node valNode = node.child( Name ) ;
	if( valNode.empty() )
		return 0 ;

	pugi::xml_attribute valAtt = valNode.attribute( "val" );

	if( valAtt.empty() )
		return 0 ;

	*oVal = valAtt.as_uint() ;

	return 1 ;
}

//------------------------------------------------------------------------

int GetXMLVal( const char* Name, pugi::xml_node& node, r3dSTLString* oVal )
{
	if( node.empty() )
		return  0;

	pugi::xml_node valNode = node.child( Name ) ;
	if( valNode.empty() )
		return 0 ;

	pugi::xml_attribute valAtt = valNode.attribute( "val" );

	if( valAtt.empty() )
		return 0 ;

	*oVal = valAtt.value() ;

	return 1 ;
}

//------------------------------------------------------------------------

int GetXMLVal( const char* Name, pugi::xml_node& node, r3dString* oVal )
{
	if( node.empty() )
		return  0;

	pugi::xml_node valNode = node.child( Name ) ;
	if( valNode.empty() )
		return 0 ;

	pugi::xml_attribute valAtt = valNode.attribute( "val" );

	if( valAtt.empty() )
		return 0 ;

	*oVal = valAtt.value() ;

	return 1 ;
}

//------------------------------------------------------------------------

int GetXMLVal( const char* Name, pugi::xml_node& node, char* oVal )
{
	if( node.empty() )
		return  0;

	pugi::xml_node valNode = node.child( Name ) ;
	if( valNode.empty() )
		return 0 ;

	pugi::xml_attribute valAtt = valNode.attribute( "val" );

	if( valAtt.empty() )
		return 0 ;

	strcpy( oVal, valAtt.value() );

	return 1 ;
}

//------------------------------------------------------------------------

int GetXMLVal( const char* Name, pugi::xml_node& node, r3dColor* oVal )
{
	if( node.empty() )
		return  0;

	pugi::xml_node valNode = node.child( Name ) ;
	if( valNode.empty() )
		return 0 ;

	pugi::xml_attribute rAtt = valNode.attribute( "r" );
	if( rAtt.empty() )
		return 0 ;

	pugi::xml_attribute gAtt = valNode.attribute( "g" );
	if( gAtt.empty() )
		return 0 ;

	pugi::xml_attribute bAtt = valNode.attribute( "b" );
	if( bAtt.empty() )
		return 0 ;

	pugi::xml_attribute aAtt = valNode.attribute( "a" );
	if( aAtt.empty() )
		return 0 ;

	oVal->R = rAtt.as_int() ;
	oVal->G = gAtt.as_int() ;
	oVal->B = bAtt.as_int() ;
	oVal->A = aAtt.as_int() ;

	return 1 ;

}

//------------------------------------------------------------------------

int GetXMLVal( const char* Name, pugi::xml_node& node, r3dPoint3D* oVal )
{
	if( node.empty() )
		return  0;

	pugi::xml_node valNode = node.child( Name ) ;
	if( valNode.empty() )
		return 0 ;

	pugi::xml_attribute xAtt = valNode.attribute( "x" );
	if( xAtt.empty() )
		return 0 ;

	pugi::xml_attribute yAtt = valNode.attribute( "y" );
	if( yAtt.empty() )
		return 0 ;

	pugi::xml_attribute zAtt = valNode.attribute( "z" );
	if( zAtt.empty() )
		return 0 ;

	oVal->x = xAtt.as_float() ;
	oVal->y = yAtt.as_float() ;
	oVal->z = zAtt.as_float() ;

	return 1 ;
}

//------------------------------------------------------------------------

int GetXMLVal( const char* Name, pugi::xml_node& node, r3dPoint2D* oVal )
{
	if( node.empty() )
		return  0;

	pugi::xml_node valNode = node.child( Name ) ;
	if( valNode.empty() )
		return 0 ;

	pugi::xml_attribute xAtt = valNode.attribute( "x" );
	if( xAtt.empty() )
		return 0 ;

	pugi::xml_attribute yAtt = valNode.attribute( "y" );
	if( yAtt.empty() )
		return 0 ;

	oVal->x = xAtt.as_float() ;
	oVal->y = yAtt.as_float() ;

	return 1 ;
}

//------------------------------------------------------------------------

int GetXMLCurve3f( const char* Name, pugi::xml_node& node, r3dTimeGradient2* oTimeGradient )
{
	if( node.empty() )
		return  0;

	pugi::xml_node curveNode = node.child( Name );

	if( curveNode.empty() )
		return 0 ;

	oTimeGradient->Reset() ;

	oTimeGradient->Smooth = curveNode.attribute( "smooth" ).as_int() ? 1 : 0 ;

	pugi::xml_node valNode = curveNode.child( "val" );

	for( int i = 0, e = r3dTimeGradient2::MAX_VALUES ; i < e && !valNode.empty() ; i ++, valNode = valNode.next_sibling() )
	{
		float t =  valNode.attribute( "t" ).as_float() ;
		float x =  valNode.attribute( "x" ).as_float() ;
		float y =  valNode.attribute( "y" ).as_float() ;
		float z =  valNode.attribute( "z" ).as_float() ;

		oTimeGradient->AddValue3f( t, x, y, z );
	}

	return 1 ;
}

//------------------------------------------------------------------------

int GetXMLCurve3f( const char* Name, pugi::xml_node& node, class r3dBezierGradient* oBezGradient )
{
	oBezGradient->Reset( ) ;

	if( node.empty() )
		return  0;

	pugi::xml_node curveNode = node.child( Name );

	if( curveNode.empty() )
		return 0 ;

	pugi::xml_node valNode = curveNode.child( "val" );

	for( int i = 0, e = r3dTimeGradient2::MAX_VALUES ; i < e && !valNode.empty() ; i ++, valNode = valNode.next_sibling() )
	{
		float t =  valNode.attribute( "t" ).as_float() ;
		float x =  valNode.attribute( "x" ).as_float() ;
		float y =  valNode.attribute( "y" ).as_float() ;
		float z =  valNode.attribute( "z" ).as_float() ;

		oBezGradient->AddValue3f( t, x, y, z );
	}

	return 1 ;
}

//------------------------------------------------------------------------

int SetXMLVal( const char* Name, pugi::xml_node& node, const float* val )
{
	pugi::xml_node valNode = node.append_child();
	
	valNode.set_name( Name ) ;
	valNode.append_attribute( "val" ) = *val ;

	return 1 ;
}

//------------------------------------------------------------------------

int SetXMLVal( const char* Name, pugi::xml_node& node, const int* val )
{
	pugi::xml_node valNode = node.append_child();

	valNode.set_name( Name ) ;
	valNode.append_attribute( "val" ) = *val ;

	return 1 ;
}

//------------------------------------------------------------------------

int SetXMLVal( const char* Name, pugi::xml_node& node, const unsigned int* val )
{
	pugi::xml_node valNode = node.append_child();

	valNode.set_name( Name ) ;
	valNode.append_attribute( "val" ) = *val ;

	return 1 ;
}

//------------------------------------------------------------------------

int SetXMLVal( const char* Name, pugi::xml_node& node, const r3dSTLString* val )
{
	pugi::xml_node valNode = node.append_child();

	valNode.set_name( Name ) ;
	valNode.append_attribute( "val" ) = val->c_str() ;

	return 1 ;
}

//------------------------------------------------------------------------

int SetXMLVal( const char* Name, pugi::xml_node& node, const r3dString* val )
{
	pugi::xml_node valNode = node.append_child();

	valNode.set_name( Name ) ;
	valNode.append_attribute( "val" ) = val->c_str() ;

	return 1 ;
}

//------------------------------------------------------------------------

int SetXMLVal( const char* Name, pugi::xml_node& node, const char* val )
{
	pugi::xml_node valNode = node.append_child();

	valNode.set_name( Name ) ;
	valNode.append_attribute( "val" ) = val ;

	return 1 ;
}

//------------------------------------------------------------------------

int SetXMLVal( const char* Name, pugi::xml_node& node, const r3dColor* val )
{
	pugi::xml_node valNode = node.append_child();

	valNode.set_name( Name ) ;
	valNode.append_attribute( "r" ) = (int)val->R ;
	valNode.append_attribute( "g" ) = (int)val->G ;
	valNode.append_attribute( "b" ) = (int)val->B ;
	valNode.append_attribute( "a" ) = (int)val->A ;

	return 1 ;
}

//------------------------------------------------------------------------

int SetXMLVal( const char* Name, pugi::xml_node& node, const r3dPoint3D* val )
{
	pugi::xml_node valNode = node.append_child();

	valNode.set_name( Name ) ;
	valNode.append_attribute( "x" ) = val->x ;
	valNode.append_attribute( "y" ) = val->y ;
	valNode.append_attribute( "z" ) = val->z ;

	return 1 ;
}

//------------------------------------------------------------------------

int SetXMLVal( const char* Name, pugi::xml_node& node, const r3dPoint2D* val )
{
	pugi::xml_node valNode = node.append_child();

	valNode.set_name( Name ) ;
	valNode.append_attribute( "x" ) = val->x ;
	valNode.append_attribute( "y" ) = val->y ;

	return 1 ;
}

//------------------------------------------------------------------------

int SetXMLCurve3f( const char* Name, pugi::xml_node& node, r3dTimeGradient2* oTimeGradient )
{
	pugi::xml_node curveNode = node.append_child();
	curveNode.set_name( Name );

	curveNode.append_attribute( "smooth" ) = (int)oTimeGradient->Smooth ;

	for( int i = 0, e = oTimeGradient->NumValues ; i < e ; i ++ )
	{
		pugi::xml_node valNode = curveNode.append_child( );

		const r3dTimeGradient2::val_s& v = oTimeGradient->Values[ i ] ;

		valNode.set_name( "val" );

		valNode.append_attribute( "t" ) = v.time ;
		valNode.append_attribute( "x" ) = v.val[ 0 ] ;
		valNode.append_attribute( "y" ) = v.val[ 1 ] ;
		valNode.append_attribute( "z" ) = v.val[ 2 ] ;
	}

	return 1 ;
}

//------------------------------------------------------------------------

int SetXMLCurve3f( const char* Name, pugi::xml_node& node, class r3dBezierGradient* bezGradient )
{
	pugi::xml_node curveNode = node.append_child();
	curveNode.set_name( Name );

	for( int i = 0, e = bezGradient->NumValues ; i < e ; i ++ )
	{
		pugi::xml_node valNode = curveNode.append_child( );

		const r3dBezierGradient::val_s& v = bezGradient->Values[ i ] ;

		valNode.set_name( "val" );

		valNode.append_attribute( "t" ) = v.time ;
		valNode.append_attribute( "x" ) = v.val[ 0 ] ;
		valNode.append_attribute( "y" ) = v.val[ 1 ] ;
		valNode.append_attribute( "z" ) = v.val[ 2 ] ;
	}

	return 1 ;
}

//------------------------------------------------------------------------

template <> pugi::xml_node SerializeXMLNode< true > ( pugi::xml_node& parent, const char* Name )
{
	pugi::xml_node child = parent.append_child( ) ;

	child.set_name( Name );

	return child ;
}

//------------------------------------------------------------------------

template <> pugi::xml_node SerializeXMLNode< false > ( pugi::xml_node& parent, const char* Name )
{
	return parent.child( Name ) ;
}

//------------------------------------------------------------------------

int GetXMLCmdVarF( const char* Name, pugi::xml_node& node, CmdVar * oVar )
{
	if( node.empty() )
		return  0;

	pugi::xml_node valNode = node.child( Name ) ;
	if( valNode.empty() )
		return 0 ;

	pugi::xml_attribute valAtt = valNode.attribute( "val" );

	if( valAtt.empty() )
		return 0 ;

	oVar->SetFloat( valAtt.as_float() );

	return 1 ;
}

//------------------------------------------------------------------------

int GetXMLCmdVarI( const char* Name, pugi::xml_node& node, CmdVar * oVar )
{
	if( node.empty() )
		return  0;

	pugi::xml_node valNode = node.child( Name ) ;
	if( valNode.empty() )
		return 0 ;

	pugi::xml_attribute valAtt = valNode.attribute( "val" );

	if( valAtt.empty() )
		return 0 ;

	oVar->SetInt( valAtt.as_int() );

	return 1 ;
}

//------------------------------------------------------------------------

int SetXMLCmdVarF( const char* Name, pugi::xml_node& node, CmdVar * var )
{
	pugi::xml_node valNode = node.append_child();

	valNode.set_name( Name ) ;
	valNode.append_attribute( "val" ) = var->GetFloat() ;

	return 1 ;

}

//------------------------------------------------------------------------

int SetXMLCmdVarI( const char* Name, pugi::xml_node& node, CmdVar * var )
{

	pugi::xml_node valNode = node.append_child();

	valNode.set_name( Name ) ;
	valNode.append_attribute( "val" ) = var->GetInt() ;

	return 1 ;
}

//------------------------------------------------------------------------

int ParseXMLInMemory( r3dFile* f,  Bytes * xmlFileBuffer, pugi::xml_document* doc )
{
	xmlFileBuffer->Resize( f->size + 1 );

	fread( &(*xmlFileBuffer)[ 0 ], f->size, 1, f );
	(*xmlFileBuffer)[ f->size ] = 0;

	pugi::xml_parse_result parseResult = doc->load_buffer_inplace( &(*xmlFileBuffer)[0], f->size );

	if( !parseResult )
	{
		r3dError( "LoadLevel: Failed to parse %s, error: %s", f->GetFileName(), parseResult.description() );
		return 0;
	}

	return 1;
}

//------------------------------------------------------------------------

bool LoadAndParseXMLFile(const char *filePath, pugi::xml_document &outDoc, Bytes &xmlFileBuffer)
{
	if (!filePath)
		return false;

	r3dFile* f = r3d_open(filePath, "rb");
	if (!f)
	{
		r3dOutToLog("Failed to load file: '%s'", filePath);
		return false;
	}
	r3dOutToLog("Loading '%s'\n", filePath) ;
	bool parseResult = ParseXMLInMemory(f, &xmlFileBuffer, &outDoc) != 0;
	fclose(f);

	return parseResult;
}