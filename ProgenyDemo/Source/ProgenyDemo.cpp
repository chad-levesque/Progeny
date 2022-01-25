#include "ProgenyDemo.h"
#include "View.h"
#include "Input.h"

#include "libnoise/noise.h"

#if HORIZON_PLATFORM == HORIZON_PLATFORM_WIN32
#include <windows.h>
#endif


//-------------------------------------------------------------------------------------------------
// Main
//-------------------------------------------------------------------------------------------------

wchar_t* Horizon::AppNameString = L"Progeny Demo";

#if HORIZON_PLATFORM == HORIZON_PLATFORM_WIN32
int WINAPI WinMain(HINSTANCE hInstace, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
	ProgenyDemo demo;
	if (demo.Init())
	{
		demo.Run();
	}
	demo.Destroy();
	return 0;
}

//-------------------------------------------------------------------------------------------------
// Class
//-------------------------------------------------------------------------------------------------

ProgenyDemo::ProgenyDemo() :
	_pView(NULL),
	_pSystem(NULL),
	_Input()
{
	#ifdef _DEBUG
	if (!AllocConsole())
	{
		assert(false);
	}
	#endif //_DEBUG
}

ProgenyDemo::~ProgenyDemo()
{
	Destroy();
}

bool ProgenyDemo::Init()
{
	// Initialize base system
	_pSystem = Horizon::CSystem::GetInstance();
	if (!_pSystem)
		return false;

	_pView = new View();
	if (!_pView->Init())
		return false;

	_Input.init();
	_pView->BindToInput(_Input);

	return true;
}

void ProgenyDemo::Run()
{
	while (_pSystem->Update())
	{
		/*NOTE: Instead of passing a deltatime through the update loop, the current approach is to
		  grab the system singleton and access the clock though it. this can be changed
		  if it becomes problamatic.
		  i.e: if you need delta time call Horizon::CSystem::GetInstance()->GetSystemClock()->GetDeltaTime();*/
		
		//This call is required on each update for the system clock to provide correct delta times
		_pSystem->GetSystemClock()->Update();
		
		_Input.update();
		_pView->Update();

		// let the view handle other input events on it's own
		_pView->handleKeyboard(_Input.getKeyboardState());

		if (_Input.hasGamepad())
			_pView->handleGamepad(_Input.getGamepadState());

		// if(_Input.getMouseState() != Input::MouseStates::NONE_CLICKED)
		_pView->DelegateMouseInputLogic(_Input.getMouseState(), _Input.getXRelative(), _Input.getYRelative(), _Input.getZRelative());

		// MetricSingleton::GetInstance()->RenderMetrics();
	}
}

void ProgenyDemo::Destroy()
{
	if(_pView)
	{
		delete _pView;
		_pView = NULL;
	}
	if(_pSystem)
	{
		_pSystem->DestroyRenderWindow();
		_pSystem->ReleaseInstance();
		_pSystem = NULL;
	}
}