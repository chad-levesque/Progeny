#include "View.h"
#include "SkyBox.h"
#include "PgROAMPlanet.h"
#include "HorizonPgRenderer.h"
#include "FreelookCameraSystem.h"
#include "OrbitalCameraSystem.h"
#include "PgVertex.h"
#include "DemoConstants.h"

#include <MathTools.h>
#include <PgConstants.h>
#include <PgColorScheme.h>

#include <Graphics/Graphics.h>
#include <Graphics/GraphicsTypes.h>
#include <Graphics/Light.h>
#include <Graphics/GraphicsTypes.h>
#include <System/System.h>
#include <Toolbox/Event.h>
#include <Interface/Interface.h>
#include <Interface/Frame.h>
#include <Interface/InterfaceFactory.h>
#include <Interface/Control/Button.h>
#include <Interface/Control/Slider.h>
#include <Interface/Control/ListBox.h>
#include <Interface/Window/Dialog.h>
#include <Types.h>

#include <conio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
#include <float.h>

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
	FLOAT x, y, z, rhw; // The transformed position for the vertex
	DWORD color;        // The vertex color
};

float View::INTERFACE_SCALE = 12.0f;

// vertex structure to give progeny vertex data to gpu
extern const Horizon::VERTEXDESC VERTEXDESC_PROGENY;
const Horizon::VERTEXDESC VERTEXDESC_PROGENY =
{
	4,
	{
		{0, 0, Horizon::TYPE_FLOAT3, Horizon::USAGE_POSITION, 0},
		{0, 12, Horizon::TYPE_FLOAT3, Horizon::USAGE_NORMAL, 0},
		{0, 24, Horizon::TYPE_FLOAT2, Horizon::USAGE_TEXCOORD, 0},
		{0, 32, Horizon::TYPE_COLOR, Horizon::USAGE_COLOR, 0}
	}
};

View::View() : 
	_pGraphics(NULL),
	_pInterface(NULL),
	_pCameraSys(NULL),
	_pSkybox(NULL),
	_pPlanet(NULL),
	_pSunlight(NULL),
	_pPgRenderer(NULL),
	_bEnableROAM(true),
	_bEnableFog(false),
	_bMainMenuEnabled(false),
	_bCustomizeWindowEnabled(false),
	_tolerance(DEFAULT_TOLERANCE),
	_bWireframe(false),
	_bOverlayEnabled(false),
	_bNotificationEnabled(false),
	_notificationTime(0),
	_frameCount(0),
	_lightRadius(ORBITAL_RADIUS),
	_lightTheta(0),
	_orbitSpeed(SUN_SPEED),
	_sunMovementEnabled(true),
	_bScreenSaverMode(false),
	_inactivityTimer(0.0f),
	_planetRecreationTimer(0.0f),
	_bAttributesUpdated(true)
{
}

bool View::Init()
{
	Horizon::CSystem* pSystem = Horizon::CSystem::GetInstance();

	Horizon::CRenderWindow* pWindow = pSystem->CreateRenderWindow(SCREEN_WIDTH, SCREEN_HEIGHT, FULLSCREEN);
	if (!pWindow)
		return false;

	_pGraphics = Horizon::IGraphics::CreateInstance(Horizon::EGD_DIRECT3D9);

	if (!_pGraphics)
		return false;
	if (!_pGraphics->Init())
		return false;

	if (!_pGraphics->CreateDisplay(pSystem, 0, SCREEN_WIDTH, SCREEN_HEIGHT, DMFORMAT_X8R8G8B8, 0, FULLSCREEN, Horizon::ANTIALIAS_2X))
		return false;

	Horizon::IGraphicsFactory* gfactory = Horizon::IGraphics::GetFactoryInstance();
	Horizon::CFont* font = gfactory->CreateFontFromXFNTFile("Data/Fonts/lithos.xfnt");
	Horizon::ITexture* fonttex = _pGraphics->CreateTextureFromFile("Data/Fonts/lithos.png");
	font->SetTexture(fonttex);
	

	_pInterface = new Horizon::CInterface(Horizon::CSystem::GetInstance()->GetSystemClock(), 1);
	_pInterface->SetScale(INTERFACE_SCALE);
	if (FULLSCREEN)
		_pInterface->SetDrawPointers(true);
	else
		_pInterface->SetDrawPointers(false);

	Horizon::CInterfaceFactory* factory = _pInterface->GetFactory();
	factory->SetStyle(INTERFACE_STYLE_TEXTURED);
	factory->SetFont(font);

	//create main screen
	_pMainScreen = factory->CreateScreen(_pInterface, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, 0x0100);
	_pMainScreen->Activate();

	// Create view overlay
	createViewOverlay(factory);
	createMainMenu(factory);
	createCustomizeMenu(factory);

	// create default planet
	_attributes._seed = PLANET_SEED;
	_attributes._radius = PLANET_RADIUS;
	_attributes._oceanHeight = OCEAN_HEIGHT;
	_attributes._mountainCount = MOUNTAIN_COUNT;
	_attributes._continentFreq = CONTINENT_FREQUENCY;
	_attributes._noiseAmplitude = NOISE_AMPLITUDE;
	_attributes._useCraters = USE_CRATERS;
	updateConfigurationFields();

	// Create the camera system (XXX: cannot be created before the graphics object atm)
	// _pCameraSys = new OrbitalCameraSystem(_attributes._radius);

	Progeny::PgColorScheme::Earth( _colorScheme );
	_colorScheme.fRangeHeights[0] = OCEAN_HEIGHT;
	_colorScheme.fRangeHeights[1] = SNOW_HEIGHT;
	
	//create skybox
	_pSkybox = new SkyBox( SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE );
	
	initializePlanet();

	// create progeny renderer
	_pPgRenderer = new HorizonPgRenderer(_pGraphics);

	setupLighting();
	setupFog();
	initializeCamera();

	return true;
}

View::~View()
{
	Destroy();
}

void View::Destroy()
{
	if (_pGraphics)
	{
		_pGraphics->ReleaseInstance();
		_pGraphics = NULL;
	}

	if(_pCameraSys)
	{
		delete _pCameraSys;
	}
}

void View::Update()
{
	//FunctionTimer timer("View::Update()");

	static long timestamp = clock();
	// First frame will have zero delta, but this is just an example.
	float deltaTime = (float)(clock() - timestamp) / CLOCKS_PER_SEC;
	if (!_bScreenSaverMode)
	{
		if (!_bMainMenuEnabled && !_bCustomizeWindowEnabled)
			_inactivityTimer += deltaTime;
		if (_inactivityTimer > SCREENSAVER_TIME)
		{
			setScreenSaverMode(true);
			if(_bCustomizeWindowEnabled) toggleCreationMenu();
			if(_bMainMenuEnabled) toggleMenu();
		}
	}
	else
	{
		_planetRecreationTimer += deltaTime;
		if (_planetRecreationTimer > SCREENSAVER_REGENERATE_TIME)
		{
			generateRandomPlanet();
			_planetRecreationTimer = 0.0f;
		}
	}

	if (!_bMainMenuEnabled && !_bCustomizeWindowEnabled)
		updateLight(deltaTime);
	if(_pInterface)
		_pInterface->Update();

	if (_bScreenSaverMode == true)
	{
		positionCameraBasedOnLightSource();
	}
	else
	{
		_pCameraSys->update( );
	}
	float altitude = _pPlanet->getAltitude( _pCameraSys->getCameraPosition() );
	//std::stringstream stream;
	//stream << "Camera altitude = " << altitude << std::endl;
	//if(std::string("").compare(stream.str()) != 0)
	//	//_cprintf(stream.str().c_str());
	if ( altitude < (NEAR_CLIPPING_DISTANCE + 1.0f) )
	{
		_pCameraSys->setDistanceToPlanetSurface(altitude);
		_pCameraSys->collide();
	}
	else
	{
		_pCameraSys->setDistanceToPlanetSurface(altitude);
	}

	Progeny::PgVector3 cameraPos = _pCameraSys->getCameraPosition();
	cameraPos.normalize();
	Progeny::PgVector3 sunPos = Progeny::PgVector3( _pSunlight->Location.x, _pSunlight->Location.y, _pSunlight->Location.z);
	sunPos.normalize();

	float cameraSunAngle = Progeny::RAD_TO_DEG*acosf(cameraPos.dot(sunPos));
	while (cameraSunAngle > 180.0f) cameraSunAngle -= 180.0f;
	updateAtmosphericOpacity(altitude, cameraSunAngle);
	
	bool lockTolerance = false;

	if ( _bEnableROAM)
		lockTolerance = _pPlanet->update( _pCameraSys, _tolerance );

	_frameCount = 0;

	// Update Overlays
	std::stringstream s;
	s << _pPlanet->getTriangleCount();
	_pTriangleCount->SetText(s.str().c_str());
	if (_bCustomizeWindowEnabled) updateConfigurationFields();

	// Vertex Count
	int vertCount = Progeny::PgVertex::Array.getVertexCount();
	s.str("");
	s.clear();
	s << vertCount;
	_pVertexCount->SetText(s.str().c_str());

	timestamp = clock();
	Render();

	//Dynamically adjust tolerance to achieve desired triangle count
	if (_bEnableROAM && !lockTolerance)
	{
		int triCount = _pPlanet->getTriangleCount();
		if (triCount < 12000) _tolerance *= 0.95f;
		if (_tolerance < EPSILON) _tolerance = EPSILON;
		if (triCount > 16000) _tolerance *= 1.05f;
	}

	if(_bNotificationEnabled)
	{
		// If enough time has elapsed, remove notification
		float delt = (float)(timestamp - _notificationTime ) / CLOCKS_PER_SEC;
		if( delt > 3.0f )
		{
			_bNotificationEnabled = false;
			_popupNotification->Deactivate();
		}
	}
}

