#include "r3dPCH.h"
#include "r3d.h"

#include "../SF/Console/Config.h"
#include "../SF/Console/EngineConsole.h"

//C
//
// Mouse
//
//

static	BOOL       		MouseAcquired	= 0;
static	IDirectInputDevice8 	*pdiMouse       = NULL;
static	DIMOUSESTATE2          	diMouseState;
static	DIDEVICEOBJECTDATA    	MouseBuffer[16];
static	int			MbdState[16];
static	int			MbdPressed[16];
static	int			MbdReleased[16];

static	int			MousePosX, MousePosY, MousePosZ;

r3dMouse::r3dMouse()
{
	MouseAcquired = FALSE;
	pdiMouse      = NULL;
	m_MouseMoveX = 0;
	m_MouseMoveY = 0;
	m_WheelScroll = 0;
}

r3dMouse::~r3dMouse()
{
	ReleaseCapture();
}

static int Mouse_Acquire()
{
	HRESULT     hres;
	if(pdiMouse == NULL)
		return TRUE;

	if((hres = pdiMouse->SetDataFormat(&c_dfDIMouse2)) != DI_OK)
		return FALSE;
	if((hres = pdiMouse->SetCooperativeLevel(r3dRenderer->HLibWin, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)) != DI_OK)
		return FALSE;

	DIPROPDWORD dipdw;

	dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj        = 0;
	dipdw.diph.dwHow        = DIPH_DEVICE;
	dipdw.dwData            = DIPROPAXISMODE_ABS;
	if((hres = pdiMouse->SetProperty(DIPROP_AXISMODE, &dipdw.diph)) != DI_OK)
		return FALSE;

	// try to acquire the mouse
	if((hres = pdiMouse->Acquire()) != DI_OK)
		return FALSE;

	MouseAcquired = TRUE;
	return TRUE;
}

int r3dMouse::SetCapture()
{
	if(pdiMouse == NULL) {
		if(r3dRenderer->pdi->CreateDevice(GUID_SysMouse, &pdiMouse, NULL) != DI_OK)
		{
			m_MouseMoveX = 0;
			m_MouseMoveY = 0;
			return FALSE;
		}
	}

	if(!MouseAcquired) 
		Mouse_Acquire();

	ClearPressed();
	int z;
	QueryMotionDistance(&m_MouseMoveX, &m_MouseMoveY);
	QueryWheelMotion(&z);

	SetRange(r3dRenderer->HLibWin);

	return TRUE;
}

void r3dMouse::ReleaseCapture()
{
	ClipCursor(NULL);

	ClearPressed();
	if(MouseAcquired) {
		if(pdiMouse)
			pdiMouse->Unacquire();
		MouseAcquired = FALSE;
	}

	if(pdiMouse != NULL) {
		pdiMouse->Release();
		pdiMouse = NULL;
	}

	Show(true);
}

bool r3dMouse_visible = true;	// mouse is visible by default

bool r3dMouse::GetMouseVisibility()
{
	return r3dMouse_visible;
}

void r3dMouse::Show(bool force)
{
	if(!force && r3dMouse_visible)
		return;

	while(::ShowCursor(TRUE) < 0);

	r3dMouse_visible = true;
	return;
}

void r3dMouse::Hide(bool force)
{
	if(!force && !r3dMouse_visible)
		return;

	while(::ShowCursor(FALSE) >= -1);

	r3dMouse_visible = false;
	return;
}

void r3dMouse::SetRange(int x1, int y1, int x2, int y2)
{
	RECT	rc;

	// Clip Cursor
	rc.left   = x1;
	rc.top    = y1;
	rc.right  = x2;
	rc.bottom = y2;
	ClipCursor(&rc);
	SetCursorPos(0, 0);
}


void r3dMouse::SetRange(HWND HWnd)
{
	if ( ! d_mouse_window_lock->GetBool() )
		return;

	RECT rc;

	/*
	GetWindowRect(HWnd, &rc);
	ClipCursor(&rc);
	SetCursorPos(0, 0);
	*/  

	// Clip Cursor
	GetClientRect(HWnd, &rc);
	POINT pp1;
	POINT pp2;
	pp1.x = rc.left;
	pp1.y = rc.top;
	pp2.x = rc.right;
	pp2.y = rc.bottom;
	ClientToScreen(r3dRenderer->HLibWin, &pp1);
	ClientToScreen(r3dRenderer->HLibWin, &pp2);
	rc.left   = pp1.x;
	rc.top    = pp1.y;
	rc.right  = pp2.x;
	rc.bottom = pp2.y;
	ClipCursor(&rc);
	//SetCursorPos(0, 0);

	return;
}


void r3dMouse::GetXY(int& X, int& Y)
{
	if(!MouseAcquired) {
		X = -100;
		Y = -100;
		return;
	}

	POINT pp;
	GetCursorPos(&pp);
	ScreenToClient(r3dRenderer->HLibWin, &pp);
	X = pp.x;
	Y = pp.y;

	return;
}

void r3dMouse::MoveTo(int X, int Y)
{
	SetCursorPos(X, Y);

	// reset motion distance
	int iHoriz, iVert;
	QueryMotionDistance(&iHoriz, &iVert);
}

WORD r3dMouse::GetRawMouseData()
{
	BYTE BP = 0;

	if(!MouseAcquired)
		return 0;

	switch(pdiMouse->GetDeviceState(sizeof(diMouseState), &diMouseState))
	{
	case DIERR_NOTACQUIRED:
	case DIERR_INPUTLOST:
		if(!Mouse_Acquire())
			break;
		// else fall thru
	case DI_OK:
		if(diMouseState.rgbButtons[0] & 0x80) BP |= 0x01;
		if(diMouseState.rgbButtons[1] & 0x80) BP |= 0x02;
		if(diMouseState.rgbButtons[2] & 0x80) BP |= 0x08;
		break;
	}

	return BP;
}

