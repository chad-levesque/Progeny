#include "Input.h"
#include "Input/DirectX/DXKeyboard.h"
#include "Input/DirectX/DXGamepad.h"

#include <Input/Input.h>
#include <Input/GameController.h>
#include <Input/GamePointer.h>
#include <System/System.h>
#include <Interface/Interface.h>
#include <Types.h>

#include <iostream>


namespace Horizon{
	class IPointerDevice;
}

Input::Input() :
	_pKeyboard(NULL),
	_pMouse(NULL),
	_pGamepad(NULL),
	_pTextDevice(NULL),
	_mouseDeltaPos(),
	_pGamepadController(NULL)
{
}

Input::~Input()
{
}

bool Input::init()
{
	Horizon::IInput* input = Horizon::IInput::CreateInstance(Horizon::EID_DIRECTX);
	if (!input)
		return false;
	if (!input->Init())
		return false;

	Horizon::CSystem* system = Horizon::CSystem::GetInstance();
	// Assumes a render window exists
	if(!input->CreateDevices(system))
		return false;

	// <-- Do we care about text devices? RL
	_pTextDevice = input->GetTextDevice();
	if(!_pTextDevice)
		return false;

	//Device 0 is the keyboard... This is used everywhere, but promised nowhere...
	//It's just an artifact of how IInput::CreateInstance(EID_DIRECTX); pushes them on a stack
	_pKeyboard = input->GetDevice(0);

	if(!_pKeyboard)
		return false;

	//Device 1 is the mouse... see above
	_pMouse = input->GetDevice(1);
	if(!_pMouse)
		return false;

	// Gamepad, if connected:
	Horizon::IInputDevice* gamepad = input->GetDevice(2);
	if (gamepad != NULL)
	{
		_pGamepad = (Horizon::CDXGamepad*)gamepad;
	}


	return true;
}

