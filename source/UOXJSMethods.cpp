//o--------------------------------------------------------------------------o
//|	File					-	UOXJSMethods.cpp
//|	Date					-	
//|	Developers		-	DarkStorm
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Version History
//|									1.0		Dark-Storm	20th December, 2001 Initial implementation
//|									Methods for the javascript objects
//|									
//|									List of supported Classes:
//|										- Gumps
//|										- Char
//|										- Item
//|										- Socket
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
#include "uox3.h"
#include "UOXJSPropertySpecs.h"
#include "SEFunctions.h"
#include "UOXJSClasses.h"
#include "UOXJSMethods.h"
#include "JSEncapsulate.h"

#include "cMagic.h"
#include "cGuild.h"
#include "skills.h"
#include "targeting.h"
#include "speech.h"
#include "gump.h"
#include "trigger.h"
#include "cScript.h"
#include "cEffects.h"
#include "teffect.h"
#include "network.h"
#include "classes.h"
#include "cRaces.h"
#include "cServerDefinitions.h"
#include "commands.h"
#include "movement.h"
#include "wholist.h"
#include "townregion.h"
#include "Dictionary.h"

namespace UOX
{

extern cCommands *Commands;
#ifndef va_start
	#include <cstdarg>
#endif

void SpawnGate( CChar *caster, SI16 srcX, SI16 srcY, SI08 srcZ, SI16 trgX, SI16 trgY, SI08 trgZ );

void MethodError( char *txt, ... )
{
	va_list argptr;
	char msg[512];
	va_start( argptr, txt );
	vsprintf( msg, txt, argptr );
	va_end( argptr );
	Console.Error( 2, msg );
}

//
//	Copied that here from SEFunctions.cpp
//	Default paramters weren't working !?
//
void MethodSpeech( cBaseObject &speaker, char *message, SpeechType sType, COLOUR sColour = 0x005A, FontType fType = FNT_NORMAL, SpeechTarget spTrg = SPTRG_PCNPC )
{
	CSpeechEntry *toAdd = SpeechSys->Add();
	toAdd->Font( fType );
	toAdd->Speech( message );
	toAdd->Speaker( speaker.GetSerial() );
	toAdd->Type( sType );
	toAdd->TargType( spTrg );

	// Added that because transparent text could cause trouble
	if( sColour != 0x1700 )
		toAdd->Colour( sColour );
	else
		toAdd->Colour( 0x0058 );

	toAdd->At( cwmWorldState->GetUICurrentTime() );
}

// JSGump constructor !
JSBool Gump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	// Allocate the GumpList here and "SetPrivate" it to the Object
	SEGump *toAdd = new SEGump;
	toAdd->one = new STRINGLIST;
	toAdd->two = new STRINGLIST;
	toAdd->TextID = 0;

	JS_SetPrivate(cx, obj, toAdd);
	return JS_TRUE;
}

// Gump related functions
JSBool CGump_Free( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	SEGump *toDelete = (SEGump*)JS_GetPrivate( cx, obj );

	if( toDelete == NULL )  
		return JS_FALSE;
	
	delete toDelete->one;
	delete toDelete->two;
	delete toDelete;

	return JS_TRUE;
}
//
JSBool CGumpData_Free(JSContext *cx, JSObject *obj,uintN argc, jsval *argv, jsval *rval)
{
	SEGumpData *toDelete = (SEGumpData*)JS_GetPrivate( cx, obj );

	if( toDelete == NULL )  
		return JS_FALSE;
	JS_SetPrivate( cx, obj, NULL ) ;
	delete toDelete;
	return JS_TRUE;
}
//
JSBool CGumpData_getEdit( JSContext *cx, JSObject *obj, uintN argc,jsval *argv, jsval *rval )
{
	if( argc == 0 )
	{
		MethodError( "(GumpData_getID) Invalid Number of Arguments %d, needs: 1 ", argc );
		*rval = STRING_TO_JSVAL("");
		return JS_TRUE;
	}
	
	SEGumpData *myItem = (SEGumpData *)JS_GetPrivate( cx, obj );
	
	if( myItem == NULL  )
	{
		MethodError( "(DataGump-getID) Invalid object assigned");
		*rval = STRING_TO_JSVAL("") ;
		return JS_TRUE;
	}
	;
	unsigned int index = JSVAL_TO_INT(argv[0]) ; 
	
	if( index < myItem->sEdits.size() )
		*rval = STRING_TO_JSVAL(JS_NewStringCopyZ( cx,myItem->sEdits[index].c_str()));
	else
		*rval = STRING_TO_JSVAL("") ;
	return JS_TRUE;
}
//
JSBool CGumpData_getID( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc == 0 )
	{
		MethodError( "(GumpData_getID) Invalid Number of Arguments %d, needs: 1 ", argc );
		*rval = INT_TO_JSVAL(-1);
		return JS_TRUE;
	}
	
	SEGumpData *myItem = (SEGumpData *)JS_GetPrivate( cx, obj );
	
	if( myItem == NULL  )
	{
		MethodError( "(DataGump-getID) Invalid object assigned");
		*rval = INT_TO_JSVAL(-1);
		return JS_TRUE;
	}
	unsigned int index = JSVAL_TO_INT(argv[0]) ; 
	
	if( index < myItem->nIDs.size() )
		*rval = INT_TO_JSVAL(myItem->nIDs[index]);
	else
		*rval = INT_TO_JSVAL(-1) ;
	
	return JS_TRUE;
}
//
JSBool CGumpData_getButton( JSContext *cx, JSObject *obj, uintN argc,jsval *argv, jsval *rval )
{
	if( argc == 0 )
	{
		MethodError( "(GumpData_getbutton) Invalid Number of Arguments %d, needs: 1 ", argc );
		*rval = INT_TO_JSVAL(-1);
		return JS_TRUE;
	}
	
	SEGumpData *myItem = (SEGumpData *)JS_GetPrivate( cx, obj );
	
	if( myItem == NULL  )
	{
		MethodError( "(DataGump-getID) Invalid object assigned");
		*rval = INT_TO_JSVAL(-1);
		return JS_TRUE;
	}
	unsigned int index = JSVAL_TO_INT(argv[0]) ; 
	if( index < myItem->nButtons.size())
		*rval = INT_TO_JSVAL(myItem->nButtons[index]);
	else
		*rval = INT_TO_JSVAL(-1) ;
	
	return JS_TRUE;
}
//

JSBool CGump_AddCheckbox( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 5 )
	{
		MethodError("AddCheckbox: Wrong count of Parameters, needs 5");
		return JS_FALSE;
	}

	SI16 tL			= (SI16)JSVAL_TO_INT( argv[0] );
	SI16 tR			= (SI16)JSVAL_TO_INT( argv[1] );
	UI16 gImage		= (UI16)JSVAL_TO_INT( argv[2] );
	SI16 initState	= (SI16)JSVAL_TO_INT( argv[3] );
	SI16 relay		= (SI16)JSVAL_TO_INT( argv[4] );


	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	
	if( gList == NULL )
	{
		MethodError( "Gump_AddCheckbox: Couldn't find gump associated with object");
		return JS_FALSE;
	}

	char temp[256];
	sprintf( temp, "checkbox %i %i %i %i %i %i", tL, tR, gImage, gImage + 1, initState, relay );
	gList->one->push_back( temp );
	
	return JS_TRUE;
}

JSBool CGump_NoClose( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError("NoClose: Wrong count of Parameters, needs 0");
		return JS_FALSE;
	}

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	
	if( gList == NULL )
	{
		MethodError( "NoClose: Couldn't find gump associated with object");
		return JS_FALSE;
	}

	gList->one->push_back( "noclose" );
	
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CGump_NoMove( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	9th February, 2003
//|	Developer(s)	-	Xuri
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Allows you to specify nomove on gumps created
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
JSBool CGump_NoMove( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "NoMove: Wrong count of Parameters, needs 0" );
		return JS_FALSE;
	}

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	
	if( gList == NULL )
	{
		MethodError( "NoMove: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( "nomove" );
	
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CGump_NoDispose( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	14th February, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Allows you to specify nodispose on create
//| Comments		-	What nodispose is, I have no idea what so ever
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
JSBool CGump_NoDispose( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "NoDispose: Wrong count of Parameters, needs 0" );
		return JS_FALSE;
	}

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	
	if( gList == NULL )
	{
		MethodError( "NoDispose: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( "nodispose" );
	
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CGump_NoResize( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	14th February, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Allows you to specify noresize on create
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
JSBool CGump_NoResize( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "NoResize: Wrong count of Parameters, needs 0" );
		return JS_FALSE;
	}

	SEGump *gList = (SEGump *)JS_GetPrivate( cx, obj );
	
	if( gList == NULL )
	{
		MethodError( "NoResize: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	gList->one->push_back( "noresize" );
	
	return JS_TRUE;
}

JSBool CGump_AddBackground( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 5 )
	{
		MethodError( "AddBackground: Invalid number of arguments (takes 5)" );
		return JS_FALSE;
	}

	SI16 tL = (SI16)JSVAL_TO_INT( argv[0] );
	SI16 tR = (SI16)JSVAL_TO_INT( argv[1] );
	SI16 bL = (SI16)JSVAL_TO_INT( argv[2] );
	SI16 bR = (SI16)JSVAL_TO_INT( argv[3] );
	UI16 gImage = (UI16)JSVAL_TO_INT( argv[4] );

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddBackground: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	char temp[256];
	sprintf( temp, "resizepic %i %i %i %i %i", tL, tR, gImage, bL, bR );
	gList->one->push_back( temp );

	return JS_TRUE;
}

JSBool CGump_AddButton( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 6 )
	{
		MethodError( "AddButton: Invalid number of arguments (takes 6)" );
		return JS_FALSE;
	}
	SI16 tL = (SI16)JSVAL_TO_INT( argv[0] );
	SI16 tR = (SI16)JSVAL_TO_INT( argv[1] );
	UI16 gImage = (UI16)JSVAL_TO_INT( argv[2] );
	SI16 x1 = (SI16)JSVAL_TO_INT( argv[3] );
	SI16 x2 = (SI16)JSVAL_TO_INT( argv[4] );
	SI16 x3 = (SI16)JSVAL_TO_INT( argv[5] );

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddButton: Couldn't find gump associated with object");
		return JS_FALSE;
	}
	char temp[256];
	sprintf( temp, "button %i %i %i %i %i %i %i", tL, tR, gImage, gImage + 1, x1, x2, x3 );
	gList->one->push_back( temp );

	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CGump_AddCheckerTrans( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	14th February, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Allows you to tile a particular gump image
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
JSBool CGump_AddCheckerTrans( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 )
	{
		MethodError( "AddCheckerTrans: Invalid number of arguments (takes 5 x y width height)" );
		return JS_FALSE;
	}

	SI16 x		= (SI16)JSVAL_TO_INT( argv[0] ); // x
	SI16 y		= (SI16)JSVAL_TO_INT( argv[1] ); // y
	SI16 width	= (SI16)JSVAL_TO_INT( argv[2] ); // width
	SI16 height	= (SI16)JSVAL_TO_INT( argv[3] ); // height

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddCheckerTrans: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	char temp[256];
	sprintf( temp, "checkertrans %i %i %i %i", x, y, width, height );
	gList->one->push_back( temp );

	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CGump_AddCroppedText( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	14th February, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Allows you to specify a text field cropped to certain
//|						dimensions
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
JSBool CGump_AddCroppedText( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 6 )
	{
		MethodError( "AddCroppedText: Invalid number of arguments (takes 6 x y hue width height text)" );
		return JS_FALSE;
	}

	SI16 TextX		= (SI16)JSVAL_TO_INT( argv[0] ); // x
	SI16 TextY		= (SI16)JSVAL_TO_INT( argv[1] ); // y
	SI16 TextHue	= (SI16)JSVAL_TO_INT( argv[2] ); // Hue
	SI16 TextWidth	= (SI16)JSVAL_TO_INT( argv[3] ); // width
	SI16 TextHeight	= (SI16)JSVAL_TO_INT( argv[4] ); // height

 	char *TextString = JS_GetStringBytes( JS_ValueToString( cx, argv[5] ) );

	if( TextString == NULL )
	{
		MethodError( "AddCroppedText: Text is required" );
		return JS_FALSE;
	}

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddCroppedText: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	UI32 TextID = gList->TextID;
	++gList->TextID;
	
	char temp[256];
	sprintf( temp, "croppedtext %i %i %i %i %i %i", TextX, TextY, TextWidth, TextHeight, TextHue, TextID );
	gList->one->push_back( temp );
	gList->two->push_back( TextString );

	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CGump_AddGroup( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	14th February, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Allows you to specify a new group to work with
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
JSBool CGump_AddGroup( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "AddGroup: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddGroup: Couldn't find gump associated with object" );
		return JS_FALSE;
	}
	char temp[256];
	sprintf( temp, "group %i", JSVAL_TO_INT( argv[0] ) );
	gList->one->push_back( temp );

	return JS_TRUE;
}

JSBool CGump_AddGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		MethodError( "AddGump: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}
	
	SI16 tL = (SI16)JSVAL_TO_INT( argv[0] );
	SI16 tR = (SI16)JSVAL_TO_INT( argv[1] );
	UI16 gImage = (UI16)JSVAL_TO_INT( argv[2] );
	
	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddGump: Couldn't find gump associated with object" );
		return JS_FALSE;
	}
	char temp[256];
	sprintf( temp, "gumppic %i %i %i", tL, tR, gImage );
	gList->one->push_back( temp );

	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CGump_AddHTMLGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	16th February, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Adds a new HTML gump to the stream of the gump
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
JSBool CGump_AddHTMLGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 7 )
	{
		MethodError( "AddHTMLGump: Invalid number of arguments (takes 7)" );
		return JS_FALSE;
	}

	SI16 x				= (SI16)JSVAL_TO_INT( argv[0] ); // x
	SI16 y				= (SI16)JSVAL_TO_INT( argv[1] ); // y
	SI16 width			= (SI16)JSVAL_TO_INT( argv[2] ); // width
	SI16 height			= (SI16)JSVAL_TO_INT( argv[3] ); // height
	bool hasBorder		= ( JSVAL_TO_BOOLEAN( argv[4] ) == JS_TRUE );
	bool hasScrollbar	= ( JSVAL_TO_BOOLEAN( argv[5] ) == JS_TRUE );

 	char *TextString = JS_GetStringBytes( JS_ValueToString( cx, argv[6] ) );

	if( TextString == NULL )
	{
		MethodError( "AddHTMLGump: Text is required" );
		return JS_FALSE;
	}

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddHTMLGump: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	UI32 TextID = gList->TextID;
	++gList->TextID;
	
	SI32 iBrd	= (hasBorder?1:0);
	SI32 iScrl	= (hasScrollbar?1:0);
	char temp[256];
	sprintf( temp, "htmlgump %i %i %i %i %i %i %i", x, y, width, height, TextID, iBrd, iScrl );
	gList->one->push_back( temp );
	gList->two->push_back( TextString );

	return JS_TRUE;
}

JSBool CGump_AddPage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "AddPage: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddPage: Couldn't find gump associated with object");
		return JS_FALSE;
	}
	char temp[256];
	sprintf( temp, "page %i", JSVAL_TO_INT( argv[0] ) );
	gList->one->push_back( temp );

	return JS_TRUE;
}
JSBool CGump_AddPicture( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		MethodError( "AddPicture: Invalid number of arguments (takes 3)" );
		return JS_FALSE;
	}

	SI16 tL = (SI16)JSVAL_TO_INT( argv[0] );
	SI16 tR = (SI16)JSVAL_TO_INT( argv[1] );
	UI16 gImage = (UI16)JSVAL_TO_INT( argv[2] );

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddPicture: Couldn't find gump associated with object");
		return JS_FALSE;
	}
	char temp[256];
	sprintf( temp, "tilepic %i %i %i", tL, tR, gImage );
	gList->one->push_back( temp );

	return JS_TRUE;
}
JSBool CGump_AddRadio( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 5 )
	{
		MethodError( "Gump_AddRadio: Invalid number of arguments (takes 5)" );
		return JS_FALSE;
	}

	SI16 tL				= (SI16)JSVAL_TO_INT( argv[0] );
	SI16 tR				= (SI16)JSVAL_TO_INT( argv[1] );
	UI16 gImage			= (UI16)JSVAL_TO_INT( argv[2] );
	SI16 initialState	= (SI16)JSVAL_TO_INT( argv[3] );
	SI16 relay			= (SI16)JSVAL_TO_INT( argv[4] );

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddRadio: Couldn't find gump associated with object");
		return JS_FALSE;
	}
	char temp[256];
	sprintf( temp, "radio %i %i %i %i %i %i", tL, tR, gImage, gImage + 1, initialState, relay );
	gList->one->push_back( temp );

	return JS_TRUE;
}
JSBool CGump_AddText( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 )
	{
		MethodError( "AddText: Invalid number of arguments (takes 4)" );
		return JS_FALSE;
	}

	UI32 TextID;
	
	SI16 TextX = (SI16)JSVAL_TO_INT( argv[0] ); // x
	SI16 TextY = (SI16)JSVAL_TO_INT( argv[1] ); // y
	SI16 TextHue = (SI16)JSVAL_TO_INT( argv[2] ); // Hue

 	char *TextString = JS_GetStringBytes( JS_ValueToString( cx, argv[3] ) );

	if( TextString == NULL )
	{
		MethodError( "AddText: Text is required" );
		return JS_FALSE;
	}

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddText: Couldn't find gump associated with object");
		return JS_FALSE;
	}

	TextID = gList->TextID;
	++gList->TextID;
	
	char temp[256];
	sprintf( temp, "text %i %i %i %i", TextX, TextY, TextHue, TextID );
	gList->one->push_back( temp );
	gList->two->push_back( TextString );

	return JS_TRUE;
}

