//DirectInput.h
//November 19, 2009



#ifndef DIRECT_INPUT_H
#define DIRECT_INPUT_H

#define DIRECTINPUT_VERSION		0x0800

#include <dinput.h>
#include <map>
#include "Logger.h"
#include "DXUtil.h"
#include "Subsystem.h"

namespace Accevo
{
	using std::map;

	struct JoystickInfo
	{
		wchar_t	szName[MAX_PATH];		//user readable names
		DWORD	nAxes;
		DWORD	nButtons;
		DWORD	nPovs;
	};

	//Notes:
	//			Controls are a complete abstraction of events, device, and button/axes.
	//			There is some level of interpretation assumed when the parameters are mixed
	//			in certain ways.  i.e.  An axes being pressed positive means the axes is currently
	//			in a signficant position beyond the dead zone in that direction, and last frame
	//			the axes was NOT past the dead zone in the same direction.
	//			Polling the keyboard for axes positions can only return -1.0f, 0, or 1.0f  and
	//			the X and Y axes are assumed to be the arrow keys by default.

	class DirectInput : public Subsystem
	{
	public:
		struct InputConfiguration
		{
			HWND			hWnd;
			HINSTANCE		hInst;
			DWORD			keyboardCoopFlags;
			DWORD			mouseCoopFlags;
			unsigned int	nMaxJoysticks;
		};

	public:
		DirectInput(Logger *pLogger);
		~DirectInput();

		DirectInput(const DirectInput &) = delete;
		DirectInput& operator=(const DirectInput &) = delete;

		bool Configure(InputConfiguration const &config);
		//from subsystem
		virtual bool Initialize();
		//from Subsystem
		virtual bool IsInitialized() const { return m_bIsInitialized; }

		virtual void Shutdown();

		void PollInputDevices();
	
		//is keyboard button currently down (DirectInput key)
		bool KeyDown(unsigned char key);
		//has keyboard button just been pressed (DirectInput key)
		bool KeyPress(unsigned char key);
		//has keyboard button just been released (DirectInput key)
		bool KeyRelease(unsigned char key);
	
		//is mouse button # currently down
		bool MouseButtonDown(int button);
		//is mouse button just being pressed
		bool MouseButtonPress(int button);
		//is mouse button just being released
		bool MouseButtonRelease(int button);
		//how far has the mouse moved on the X axis
		float MouseDX();
		//how far has the mouse moved on the Y axis
		float MouseDY();
		//how much has the mouse wheel moved (mouse Z)
		float MouseDZ();

		//is a joystick(N) button # currently down
		bool JoyDown(int button, int nJoy);
		//was a joystick(N) button # down last frame (internal use only)
		bool JoyOldDown(int button, int nJoy);
		//is a joystick(N) button # just pressed
		bool JoyPress(int button, int nJoy);
		//is a joystick(N) button # just released
		bool JoyRelease(int button, int nJoy);
		//get a normalized (-1.0f -> 1.0f) axes position from joystick(N) device
		float JoyAxesPos(DWORD axes, int nJoy);
		//get a normalized (-1.0f -> 1.0f) axes position from joystick(N) device last frame
		float JoyOldAxesPos(DWORD axes, int nJoy);

		//is a given control parameter satisfied
		//must specify: device, action, button/axes-direction
		bool IsControl(DWORD control);
		//is joystick control parameter satisfied
		//must specify: joystick number, action, button/axes-direction
		bool IsJoyControl(DWORD control);
		//is keyboard control parameter satisfied
		//must specify: action, button/axes-direction (X and Y  axes are bound to arrow keys)
		bool IsKeyControl(DWORD control);
		//is mouse control parameter satisfied
		//must specify: button/axes-direction
		bool IsMouseControl(DWORD control);
		//is joystick control parameter satisfied this frame, but not last frame
		//must specify: joystick number(in upper 4 bits of DWORD), button/axes-direction
		bool IsJoyControlPress(DWORD control);
		//is keyboard control parameter satisfied this frame, but not last frame
		//must specify: button/axes-direction
		bool IsKeyControlPress(DWORD control);
		//is mouse control parameter satisfied this frame, but not last frame
		//must specify: button/axes-direction
		bool IsMouseControlPress(DWORD control);
		//is joystick control not satisfied this frame, but was last frame
		//must specify: joystick number, button/axes-direction
		bool IsJoyControlRelease(DWORD control);
		//is keyboard control parameter not satisfied this frame, but was last frame
		//must specify: button/axes-direction
		bool IsKeyControlRelease(DWORD control);
		//is mouse control parameter not satisfied this frame, but was last frame
		//must specify: button/axes-direction
		bool IsMouseControlRelease(DWORD control);
		//is joystick control satisfied this frame
		//must specify: joystick number, button/axes-direction
		bool IsJoyControlDown(DWORD control);
		//is joystick control parameter satisfied this frame
		//must specify: joystick number, button/axes-direction
		bool IsOldJoyControlDown(DWORD control);
		//is keyboard control parameter satisfied this frame
		//must specify: button/axes-direction
		bool IsKeyControlDown(DWORD control);
		//is mouse control parameter satisfied this frame
		//must specify: button/axes-direction
		bool IsMouseControlDown(DWORD control);