void Input::bindToInterface(Horizon::CInterface* pInterface)
{
	// Game controllers are generic containers for input devices,
	// The "Interface" updates the controller state by calling supplied CGameControllers
	_pGameController = new Horizon::CGameController();
	//Wrap the keyboard device in a CGameController interface
	_pGameController->AddInputDevice(_pKeyboard);
	_pGameController->CreateActionMap(KeyboardState::KEYSTATES_COUNT);
	// Add the controller to the "Interface" so that it can update it,
	// Alternatively you could update it yourself, but it makes more sense to
	// let interface cascade the update using Interface->Update() in your main loop
	//pInterface->AddController(_pGameController);

	//set up action keys
	_pGameController->AddActionKey(KeyboardState::KEY_ESC, 0, DIK_ESCAPE);
	_pGameController->AddActionKey(KeyboardState::KEY_SPACE, 0, DIK_SPACE);
	_pGameController->AddActionKey(KeyboardState::KEY_W, 0, DIK_W);
	_pGameController->AddActionKey(KeyboardState::KEY_A, 0, DIK_A);
	_pGameController->AddActionKey(KeyboardState::KEY_S, 0, DIK_S);
	_pGameController->AddActionKey(KeyboardState::KEY_D, 0, DIK_D);
	_pGameController->AddActionKey(KeyboardState::KEY_Q, 0, DIK_Q);
	_pGameController->AddActionKey(KeyboardState::KEY_E, 0, DIK_E);
	_pGameController->AddActionKey(KeyboardState::KEY_F, 0, DIK_F);
	_pGameController->AddActionKey(KeyboardState::KEY_R, 0, DIK_R);
	_pGameController->AddActionKey(KeyboardState::KEY_G, 0, DIK_G);
	_pGameController->AddActionKey(KeyboardState::KEY_H, 0, DIK_H);
	_pGameController->AddActionKey(KeyboardState::KEY_Z, 0, DIK_Z);
	_pGameController->AddActionKey(KeyboardState::KEY_X, 0, DIK_X);
	_pGameController->AddActionKey(KeyboardState::KEY_M, 0, DIK_M);
	_pGameController->AddActionKey(KeyboardState::KEY_N, 0, DIK_N);
	_pGameController->AddActionKey(KeyboardState::KEY_T, 0, DIK_T);
	_pGameController->AddActionKey(KeyboardState::KEY_U, 0, DIK_U);
	_pGameController->AddActionKey(KeyboardState::KEY_Y, 0, DIK_Y);
	_pGameController->AddActionKey(KeyboardState::KEY_TAB, 0, DIK_TAB);
	_pGameController->AddActionKey(KeyboardState::KEY_COMMA, 0, DIK_COMMA);
	_pGameController->AddActionKey(KeyboardState::KEY_RIGHT_SHIFT, 0, DIK_RSHIFT);
	_pGameController->AddActionKey(KeyboardState::KEY_MINUS, 0, DIK_MINUS);
	_pGameController->AddActionKey(KeyboardState::KEY_EQUALS, 0, DIK_EQUALS);
	_pGameController->AddActionKey(KeyboardState::KEY_ONE, 0, DIK_1);
	_pGameController->AddActionKey(KeyboardState::KEY_TWO, 0, DIK_2);
	_pGameController->AddActionKey(KeyboardState::KEY_ZERO, 0, DIK_0);
	_pGameController->AddActionKey(KeyboardState::KEY_SLASH, 0, DIK_SLASH);
	
	if (_pGamepad)
	{
		_pGamepadController = new Horizon::CGameController();
		_pGamepadController->AddInputDevice(_pGamepad);
		_pGamepadController->CreateActionMap(GamepadState::BUTTONS_COUNT);
		_pGamepadController->AddActionKey(GamepadState::BUTTON_0, 0, 0);
		_pGamepadController->AddActionKey(GamepadState::BUTTON_1, 0, 1);
		_pGamepadController->AddActionKey(GamepadState::BUTTON_2, 0, 2);
		_pGamepadController->AddActionKey(GamepadState::BUTTON_3, 0, 3);
		_pGamepadController->AddActionKey(GamepadState::BUTTON_4, 0, 4);
		_pGamepadController->AddActionKey(GamepadState::BUTTON_5, 0, 5);
		_pGamepadController->AddActionKey(GamepadState::BUTTON_6, 0, 6);
		_pGamepadController->AddActionKey(GamepadState::BUTTON_7, 0, 7);
		_pGamepadController->AddActionKey(GamepadState::BUTTON_8, 0, 8);
		_pGamepadController->AddActionKey(GamepadState::BUTTON_9, 0, 9);
		_pGamepadController->AddActionKey(GamepadState::LEFTSTICK_PX, 0, KEY_GAMEPAD_PXAXIS);
		_pGamepadController->AddActionKey(GamepadState::LEFTSTICK_NX, 0, KEY_GAMEPAD_NXAXIS);
		_pGamepadController->AddActionKey(GamepadState::LEFTSTICK_PY, 0, KEY_GAMEPAD_PYAXIS);
		_pGamepadController->AddActionKey(GamepadState::LEFTSTICK_NY, 0, KEY_GAMEPAD_NYAXIS);
	}
	//Init Mouse Input
	_pGamePointer = new Horizon::CGamePointer();
	_pGamePointer->SetPointerDevice(static_cast<Horizon::IPointerDevice*>(_pMouse));
	_pGamePointer->AddInputDevice(_pMouse);
	_pGamePointer->CreateActionMap(Horizon::INTPTR_ACTIONS_COUNT);
	_pGamePointer->SetActionKey(Horizon::INTPTR_LEFTCLICK, 0, 0);
	_pGamePointer->SetActionKey(Horizon::INTPTR_RIGHTCLICK, 0, 1);
	_pGamePointer->SetActionKey(Horizon::INTPTR_MIDDLECLICK, 0, 2);
	pInterface->AddPointer(_pGamePointer);
}