void View::Render() 
{
	//FunctionTimer timer("View::Render()");

	_pGraphics->BeginScene();
	_pGraphics->SetAmbientLight(NULL);
	_pGraphics->SetLight(0, _pSunlight );
	_pGraphics->SetLightEnable(0, true);
	_pGraphics->SetCamera(_pCameraSys->getCamera());
	_pGraphics->SetCullMode(Horizon::CULLMODE_NONE);
	_pGraphics->SetFillMode(Horizon::FILLMODE_SOLID);


	_pGraphics->SetLightingEnable(false);
	if(_pSkybox)
	{
		_pSkybox->render(_pGraphics, _pCameraSys, _pSunlight->Location);
	}
	_pGraphics->SetFogEnable(_bEnableFog);
	_pGraphics->SetLightingEnable(true);

	_pGraphics->SetCullMode(Horizon::CULLMODE_CCW);
	if (_bWireframe) {
		_pGraphics->SetFillMode(Horizon::FILLMODE_WIREFRAME);
	} else {
		_pGraphics->SetFillMode(Horizon::FILLMODE_SOLID);
	}

	_pGraphics->SetAlphaBlendEnable(true);
	_pGraphics->SetTexture(0, NULL);
	_pGraphics->SetDepthEnable(true);
	_pGraphics->SetDepthWriteEnable(true);
	_pPlanet->render(_pPgRenderer);

	if(_pInterface )
	{
		_pGraphics->SetFillMode(Horizon::FILLMODE_SOLID);
		_pInterface->Render(_pGraphics);
	}
	_pGraphics->EndScene();
}

void View::BindToInput(Input& pInput)
{
	pInput.bindToInterface(_pInterface);
}

void View::handleKeyboard( const KeyboardState& keyboardState )
{
	if (keyboardState.isAnyKeyPressed() && !keyboardState.isKeyPressed(KeyboardState::KEY_SLASH))
	{
		_inactivityTimer = 0.0f;
		if (_bScreenSaverMode) setScreenSaverMode(false);
	}	
	// Need to catch this one outside the if below
	if (keyboardState.isKeyNewlyPressed(KeyboardState::KEY_ESC))
	{
		if (_bCustomizeWindowEnabled) toggleCreationMenu();
		toggleMenu();
	}
	if(!_bCustomizeWindowEnabled)
		{
		if (keyboardState.isKeyPressed(KeyboardState::KEY_W))
			_pCameraSys->moveForward();
		if (keyboardState.isKeyPressed(KeyboardState::KEY_A))
			_pCameraSys->moveLeft();
		if (keyboardState.isKeyPressed(KeyboardState::KEY_S))
			_pCameraSys->moveBackwards();
		if (keyboardState.isKeyPressed(KeyboardState::KEY_D))
			_pCameraSys->moveRight();
		if (keyboardState.isKeyPressed(KeyboardState::KEY_Q))
			_pCameraSys->moveUp();
		if (keyboardState.isKeyPressed(KeyboardState::KEY_E))
			_pCameraSys->moveDown();
		if (keyboardState.isKeyNewlyPressed(KeyboardState::KEY_R))
		{
			toggleWireframe();
			std::stringstream stream;
			stream << "Wireframe View " << (_bWireframe ? "Enabled" : "Disabled") << std::endl;
			triggerNotification(stream.str().c_str());
		}
		if (keyboardState.isKeyPressed(KeyboardState::KEY_N))
		{
			_orbitSpeed = _orbitSpeed*0.95f;
			std::stringstream stream;
			stream << "Decrease Orbital Speed: " << _orbitSpeed << std::endl;
			triggerNotification(stream.str().c_str());
		}
		if (keyboardState.isKeyPressed(KeyboardState::KEY_M))
		{
			_orbitSpeed = _orbitSpeed*1.05f;
			std::stringstream stream;
			stream << "Increase Orbital Speed: " << _orbitSpeed << std::endl;
			triggerNotification(stream.str().c_str());
		}
		if (keyboardState.isKeyNewlyPressed(KeyboardState::KEY_COMMA))
		{
			_sunMovementEnabled = !_sunMovementEnabled;
			std::stringstream stream;
			stream << "Orbital Movement " << (_sunMovementEnabled ? "Enabled" : "Disabled") << std::endl;
			triggerNotification(stream.str().c_str());
		}
		if (keyboardState.isKeyPressed(KeyboardState::KEY_Z))
		{
			_tolerance = _tolerance*0.9f;
			std::stringstream stream;
			stream << "Decrease Tolerance: " << _tolerance << std::endl;
			triggerNotification(stream.str().c_str());
		}
		if (keyboardState.isKeyPressed(KeyboardState::KEY_X))
		{
			_tolerance = _tolerance*1.1f;
			std::stringstream stream;
			stream << "Increase Tolerance: " << _tolerance << std::endl;
			triggerNotification(stream.str().c_str());
		}
		if (keyboardState.isKeyNewlyPressed(KeyboardState::KEY_U))
		{
			_bEnableROAM = !_bEnableROAM;
			std::stringstream stream;
			stream << "ROAM Algorithm " << (_bEnableROAM ? "Enabled" : "Disabled") << std::endl;
			triggerNotification(stream.str().c_str());
		}
		if (keyboardState.isKeyNewlyPressed(KeyboardState::KEY_T))
		{
			positionCameraBasedOnLightSource();
		}
		if (keyboardState.isKeyNewlyPressed(KeyboardState::KEY_Y))
		{
			generateRandomPlanet();
		}
		if (keyboardState.isKeyNewlyPressed(KeyboardState::KEY_TAB))
		{
			toggleOverlay();
		}
		if (keyboardState.isKeyPressed(KeyboardState::KEY_MINUS))
		{
			_pCameraSys->setCameraMovementMultiplier(_pCameraSys->getCameraMovementMultiplier() - 0.05f);
		}
		if (keyboardState.isKeyPressed(KeyboardState::KEY_EQUALS))
		{
			_pCameraSys->setCameraMovementMultiplier(_pCameraSys->getCameraMovementMultiplier() + 0.05f);
		}
		if (keyboardState.isKeyNewlyPressed(KeyboardState::KEY_ONE))
		{
			if(_pCameraSys->getType() == ORBITAL)
			{
				changeCameraSystem(FREELOOK);
			}
			else if (_pCameraSys->getType() == FREELOOK)
			{
				changeCameraSystem(ORBITAL);
			}
		}
		if (keyboardState.isKeyNewlyPressed(KeyboardState::KEY_ZERO))
		{
			_pCameraSys->setCameraMovementMultiplier(1.0f);
		}
		if (keyboardState.isKeyNewlyPressed(KeyboardState::KEY_SLASH))
		{
			setScreenSaverMode(true);
			if(_bMainMenuEnabled) toggleMenu();
		}
	}
}

