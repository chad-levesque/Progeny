#ifndef __PROGENYDEMO_DEMOCONSTANTS_H__
#define __PROGENYDEMO_DEMOCONSTANTS_H__

#include "DemoTypes.h"

#define FULLSCREEN false
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 800

#define DEFAULT_TOLERANCE 0.035f
#define SCREENSAVER_TIME 30.0f
#define SCREENSAVER_REGENERATE_TIME 10.0f
#define SCREENSAVER_CAMERA_OFFSET  0.34906585f
#define SCREENSAVER_CAMERA_RADIUS_MULTIPLIER 3.0f
#define SCREENSAVER_CAMERA_MAX_TILT 0.5f

//gamepad controls:
#define GAMEPAD_START GamepadState::BUTTON_7
#define GAMEPAD_RANDOMPLANET GamepadState::BUTTON_6
#define GAMEPAD_TOGGLEWIREFRAME GamepadState::BUTTON_2
#define GAMEPAD_CHANGECAMERA GamepadState::BUTTON_0
#define GAMEPAD_FREELOOK_UP GamepadState::BUTTON_5
#define GAMEPAD_FREELOOK_DOWN GamepadState::BUTTON_4
#define GAMEPAD_DEADZONE_CUTOFFF 0.1f

static const float PLANET_RADIUS = 6371.0f; // default planet radius
static const SPHERECOORDS INITAL_CAMERA_POS(PLANET_RADIUS * 4, 90.0f, 0.0f); // Note: Initial Camera Pos is currently set in the view by positionCameraBasedOnLightSource method, not this value

// Environment parameters
static float SUN_SPEED = 0.08f;	// default rotational speed of the sun
static float SUN_SIZE = 30000000.0f; // size of the sun object
static float ORBITAL_RADIUS = 150000000.0f; // distance to the sun
static float SKYBOX_SIZE = 500000000.0f; // length/width/height of the skybox

// Camera parameters
static const float NEAR_CLIPPING_DISTANCE = 10.0f;
static const float FAR_CLIPPING_DISTANCE = 400000000.0f;

//Currently this is only a boolean, eventually we want this to be an adjustable frequency or depth.
static const bool USE_CRATERS = true;

//Defines the planet's seed value, you may replace this with an integer for testing purposes.
#define PLANET_SEED (unsigned)clock()

//These Default Parameters are designed to generate a geography,
//Statistically similar to that of the planet earth.
static const float NOISE_AMPLITUDE = 0.025f;

//Low frequency generates a Pangaea type planet, high generates many islands
static const float CONTINENT_FREQUENCY = 0.3f;  // safezone: 0-1
//Controls the land to water ratio
static const float OCEAN_HEIGHT = 0.7f;       // safezone: 0-1
// Controls the default height of snow caps
static const float SNOW_HEIGHT = 0.97f;

//Controls that amount of mountains that the planet's islands contain.
static const float MOUNTAIN_COUNT = 0.4f;     // safezone: 0-1

// TODO parameterize these to be specific to planet instance
static const float MAXIMUM_ATMOSPHERE = 2500.0f; // in 1000's km's
static const float MINIMUM_ATMOSPHERE = 1250.0f; // ""
static const float MAXIMUM_DAYTIME_ANGLE = 120.0f;
static const float MINIMUM_DAYTIME_ANGLE = 95.0f;
static const float ALTITUDE_SKY_COEFFICIENT = 0.5f;
static const float DAYTIME_SKY_COEFFICIENT = 0.5f;
static const int DEFAULT_SKY_COLOR = 0xFF71B8EB;


// View constants
static const float STATS_POSITION_X = 0.0f;
static const float STATS_POSITION_Y = 0.92f;
static const float STATS_WIDTH = 2.5f;
static const float STATS_HEIGHT = 0.22f;

static const float NOTIFICATION_POSITION_X = 0.0f;
static const float NOTIFICATION_POSITION_Y = -0.92f;
static const float NOTIFICATION_WIDTH = 2.5f;
static const float NOTIFICATION_HEIGHT = 0.22f;

static const float MAIN_MENU_POSITION_X = 0.0f;
static const float MAIN_MENU_POSITION_Y = 0.0f;
static const float MAIN_MENU_WIDTH = 0.7f;
static const float MAIN_MENU_HEIGHT = 0.75f;

static const float CUSTOMIZATION_WINDOW_POSITION_X = 0.0f;
static const float CUSTOMIZATION_WINDOW_POSITION_Y = 0.0f;
static const float CUSTOMIZATION_WINDOW_WIDTH = 2.5f;
static const float CUSTOMIZATION_WINDOW_HEIGHT = 1.8f;

#endif // __PROGENYDEMO_DEMOCONSTANTS_H__