static int mouseSmoothingX[32] = {0};
static int mouseSmoothingY[32] = {0};
static int mouseCounter = 0;
int g_mouseSmoothingNumFrames = 3;
float g_mouseSmoothingWeight = 0.0f; // 0 - no smoothing, 1.0f-averaged, in between - smoothing

int getSmoothedMouseX()
{
	if(g_mouseSmoothingNumFrames == 0)
		return mouseSmoothingX[mouseCounter];

	int num = 0;
	float weight = 1.0f;
	for(int i=0; i<g_mouseSmoothingNumFrames; ++i)
	{
		num += int(mouseSmoothingX[(i+mouseCounter)%32]*weight);
		weight *= g_mouseSmoothingWeight;
	}

	return num;
}
int getSmoothedMouseY()
{
	if(g_mouseSmoothingNumFrames == 0)
		return mouseSmoothingY[mouseCounter];

	int num = 0;
	float weight = 1.0f;
	for(int i=0; i<g_mouseSmoothingNumFrames; ++i)
	{
		num += int(mouseSmoothingY[(i+mouseCounter)%32]*weight);
		weight *= g_mouseSmoothingWeight;
	}

	return num;
}

void r3dMouse::QueryMotionDistance(int *iHoriz, int *iVert)
{
	*iHoriz = 0;
	*iVert  = 0;


	if(!MouseAcquired)
		return;

	switch(pdiMouse->GetDeviceState(sizeof(diMouseState), &diMouseState))
	{
	case DIERR_NOTACQUIRED:
	case DIERR_INPUTLOST:
		if(!Mouse_Acquire())
			break;
		// else fall thru
	case DI_OK:
		mouseSmoothingX[mouseCounter] = diMouseState.lX - MousePosX;
		mouseSmoothingY[mouseCounter] = diMouseState.lY - MousePosY;;
		MousePosX = diMouseState.lX;
		MousePosY = diMouseState.lY;
		*iHoriz = getSmoothedMouseX();
		*iVert    = getSmoothedMouseY();
		--mouseCounter;
		if(mouseCounter < 0)
			mouseCounter = 31;
		break;
	}

	return;
}

void r3dMouse::QueryWheelMotion(int *iWheel)
{
	*iWheel = 0;

	if(!MouseAcquired)
		return;

	switch(pdiMouse->GetDeviceState(sizeof(diMouseState), &diMouseState))
	{
	case DIERR_NOTACQUIRED:
	case DIERR_INPUTLOST:
		if(!Mouse_Acquire())
			break;
		// else fall thru
	case DI_OK:
		*iWheel   = diMouseState.lZ - MousePosZ;
		MousePosZ = diMouseState.lZ;
		break;
	}

	return;
}


bool r3dMouse::IsPressed(int key)
{
	if(!MouseAcquired)
		return false; 

	if(MbdState[key])
		return true;
	
	return false;
}

bool r3dMouse::WasPressed(int key)
{
	if(!MouseAcquired)
		return false;

	if(MbdPressed[key]) 
		return true;

	return false;
}

bool r3dMouse::WasReleased(int key)
{
	if(!MouseAcquired)
		return false;

	if(MbdReleased[key]) 
		return true;

	return false;
}

void r3dMouse::ClearPressed()
{
	memset(MbdReleased, 0, sizeof(MbdReleased));
	memset(MbdPressed,  0, sizeof(MbdPressed));
	memset(MbdState,    0, sizeof(MbdState));
}


//C
//
// r3dKeyboard
//
//

static	IDirectInputDevice8 	*pdiKbd;
static	int			KbdAcquired;
static	int			KbdState[256];
static	int			KbdPressed[256];
static	int			KbdReleased[256];
static	DWORD			KbdPressedID       = -1;
static	DWORD			KbdPressedNextTime = 0;		// next pressed time

#define	KBD_BUFFER_SIZE		500 //BP was 100 raised to fix bug??


//--------------------------------------------------------------------------------------------------------
static bool Acquire()
{
	if ( ! pdiKbd )
		return true;

	HRESULT hr = pdiKbd->Acquire();
	KbdAcquired = SUCCEEDED( hr );
	return KbdAcquired != 0;
}

//--------------------------------------------------------------------------------------------------------
static bool Unacquire()
{
	if ( ! pdiKbd )
		return true;

	HRESULT hr = pdiKbd->Unacquire();
	if ( SUCCEEDED( hr ) )
		KbdAcquired = false;

	return ! FAILED( hr );
}


r3dKeyboard::r3dKeyboard()
{
	KbdAcquired = 0;
}

r3dKeyboard::~r3dKeyboard()
{
	ReleaseCapture();
}

int r3dKeyboard::SetCapture()
{
	DIPROPDWORD  dipdw;
	HRESULT      hres;

	if(KbdAcquired)
		return 1;

	if((hres=r3dRenderer->pdi->CreateDevice(GUID_SysKeyboard, &pdiKbd, NULL)) != DI_OK)
		return 0;
	if((hres=pdiKbd->SetDataFormat(&c_dfDIKeyboard)) != DI_OK)
		return 0;
	DWORD flag = DISCL_NONEXCLUSIVE | DISCL_FOREGROUND;

	if((hres=pdiKbd->SetCooperativeLevel(r3dRenderer->HLibWin, flag)) != DI_OK)
		return 0;

	dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj        = 0;
	dipdw.diph.dwHow        = DIPH_DEVICE;
	dipdw.dwData            = KBD_BUFFER_SIZE;
	if((hres = pdiKbd->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)) != DI_OK)
		return 0;

	Acquire();
	Reset();

	return KbdAcquired;
}