void
View::handleGamepad(const GamepadState& padState)
{
	if (padState.isAnyButtonPressed())
	{
		_inactivityTimer = 0.0f;
		if (_bScreenSaverMode) setScreenSaverMode(false);
	}	
	if (_pCameraSys->getType() == ORBITAL)
	{
		if (padState.getVertical() > GAMEPAD_DEADZONE_CUTOFFF)
			_pCameraSys->moveUp(abs(padState.getVertical()));
		else if (padState.getVertical() < -GAMEPAD_DEADZONE_CUTOFFF)
			_pCameraSys->moveDown(abs(padState.getVertical()));
		if (padState.getHorizontal() > GAMEPAD_DEADZONE_CUTOFFF)
			_pCameraSys->moveRight(abs(padState.getHorizontal()));
		else if (padState.getHorizontal() < -GAMEPAD_DEADZONE_CUTOFFF)
			_pCameraSys->moveLeft(abs(padState.getHorizontal()));
		if (padState.getRightVertical() > GAMEPAD_DEADZONE_CUTOFFF)
			_pCameraSys->moveBackwards(abs(padState.getRightVertical()));
		else if (padState.getRightVertical() < -GAMEPAD_DEADZONE_CUTOFFF)
			_pCameraSys->moveForward(abs(padState.getRightVertical()));
		if (padState.getPOV() > GAMEPAD_DEADZONE_CUTOFFF)
			_pCameraSys->moveBackwards(abs(padState.getPOV()));
		else if (padState.getPOV() < -GAMEPAD_DEADZONE_CUTOFFF)
			_pCameraSys->moveForward(abs(padState.getPOV()));
	}
	else // freelook camera:
	{
		if (padState.getVertical() > GAMEPAD_DEADZONE_CUTOFFF)
			_pCameraSys->moveBackwards(abs(padState.getVertical()));
		else if (padState.getVertical() < -GAMEPAD_DEADZONE_CUTOFFF)
			_pCameraSys->moveForward(abs(padState.getVertical()));
		if (padState.getHorizontal() > GAMEPAD_DEADZONE_CUTOFFF)
			_pCameraSys->moveRight(abs(padState.getHorizontal()));
		else if (padState.getHorizontal() < -GAMEPAD_DEADZONE_CUTOFFF)
			_pCameraSys->moveLeft(abs(padState.getHorizontal()));
		// TODO better interface for rotating the camera with right stick
		if (padState.getRightVertical() > GAMEPAD_DEADZONE_CUTOFFF)
			_pCameraSys->spinCamera(0.0f, abs(padState.getRightVertical()) * 10.0f);
		else if (padState.getRightVertical() < -GAMEPAD_DEADZONE_CUTOFFF)
			_pCameraSys->spinCamera(0.0f,  abs(padState.getRightVertical()) * -10.0f);
		if (padState.getRightHorizontal() > GAMEPAD_DEADZONE_CUTOFFF)
			_pCameraSys->spinCamera(abs(padState.getRightHorizontal()) * 10.0f, 0.0f);
		else if (padState.getRightHorizontal() < -GAMEPAD_DEADZONE_CUTOFFF)
			_pCameraSys->spinCamera(abs(padState.getRightHorizontal()) * -10.0f, 0.0f);
		if (padState.getPOV() > GAMEPAD_DEADZONE_CUTOFFF)
			_pCameraSys->moveUp(abs(padState.getPOV()));
		else if (padState.getPOV() < -GAMEPAD_DEADZONE_CUTOFFF)
			_pCameraSys->moveDown(abs(padState.getPOV()));
		if (padState.isButtonPressed(GAMEPAD_FREELOOK_UP))
			_pCameraSys->adjustRadius(1.0f);
		if(padState.isButtonPressed(GAMEPAD_FREELOOK_DOWN))
			_pCameraSys->adjustRadius(-1.0f);

	}
	if (padState.isButtonNewlyPressed(GAMEPAD_RANDOMPLANET))
	{
		generateRandomPlanet();
	}
	if (padState.isButtonNewlyPressed(GAMEPAD_START))
	{
		positionCameraBasedOnLightSource();
	}
	if (padState.isButtonNewlyPressed(GAMEPAD_TOGGLEWIREFRAME))
	{
		toggleWireframe();
		std::stringstream stream;
		stream << "Wireframe View " << (_bWireframe ? "Enabled" : "Disabled") << std::endl;
		triggerNotification(stream.str().c_str());
	}
	// TODO maybe camera speed adjustors?:
	/*if (padState.isButtonNewlyPressed(GamepadState::Button_MINUS))
	{
		_pCameraSys->setCameraMovementMultiplier(_pCameraSys->getCameraMovementMultiplier() - 0.25f);
	}
	if (padState.isButtonNewlyPressed(GamepadState::Button_EQUALS))
	{
		_pCameraSys->setCameraMovementMultiplier(_pCameraSys->getCameraMovementMultiplier() + 0.25f);
	}*/
	if (padState.isButtonNewlyPressed(GAMEPAD_CHANGECAMERA))
	{
		if(_pCameraSys->getType() == ORBITAL)
		{
			changeCameraSystem(FREELOOK);
		}
		else if (_pCameraSys->getType() == FREELOOK)
		{
			changeCameraSystem(ORBITAL);
		}
	}
}

void View::DelegateMouseInputLogic( Input::MouseStates mouseButtonPressed, float xRelative, float yRelative, float zRelative )
{
	if(!_bCustomizeWindowEnabled)
	{
		switch (mouseButtonPressed)
		{
		case Input::LEFT_CLICK:
			_pCameraSys->spinCamera(xRelative, yRelative);
			_inactivityTimer = 0.0f;
			setScreenSaverMode(false);
			break;
		case Input::RIGHT_CLICK:
			break;
		case Input::MIDDLE_CLICK:
			break;
		default:
			break; // throw away the input event
		}

		if(zRelative != 0.0f)
		{
			_pCameraSys->adjustRadius(zRelative);
		}
	}
}

Horizon::CButton* View::createButton(Horizon::IFrame* parent, Horizon::ITexture* buttonTexture, float x, float y, float width, float height, const char* buttonText) {
	Horizon::CInterfaceFactory* factory = _pInterface->GetFactory();
	Horizon::CButton* button = factory->CreateButton(parent, x, y, width, height);
	button->SetAnchor(Horizon::ORIGIN_CENTRE);

	// Button Label
	Horizon::SFrameDesc textDesc;
	textDesc.Label = buttonText;
	textDesc.Anchor = Horizon::ORIGIN_CENTRE;
	Horizon::CStaticText* label = factory->CreateStaticText(button, 0.0f, 0.0f, width*0.9f, height*0.9f, &textDesc);
	label->SetHAlign(Horizon::EHA_CENTER);
	
	Horizon::CTexturedButton* texBut = (Horizon::CTexturedButton*)button;
	texBut->SetNormalTexture(buttonTexture);
	texBut->SetHoverTexture(buttonTexture);
	texBut->SetFocusTexture(buttonTexture);
	texBut->SetDisabledTexture(buttonTexture);
	texBut->SetPressedTexture(buttonTexture);
	texBut->SetNormalColor(0xAAFFFFFF);
	texBut->SetHoverColor(0xD4FFFFFF);
	texBut->SetFocusColor(0xCCFFFFFF);
	texBut->SetDisabledColor(0xCCFFFFFF);
	texBut->SetPressedColor(0xFFFFFFFF);
	return button;
}
void View::toggleWireframe() {
	_bWireframe = !_bWireframe;
}

/** Initializes the parameters required for rendering fog */
void View::setupFog()
{
	_pGraphics->SetFogEnable(true);
	Horizon::COLORF fogColor = { 0.435f, 0.655f, 0.988f, 0.5f };
	Horizon::COLORI iFogColor = Horizon::ColorF2I(fogColor);
	_pGraphics->SetFogColor(iFogColor);
	_pGraphics->SetFogMode(Horizon::FOGMODE_LINEAR);
	_pGraphics->SetFogStart(0.00001f);
	_pGraphics->SetFogEnd(4000.0f);
}

/** Initializes the parameters required for proper lighting */
void View::setupLighting()
{
	_pSunlight = new Horizon::CLight();
	_pSunlight->Type = Horizon::ELT_DIRECTIONAL;
	_pSunlight->Location = Horizon::VECTOR3(0.0f, 0.0f, _lightRadius);
	_pSunlight->Direction = Horizon::VECTOR3(0.0f, 0.0f, -1.0f);
	_pSunlight->Falloff = 1.0f;
	_pSunlight->Attenuation0 = 0.0;
	_pSunlight->Attenuation1 = 1.0;

	Horizon::COLORF diffuse;
	diffuse.a = 1.0f;
	diffuse.r = 1.0f;
	diffuse.g = 1.0f;
	diffuse.b = 1.0f;

	Horizon::COLORF specular;
	specular.a = 1.0f;
	specular.r = 1.0f;
	specular.g = 1.0f;
	specular.b = 1.0f;

	Horizon::COLORF ambient;
	ambient.a = 1.0f;
	ambient.r = 0.0f;
	ambient.g = 0.0f;
	ambient.b = 0.0f;

	_pSunlight->Diffuse = diffuse;
	_pSunlight->Ambient = ambient;
	_pSunlight->Specular = specular;
}

