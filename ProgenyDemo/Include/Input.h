#ifndef __INPUT_H__
#define __INPUT_H__

#include "Math/MathTypes.h"
#include "KeyboardState.h"
#include "GamepadState.h"

class DeviceInterfaceBinder;
namespace Horizon {
	class IInputDevice;
	class CTextDevice;
	class CInterface;
	class CGameController;
	class CGamePointer;
	class CDXGamepad;
}

/************************************************************************/
/* Encapsulates Horizon's input functionality                           */
/************************************************************************/
class Input
{
public:

	enum MouseStates
	{
		NONE_CLICKED,
		LEFT_CLICK,
		RIGHT_CLICK,
		MIDDLE_CLICK,
		MOUSESTATES_COUNT // Must be last enum value
	};

	Input();
	virtual ~Input();
	bool init();
	void update();

	const KeyboardState& getKeyboardState();
	const GamepadState& getGamepadState();
	MouseStates getMouseState();

	float getXRelative();
	float getYRelative();
	float getZRelative();

	void bindToInterface(Horizon::CInterface* pInterface);
	bool hasGamepad() { return (_pGamepad != NULL); }
private:
	Horizon::IInputDevice* _pKeyboard;
	Horizon::IInputDevice* _pMouse;
	Horizon::CDXGamepad* _pGamepad;
	Horizon::CTextDevice* _pTextDevice; //<-- not sure what this does yet, RL
	Horizon::CGameController* _pGameController;
	Horizon::CGameController* _pGamepadController;
	Horizon::CGamePointer* _pGamePointer;
	Horizon::VECTOR3 _mouseDeltaPos;
	KeyboardState _keyboardState;
	GamepadState _gamepadState;
};

#endif