//David Oguns
//January 17, 2011
//DirectInput.cpp


#include <stdio.h>
#include <iostream>
#include "DirectInput.h"
#include <string.h>
#include "DXUtil.h"

namespace Accevo
{
	using std::endl;
	using std::wcout;
	
	BOOL CALLBACK EnumJoysticksCallbackCount(const DIDEVICEINSTANCE * pdidInstance, void * nJoysticks)
	{	//increment number of joysticks found
		(*(int *)nJoysticks)++;

		return DIENUM_CONTINUE;
	}

	BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE * pdidInstance, void * pContext)
	{
		DIDEVCAPS	diDevCaps;
		DirectInput	*lpdi = (DirectInput *)pContext;
		DWORD hr;

		hr = lpdi->m_pDInput->CreateDevice(pdidInstance->guidInstance, &(lpdi->m_ppJoysticks[lpdi->m_nEnumJoystick]), NULL);
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(lpdi->m_pLogger, L"Could not create joystick device.", hr, return DIENUM_CONTINUE);

		lpdi->m_pEnumJoystick = lpdi->m_ppJoysticks[lpdi->m_nEnumJoystick];
	
		hr = lpdi->m_pEnumJoystick->SetDataFormat(&c_dfDIJoystick2);
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(lpdi->m_pLogger, L"Could not set joystick device format.", hr, return DIENUM_CONTINUE);

		hr = lpdi->m_pEnumJoystick->SetCooperativeLevel(lpdi->m_config.hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(lpdi->m_pLogger, L"Could not set joystick cooperative level.", hr, return DIENUM_CONTINUE);
		
		hr = lpdi->m_pEnumJoystick->Acquire();
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(lpdi->m_pLogger, L"WARNING: Failed to Acquire joystick on application launch.", hr, return DIENUM_CONTINUE);

		hr = lpdi->m_pEnumJoystick->EnumObjects(EnumAxesCallback, pContext, DIDFT_AXIS);
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(lpdi->m_pLogger, L"Could not enumerate joystick objects.", hr, return DIENUM_CONTINUE);

		//copy the name of the joystick
		//strcpy_s(lpdi->m_pJoystickInfo[lpdi->m_nEnumJoystick].szName, pdidInstance->tszInstanceName);
		wcscpy_s(lpdi->m_pJoystickInfo[lpdi->m_nEnumJoystick].szName, pdidInstance->tszInstanceName);
		diDevCaps.dwSize = sizeof(DIDEVCAPS);
		hr = lpdi->m_pEnumJoystick->GetCapabilities(&diDevCaps);
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(lpdi->m_pLogger, L"Could not get device capabilities.", hr, return DIENUM_CONTINUE);

		lpdi->m_pJoystickInfo[lpdi->m_nEnumJoystick].nAxes = diDevCaps.dwAxes;
		lpdi->m_pJoystickInfo[lpdi->m_nEnumJoystick].nButtons = diDevCaps.dwButtons;
		lpdi->m_pJoystickInfo[lpdi->m_nEnumJoystick].nPovs = diDevCaps.dwPOVs;

		lpdi->m_nEnumJoystick++;
		return DIENUM_CONTINUE;
	}

	BOOL CALLBACK EnumAxesCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
	{
		DWORD hr;
		DirectInput *lpdi = (DirectInput *)pvRef;
		DIPROPRANGE		diPropRange;
		diPropRange.diph.dwSize       = sizeof(DIPROPRANGE); 
		diPropRange.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
		diPropRange.diph.dwHow        = DIPH_BYID; 
		diPropRange.diph.dwObj        = lpddoi->dwType;
		diPropRange.lMin              = (LONG)-AEINPUT_AXES_MAX;
		diPropRange.lMax              = (LONG)AEINPUT_AXES_MAX;
	
		//set every axis to max positive value of 1000 and min negative value to -1000
		hr = lpdi->m_pEnumJoystick->SetProperty(DIPROP_RANGE, &diPropRange.diph);
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(lpdi->m_pLogger, L"Could not set property of joystick axis.", hr, return DIENUM_CONTINUE);
		
		return DIENUM_CONTINUE;
	}

	BOOL CALLBACK EnumPovCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
	{


		return DIENUM_CONTINUE;
	}

	DirectInput::DirectInput(Logger *pLogger) :
		m_pLogger(pLogger),
		m_pMouse(nullptr),
		m_pKeyboard(nullptr),
		m_pDInput(nullptr),
		m_ppJoysticks(nullptr),
		m_bIsInitialized(false)
	{

	}

	bool DirectInput::Configure(InputConfiguration const &config)
	{
		if(!m_bIsInitialized)
		{
			m_config = config;
			return true;
		}
		AELOG_WARN(m_pLogger, "Attempting to configure input while already initialized!  Shut down first.");
		return false;
	}

	bool DirectInput::Initialize()
	{
		if(!IsInitialized())
		{
			m_bIsInitialized = false;

			m_nJoysticks = 0;
			m_nEnumJoystick = 0;
			m_pEnumJoystick = NULL;
			m_pJoystates = NULL;
			m_pOldJoystates = NULL;

			ZeroMemory(m_KeyboardState, sizeof(m_KeyboardState));
			ZeroMemory(&m_MouseState, sizeof(m_MouseState));
			ZeroMemory(m_KeyboardState2, sizeof(m_KeyboardState2));
			ZeroMemory(&m_MouseState2, sizeof(m_MouseState2));

			m_pMouseState = &m_MouseState;
			m_pOldMouseState = &m_MouseState2;
			m_pKeyboardState = m_KeyboardState;
			m_pOldKeyboardState = m_KeyboardState2;

			m_hr = DirectInput8Create(m_config.hInst, DIRECTINPUT_VERSION, 
				IID_IDirectInput8, (void**)&m_pDInput, 0);
			AELOG_DXERR_CONDITIONAL_CODE_FATAL(m_pLogger, L"Could not create DirectInput8.", m_hr, return false);

			m_hr = m_pDInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, 0);
			AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not create keyboard device.", m_hr, return false);

			m_hr = m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
			AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not set keyboard data format.", m_hr, return false);

			m_hr = m_pKeyboard->SetCooperativeLevel(m_config.hWnd, m_config.keyboardCoopFlags);
			AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not set keyboard cooperative level.", m_hr, return false);

			m_hr = m_pKeyboard->Acquire();
			AELOG_DXERR_CONDITIONAL_WARN(m_pLogger, L"WARNING: Failed to acquire keyboard on application startup.", m_hr);
		
			m_hr = m_pDInput->CreateDevice(GUID_SysMouse, &m_pMouse, 0);
			AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger,L"Could not create mouse device.", m_hr, return false);

			m_hr = m_pMouse->SetDataFormat(&c_dfDIMouse2);
			AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger,L"Could not set mouse data format.", m_hr, return false);

			m_hr = m_pMouse->SetCooperativeLevel(m_config.hWnd, m_config.mouseCoopFlags);
			AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not set mouse cooperative level.", m_hr, return false);

			m_hr = m_pMouse->Acquire();
			AELOG_DXERR_CONDITIONAL_WARN(m_pLogger, L"WARNING: Could not acquire mouse on application startup.", m_hr);

			//stop showing windows cursor - if the game needs one, it should create one
			ShowCursor(FALSE);

			//Allocate memory for joystick structures
			m_ppJoysticks = new IDirectInputDevice8*[m_config.nMaxJoysticks];
			ZeroMemory(m_ppJoysticks, sizeof(IDirectInputDevice8*) * m_config.nMaxJoysticks);
			m_pJoystates = new DIJOYSTATE2[m_config.nMaxJoysticks];
			ZeroMemory(m_pJoystates, sizeof(DIJOYSTATE2) * m_config.nMaxJoysticks);
			m_pOldJoystates = new DIJOYSTATE2[m_config.nMaxJoysticks];
			ZeroMemory(m_pOldJoystates, sizeof(DIJOYSTATE2) * m_config.nMaxJoysticks);
			m_pJoystickInfo = new JoystickInfo[m_config.nMaxJoysticks];			//information about each joystick
			ZeroMemory(m_pJoystickInfo, sizeof(JoystickInfo) * m_config.nMaxJoysticks);

			//find out how many joysticks are attached
			m_hr = m_pDInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallbackCount, &m_nJoysticks, DIEDFL_ATTACHEDONLY);
			if(FAILED(m_hr))
			{
				AELOG_DXERR_ERROR(m_pLogger, L"Error enumerating joystick devices for count.", m_hr);
			}
			else
			{
				//setup joysticks that are attached and get info from each
				m_hr = m_pDInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, this, DIEDFL_ATTACHEDONLY);
				AELOG_DXERR_CONDITIONAL_ERROR(m_pLogger, L"Error enumerating joystick devices.", m_hr);
			}

			m_parsebuffer = new char[512];
			m_controlMap = map<char*, DWORD, lessThanStr>();

			m_controlMap[(char*)"GAMECTRL_NULL"] = AEINPUT_GAMECTRL_NULL;
			m_controlMap[(char*)"GAMECTRL_KEYBOARD"] = AEINPUT_GAMECTRL_KEYBOARD;
			m_controlMap[(char*)"GAMECTRL_MOUSE"] = AEINPUT_GAMECTRL_MOUSE;

			m_controlMap[(char*)"GAMECTRL_JOYSTICKN(0)"] = AEINPUT_GAMECTRL_JOYSTICKN(0);
			m_controlMap[(char*)"GAMECTRL_JOYSTICKN(1)"] = AEINPUT_GAMECTRL_JOYSTICKN(1);
			m_controlMap[(char*)"GAMECTRL_JOYSTICKN(2)"] = AEINPUT_GAMECTRL_JOYSTICKN(2);
			m_controlMap[(char*)"GAMECTRL_JOYSTICKN(3)"] = AEINPUT_GAMECTRL_JOYSTICKN(3);
			m_controlMap[(char*)"GAMECTRL_JOYSTICKN(3)"] = AEINPUT_GAMECTRL_JOYSTICKN(3);
			m_controlMap[(char*)"GAMECTRL_JOYSTICKN(4)"] = AEINPUT_GAMECTRL_JOYSTICKN(4);
			m_controlMap[(char*)"GAMECTRL_JOYSTICKN(5)"] = AEINPUT_GAMECTRL_JOYSTICKN(5);
			m_controlMap[(char*)"GAMECTRL_JOYSTICKN(6)"] = AEINPUT_GAMECTRL_JOYSTICKN(6);
			m_controlMap[(char*)"GAMECTRL_JOYSTICKN(7)"] = AEINPUT_GAMECTRL_JOYSTICKN(7);
			m_controlMap[(char*)"GAMECTRL_JOYSTICKN(8)"] = AEINPUT_GAMECTRL_JOYSTICKN(8);
			m_controlMap[(char*)"GAMECTRL_JOYSTICKN(9)"] = AEINPUT_GAMECTRL_JOYSTICKN(9);
			m_controlMap[(char*)"GAMECTRL_JOYSTICKN(10)"] = AEINPUT_GAMECTRL_JOYSTICKN(10);
			m_controlMap[(char*)"GAMECTRL_JOYSTICKN(11)"] = AEINPUT_GAMECTRL_JOYSTICKN(11);
			m_controlMap[(char*)"GAMECTRL_JOYSTICKN(12)"] = AEINPUT_GAMECTRL_JOYSTICKN(12);
			m_controlMap[(char*)"GAMECTRL_JOYSTICKN(13)"] = AEINPUT_GAMECTRL_JOYSTICKN(13);
			m_controlMap[(char*)"GAMECTRL_JOYSTICKN(14)"] = AEINPUT_GAMECTRL_JOYSTICKN(14);
			m_controlMap[(char*)"GAMECTRL_JOYSTICKN(15)"] = AEINPUT_GAMECTRL_JOYSTICKN(15);

			m_controlMap[(char*)"CTRLACTION_DOWN"] = AEINPUT_CTRLACTION_DOWN;
			m_controlMap[(char*)"CTRLACTION_PRESS"] = AEINPUT_CTRLACTION_PRESS;
			m_controlMap[(char*)"CTRLACTION_RELEASE"] = AEINPUT_CTRLACTION_RELEASE;
			m_controlMap[(char*)"CTRLACTION_AXES"] = AEINPUT_CTRLACTION_AXES;

			m_controlMap[(char*)"AXIS_X"] = AEINPUT_AXIS_X;
			m_controlMap[(char*)"AXIS_Y"] = AEINPUT_AXIS_Y;
			m_controlMap[(char*)"AXIS_Z"] = AEINPUT_AXIS_Z;
			m_controlMap[(char*)"AXIS_Rx"] = AEINPUT_AXIS_Rx;
			m_controlMap[(char*)"AXIS_Ry"] = AEINPUT_AXIS_Ry;
			m_controlMap[(char*)"AXIS_Rz"] = AEINPUT_AXIS_Rz;

			m_controlMap[(char*)"AXIS_POS"] = AEINPUT_AXIS_POS;
			m_controlMap[(char*)"AXIS_NEG"] = AEINPUT_AXIS_NEG;

			m_controlMap[(char*)"JPOV_UP"] = AEINPUT_JPOV_UP;
			m_controlMap[(char*)"JPOV_DOWN"] = AEINPUT_JPOV_DOWN;
			m_controlMap[(char*)"JPOV_LEFT"] = AEINPUT_JPOV_LEFT;
			m_controlMap[(char*)"JPOV_RIGHT"] = AEINPUT_JPOV_RIGHT;

			m_controlMap[(char*)"DIK_A"] = DIK_A;
			m_controlMap[(char*)"DIK_B"] = DIK_B;
			m_controlMap[(char*)"DIK_C"] = DIK_C;
			m_controlMap[(char*)"DIK_D"] = DIK_D;
			m_controlMap[(char*)"DIK_E"] = DIK_E;
			m_controlMap[(char*)"DIK_F"] = DIK_F;
			m_controlMap[(char*)"DIK_G"] = DIK_G;
			m_controlMap[(char*)"DIK_H"] = DIK_H;
			m_controlMap[(char*)"DIK_I"] = DIK_I;
			m_controlMap[(char*)"DIK_J"] = DIK_J;
			m_controlMap[(char*)"DIK_K"] = DIK_K;
			m_controlMap[(char*)"DIK_L"] = DIK_L;
			m_controlMap[(char*)"DIK_M"] = DIK_M;
			m_controlMap[(char*)"DIK_N"] = DIK_N;
			m_controlMap[(char*)"DIK_O"] = DIK_O;
			m_controlMap[(char*)"DIK_P"] = DIK_P;
			m_controlMap[(char*)"DIK_Q"] = DIK_Q;
			m_controlMap[(char*)"DIK_R"] = DIK_R;
			m_controlMap[(char*)"DIK_S"] = DIK_S;
			m_controlMap[(char*)"DIK_T"] = DIK_T;
			m_controlMap[(char*)"DIK_U"] = DIK_U;
			m_controlMap[(char*)"DIK_V"] = DIK_V;
			m_controlMap[(char*)"DIK_W"] = DIK_W;
			m_controlMap[(char*)"DIK_X"] = DIK_X;
			m_controlMap[(char*)"DIK_Y"] = DIK_Y;
			m_controlMap[(char*)"DIK_Z"] = DIK_Z;
			m_controlMap[(char*)"DIK_ESCAPE"] = DIK_ESCAPE;
			m_controlMap[(char*)"DIK_1"] = DIK_1;
			m_controlMap[(char*)"DIK_2"] = DIK_2;
			m_controlMap[(char*)"DIK_3"] = DIK_3;
			m_controlMap[(char*)"DIK_4"] = DIK_4;
			m_controlMap[(char*)"DIK_5"] = DIK_5;
			m_controlMap[(char*)"DIK_6"] = DIK_6;
			m_controlMap[(char*)"DIK_7"] = DIK_7;
			m_controlMap[(char*)"DIK_8"] = DIK_8;
			m_controlMap[(char*)"DIK_9"] = DIK_9;
			m_controlMap[(char*)"DIK_0"] = DIK_0;
			m_controlMap[(char*)"DIK_MINUS"] = DIK_MINUS;
			m_controlMap[(char*)"DIK_EQUALS"] = DIK_EQUALS;
			m_controlMap[(char*)"DIK_BACK"] = DIK_BACK;
			m_controlMap[(char*)"DIK_TAB"] = DIK_TAB;
			m_controlMap[(char*)"DIK_LBRACKET"] = DIK_LBRACKET;
			m_controlMap[(char*)"DIK_RBRACKET"] = DIK_RBRACKET;
			m_controlMap[(char*)"DIK_RETURN"] = DIK_RETURN;
			m_controlMap[(char*)"DIK_LCONTROL"] = DIK_LCONTROL;
			m_controlMap[(char*)"DIK_SEMICOLON"] = DIK_SEMICOLON;
			m_controlMap[(char*)"DIK_APOSTROPHE"] = DIK_APOSTROPHE;
			m_controlMap[(char*)"DIK_GRAVE"] = DIK_GRAVE;
			m_controlMap[(char*)"DIK_LSHIFT"] = DIK_LSHIFT;
			m_controlMap[(char*)"DIK_BACKSLASH"] = DIK_BACKSLASH;
			m_controlMap[(char*)"DIK_COMMA"] = DIK_COMMA;
			m_controlMap[(char*)"DIK_PERIOD"] = DIK_PERIOD;
			m_controlMap[(char*)"DIK_SLASH"] = DIK_SLASH;
			m_controlMap[(char*)"DIK_RSHIFT"] = DIK_RSHIFT;
			m_controlMap[(char*)"DIK_MULTIPLY"] = DIK_MULTIPLY;
			m_controlMap[(char*)"DIK_LMENU"] = DIK_LMENU;
			m_controlMap[(char*)"DIK_SPACE"] = DIK_SPACE;
			m_controlMap[(char*)"DIK_CAPITAL"] = DIK_CAPITAL;
			m_controlMap[(char*)"DIK_NUMPAD0"] = DIK_NUMPAD0;
			m_controlMap[(char*)"DIK_NUMPAD1"] = DIK_NUMPAD1;
			m_controlMap[(char*)"DIK_NUMPAD2"] = DIK_NUMPAD2;
			m_controlMap[(char*)"DIK_NUMPAD3"] = DIK_NUMPAD3;
			m_controlMap[(char*)"DIK_NUMPAD4"] = DIK_NUMPAD4;
			m_controlMap[(char*)"DIK_NUMPAD5"] = DIK_NUMPAD5;
			m_controlMap[(char*)"DIK_NUMPAD6"] = DIK_NUMPAD6;
			m_controlMap[(char*)"DIK_NUMPAD7"] = DIK_NUMPAD7;
			m_controlMap[(char*)"DIK_NUMPAD8"] = DIK_NUMPAD8;
			m_controlMap[(char*)"DIK_NUMPAD9"] = DIK_NUMPAD9;
			m_controlMap[(char*)"DIK_RCONTROL"] = DIK_RCONTROL;
			m_controlMap[(char*)"DIK_UP"] = DIK_UP;
			m_controlMap[(char*)"DIK_LEFT"] = DIK_LEFT;
			m_controlMap[(char*)"DIK_RIGHT"] = DIK_RIGHT;
			m_controlMap[(char*)"DIK_END"] = DIK_END;
			m_controlMap[(char*)"DIK_DOWN"] = DIK_DOWN;
			m_controlMap[(char*)"DIK_NEXT"] = DIK_NEXT;
			m_controlMap[(char*)"DIK_PRIOR"] = DIK_PRIOR;
			m_controlMap[(char*)"DIK_LWIN"] = DIK_LWIN;
			m_controlMap[(char*)"DIK_RWIN"] = DIK_RWIN;

			m_bIsInitialized = true;
			return true;
		}
		else
		{
			AELOG_WARN(m_pLogger, "Attempting to initialize already initialized subsystem");
			return false;
		}
	}

	/*
	 * Takes in a const char* which describes a string containing
	 * human-readable control values, returns a DWORD that is used
	 * to reference that particular control.
	 *
	 * control - the human readable control we're interested in
	 * returns: the bitmask of the control (once all portions are masked in)
	 */
	DWORD DirectInput::ParseControl(const char* control)
	{
			DWORD retval = 0;

			//cout << "Input string: " << control << endl;
			//char* tmpstring = (char*)malloc( strlen( control ) + 1 );
			char* nexttoken;
			char* saveptr = m_parsebuffer;
			strncpy_s( m_parsebuffer, strlen(control) + 1, control, -1 );
			//cout << "Input string: " << tmpstring << endl;

			m_parsebuffer = strtok_s( m_parsebuffer, "|", &nexttoken );
			unsigned int buttonvar;
			while ( m_parsebuffer != NULL ){
				if ( (buttonvar = atoi(m_parsebuffer) ) != 0 ){
					//cout << "HEY WE FOUND A BUTTON NUMBER " << buttonvar << endl;
					retval |= buttonvar;
				} else {
					retval |= m_controlMap[m_parsebuffer];
					//cout << parsebuffer << ": " << controlMap[parsebuffer] << endl;
					//printf( "%s: %X\n", tmpstring, controlMap[tmpstring] );
				}
				m_parsebuffer = strtok_s( NULL, "|", &nexttoken );
			}

			//free( tmpstring );
			m_parsebuffer = saveptr;

			//cout << "DWORD returned by parseControl(): " << retval << endl;
			//printf( "retval: %X, or %d in decimal\n", retval, retval );

			return retval;
	}

	void DirectInput::Shutdown()
	{
		if(m_bIsInitialized)
		{
			AELOG_DEBUG(m_pLogger, "Shutting down input system.");

			// get rid of the parse control buffer.
			delete [] m_parsebuffer;

			if(m_pKeyboard)
				m_pKeyboard->Unacquire();
			if(m_pMouse)
			m_pMouse->Unacquire();
			DX_RELEASE(m_pKeyboard);
			DX_RELEASE(m_pMouse);
			//Show windows cursor
			ShowCursor(TRUE);

			if(m_pJoystates)
			{
				delete [] m_pJoystates;
				m_pJoystates = NULL;
			}

			if(m_pOldJoystates)
			{
				delete [] m_pOldJoystates;
				m_pOldJoystates = NULL;
			}

			if(m_ppJoysticks)
			{
				for(int i = 0; i < m_nJoysticks; i++)
				{
					m_ppJoysticks[i]->Unacquire();
					DX_RELEASE(m_ppJoysticks[i]);
				}
				delete [] m_ppJoysticks;
			}

			if(m_pJoystickInfo)
			{
				delete [] m_pJoystickInfo;
				m_pJoystickInfo = NULL;
			}

			DX_RELEASE(m_pDInput);
			m_bIsInitialized = false;
		}
		else
		{
			AELOG_WARN(m_pLogger, "Attempting to shutdown an already uninitialized input subsystem.");
		}
	}

	DirectInput::~DirectInput()
	{
		
	}

	void DirectInput::PollInputDevices()
	{

		//swap input state pointers -- could optimize slightly with binary swap
		void *tmp = m_pKeyboardState;
		m_pKeyboardState = m_pOldKeyboardState;
		m_pOldKeyboardState = (unsigned char *)tmp;

		tmp = m_pMouseState;
		m_pMouseState = m_pOldMouseState;
		m_pOldMouseState = (DIMOUSESTATE2 *)tmp;

		tmp = m_pJoystates;
		m_pJoystates = m_pOldJoystates;
		m_pOldJoystates = (DIJOYSTATE2 *)tmp;

		// Poll keyboard.(Take a "snapshot" of the current state of the keyboard
		HRESULT hr = m_pKeyboard->GetDeviceState(sizeof(m_KeyboardState), (void*)m_pKeyboardState); 
		if( FAILED(hr) )
		{
			// Keyboard lost, zero out keyboard data structure.
			ZeroMemory(m_KeyboardState, sizeof(m_KeyboardState));
			ZeroMemory(m_KeyboardState2, sizeof(m_KeyboardState2));

			 // Try to acquire for next time we poll.
			hr = m_pKeyboard->Acquire();
			if(FAILED(hr))
			{
				//OutputDXError("Failed to re-acquire keyboard.");
			}
		}

		// Poll mouse.
		hr = m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE2), (void*)m_pMouseState); 
		if( FAILED(hr) )
		{
			// Mouse lost, zero out mouse data structure.
			ZeroMemory(&m_MouseState, sizeof(m_MouseState));
			ZeroMemory(&m_MouseState2, sizeof(m_MouseState2));

			// Try to acquire for next time we poll.
			hr = m_pMouse->Acquire();
			if(FAILED(hr))
			{
				//OutputDXError("Failed to re-acquire mouse.");
			}
		}

		// Poll every joystick
		if(m_ppJoysticks)
		{
			for(int i = 0; i < m_nJoysticks; i++)
			{
				hr = m_ppJoysticks[i]->GetDeviceState(sizeof(DIJOYSTATE2), (void*)&m_pJoystates[i]);
				if( FAILED( hr ) )
				{
					// Joystick lost, zero out joystick data structure
					ZeroMemory(m_pJoystates, sizeof(DIJOYSTATE2) * m_nJoysticks);
					ZeroMemory(m_pOldJoystates, sizeof(DIJOYSTATE2) * m_nJoysticks);
					// Try to acquire for next time we poll.
					hr = m_ppJoysticks[i]->Acquire();
					if(FAILED(hr))
					{
						//OutputDXError("Failed to re-acquire a joystick.");
					}
				}
			}
		}
	}

	bool DirectInput::KeyDown(unsigned char key)
	{
		return KEYDOWN(m_pKeyboardState, key) != 0;
	}

	bool DirectInput::MouseButtonDown(int button)
	{
		return KEYDOWN(m_pMouseState->rgbButtons, button) != 0;
	}

	bool DirectInput::KeyPress(unsigned char key)
	{	//pressed this frame
		return KEYDOWN(m_pKeyboardState, key) && !KEYDOWN(m_pOldKeyboardState, key);
	}

	bool DirectInput::KeyRelease(unsigned char key)
	{	//down last frame but not this one
		return !KEYDOWN(m_pKeyboardState, key) && KEYDOWN(m_pOldKeyboardState, key);
	}

	bool DirectInput::MouseButtonPress(int button)
	{
		return KEYDOWN(m_pMouseState->rgbButtons, button) && !KEYDOWN(m_pOldMouseState->rgbButtons, button);
	}

	bool DirectInput::MouseButtonRelease(int button)
	{
		return !KEYDOWN(m_pMouseState->rgbButtons, button) && KEYDOWN(m_pOldMouseState->rgbButtons, button);
	}

	float DirectInput::MouseDX()
	{
		return (float)m_pMouseState->lX;
	}

	float DirectInput::MouseDY()
	{
		return (float)m_pMouseState->lY;
	}

	float DirectInput::MouseDZ()
	{
		return (float)m_pMouseState->lZ;
	}

	bool DirectInput::JoyDown(int button, int nJoy)
	{
		if(nJoy >= m_nJoysticks)
			return false;

		return KEYDOWN(m_pJoystates[nJoy].rgbButtons, button) != 0;
	}

	bool DirectInput::JoyOldDown(int button, int nJoy)
	{
		if(nJoy >= m_nJoysticks)
			return false;

		return KEYDOWN(m_pOldJoystates[nJoy].rgbButtons, button) != 0;
	}

	bool DirectInput::JoyPress(int button, int nJoy)
	{
		if(nJoy >= m_nJoysticks)
			return false;

		return KEYDOWN(m_pJoystates[nJoy].rgbButtons, button) && !KEYDOWN(m_pOldJoystates[nJoy].rgbButtons, button);
	}

	bool DirectInput::JoyRelease(int button, int nJoy)
	{
		if(nJoy >= m_nJoysticks)
			return false;

		return !KEYDOWN(m_pJoystates[nJoy].rgbButtons, button) && KEYDOWN(m_pOldJoystates[nJoy].rgbButtons, button);
	}

	float DirectInput::JoyAxesPos(DWORD axes, int nJoy)
	{
		float ret;
		if(nJoy >= m_nJoysticks)
			return 0.0f;

		switch(axes)
		{
			case AEINPUT_AXIS_X:
				ret = m_pJoystates[nJoy].lX / AEINPUT_AXES_MAX;
				break;
			case AEINPUT_AXIS_Y:
				ret = m_pJoystates[nJoy].lY / AEINPUT_AXES_MAX;
				break;
			case AEINPUT_AXIS_Z:
				ret = m_pJoystates[nJoy].lZ / AEINPUT_AXES_MAX;
				break;
			case AEINPUT_AXIS_Rx:
				ret = m_pJoystates[nJoy].lRx / AEINPUT_AXES_MAX;
				break;
			case AEINPUT_AXIS_Ry:
				ret = m_pJoystates[nJoy].lRy / AEINPUT_AXES_MAX;
				break;
			case AEINPUT_AXIS_Rz:
				ret = m_pJoystates[nJoy].lRz / AEINPUT_AXES_MAX;
				break;
		}

		if( ret < AEINPUT_AXES_DEADZONE && ret > -AEINPUT_AXES_DEADZONE )
			ret = 0.0f;

		return ret;
	}

	float DirectInput::JoyOldAxesPos(DWORD axes, int nJoy)
	{
		float ret;
		if(nJoy >= m_nJoysticks)
			return 0.0f;

		switch(axes)
		{
			case AEINPUT_AXIS_X:
				ret = m_pOldJoystates[nJoy].lX / AEINPUT_AXES_MAX;
				break;
			case AEINPUT_AXIS_Y:
				ret = m_pOldJoystates[nJoy].lY / AEINPUT_AXES_MAX;
				break;
			case AEINPUT_AXIS_Z:
				ret = m_pOldJoystates[nJoy].lZ / AEINPUT_AXES_MAX;
				break;
			case AEINPUT_AXIS_Rx:
				ret = m_pOldJoystates[nJoy].lRx / AEINPUT_AXES_MAX;
				break;
			case AEINPUT_AXIS_Ry:
				ret = m_pOldJoystates[nJoy].lRy / AEINPUT_AXES_MAX;
				break;
			case AEINPUT_AXIS_Rz:
				ret = m_pOldJoystates[nJoy].lRz / AEINPUT_AXES_MAX;
				break;
		}

		if( ret < AEINPUT_AXES_DEADZONE && ret > -AEINPUT_AXES_DEADZONE )
			ret = 0.0f;

		return ret;
	}

	bool DirectInput::IsControl(DWORD control)
	{
		switch( AEINPUT_GAMECTRL_GETDEVICE(control) )
		{
			case AEINPUT_GAMECTRL_KEYBOARD:
				return IsKeyControl( control );
			case AEINPUT_GAMECTRL_MOUSE:
				return IsMouseControl( control );
			case AEINPUT_GAMECTRL_JOYSTICK:
				return IsJoyControl( control );
			case AEINPUT_GAMECTRL_NULL:
				return false;
		}
		//control not defined properly, return false
		return false;
	}

	bool DirectInput::IsJoyControl(DWORD control)
	{
		switch( AEINPUT_GAMECTRL_GETACTION(control) )
		{	//axes action type falls through
			case AEINPUT_CTRLACTION_PRESS:
				return IsJoyControlPress( control );
			case AEINPUT_CTRLACTION_DOWN:
			case AEINPUT_CTRLACTION_AXES:
				return IsJoyControlDown( control );
			case AEINPUT_CTRLACTION_RELEASE:
				return IsJoyControlRelease( control );
		}
		//control not defined properly, return false
		return false;
	}

	bool DirectInput::IsKeyControl(DWORD control)
	{
		switch( AEINPUT_GAMECTRL_GETACTION(control) )
		{
			case AEINPUT_CTRLACTION_PRESS:
				return IsKeyControlPress( control );
			case AEINPUT_CTRLACTION_DOWN:
			case AEINPUT_CTRLACTION_AXES:
				return IsKeyControlDown( control );
			case AEINPUT_CTRLACTION_RELEASE:
				return IsKeyControlRelease( control );
		}
		//control not defined properly, return false
		return false;
	}

	bool DirectInput::IsMouseControl(DWORD control)
	{
		switch( AEINPUT_GAMECTRL_GETACTION(control) )
		{
			case AEINPUT_CTRLACTION_PRESS:
				return IsMouseControlPress( control );
			case AEINPUT_CTRLACTION_DOWN:
			case AEINPUT_CTRLACTION_AXES:
				return IsMouseControlDown( control );
			case AEINPUT_CTRLACTION_RELEASE:
				return IsMouseControlRelease( control );
		}
		//control not defined properly, return false
		return false;
	}

	bool DirectInput::IsJoyControlPress(DWORD control)
	{
		float pos = 0.0f;
		float oldPos = 0.0f;
		if(control & AEINPUT_CTRLACTION_AXES)
		{
			return IsJoyControlDown(control) && !IsOldJoyControlDown(control);
		}
		if(AEINPUT_GAMECTRL_GETPOV(control))
		{
			return IsJoyPOVPress(control);
		}

		return JoyPress(AEINPUT_GAMECTRL_GETBUTTON(control), AEINPUT_GAMECTRL_GETJOYSTICK(control));
	}

	bool DirectInput::IsKeyControlPress(DWORD control)
	{
		if(control & AEINPUT_CTRLACTION_AXES)
		{
			switch(AEINPUT_GAMECTRL_GETAXIS(control))
			{
				case AEINPUT_AXIS_X:
					if(AEINPUT_GAMECTRL_GETAXISPOLARITY(control) & AEINPUT_AXIS_POS)
						return KeyPress(DIK_RIGHT);
					else if(AEINPUT_GAMECTRL_GETAXISPOLARITY(control) & AEINPUT_AXIS_NEG)
						return KeyPress(DIK_LEFT);
					break;
				case AEINPUT_AXIS_Y:
					if(AEINPUT_GAMECTRL_GETAXISPOLARITY(control) & AEINPUT_AXIS_POS)
						return KeyPress(DIK_UP);
					else if(AEINPUT_GAMECTRL_GETAXISPOLARITY(control) & AEINPUT_AXIS_NEG)
						return KeyPress(DIK_DOWN);
					break;
				case AEINPUT_AXIS_Z:
					return false;
					break;
				case AEINPUT_AXIS_Rx:
					return false;
					break;
				case AEINPUT_AXIS_Ry:
					return false;
					break;
				case AEINPUT_AXIS_Rz:
					return false;
					break;
			}
			return false;
		}

		return KeyPress((unsigned char)AEINPUT_GAMECTRL_GETBUTTON(control)) != 0;
	}

	bool DirectInput::IsMouseControlPress(DWORD control)
	{
		if(control & AEINPUT_CTRLACTION_AXES)
		{
			return false;
		}

		return MouseButtonPress(AEINPUT_GAMECTRL_GETBUTTON(control));
	}

	bool DirectInput::IsJoyControlRelease(DWORD control)
	{
		if(control & AEINPUT_CTRLACTION_AXES)
		{
			return !IsJoyControlDown(control) && IsOldJoyControlDown(control);
		}
		if(AEINPUT_GAMECTRL_GETPOV(control))
		{
			return IsJoyPOVRelease(control);
		}

		return JoyRelease(AEINPUT_GAMECTRL_GETBUTTON(control), AEINPUT_GAMECTRL_GETJOYSTICK(control));
	}

	bool DirectInput::IsKeyControlRelease(DWORD control)
	{
		if(control & AEINPUT_CTRLACTION_AXES)
		{
			switch(AEINPUT_GAMECTRL_GETAXIS(control))
			{
				case AEINPUT_AXIS_X:
					if(AEINPUT_GAMECTRL_GETAXISPOLARITY(control) & AEINPUT_AXIS_POS)
						return KeyRelease(DIK_RIGHT);
					else if(AEINPUT_GAMECTRL_GETAXISPOLARITY(control) & AEINPUT_AXIS_NEG)
						return KeyRelease(DIK_LEFT);
					break;
				case AEINPUT_AXIS_Y:
					if(AEINPUT_GAMECTRL_GETAXISPOLARITY(control) & AEINPUT_AXIS_POS)
						return KeyRelease(DIK_UP);
					else if(AEINPUT_GAMECTRL_GETAXISPOLARITY(control) & AEINPUT_AXIS_NEG)
						return KeyRelease(DIK_DOWN);
					break;
				case AEINPUT_AXIS_Z:
					return false;
					break;
				case AEINPUT_AXIS_Rx:
					return false;
					break;
				case AEINPUT_AXIS_Ry:
					return false;
					break;
				case AEINPUT_AXIS_Rz:
					return false;
					break;
			}
			return false;
		}

		return KeyRelease((unsigned char)AEINPUT_GAMECTRL_GETBUTTON(control)) != 0;
	}

	bool DirectInput::IsMouseControlRelease(DWORD control)
	{
		if(control & AEINPUT_CTRLACTION_AXES)
		{
			return false;
		}

		return MouseButtonRelease(AEINPUT_GAMECTRL_GETBUTTON(control));
	}

	bool DirectInput::IsJoyControlDown(DWORD control)
	{
		tmpPosition = 0.0f;

		if(control & AEINPUT_CTRLACTION_AXES)
		{
			tmpPosition = JoyAxesPos(AEINPUT_GAMECTRL_GETAXIS(control), AEINPUT_GAMECTRL_GETJOYSTICK(control));

			if(control & AEINPUT_AXIS_POS)
			{
				return tmpPosition > AEINPUT_AXES_DEADZONE;
			}
			else if(control & AEINPUT_AXIS_NEG)
			{
				return tmpPosition < -AEINPUT_AXES_DEADZONE;
			}
			else
			{	//badly defined control
				return false;
			}
		}
		if(AEINPUT_GAMECTRL_GETPOV(control))
		{
			return IsJoyPOVDown(control);
		}

		return JoyDown(AEINPUT_GAMECTRL_GETBUTTON(control), AEINPUT_GAMECTRL_GETJOYSTICK(control));
	}

	bool DirectInput::IsOldJoyControlDown(DWORD control)
	{
		tmpPosition = 0.0f;

		if(control & AEINPUT_CTRLACTION_AXES)
		{
			tmpPosition = JoyOldAxesPos(AEINPUT_GAMECTRL_GETAXIS(control), AEINPUT_GAMECTRL_GETJOYSTICK(control));

			if(control & AEINPUT_AXIS_POS)
			{
				return tmpPosition > AEINPUT_AXES_DEADZONE;
			}
			else if(control & AEINPUT_AXIS_NEG)
			{
				return tmpPosition < -AEINPUT_AXES_DEADZONE;
			}
			else
			{	//badly defined control
				return false;
			}
		}
		if(AEINPUT_GAMECTRL_GETPOV(control))
		{
			return IsOldJoyPOVDown(control);
		}

		return JoyDown(AEINPUT_GAMECTRL_GETBUTTON(control), AEINPUT_GAMECTRL_GETJOYSTICK(control));
	}

	bool DirectInput::IsKeyControlDown(DWORD control)
	{
		if(control & AEINPUT_CTRLACTION_AXES)
		{
			switch(AEINPUT_GAMECTRL_GETAXIS(control))
			{
				case AEINPUT_AXIS_X:
					if(AEINPUT_GAMECTRL_GETAXISPOLARITY(control) & AEINPUT_AXIS_POS)
						return KeyDown(DIK_RIGHT);
					else if(AEINPUT_GAMECTRL_GETAXISPOLARITY(control) & AEINPUT_AXIS_NEG)
						return KeyDown(DIK_LEFT);
					break;
				case AEINPUT_AXIS_Y:
					if(AEINPUT_GAMECTRL_GETAXISPOLARITY(control) & AEINPUT_AXIS_POS)
						return KeyDown(DIK_UP);
					else if(AEINPUT_GAMECTRL_GETAXISPOLARITY(control) & AEINPUT_AXIS_NEG)
						return KeyDown(DIK_DOWN);
					break;
				case AEINPUT_AXIS_Z:
					return false;
					break;
				case AEINPUT_AXIS_Rx:
					return false;
					break;
				case AEINPUT_AXIS_Ry:
					return false;
					break;
				case AEINPUT_AXIS_Rz:
					return false;
					break;
			}
			return false;
		}

		return KeyDown((unsigned char)AEINPUT_GAMECTRL_GETBUTTON(control)) !=0 ;
	}

	bool DirectInput::IsMouseControlDown(DWORD control)
	{
		if(control & AEINPUT_CTRLACTION_AXES)
		{
			return false;
		}

		return MouseButtonDown(AEINPUT_GAMECTRL_GETBUTTON(control));
	}


	float DirectInput::AxesControl(DWORD control)
	{
		switch(AEINPUT_GAMECTRL_GETDEVICE(control))
		{
			case AEINPUT_GAMECTRL_JOYSTICK:
				//return axesJoystickControl(control);
				return JoyAxesPos(AEINPUT_GAMECTRL_GETAXIS(control), AEINPUT_GAMECTRL_GETJOYSTICK(control));
			case AEINPUT_GAMECTRL_MOUSE:
				return AxesMouseControl(control);
			case AEINPUT_GAMECTRL_KEYBOARD:
				return AxesKeyboardControl(control);
			case AEINPUT_GAMECTRL_NULL:
				return 0.0f;
		}
		return 0.0f;
	}

	/*
	float DirectInput::axesJoystickControl(DWORD control)
	{
		return 0.0f;
	}
	*/

	float DirectInput::AxesMouseControl(DWORD control)
	{
		switch(AEINPUT_GAMECTRL_GETAXIS(control))
		{
			case AEINPUT_AXIS_X:
				return (float)m_pMouseState->lX;		//performance hit here during conversion
			case AEINPUT_AXIS_Y:
				return (float)m_pMouseState->lY;		//performance hit here during conversion
			case AEINPUT_AXIS_Z:
				return (float)m_pMouseState->lZ;		//performance hit here during conversion
		}
		return 0.0f;
	}

	float DirectInput::AxesKeyboardControl(DWORD control)
	{
		tmpPosition = 0.0f;
		switch(AEINPUT_GAMECTRL_GETAXIS(control))
		{
			case AEINPUT_AXIS_X:
				if(KeyDown(DIK_LEFT))
					tmpPosition += 1.0f;
				if(KeyDown(DIK_RIGHT))
					tmpPosition -= 1.0f;
				break;
			case AEINPUT_AXIS_Y:
				if(KeyDown(DIK_UP))
					tmpPosition += 1.0f;
				if(KeyDown(DIK_DOWN))
					tmpPosition -= 1.0f;
				break;
		}
		return tmpPosition;
	}


	DWORD DirectInput::JoyPOVPosition(DWORD control)
	{
		return m_pJoystates[AEINPUT_GAMECTRL_GETJOYSTICK(control)].rgdwPOV[0];
	}

	DWORD DirectInput::OldJoyPOVPosition(DWORD control)
	{
		return m_pOldJoystates[AEINPUT_GAMECTRL_GETJOYSTICK(control)].rgdwPOV[0];
	}

	bool DirectInput::IsJoyPOVControl(DWORD control)
	{
		switch(AEINPUT_GAMECTRL_GETACTION(control))
		{
			case AEINPUT_CTRLACTION_PRESS:
				return IsJoyPOVPress(control);
				break;
			case AEINPUT_CTRLACTION_AXES:		//badly defined or allow it?
			case AEINPUT_CTRLACTION_DOWN:
				return IsJoyPOVDown(control);
				break;
			case AEINPUT_CTRLACTION_RELEASE:
				return IsJoyPOVRelease(control);
				break;
		}
		return false;
	}

	bool DirectInput::IsJoyPOVPress(DWORD control)
	{
		return IsJoyPOVDown(control) && !IsOldJoyPOVDown(control);
	}

	bool DirectInput::IsJoyPOVRelease(DWORD control)
	{
		return !IsJoyPOVDown(control) && IsOldJoyPOVDown(control);
	}

	bool DirectInput::IsJoyPOVDown(DWORD control)
	{
		dwPosition = JoyPOVPosition(control);
		switch(AEINPUT_GAMECTRL_GETPOV(control))
		{
			case AEINPUT_JPOV_UP:
				return dwPosition == 0 || dwPosition == 4500 || dwPosition == 31500;
				break;
			case AEINPUT_JPOV_DOWN:
				return dwPosition == 18000 || dwPosition == 13500 || dwPosition == 22500;
				break;
			case AEINPUT_JPOV_LEFT:
				return dwPosition == 27000 || dwPosition == 31500 || dwPosition == 2250;
				break;
			case AEINPUT_JPOV_RIGHT:
				return dwPosition == 9000 || dwPosition == 4500 || dwPosition == 13500;
				break;
		}
		return false;		//badly defined POV control
	}

	bool DirectInput::IsOldJoyPOVDown(DWORD control)
	{
		dwPosition = OldJoyPOVPosition(control);
		switch(AEINPUT_GAMECTRL_GETPOV(control))
		{
			case AEINPUT_JPOV_UP:
				return dwPosition == 0 || dwPosition == 4500 || dwPosition == 31500;
				break;
			case AEINPUT_JPOV_DOWN:
				return dwPosition == 18000 || dwPosition == 13500 || dwPosition == 22500;
				break;
			case AEINPUT_JPOV_LEFT:
				return dwPosition == 27000 || dwPosition == 31500 || dwPosition == 2250;
				break;
			case AEINPUT_JPOV_RIGHT:
				return dwPosition == 9000 || dwPosition == 4500 || dwPosition == 13500;
				break;
		}
		return false;		//badly defined POV control
	}

}	//namespace Accevo