void
View::updateLight(float deltaTime)
{
	if(_sunMovementEnabled)
	{
		_lightTheta += _orbitSpeed*deltaTime;
		while (_lightTheta > 6.28318531f) _lightTheta -= 6.28318531f;
		float x = _lightRadius * cosf(_lightTheta);
		float z = _lightRadius * sinf(_lightTheta);

		_pSunlight->Location = Horizon::VECTOR3(x, 0.0f, z);
		Horizon::VECTOR3 normalizedVector;
		Horizon::VectorNormalize(&normalizedVector, &_pSunlight->Location );
		normalizedVector.x  *= -1.0f;
		normalizedVector.z  *= -1.0f;
		_pSunlight->Direction = normalizedVector;
	}
	Progeny::PgVector3 cameraPos = _pCameraSys->getCameraPosition();
}

void
View::positionCameraBasedOnLightSource()
{
	float cameraRadius = _attributes._radius * SCREENSAVER_CAMERA_RADIUS_MULTIPLIER;
	float cameraAngle = _lightTheta + SCREENSAVER_CAMERA_OFFSET;
	float x = cameraRadius * cosf(cameraAngle);
	float y = SCREENSAVER_CAMERA_MAX_TILT*cameraRadius*cosf(cameraAngle);
	float z = cameraRadius * sinf(cameraAngle);
	Horizon::VECTOR3 cameraPos = Horizon::VECTOR3(x, y, z);
	_pCameraSys->setCameraPosition(cameraPos);
}

void
View::updateAtmosphericOpacity(float cameraAltitude, float cameraSunAngleDegs)
{
	float opacity = 0.0f;
	if (cameraAltitude < MAXIMUM_ATMOSPHERE && cameraSunAngleDegs < MAXIMUM_DAYTIME_ANGLE)
	{
		// Calculate opacity of planet sky due to altitude
		float altitudeOpacity = 1.0f;
		if (cameraAltitude > MINIMUM_ATMOSPHERE)
		{
			altitudeOpacity *= (MAXIMUM_ATMOSPHERE-cameraAltitude)/(MAXIMUM_ATMOSPHERE-MINIMUM_ATMOSPHERE);
		}
		// Calculate opacity of planet sky due to time of day
		float timeOpacity = 1.0f;
		if (cameraSunAngleDegs > MINIMUM_DAYTIME_ANGLE)
		{
			timeOpacity *= (MAXIMUM_DAYTIME_ANGLE-cameraSunAngleDegs)/(MAXIMUM_DAYTIME_ANGLE-MINIMUM_DAYTIME_ANGLE);
		}
		opacity = altitudeOpacity*timeOpacity;
	}

	// TODO use calculated opacity to set transparency of inner skybox colored to planet's "atmosphere color"
	_pSkybox->setSkyOpacity(opacity);

}

void
View::createViewOverlay( Horizon::CInterfaceFactory* factory )
{
	factory->SetStyle(INTERFACE_STYLE_TEXTURED);
	Horizon::ITexture* bgtexture = _pGraphics->CreateTextureFromFile("Data/Images/creator_menu_bg.png", false);

	_pInfoOverlay = factory->CreateDlg(NULL, 
										STATS_POSITION_X*INTERFACE_SCALE, 
										STATS_POSITION_Y*INTERFACE_SCALE, 
										STATS_WIDTH*INTERFACE_SCALE, 
										STATS_HEIGHT*INTERFACE_SCALE);
	Horizon::CTexturedDialog* texDlg = (Horizon::CTexturedDialog*)_pInfoOverlay;
	texDlg->SetNormalTexture(bgtexture);
	texDlg->SetHoverTexture(bgtexture);
	texDlg->SetFocusTexture(bgtexture);
	texDlg->SetDisabledTexture(bgtexture);
	texDlg->SetNormalColor(0x30FFFFFF);
	texDlg->SetHoverColor(0x30FFFFFF);
	texDlg->SetFocusColor(0x30FFFFFF);
	texDlg->SetDisabledColor(0x30FFFFFF);
	_pInfoOverlay->SetUserPositionable(false);
	_pInfoOverlay->SetTitleBarDimensions(0.0f,0.0f);

	Horizon::CContainer* _container = factory->CreateContainer( _pInfoOverlay, 
																0.0f, 
																0.0f, 
																STATS_WIDTH*INTERFACE_SCALE, 
																STATS_HEIGHT*INTERFACE_SCALE);
	_container->SetVisible(true);

	Horizon::SFrameDesc desc;
	desc.Label = "Triangles: ";
	desc.HAlign = Horizon::EHA_LEFT;
	desc.VAlign = Horizon::EVA_CENTER;
	desc.TextScale = 0.6f;
	Horizon::CStaticText* text = factory->CreateStaticText(_container, -11.5f, -0.2f, 5.0f, STATS_HEIGHT*INTERFACE_SCALE+0.2f, &desc);
	text->SetTextScale(0.6f);
	_pTriangleCount = factory->CreateStaticText(_container, -7.0f, -0.2f, 5.5f, STATS_HEIGHT*INTERFACE_SCALE+0.2f, &desc);
	_pTriangleCount->SetText("0");
	_pTriangleCount->SetTextScale(0.6f);

	desc.Label = "|  Vertex Count: ";
	text = factory->CreateStaticText( _container, -2.0f, -0.2f, 9.0f, STATS_HEIGHT*INTERFACE_SCALE+0.2f, &desc);
	text->SetTextScale(0.6f);
	_pVertexCount = factory->CreateStaticText(_container, 3.0f, -0.2f, 5.0f, STATS_HEIGHT*INTERFACE_SCALE+0.2f, &desc);
	_pVertexCount->SetText("0");
	_pVertexCount->SetTextScale(0.6f);

	// Setup notification window
	_popupNotification = factory->CreateDlg(NULL, 
											NOTIFICATION_POSITION_X*INTERFACE_SCALE, 
											NOTIFICATION_POSITION_Y*INTERFACE_SCALE, 
											NOTIFICATION_WIDTH*INTERFACE_SCALE, 
											NOTIFICATION_HEIGHT*INTERFACE_SCALE);
	texDlg = (Horizon::CTexturedDialog*)_popupNotification;
	texDlg->SetNormalTexture(bgtexture);
	texDlg->SetHoverTexture(bgtexture);
	texDlg->SetFocusTexture(bgtexture);
	texDlg->SetDisabledTexture(bgtexture);
	texDlg->SetNormalColor(0x30FFFFFF);
	texDlg->SetHoverColor(0x30FFFFFF);
	texDlg->SetFocusColor(0x30FFFFFF);
	texDlg->SetDisabledColor(0x30FFFFFF);
	_popupNotification->SetUserPositionable(false);
	_popupNotification->SetTitleBarDimensions(0.0f,0.0f);

	Horizon::CContainer* _container2 = factory->CreateContainer(_popupNotification,
																0.0f, 
																0.0f, 
																NOTIFICATION_WIDTH*INTERFACE_SCALE,
																NOTIFICATION_HEIGHT*INTERFACE_SCALE+0.2f);
	_container2->SetVisible(true);

	Horizon::SFrameDesc notDesc;
	notDesc.HAlign = Horizon::EHA_CENTER;
	notDesc.VAlign = Horizon::EVA_CENTER;
	_notificationText = factory->CreateStaticText( _container2, 
													0.0f, 
													-0.2f, 
													NOTIFICATION_WIDTH*INTERFACE_SCALE,
													NOTIFICATION_HEIGHT*INTERFACE_SCALE+0.2f, 
													&notDesc );
	_notificationText->SetText("TestNotification");
	_notificationText->SetTextScale(0.6f);
}