JSBool CGump_AddTextEntry( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 8 )
	{
		MethodError( "AddTextEntry: Invalid number of arguments (takes 8)" );
		return JS_FALSE;
	}
	
	SI16 tL					= (SI16)JSVAL_TO_INT( argv[0] );
	SI16 tR					= (SI16)JSVAL_TO_INT( argv[1] );
	SI16 width				= (SI16)JSVAL_TO_INT( argv[2] );
	SI16 height				= (SI16)JSVAL_TO_INT( argv[3] );
	SI16 hue				= (SI16)JSVAL_TO_INT( argv[4] );
	SI16 relay				= (SI16)JSVAL_TO_INT( argv[5] );
	SI16 initialTextIndex	= (SI16)JSVAL_TO_INT( argv[6] );
 	char *test = JS_GetStringBytes( JS_ValueToString( cx, argv[7] ) );

	if( test == NULL )
	{
		MethodError( "AddTextEntry: Text is required" );
		return JS_FALSE;
	}

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddTextEntry: Couldn't find gump associated with object");
		return JS_FALSE;
	}
	char temp[256];
	sprintf( temp, "textentry %i %i %i %i %i %i %i", tL, tR, width, height, hue, relay, initialTextIndex );
	gList->one->push_back( temp );
	gList->two->push_back( test );

	return JS_TRUE;
}
//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CGump_AddTiledGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	14th February, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Allows you to tile a particular gump image
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
JSBool CGump_AddTiledGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 5 )
	{
		MethodError( "AddTiledGump: Invalid number of arguments (takes 5 x y width height gump)" );
		return JS_FALSE;
	}

	SI16 x		= (SI16)JSVAL_TO_INT( argv[0] ); // x
	SI16 y		= (SI16)JSVAL_TO_INT( argv[1] ); // y
	SI16 width	= (SI16)JSVAL_TO_INT( argv[2] ); // width
	SI16 height	= (SI16)JSVAL_TO_INT( argv[3] ); // height
	SI16 gumpID = (SI16)JSVAL_TO_INT( argv[4] ); // gump to tile

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddTiledGump: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	char temp[256];
	sprintf( temp, "gumppictiled %i %i %i %i %i", x, y, width, height, gumpID );
	gList->one->push_back( temp );

	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CGump_AddHTMLGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	16th February, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Adds a new HTML gump to the stream of the gump
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
JSBool CGump_AddXMFHTMLGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 7 )
	{
		MethodError( "AddXMFHTMLGump: Invalid number of arguments (takes 7)" );
		return JS_FALSE;
	}

	SI16 x				= (SI16)JSVAL_TO_INT( argv[0] ); // x
	SI16 y				= (SI16)JSVAL_TO_INT( argv[1] ); // y
	SI16 width			= (SI16)JSVAL_TO_INT( argv[2] ); // width
	SI16 height			= (SI16)JSVAL_TO_INT( argv[3] ); // height
	SI32 number			= (SI32)JSVAL_TO_INT( argv[4] ); // number
	bool hasBorder		= ( JSVAL_TO_BOOLEAN( argv[5] ) == JS_TRUE );
	bool hasScrollbar	= ( JSVAL_TO_BOOLEAN( argv[6] ) == JS_TRUE );

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddXMFHTMLGump: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	SI32 iBrd	= (hasBorder?1:0);
	SI32 iScrl	= (hasScrollbar?1:0);
	char temp[256];
	sprintf( temp, "xmfhtmlgump %i %i %i %i %i %i %i", x, y, width, height, number, iBrd, iScrl );
	gList->one->push_back( temp );

	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CGump_AddXMFHTMLGumpColor( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	16th February, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Adds a new HTML gump to the stream of the gump
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
JSBool CGump_AddXMFHTMLGumpColor( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 8 )
	{
		MethodError( "AddXMFHTMLGumpColor: Invalid number of arguments (takes 8)" );
		return JS_FALSE;
	}

	SI16 x				= (SI16)JSVAL_TO_INT( argv[0] ); // x
	SI16 y				= (SI16)JSVAL_TO_INT( argv[1] ); // y
	SI16 width			= (SI16)JSVAL_TO_INT( argv[2] ); // width
	SI16 height			= (SI16)JSVAL_TO_INT( argv[3] ); // height
	SI32 number			= (SI32)JSVAL_TO_INT( argv[4] ); // number
	bool hasBorder		= ( JSVAL_TO_BOOLEAN( argv[5] ) == JS_TRUE );
	bool hasScrollbar	= ( JSVAL_TO_BOOLEAN( argv[6] ) == JS_TRUE );
	SI32 rgbColour		= (SI32)JSVAL_TO_INT( argv[7] );	// colour

	SEGump *gList = (SEGump*)JS_GetPrivate( cx, obj );
	if( gList == NULL )
	{
		MethodError( "AddXMFHTMLGumpColor: Couldn't find gump associated with object" );
		return JS_FALSE;
	}

	SI32 iBrd	= (hasBorder?1:0);
	SI32 iScrl	= (hasScrollbar?1:0);
	char temp[256];
	sprintf( temp, "xmfhtmlgumpcolor %i %i %i %i %i %i %i %i", x, y, width, height, number, iBrd, iScrl, rgbColour );
	gList->one->push_back( temp );

	return JS_TRUE;
}

JSBool CGump_Send( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Send: Invalid number of arguments (takes 1, socket or char)" );
		return JS_FALSE;
	}

	if( !JSVAL_IS_OBJECT( argv[0] ) )
	{
		MethodError( "You have to pass a valid Socket or Character" );
	}

	JSObject *myDest = JSVAL_TO_OBJECT( argv[0] );
	JSClass *myClass = JS_GetClass( myDest );
	SEGump *myGump = (SEGump*)JS_GetPrivate( cx, obj );

	if( !strcmp( myClass->name, "UOXSocket" ) ) 
	{
		cSocket *mySock = (cSocket*)JS_GetPrivate( cx, myDest );
		if( mySock == NULL ) 
		{
			MethodError( "Send: Passed an invalid Socket" );
			return JS_FALSE;
		}

		mySock->TempInt( (SI32)Trigger->GetAssociatedScript( JS_GetGlobalObject( cx ) ) );
		SendVecsAsGump( mySock, *(myGump->one), *(myGump->two), 20, INVALIDSERIAL );
	}
	else if( !strcmp( myClass->name, "UOXChar" ) ) 
	{
		CChar *myChar = (CChar*)JS_GetPrivate( cx, myDest );
		if( !ValidateObject( myChar ) )  
		{
			MethodError( "Send: Passed an invalid Character" );
			return JS_FALSE;
		}

		cSocket *mySock = calcSocketObjFromChar( myChar );

		mySock->TempInt( (SI32)Trigger->GetAssociatedScript( JS_GetGlobalObject( cx ) ) );
		SendVecsAsGump( mySock, *(myGump->one), *(myGump->two), 20, INVALIDSERIAL );
	}
	else
	{
		MethodError( "Send: Unknown Object has been passed" );
		return JS_FALSE;
	}
	
	return JS_TRUE;
}

JSBool JS_ItembySerial( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	JSString *SerialJSString	= JSVAL_TO_STRING( argv[0] );
	UString SerialString		= "0x";
	SerialString				+= JS_GetStringBytes( SerialJSString );
	CItem *myItem				= calcItemObjFromSer( SerialString.toULong() );

	if( !ValidateObject( myItem ) )
	{
		*rval = JSVAL_NULL;
	}
	else
	{
		cScript *myScript	= Trigger->GetAssociatedScript( JS_GetGlobalObject( cx ) );
		JSObject *myObj		= myScript->AcquireObject( IUE_ITEM );
		JS_SetPrivate( cx, myObj, myItem );
		*rval = OBJECT_TO_JSVAL( myObj );
	}

	return JS_TRUE;
}

JSBool JS_CharbySerial( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	JSString *SerialJSString	= JSVAL_TO_STRING( argv[0] );
	UString SerialString		= "0x";
	SerialString				+= JS_GetStringBytes( SerialJSString );
	CChar *myChar				= calcCharObjFromSer( SerialString.toULong() );

	if( !ValidateObject( myChar ) )
	{
		*rval = JSVAL_NULL;
	}
	else
	{
		cScript *myScript	= Trigger->GetAssociatedScript( JS_GetGlobalObject( cx ) );
		JSObject *myObj		= myScript->AcquireObject( IUE_CHAR );
		JS_SetPrivate( cx, myObj, myChar );
		*rval = OBJECT_TO_JSVAL( myObj );
	}

	return JS_TRUE;
}

JSBool JS_WorldBrightLevel( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		MethodError( "Unknown Count of Arguments: %d", argc );
		return JS_FALSE;
	}
	else if( argc == 1 )
	{
		LIGHTLEVEL brightLevel = (LIGHTLEVEL)JSVAL_TO_INT( argv[0] );
		cwmWorldState->ServerData()->WorldLightBrightLevel( brightLevel );
	}
	*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->WorldLightBrightLevel() );
	return JS_TRUE;
}

JSBool JS_WorldDarkLevel( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		MethodError( "Unknown Count of Arguments: %d", argc );
		return JS_FALSE;
	}
	else if( argc == 1 )
	{
		LIGHTLEVEL darkLevel = (LIGHTLEVEL)JSVAL_TO_INT( argv[0] );
		cwmWorldState->ServerData()->WorldLightDarkLevel( darkLevel );
	}
	*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->WorldLightDarkLevel() );
	return JS_TRUE;
}

JSBool JS_WorldDungeonLevel( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc > 1 )
	{
		MethodError( "Unknown Count of Arguments: %d", argc );
		return JS_FALSE;
	}
	else if( argc == 1 )
	{
		LIGHTLEVEL dungeonLevel = (LIGHTLEVEL)JSVAL_TO_INT( argv[0] );
		cwmWorldState->ServerData()->DungeonLightLevel( dungeonLevel );
	}
	*rval = INT_TO_JSVAL( cwmWorldState->ServerData()->DungeonLightLevel() );
	return JS_TRUE;
}

// Spawns an Item
JSBool JS_AddItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	std::string itemString	= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	CItem *myItem			= NULL;

	if( argc == 1 ) 
	{
		myItem = Items->CreateBaseScriptItem( itemString, 0 );
	}
	else if( argc == 2 )
	{		
		CChar *myChar	= (CChar*)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[1] ) );
		myItem			= Items->CreateScriptItem( calcSocketObjFromChar( myChar ), myChar, itemString, 1, OT_ITEM, true );
	}
	else if( argc == 3 )
	{
		CChar *myChar	= (CChar*)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[1] ) );
		myItem			= Items->CreateScriptItem( calcSocketObjFromChar( myChar ), myChar, itemString, 1, OT_ITEM, true );
	}
	else
	{
		MethodError( "Unknown Count of Arguments: %d", argc );
		return JS_FALSE;
	}

	cScript *myScript	= Trigger->GetAssociatedScript( JS_GetGlobalObject( cx ) );
	JSObject *myJSItem	= myScript->AcquireObject( IUE_ITEM );
	JS_SetPrivate( cx, myJSItem, myItem );

	*rval = OBJECT_TO_JSVAL( myJSItem );

	return JS_TRUE;
}

// Spawn a NPC
JSBool JS_AddNPC( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc == 1 ) 
	{
		std::string npcString	= JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
		CChar *myNPC			= Npcs->CreateBaseNPC( npcString );
		cScript *myScript		= Trigger->GetAssociatedScript( JS_GetGlobalObject( cx ) );
		JSObject *myJSChar		= myScript->AcquireObject( IUE_CHAR );
		JS_SetPrivate( cx, myJSChar, myNPC );

		*rval = OBJECT_TO_JSVAL( myJSChar );
	}
	else
	{		
		MethodError( "Unknown Count of Arguments: %d", argc );
		return JS_FALSE;
	}

	return JS_TRUE;
}

