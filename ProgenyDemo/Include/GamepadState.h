#ifndef __GAMEPAD_H__
#define __GAMEPAD_H__

#include "Input.h"
#include "DemoConstants.h"

#include <cstring>

class GamepadState
{
public:
	enum ButtonStates
	{
		BUTTON_NONE,
		BUTTON_0,
		BUTTON_1,
		BUTTON_2,
		BUTTON_3,
		BUTTON_4,
		BUTTON_5,
		BUTTON_6,
		BUTTON_7,
		BUTTON_8,
		BUTTON_9,
		LEFTSTICK_PX,
		LEFTSTICK_NX,
		LEFTSTICK_PY,
		LEFTSTICK_NY,
		BUTTONS_COUNT
		// Must be last enum value
	};

	inline GamepadState()
	{
		memset(_buttonStates, 0, sizeof(_buttonStates));
		memset(_deltaStates, 0, sizeof(_deltaStates));
	}
	inline bool isButtonPressed(int Button) const { return _buttonStates[Button]; }
	inline bool getButtonDelta(int Button) const { return _deltaStates[Button]; }
	inline bool isButtonNewlyPressed(int Button) const { return _buttonStates[Button] && _deltaStates[Button]; }
	inline void setButtonPressed(int Button, bool state) { _buttonStates[Button] = state; }
	inline void setButtonDelta(int Button, bool state) { _deltaStates[Button] = state; }
	inline bool isAnyButtonPressed() const
	{
		for (int i = 1; i < BUTTONS_COUNT; ++i)
		{
			if (_buttonStates[i])
				return true;
		}
		if (abs(_verticalAxis) > GAMEPAD_DEADZONE_CUTOFFF || abs(_horizontalAxis) > GAMEPAD_DEADZONE_CUTOFFF || abs(_povAxis) > GAMEPAD_DEADZONE_CUTOFFF) return true;
		return false;
	}
	inline void setVertical(float val) { _verticalAxis = val; }
	inline float getVertical() const { return _verticalAxis; }
	inline void setHorizontal(float val) { _horizontalAxis = val; }
	inline float getHorizontal() const { return _horizontalAxis; }
	inline void setRightVertical(float val) { _rightVerticalAxis = val; }
	inline float getRightVertical() const { return _rightVerticalAxis; }
	inline void setRightHorizontal(float val) { _rightHorizontalAxis = val; }
	inline float getRightHorizontal() const { return _rightHorizontalAxis; }
	inline void setPOV(float val) { _povAxis = val; }
	inline float getPOV() const { return _povAxis; }
private:
	bool _buttonStates[BUTTONS_COUNT];
	bool _deltaStates[BUTTONS_COUNT];
	float _verticalAxis;
	float _horizontalAxis;
	float _rightVerticalAxis;
	float _rightHorizontalAxis;
	float _povAxis;
};

#endif