void
View::createMainMenu(Horizon::CInterfaceFactory* factory)
{
	factory->SetStyle(INTERFACE_STYLE_TEXTURED);
	Horizon::ITexture* bgtexture = _pGraphics->CreateTextureFromFile("Data/Images/creator_menu_bg.png", false);

	_pMainMenu = factory->CreateDlg(NULL, MAIN_MENU_POSITION_X*INTERFACE_SCALE, MAIN_MENU_POSITION_Y*INTERFACE_SCALE, MAIN_MENU_WIDTH*INTERFACE_SCALE, MAIN_MENU_HEIGHT*INTERFACE_SCALE);
	Horizon::CTexturedDialog* texDlg = (Horizon::CTexturedDialog*)_pMainMenu;
	texDlg->SetNormalTexture(bgtexture);
	texDlg->SetHoverTexture(bgtexture);
	texDlg->SetFocusTexture(bgtexture);
	texDlg->SetDisabledTexture(bgtexture);
	texDlg->SetNormalColor(0x80FFFFFF);
	texDlg->SetHoverColor(0x80FFFFFF);
	texDlg->SetFocusColor(0x80FFFFFF);
	texDlg->SetDisabledColor(0x80FFFFFF);
	_pMainMenu->SetUserPositionable(false);
	_pMainMenu->SetTitleBarDimensions(0.0f, 0.2f);

	Horizon::CContainer* _container = factory->CreateContainer(_pMainMenu, MAIN_MENU_POSITION_X*INTERFACE_SCALE, MAIN_MENU_POSITION_Y*INTERFACE_SCALE, MAIN_MENU_WIDTH*INTERFACE_SCALE, MAIN_MENU_HEIGHT*INTERFACE_SCALE);
	_container->SetVisible(true);
	_container->SetAnchor(Horizon::ORIGIN_CENTRE);
	_container->SetOrigin(Horizon::ORIGIN_CENTRE);

	Horizon::ITexture* buttontexture = _pGraphics->CreateTextureFromFile("Data/Images/menubutton.png", false);
	Horizon::CButton* returnButton = createButton(_pMainMenu ,buttontexture, 0.0f, 2.55f, 7.0f, 1.5f, "RETURN");
	returnButton->AddEventHandler(Horizon::EVNCLASS_BUTTON, Horizon::EVN_BUTTON_ACTIVATED, returnClicked, this);

	Horizon::CButton* randomButton = createButton(_pMainMenu ,buttontexture, 0.0f, 0.85f, 7.0f, 1.5f, "RANDOM");
	randomButton->AddEventHandler(Horizon::EVNCLASS_BUTTON, Horizon::EVN_BUTTON_ACTIVATED, randomClicked, this);

	Horizon::CButton* customizeButton = createButton(_pMainMenu ,buttontexture, 0.0f, -0.85f, 7.0f, 1.5f, "CUSTOMIZE");
	customizeButton->AddEventHandler(Horizon::EVNCLASS_BUTTON, Horizon::EVN_BUTTON_ACTIVATED, customizeClicked, this);

	Horizon::CButton* exitButton = createButton(_container ,buttontexture, 0.0f, -2.55f, 7.0f, 1.5f, "EXIT");
	exitButton->AddEventHandler(Horizon::EVNCLASS_BUTTON, Horizon::EVN_BUTTON_ACTIVATED, exitClicked, this);
}