void r3dKeyboard::ReleaseCapture()
{
	if(!KbdAcquired)
		return;

	if(pdiKbd != NULL) {
		Unacquire();
		pdiKbd->Release();
	}

	KbdAcquired = 0;
	return;
}


int Keyboard_Poll()
{
	DIDEVICEOBJECTDATA	d[KBD_BUFFER_SIZE];
	DWORD 	  	dwItems = KBD_BUFFER_SIZE - 1;
	HRESULT		hres;
	unsigned int	i, id;

	if(win::bSuspended)
		return 0;

	if(!KbdAcquired)
		return 0;

	hres = pdiKbd->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), d, &dwItems, 0 );
	if( FAILED(hres) )
	{
		Acquire();
		return 0;
	}

	memset(KbdReleased, 0, sizeof(KbdReleased));
	memset(KbdPressed,  0, sizeof(KbdPressed));
	for(i=0; i < dwItems; i++)
	{
		id = d[i].dwOfs;
		if(d[i].dwData & 0x80) 
		{
			if(KbdState[id]==0)
				KbdPressed[id]     = 1;
			KbdState[id]       = 1;
		}
		else 
		{
			KbdState[id]        = 0;
			KbdReleased[id]     = 1;
		}
	}

	return 1;     
}

int Mouse_Poll()
{

	Mouse->m_MouseMoveX = 0;
	Mouse->m_MouseMoveY = 0;

	if(win::bSuspended)
		return 0;

	if(!MouseAcquired)
		return 0;

	HRESULT hr = pdiMouse->GetDeviceState(sizeof(diMouseState), &diMouseState);
	if(FAILED(hr))
	{
		Mouse_Acquire();
		return 0;
	}

	memset(MbdReleased, 0, sizeof(MbdReleased));
	memset(MbdPressed,  0, sizeof(MbdPressed));
	
	for(int i=0; i < 8; ++i)
	{
		if(diMouseState.rgbButtons[i] & 0x80) 
		{
			// if key was pressed
			if(MbdState[i]==0)
				MbdPressed[i]     = 1;
			MbdState[i]       = 1;
		}
		else 
		{
			// released
			if(MbdState[i]==1)
				MbdReleased[i]     = 1;
			MbdState[i]        = 0;
		}
	}

	Mouse->QueryMotionDistance(&Mouse->m_MouseMoveX, &Mouse->m_MouseMoveY);
	Mouse->QueryWheelMotion(&Mouse->m_WheelScroll);

	return 1;     
}
void InputUpdate()
{
	R3DPROFILE_FUNCTION("InputUpdate");
	if ( ! KbdAcquired )
		Acquire();
	Keyboard_Poll();

	if(!MouseAcquired)
		Mouse_Acquire();
	Mouse_Poll();

	if(Gamepad)
		Gamepad->Update();

}


bool r3dKeyboard::IsPressed( int key, bool noConsoleLock /*= false*/ )
{
	if(!KbdAcquired || win::bSuspended)
		return false; 

	// when console is open, do not process keyboard
	if(g_pDefaultConsole && g_pDefaultConsole->IsVisible() && !noConsoleLock )
		return false;

	if(KbdState[key])
		return true;

	return false;
}

bool r3dKeyboard::WasPressed(int key)
{
	if(!KbdAcquired || win::bSuspended)
		return false;

	// when console is open, do not process keyboard
	if(g_pDefaultConsole && g_pDefaultConsole->IsVisible())
		return false;

	if(KbdPressed[key]) 
		return true;

	return false;
}

bool r3dKeyboard::WasReleased(int key)
{
	if(!KbdAcquired || win::bSuspended)
		return false;

	// when console is open, do not process keyboard
	if(g_pDefaultConsole && g_pDefaultConsole->IsVisible())
		return false;

	if(KbdReleased[key]) 
		return true;

	return false;
}

int r3dKeyboard::ClearPressed()
{
	memset(KbdReleased, 0, sizeof(KbdReleased));
	memset(KbdPressed,  0, sizeof(KbdPressed));
	memset(KbdState,    0, sizeof(KbdState));

	return 1;
}

int r3dKeyboard::Reset()
{
	DWORD	dwItems = INFINITE;

	ClearPressed();

	if(!KbdAcquired)
		return 0;

	pdiKbd->GetDeviceData(
		sizeof(DIDEVICEOBJECTDATA),
		NULL,
		&dwItems,
		0);

	return 1;
}