// Character related methods!
JSBool CBase_TextMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "TextMessage: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	JSClass *myClass		= JS_GetClass( obj );
	cBaseObject *myObj		= (cBaseObject*)JS_GetPrivate( cx, obj );

	JSString *targMessage	= JSVAL_TO_STRING( argv[0] );
	char *trgMessage		= JS_GetStringBytes( targMessage );

	if( trgMessage == NULL )
	{
		MethodError( "You have to supply a messagetext" );
	}
	
	if( !strcmp(myClass->name, "UOXItem") ) 
	{
		CItem *myItem = (CItem*)myObj;
		if( !ValidateObject( myItem ) )  
		{
			MethodError( "TextMessage: Invalid Item" );
			return JS_FALSE;
		}
		MethodSpeech( *myItem, trgMessage, OBJ, 0x0026, FNT_SMALL_LIGHT );
	}
	else if( !strcmp( myClass->name, "UOXChar" ) ) 
	{
		CChar *myChar = (CChar*)myObj;
		if( !ValidateObject( myChar ) )  
		{
			MethodError( "TextMessage: Invalid Character" );
			return JS_FALSE;
		}

		if( myChar->GetNPCAiType() == aiEVIL )
			MethodSpeech( *myChar, trgMessage, TALK, 0x0026, (FontType)myChar->GetFontType() );
		else
			MethodSpeech( *myChar, trgMessage, TALK, myChar->GetSayColour(), (FontType)myChar->GetFontType() );
	}	

	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	JSBool CBase_KillTimers( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date					-	04/20/2002
//|	Developer(s)	-	Rukus
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	Kill all related timers that have been association with
//|									an item or character.
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o	
JSBool CBase_KillTimers( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if( argc != 0 )
	{
		MethodError( "KillTimers: Invalid count of arguments :%d, needs :0", argc );
		return JS_FALSE;
	}
	cBaseObject *myObj = static_cast<cBaseObject*>(JS_GetPrivate(cx, obj));
	if( myObj==NULL )
	{
		MethodError("KillTimers: Invalid object assigned.");
		return JS_FALSE;
	}
	SERIAL mySer = myObj->GetSerial();
	TEffects->StartQueue();
	for( CTEffect *Effect = TEffects->First(); !TEffects->AtEnd(); Effect = TEffects->Next() )
	{
		if( mySer == Effect->Destination() )
			TEffects->QueueToRemove( Effect );
		else
			TEffects->QueueToKeep( Effect );
	}
	TEffects->Prune();
	return JS_TRUE;
}


JSBool CBase_Delete( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	cBaseObject *myObj = (cBaseObject*)JS_GetPrivate( cx, obj );

	if( !ValidateObject( myObj ) )
	{
		MethodError( "Delete: Invalid object");
		return JS_FALSE;
	}

	myObj->Delete();
	return JS_TRUE;
}

JSBool CChar_WanderBox( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 4 )
	{
		MethodError( "WanderBox: Invalid number of arguments (takes 4, rectangle coordinates)" );
		return JS_FALSE;
	}

	UI16 x1 = (UI16)JSVAL_TO_INT( argv[0] );
	UI16 y1 = (UI16)JSVAL_TO_INT( argv[1] );
	UI16 x2 = (UI16)JSVAL_TO_INT( argv[2] );
	UI16 y2 = (UI16)JSVAL_TO_INT( argv[3] );

	CChar *myChar = static_cast<CChar*>(JS_GetPrivate( cx, obj ) );

	if( !ValidateObject( myChar ) || !myChar->IsNpc() )
	{
		MethodError( "WanderBox: Invalid character" );
		return JS_FALSE;
	}
	myChar->SetFx( x1, 0 );
	myChar->SetFy( y1, 0 );
	myChar->SetFz( -1 );
	myChar->SetFx( x2, 1 );
	myChar->SetFy( y2, 1 );
	myChar->SetNpcWander( 3 );

	return JS_TRUE;
}

JSBool CChar_Follow( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Follow: Invalid number of arguments (takes 1, char object)" );
		return JS_FALSE;
	}

	JSObject *jsObj = JSVAL_TO_OBJECT( argv[0] );
	cBaseObject *myObj = (cBaseObject *)JS_GetPrivate( cx, jsObj );

	CChar *myChar = static_cast<CChar*>(JS_GetPrivate( cx, obj ) );

	if( !ValidateObject( myChar ) || !myChar->IsNpc() || !ValidateObject( myObj ) || myObj->GetSerial() >= BASEITEMSERIAL )
	{
		MethodError( "Follow: Invalid character" );
		return JS_FALSE;
	}
	
	myChar->SetFTarg( (CChar*)myObj );
	myChar->SetNpcWander( 1 );

	return JS_TRUE;
}

JSBool CChar_Action( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Action: Invalid number of arguments (takes 1, action)" );
		return JS_FALSE;
	}

	UI08 targAction = (UI08)JSVAL_TO_INT( argv[0] );
	CChar *myChar = static_cast<CChar*>(JS_GetPrivate( cx, obj ) );

	if( !ValidateObject( myChar ) )
	{
		MethodError( "Action: Invalid character" );
		return JS_FALSE;
	}
	Effects->PlayCharacterAnimation( myChar, targAction );
	return JS_TRUE;
}

JSBool CChar_EmoteMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "EmoteMessage: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	
	CChar *myChar = static_cast<CChar*>(JS_GetPrivate(cx, obj));
	JSString *targMessage = JSVAL_TO_STRING( argv[0] );
	char *trgMessage = JS_GetStringBytes( targMessage );
	
	if( !ValidateObject( myChar ) || trgMessage == NULL )
	{
		MethodError( "EmoteMessage: Invalid character or speech");
		return JS_FALSE;
	}
	MethodSpeech( *myChar, trgMessage, EMOTE, myChar->GetEmoteColour(), (FontType)myChar->GetFontType() );
	return JS_TRUE;
}

JSBool CChar_Dismount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Dismount: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	
	CChar *myChar = static_cast<CChar*>(JS_GetPrivate(cx, obj));
	
	if( !ValidateObject( myChar ) )
	{
		MethodError( "Dismount: Invalid character");
		return JS_FALSE;
	}
	if( myChar->IsOnHorse() )
		DismountCreature( myChar );
	return JS_TRUE;
}

JSBool CSocket_SysMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "SysMessage: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	
	JSString *targMessage	= JS_ValueToString( cx, argv[0] );
	char *trgMessage		= JS_GetStringBytes( targMessage );
	cSocket *targSock		= (cSocket*)JS_GetPrivate( cx, obj );

	if( targSock == NULL || trgMessage == NULL )
	{
		MethodError( "SysMessage: Invalid socket or speech (%s)", targMessage );
		return JS_FALSE;
	}
	targSock->sysmessage( trgMessage );
	return JS_TRUE;
}

JSBool CSocket_Disconnect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Disconnect: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	
	cSocket *targSock = (cSocket*)JS_GetPrivate( cx, obj );

	if( targSock == NULL )
	{
		MethodError( "SysMessage: Invalid socket" );
		return JS_FALSE;
	}
	
	Network->Disconnect( targSock );
	JS_SetPrivate( cx, obj, NULL ); // yes we should do that...

	return JS_TRUE;
}

//
//	The following assumptions are made:
//	1 argument: Split the list by , (1,1,1,1) i.e.
//	2 arguments: x + y
//	3 arguments: x + y + z
//	4 arguments: x + y + z + world
//
JSBool CBase_Teleport( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	JSClass *myClass = JS_GetClass( obj );

	cBaseObject *myObj = (cBaseObject*)JS_GetPrivate( cx, obj );

	SI16 x		= -1;
	SI16 y		= -1;
	SI08 z		= myObj->GetZ();
	UI08 world	= myObj->WorldNumber();

	switch( argc )
	{
		// Just Teleport...
		case 0:
			if( !strcmp( myClass->name, "UOXChar" ) )
			{
				((CChar*)myObj)->Teleport();
				return JS_TRUE;
			}
			MethodError( "For Items you need at least one parameter for Teleport" );
			break;

		// Parameters as a string
		case 1:
			if( JSVAL_IS_OBJECT( argv[0] ) )
			{	// we can work with this, it should be either a character or item, hopefully
				JSObject *jsToGoTo = JSVAL_TO_OBJECT( argv[0] );
				cBaseObject *toGoTo = (cBaseObject *)JS_GetPrivate( cx, jsToGoTo );
				if( !ValidateObject( toGoTo ) )
				{
					MethodError( "No object associated with this object" );
					break;
				}
				JSClass *myClass = JS_GetClass( jsToGoTo );
				if( !strcmp( myClass->name, "UOXItem" ) || !strcmp( myClass->name, "UOXChar" ) )
				{
					x		= toGoTo->GetX();
					y		= toGoTo->GetY();
					z		= toGoTo->GetZ();
					world	= toGoTo->WorldNumber();
				}
				else if( !strcmp( myClass->name, "UOXSocket" ) )
				{
					cSocket *mySock		= (cSocket *)toGoTo;
					CChar *mySockChar	= mySock->CurrcharObj();
					x		= mySockChar->GetX();
					y		= mySockChar->GetY();
					z		= mySockChar->GetZ();
					world	= mySockChar->WorldNumber();
				}
				else
				{
					MethodError( "Invalid class of object" );
					break;
				}
			}
			// Needs yet to be implemented
			MethodError( "Text-styled Parameters may be added later" );
			break;

		// 2 Parameters, x + y
		case 2: 
			x = (SI16)JSVAL_TO_INT( argv[0] );
			y = (SI16)JSVAL_TO_INT( argv[1] );
			break;

		// x,y,z
		case 3:
			x = (SI16)JSVAL_TO_INT( argv[0] );
			y = (SI16)JSVAL_TO_INT( argv[1] );
			z = (SI08)JSVAL_TO_INT( argv[2] );
			break;

		// x,y,z,world
		case 4:
			x		= (SI16)JSVAL_TO_INT( argv[0] );
			y		= (SI16)JSVAL_TO_INT( argv[1] );
			z		= (SI08)JSVAL_TO_INT( argv[2] );
			world	= (UI08)JSVAL_TO_INT( argv[3] );
			break;
		default:
			MethodError( "Invalid number of arguments passed to Teleport, needs either 1, 2, 3 or 4" );
			break;
	}

	if( !strcmp( myClass->name, "UOXItem" ) ) 
	{
		CItem *myItem = (CItem*)myObj;
		if( !ValidateObject( myItem ) )  
		{
			MethodError( "Teleport: Invalid Item" );
			return JS_FALSE;
		}

		myItem->SetLocation( x, y, z, world );
	}
	else if( !strcmp( myClass->name, "UOXChar" ) ) 
	{
		CChar *myChar = (CChar*)myObj;
		if( !ValidateObject( myChar ) )  
		{
			MethodError( "Teleport: Invalid Character" );
			return JS_FALSE;
		}

		if( world != myChar->WorldNumber() && !myChar->IsNpc() )
		{
			cSocket *mySock = calcSocketObjFromChar( myChar );
			if( mySock == NULL ) 
				return JS_TRUE;

			myChar->RemoveFromSight();
			myChar->SetLocation( x, y, z, world );
			SendMapChange( world, mySock );
			myChar->Teleport();
		} 
		else 
		{
			myChar->SetLocation( x, y, z, world );
			myChar->Teleport();
		}
	}	

	return JS_TRUE;
}

JSBool CBase_StaticEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	UI16 effectID		= (UI16)JSVAL_TO_INT( argv[0] );
	UI08 speed			= (UI08)JSVAL_TO_INT( argv[1] );
	UI08 loop			= (UI08)JSVAL_TO_INT( argv[2] );
	cBaseObject *myObj	= (cBaseObject*)JS_GetPrivate( cx, obj );
	
	if( !ValidateObject( myObj ) )
	{
		MethodError( "StaticEffect: Invalid Object" );
		return JS_FALSE;
	}

	JSClass *myClass = JS_GetClass( obj );

	if( !strcmp( myClass->name, "UOXItem" ) )
	{
		bool explode = ( JSVAL_TO_BOOLEAN( argv[3] ) == JS_TRUE );
		Effects->PlayStaticAnimation( myObj, effectID, speed, loop, explode );		
	}
	else
	{
		Effects->PlayStaticAnimation( myObj, effectID, speed, loop );
	}

	return JS_TRUE;
}

JSBool CMisc_SoundEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "SoundEffect: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}

	JSClass *myClass = JS_GetClass( obj );

	UI16 sound = (UI16)JSVAL_TO_INT( argv[0] );
	bool allHear = ( JSVAL_TO_BOOLEAN( argv[1] ) == JS_TRUE );

	if( !strcmp( myClass->name, "UOXChar" ) || !strcmp( myClass->name, "UOXItem" ) )
	{
		cBaseObject *myObj = (cBaseObject*)JS_GetPrivate( cx, obj );

		if( ValidateObject( myObj ) )
		{
			Effects->PlaySound( myObj, (sound-1), allHear );
		}
	}
	else if( !strcmp( myClass->name, "UOXSocket" ) )
	{
		cSocket *mySock = (cSocket*)JS_GetPrivate( cx, obj );

		if( mySock != NULL )
			Effects->PlaySound( mySock, (sound-1), allHear );
	}

	return JS_TRUE;
}

JSBool CMisc_SellTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "SellTo: Invalid Number of Arguments: %d", argc );
		return JS_FALSE;
	}

	JSClass *myClass	= JS_GetClass( obj );
	CChar *myNPC		= (CChar*)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
	if( !ValidateObject( myNPC ) )
	{
		MethodError( "SellTo: Invalid NPC" );
		return JS_FALSE;
	}

	if( !strcmp( myClass->name, "UOXSocket" ) )
	{
		cSocket *mySock = (cSocket*)JS_GetPrivate( cx, obj );
		if( mySock == NULL )
		{
			MethodError( "Passed an invalid socket to SellTo" );
			return JS_FALSE;
		}

		myNPC->SetTimer( tNPC_MOVETIME, BuildTimeValue( 60.0f ) );
		sendSellStuff( mySock, myNPC );
	}
	else if( !strcmp( myClass->name, "UOXChar" ) )
	{
		CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );
		if( !ValidateObject( myChar ) )
		{
			MethodError( "Passed an invalid char to SellTo" );
			return JS_FALSE;
		}		

		myNPC->SetTimer( tNPC_MOVETIME, BuildTimeValue( 60.0f ) );
		sendSellStuff( calcSocketObjFromChar( myChar ), myNPC );
	}

	return JS_TRUE;
}

JSBool CMisc_BuyFrom( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "SellTo: Invalid Number of Arguments: %d", argc );
		return JS_FALSE;
	}

	JSClass *myClass	= JS_GetClass( obj );
	CChar *myNPC		= (CChar*)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
	if( !ValidateObject( myNPC ) )
	{
		MethodError( "BuyFrom: Invalid NPC" );
		return JS_FALSE;
	}

	if( !strcmp( myClass->name, "UOXSocket" ) )
	{
		cSocket *mySock = (cSocket *)JS_GetPrivate( cx, obj );
		if( mySock == NULL )
		{
			MethodError( "Invalid source socket in BuyFrom" );
			return JS_FALSE;
		}

		if( myNPC->GetNPCAiType() == aiPLAYERVENDOR )
		{
			mySock->TempObj( myNPC );
			myNPC->talk( mySock, 772, false );
			mySock->target( 0, TARGET_PLVBUY, " ");
		} 
		else
			Targ->BuyShop( mySock, myNPC );
	}
	else if( !strcmp( myClass->name, "UOXChar" ) )
	{
		CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );
		if( !ValidateObject( myChar ) )
		{
			MethodError( "Passed an invalid char to BuyFrom" );
			return JS_FALSE;
		}		

		cSocket *mySock = calcSocketObjFromChar( myChar );	

		if( myNPC->GetNPCAiType() == aiPLAYERVENDOR )
		{
			mySock->TempObj( myNPC );
			myNPC->talk( mySock, 772, false );
			mySock->target( 0, TARGET_PLVBUY, " ");
		} 
		else
			Targ->BuyShop( mySock, myNPC );
	}

	return JS_TRUE;
}

