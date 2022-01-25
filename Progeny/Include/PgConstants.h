#ifndef __PGCONSTANTS_H__
#define __PGCONSTANTS_H__

#define MAX_NUMBER_VERTICES 65535
#define MAX_NUMBER_TRIANGLES 20000
#define OUTPUT_NOISES_MODULES_TO_FILE 0 // 0 off, 1 on

#define EPSILON 1.0E-06f

static const float MINIMUM_PLANET_RADIUS = 2000.0f; // km
static const float MAXIMUM_PLANET_RADIUS = 10000.0f; // km

//------Planet Parameters--------------------
static const float OCEAN_CLIFF_HEIGHT = 0.2f; //The hardness of the boundaries between continents and the ocean.

static const long MAX_MERGE_TIME = 60L;
static const long MAX_SPLIT_TIME = 60L;

#endif // __PGCONSTANTS_H__