void View::createCustomizeMenu(Horizon::CInterfaceFactory* factory)
{
	factory->SetStyle(INTERFACE_STYLE_TEXTURED);
	Horizon::ITexture* bgtexture = _pGraphics->CreateTextureFromFile("Data/Images/creator_menu_bg.png", false);

	_pCustomizeWindow = factory->CreateDlg(NULL, 
										   CUSTOMIZATION_WINDOW_POSITION_X*INTERFACE_SCALE, 
										   CUSTOMIZATION_WINDOW_POSITION_Y*INTERFACE_SCALE, 
										   CUSTOMIZATION_WINDOW_WIDTH*INTERFACE_SCALE, 
										   CUSTOMIZATION_WINDOW_HEIGHT*INTERFACE_SCALE);
	Horizon::CTexturedDialog* texDlg = (Horizon::CTexturedDialog*)_pCustomizeWindow;
	texDlg->SetNormalTexture(bgtexture);
	texDlg->SetHoverTexture(bgtexture);
	texDlg->SetFocusTexture(bgtexture);
	texDlg->SetDisabledTexture(bgtexture);
	texDlg->SetNormalColor(0x40FFFFFF);
	texDlg->SetHoverColor(0x40FFFFFF);
	texDlg->SetFocusColor(0x40FFFFFF);
	texDlg->SetDisabledColor(0x40FFFFFF);
	_pCustomizeWindow->SetUserPositionable(false);
	_pCustomizeWindow->SetTitleBarDimensions(0.0f,0.0f);

	Horizon::CContainer* _container = factory->CreateContainer(_pCustomizeWindow, 0.0f, 0.0f, CUSTOMIZATION_WINDOW_WIDTH*INTERFACE_SCALE, CUSTOMIZATION_WINDOW_HEIGHT*INTERFACE_SCALE);
	_container->SetVisible(true);
	_container->SetOrigin(Horizon::ORIGIN_CENTRE);

	// Labels
	Horizon::SFrameDesc desc;
	desc.HAlign = Horizon::EHA_RIGHT;
	desc.VAlign = Horizon::EVA_CENTER;
	desc.Anchor = Horizon::ORIGIN_LEFT;

	desc.Label = "RADIUS";
	Horizon::CStaticText* text = factory->CreateStaticText(_container, -14.0f, 7.0f, 7.0f, 1.0f, &desc);
	text->SetTextScale(0.75);

	desc.Label = "OCEAN LEVEL";
	text = factory->CreateStaticText(_container, -14.0f, 5.0f, 7.0f, 1.0f, &desc);
	text->SetTextScale(0.75);

	desc.Label = "CONTINENT FREQ";
	text = factory->CreateStaticText(_container, -14.0f, 3.0f, 7.0f, 1.0f, &desc);
	text->SetTextScale(0.75);

	desc.Label = "MOUNTAIN FREQ";
	text = factory->CreateStaticText(_container, -14.0f, 1.0f, 7.0f, 1.0f, &desc);
	text->SetTextScale(0.75);

	desc.Label = "COLOR SCHEME:";
	desc.HAlign = Horizon::EHA_LEFT;
	text = factory->CreateStaticText(_container, -14.0f, -1.0f, 7.0f, 1.0f, &desc);
	text->SetTextScale(0.75);

	desc.Label = "SEED";
	text = factory->CreateStaticText(_container,  1.0f, -6.0f, 7.0f, 1.0f, &desc);
	text->SetTextScale(0.75);

	desc.Label = "CRATERS";
	text = factory->CreateStaticText(_container,  1.0f, -3.0f, 7.0f, 1.0f, &desc);
	text->SetTextScale(0.75);

	// Checkbox 
	factory->SetStyle(INTERFACE_STYLE_WIREFRAME);
	_craterCheckbox = factory->CreateCheckBox(_container, 7.0f, -3.0f, 1.0f, 1.0f);
	_craterCheckbox->SetVisible(true);
	_craterCheckbox->AddEventHandler(Horizon::EVNCLASS_CHECKBOX, Horizon::EVN_CHECKBOX_CHANGED, checkboxChangedEvent, this);

	// Editable Text
	factory->SetStyle(INTERFACE_STYLE_WIREFRAME);
	Horizon::SEditableTextDesc editDesc;
	editDesc.HAlign = Horizon::EHA_CENTER;
	editDesc.VAlign = Horizon::EVA_CENTER;
	editDesc.Label = "1";
	_seedEditor = factory->CreateEditableText(_container, 7.0f, -6.0f, 7.0f, 1.5f, &editDesc);
	_seedEditor->SetText(L"1");
	_seedEditor->SetCustomDataInt(SEED);
	_seedEditor->AddEventHandler(Horizon::EVNCLASS_FOCUS, Horizon::EVN_FOCUS_LOSEFOCUS, textEditedEvent, this);
	_seedEditor->SetMultiLine(false);
	
	_radiusEditor = factory->CreateEditableText(_container, 8.0f, 7.0f, 7.0f, 1.5f, &editDesc);
	_radiusEditor->SetText(L"1");
	_radiusEditor->SetCustomDataInt(RADIUS);
	_radiusEditor->AddEventHandler(Horizon::EVNCLASS_FOCUS, Horizon::EVN_FOCUS_LOSEFOCUS, textEditedEvent, this);
	_radiusEditor->SetMultiLine(false);

	_oceanHeightEditor = factory->CreateEditableText(_container, 8.0f, 5.0f, 7.0f, 1.5f, &editDesc);
	_oceanHeightEditor->SetText(L"1");
	_oceanHeightEditor->SetCustomDataInt(OCEAN);
	_oceanHeightEditor->AddEventHandler(Horizon::EVNCLASS_FOCUS, Horizon::EVN_FOCUS_LOSEFOCUS, textEditedEvent, this);
	_oceanHeightEditor->SetMultiLine(false);
	
	_continentFreqEditor = factory->CreateEditableText(_container, 8.0f, 3.0f, 7.0f, 1.5f, &editDesc);
	_continentFreqEditor->SetText(L"1");
	_continentFreqEditor->SetCustomDataInt(CONTINENT);
	_continentFreqEditor->AddEventHandler(Horizon::EVNCLASS_FOCUS, Horizon::EVN_FOCUS_LOSEFOCUS, textEditedEvent, this);
	_continentFreqEditor->SetMultiLine(false);

	_mountainCountEditor = factory->CreateEditableText(_container, 8.0f, 1.0f, 7.0f, 1.5f, &editDesc);
	_mountainCountEditor->SetText(L"1");
	_mountainCountEditor->SetCustomDataInt(MOUNTAIN);
	_mountainCountEditor->AddEventHandler(Horizon::EVNCLASS_FOCUS, Horizon::EVN_FOCUS_LOSEFOCUS, textEditedEvent, this);
	_mountainCountEditor->SetMultiLine(false);

	// Sliders
	factory->SetStyle(INTERFACE_STYLE_TEXTURED);
	Horizon::ITexture* sliderTexture = _pGraphics->CreateTextureFromFile("Data/Images/Slider/slider.png", false);
	Horizon::ITexture* sliderLeafTexture = _pGraphics->CreateTextureFromFile("Data/Images/Slider/slider_leaf.png", false);

	Horizon::CTexturedSlider* slider = (Horizon::CTexturedSlider*)Horizon::CTexturedSlider::Create(_pInterface, _container, -1.25f, 7.0f, 10.0f, 1.7f);;
	slider->SetNormalTexture(sliderTexture);
	slider->SetHoverTexture(sliderTexture);
	slider->SetFocusTexture(sliderTexture);
	slider->SetDisabledTexture(sliderTexture);
	slider->SetLeafNormalTexture(sliderLeafTexture);
	slider->SetLeafHoverTexture(sliderLeafTexture);
	slider->SetLeafFocusTexture(sliderLeafTexture);
	slider->SetLeafDisabledTexture(sliderLeafTexture);
	slider->SetVisible(true);
	_radiusSlider = slider;
	_radiusSlider->SetCustomDataInt(RADIUS);
	_radiusSlider->AddEventHandler(Horizon::EVNCLASS_SLIDER, Horizon::EVN_SLIDER_CHANGED, sliderEditedEvent, this);

	slider = (Horizon::CTexturedSlider*)Horizon::CTexturedSlider::Create(_pInterface, _container, -1.25f, 5.0f, 10.0f, 1.7f);;
	slider->SetNormalTexture(sliderTexture);
	slider->SetHoverTexture(sliderTexture);
	slider->SetFocusTexture(sliderTexture);
	slider->SetDisabledTexture(sliderTexture);
	slider->SetLeafNormalTexture(sliderLeafTexture);
	slider->SetLeafHoverTexture(sliderLeafTexture);
	slider->SetLeafFocusTexture(sliderLeafTexture);
	slider->SetLeafDisabledTexture(sliderLeafTexture);
	slider->SetVisible(true);
	_oceanHeightSlider = slider;
	_oceanHeightSlider->SetCustomDataInt(OCEAN);
	_oceanHeightSlider->AddEventHandler(Horizon::EVNCLASS_SLIDER, Horizon::EVN_SLIDER_CHANGED, sliderEditedEvent, this);

	slider = (Horizon::CTexturedSlider*)Horizon::CTexturedSlider::Create(_pInterface, _container, -1.25f, 3.0f, 10.0f, 1.7f);;
	slider->SetNormalTexture(sliderTexture);
	slider->SetHoverTexture(sliderTexture);
	slider->SetFocusTexture(sliderTexture);
	slider->SetDisabledTexture(sliderTexture);
	slider->SetLeafNormalTexture(sliderLeafTexture);
	slider->SetLeafHoverTexture(sliderLeafTexture);
	slider->SetLeafFocusTexture(sliderLeafTexture);
	slider->SetLeafDisabledTexture(sliderLeafTexture);
	slider->SetVisible(true);
	_continentFreqSlider = slider;
	_continentFreqSlider->SetCustomDataInt(CONTINENT);
	_continentFreqSlider->AddEventHandler(Horizon::EVNCLASS_SLIDER, Horizon::EVN_SLIDER_CHANGED, sliderEditedEvent, this);

	slider = (Horizon::CTexturedSlider*)Horizon::CTexturedSlider::Create(_pInterface, _container, -1.25f, 1.0f, 10.0f, 1.7f);;
	slider->SetNormalTexture(sliderTexture);
	slider->SetHoverTexture(sliderTexture);
	slider->SetFocusTexture(sliderTexture);
	slider->SetDisabledTexture(sliderTexture);
	slider->SetLeafNormalTexture(sliderLeafTexture);
	slider->SetLeafHoverTexture(sliderLeafTexture);
	slider->SetLeafFocusTexture(sliderLeafTexture);
	slider->SetLeafDisabledTexture(sliderLeafTexture);
	slider->SetVisible(true);
	_mountainCountSlider = slider;
	_mountainCountSlider->SetCustomDataInt(MOUNTAIN);
	_mountainCountSlider->AddEventHandler(Horizon::EVNCLASS_SLIDER, Horizon::EVN_SLIDER_CHANGED, sliderEditedEvent, this);

	// Buttons
	Horizon::ITexture* buttontexture = _pGraphics->CreateTextureFromFile("Data/Images/menubutton.png", false);
	Horizon::ITexture* dicetexture = _pGraphics->CreateTextureFromFile("Data/Images/dice.png", false);

	Horizon::CButton* radiusRandom = createButton(_container, dicetexture, 13.0f, 7.0f, 1.5f, 1.5f, "");
	radiusRandom->SetCustomDataInt(RADIUS);
	radiusRandom->AddEventHandler(Horizon::EVNCLASS_BUTTON, Horizon::EVN_BUTTON_ACTIVATED, randomStat, this);

	Horizon::CButton* oceanRandom = createButton(_container, dicetexture, 13.0f, 5.0f, 1.5f, 1.5f, "");
	oceanRandom->SetCustomDataInt(OCEAN);
	oceanRandom->AddEventHandler(Horizon::EVNCLASS_BUTTON, Horizon::EVN_BUTTON_ACTIVATED, randomStat, this);

	Horizon::CButton* continentRandom = createButton(_container, dicetexture, 13.0f, 3.0f, 1.5f, 1.5f, "");
	continentRandom->SetCustomDataInt(CONTINENT);
	continentRandom->AddEventHandler(Horizon::EVNCLASS_BUTTON, Horizon::EVN_BUTTON_ACTIVATED, randomStat, this);

	Horizon::CButton* mountainRandom = createButton(_container, dicetexture, 13.0f, 1.0f, 1.5f, 1.5f, "");
	mountainRandom->SetCustomDataInt(MOUNTAIN);
	mountainRandom->AddEventHandler(Horizon::EVNCLASS_BUTTON, Horizon::EVN_BUTTON_ACTIVATED, randomStat, this);

	Horizon::CButton* seedRandom = createButton(_container, dicetexture, 13.0f, -6.0f, 1.5f, 1.5f, "");
	seedRandom->SetCustomDataInt(SEED);
	seedRandom->AddEventHandler(Horizon::EVNCLASS_BUTTON, Horizon::EVN_BUTTON_ACTIVATED, randomStat, this);

	Horizon::CButton* previewButton = createButton(_container, buttontexture, 3.5f, -9.0f, 6.0f, 1.1f, "PREVIEW");
	previewButton->AddEventHandler(Horizon::EVNCLASS_BUTTON, Horizon::EVN_BUTTON_ACTIVATED, previewPlanetClicked, this);

	Horizon::CButton* generateButton = createButton(_container, buttontexture, 10.5f, -9.0f, 6.0f, 1.1f, "GENERATE");
	generateButton->AddEventHandler(Horizon::EVNCLASS_BUTTON, Horizon::EVN_BUTTON_ACTIVATED, generateCustomPlanetClicked, this);

	// Color selector
	factory->SetStyle(INTERFACE_STYLE_WIREFRAME);
	Horizon::SFrameDesc colorListDesc;
	colorListDesc.HAlign = Horizon::EHA_CENTER;
	colorListDesc.VAlign = Horizon::EVA_CENTER;
	colorListDesc.Anchor = Horizon::ORIGIN_TOPLEFT;
	colorListDesc.TextScale = 0.75f;
	_colorList = factory->CreateListBox(_container, -14.0f, -2.0f, 12.0f, 5.0f, &colorListDesc);
	_colorList->GetScrollBox()->SetHorizontalScrollMode(Horizon::SCROLL_FORCEOFF);
	_colorList->GetScrollBox()->SetVerticalScrollMode(Horizon::SCROLL_FORCEON);

	Horizon::SListElementDesc listElementDesc;
	listElementDesc.Label = "Earth";
	listElementDesc.Selected = true;
	Horizon::CListElement* listElement = factory->CreateListElement(_colorList, 0.0f, 0.0f, 10.0f, 1.0f, &listElementDesc);
	listElement->SetCustomDataInt(Progeny::COLORS_EARTH);

	listElementDesc.Label = "Lunar";
	listElementDesc.Selected = false;
	listElementDesc.ID = 1;
	listElement = factory->CreateListElement(_colorList, 0.0f, 0.0f, 10.0f, 1.0f, &listElementDesc);
	listElement->SetCustomDataInt(Progeny::COLORS_MOON);

	listElementDesc.Label = "Mercury";
	listElementDesc.Selected = false;
	listElementDesc.ID = 2;
	listElement = factory->CreateListElement(_colorList, 0.0f, 0.0f, 10.0f, 1.0f, &listElementDesc);
	listElement->SetCustomDataInt(Progeny::COLORS_MERCURY);

	listElementDesc.Label = "Venus";
	listElementDesc.Selected = false;
	listElementDesc.ID = 3;
	listElement = factory->CreateListElement(_colorList, 0.0f, 0.0f, 10.0f, 1.0f, &listElementDesc);
	listElement->SetCustomDataInt(Progeny::COLORS_VENUS);

	listElementDesc.Label = "Mars";
	listElementDesc.Selected = false;
	listElementDesc.ID = 4;
	listElement = factory->CreateListElement(_colorList, 0.0f, 0.0f, 10.0f, 1.0f, &listElementDesc);
	listElement->SetCustomDataInt(Progeny::COLORS_MARS);

	listElementDesc.Label = "Jupiter";
	listElementDesc.Selected = false;
	listElementDesc.ID = 5;
	listElement = factory->CreateListElement(_colorList, 0.0f, 0.0f, 10.0f, 1.0f, &listElementDesc);
	listElement->SetCustomDataInt(Progeny::COLORS_JUPITER);

	listElementDesc.Label = "Saturn";
	listElementDesc.Selected = false;
	listElementDesc.ID = 6;
	listElement = factory->CreateListElement(_colorList, 0.0f, 0.0f, 10.0f, 1.0f, &listElementDesc);
	listElement->SetCustomDataInt(Progeny::COLORS_SATURN);

	listElementDesc.Label = "Uranus";
	listElementDesc.Selected = false;
	listElementDesc.ID = 7;
	listElement = factory->CreateListElement(_colorList, 0.0f, 0.0f, 10.0f, 1.0f, &listElementDesc);
	listElement->SetCustomDataInt(Progeny::COLORS_URANUS);

	listElementDesc.Label = "Neptune";
	listElementDesc.Selected = false;
	listElementDesc.ID = 8;
	listElement = factory->CreateListElement(_colorList, 0.0f, 0.0f, 10.0f, 1.0f, &listElementDesc);
	listElement->SetCustomDataInt(Progeny::COLORS_NEPTUNE);

	listElementDesc.Label = "Asteroid";
	listElementDesc.Selected = false;
	listElementDesc.ID = 9;
	listElement = factory->CreateListElement(_colorList, 0.0f, 0.0f, 10.0f, 1.0f, &listElementDesc);
	listElement->SetCustomDataInt(Progeny::COLORS_ASTEROID);

	_colorList->SortElementsByID(true);
	_colorList->SetPointerFocusable(true);
	_colorList->SetEnabled(true);
	_colorList->UpdateListLayout();

	_colorList->AddEventHandler(Horizon::EVNCLASS_LISTELEMENT, Horizon::EVN_LISTELEMENT_SELECTED, colorSchemeSelected, this);
}