JSBool CMisc_HasSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "HasSpell: Invalid Number of Arguments: %d", argc );
		return JS_FALSE;
	}

	JSClass *myClass = JS_GetClass( obj );
	UI08 SpellID = (UI08)JSVAL_TO_INT( argv[0] );

	if( !strcmp( myClass->name, "UOXChar" ) )
	{
		CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );
		if( !ValidateObject( myChar ) )
		{
			MethodError( "Invalid char for HasSpell" );
			return JS_FALSE;
		}

		CItem *myItem = FindItemOfType( myChar, IT_SPELLBOOK );

		if( !ValidateObject( myItem ) )
		{
			*rval = BOOLEAN_TO_JSVAL( JS_FALSE );
			return JS_TRUE;
		}

		if( Magic->HasSpell( myItem, SpellID ) )
			*rval = BOOLEAN_TO_JSVAL( JS_TRUE );
		else
			*rval = BOOLEAN_TO_JSVAL( JS_FALSE );
	}
	else if( !strcmp( myClass->name, "UOXItem" ) )
	{
		CItem *myItem = (CItem*)JS_GetPrivate( cx, obj );
		if( !ValidateObject( myItem ) )
		{
			MethodError( "Invalid item for HasSpell" );
			return JS_FALSE;
		}		

		if( Magic->HasSpell( myItem, SpellID ) )
			*rval = BOOLEAN_TO_JSVAL( JS_TRUE );
		else
			*rval = BOOLEAN_TO_JSVAL( JS_FALSE );
	}

	return JS_TRUE;
}

JSBool CMisc_RemoveSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "RemoveSpell: Invalid Number of Arguments: %d", argc );
		return JS_FALSE;
	}

	JSClass *myClass = JS_GetClass( obj );
	UI08 SpellID = (UI08)JSVAL_TO_INT( argv[0] );

	if( !strcmp( myClass->name, "UOXChar" ) )
	{
		CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );
		if( !ValidateObject( myChar ) )
		{
			MethodError( "Invalid char for HasSpell" );
			return JS_FALSE;
		}

		CItem *myItem = FindItemOfType( myChar, IT_SPELLBOOK );

		if( ValidateObject( myItem ) )
		{
			Magic->RemoveSpell( myItem, SpellID );			
		}
	}
	else if( !strcmp( myClass->name, "UOXItem" ) )
	{
		CItem *myItem = (CItem*)JS_GetPrivate( cx, obj );
		if( !ValidateObject( myItem ) )
		{
			MethodError( "Invalid item for RemoveSpell" );
			return JS_FALSE;
		}
		
		Magic->RemoveSpell( myItem, SpellID );
	}

	return JS_TRUE;
}

JSBool CBase_GetTag( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Invalid Count of Parameters: %d, need: 1", argc );
	}
	
	cBaseObject *myObj = (cBaseObject*)JS_GetPrivate( cx, obj );

	if( !ValidateObject( myObj ) )
	{
		MethodError( "Invalid Object assigned (GetTag)" );
		return JS_FALSE;
	}
	
	char *TagName = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );

	*rval = myObj->GetTag( TagName );
	return JS_TRUE;
}

JSBool CBase_SetTag( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if(( argc != 2 ) && ( argc != 1 ))
	{
		MethodError( "SetTag: Invalid Count of Parameters: %d, need: 2", argc );
	}
	
	cBaseObject *myObj = (cBaseObject*)JS_GetPrivate( cx, obj );

	if( !ValidateObject( myObj ) )
	{
		MethodError( "Invalid Object assigned (SetTag)" );
		return JS_FALSE;
	}
	
	char *TagName = JS_GetStringBytes( JS_ValueToString( cx, argv[0] ) );
	
	if( argc == 2 && JSVAL_IS_STRING( argv[1] ) && !strcmp( "", JS_GetStringBytes( JS_ValueToString( cx, argv[1] ) ) ) )
		argv[1] = JSVAL_NULL;

	if( argc == 2 )
		myObj->SetTag( TagName, argv[1] );
	else
		myObj->SetTag( TagName, JSVAL_NULL );

	return JS_TRUE;
}

JSBool CBase_GetNumTags( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Invalid Count of Parameters: %d, need: 0", argc );
	}
	
	cBaseObject *myObj = (cBaseObject*)JS_GetPrivate( cx, obj );

	if( !ValidateObject( myObj ) )
	{
		MethodError( "Invalid Object assigned (GetTag)" );
		return JS_FALSE;
	}
	
	*rval = INT_TO_JSVAL( myObj->GetNumTags() );
	return JS_TRUE;
}

JSBool CChar_OpenBank( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );
	if( !ValidateObject( myChar ) )
	{
		MethodError( "OpenBank: Invalid Character object assigned" );
		return JS_FALSE;
	}

	cSocket *mySock = NULL;
	// Open the bank of myChar to myChar
	if( argc == 0 )
	{
		mySock = calcSocketObjFromChar( myChar );
		if( mySock != NULL )
			mySock->openBank( myChar );
	}
	// Open it to the passed socket
	else if( argc == 1 )
	{
		mySock = (cSocket*)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
		if( mySock != NULL )
			mySock->openBank( myChar );
	}
	else
	{
		MethodError( "OpenBank, Invalid count of Paramters: %d", argc );
		return JS_FALSE;
	}

	return JS_TRUE;
}

JSBool CChar_OpenLayer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );
	if( !ValidateObject( myChar ) )
	{
		MethodError( "OpenLayer: Invalid Character object assigned" );
		return JS_FALSE;
	}

	if( argc != 2 )
	{
		MethodError( "OpenLayer, Invalid count of Paramters: %d", argc );
		return JS_FALSE;
	}
	cSocket *mySock = (cSocket*)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
	if( mySock != NULL )
	{
		CItem *iLayer = myChar->GetItemAtLayer( (UI08)JSVAL_TO_INT( argv[1] ) );
		if( ValidateObject( iLayer ) )
			mySock->openPack( iLayer );
	}

	return JS_TRUE;
}

JSBool CChar_TurnToward( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	//	
	//	2 Parameters: x + y
	//	1 Parameter: either item or character
	//	

	CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );

	if( !ValidateObject( myChar ) )
	{
		MethodError( "(TurnToward) Wrong object assigned" );
		return JS_FALSE;
	}

	SI16 x, y;

	if( argc == 1 )
	{
		if( !JSVAL_IS_OBJECT( argv[0] ) )
		{
			MethodError( "(TurnToward) Invalid Object passed" );
			return JS_FALSE;
		}

		cBaseObject *myObj = (cBaseObject*)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );

		x = myObj->GetX();
		y = myObj->GetY();
	}
	else if( argc == 2 )
	{
		// x + y
		x = (SI16)JSVAL_TO_INT( argv[0] );
		y = (SI16)JSVAL_TO_INT( argv[1] );
	}
	else
	{
		MethodError( "(TurnToward) Wrong paramter count: %d, needs either one char/item or x+y", argc );
		return JS_FALSE;
	}

	// Just don't do anything if NewDir eq OldDir
	
	UI08 NewDir = Movement->Direction( myChar, x, y );

	if( NewDir != myChar->GetDir() )
		myChar->SetDir( NewDir );

	return JS_TRUE;
}

JSBool CChar_DirectionTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	//	
	//	2 Parameters: x + y
	//	1 Parameter: either item or character
	//	

	CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );

	if( !ValidateObject( myChar ) )
	{
		MethodError( "(DirectionTo) Wrong object assigned" );
		return JS_FALSE;
	}

	SI16 x, y;

	if( argc == 1 )
	{
		if( !JSVAL_IS_OBJECT( argv[0] ) )
		{
			MethodError( "(DirectionTo) Invalid Object passed" );
			return JS_FALSE;
		}

		cBaseObject *myObj = (cBaseObject*)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );

		x = myObj->GetX();
		y = myObj->GetY();
	}
	else if( argc == 2 )
	{
		// x + y
		x = (SI16)JSVAL_TO_INT( argv[0] );
		y = (SI16)JSVAL_TO_INT( argv[1] );
	}
	else
	{
		MethodError( "(DirectionTo) Wrong paramter count: %d, needs either one char/item or x+y", argc );
		return JS_FALSE;
	}

	// Just don't do anything if NewDir eq OldDir
	
	UI08 NewDir = Movement->Direction( myChar, x, y );

	*rval = INT_TO_JSVAL( NewDir );

	return JS_TRUE;
}

JSBool CChar_ExecuteCommand( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
 		MethodError( "ExecuteCommand: Invalid number of arguments (takes 1)" );
 		return JS_FALSE;
 	}
 	
 	JSString *targMessage	= JS_ValueToString( cx, argv[0] );
 	CChar *myChar			= (CChar*)JS_GetPrivate( cx, obj );
 	char *trgMessage		= JS_GetStringBytes( targMessage );
 	cSocket *targSock		= calcSocketObjFromChar( myChar ) ;
 	if( targSock == NULL || trgMessage == NULL )
 	{
 		MethodError( "ExecuteCommand: Invalid socket or speech (%s)", targMessage );
		return JS_FALSE;
 	}
 	Commands->Command( targSock, myChar, trgMessage );
	return JS_TRUE;
}

JSBool CGuild_AcceptRecruit( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CGuild *myGuild = (CGuild*)JS_GetPrivate( cx, obj );

	if( myGuild == NULL )
	{
		MethodError( "(AcceptRecruit) Invalid Object assigned" );
		return JS_FALSE;
	}
	
	// Two choices here... 0 paramteres = accept the JS_GetParent object
	// 1 parameter = get the cchar from there
	if( argc == 0 )
	{
		JSObject *Parent = JS_GetParent( cx, obj );
		CChar *myChar = (CChar*)JS_GetPrivate( cx, Parent );
		myGuild->RecruitToMember( *myChar );
	}
	else if( argc == 1 )
	{
		CChar *myChar = (CChar*)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
		myGuild->RecruitToMember( *myChar );
	}
	else
	{
		MethodError( "(AcceptRecruit) Invalid Parameter Count: %d", argc );
		return JS_FALSE;
	}

	return JS_TRUE;
}

JSBool CChar_ResourceCount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );

	if( !ValidateObject( myChar ) )
	{
		MethodError( "(ResourceCount) Invalid Object assigned" );
		return JS_FALSE;
	}

	UI16 realID = (UI16)JSVAL_TO_INT( argv[0] );
	UI16 itemColour = 0;
		
	if(( argc < 1 ) || ( argc > 2 ))
	{
		MethodError( "(ResourceCount) Invalid count of parameters: %d, either needs 1 or 2" , argc );
		return JS_FALSE;
	}

	if( argc == 2 )
	{
		itemColour = (UI16)JSVAL_TO_INT( argv[1] );
	}
	
	*rval = INT_TO_JSVAL( GetItemAmount( myChar, realID, itemColour ) );
	return JS_TRUE;
}

JSBool CChar_UseResource( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );

	if( !ValidateObject( myChar ) )
	{
		MethodError( "(UseResource) Invalid Object assigned" );
		return JS_FALSE;
	}

	UI16 realID = (UI16)JSVAL_TO_INT( argv[1] );
	UI16 itemColour = 0;
	UI16 amount = (UI16)JSVAL_TO_INT( argv[0] );	

	// Min. 2 Arguments (amount + id) or 3
	if(( argc < 2 ) || ( argc > 3 ))
	{
		MethodError( "(UseResource) Invalid count of parameters: %d, either needs 2 or 3" , argc );
		return JS_FALSE;
	}

	if( argc == 3 )
	{
		itemColour = (UI16)JSVAL_TO_INT( argv[2] );
	}
	Console.Print("\n*****\nCharname: %s\nRealID: %i\nitemColor: %i\nAmount: %i\n*****\n", myChar->GetName().c_str(), realID, itemColour, amount );
	DeleteItemAmount( myChar, amount, realID, itemColour );
	return JS_TRUE;
}

JSBool CChar_BoltEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );
	if( ValidateObject( myChar ) )
		Effects->bolteffect( myChar );

	return JS_TRUE;
}

JSBool CMisc_CustomTarget( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	// Either useable with sockets OR characters
	JSClass *myClass = JS_GetClass( obj );

	if(( argc > 2 ) || ( argc < 1 ))
	{
		MethodError( "(CustomTarget) Invalid count of parameters: %d, either needs 1 or 2", argc );
		return JS_FALSE;
	}

	cSocket *mySock = NULL;

	if( !strcmp( myClass->name, "UOXChar" ) )
	{
		// Character
		CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );

		if( !ValidateObject( myChar ) )
		{
			MethodError( "(CustomTarget) Invalid Character assigned" );
			return JS_FALSE;
		}

		mySock = calcSocketObjFromChar( myChar );
	}
	else if( !strcmp( myClass->name, "UOXSocket" ) )
	{
		// We have a socket here
		mySock = (cSocket*)JS_GetPrivate( cx, obj );
	}

	if( mySock == NULL )
	{
		// COULD be a NPC too so just exit here!
		// and DONT create a non-running jscript
		return JS_TRUE;
	}

	mySock->TempInt( (SI32)Trigger->GetAssociatedScript( JS_GetGlobalObject( cx ) ) );
	UI08 tNum = (UI08)JSVAL_TO_INT( argv[0] );
	char toSay[512] = { 0, }; // Could become long (make sure it's NULL )
 	
	if( argc == 2 )
	{
		sprintf( toSay, "%s", JS_GetStringBytes( JS_ValueToString( cx, argv[1] ) ) );
	}

	mySock->target( 1, tNum, toSay );
	return JS_TRUE;
}

JSBool CMisc_PopUpTarget( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if(( argc > 2 ) || ( argc < 1 ))
	{
		MethodError( "(PopUpTarget) Invalid count of parameters: %d, either needs 1 or 2", argc );
		return JS_FALSE;
	}

	// Either useable with sockets OR characters
	JSClass *myClass = JS_GetClass( obj );

	cSocket *mySock = NULL;

	if( !strcmp( myClass->name, "UOXChar" ) )
	{
		// Character
		CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );

		if( !ValidateObject( myChar ) )
		{
			MethodError( "(PopUpTarget) Invalid Character assigned" );
			return JS_FALSE;
		}

		mySock = calcSocketObjFromChar( myChar );
	}

	else if( !strcmp( myClass->name, "UOXSocket" ) )
	{
		// We have a socket here
		mySock = (cSocket*)JS_GetPrivate( cx, obj );
	}

	if( mySock == NULL )
	{
		// COULD be a NPC too so just exit here!
		// and DONT create a non-running jscript
		return JS_TRUE;
	}

	UI08 tNum = (UI08)JSVAL_TO_INT( argv[0] );
	char toSay[512] = { 0, }; // Could become long (make sure it's NULL )

	if( argc == 2 )
	{
		sprintf( toSay, "%s", JS_GetStringBytes( JS_ValueToString( cx, argv[1] ) ) );
	}

	mySock->target( 0, tNum, toSay );
	return JS_TRUE;
}