const KeyboardState&
Input::getKeyboardState()
{
	return _keyboardState;
}

const GamepadState&
Input::getGamepadState()
{
	return _gamepadState;
}

Input::MouseStates Input::getMouseState()
{
	if( _pGamePointer->GetThisActionState(Horizon::INTPTR_LEFTCLICK))
	{
		return LEFT_CLICK;
	}
	if( _pGamePointer->GetThisActionState(Horizon::INTPTR_RIGHTCLICK))
	{
		return RIGHT_CLICK;
	}
	if( _pGamePointer->GetThisActionState(Horizon::INTPTR_MIDDLECLICK))
	{
		return MIDDLE_CLICK;
	}
	return NONE_CLICKED;
}

float Input::getXRelative()
{
	return _mouseDeltaPos.x;
}

float Input::getYRelative()
{
	return _mouseDeltaPos.y;
}

float Input::getZRelative()
{
	return _mouseDeltaPos.z;
}

void Input::update()
{
	Horizon::IInput::GetInstance()->UpdateDevices();

	Horizon::CSystemClock* clock = Horizon::CSystem::GetInstance()->GetSystemClock();
	_pGameController->Update(clock->GetLocalTime(), clock->GetDeltaTime());
	if (_pGamepadController) _pGamepadController->Update(clock->GetLocalTime(), clock->GetDeltaTime());
	// Update Mouse Deltas
	_pGamePointer->GetDeltaPosition(_mouseDeltaPos);

	// Update keyboard state
	for (int i = 0; i < KeyboardState::KEYSTATES_COUNT; ++i)
	{
		_keyboardState.setKeyPressed( i, _pGameController->GetThisActionState(i) > 0 );
		_keyboardState.setKeyDelta( i, _pGameController->GetDeltaActionState(i) > 0 );
	}
	if (_pGamepadController)
	{
		// Update gamepad state
		for (int i = 0; i < GamepadState::LEFTSTICK_PX; ++i)
		{
			_gamepadState.setButtonPressed( i, _pGamepadController->GetThisActionState(i) > 0 );
			_gamepadState.setButtonDelta( i, _pGamepadController->GetDeltaActionState(i) > 0 );
		}
		// Update left stick:
		float px = _pGamepad->GetState(KEY_GAMEPAD_PXAXIS);
		float nx = _pGamepad->GetState(KEY_GAMEPAD_NXAXIS);
		float py = _pGamepad->GetState(KEY_GAMEPAD_PYAXIS);
		float ny = _pGamepad->GetState(KEY_GAMEPAD_NYAXIS);
		if (px > 0.0f)
			_gamepadState.setHorizontal( px );
		else
			_gamepadState.setHorizontal( -nx );
		if (py > 0.0f)
			_gamepadState.setVertical( py );
		else
			_gamepadState.setVertical( -ny );
		// Update right stick:
		float rpx = _pGamepad->GetState(KEY_GAMEPAD_PRXAXIS);
		float rnx = _pGamepad->GetState(KEY_GAMEPAD_NRXAXIS);
		float rpy = _pGamepad->GetState(KEY_GAMEPAD_PRYAXIS);
		float rny = _pGamepad->GetState(KEY_GAMEPAD_NRYAXIS);
		if (rpx > 0.0f)
			_gamepadState.setRightHorizontal( rpx );
		else
			_gamepadState.setRightHorizontal( -rnx );
		if (rpy > 0.0f)
			_gamepadState.setRightVertical( rpy );
		else
			_gamepadState.setRightVertical( -rny );
		// Update triggers:
		float lt = _pGamepad->GetState(KEY_GAMEPAD_PZAXIS);
		float rt = _pGamepad->GetState(KEY_GAMEPAD_NZAXIS);
		if (lt > 0.0f)
			_gamepadState.setPOV( lt );
		else
			_gamepadState.setPOV( -rt );
	}

}
