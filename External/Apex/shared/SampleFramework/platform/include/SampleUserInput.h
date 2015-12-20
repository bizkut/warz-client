#ifndef SAMPLE_USER_INPUT_H
#define SAMPLE_USER_INPUT_H
/*
 * Copyright 2008-2012 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */

#include <foundation/PxSimpleTypes.h>
#include <vector>
#include <map>
#include <foundation/PxString.h>

namespace SampleRenderer
{
	class Renderer;
}

namespace SampleFramework
{
	class InputEventListener;

	struct UserInput 
	{
		physx::PxU16	m_Id;
		char			m_IdName[256];	// this name is used for mapping (enum name)
		char			m_Name[256];	// this name is used for help
	};

	struct InputEvent
	{
		InputEvent(physx::PxU16 id, const char* name, bool display = true, bool analog = false, float sens = 1.0f)
			:m_Id(id), m_Analog(analog), m_Sensitivity(sens), m_display(display)
		{
			physx::PxStrcpy(m_Name, sizeof(m_Name), name);
		};

		InputEvent()
			: m_Analog(false), m_Sensitivity(1.0f), m_display(true)
		{
		}

		physx::PxU16	m_Id;
		char			m_Name[256];
		bool			m_Analog;		
		float			m_Sensitivity;
		bool			m_display;
	};

	struct SampleInputData
	{
		char			m_InputEventName[256];
		char			m_UserInputName[256];
	};

	struct SampleInputMapping
	{
		physx::PxU16	m_InputEventId;
		size_t			m_InputEventIndex;
		physx::PxU16	m_UserInputId;
		size_t			m_UserInputIndex;
	};

	typedef std::vector<SampleInputData>		T_SampleInputData;

	enum InputType
	{
		UNDEFINED_INPUT =		0,
		KEYBOARD_INPUT =		(1 << 0),
		GAMEPAD_INPUT =			(1 << 1),
		TOUCH_BUTTON_INPUT =	(1 << 2),
		TOUCH_PAD_INPUT =		(1 << 3),
		MOUSE_INPUT =			(1 << 4),
	};

	enum InputDataReadState
	{
		STATE_INPUT_EVENT_ID,
		STATE_USER_INPUT_ID,
		STATE_DIGITAL,
		STATE_INPUT_EVENT_NAME,
	};

	class SampleUserInput
	{
	public:
		// key codes for console and raw key info
		enum KeyCode
		{
			KEY_UNKNOWN = 0,

			KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G,
			KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N,
			KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U,
			KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,

			KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, 
			KEY_7, KEY_8, KEY_9,

			KEY_SPACE, KEY_RETURN, KEY_SHIFT, KEY_CONTROL, KEY_ESCAPE, KEY_COMMA, KEY_DOT,
			KEY_NUMPAD0, KEY_NUMPAD1, KEY_NUMPAD2, KEY_NUMPAD3, KEY_NUMPAD4, KEY_NUMPAD5, KEY_NUMPAD6, KEY_NUMPAD7, KEY_NUMPAD8, KEY_NUMPAD9,
			KEY_MULTIPLY, KEY_ADD, KEY_SEPARATOR, KEY_SUBTRACT, KEY_DECIMAL, KEY_DIVIDE,

			KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6,
			KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,

			KEY_TAB, KEY_PRIOR, KEY_NEXT,
			KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,

			NUM_KEY_CODES,
		};

		SampleUserInput();

		virtual ~SampleUserInput();

		void						registerUserInput(physx::PxU16 id,const char* idName ,const char* name);
		virtual const InputEvent*	registerInputEvent(const InputEvent& inputEvent, physx::PxU16 userInputId);
        virtual const InputEvent*   registerTouchInputEvent(const InputEvent& inputEvent, physx::PxU16 userInputId, const char* caption) 
		{ 
			PX_UNUSED(inputEvent);
			PX_UNUSED(userInputId);
			PX_UNUSED(caption);
			return NULL; 
		}
		virtual void				unregisterInputEvent(physx::PxU16 inputEventId);
		virtual void				registerInputEvent(const SampleInputMapping& mapping);

		virtual bool				keyboardSupported() const { return false; }
		virtual bool				gamepadSupported() const { return false; }
		virtual bool 				mouseSupported() const { return false; }

		virtual InputType			getInputType(const UserInput& ) const { return UNDEFINED_INPUT; }

		void						registerInputEventListerner(InputEventListener* listener) { mListener = listener; }
		InputEventListener*			getInputEventListener() const { return mListener; }

		virtual void				updateInput();
		virtual void				shutdown();

		virtual void				setRenderer(SampleRenderer::Renderer* ) {}

		virtual bool				getDigitalInputEventState(physx::PxU16 inputEventId ) const = 0;
		virtual float				getAnalogInputEventState(physx::PxU16 inputEventId ) const = 0;		

		const std::vector<size_t>*	getUserInputs(physx::PxI32 inputEventId) const;
		const std::vector<size_t>*	getInputEvents(physx::PxU16 userInputId) const;

		const std::vector<InputEvent>& getInputEventList() const { return mInputEvents; }
		const std::vector<UserInput>& getUserInputList() const { return mUserInputs; }
		const std::map<physx::PxU16, std::vector<size_t> >& getInputEventUserInputMap() const { return mInputEventUserInputMap; }

		physx::PxU16				getUserInputKeys(physx::PxU16 inputEventId, const char* names[], physx::PxU16 maxNames, physx::PxU32 inputTypeMask) const;

		physx::PxI32				translateUserInputNameToId(const char* name, size_t& index) const;
		physx::PxI32				translateInputEventNameToId(const char* name, size_t& index) const;		

		const InputEvent*			getInputEventSlow(physx::PxU16 inputEventId) const;		

	protected:
		virtual void				processGamepads();

		std::vector<UserInput>	mUserInputs;
		std::vector<InputEvent>	mInputEvents;

	private:		
		
		InputEventListener*	mListener;
		std::map<physx::PxU16, std::vector<size_t> >	mInputEventUserInputMap;
		std::map<physx::PxU16, std::vector<size_t> >	mUserInputInputEventMap;
	};

	class InputEventListener
	{
	public:
		InputEventListener() {}

		virtual ~InputEventListener() {}

		// special case for text console
		virtual void onKeyDownEx(SampleUserInput::KeyCode, physx::PxU32) {}

		virtual void onPointerInputEvent(const InputEvent&, physx::PxU32, physx::PxU32, physx::PxReal, physx::PxReal) {}
		virtual void onAnalogInputEvent(const InputEvent& , float val) = 0;
		virtual bool onDigitalInputEvent(const InputEvent& , bool val) = 0;		
	};
}

#endif