		//get axes position from a given device
		//must specify: device(if joystick, number must be specified), axis
		float	AxesControl(DWORD control);
		//get axes position from mouse relative to last frame
		//i.e.  returns +/-1.0f or 0 if mouse moved this frame positively
		//		or negatively on specified axis, or not at all
		float	AxesMouseControl(DWORD control);
		//get axes position from keyboard
		//uses arrow keys to generate appropriate values
		float	AxesKeyboardControl(DWORD control);

		//joystick POV stuff is weird - will document later
		DWORD JoyPOVPosition(DWORD control);
		DWORD OldJoyPOVPosition(DWORD control);
		bool IsJoyPOVControl(DWORD control);
		bool IsJoyPOVPress(DWORD control);
		bool IsJoyPOVRelease(DWORD control);
		bool IsJoyPOVDown(DWORD control);
		bool IsOldJoyPOVDown(DWORD control);

		DWORD ParseControl(const char* control);


	private:
		char									*m_parsebuffer;
		map<char *, DWORD, lessThanStr>			m_controlMap;

		IDirectInput8*			m_pDInput;

		IDirectInputDevice8*	m_pKeyboard;
		unsigned char			m_KeyboardState[256];
		unsigned char			m_KeyboardState2[256];
		unsigned char*			m_pKeyboardState;
		unsigned char*			m_pOldKeyboardState;

		IDirectInputDevice8*	m_pMouse;
		DIMOUSESTATE2			m_MouseState;
		DIMOUSESTATE2			m_MouseState2;
		DIMOUSESTATE2*			m_pMouseState;
		DIMOUSESTATE2*			m_pOldMouseState;

		int						m_nJoysticks;
		IDirectInputDevice8**	m_ppJoysticks;
		IDirectInputDevice8*	m_pEnumJoystick;			//current joystick being setup during callbacks
		int						m_nEnumJoystick;			//index of the current joystick being setup during callbacks
		DIJOYSTATE2*			m_pJoystates;
		DIJOYSTATE2*			m_pOldJoystates;
		JoystickInfo*			m_pJoystickInfo;

		friend BOOL CALLBACK EnumJoysticksCallbackCount(const DIDEVICEINSTANCE * pdidInstance, void * pContext);
		friend BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE * pdidInstance, void * pContext);
		friend BOOL CALLBACK EnumAxesCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef);
		friend BOOL CALLBACK EnumPovCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef);

		//temporary values used in functions
		float tmpPosition;
		DWORD dwPosition;

		Logger				*m_pLogger;
		HRESULT				m_hr;
		bool				m_bIsInitialized;
		InputConfiguration	m_config;
	};

}	//namespace Accevo

#define AEINPUT_AXES_MAX					1000.0f			//use float so division returns float
#define AEINPUT_AXES_DEADZONE				0.3f

#define AEINPUT_GAMECTRL_NULL				0x01000000
#define AEINPUT_GAMECTRL_KEYBOARD			0x02000000
#define AEINPUT_GAMECTRL_MOUSE				0x04000000
#define AEINPUT_GAMECTRL_JOYSTICK			0x08000000
#define AEINPUT_GAMECTRL_JOYSTICKN(n)		(0x08000000 | (n << 28))

#define AEINPUT_CTRLACTION_DOWN				0x00100000
#define AEINPUT_CTRLACTION_PRESS			0x00200000
#define AEINPUT_CTRLACTION_RELEASE			0x00400000
#define AEINPUT_CTRLACTION_AXES				0x00800000

#define AEINPUT_AXIS_X				0x00001000
#define AEINPUT_AXIS_Y				0x00002000
#define AEINPUT_AXIS_Z				0x00003000
#define AEINPUT_AXIS_Rx				0x00004000
#define AEINPUT_AXIS_Ry				0x00005000
#define AEINPUT_AXIS_Rz				0x00006000

#define AEINPUT_AXIS_POS			0x00040000
#define AEINPUT_AXIS_NEG			0x00080000

#define AEINPUT_JPOV_UP				0x00000100
#define AEINPUT_JPOV_DOWN			0x00000200
#define AEINPUT_JPOV_LEFT			0x00000400
#define AEINPUT_JPOV_RIGHT			0x00000800

#define AEINPUT_GAMECTRL_GETDEVICE(x)		(x & 0x0F000000)
#define AEINPUT_GAMECTRL_GETACTION(x)		(x & 0x00F00000)
#define AEINPUT_GAMECTRL_GETJOYSTICK(x)		((x & 0xF0000000) >> 28)
#define AEINPUT_GAMECTRL_GETBUTTON(x)		(x & 0x000000FF)
#define AEINPUT_GAMECTRL_GETAXIS(x)			(x & 0x0003F000)
#define AEINPUT_GAMECTRL_GETAXISPOLARITY(x)	(x & 0x000C0000)
#define AEINPUT_GAMECTRL_GETPOV(x)			(x & 0x00000F00)

#define KEYDOWN(keys, x)			( keys[x] & 0x80 )

#endif // DIRECT_INPUT_H