struct 
{
	int		KeyCode;
	char		*KeyName;
} \
_KeyNames[] =
{
	{kbsEsc, 	"ESC" },
	{kbs1,  	"1" },
	{kbs2,  	"2" },
	{kbs3,  	"3" },
	{kbs4,  	"4" },
	{kbs5,  	"5" },
	{kbs6,  	"6" },
	{kbs7,  	"7" },
	{kbs8,  	"8" },
	{kbs9,  	"9" },
	{kbs0,  	"0" },
	{kbsMinus,	"MINUS" },
	{kbsEquals             , "EQUALS" },
	{kbsBACK               , "BACK" },
	{kbsTab                , "TAB" },
	{kbsQ                  , "Q" },
	{kbsW                  , "W" },
	{kbsE                  , "E" },
	{kbsR                  , "R" },
	{kbsT                  , "T" },
	{kbsY                  , "Y" },
	{kbsU                  , "U" },
	{kbsI                  , "I" },
	{kbsO                  , "O" },
	{kbsP                  , "P" },
	{kbsLeftBracket        , "LBRACKET" },
	{kbsRightBracket       , "RBRACKET" },
	{kbsEnter           	, "RETURN" },
	{kbsLeftControl        , "Left Ctrl" },
	{kbsA                  , "A" },
	{kbsS                  , "S" },
	{kbsD                  , "D" },
	{kbsF                  , "F" },
	{kbsG                  , "G" },
	{kbsH                  , "H" },
	{kbsJ                  , "J" },
	{kbsK                  , "K" },
	{kbsL                  , "L" },
	{kbsSemiColon          , "SEMICOLON" },
	{kbsApostrophe         , "APOSTROPHE" },
	{kbsGrave		, "GRAVE" },
	{kbsLeftShift          , "LEFT SHIFT" },
	{kbsBackSlash          , "BACKSLASH" },
	{kbsZ                  , "Z" },
	{kbsX                  , "X" },
	{kbsC                  , "C" },
	{kbsV                  , "V" },
	{kbsB                  , "B" },
	{kbsN                  , "N" },
	{kbsM                  , "M" },
	{kbsComma		, "COMMA" },
	{kbsPeriod		, "PERIOD" },
	{kbsSlash		, "SLASH" },
	{kbsRightShift         , "RIGHT SHIFT" },
	{kbsPrtScr             , "SYSRQ" },
	{kbsLeftAlt            , "LEFT ALT" },
	{kbsSpace              , "SPACE" },
	{kbsCapsLock           , "CAPITAL" },
	{kbsF1                 , "F1" },
	{kbsF2                 , "F2" },
	{kbsF3                 , "F3" },
	{kbsF4                 , "F4" },
	{kbsF5                 , "F5" },
	{kbsF6                 , "F6" },
	{kbsF7                 , "F7" },
	{kbsF8                 , "F8" },
	{kbsF9                 , "F9" },
	{kbsF10                , "F10" },
	{kbsF11                , "F11" },
	{kbsF12                , "F12" },
	{kbsNumLock            , "NUMLOCK" },
	{kbsScrollLock         , "SCROLL" },
	{kbsNumericHome        , "NUMPAD7" },
	{kbsNumericUp          , "NUMPAD8" },
	{kbsNumericPgUp        , "NUMPAD9" },
	{kbsGrayMinus          , "SUBTRACT" },
	{kbsNumericLeft        , "NUMPAD4" },
	{kbsNumericFive        , "NUMPAD5" },
	{kbsNumericRight       , "NUMPAD6" },
	{kbsGrayPlus           , "ADD" },
	{kbsNumericEnd         , "NUMPAD1" },
	{kbsNumericDown        , "NUMPAD2" },
	{kbsNumericPgDn        , "NUMPAD3" },
	{kbsNumericIns         , "NUMPAD0" },
	{kbsNumericDel         , "DECIMAL" },
	{kbsNumericEnter	, "NUMPADENTER" },  
	{kbsRightAlt           , "RMENU" },
	{kbsRightControl       , "RCONTROL" },
	{kbsGraySlash          , "DIVIDE" },
	{kbsGrayHome           , "HOME" },
	{kbsUp			, "UP" },
	{kbsDown		, "DOWN" },
	{kbsLeft		, "LEFT" },
	{kbsRight		, "RIGHT" },
	{kbsHome		, "HOME" },
	{kbsEnd		, "END" },
	{kbsPgUp		, "PRIOR" },
	{kbsPgDn		, "NEXT" },
	{kbsIns		, "INSERT" },
	{kbsDel		, "DELETE" },
};
static	int	_Num_KeyNames = sizeof(_KeyNames) / sizeof(*_KeyNames);



char *r3dGetKeyName(int KeyCode)
{
	for (int i=0;i<_Num_KeyNames;i++)
		if ( _KeyNames[i].KeyCode == KeyCode ) return _KeyNames[i].KeyName;

	return "UNDEFINED KEY"; 
}


void r3dGamepad::Reset()
{
	isConnected = false;
	ZeroMemory( &State, sizeof(XINPUT_STATE) );
	ZeroMemory( &PrevState, sizeof(XINPUT_STATE) );

	leftTrigger = 0.0f;
	rightTrigger = 0.0f;
	leftThumbX = 0.0f;
	leftThumbY = 0.0f;
	rightThumbX = 0.0f;
	rightThumbY = 0.0f;
}

void r3dGamepad::Update()
{
	R3DPROFILE_FUNCTION("r3dGamepad::Update");
	GetState();
}

