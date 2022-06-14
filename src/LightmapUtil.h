#pragma once

#include <cassert>
#include "bsplib.h"

using namespace std;

#define PROGRAM_VERSION		2

ifstream in; // This is the default stream we use
void* bspFile; // This is going to be where the full BSP file is stored
			   // it is a separate thing from the input buffer because in order to do memcpy we need to actually know where the damn thing is in memory
ofstream logFile; // This stores the log file which we output the data to!
string fileName; // The file name for our log file
string filePath; // the path to our log file

struct launchArgument
{
	string command;
	string description;
};

// this is probably not the best way to do these but in my eyes its at least better than just having all the text in the if/else which reads these things
vector<launchArgument> launchArgs = {
	launchArgument{"-h", "Show this screen (program will stop afterwards)"},
	launchArgument{"-help", "Show this screen (program will stop afterwards)"},
	launchArgument{"-usenormals", "Display face's normal coordinates instead of general directions"},
	launchArgument{"-logfile", "Set location of logfile (Default is map file location)"},
	launchArgument{"-v", "Show additional debug info"},
	launchArgument{"-verbose", "Show additional debug info"},
	launchArgument{"-qualitythreshold", "Set a custom threshold percentage for lightmap sample similarity\n                    Accepts decimals from 0-1 (Default is 0.03). It is recommended to set a percentage under 10%!"}, // spaces here are for formatting. its bad i know
};

bool verboseMode = false;

// This quality epsilon is a float because I want it to be able to be changed at runtime
// .03 is an arbitrary number, it seems to me from testing that this is probably one of the best numbers to lower false positives and false negatives.
float lightmapQualityEpsilon = 0.03f;

bool normalsForDirection = false;

// Both are true unless -ldr or -hdr is used, in which case the unneeded lightmap is set to false.
bool checkLDRLightmap = true;
bool checkHDRLightmap = true;

bool hasLDRLightmap = false;
bool hasHDRLightmap = false;

int main( int argc, char* argv[] );

template< class T >
void getLumpData( int lump, unsigned int &numObjects, T *lumpType );

void readFaces();
void helpScreen();

// ------------------------- //

dheader_t *dheader;

unsigned int numPlanes = 0;
dplane_t dplane[ MAX_MAP_PLANES ];

unsigned int numFaces = 0;
dface_t dface[ MAX_MAP_FACES ];

// I don't need these but i'm too lazy to write unique code when I can just reuse the lump reading code i already have
unsigned int numLightmapsLDR = 0;
unsigned int numLightmapsHDR = 0;

ColorRGBExp32 dlightdataLDR[ MAX_MAP_LIGHTING ];
ColorRGBExp32 dlightdataHDR[ MAX_MAP_LIGHTING ];

unsigned int numTexinfo = 0;
texinfo_t texinfo[ MAX_MAP_TEXINFO ];

unsigned int numTexdata = 0;
dtexdata_t dtexdata[ MAX_MAP_TEXDATA ];

unsigned int numTexdataStringData = 0;
char g_TexDataStringData[ MAX_MAP_TEXDATA_STRING_DATA ];

unsigned int numTexdataStringTable = 0;
int g_TexDataStringTable[ MAX_MAP_TEXDATA_STRING_TABLE ];

unsigned int numEdges = 0;
dedge_t dedges[ MAX_MAP_EDGES ];

unsigned int numSurfedges = 0;
int dsurfedges[ MAX_MAP_SURFEDGES ];

unsigned int numVertices = 0;
dvertex_t dvertexes[ MAX_MAP_VERTS ];