JSBool CBase_InRange( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "(InRange): Invalid count of parameters: %d needs 2 (Item/Char and distance)", argc );
		return JS_FALSE;
	}

	UI16 distance = (UI16)JSVAL_TO_INT( argv[1] );
	
	cBaseObject *me = (cBaseObject*)JS_GetPrivate( cx, obj );
	if( !ValidateObject( me ) )
	{
			MethodError( "(InRange) Invalid Object assigned to self" );
			return JS_FALSE;
	}

	cBaseObject *them = (cBaseObject*)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
	if( !ValidateObject( them ) )
	{
			MethodError( "(InRange) Invalid Object assigned to target" );
			return JS_FALSE;
	}

	if( ( them->GetObjType() == OT_ITEM ) && ( me->GetObjType() == OT_CHAR ) )
	{
		CItem *myItem = static_cast< CItem * >(them);
		if( myItem->GetCont() != NULL )
		{
			*rval = BOOLEAN_TO_JSVAL( FindItemOwner( myItem ) == me );
			return JS_TRUE;
		}
	}

	*rval = BOOLEAN_TO_JSVAL( objInRange( me, them, distance ) );
	return JS_TRUE;
}

JSBool CBase_StartTimer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	cBaseObject *myObj = static_cast< cBaseObject* >( JS_GetPrivate( cx, obj ) );
	
	if( !ValidateObject( myObj ) )
	{
		MethodError( "(StartTimer) Invalid Object assigned" );
		return JS_FALSE;
	}

	if( argc != 2 && argc != 3 )
	{
		MethodError( "(StartTimer) Invalid count of parameter: %d (needs 2 or 3)", argc );
		return JS_FALSE;
	}

	// 1. Parameter Delay, 2. Parameter Callback
	UI32 ExpireTime = BuildTimeValue( JSVAL_TO_INT( argv[0] ) / 1000.0f );
	UI08 TriggerNum = (UI08)JSVAL_TO_INT( argv[1] );
	
	CTEffect *Effect = TEffects->CreateEffect();

	if( argc == 3 )
	{
		if( JSVAL_IS_BOOLEAN( argv[2] ) )	// Is it a boolean?  If so, might be calling back into here
		{
			if( JSVAL_TO_BOOLEAN( argv[2] ) == JS_TRUE )
				Effect->AssocScript( Trigger->GetAssociatedScript( JS_GetGlobalObject( cx ) ) );
			else
				Effect->More2( 0xFFFF );
		}
		else
			Effect->More2( (UI16)JSVAL_TO_INT( argv[2] ) );
	}
	else
		Effect->More2( 0xFFFF );

	Effect->Destination( myObj->GetSerial() );
	Effect->ExpireTime( ExpireTime );
	Effect->Source( myObj->GetSerial() );
	Effect->Number( 40 );
	Effect->More1( TriggerNum );

	TEffects->Add( Effect );
	
	return JS_TRUE;
}

JSBool CChar_CheckSkill( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 3 )
	{
		MethodError( "CheckSkill: Invalid number of arguments (takes 3, skillNum, minSkill and maxSkill)" );
		return JS_FALSE;
	}

	CChar *myChar = static_cast<CChar*>(JS_GetPrivate( cx, obj ) );

	if( !ValidateObject( myChar ) )
	{
		MethodError( "CheckSkill: Invalid character" );
		return JS_FALSE;
	}

	UI08 skillNum = (UI08)JSVAL_TO_INT( argv[0] );
	UI16 minSkill = (UI16)JSVAL_TO_INT( argv[1] );
	UI16 maxSkill = (UI16)JSVAL_TO_INT( argv[2] );
	*rval = BOOLEAN_TO_JSVAL( Skills->CheckSkill( myChar, skillNum, minSkill, maxSkill ) );
	return JS_TRUE;
}

JSBool CChar_FindItemLayer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CItem *myItem;
	CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );

	if( !ValidateObject( myChar ) )
	{
		MethodError( "(FindItemLayer) Invalid Charobject assigned" );
		return JS_FALSE;
	}

	if( argc == 1 ) 
	{
		myItem = myChar->GetItemAtLayer( (UI08)JSVAL_TO_INT( argv[0] ) );
	}
	else
	{
		MethodError( "(FindItemLayer) Unknown Count of Arguments: %d, needs: 1", argc );
		return JS_FALSE;
	}

	if( !ValidateObject( myItem ) )
	{
		*rval = JSVAL_NULL;
		return JS_TRUE;
	}

	cScript *myScript	= Trigger->GetAssociatedScript( JS_GetGlobalObject( cx ) );
	JSObject *myJSItem	= myScript->AcquireObject( IUE_ITEM );
	JS_SetPrivate( cx, myJSItem, myItem );

	*rval = OBJECT_TO_JSVAL( myJSItem );

	return JS_TRUE;
}

// Open the Plank
void OpenPlank( CItem *p );
JSBool CItem_OpenPlank( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "(OpenPlank) Invalid Count of Arguments: %d, needs: 0", argc );
		return JS_FALSE;
	}

	CItem *myItem = (CItem*)JS_GetPrivate( cx, obj );

	if( !ValidateObject( myItem ) )
	{
		MethodError( "(OpenPlank) Invalid Object assigned" );
		return JS_FALSE;
	}

	OpenPlank( myItem );

	return JS_TRUE;
}

JSBool CChar_SpeechInput( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	// Get our own Script ID
	UI08 speechID		= 0;
	CItem *speechItem	= NULL;

	if( argc == 1 ) // Just the ID has been passed
	{
		speechID = (UI08)JSVAL_TO_INT( argv[0] );
	}
	else if( argc == 2 ) // We got an "affected" item as well
	{
		speechID = (UI08)JSVAL_TO_INT( argv[0] );
		
		if( argv[1] != JSVAL_NULL )
		{
			JSObject *myObj = JSVAL_TO_OBJECT( argv[1] );
			speechItem = (CItem*)JS_GetPrivate( cx, myObj );
		}		
	}
	else
	{
		MethodError( "(SpeechInput) Invalid Count of parameters: %d, needs: either 1 or 2", argc );
		return JS_FALSE;
	}

	CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );

	if( !ValidateObject( myChar ) )
	{
		MethodError( "(SpeechInput) Invalid object assigned" );
		return JS_FALSE;
	}

	myChar->SetSpeechMode( 9 );
	if( ValidateObject( speechItem ) )
		myChar->SetSpeechItem( speechItem );
	else
		myChar->SetSpeechItem( NULL );

	myChar->SetSpeechID( speechID );
	myChar->SetSpeechCallback( Trigger->GetAssociatedScript( JS_GetGlobalObject( cx ) ) );

	return JS_TRUE;
}

JSBool CChar_Freeze( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "(Freeze) Invalid Count of Arguments: %d, needs: 0", argc );
		return JS_FALSE;
	}
	CChar *myChar = (CChar *)JS_GetPrivate( cx, obj );

	if( !ValidateObject( myChar ) )
	{
		MethodError( "Freeze: Invalid character" );
		return JS_FALSE;
	}
	myChar->SetFrozen( true );
	return JS_TRUE;
}

JSBool CChar_Unfreeze( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "(Unfreeze) Invalid Count of Arguments: %d, needs: 0", argc );
		return JS_FALSE;
	}
	CChar *myChar = (CChar *)JS_GetPrivate( cx, obj );

	if( !ValidateObject( myChar ) )
	{
		MethodError( "Unfreeze: Invalid character" );
		return JS_FALSE;
	}
	myChar->SetFrozen( false );
	return JS_TRUE;
}

JSBool CChar_CastSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( ( argc != 1 ) && ( argc != 2 ) )
	{
		MethodError( "(CastSpell) Invalid Number of Arguments %d, needs: 1 or 2", argc );
		return JS_FALSE;
	}

	CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );
	
	if( !ValidateObject( myChar ) )
	{
		MethodError( "(CastSpell) Invalid object assigned" );
		return JS_FALSE;
	}
	
	SI08 spellCast = (SI08)JSVAL_TO_INT( argv[0] );
	bool spellSuccess;

	if( myChar->IsNpc() )
	{
		myChar->SetSpellCast( spellCast );
		Magic->CastSpell( NULL, myChar );
	}
	else
	{
		cSocket *sock = calcSocketObjFromChar( myChar );
		
		if(( argc == 2 ) && ( argv[1] == JSVAL_TRUE ) )
		{
			// Next...
			myChar->SetSpellCast( spellCast );
			Magic->CastSpell( sock, myChar );
		}
		else
			spellSuccess = Magic->SelectSpell( sock, spellCast );
	}
	return JS_TRUE;
}

JSBool CChar_SysMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "SysMessage: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	
	JSString *targMessage = JS_ValueToString( cx, argv[0] );
	char *trgMessage = JS_GetStringBytes( targMessage );
	CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );
	cSocket *targSock = calcSocketObjFromChar( myChar );

	if( targSock == NULL || trgMessage == NULL )
	{
		MethodError( "SysMessage: Invalid socket or speech (%s)", targMessage );
		return JS_FALSE;
	}
	targSock->sysmessage( trgMessage );
	return JS_TRUE;
}

JSBool CChar_MagicEffect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	SI08 spellnum = (SI08)JSVAL_TO_INT( argv[0] );

	CChar *myObj = (CChar*)JS_GetPrivate( cx, obj );
	
	if( !ValidateObject( myObj ) )
	{
		MethodError( "StaticEffect: Invalid Object" );
		return JS_FALSE;
	}

	Magic->doStaticEffect( myObj, spellnum );

	return JS_TRUE;
}

JSBool CChar_MakeMenu( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "MakeMenu: Invalid number of arguments (takes 1, number of menu)" );
		return JS_FALSE;
	}
	CChar *myChar = (CChar*)JS_GetPrivate( cx, obj );
	cSocket *mySock = calcSocketObjFromChar( myChar );

	if( mySock == NULL )
	{
		MethodError( "MakeMenu: invalid socket" );
		return JS_FALSE;
	}
	int menu = JSVAL_TO_INT( argv[0] );
	Skills->NewMakeMenu( mySock, menu, 0 );
	return JS_TRUE;
}

JSBool CChar_GetSerial( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CChar *myObj = static_cast<CChar*>(JS_GetPrivate(cx, obj));
	UI08 part = (UI08)JSVAL_TO_INT( argv[0] );
	
	if( !ValidateObject( myObj ) || ( part <= 0 ) || ( part > 4 ) )
	{
		MethodError( "GetSerial: Invalid Object/Argument, takes 1 arg: part of serial (1-4)" );
		*rval = INT_TO_JSVAL( 0 );
		return JS_FALSE;
	}

	*rval = INT_TO_JSVAL( myObj->GetSerial( part ) );

	return JS_TRUE;
}

JSBool CChar_SetPoisoned( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 2 && argc > 3 )
	{
		MethodError( "(SetPoisoned) Invalid Number of Arguments %d, needs: 2 or 3", argc );
		return JS_FALSE;
	}

	CChar *myChar = (CChar *)JS_GetPrivate( cx, obj );
	
	if( !ValidateObject( myChar ) || myChar->GetObjType() != OT_CHAR )
	{
		MethodError( "(SetPoisoned) Invalid object assigned" );
		return JS_FALSE;
	}
	
	SI08 newVal = (SI08)JSVAL_TO_INT( argv[0] );
	SI32 wearOff = (SI32)JSVAL_TO_INT( argv[1] );
	
	if( argc == 2 || ( argc == 3 && JSVAL_TO_BOOLEAN( argv[2] ) ) )
	{
		if( myChar->GetPoisoned() > newVal )
			newVal = myChar->GetPoisoned();
	}

	myChar->SetPoisonStrength( newVal );
	myChar->SetPoisoned( newVal );
	myChar->SetTimer( tCHAR_POISONWEAROFF, BuildTimeValue( (R32)wearOff / 1000.0f ) );
	return JS_TRUE;
}

JSBool CItem_SetPoison( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 1 && argc > 2 )
	{
		MethodError( "(SetPoison) Invalid Number of Arguments %d, needs: 1 or 2", argc );
		return JS_FALSE;
	}

	CItem *myItem = (CItem *)JS_GetPrivate( cx, obj );
	
	if( !ValidateObject( myItem ) || myItem->GetObjType() != OT_ITEM )
	{
		MethodError( "(SetPoisoned) Invalid object assigned" );
		return JS_FALSE;
	}
	
	UI08 newVal = (UI08)JSVAL_TO_INT( argv[0] );
	
	if( argc == 1 || ( argc == 2 && JSVAL_TO_BOOLEAN( argv[1] ) ) )
	{
		if( myItem->GetPoisoned() > newVal )
			newVal = myItem->GetPoisoned();
	}

	myItem->SetPoisoned( newVal );
	return JS_TRUE;
}

void explodeItem( cSocket *mSock, CItem *nItem );
JSBool CChar_ExplodeItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(ExplodeItem) Invalid Number of Arguments %d, needs: 1", argc );
		return JS_FALSE;
	}

	CChar *myChar = (CChar *)JS_GetPrivate( cx, obj );
	JSObject *tObj = JSVAL_TO_OBJECT( argv[0] );
	cBaseObject *trgObj = (cBaseObject *)JS_GetPrivate( cx, tObj );
	
	if( !ValidateObject( trgObj ) || trgObj->GetObjType() != OT_ITEM || calcSocketObjFromChar( myChar ) == NULL )
	{
		MethodError( "(ExplodeItem) Invalid object" );
		return JS_FALSE;
	}
	
	explodeItem( calcSocketObjFromChar( myChar ), (CItem *)trgObj );
	return JS_TRUE;
}

JSBool CChar_SetInvisible( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc < 1 && argc > 2 )
	{
		MethodError( "(SetInvisible) Invalid Number of Arguments %d, needs: 1 or 2", argc );
		return JS_FALSE;
	}

	CChar *myChar = (CChar *)JS_GetPrivate( cx, obj );
	SI08 newVal = (SI08)JSVAL_TO_INT( argv[0] );

	myChar->SetHidden( newVal );
	if( argc == 2 )
	{
		UI32 TimeOut = (UI32)JSVAL_TO_INT( argv[1] );
		myChar->SetTimer( tCHAR_INVIS, BuildTimeValue( (R32)TimeOut / 1000.0f ) );
	}
	return JS_TRUE;
}

JSBool CItem_GetSerial( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CItem *myObj = (CItem*)JS_GetPrivate( cx, obj );
	UI08 part = (UI08)JSVAL_TO_INT( argv[0] );
	
	if( !ValidateObject( myObj ) || ( part <= 0 ) || ( part > 4 ) )
	{
		MethodError( "GetSerial: Invalid Object/Argument, takes 1 arg: part of serial (1-4)" );
		*rval = INT_TO_JSVAL( 0 );
		return JS_FALSE;
	}

	*rval = INT_TO_JSVAL( myObj->GetSerial( part ) );

	return JS_TRUE;
}

