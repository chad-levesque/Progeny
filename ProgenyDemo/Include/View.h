#ifndef __VIEW_H__
#define __VIEW_H__

#include "Input.h"
#include "Graphics/Graphics.h"
#include "CameraSystem.h"

#include "PgPlanetStrategyRandom.h"
#include "PgPlanetStrategyFlat.h"
#include "PgPlanetAttributes.h"
#include "PgColorScheme.h"

#include <Toolbox/Event.h>

class Input;
class BoxCreationMenu;
class SphereCreationMenu;
class SkyBox;
class HorizonPgRenderer;

namespace Progeny {
	class PgROAMPlanet;
	class PgColorScheme;
}

namespace Horizon {
	class IGraphics;
	class CSystem;
	class CInterface;
	class CScreen;
	class CContainer;
	class CInterface;
	class CDialog;
	class IFrame;
	class CInterfaceFactory;
	class CStaticImage;
	class ITexture;
	class CLight;
	class CButton;
	class CStaticText;
	class CEditableText;
	class CSlider;
	class CListBox;
	class CCheckBox;
}

class View
{
public:
	View();
	virtual ~View();

	bool Init();
	void Update();
	void Destroy();

	void BindToInput(Input& pInput);
	void handleKeyboard(const KeyboardState& keyPressed);
	void handleGamepad(const GamepadState& padState);
	void DelegateMouseInputLogic(Input::MouseStates mouseButtonPressed, float xRelative, float yRelative, float zRelative);

	void triggerNotification(const char* text);

	void toggleWireframe();
	void setScreenSaverMode(bool enable);
	bool _bAttributesUpdated;
private:
	Horizon::CInterface* _pInterface;
	BoxCreationMenu* _boxCreationMenu;
	SphereCreationMenu* _sphereCreationMenu;

	void Render();

	static bool activateParameterMenu(Horizon::IFrame* frame, Horizon::SEvent evn, void* evndata, void* userdata);
	
	Horizon::CContainer* createBoxParameterMenu(Horizon::CInterfaceFactory* factory);
	Horizon::CButton* createButton(Horizon::IFrame* parent, Horizon::ITexture* buttonTexture, float x, float y, float width, float height, const char* buttonText);

	void setupFog();
	void setupLighting();
	void updateLight(float deltaTime);
	void updateAtmosphericOpacity(float cameraAltitude, float cameraSunAngleDegs);
	void positionCameraBasedOnLightSource();
	void createViewOverlay(Horizon::CInterfaceFactory* factory);
	void createMainMenu(Horizon::CInterfaceFactory* factory);
	void createCustomizeMenu(Horizon::CInterfaceFactory* factory);

	void toggleMenu();
	void toggleOverlay();
	void toggleCreationMenu();
	void toggleFog();

	void generateRandomPlanet();

	void initializePlanet();
	void generateCustomPlanet();

	void initializeCamera();
	void changeCameraSystem(CameraSystemTypes camSysType);

	void cleanupPlanet();
	Horizon::IGraphics* _pGraphics;
	Horizon::CScreen* _pMainScreen;
	
	// Main Menu
	Horizon::CDialog* _pMainMenu;
	bool _bMainMenuEnabled;
	static bool returnClicked(Horizon::IFrame* frame, Horizon::SEvent evn, void* evndata, void* userdata);
	static bool randomClicked(Horizon::IFrame* frame, Horizon::SEvent evn, void* evndata, void* userdata);
	static bool customizeClicked(Horizon::IFrame* frame, Horizon::SEvent evn, void* evndata, void* userdata);
	static bool exitClicked(Horizon::IFrame* frame, Horizon::SEvent evn, void* evndata, void* userdata);

	// Overlay objects
	Horizon::CDialog* _pInfoOverlay;
	Horizon::CStaticText* _pTriangleCount;
	Horizon::CStaticText* _pVertexCount;
	Horizon::CStaticText* _pCameraMoveSpeed;
	bool _bOverlayEnabled;

	Horizon::CDialog* _popupNotification;
	Horizon::CStaticText* _notificationText;
	bool _bNotificationEnabled;
	long _notificationTime;

	//CustomizeMenu
	Horizon::CDialog* _pCustomizeWindow;
	bool _bCustomizeWindowEnabled;
	Progeny::PgPlanetAttributes _attributes;
	Progeny::PgColorScheme _colorScheme;
	void updateConfigurationFields();
	// Customize controls
	Horizon::CEditableText* _seedEditor;
	Horizon::CEditableText* _radiusEditor;
	Horizon::CSlider* _radiusSlider;
	Horizon::CEditableText* _oceanHeightEditor;
	Horizon::CSlider* _oceanHeightSlider;
	Horizon::CEditableText* _continentFreqEditor;
	Horizon::CSlider* _continentFreqSlider;
	Horizon::CEditableText* _mountainCountEditor;
	Horizon::CSlider* _mountainCountSlider;
	Horizon::CListBox* _colorList;
	Horizon::CCheckBox* _craterCheckbox;
	// Control Enum
	enum control{ SEED, RADIUS, OCEAN, CONTINENT, MOUNTAIN, COUNT};
	//Control Functions
	static bool previewPlanetClicked(Horizon::IFrame* frame, Horizon::SEvent evn, void* evndata, void* userdata);
	static bool generateCustomPlanetClicked(Horizon::IFrame* frame, Horizon::SEvent evn, void* evndata, void* userdata);
	static bool sliderEditedEvent(Horizon::IFrame* frame, Horizon::SEvent evn, void* evndata, void* userdata);
	static bool textEditedEvent(Horizon::IFrame* frame, Horizon::SEvent evn, void* evndata, void* userdata);
	static bool checkboxChangedEvent(Horizon::IFrame* frame, Horizon::SEvent evn, void* evndata, void* userdata);
	static bool randomStat(Horizon::IFrame* frame, Horizon::SEvent evn, void* evndata, void* userdata);
	static bool colorSchemeSelected(Horizon::IFrame* frame, Horizon::SEvent evn, void* evndata, void* userdata);

	void updateAttributesFromText(float val, int control);
	void updateAttributeFromSlider( float val, int control );
	float getFloatFromText( const wchar_t* text);

	Horizon::CLight* _pSunlight;
	bool _sunMovementEnabled;
	float _lightTheta; // Used for orbiting the light around the planet.
	float _lightRadius;
	float _orbitSpeed;

	SkyBox* _pSkybox;
	Progeny::PgROAMPlanet* _pPlanet;
	HorizonPgRenderer* _pPgRenderer;
	bool _bMenuEnabled;
	bool _bWireframe;
	bool _bEnableROAM;
	bool _bScreenSaverMode;
	float _inactivityTimer;
	float _planetRecreationTimer;

	int _currentColorSchemeId;

	bool _bEnableFog;

	CameraSystem* _pCameraSys;

	static float INTERFACE_SCALE;
	int _currentMenuType;

	// For updating overlay's
	int _frameCount;

	float _tolerance;
};

#endif