void r3dGamepad::GetState()
{
	isConnected = false;
	memcpy(&PrevState, &State, sizeof(XINPUT_STATE));

	leftTrigger = 0.0f;
	rightTrigger = 0.0f;
	leftThumbX = 0.0f;
	leftThumbY = 0.0f;
	rightThumbX = 0.0f;
	rightThumbY = 0.0f;

	if( XInputGetState( 0, &State ) == ERROR_SUCCESS )
	{
		isConnected = true;
		
		// left thumb
		{
			float LX = State.Gamepad.sThumbLX;
			float LY = State.Gamepad.sThumbLY;

			//determine how far the controller is pushed
			float magnitude = sqrt(LX*LX + LY*LY);
			if(magnitude == 0.0f) magnitude = 1.0f;

			//determine the direction the controller is pushed
			float normalizedLX = LX / magnitude;
			float normalizedLY = LY / magnitude;

			float normalizedMagnitude = 0;

			//check if the controller is outside a circular dead zone
			if (magnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			{
				//clip the magnitude at its expected maximum value
				if (magnitude > 32767) magnitude = 32767;

				//adjust magnitude relative to the end of the dead zone
				magnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

				//optionally normalize the magnitude with respect to its expected range
				//giving a magnitude value of 0.0 to 1.0
				normalizedMagnitude = magnitude / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
			}
			else //if the controller is in the deadzone zero out the magnitude
			{
				magnitude = 0.0;
				normalizedMagnitude = 0.0;
			}

			leftThumbX = normalizedLX * normalizedMagnitude;
			leftThumbY = normalizedLY * normalizedMagnitude;
		}
		// right thumb
		{
			float LX = State.Gamepad.sThumbRX;
			float LY = State.Gamepad.sThumbRY;

			//determine how far the controller is pushed
			float magnitude = sqrt(LX*LX + LY*LY);
			if(magnitude == 0.0f) magnitude = 1.0f;

			//determine the direction the controller is pushed
			float normalizedLX = LX / magnitude;
			float normalizedLY = LY / magnitude;

			float normalizedMagnitude = 0;

			//check if the controller is outside a circular dead zone
			if (magnitude > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE )
			{
				//clip the magnitude at its expected maximum value
				if (magnitude > 32767) magnitude = 32767;

				//adjust magnitude relative to the end of the dead zone
				magnitude -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ;

				//optionally normalize the magnitude with respect to its expected range
				//giving a magnitude value of 0.0 to 1.0
				normalizedMagnitude = magnitude / (32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE );
			}
			else //if the controller is in the deadzone zero out the magnitude
			{
				magnitude = 0.0;
				normalizedMagnitude = 0.0;
			}

			rightThumbX = normalizedLX * normalizedMagnitude;
			rightThumbY = normalizedLY * normalizedMagnitude;
		}
		// trigger
		{
			float LX = State.Gamepad.bLeftTrigger;
			float LY = State.Gamepad.bRightTrigger;

			//determine how far the controller is pushed
			float magnitude = sqrt(LX*LX + LY*LY);
			if(magnitude == 0.0f) magnitude = 1.0f;

			//determine the direction the controller is pushed
			float normalizedLX = LX / magnitude;
			float normalizedLY = LY / magnitude;

			float normalizedMagnitude = 0;

			//check if the controller is outside a circular dead zone
			if (magnitude > XINPUT_GAMEPAD_TRIGGER_THRESHOLD )
			{
				//clip the magnitude at its expected maximum value
				if (magnitude > 255) magnitude = 255;

				//adjust magnitude relative to the end of the dead zone
				magnitude -= XINPUT_GAMEPAD_TRIGGER_THRESHOLD ;

				//optionally normalize the magnitude with respect to its expected range
				//giving a magnitude value of 0.0 to 1.0
				normalizedMagnitude = magnitude / (255 - XINPUT_GAMEPAD_TRIGGER_THRESHOLD );
			}
			else //if the controller is in the deadzone zero out the magnitude
			{
				magnitude = 0.0;
				normalizedMagnitude = 0.0;
			}

			leftTrigger = normalizedLX * normalizedMagnitude;
			rightTrigger = normalizedLY * normalizedMagnitude;
		}
	}
}

void r3dGamepad::GetTrigger(float &Left, float &Right)
{
	Left = leftTrigger;
	Right = rightTrigger;
}


void r3dGamepad::GetLeftThumb(float &X, float &Y)
{
	X = leftThumbX;
	Y = leftThumbY;
}


void r3dGamepad::GetRightThumb(float &X, float &Y)
{
	X = rightThumbX;
	Y = rightThumbY;
}

bool r3dGamepad::IsPressed(int key)
{
	if(!isConnected)
		return false;
	return (State.Gamepad.wButtons & key)?true:false;
}

bool r3dGamepad::WasPressed(int key)
{
	if(!isConnected)
		return false;
	return !(PrevState.Gamepad.wButtons & key) && (State.Gamepad.wButtons & key);
}

bool r3dGamepad::WasReleased(int key)
{
	if(!isConnected)
		return false;
	return (PrevState.Gamepad.wButtons & key) && !(State.Gamepad.wButtons & key);
}


r3dInputMappingMngr::r3dInputMappingMngr()
{
	// init to default mapping here
	m_Mapping[KS_PRIMARY_FIRE] =			KeyboardMapping(INPUTMAP_MOUSE, r3dMouse::mLeftButton, "$HUD_Cntrl_PrimaryFire", false);
	m_Mapping[KS_AIM] =						KeyboardMapping(INPUTMAP_MOUSE, r3dMouse::mRightButton, "$HUD_Cntrl_Aim", false);

	m_Mapping[KS_MOVE_FORWARD] =			KeyboardMapping(INPUTMAP_KEYBOARD, kbsW, "$HUD_Cntrl_MoveForw", true);
	m_Mapping[KS_MOVE_FORWARD_TOGGLE] =		KeyboardMapping(INPUTMAP_KEYBOARD, kbsNumLock, "$HUD_Cntrl_MoveForwToggle", false);
	m_Mapping[KS_MOVE_BACKWARD] =			KeyboardMapping(INPUTMAP_KEYBOARD, kbsS, "$HUD_Cntrl_MoveBack", true);
	m_Mapping[KS_MOVE_LEFT] =				KeyboardMapping(INPUTMAP_KEYBOARD, kbsA, "$HUD_Cntrl_MoveLeft", true);
	m_Mapping[KS_MOVE_RIGHT] =				KeyboardMapping(INPUTMAP_KEYBOARD, kbsD, "$HUD_Cntrl_MoveRight", true);
	m_Mapping[KS_SPRINT] =					KeyboardMapping(INPUTMAP_KEYBOARD, kbsLeftShift, "$HUD_Cntrl_Sprint", true);
	m_Mapping[KS_JUMP] =					KeyboardMapping(INPUTMAP_KEYBOARD, kbsSpace, "$HUD_Cntrl_Jump", false);
	m_Mapping[KS_CROUCH] =					KeyboardMapping(INPUTMAP_KEYBOARD, kbsLeftControl, "$HUD_Cntrl_Crouch", false);
	m_Mapping[KS_INTERACT] =				KeyboardMapping(INPUTMAP_KEYBOARD, kbsE, "$HUD_Cntrl_Interact", false);
	m_Mapping[KS_SECONDARY_INTERACT] =		KeyboardMapping(INPUTMAP_KEYBOARD, kbsY, "$HUD_Cntrol_SecondaryInteract", false);
	m_Mapping[KS_RELOAD] =					KeyboardMapping(INPUTMAP_KEYBOARD, kbsR, "$HUD_Cntrl_Reload", false);
	m_Mapping[KS_CHANGE_RATE_FIRE] =		KeyboardMapping(INPUTMAP_KEYBOARD, kbsX, "$HUD_Cntrl_ChangeFireRate", false);

	m_Mapping[KS_PRIMARY_WEAPON] =			KeyboardMapping(INPUTMAP_KEYBOARD, kbs1, "$HUD_Cntrl_PrimaryWeapon", false);
	m_Mapping[KS_HANDGUN_WEAPON] =			KeyboardMapping(INPUTMAP_KEYBOARD, kbs2, "$HUD_Cntrl_Handgun", false);
	m_Mapping[KS_ITEM1] =					KeyboardMapping(INPUTMAP_KEYBOARD, kbs3, "$HUD_Cntrl_Item1", false);
	m_Mapping[KS_ITEM2] =					KeyboardMapping(INPUTMAP_KEYBOARD, kbs4, "$HUD_Cntrl_Item2", false);
	m_Mapping[KS_ITEM3] =					KeyboardMapping(INPUTMAP_KEYBOARD, kbs5, "$HUD_Cntrl_Item3", false);
	m_Mapping[KS_ITEM4] =					KeyboardMapping(INPUTMAP_KEYBOARD, kbs6, "$HUD_Cntrl_Item4", false);
	m_Mapping[KS_NEXTITEM] =				KeyboardMapping(INPUTMAP_KEYBOARD, kbsGrave, "$HUD_Cntrl_NextItem", false);

	m_Mapping[KS_HOLD_BREATH] =				KeyboardMapping(INPUTMAP_KEYBOARD, kbsLeftShift, "$HUD_Cntrl_SteadyAim", true);

	m_Mapping[KS_CHAT] =					KeyboardMapping(INPUTMAP_KEYBOARD, kbsEnter, "$HUD_Cntrl_Chat", false);
	m_Mapping[KS_VOICE_PTT] =				KeyboardMapping(INPUTMAP_KEYBOARD, kbsT, "$HUD_Cntrl_VoicePTT", false);
	m_Mapping[KS_SWITCH_MINIMAP] =			KeyboardMapping(INPUTMAP_KEYBOARD, kbsM, "$HUD_Cntrl_SwitchMinimap", false);

	m_Mapping[KS_QUICK_SWITCH_WEAPON] =		KeyboardMapping(INPUTMAP_KEYBOARD, kbsQ, "$HUD_Cntrl_QuickSwitch", false);

	m_Mapping[KS_CONSTRUCTOR] =				KeyboardMapping(INPUTMAP_KEYBOARD, kbsH, "$HUD_Constructor_Mode", false);
	m_Mapping[KS_UPBLOCK] =				    KeyboardMapping(INPUTMAP_KEYBOARD, kbsNumericHome, "$HUD_UpBlock", false);
	m_Mapping[KS_DOWNBLOCK] =				KeyboardMapping(INPUTMAP_KEYBOARD, kbsNumericPgUp, "$HUD_DownBlock", false);
	m_Mapping[KS_RLEFTBLOCK] =				KeyboardMapping(INPUTMAP_KEYBOARD, kbsNumericEnd, "$HUD_Rotate_LeftBlock", false);
	m_Mapping[KS_RRIGHTBLOCK] =				KeyboardMapping(INPUTMAP_KEYBOARD, kbsNumericPgDn, "$HUD_Rotate_RightBlock", false);
	m_Mapping[KS_MLEFTBLOCK] =				KeyboardMapping(INPUTMAP_KEYBOARD, kbsNumericLeft, "$HUD_Move_LeftBlock", false);
	m_Mapping[KS_MRIGHTBLOCK] =				KeyboardMapping(INPUTMAP_KEYBOARD, kbsNumericRight, "$HUD_Move_RightBlock", false);
	m_Mapping[KS_FRONTBLOCK] =				KeyboardMapping(INPUTMAP_KEYBOARD, kbsNumericUp, "$HUD_Move_FrontBlock", false);
	m_Mapping[KS_BACKBLOCK] =				KeyboardMapping(INPUTMAP_KEYBOARD, kbsNumericDown, "$HUD_Move_BackBlock", false);
	m_Mapping[KS_PUTBLOCK] =				KeyboardMapping(INPUTMAP_KEYBOARD, kbsNumericFive, "$HUD_PutBlock", false);

	m_Mapping[KS_TOGGLE_NIGHTVISION] =		KeyboardMapping(INPUTMAP_KEYBOARD, kbsN, "$HUD_Cntrl_ToggleNightvision", false);
	m_Mapping[KS_TOGGLE_FLASHLIGHT] =		KeyboardMapping(INPUTMAP_KEYBOARD, kbsF, "$HUD_Cntrl_ToggleFlashlight", false);

	m_Mapping[KS_SWITCH_FPS_TPS] =			KeyboardMapping(INPUTMAP_KEYBOARD, kbsC, "$HUD_Switch_FPS_TPS", false);
	m_Mapping[KS_SWITCH_TPS_CAMERA_SIDE] =	KeyboardMapping(INPUTMAP_KEYBOARD, kbsLeftAlt, "$HUD_Switch_TPS_Camera_Side", false);

	m_Mapping[KS_INVENTORY] =				KeyboardMapping(INPUTMAP_KEYBOARD, kbsI, "$HUD_Cntrl_Inventory", false);

	m_Mapping[KS_CHAT_CHANNEL1] =			KeyboardMapping(INPUTMAP_KEYBOARD, kbsF1, "$HUD_Cntrl_Chat1", false);
	m_Mapping[KS_CHAT_CHANNEL2] =			KeyboardMapping(INPUTMAP_KEYBOARD, kbsF2, "$HUD_Cntrl_Chat2", false);
	m_Mapping[KS_CHAT_CHANNEL3] =			KeyboardMapping(INPUTMAP_KEYBOARD, kbsF3, "$HUD_Cntrl_Chat3", false);
	m_Mapping[KS_CHAT_CHANNEL4] =			KeyboardMapping(INPUTMAP_KEYBOARD, kbsF4, "$HUD_Cntrl_Chat4", false);

	m_Mapping[KS_SHOW_ATTACHMENTS] =		KeyboardMapping(INPUTMAP_KEYBOARD, kbsF6, "$HUD_Cntrl_ShowAttachments", false);
	m_Mapping[KS_SHOW_CRAFT_MENU] =			KeyboardMapping(INPUTMAP_KEYBOARD, kbsK, "$HUD_Cntrl_ShowCraftMenu", false);
#ifdef ENABLE_INGAME_STORE
	m_Mapping[KS_SHOW_STORE] =				KeyboardMapping(INPUTMAP_KEYBOARD, kbsF10, "$HUD_Cntrl_ShowStore", false);
#endif

#ifdef MISSIONS
	m_Mapping[KS_TOGGLE_MISSION_HUD] =		KeyboardMapping(INPUTMAP_KEYBOARD, kbsL, "$HUD_Toggle_Mission_HUD", false);
#endif

	m_Mapping[KS_FREE_HANDS] =				KeyboardMapping(INPUTMAP_KEYBOARD, kbsBACK, "$HUD_Cntrl_FreeHands", false);

	m_Mapping[KS_PRONE] =					KeyboardMapping(INPUTMAP_KEYBOARD, kbsZ, "$HUD_Cntrl_Prone", false);

	m_Mapping[KS_SHOW_PLAYERS] =			KeyboardMapping(INPUTMAP_KEYBOARD, kbsTab, "$HUD_Cntrl_ShowPlayers", false);

	m_Mapping[KS_TURN_OFF_HUD] =			KeyboardMapping(INPUTMAP_KEYBOARD, kbsF12, "$HUD_Cntrl_TurnOffHUD", false);
	
#ifdef VEHICLES_ENABLED
	m_Mapping[KS_VEHICLE_UNSTUCK] =			KeyboardMapping(INPUTMAP_KEYBOARD, kbsF5, "$HUD_Cntrl_VehicleUnstuck", false);
	m_Mapping[KS_VEHICLE_ACCEL] =			KeyboardMapping(INPUTMAP_KEYBOARD, kbsUp, "$HUD_Cntrl_VehicleAccel", true);
	m_Mapping[KS_VEHICLE_BRAKE] =			KeyboardMapping(INPUTMAP_KEYBOARD, kbsDown, "$HUD_Cntrl_VehicleBrake", true);
	m_Mapping[KS_VEHICLE_TURNLEFT] =		KeyboardMapping(INPUTMAP_KEYBOARD, kbsLeft, "$HUD_Cntrl_VehicleTurnLeft", true);
	m_Mapping[KS_VEHICLE_TURNRIGHT] =		KeyboardMapping(INPUTMAP_KEYBOARD, kbsRight, "$HUD_Cntrl_VehicleTurnRight", true);
#endif

	m_Mapping[KS_UAV_TARGET] =		KeyboardMapping(INPUTMAP_MOUSE, r3dMouse::mLeftButton, "$HUD_Cntrl_UAVTarget", true);
	m_Mapping[KS_UAV_UP] =			KeyboardMapping(INPUTMAP_KEYBOARD, kbsQ, "$HUD_Cntrl_UAVUp", true);
	m_Mapping[KS_UAV_DOWN] =		KeyboardMapping(INPUTMAP_KEYBOARD, kbsZ, "$HUD_Cntrl_UAVDown", true);
}

r3dInputMappingMngr::~r3dInputMappingMngr()
{
}

bool r3dInputMappingMngr::isPressed(KeybordShortcuts shortcut)
{
	r3d_assert(m_Mapping[shortcut].type!=INPUTMAP_INVALID);

	if(m_Mapping[shortcut].type==INPUTMAP_KEYBOARD) 
		return Keyboard->IsPressed(m_Mapping[shortcut].key);
	else if(m_Mapping[shortcut].type==INPUTMAP_MOUSE)
		return Mouse->IsPressed(m_Mapping[shortcut].key);

	r3dError("Unsupported input type: %d\n", m_Mapping[shortcut].type);
	return false;
}

bool r3dInputMappingMngr::wasPressed(KeybordShortcuts shortcut)
{
	r3d_assert(m_Mapping[shortcut].type!=INPUTMAP_INVALID);

	if(m_Mapping[shortcut].type==INPUTMAP_KEYBOARD) 
		return Keyboard->WasPressed(m_Mapping[shortcut].key);
	else if(m_Mapping[shortcut].type==INPUTMAP_MOUSE) 
		return Mouse->WasPressed(m_Mapping[shortcut].key);

	r3dError("Unsupported input type: %d\n", m_Mapping[shortcut].type);
	return false;
}

bool r3dInputMappingMngr::wasReleased(KeybordShortcuts shortcut)
{
	r3d_assert(m_Mapping[shortcut].type!=INPUTMAP_INVALID);

	if(m_Mapping[shortcut].type==INPUTMAP_KEYBOARD) 
		return Keyboard->WasReleased(m_Mapping[shortcut].key);
	else if(m_Mapping[shortcut].type==INPUTMAP_MOUSE) 
		return Mouse->WasReleased(m_Mapping[shortcut].key);

	r3dError("Unsupported input type: %d\n", m_Mapping[shortcut].type);
	return false;
}

static const int INPUT_MAPPING_FILE_VERSION = 11;

void r3dInputMappingMngr::saveMapping(const char* path)
{
	r3d_assert(path);

	// save into xml
	pugi::xml_document xmlMappingFile;
	xmlMappingFile.append_child(pugi::node_comment).set_value("Input mapping");
	pugi::xml_node xmlMappings = xmlMappingFile.append_child();
	xmlMappings.set_name("mappings");
	xmlMappings.append_attribute("version") = INPUT_MAPPING_FILE_VERSION;
	xmlMappings.append_attribute("num_maps") = KS_NUM;
	
	for(int i =0; i<KS_NUM; ++i)
	{
		pugi::xml_node xmlMap = xmlMappings.append_child();
		xmlMap.set_name("map");
		xmlMap.append_attribute("type") = int(m_Mapping[i].type);
		xmlMap.append_attribute("key") = int(m_Mapping[i].key);
	}

	xmlMappingFile.save_file(path);
}

void r3dInputMappingMngr::loadMapping(const char* path)
{
	r3d_assert(path);

	r3dFile* f = r3d_open(path, "rb");
	if ( !f )
	{
		r3dOutToLog("Failed to open key mapping file: %s\n", path);
		return;
	}

	char* fileBuffer = gfx_new char[f->size + 1];
	fread(fileBuffer, f->size, 1, f);
	fileBuffer[f->size] = 0;

	pugi::xml_document xmlMappingFile;
	pugi::xml_parse_result parseResult = xmlMappingFile.load_buffer_inplace(fileBuffer, f->size);
	fclose(f);
	if(!parseResult)
		r3dError("Failed to parse XML, error: %s", parseResult.description());
	
	pugi::xml_node xmlMappings = xmlMappingFile.child("mappings");

	// check for version
	if(xmlMappings.attribute("version").as_int() != INPUT_MAPPING_FILE_VERSION)
	{
		r3dOutToLog("Incompatible key mapping file, switching to default key map\n");
		delete [] fileBuffer;
		return;
	}

	int numMaps = xmlMappings.attribute("num_maps").as_int();
	if(numMaps != KS_NUM)
	{
		r3dOutToLog("Incompatible key mapping file, numMaps=%d versus required %d, switching to default key map\n", numMaps, KS_NUM);
		delete [] fileBuffer;
		return;
	}

	// validate data is correct in xml file
	{
		bool data_valid = true;
		pugi::xml_node xmlMap = xmlMappings.child("map");
		for(int i=0; i<numMaps; ++i)
		{
			int type = (InputMapType)xmlMap.attribute("type").as_int();
			if(type != INPUTMAP_KEYBOARD && type != INPUTMAP_MOUSE)
			{
				data_valid = false;
				break;
			}
			xmlMap = xmlMap.next_sibling();
		}
		if(!data_valid)
		{
			r3dOutToLog("Corrupted key mapping file, switching to default key map\n");
			delete [] fileBuffer;
			return;
		}
	}

	pugi::xml_node xmlMap = xmlMappings.child("map");
	for(int i=0; i<numMaps; ++i)
	{
		m_Mapping[i].type = (InputMapType)xmlMap.attribute("type").as_int();
		m_Mapping[i].key = xmlMap.attribute("key").as_int();
		xmlMap = xmlMap.next_sibling();
	}

	// delete only after we are done parsing xml!
	delete [] fileBuffer;
}

const char* r3dInputMappingMngr::getMapName(KeybordShortcuts shortcut)
{
	return m_Mapping[shortcut].name;
}

const char* r3dInputMappingMngr::getKeyName(KeybordShortcuts shortcut)
{
	if(m_Mapping[shortcut].type == INPUTMAP_MOUSE)
	{
		switch (m_Mapping[shortcut].key)
		{
		case r3dMouse::mLeftButton:
			return "LMB";
		case r3dMouse::mRightButton:
			return "RMB";
		case r3dMouse::mCenterButton:
			return "CMB";
		case r3dMouse::mWheelButton:
			return "MMB";
		case 4:
			return "MB4";
		case 5:
			return "MB5";
		case 6:
			return "MB6";
		case 7:
			return "MB7";
		default:
			return "MOUSE BUTTON";
		}
	}
	else if(m_Mapping[shortcut].type == INPUTMAP_KEYBOARD)
	{
		return r3dGetKeyName(m_Mapping[shortcut].key);
	}

	return "unknown key";
}

bool r3dInputMappingMngr::attemptRemapKey(KeybordShortcuts shortcut, bool& conflict)
{
	r3d_assert(int(shortcut)>=0 && int(shortcut)<r3dInputMappingMngr::KS_NUM);
	conflict = false;
	// check mouse firstly
	for(int i=0; i<8; ++i)
	{
		if(MbdReleased[i])
		{
			// check for conflicts
			for(int j=0; j<KS_NUM; ++j)
			{
				if(j!=shortcut && m_Mapping[j].key == i && m_Mapping[j].type == INPUTMAP_MOUSE && !m_Mapping[j].allowKeySharing)
				{
					conflict = true;
					return true;
				}
			}
			m_Mapping[shortcut].type = INPUTMAP_MOUSE;
			m_Mapping[shortcut].key = i;
			return true;
		}
	}

	for(int i=0; i<256; ++i)
	{
		if(KbdReleased[i] && i!=kbsEsc && i!=kbsLWin && i!=kbsRWin && i!=kbsIns && i!=kbsDel) // do not allow to remap to Esc key, win keys. also do not allow to remap to insert key, as it is used by hackers as default key to enable cheat menu
		{
			// check for conflicts
			for(int j=0; j<KS_NUM; ++j)
			{
				if(j!=shortcut && m_Mapping[j].key == i && m_Mapping[j].type == INPUTMAP_KEYBOARD && !m_Mapping[j].allowKeySharing)
				{
					conflict = true;
					return true;
				}
			}
			m_Mapping[shortcut].type = INPUTMAP_KEYBOARD;
			m_Mapping[shortcut].key = i;
			return true;
		}
	}

	return false;
}
