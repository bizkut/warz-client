#ifndef WINDOWS_SAMPLE_USER_INPUT_H
#define WINDOWS_SAMPLE_USER_INPUT_H
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

#include <SampleUserInput.h>
#include <windows/WindowsSampleUserInputIds.h>
#include <WTypes.h>
#include <XInput.h>

namespace SampleFramework
{
	class WindowsSampleUserInput: public SampleUserInput
	{
	public:
		enum KeyEventFlag
		{
			KEY_EVENT_NONE = 0,
			KEY_EVENT_UP,
			KEY_EVENT_DOWN,
		};

		enum MouseButtons
		{
			LEFT_MOUSE_BUTTON,
			RIGHT_MOUSE_BUTTON,
			CENTER_MOUSE_BUTTON,
		};

		struct KeyEvent
		{
			WPARAM m_Param;
			USHORT m_ScanCode;
			KeyEventFlag m_Flags;
		};

		struct InputState
		{
			InputState()
			{
			};

			InputState(bool val)
			{
				m_State = val;
			};

			InputState(float val)
			{
				m_Value = val;
			};

			union
			{
				bool	m_State;
				float	m_Value;
			};
		};

		WindowsSampleUserInput();
		~WindowsSampleUserInput();

		bool keyCodeToASCII( WindowsSampleUserInputIds code, char& c )
		{
			if( code >= KEY_A && code <= KEY_Z )
			{
				c = (char)code + 'A' - 1;
			}
			else if( code >= KEY_0 && code <= KEY_9 )
			{
				c = (char)code + '0' - 1;
			}
			else if( code == KEY_SPACE )
			{
				c = ' ';
			}
			else
			{
				return false;
			}
			return true;
		}

		void doOnMouseMove(physx::PxU32 x, physx::PxU32 y, physx::PxReal dx, physx::PxReal dy, WindowsSampleUserInputIds button);
		void doOnMouseButton(physx::PxU32 x, physx::PxU32 y, MouseButtons button, bool down);
		void onKeyDownEx(WPARAM wParam);
		void onKeyDown(WPARAM wParam, LPARAM lParam);
		void onKeyUp(WPARAM wParam, LPARAM lParam);
		void onKeyEvent(const KeyEvent& keyEvent);

		void onGamepadButton(physx::PxU32 buttonIndex, bool buttonDown);
		void onGamepadAnalogButton(physx::PxU32 buttonIndex,const BYTE oldValue,const BYTE newValue);
		void onGamepadAxis(physx::PxU32 axis, physx::PxReal val);

		virtual void updateInput();
		virtual void processGamepads();

		virtual void shutdown();

		virtual bool keyboardSupported() const { return true; }
		virtual bool gamepadSupported() const;
		virtual bool mouseSupported() const { return true; }
		virtual InputType getInputType(const UserInput&) const;

		virtual bool	getDigitalInputEventState(physx::PxU16 inputEventId ) const;
		virtual float	getAnalogInputEventState(physx::PxU16 inputEventId ) const;

	protected:
		WindowsSampleUserInputIds	getKeyCode(WPARAM wParam) const;
		SampleUserInput::KeyCode	getSampleUserInputKeyCode(WPARAM wParam) const;

		const UserInput*			getUserInputFromId(WindowsSampleUserInputIds id) const;

		bool						hasXInput() const { return mpXInputGetState && mpXInputGetCapabilities; }

		std::map<physx::PxU16, physx::PxU16>	mScanCodesMap;
		std::map<physx::PxU16,float>	mAnalogStates;
		std::map<physx::PxU16,bool>		mDigitalStates;

		bool							mGamePadConnected;
		physx::PxU32					mConnectedPad;

		typedef DWORD (WINAPI *LPXINPUTGETSTATE)(DWORD, XINPUT_STATE*);
		typedef DWORD (WINAPI *LPXINPUTGETCAPABILITIES)(DWORD,DWORD,XINPUT_CAPABILITIES*);

		HMODULE							mXInputLibrary;
		LPXINPUTGETSTATE				mpXInputGetState;
		LPXINPUTGETCAPABILITIES			mpXInputGetCapabilities;
	};
}

#endif