JSBool CItem_SetCont( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	CItem *myItem = (CItem*)JS_GetPrivate( cx, obj );
	JSObject *tObj = JSVAL_TO_OBJECT( argv[0] );
	cBaseObject *trgObj = (cBaseObject *)JS_GetPrivate( cx, tObj );

	if( !ValidateObject( myItem ) || !ValidateObject( trgObj ) || ( trgObj->GetSerial() == INVALIDSERIAL ) )
	{
		MethodError( "SetCont: Invalid Object/Argument, takes 1 arg: containerobject" );
		return JS_FALSE;
	}
	
	// return true if the change was successful, false otherwise
	*rval = BOOLEAN_TO_JSVAL( myItem->SetCont( trgObj ) );
	
	return JS_TRUE;
}

JSBool CItem_IsMulti( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "(IsMulti) Invalid Number of Arguments %d, needs: 0", argc );
		*rval = JS_FALSE;
		return JS_TRUE;
	}

	CItem *myItem = (CItem *)JS_GetPrivate( cx, obj );
	
	if( !ValidateObject( myItem ) )
	{
		MethodError( "(IsMulti) Invalid object assigned" );
		*rval = JS_FALSE;
		return JS_TRUE;
	}
	
	*rval = BOOLEAN_TO_JSVAL( myItem->CanBeObjType( OT_MULTI ) );
	return JS_TRUE;
}

JSBool CItem_IsInMulti( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(IsInMulti) Invalid Number of Arguments %d, needs: 1", argc );
		*rval = JS_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = (CMultiObj *)JS_GetPrivate( cx, obj );
	
	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ) )
	{
		MethodError( "(IsInMulti) Invalid object assigned" );
		*rval = JS_FALSE;
		return JS_TRUE;
	}
	cBaseObject *toFind = (cBaseObject *)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
	if( !ValidateObject( toFind ) )
	{
		MethodError( "(IsInMulti) Invalid object in house" );
		*rval = JS_FALSE;
		return JS_TRUE;
	}

	*rval = BOOLEAN_TO_JSVAL( myItem->IsInMulti( toFind ) );
	return JS_TRUE;
}
JSBool CItem_IsOnBanList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(IsOnBanList) Invalid Number of Arguments %d, needs: 1", argc );
		*rval = JS_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = (CMultiObj *)JS_GetPrivate( cx, obj );
	
	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ) )
	{
		MethodError( "(IsOnBanList) Invalid object assigned" );
		*rval = JS_FALSE;
		return JS_TRUE;
	}
	CChar *toFind = (CChar *)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
	if( !ValidateObject( toFind ) )
	{
		MethodError( "(IsOnBanList) Invalid character" );
		*rval = JS_FALSE;
		return JS_TRUE;
	}
	
	*rval = BOOLEAN_TO_JSVAL( myItem->IsOnBanList( toFind ) );
	return JS_TRUE;
}
JSBool CItem_IsOnOwnerList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(IsOnOwnerList) Invalid Number of Arguments %d, needs: 1 or 2", argc );
		*rval = JS_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = (CMultiObj *)JS_GetPrivate( cx, obj );
	
	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ) )
	{
		MethodError( "(IsOnOwnerList) Invalid object assigned" );
		*rval = JS_FALSE;
		return JS_TRUE;
	}
	CChar *toFind = (CChar *)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
	if( !ValidateObject( toFind ) )
	{
		MethodError( "(IsOnOwnerList) Invalid character" );
		*rval = JS_FALSE;
		return JS_TRUE;
	}
	
	*rval = BOOLEAN_TO_JSVAL( myItem->IsOwner( toFind ) );
	return JS_TRUE;
}

JSBool CItem_AddToBanList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(AddToBanList) Invalid Number of Arguments %d, needs: 1", argc );
		return JS_FALSE;
	}

	CMultiObj *myItem = (CMultiObj *)JS_GetPrivate( cx, obj );
	
	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ) )
	{
		MethodError( "(AddToBanList) Invalid object assigned" );
		return JS_FALSE;
	}
	CChar *toFind = (CChar *)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
	if( !ValidateObject( toFind ) )
	{
		MethodError( "(AddToBanList) Invalid character" );
		return JS_FALSE;
	}
	
	*rval = JS_TRUE;
	myItem->AddToBanList( toFind );
	return JS_TRUE;
}

JSBool CItem_AddToOwnerList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(AddToOwnerList) Invalid Number of Arguments %d, needs: 1", argc );
		*rval = JS_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = (CMultiObj *)JS_GetPrivate( cx, obj );
	
	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ) )
	{
		MethodError( "(AddToOwnerList) Invalid object assigned" );
		*rval = JS_FALSE;
		return JS_TRUE;
	}
	CChar *toFind = (CChar *)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
	if( !ValidateObject( toFind ) )
	{
		MethodError( "(AddToOwnerList) Invalid character" );
		*rval = JS_FALSE;
		return JS_TRUE;
	}

	*rval = JS_TRUE;
	myItem->AddAsOwner( toFind );
	return JS_TRUE;
}
JSBool CItem_RemoveFromBanList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(RemoveFromBanList) Invalid Number of Arguments %d, needs: 1", argc );
		*rval = JS_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = (CMultiObj *)JS_GetPrivate( cx, obj );
	
	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ) )
	{
		MethodError( "(RemoveFromBanList) Invalid object assigned" );
		*rval = JS_FALSE;
		return JS_TRUE;
	}
	CChar *toFind = (CChar *)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
	if( !ValidateObject( toFind ) )
	{
		MethodError( "(RemoveFromBanList) Invalid character" );
		*rval = JS_FALSE;
		return JS_TRUE;
	}
	
	*rval = JS_TRUE;
	myItem->RemoveFromBanList( toFind );
	return JS_TRUE;
}
JSBool CItem_RemoveFromOwnerList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "(RemoveFromOwnerList) Invalid Number of Arguments %d, needs: 1", argc );
		*rval = JS_FALSE;
		return JS_TRUE;
	}

	CMultiObj *myItem = (CMultiObj *)JS_GetPrivate( cx, obj );
	
	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_MULTI ) )
	{
		MethodError( "(RemoveFromOwnerList) Invalid object assigned" );
		*rval = JS_FALSE;
		return JS_TRUE;
	}
	CChar *toFind = (CChar *)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
	if( !ValidateObject( toFind ) )
	{
		MethodError( "(RemoveFromOwnerList) Invalid character" );
		*rval = JS_FALSE;
		return JS_TRUE;
	}
	
	*rval = JS_TRUE;
	myItem->RemoveAsOwner( toFind );
	return JS_TRUE;
}
JSBool CItem_PlaceInPack( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "(PlaceInPack) Invalid Number of Arguments %d, needs: 0", argc );
		return JS_FALSE;
	}
	CItem *myItem = (CItem *)JS_GetPrivate( cx, obj );
	if( !ValidateObject( myItem ) || !myItem->CanBeObjType( OT_ITEM ) )
	{
		MethodError( "(PlaceInPack) Invalid object assigned" );
		return JS_FALSE;
	}
	myItem->PlaceInPack();
	return JS_TRUE;
}

JSBool CSocket_OpenURL( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
    if( argc != 1 ) // 3 parameters
    {
        MethodError( "OpenURL: Invalid Number of Arguments %d, needs: 1" );
        return JS_FALSE;
    }
    cSocket *mySock = (cSocket*)JS_GetPrivate( cx, obj );

    if( mySock == NULL )
    {
        MethodError( "OpenURL: Invalid socket!" );
        return JS_FALSE;
    }
	std::string url = JS_GetStringBytes( JSVAL_TO_STRING( argv[0] ) );
    mySock->OpenURL( url );
    return JS_TRUE;
}

JSBool CSocket_GetByte( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
    if( argc != 1 ) // 1 parameters
    {
        MethodError( "GetByte: Invalid Number of Arguments %d, needs: 1 (offset)" );
        return JS_FALSE;
    }
    cSocket *mySock = (cSocket*)JS_GetPrivate( cx, obj );

    if( mySock == NULL )
    {
        MethodError( "GetByte: Invalid socket!");
        return JS_FALSE;
    }
	int offset	= JSVAL_TO_INT( argv[0] );
	*rval		= INT_TO_JSVAL( mySock->GetByte( offset ) );
    return JS_TRUE;
}
JSBool CSocket_GetWord( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
    if( argc != 1 ) // 1 parameters
    {
        MethodError( "GetWord: Invalid Number of Arguments %d, needs: 1 (offset)" );
        return JS_FALSE;
    }
    cSocket *mySock = (cSocket*)JS_GetPrivate( cx, obj );

    if( mySock == NULL )
    {
        MethodError( "GetWord: Invalid socket!");
        return JS_FALSE;
    }
	int offset = JSVAL_TO_INT( argv[0] );
	*rval = INT_TO_JSVAL( mySock->GetWord( offset ) );
    return JS_TRUE;
}
JSBool CSocket_WhoList( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
    cSocket *mySock = (cSocket*)JS_GetPrivate( cx, obj );

    if( mySock == NULL )
    {
        MethodError( "WhoList: Invalid socket!");
        return JS_FALSE;
    }

	bool sendOnList = true;
	if( argc == 1 )
		sendOnList = ( JSVAL_TO_BOOLEAN( argv[0] ) == JS_TRUE );

	if( sendOnList )
		WhoList->SendSocket( mySock );
	else
		OffList->SendSocket( mySock );

	return JS_TRUE;
}

JSBool CSocket_Midi( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Midi: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	JSClass *myClass = JS_GetClass( obj );

	UI16 midi = (UI16)JSVAL_TO_INT( argv[0] );

	cSocket *mySock = (cSocket*)JS_GetPrivate( cx, obj );

	if( mySock != NULL )
		Effects->playMidi( mySock, midi );

	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CChar_YellMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	January 30, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Yells a text message to those in range
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
JSBool CChar_YellMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "YellMessage: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	//JSClass *myClass = JS_GetClass( obj );
	cBaseObject *myObj = (cBaseObject*)JS_GetPrivate( cx, obj );

	JSString *targMessage	= JSVAL_TO_STRING( argv[0] );
	char *trgMessage		= JS_GetStringBytes( targMessage );

	if( trgMessage == NULL )
	{
		MethodError( "YellMessage: You have to supply a messagetext" );
	}
	
	CChar *myChar = (CChar*)myObj;
	if( !ValidateObject( myChar ) )  
	{
		MethodError( "YellMessage: Invalid Character" );
		return JS_FALSE;
	}

	if( myChar->GetNPCAiType() == aiEVIL )
		MethodSpeech( *myChar, trgMessage, YELL, 0x0026, (FontType)myChar->GetFontType() );
	else
		MethodSpeech( *myChar, trgMessage, YELL, myChar->GetSayColour(), (FontType)myChar->GetFontType() );

	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CChar_WhisperMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	January 30, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Whispers a text message to those in range
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	
JSBool CChar_WhisperMessage( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "WhisperMessage: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	//JSClass *myClass = JS_GetClass( obj );
	cBaseObject *myObj = (cBaseObject*)JS_GetPrivate( cx, obj );

	JSString *targMessage = JSVAL_TO_STRING( argv[0] );
	char *trgMessage = JS_GetStringBytes( targMessage );

	if( trgMessage == NULL )
	{
		MethodError( "WhisperMessage: You have to supply a messagetext" );
	}
	
	CChar *myChar = (CChar*)myObj;
	if( !ValidateObject( myChar ) )  
	{
		MethodError( "WhisperMessage: Invalid Character" );
		return JS_FALSE;
	}

	if( myChar->GetNPCAiType() == aiEVIL )
		MethodSpeech( *myChar, trgMessage, WHISPER, 0x0026, (FontType)myChar->GetFontType() );
	else
		MethodSpeech( *myChar, trgMessage, WHISPER, myChar->GetSayColour(), (FontType)myChar->GetFontType() );

	return JS_TRUE;
}

void BuildGumpFromScripts( cSocket *s, UI16 m );

JSBool CSocket_OpenGump( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "OpenGump: Invalid number of arguments (takes 1, number)" );
		return JS_FALSE;
	}

	if( !JSVAL_IS_INT( argv[0] ) )
	{
		MethodError( "You have to pass a valid menu number" );
	}

	int menuNumber = JSVAL_TO_INT( argv[0] );
	cSocket *mySock = (cSocket *)JS_GetPrivate( cx, obj );

	if( mySock == NULL )
	{
		MethodError( "OpenGump: Unknown Object has been passed" );
		return JS_FALSE;
	}
	
	BuildGumpFromScripts( mySock, menuNumber );

	return JS_TRUE;
}

// Race methods


