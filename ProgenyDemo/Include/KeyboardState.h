#ifndef __KEYBOARDSTATE_H__
#define __KEYBOARDSTATE_H__

#include "Input.h"

#include <cstring>

class KeyboardState
{
public:
	enum KeyStates
	{
		NONE,
		KEY_ESC,
		KEY_SPACE,
		KEY_W,
		KEY_A,
		KEY_S,
		KEY_D,
		KEY_Q,
		KEY_E,
		KEY_F,
		KEY_R,
		KEY_G,
		KEY_H,
		KEY_Z,
		KEY_X,
		KEY_M,
		KEY_N,
		KEY_T,
		KEY_U,
		KEY_Y,
		KEY_TAB,
		KEY_COMMA,
		KEY_RIGHT_SHIFT,
		KEY_MINUS,
		KEY_EQUALS,
		KEY_ONE,
		KEY_TWO,
		KEY_ZERO,
		KEY_SLASH,
		KEYSTATES_COUNT // Must be last enum value
	};
	inline KeyboardState()
	{
		memset(_keyStates, 0, sizeof(_keyStates));
		memset(_deltaStates, 0, sizeof(_deltaStates));
	}
	inline bool isKeyPressed(int key) const { return _keyStates[key]; }
	inline bool getKeyDelta(int key) const { return _deltaStates[key]; }
	inline bool isKeyNewlyPressed(int key) const { return _keyStates[key] && _deltaStates[key]; }
	inline void setKeyPressed(int key, bool state) { _keyStates[key] = state; }
	inline void setKeyDelta(int key, bool state) { _deltaStates[key] = state; }
	inline bool isAnyKeyPressed() const
	{
		for (int i = 1; i < KEYSTATES_COUNT; ++i)
		{
			if (_keyStates[i])
				return true;
		}
		return false;
	}
private:
	bool _keyStates[KEYSTATES_COUNT];
	bool _deltaStates[KEYSTATES_COUNT];
};

#endif