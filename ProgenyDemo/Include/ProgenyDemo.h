#ifndef __PROGENY_DEMO_H__
#define __PROGENY_DEMO_H__

#include "Input.h"
#include <Input/TextDevice.h>
#include <System/System.h>
#include <Graphics/Graphics.h>

class View;

namespace Horizon{
	Horizon::CSystem;
}

class ProgenyDemo : public Horizon::CTextDevice //<-- do we know what this does?, RL
{
public:

	ProgenyDemo();
	virtual ~ProgenyDemo();

	bool Init();
	void Run();
	void Destroy();

private:
	View* _pView;
	Horizon::CSystem* _pSystem;
	Input _Input;
};

#endif