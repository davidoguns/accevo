//David Oguns
//March 12, 2012
//Input.h

#pragma once

#include "Handle.h"
#include "aetypes.h"

namespace Accevo
{
	enum InputController : AUINT8 {
		NONE		= 0,		//used for unbound controls
		KEYBOARD	= 1,
		MOUSE		= 2,
		JOYSTICK	= 3,
	}; //2 bits

	enum InputType : AUINT8 {
		BUTTON		= 0,	//interested in a button on said device
		AXIS		= 1,	//interested in an axis on said device
		POV			= 2,	//interested in a POV hat on said device
		SLIDER		= 3,	//interested in a slider on said device
	};	//looks like 2 bits

	//define which axis to look at if axis is picked
	enum InputAxis : AUINT8 {
		X			= 0,
		Y			= 1,
		Z			= 2,
		RX			= 3,
		RY			= 4,
		RZ			= 5,
		POVX		= 6,
		POVY		= 7,
	};	//looks like 4 bits
	
	//32 bits for data - which button, key, axes direction
	typedef AUINT32 InputData;
	//button -- button number or constant
	//axis -- reinterpret cast as float percent (0.0f to 1.0f) to look for excess


	enum InputAction : AUINT8 {
		PRESS		= 0,	//control state was not valid previous frame, but is this frame
		RELEASE		= 1,	//control state was valid previous frame, but is not this frame
		DOWN		= 2,	//control state is valid this frame
		UP			= 3,	//control state is not no valid this frame
	};	//2 bits

	/* Represents a control state that can be checked for validity.  A control
	 * state is always considered true or false with respect to this frame, or
	 * this frame and the last.
	 *
	 * Control states can also be used to register event handlers.
	 */
	struct ControlState {
		InputData				ex_data;		//extra data like a threshold to pass before being considered valid
		Handle					device;			//handle to control device
		union {
			InputAxis			axis			: 4;
			AUINT8				povSliderIndex	: 4;
		};
		InputAction				action			: 2;
		InputType				type			: 2;
	};
}