JSBool CRace_CanWearArmour( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "CanWearArmour: Invalid number of arguments (takes 1, number)" );
		return JS_FALSE;
	}

	CRace *myRace = (CRace *)JS_GetPrivate( cx, obj );
	if( myRace == NULL )
	{
		MethodError( "CanWearArmour: Unknown Object has been passed" );
		return JS_FALSE;
	}

	CItem *toFind = (CItem *)JS_GetPrivate( cx, JSVAL_TO_OBJECT( argv[0] ) );
	if( !ValidateObject( toFind ) )
	{
		MethodError( "CanWearArmour: Invalid item passed" );
		return JS_TRUE;
	}
	
	ARMORCLASS srcClass = myRace->ArmourClassRestriction();
	ARMORCLASS trgClass = toFind->GetArmourClass();
	*rval = BOOLEAN_TO_JSVAL( ( trgClass == 0 ) || ( ( srcClass & trgClass ) != 0 ) );	// they have a matching class
	return JS_TRUE;
}
JSBool CRace_IsValidHairColour( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "IsValidHairColour: Invalid number of arguments (takes 1, number)" );
		return JS_FALSE;
	}

	CRace *myRace = (CRace *)JS_GetPrivate( cx, obj );
	if( myRace == NULL )
	{
		MethodError( "IsValidHairColour: Unknown Object has been passed" );
		return JS_FALSE;
	}
	
	COLOUR cVal = (COLOUR)JSVAL_TO_INT( argv[0] );
	*rval = BOOLEAN_TO_JSVAL( ( myRace->IsValidHair( cVal ) ) );
	return JS_TRUE;
}
JSBool CRace_IsValidSkinColour( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "IsValidSkinColour: Invalid number of arguments (takes 1, number)" );
		return JS_FALSE;
	}

	CRace *myRace = (CRace *)JS_GetPrivate( cx, obj );
	if( myRace == NULL )
	{
		MethodError( "IsValidSkinColour: Unknown Object has been passed" );
		return JS_FALSE;
	}
	
	COLOUR cVal = (COLOUR)JSVAL_TO_INT( argv[0] );
	*rval = BOOLEAN_TO_JSVAL( ( myRace->IsValidSkin( cVal ) ) );
	return JS_TRUE;
}
JSBool CRace_IsValidBeardColour( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "IsValidBeardColour: Invalid number of arguments (takes 1, number)" );
		return JS_FALSE;
	}

	CRace *myRace = (CRace *)JS_GetPrivate( cx, obj );
	if( myRace == NULL )
	{
		MethodError( "IsValidBeardColour: Unknown Object has been passed" );
		return JS_FALSE;
	}
	
	COLOUR cVal = (COLOUR)JSVAL_TO_INT( argv[0] );
	*rval = BOOLEAN_TO_JSVAL( ( myRace->IsValidBeard( cVal ) ) );
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CBase_ApplySection( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	23 June, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Apply's a DFN section to an Item/Character
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o	

bool ApplyItemSection( CItem *applyTo, ScriptSection *toApply );

JSBool CBase_ApplySection( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "ApplySection: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	JSClass *myClass		= JS_GetClass( obj );
	cBaseObject *myObj		= (cBaseObject*)JS_GetPrivate( cx, obj );
	std::string trgSection	= JS_GetStringBytes( JSVAL_TO_STRING( argv[0] ) );

	if( trgSection.empty() || trgSection.length() == 0)
	{
		MethodError( "You have to supply a section to apply" );
	}
	
	if( !strcmp( myClass->name, "UOXItem" ) ) 
	{
		CItem *myItem = (CItem*)myObj;
		if( !ValidateObject( myItem ) )  
		{
			MethodError( "ApplySection: Invalid Item" );
			return JS_FALSE;
		}
		ScriptSection *toFind = FileLookup->FindEntry( trgSection, items_def );
		ApplyItemSection( myItem, toFind );
	}
	else if( !strcmp( myClass->name, "UOXChar" ) ) 
	{
		CChar *myChar = (CChar*)myObj;
		if( !ValidateObject( myChar ) )  
		{
			MethodError( "ApplySection: Invalid Character" );
			return JS_FALSE;
		}

		ScriptSection *toFind = FileLookup->FindEntry( trgSection, npc_def );
		Npcs->ApplyNpcSection( myChar, toFind );
	}	

	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CChar_AddSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	23 June, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Adds a spell to the character's spell book
//o--------------------------------------------------------------------------o
//|	Returns			-	True
//o--------------------------------------------------------------------------o	
JSBool CChar_AddSpell( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "AddSpell: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	CChar *myChar	= (CChar *)JS_GetPrivate( cx, obj );
	UI08 spellNum	= (UI08)JSVAL_TO_INT( argv[0] );
	CItem *sBook	= FindItemOfType( myChar, IT_SPELLBOOK );
	if( ValidateObject( sBook ) )
		Magic->AddSpell( sBook, spellNum );
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CChar_SpellFail( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	23 June, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Does the actions associated with spell failure
//o--------------------------------------------------------------------------o
//|	Returns			-	True
//o--------------------------------------------------------------------------o	
JSBool CChar_SpellFail( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "AddSpell: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}

	CChar *myChar = (CChar *)JS_GetPrivate( cx, obj );

	Effects->PlayStaticAnimation( myChar, 0x3735, 0, 30 );
	Effects->PlaySound( myChar, 0x005C );
	if( !myChar->IsNpc() )
	{
		cSocket *mSock = calcSocketObjFromChar( myChar );
		if( mSock != NULL )
			myChar->emote( mSock, 771, false );
	}
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CItem_Refresh( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	23 June, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Causes the item to be refreshed to sockets
//o--------------------------------------------------------------------------o
//|	Returns			-	True
//o--------------------------------------------------------------------------o	
JSBool CItem_Refresh( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Refresh: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}

	CItem *myItem = (CItem *)JS_GetPrivate( cx, obj );
	myItem->Dirty( UT_UPDATE );

	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CItem_ApplyRank( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	23 June, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Applies the specified rank to the item
//o--------------------------------------------------------------------------o
//|	Returns			-	True
//o--------------------------------------------------------------------------o	
JSBool CItem_ApplyRank( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "ApplyRank: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	CItem *myItem	= (CItem *)JS_GetPrivate( cx, obj );
	int rank		= JSVAL_TO_INT( argv[0] );

	Skills->ApplyRank( NULL, myItem, rank );
	return JS_TRUE;
}

JSBool CAccount_GetAccount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{ 
	return JS_FALSE;
}

JSBool CAccount_SetAccount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{ 
	return JS_FALSE;
}

//o--------------------------------------------------------------------------o
//|	Function		-	JSBool CAccount_AddAccount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	8/15/2003 10:43:18 PM
//|	Developers		-	Matthew J. Randall / mrandall (mrandall@adtastik.net)
//|	Organization	-	MyndTrip Technologies / MyndTrip Studios / EAWare
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Add an account through the JS. The calling script will
//|							be required to specifiy the username, password, flags, 
//|							and comment/contact information.
//o--------------------------------------------------------------------------o
//| Modifications	-	Removed UOXAccountWrapper and exposed global var Accounts
//o--------------------------------------------------------------------------o
JSBool CAccount_AddAccount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{ 
	if( argc != 4 )
	{
		MethodError( "Account.AddAccount(user,pass,email,flags): Invalid number of arguments (takes 4)" );
		return JS_FALSE;
	}
	// Ok get our object from the global context
	if( !JSVAL_IS_STRING(argv[0]) || !JSVAL_IS_STRING(argv[1]) || !JSVAL_IS_STRING(argv[2]) || !( JSVAL_IS_INT( argv[3] ) || JSVAL_IS_STRING( argv[3] ) ) )
	{
		MethodError( "Account.AddAccount(user,pass,email,flags): Invalid parameter specifled, please check param types." );
		return JS_FALSE;
	}
	std::string lpszUsername	= JS_GetStringBytes( JSVAL_TO_STRING( argv[0] ) );
	std::string lpszPassword	= JS_GetStringBytes( JSVAL_TO_STRING( argv[1] ) );
	std::string lpszComment		= JS_GetStringBytes( JSVAL_TO_STRING( argv[2] ) );
	UI16 u16Flags		= 0;
	
	if( JSVAL_IS_INT( argv[3] ) )
		u16Flags = (UI16)JSVAL_TO_INT( argv[3] );
	else
		u16Flags = UString( JS_GetStringBytes( JSVAL_TO_STRING( argv[3] ) ) ).toUShort();

	if( lpszUsername.empty() || lpszPassword.empty() || lpszComment.empty() || lpszUsername.length() == 0 || lpszPassword.length() == 0 || lpszComment.length() == 0 )
		return JS_FALSE;

	
	if( !Accounts->AddAccount( lpszUsername, lpszPassword, lpszComment, u16Flags ) )
	{
		MethodError( "Account.AddAccount( user, pass, email, flags ): Account was not added. Duplicate expected." );
		return JS_FALSE;
	}
	return JS_TRUE;
}
//o--------------------------------------------------------------------------o
//|	Function		-	JSBool CAccount_DelAccount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	8/21/2003 2:39:27 PM
//|	Developers		-	Matthew J. Randall / mrandall (mrandall@adtastik.net)
//|	Organization	-	MyndTrip Technologies / Stiffed-Attic Software / EAWare
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Remove an account from the memory. This is a perminent 
//|							action that CAN NOT be undone. After a call to this 
//|							function the account will be immidiatly removed from
//|							memory, and will not be written out to the accounts.adm
//|							file, and will have the associated directory tagged and
//|							renamed. 
//o--------------------------------------------------------------------------o
//| Modifications	-	Removed UOXAccountWrapper and exposed global var Accounts
//o--------------------------------------------------------------------------o
JSBool CAccount_DelAccount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{ 
	if( argc != 1 )
	{
		MethodError( "Account.DelAccount([username/id]): Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	char	*lpszUsername = NULL;
	UI16	ui16AccountID;
	// Ok get out object from the global context
	if( JSVAL_IS_STRING(argv[0]) )
	{
		lpszUsername = JS_GetStringBytes( JSVAL_TO_STRING( argv[0] ) );
		if( !Accounts->DelAccount( lpszUsername ) )
		{
			MethodError(" Account.DelAccount(username): Unable to remove account specified.");
			return JS_FALSE;
		}

	}	
	else if( JSVAL_IS_INT( argv[0] ) )
	{
		ui16AccountID = (UI16)JSVAL_TO_INT(argv[0]);
		if( !Accounts->DelAccount( ui16AccountID ) )
		{
			MethodError(" Account.DelAccount(accountID): Unable to remove account specified.");
			return JS_FALSE;
		}
	}
	else
	{
		return JS_FALSE;
	}
	return JS_TRUE;
}

JSBool CAccount_ModAccount( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{ 
	return JS_FALSE;
}

JSBool CAccount_SaveAccounts( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{ 
	return JS_FALSE;
}

// Basic file wrapping structure for abstracting away file IO for the JS file funcs
// UOXFile constructor !
JSBool UOXCFile( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	UOXFileWrapper *toAdd = new UOXFileWrapper;
	toAdd->mWrap = NULL;

	JS_SetPrivate( cx, obj, toAdd );
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CFile_Free( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	23 June, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Frees memory allocated by file
//o--------------------------------------------------------------------------o
//|	Returns			-	True
//o--------------------------------------------------------------------------o	
JSBool CFile_Free( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Free: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	UOXFileWrapper *mFile	= (UOXFileWrapper *)JS_GetPrivate( cx, obj );
	delete mFile;
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CFile_Open( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	23 June, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Opens a file
//o--------------------------------------------------------------------------o
//|	Returns			-	True
//o--------------------------------------------------------------------------o	
JSBool CFile_Open( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "Open: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	UOXFileWrapper *mFile	= (UOXFileWrapper *)JS_GetPrivate( cx, obj );

	char *filename	= JS_GetStringBytes( JSVAL_TO_STRING( argv[0] ) );
	char *mode		= JS_GetStringBytes( JSVAL_TO_STRING( argv[1] ) );
	strlwr( mode );
	if( mode[0] != 'r' && mode[0] != 'w' && mode[0] != 'a' )
	{
		MethodError( "Open: Invalid mode must be \"read\", \"write\", or \"append\"!" );
		return JS_FALSE;
	}
	if( strstr( filename, ".." ) || strstr( filename, "\\" ) || strstr( filename, "/" ) )
	{
		MethodError( "Open: file names may not contain \"..\", \"\\\", or \"/\"." );
		return JS_FALSE;
	}
	mode[1] = 0;

	mFile->mWrap = fopen( filename, mode );

	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CFile_Close( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	23 June, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Closes a file
//o--------------------------------------------------------------------------o
//|	Returns			-	True
//o--------------------------------------------------------------------------o	
JSBool CFile_Close( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "Open: Invalid number of arguments (takes 0)" );
		return JS_FALSE;
	}
	UOXFileWrapper *mFile	= (UOXFileWrapper *)JS_GetPrivate( cx, obj );

	fclose( mFile->mWrap );
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CFile_Read( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	23 June, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Reads from a file
//o--------------------------------------------------------------------------o
//|	Returns			-	True
//o--------------------------------------------------------------------------o	
JSBool CFile_Read( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Read: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	UOXFileWrapper *mFile	= (UOXFileWrapper *)JS_GetPrivate( cx, obj );

	if( !mFile || !mFile->mWrap || feof( mFile->mWrap ) )
	{
		MethodError( "Read: Error reading file, is not opened or has reached EOF" );
		return JS_FALSE;
	}
	int bytes = JSVAL_TO_INT( argv[0] );
	char data[512];

	if( bytes > 512 || bytes < 1 )
	{
		MethodError( "Read: Invalid byte count, must be from 1 to 512!" );
		return JS_FALSE;
	}

	fread( data, 1, bytes, mFile->mWrap );

	*rval = STRING_TO_JSVAL( JS_NewStringCopyZ( cx, data ) );
	return JS_TRUE;
}


//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CFile_ReadUntil( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	23 June, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Reads from a file
//o--------------------------------------------------------------------------o
//|	Returns			-	True
//o--------------------------------------------------------------------------o	
JSBool CFile_ReadUntil( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "ReadUntil: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	UOXFileWrapper *mFile	= (UOXFileWrapper *)JS_GetPrivate( cx, obj );

	if( !mFile || !mFile->mWrap || feof( mFile->mWrap ) )
	{
		MethodError( "ReadUntil: Error reading file, is not opened or has reached EOF" );
		return JS_FALSE;
	}
	char *until = JS_GetStringBytes( JSVAL_TO_STRING( argv[0] ) );
	char line[512];
	int c;

	if( until[0] == '\\' && strlen( until ) > 1 )
	{
		switch( until[1] )
		{
			case '\\': until[0] = '\\'; break;
			case '\'': until[0] = '\''; break;
			case '\"': until[0] = '\"'; break;
			case 'n':  until[0] = '\n'; break;
			default: MethodError( "ReadUntil: Unsupported character escape sequence %s", until );	break;
		}
	}

	for( c = 0; c < 512 && !feof( mFile->mWrap ); ++c )
	{
		line[c] = fgetc( mFile->mWrap );
		if( line[c] == until[0] || line[c] == '\n' )
			break;
	}
	line[c<512 ? c : 511] = 0;

	*rval = STRING_TO_JSVAL( JS_NewStringCopyZ( cx, line ) );
	return JS_TRUE;
}


//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CFile_Write( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	23 June, 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Writes to a file
//o--------------------------------------------------------------------------o
//|	Returns			-	True
//o--------------------------------------------------------------------------o	
JSBool CFile_Write( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Write: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	UOXFileWrapper *mFile	= (UOXFileWrapper *)JS_GetPrivate( cx, obj );

	if( !mFile || !mFile->mWrap || feof( mFile->mWrap ) )
	{
		MethodError( "Write: Error writing to file, file was not opened sucessfully!" );
		return JS_FALSE;
	} 
	else if( ftell( mFile->mWrap ) > 1049600 ) 
	{
		MethodError( "Write: Error writing to file.  File my not exceed 1mb." );
		return JS_FALSE;
	}

	char *str = JS_GetStringBytes( JSVAL_TO_STRING( argv[0] ) );
	fprintf( mFile->mWrap, str );
	
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CBase_FirstItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	02 Aug 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Returns first object in the object's list
//o--------------------------------------------------------------------------o
//|	Returns			-	True if script can execute, false otherwise
//o--------------------------------------------------------------------------o	
JSBool CBase_FirstItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "KillTimers: Invalid count of arguments :%d, needs :0", argc );
		return JS_FALSE;
	}
	cBaseObject *myObj = static_cast<cBaseObject*>( JS_GetPrivate( cx, obj ) );
	if( !ValidateObject( myObj ) )
	{
		MethodError( "FirstItem: Invalid object assigned." );
		return JS_FALSE;
	}
	CItem *firstItem = NULL;
	if( myObj->GetObjType() == OT_CHAR )
		firstItem = ( static_cast< CChar * >(myObj) )->FirstItem();
	else
		firstItem = ( static_cast< CItem * >(myObj) )->FirstItem();
	if( ValidateObject( firstItem ) )
	{
		cScript *myScript	= Trigger->GetAssociatedScript( JS_GetGlobalObject( cx ) );
		JSObject *myObj		= myScript->AcquireObject( IUE_ITEM );
		JS_SetPrivate( cx, myObj, firstItem );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CBase_NextItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	02 Aug 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Returns next object in the object's list
//o--------------------------------------------------------------------------o
//|	Returns			-	True if script can execute, false otherwise
//o--------------------------------------------------------------------------o	
JSBool CBase_NextItem( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "KillTimers: Invalid count of arguments :%d, needs :0", argc );
		return JS_FALSE;
	}
	cBaseObject *myObj = static_cast<cBaseObject*>( JS_GetPrivate( cx, obj ) );
	if( !ValidateObject( myObj ) )
	{
		MethodError( "NextItem: Invalid object assigned." );
		return JS_FALSE;
	}
	CItem *firstItem = NULL;
	if( myObj->GetObjType() == OT_CHAR )
		firstItem = ( static_cast< CChar * >(myObj) )->NextItem();
	else
		firstItem = ( static_cast< CItem * >(myObj) )->NextItem();
	if( ValidateObject( firstItem ) )
	{
		cScript *myScript	= Trigger->GetAssociatedScript( JS_GetGlobalObject( cx ) );
		JSObject *myObj		= myScript->AcquireObject( IUE_ITEM );
		JS_SetPrivate( cx, myObj, firstItem );
		*rval = OBJECT_TO_JSVAL( myObj );
	}
	else
		*rval = JSVAL_NULL;
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CBase_FinishedItems( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
//|	Date			-	02 Aug 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Returns true if finished all items in object's list
//o--------------------------------------------------------------------------o
//|	Returns			-	True if script can execute, false otherwise
//o--------------------------------------------------------------------------o	
JSBool CBase_FinishedItems( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 0 )
	{
		MethodError( "KillTimers: Invalid count of arguments :%d, needs :0", argc );
		return JS_FALSE;
	}
	cBaseObject *myObj = static_cast<cBaseObject*>( JS_GetPrivate( cx, obj ) );
	if( !ValidateObject( myObj ) )
	{
		MethodError( "NextItem: Invalid object assigned." );
		return JS_FALSE;
	}
	if( myObj->GetObjType() == OT_CHAR )
		*rval = BOOLEAN_TO_JSVAL( ( static_cast< CChar * >(myObj) )->FinishedItems() );
	else
		*rval = BOOLEAN_TO_JSVAL( ( static_cast< CItem * >(myObj) )->FinishedItems() );
	return JS_TRUE;
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	JSBool CChar_WalkTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
//|	Date			-	06 Sep 2003
//|	Developer(s)	-	Maarc
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	Complete
//o--------------------------------------------------------------------------o
//|	Description		-	Begins pathfinding for a character
//o--------------------------------------------------------------------------o
//|	Returns			-	Nothing
//o--------------------------------------------------------------------------o	
JSBool CChar_WalkTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 && argc != 3 )
	{
		MethodError( "WalkTo: Invalid number of arguments (takes 2 or 3)" );
		return JS_FALSE;
	}
	CChar *cMove = static_cast<CChar*>( JS_GetPrivate( cx, obj ) );
	if( !ValidateObject( cMove ) )
	{
		MethodError( "WalkTo: Invalid source character" );
		return JS_FALSE;
	}
	SI16 gx			= 0;
	SI16 gy			= 0;
	UI08 maxSteps	= 0;
	switch( argc )
	{
		case 2:
			if( JSVAL_IS_OBJECT( argv[0] ) )
			{	// we can work with this, it should be either a character or item, hopefully
				JSObject *jsToGoTo	= JSVAL_TO_OBJECT( argv[0] );
				cBaseObject *toGoTo = (cBaseObject *)JS_GetPrivate( cx, jsToGoTo );
				if( !ValidateObject( toGoTo ) )
				{
					MethodError( "No object associated with this object" );
					break;
				}
				JSClass *myClass = JS_GetClass( jsToGoTo );
				if( !strcmp( myClass->name, "UOXItem" ) || !strcmp( myClass->name, "UOXChar" ) )
				{
					gx		= toGoTo->GetX();
					gy		= toGoTo->GetY();
				}
				else if( !strcmp( myClass->name, "UOXSocket" ) )
				{
					cSocket *mySock		= (cSocket *)toGoTo;
					CChar *mySockChar	= mySock->CurrcharObj();
					gx		= mySockChar->GetX();
					gy		= mySockChar->GetY();
				}
				else
				{
					MethodError( "Invalid class of object" );
					break;
				}
				maxSteps = (UI08)JSVAL_TO_INT( argv[1] );
				break;
			}
			MethodError( "Text-styled Parameters may be added later" );
			break;

		// 2 Parameters, x + y
		case 3: 
			gx			= (SI16)JSVAL_TO_INT( argv[0] );
			gy			= (SI16)JSVAL_TO_INT( argv[1] );
			maxSteps	= (UI08)JSVAL_TO_INT( argv[2] );
			break;
		default:
			MethodError( "Invalid number of arguments passed to WalkTo, needs either 2 or 3" );
			break;
	}

	cMove->FlushPath();
#ifdef _DEBUG
	Console.Print( "WalkTo: Moving character %i to (%i,%i) with a maximum of %i steps", cMove->GetSerial(), gx, gy, maxSteps );
#endif
	cMove->SetOldNpcWander( cMove->GetNpcWander() );
	cMove->SetNpcWander( 6 );
	Movement->PathFind( cMove, gx, gy, false, maxSteps );
	return JS_TRUE;
}

JSBool CChar_RunTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 && argc != 3 )
	{
		MethodError( "RunTo: Invalid number of arguments (takes 2 or 3)" );
		return JS_FALSE;
	}
	CChar *cMove = static_cast<CChar*>( JS_GetPrivate( cx, obj ) );
	if( !ValidateObject( cMove ) )
	{
		MethodError( "RunTo: Invalid source character" );
		return JS_FALSE;
	}
	UI16 gx			= 0;
	UI16 gy			= 0;
	UI08 maxSteps	= 0;
	switch( argc )
	{
		case 2:
			if( JSVAL_IS_OBJECT( argv[0] ) )
			{	// we can work with this, it should be either a character or item, hopefully
				JSObject *jsToGoTo	= JSVAL_TO_OBJECT( argv[0] );
				cBaseObject *toGoTo = (cBaseObject *)JS_GetPrivate( cx, jsToGoTo );
				if( !ValidateObject( toGoTo ) )
				{
					MethodError( "No object associated with this object" );
					break;
				}
				JSClass *myClass = JS_GetClass( jsToGoTo );
				if( !strcmp( myClass->name, "UOXItem" ) || !strcmp( myClass->name, "UOXChar" ) )
				{
					gx		= toGoTo->GetX();
					gy		= toGoTo->GetY();
				}
				else if( !strcmp( myClass->name, "UOXSocket" ) )
				{
					cSocket *mySock		= (cSocket *)toGoTo;
					CChar *mySockChar	= mySock->CurrcharObj();
					gx		= mySockChar->GetX();
					gy		= mySockChar->GetY();
				}
				else
				{
					MethodError( "Invalid class of object" );
					break;
				}
				maxSteps = (UI08)JSVAL_TO_INT( argv[1] );
				break;
			}
			MethodError( "Text-styled Parameters may be added later" );
			break;

		// 2 Parameters, x + y
		case 3: 
			gx			= (SI16)JSVAL_TO_INT( argv[0] );
			gy			= (SI16)JSVAL_TO_INT( argv[1] );
			maxSteps	= (UI08)JSVAL_TO_INT( argv[2] );
			break;
		default:
			MethodError( "Invalid number of arguments passed to RunTo, needs either 2 or 3" );
			break;
	}

	cMove->FlushPath();
#ifdef _DEBUG
	Console.Print( "RunTo: Moving character %i to (%i,%i) with a maximum of %i steps", cMove->GetSerial(), gx, gy, maxSteps );
#endif
	cMove->SetOldNpcWander( cMove->GetNpcWander() );
	cMove->SetNpcWander( 6 );
	Movement->PathFind( cMove, gx, gy, true, maxSteps );
	return JS_TRUE;
}

JSBool CChar_GetTimer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "GetTimer: Invalid number of arguments (takes 1)" );
		return JS_FALSE;
	}
	JSEncapsulate encaps( cx, &(argv[0]) );
	CChar *cMove = static_cast<CChar*>( JS_GetPrivate( cx, obj ) );
	if( !ValidateObject( cMove ) )
	{
		MethodError( "GetTimer: Invalid source character" );
		return JS_FALSE;
	}

	*rval = INT_TO_JSVAL( cMove->GetTimer( (cC_TID)encaps.toInt() ) );
	return JS_TRUE;
}

JSBool CChar_SetTimer( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 2 )
	{
		MethodError( "SetTimer: Invalid number of arguments (takes 2)" );
		return JS_FALSE;
	}
	JSEncapsulate encaps( cx, &(argv[0]) );
	JSEncapsulate encaps2( cx, &(argv[1]) );

	CChar *cMove = static_cast<CChar*>( JS_GetPrivate( cx, obj ) );
	if( !ValidateObject( cMove ) )
	{
		MethodError( "SetTimer: Invalid source character" );
		return JS_FALSE;
	}

	cMove->SetTimer( (cC_TID)encaps.toInt(), BuildTimeValue( encaps2.toFloat() / 1000.0f ) );
	return JS_TRUE;
}

JSBool CBase_DistanceTo( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "DistanceTo: Invalid number of arguments (takes 1, game object)" );
		return JS_FALSE;
	}

	JSObject *jsObj		= JSVAL_TO_OBJECT( argv[0] );
	cBaseObject *myObj	= (cBaseObject *)JS_GetPrivate( cx, jsObj );

	cBaseObject *myChar	= static_cast<cBaseObject *>(JS_GetPrivate( cx, obj ) );

	if( !ValidateObject( myChar ) || !ValidateObject( myObj ) )
	{
		MethodError( "DistanceTo: Invalid character" );
		return JS_FALSE;
	}
	
	*rval = INT_TO_JSVAL( getDist( myChar, myObj ) );
	return JS_TRUE;
}

JSBool CItem_Glow( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	JSObject *mSock		= JSVAL_TO_OBJECT( argv[0] );
	cSocket *mySock	= (cSocket *)JS_GetPrivate( cx, mSock );

	CItem *mItem	= static_cast<CItem *>(JS_GetPrivate( cx, obj ) );

	if( !ValidateObject( mItem ) )
	{
		MethodError( "Glow: Invalid item" );
		mySock->sysmessage( 1095 ); 
		return JS_FALSE;
	}

	CChar *mChar = mySock->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return JS_FALSE;

	if( mItem->GetGlow() != INVALIDSERIAL ) 
	{
		mySock->sysmessage( 1097 );
		return JS_FALSE;
	}
	if( mItem->GetCont() == NULL && FindItemOwner( mItem ) != mChar )
	{
		mySock->sysmessage( 1096 );
		return JS_FALSE;
	}

	mItem->SetGlowColour( mItem->GetColour() );

	CItem *glowItem = Items->CreateItem( mySock, mChar, 0x1647, 1, 0, OT_ITEM ); // spawn light emitting object
	if( glowItem == NULL )
		return JS_FALSE;
	glowItem->SetName( "glower" );;
	glowItem->SetVisible( 0 );
	glowItem->SetMovable( 2 );

	mItem->SetGlow( glowItem->GetSerial() );
	Items->GlowItem( mItem );

	mChar->Update( mySock );
	mySock->sysmessage( 1098 );

	return JS_TRUE;
}

JSBool CItem_UnGlow( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	JSObject *mSock		= JSVAL_TO_OBJECT( argv[0] );
	cSocket *mySock	= (cSocket *)JS_GetPrivate( cx, mSock );

	CItem *mItem	= static_cast<CItem *>(JS_GetPrivate( cx, obj ) );

	if( !ValidateObject( mItem ) )
	{
		MethodError( "UnGlow: Invalid item" );
		mySock->sysmessage( 1099 ); 
		return JS_FALSE;
	}

	CChar *mChar = mySock->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return JS_FALSE;

	if( mItem->GetCont() == NULL && FindItemOwner( mItem ) != mChar )
	{
		mySock->sysmessage( 1100 );
		return JS_FALSE;
	}

	CItem *glowItem = calcItemObjFromSer( mItem->GetGlow() );
	if( mItem->GetGlow() == INVALIDSERIAL || !ValidateObject( glowItem ) ) 
	{
		mySock->sysmessage( 1101 );
		return JS_FALSE;
	}

	mItem->SetColour( mItem->GetGlowColour() );

	glowItem->Delete();
	mItem->SetGlow( INVALIDSERIAL );

	mChar->Update( mySock );
	mySock->sysmessage( 1102 );

	return JS_TRUE;
}

JSBool CChar_Gate( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Gate: Invalid number of arguments (takes 1, item)" );
		return JS_FALSE;
	}

	CChar *mChar	= static_cast<CChar *>(JS_GetPrivate( cx, obj ) );
	if( !ValidateObject( mChar ) )
	{
		MethodError( "Gate: Invalid source character" );
		return JS_FALSE;
	}

	JSObject *jsObj		= JSVAL_TO_OBJECT( argv[0] );
	CItem *mItem		= (CItem *)JS_GetPrivate( cx, jsObj );
	if( !ValidateObject( mItem ) )
	{
		MethodError( "Gate: Invalid item passed" );
		return JS_FALSE;
	}

	SI16 destX = mItem->GetTempVar( CITV_MOREX ), destY = mItem->GetTempVar( CITV_MOREY );
	SI08 destZ = mItem->GetTempVar( CITV_MOREZ );
	SpawnGate( mChar, mChar->GetX(), mChar->GetY(), mChar->GetZ(), destX, destY, destZ );

	return JS_TRUE;
}

JSBool CChar_Recall( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Recall: Invalid number of arguments (takes 1, item)" );
		return JS_FALSE;
	}

	CChar *mChar	= static_cast<CChar *>(JS_GetPrivate( cx, obj ) );
	if( !ValidateObject( mChar ) )
	{
		MethodError( "Recall: Invalid source character" );
		return JS_FALSE;
	}

	JSObject *jsObj		= JSVAL_TO_OBJECT( argv[0] );
	CItem *mItem		= (CItem *)JS_GetPrivate( cx, jsObj );
	if( !ValidateObject( mItem ) )
	{
		MethodError( "Recall: Invalid item passed" );
		return JS_FALSE;
	}

	SI16 destX = mItem->GetTempVar( CITV_MOREX ), destY = mItem->GetTempVar( CITV_MOREY );
	SI08 destZ = mItem->GetTempVar( CITV_MOREZ );
	mChar->SetLocation( destX, destY, destZ );

	return JS_TRUE;
}

JSBool CChar_Mark( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	if( argc != 1 )
	{
		MethodError( "Mark: Invalid number of arguments (takes 1, character)" );
		return JS_FALSE;
	}

	CChar *mChar	= static_cast<CChar *>(JS_GetPrivate( cx, obj ) );
	if( !ValidateObject( mChar ) )
	{
		MethodError( "Mark: Invalid source character" );
		return JS_FALSE;
	}

	JSObject *jsObj		= JSVAL_TO_OBJECT( argv[0] );
	CItem *mItem		= (CItem *)JS_GetPrivate( cx, jsObj );
	if( !ValidateObject( mItem ) )
	{
		MethodError( "Mark: Invalid item passed" );
		return JS_FALSE;
	}

	mItem->SetTempVar( CITV_MOREX, mChar->GetX() );
	mItem->SetTempVar( CITV_MOREY, mChar->GetY() );
	mItem->SetTempVar( CITV_MOREZ, mChar->GetZ() );

	char tempname[512];
	if( mChar->GetRegion()->GetName()[0] != 0 )
		sprintf( tempname, Dictionary->GetEntry( 684 ).c_str(), mChar->GetRegion()->GetName().c_str() );
	else 
		strcpy( tempname, Dictionary->GetEntry( 685 ).c_str() );
	mItem->SetName( tempname );

	return JS_TRUE;
}

}