bool
	View::colorSchemeSelected(Horizon::IFrame* frame, Horizon::SEvent evn, void* evndata, void* userdata)
{
	
	Horizon::CListElement* evnelement = (Horizon::CListElement*)frame;
	View* view = (View*)userdata;
	if (evnelement->GetSelected())
	{
		view->_currentColorSchemeId = evnelement->GetCustomDataInt();
	}
	view->_bAttributesUpdated = true;
	return true;
}
void View::triggerNotification(const char* text)
{
	_notificationText->SetText(text);
	if(!_bNotificationEnabled)
	{
		_popupNotification->Activate(_pInterface);
		_bNotificationEnabled = true;
	}	
	_notificationTime = clock();
}

void View::toggleMenu()
{
	_bMainMenuEnabled = !_bMainMenuEnabled;
	if(_bMainMenuEnabled)
		_pMainMenu->Activate(_pInterface);
	else
		_pMainMenu->Deactivate();
}

void View::toggleOverlay()
{
	_bOverlayEnabled = !_bOverlayEnabled;
	if(_bOverlayEnabled)
		_pInfoOverlay->Activate(_pInterface);
	else
		_pInfoOverlay->Deactivate();
}

void View::toggleCreationMenu()
{
	_bCustomizeWindowEnabled = !_bCustomizeWindowEnabled;
	if(_bCustomizeWindowEnabled)
		_pCustomizeWindow->Activate(_pInterface);
	else
		_pCustomizeWindow->Deactivate();
}

void View::generateRandomPlanet()
{
	cleanupPlanet();
	srand((unsigned int)clock());
	_attributes._seed = rand();
	_attributes._radius = MINIMUM_PLANET_RADIUS + ((float)rand()/(float)RAND_MAX)*(MAXIMUM_PLANET_RADIUS-MINIMUM_PLANET_RADIUS);
	_attributes._oceanHeight = 0.75f*(float)rand()/(float)RAND_MAX;
	_attributes._mountainCount = (float)rand()/(float)RAND_MAX;
	_attributes._continentFreq = (float)rand()/(float)RAND_MAX;

	Progeny::PgColorScheme::RandomScheme(_colorScheme);
	_colorScheme.fRangeHeights[0] = _attributes._oceanHeight;
	initializePlanet();
	
	initializeCamera();

	_bAttributesUpdated = true;
}

float View::getFloatFromText( const wchar_t* text ) 
{
	return _wtof(text);
}

void View::generateCustomPlanet()
{
	cleanupPlanet();

	Progeny::PgColorScheme::init(_colorScheme, (Progeny::ColorScheme)_currentColorSchemeId);
	_colorScheme.fRangeHeights[0] = _attributes._oceanHeight;

	initializePlanet();

	updateConfigurationFields();

	initializeCamera();

}

void
View::updateConfigurationFields()
{
	if (_bAttributesUpdated)
	{
		std::wstring ws;

		Progeny::floatToWstring( _attributes._seed, ws, 10);
		_seedEditor->SetText(ws.c_str());

		Progeny::floatToWstring( _attributes._oceanHeight*100, ws, 3);
		_oceanHeightEditor->SetText(ws.c_str());
		_oceanHeightSlider->SetValue(_attributes._oceanHeight);

		Progeny::floatToWstring( _attributes._mountainCount*100, ws, 3);
		_mountainCountEditor->SetText(ws.c_str());
		_mountainCountSlider->SetValue(_attributes._mountainCount);

		Progeny::floatToWstring( _attributes._continentFreq*100, ws, 3);
		_continentFreqEditor->SetText(ws.c_str());
		_continentFreqSlider->SetValue(_attributes._continentFreq);

		Progeny::floatToWstring( _attributes._radius, ws, 5);
		_radiusEditor->SetText(ws.c_str());
		float normalizedRadius = (_attributes._radius - MINIMUM_PLANET_RADIUS)/(MAXIMUM_PLANET_RADIUS - MINIMUM_PLANET_RADIUS);
		_radiusSlider->SetValue(normalizedRadius);

		_craterCheckbox->SetValue((int)_attributes._useCraters);

		// Update color selection listbox:
		Horizon::IFrame* client = _colorList->GetClientArea();
		for (int i = 0; i < client->GetChildCount(); ++i)
		{
			Horizon::CListElement* element = (Horizon::CListElement*)client->GetChild(i);
			if (element->GetCustomDataInt() != _currentColorSchemeId)
			{
				element->SetSelected(false);
			}
			else
			{
				element->SetSelected(true);
			}
		}

		_bAttributesUpdated = false;

	}
	
}

bool View::returnClicked(Horizon::IFrame* frame, Horizon::SEvent evn, void* evndata, void* userdata)
{
	View* view = (View*)userdata;
	view->toggleMenu();
	return true;
}
bool View::randomClicked(Horizon::IFrame* frame, Horizon::SEvent evn, void* evndata, void* userdata)
{
	View* view = (View*)userdata;
	view->generateRandomPlanet();
	return true;
}
bool View::customizeClicked(Horizon::IFrame* frame, Horizon::SEvent evn, void* evndata, void* userdata)
{
	View* view = (View*)userdata;
	view->toggleMenu();
	view->toggleCreationMenu();
	return true;
}
bool View::exitClicked(Horizon::IFrame* frame, Horizon::SEvent evn, void* evndata, void* userdata)
{
	exit(0);
}
bool View::previewPlanetClicked(Horizon::IFrame* frame, Horizon::SEvent evn, void* evndata, void* userdata)
{
	View* view = (View*)userdata;
	view->generateCustomPlanet();
	return true;
}
bool View::generateCustomPlanetClicked(Horizon::IFrame* frame, Horizon::SEvent evn, void* evndata, void* userdata)
{
	View* view = (View*)userdata;
	view->toggleCreationMenu();
	view->generateCustomPlanet();
	return true;
}

void
View::setScreenSaverMode(bool enable)
{
	if(enable != _bScreenSaverMode)
	{
		std::stringstream stream;
		stream << "Demo Mode " << (enable ? "Enabled" : "Disabled") << std::endl;
		triggerNotification(stream.str().c_str());
	}
	_bScreenSaverMode = enable;
}

void View::cleanupPlanet()
{
	delete _pPlanet;
	Progeny::PgVertex::Array.reset();
}

void View::initializePlanet()
{
	Progeny::PgPlanetStrategy* strategy = new Progeny::PgPlanetStrategyRandom( _attributes, _colorScheme );
	_pPlanet = new Progeny::PgROAMPlanet(strategy);
	_pPlanet->init();
	_tolerance = DEFAULT_TOLERANCE;
	_pSkybox->setSkyColor( _colorScheme.iSkyColor );
	triggerNotification("Generating Planet...");
}

void View::initializeCamera()
{
	delete _pCameraSys;
	_pCameraSys = new OrbitalCameraSystem(_attributes._radius);
	positionCameraBasedOnLightSource();
}

void View::changeCameraSystem(CameraSystemTypes camSysType)
{
	Progeny::PgVector3 tempPos;
	Horizon::VECTOR3 tempPosHorizonVec;
	Horizon::QUATERNION tempOrientation;
	std::stringstream stream;

	switch(camSysType)
	{
	case FREELOOK:
		tempPos = _pCameraSys->getCameraPosition();
		delete _pCameraSys;

		tempPosHorizonVec.x = tempPos._x;
		tempPosHorizonVec.y = tempPos._y;
		tempPosHorizonVec.z = tempPos._z;

		_pCameraSys = new FreelookCameraSystem(_attributes._radius);
		_pCameraSys->setCameraPosition(tempPosHorizonVec);

		stream << "Freelook Camera Enabled" << std::endl;
		triggerNotification(stream.str().c_str());
		break;
	case ORBITAL:
		tempPos = _pCameraSys->getCameraPosition();
		tempOrientation = ((FreelookCameraSystem* )_pCameraSys)->getCameraOrientation();
		delete _pCameraSys;

		tempPosHorizonVec.x = tempPos._x;
		tempPosHorizonVec.y = tempPos._y;
		tempPosHorizonVec.z = tempPos._z;

		_pCameraSys = new OrbitalCameraSystem(_attributes._radius);
		_pCameraSys->setCameraPosition(tempPosHorizonVec);
		((OrbitalCameraSystem*)_pCameraSys)->setSlerpFrom(tempOrientation);

		stream << "Orbital Camera Enabled" << std::endl;
		triggerNotification(stream.str().c_str());
		break;
	default:
		break;
	}
}


void
View::toggleFog()
{
	_bEnableFog = !_bEnableFog;
	std::stringstream stream;
	stream << "Fog " << (_bEnableFog ? "Enabled" : "Disabled") << std::endl;
	triggerNotification(stream.str().c_str());
}

void View::updateAttributeFromSlider(float val, int control)
{
	if(control == RADIUS)
	{
		_attributes._radius = MINIMUM_PLANET_RADIUS + val*(MAXIMUM_PLANET_RADIUS-MINIMUM_PLANET_RADIUS);
	}
	else if( control == OCEAN )
	{
		_attributes._oceanHeight = val;
	}
	else if( control == CONTINENT )
	{
		_attributes._continentFreq = val;
	}
	else if( control == MOUNTAIN )
	{
		_attributes._mountainCount = val;
	}
	_bAttributesUpdated = true;
}

void View::updateAttributesFromText(float val, int control)
{	
	if( control == SEED )
	{
		_attributes._seed = val;
	}
	else if(control == RADIUS)
	{
		val = Progeny::minf(MAXIMUM_PLANET_RADIUS, val);
		val = Progeny::maxf(MINIMUM_PLANET_RADIUS, val);
		_attributes._radius = val;
	}
	else if( control == OCEAN )
	{
		val = Progeny::minf(100.0f, val);
		val = Progeny::maxf(0.0f, val);
		_attributes._oceanHeight  = val / 100.0f;
	}
	else if( control == CONTINENT )
	{
		val = Progeny::minf(100.0f, val);
		val = Progeny::maxf(0.0f, val);
		_attributes._continentFreq = val / 100.0f;
	}
	else if( control == MOUNTAIN )
	{
		val = Progeny::minf(100.0f, val);
		val = Progeny::maxf(0.0f, val);
		_attributes._mountainCount = val/100.0f;
	}
	_bAttributesUpdated = true;
}

bool View::randomStat(Horizon::IFrame* frame, Horizon::SEvent evn, void* evndata, void* userdata)
{
	View* view = (View*)userdata;
	Horizon::CButton* button = (Horizon::CButton*)frame;
	int control = button->GetCustomDataInt();
	if(control == View::SEED)
	{
		view->_attributes._seed = rand();
	}
	else if(control == RADIUS)
	{
		view->_attributes._radius = MINIMUM_PLANET_RADIUS + ((float)rand()/(float)RAND_MAX)*(MAXIMUM_PLANET_RADIUS-MINIMUM_PLANET_RADIUS);
	}
	else if( control == OCEAN )
	{
		view->_attributes._oceanHeight = (float)rand()/(float)RAND_MAX;
	}
	else if( control == CONTINENT )
	{
		view->_attributes._continentFreq = (float)rand()/(float)RAND_MAX;
	}
	else if( control == MOUNTAIN )
	{
		view->_attributes._mountainCount = (float)rand()/(float)RAND_MAX;
	}
	view->_bAttributesUpdated = true;
	return true;
}

bool View::sliderEditedEvent(Horizon::IFrame* frame, Horizon::SEvent evn, void* evndata, void* userdata)
{
	View* view = (View*)userdata;
	Horizon::CSlider* slider = (Horizon::CSlider*)frame;
	view->updateAttributeFromSlider(slider->GetValue(), slider->GetCustomDataInt());
	return true;
}

bool View::textEditedEvent(Horizon::IFrame* frame, Horizon::SEvent evn, void* evndata, void* userdata)
{
	View* view = (View*)userdata;
	Horizon::CEditableText* text = (Horizon::CEditableText*)frame;
	view->updateAttributesFromText(view->getFloatFromText(text->GetText()), text->GetCustomDataInt());
	return true;
}

bool View::checkboxChangedEvent(Horizon::IFrame* frame, Horizon::SEvent evn, void* evndata, void* userdata)
{
	View* view = (View*)userdata;
	Horizon::CCheckBox* checkbox = (Horizon::CCheckBox*)frame;
	view->_attributes._useCraters = (bool)checkbox->GetValue();
	view->_bAttributesUpdated = true;
	return true;
}