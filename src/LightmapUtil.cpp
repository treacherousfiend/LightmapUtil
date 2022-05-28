#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <filesystem>
#include <cstring>

#include "LightmapUtil.h"

using namespace std;

int main( int argc, char *argv[] )
{

	cout << "LightmapUtil v" << PROGRAM_VERSION << "\n";
	
	if (argc == 1)
	{
		helpScreen();
		return 1;
	}

	// open up our file if it exists, if it doesn't, don't stop here, we still need to show the help screen
	bool doesFileExist = filesystem::exists( argv[ 1 ] );
	if (doesFileExist)
	{
		in.open( argv[ 1 ], ios::binary );
	}

	fileName = filesystem::path( argv[ 1 ] ).filename().stem().string() + "-LightmapUtilLog.txt";

	for (int i = 1; i < argc; i++)
	{

		// -h OR -help
		if (argv[ i ] == launchArgs[ 0 ].command || argv[ i ] == launchArgs[ 1 ].command)
		{
			helpScreen();
			return 1;
		}
		// -usenormals
		else if ( argv[ i ] == launchArgs[ 2 ].command )
		{
			normalsForDirection = true;
		}
		// -logfile
		else if (argv[ i ] == launchArgs[ 3 ].command)
		{
			filePath = filesystem::path(argv[ i + 1 ]).string();
			if (!filesystem::path( filePath ).has_relative_path())
			{
				cerr << "Logfile path is invalid!\n";
				return 4;
			}
		}
		// -v OR -verbose
		else if (argv[ i ] == launchArgs[ 4 ].command || argv[ i ] == launchArgs[ 5 ].command)
		{
			verboseMode = true;
		}
		// -qualitythreshold
		else if (argv[ i ] == launchArgs[ 6 ].command)
		{
			if (!argv[ i + 1 ])
			{
				cerr << "No value input after -qualitythreshold!\n";
				return 5;
			}

			lightmapQualityEpsilon = strtof(argv[ i + 1 ], NULL);
			// crappily clamap lightmapQualityEpsilon to 0-1
			if (lightmapQualityEpsilon > 1.0f)
			{
				lightmapQualityEpsilon = 1.0f;
			}
			else if (lightmapQualityEpsilon <= 0.0f)
			{
				lightmapQualityEpsilon = 0.1f;
				cout << "qualitythreshold was less than or equal to 0 and was clamped to 0.1\nThis may have happened because you entered something that wasn't a decimal!\n";
			}
		}
	}
	
	// if our file isn't open (but -h is not part of the launch args), error out
	if ( !in.is_open() )
	{
		cerr << "Couldn't find input file! Check if you spelled it correctly!\n";
		return 2;
	}

	// if logFile is not already set, set it to default
	if (filePath.empty())
	{
		filePath = filesystem::path( argv[ 1 ] ).remove_filename().string();
	}
	filePath = filePath + fileName;

	logFile = ofstream( filePath, ofstream::out | ofstream::trunc );
	logFile << "Lightmap \'Too Big\' threshold is " << lightmapQualityEpsilon << "\n";

	// Get the length of the file
	in.seekg( 0, in.end );
	int fileLength = in.tellg();
	in.seekg( 0, in.beg );
	
	// This is taken directly from Valve's code
	bspFile = malloc( fileLength + 1 );
	// i'm not entirely sure what this does? I think it just makes sure that all the memory is filled with blank data?
	// and i think the only reason they recast it as a char here is because its easier to use malloc beforehand.
	( ( char* ) bspFile )[ fileLength ] = 0;

	in.read( (char*)bspFile, fileLength );
	// we've read the BSP file into memory, we can close the stream now
	in.close();

	// copy the header
	// kinda bad because i hardcode the headersize, but oh well.
	dheader = (dheader_t*)malloc(1036);
	memcpy( dheader, ( unsigned char* ) bspFile, 1036 );

	if (dheader->ident != IDBSPHEADER)
	{
		cerr << "Not a BSP file! If you input a BSP file, there may be something wrong with it!\n";
		if (verboseMode)
		{
			cout << "DEBUG: BSP ident was " << dheader->ident << "\n";
		}
		return 3;
	}

	if (dheader->version < 17)
	{
		cerr << "BSP file version is under 17, this map might use a different format!\n" << "Continuing anyway in case it still works!\n";
	}
	else if (dheader->version > 29)
	{
		cerr << "BSP file version is over 29, this map might use a different format!\n" << "Continuing anyway in case it still works!\n";
	}

	// Grab the offsets and data we need
	getLumpData( LUMP_PLANES, numPlanes, dplane );
	
	// this is kinda stupid because numLightmaps isn't helpful, but its easier than writing a new function just for reading lightmap lumps
	getLumpData( LUMP_LIGHTING, numLightmapsLDR, dlightdataLDR );
	getLumpData( LUMP_LIGHTING_HDR, numLightmapsHDR, dlightdataHDR);
	
	getLumpData( LUMP_FACES, numFaces, dface );
	getLumpData( LUMP_TEXINFO, numTexinfo, texinfo );
	getLumpData( LUMP_TEXDATA, numTexdata, dtexdata );
	getLumpData( LUMP_TEXDATA_STRING_TABLE, numTexdataStringTable, g_TexDataStringTable );
	getLumpData( LUMP_TEXDATA_STRING_DATA, numTexdataStringData, g_TexDataStringData );
	getLumpData( LUMP_EDGES, numEdges, dedges );
	getLumpData( LUMP_SURFEDGES, numSurfedges, dsurfedges );
	getLumpData( LUMP_VERTEXES, numVertices, dvertexes );

	cout << "Copied all lumps! Checking lightmaps...\n";

	readFaces();

	cout << "Finished reading lightmaps, check the log file (" << fileName << ") to see the results!\n";
	
	return 0;
}


template< class T >
void getLumpData( int lump, unsigned int &numObjects, T *lumpType )
{
	// grab our offset and length from the header
	unsigned int lumpOffset = dheader->lumps[ lump ].fileofs;
	unsigned int lumpLength = dheader->lumps[ lump ].filelen;

	numObjects = lumpLength / sizeof( T );

	memcpy( lumpType, (unsigned char*) bspFile + lumpOffset, lumpLength);
	if (verboseMode)
	{
		cout << "Copied lump " << lump << "!\n";
	}
}

void readFaces()
{
	for (unsigned int i = 0; i < numFaces; i++)
	{
		dface_t curFace = dface[ i ];
		if ( curFace.lightofs != -1 && curFace.texinfo  != -1 )
		{
			// hack?? offset is divided by 4 because we store each lightmap sample in a 4 byte struct.
			int lightmapOffset = curFace.lightofs / 4;
			int lightmapSize = ( curFace.m_LightmapTextureSizeInLuxels[ 0 ] + 1 ) * ( curFace.m_LightmapTextureSizeInLuxels[ 1 ] + 1 );

			// these are in terms of LIGHTMAP RESOLUTION, not LUXEL SIZE.
			//bool lightmapTooSmall; // Is our lightmap small enough that we're losing out on potential detail? (NOT IMPLEMENTED YET)
			bool lightmapTooBig = false; // is our lightmap big enough that we're wasting precious precious disk space

			// We check through every single sample of a face
			// probably could do some interesting optimizations where we do every other and extrapolate info, but this doesn't right now
			for (int lightmapSample1 = 0; lightmapSample1 < lightmapSize && lightmapTooBig != true; lightmapSample1++)
			{
				// first neighbor lightmap sample is out here because the value needed to be initialized. i am dumb and forgot that.
				int lightmapSample2 = lightmapSample1 - 1;
				for (int j = 0; j < 4; ++j)
				{
					switch (j)
					{
					case 1:
						lightmapSample2 = lightmapSample1 + 1;
						break;
					case 2:
						lightmapSample2 = lightmapSample1 - ( curFace.m_LightmapTextureSizeInLuxels[ 0 ] + 1 );
						break;
					case 3:
						lightmapSample2 = lightmapSample1 + ( curFace.m_LightmapTextureSizeInLuxels[ 0 ] + 1 );
						break;
					}

					if (lightmapSample2 >= 0 && lightmapSample2 < lightmapSize)
					{
						// this is a mess and i hate it, essentially the RGB values they use in game are the raw color values multiplied by 2^exponent
						// I don't know why, but the exponent is a *signed* char, and they add 128 to it
						// like, because they add 128, the max value is 256 and the min is 1. so its just an unsigned char but stupid.
						char sample1E = dlightdataLDR[ lightmapSample1 ].exponent;
						float sample1R = dlightdataLDR[ lightmapSample1 ].r * 2 ^ ( (int)sample1E + 128 );
						float sample1G = dlightdataLDR[ lightmapSample1 ].g * 2 ^ ( (int)sample1E + 128 );
						float sample1B = dlightdataLDR[ lightmapSample1 ].b * 2 ^ ( (int)sample1E + 128 );
						char sample2E = dlightdataLDR[ lightmapSample2 ].exponent;
						float sample2R = dlightdataLDR[ lightmapSample2 ].r * 2 ^ ( (int)sample2E + 128 );
						float sample2G = dlightdataLDR[ lightmapSample2 ].g * 2 ^ ( (int)sample2E + 128 );
						float sample2B = dlightdataLDR[ lightmapSample2 ].b * 2 ^ ( (int)sample2E + 128 );

						// not a big fan of this, but it used to be worse.
						if (abs( sample1R - sample2R ) < lightmapQualityEpsilon
							&& abs( sample1G - sample2G ) < lightmapQualityEpsilon
							&& abs( sample1B - sample2B ) < lightmapQualityEpsilon)
						{
							lightmapTooBig = true;
						}
					}
				}
			}

			if (lightmapTooBig == true)
			{
				int FaceID = i;
				int texdatastringoffset = g_TexDataStringTable[dtexdata[ texinfo[ curFace.texinfo ].texdata ].nameStringTableID];
				string Texture = &g_TexDataStringData[ texdatastringoffset ];
				string LightmapIssue = "big"; // right now this is hardcoded to be big, once we support lightmaps that are too small, it'll be changed based on the bools above
				
				// This is how we get the coords of the face
				// its not the exact center, we only use 3 verts because its quicker and easier and i don't care enough right now
				dvertex_t approxCenter;
				approxCenter.vertX = 0;
				approxCenter.vertY = 0;
				approxCenter.vertZ = 0;
				// because all of the edges except the last one will share 1 vert with the previous edge, we can assume that numVerts is numedges - 1
				// the last edge doesn't have any new verts, since its the last vert of the previous edge and the first vert of the first edge
				int numVerts = curFace.numedges - 1;
				int curEdge = dsurfedges[curFace.firstedge];
				bool isNegative = false;
				if (curEdge < 0)
				{
					isNegative = true;
					curEdge = abs( curEdge );
				}
				// 512 is a magic number, but the chances of a single face having 512, let alone 128 verts is ridiculous
				// but its here because (simple) arrays like this cannot have sizes defined at runtime.
				// maybe sometime i'll go back and make this actually good.
				// but until then, this code will work until some crazy person somehow makes a face with 513 verts.
				int vertArray[ 512 ];
				vertArray[ 0 ] = dedges[curEdge].v[ 0 ];
				
				// potentially I can have the isNegative check outside of this for loop and save a little bit of time.
				// just haven't done it (yet)
				for (int i = 1; i < numVerts; i++)
				{
					if (isNegative)
					{
						curEdge -= 1;
					}
					else
					{
						curEdge += 1;
					}

					if (vertArray[ i - 1 ] != dedges[ curEdge ].v[ 0 ])
					{
						vertArray[ i ] = dedges[ curEdge ].v[ 0 ];
					}
					else if (vertArray[ i - 1 ] != dedges[ curEdge ].v[ 1 ])
					{
						vertArray[ i ] = dedges[ curEdge ].v[ 1 ];
					}
					// if both of these fail its ok, that just means we're at the end of our list!
				}

				// Get the approximate center of our face, its approximate because we only use 3 verts, i tried using all of them but couldn't figure it out
				approxCenter.vertX = dvertexes[ vertArray[ 1 ] ].vertX - ( ( ( dvertexes[ vertArray[ 1 ] ].vertX - dvertexes[ vertArray[ 0 ] ].vertX ) + ( dvertexes[ vertArray[ 1 ] ].vertX - dvertexes[ vertArray[ 2 ] ].vertX ) ) / 2 );
				approxCenter.vertY = dvertexes[ vertArray[ 1 ] ].vertY - ( ( ( dvertexes[ vertArray[ 1 ] ].vertY - dvertexes[ vertArray[ 0 ] ].vertY ) + ( dvertexes[ vertArray[ 1 ] ].vertY - dvertexes[ vertArray[ 2 ] ].vertY ) ) / 2 );
				approxCenter.vertZ = dvertexes[ vertArray[ 1 ] ].vertZ - ( ( ( dvertexes[ vertArray[ 1 ] ].vertZ - dvertexes[ vertArray[ 0 ] ].vertZ ) + ( dvertexes[ vertArray[ 1 ] ].vertZ - dvertexes[ vertArray[ 2 ] ].vertZ ) ) / 2 );

				string direction = "UNKNOWN";
				dplane_t curPlane = dplane[ curFace.planenum ];

				if (normalsForDirection == true)
				{
					direction = ( std::to_string(curPlane.normalX) + " " + std::to_string( curPlane.normalY ) + " " + std::to_string( curPlane.normalZ ));
				}
				else
				{
					switch (curPlane.type)
					{
					case 0:
						if (curPlane.normalX < 0)
						{
							direction = "west";
						}
						else
						{
							direction = "east";
						}
					case 1:
						if (curPlane.normalY < 0)
						{
							direction = "south";
						}
						else
						{
							direction = "north";
						}
					case 2:
						if (curPlane.normalZ < 0)
						{
							direction = "up";
						}
						else
						{
							direction = "down";
						}
					case 3:
						if (curPlane.normalX < 0)
						{
							direction = "mostly west";
						}
						else
						{
							direction = "mostly east";
						}
					case 4:
						if (curPlane.normalY < 0)
						{
							direction = "mostly south";
						}
						else
						{
							direction = "mostly north";
						}
					case 5:
						if (curPlane.normalZ < 0)
						{
							direction = "mostly up";
						}
						else
						{
							direction = "mostly down";
						}
					}
				}

				if (Texture == "")
				{
					Texture = "UNKNOWN";
				}

				logFile << "Face " << FaceID << " (" << approxCenter.vertX << " " << approxCenter.vertY << " " << approxCenter.vertZ << ") facing " << direction << " with texture " << Texture << " has " << LightmapIssue << " lightmap!\n";

				// if -verbose is used, also output to the console
				if (verboseMode)
				{
					cout << "Face " << FaceID << " (" << approxCenter.vertX << " " << approxCenter.vertY << " " << approxCenter.vertZ << ") facing " << direction << " with texture " << Texture << " has " << LightmapIssue << " lightmap!\n";
				}
			}
		}
	}
}

void helpScreen()
{
	cout << "Usage: LightmapUtil [BSP_FILE] [LAUNCH_ARGUMENTS]\n\n" << "Available options:\n";
	for (int i = 0; i < launchArgs.size(); i++)
	{
		string commandMessage;

		// if -help or -verbose (the alt commands), just skip the rest of the commands, we don't need to display these.
		if (i != 1 && i != 5)
		{
			commandMessage.append( launchArgs[ i ].command );
			int spaces = 20 - launchArgs[ i ].command.length();

			// if -h or -v, also show the alt command for it
			if (i == 0 || i == 4)
			{
				commandMessage.append( " / " );
				commandMessage.append( launchArgs[ i + 1 ].command );
				// poorly remove the extra spaces caused by the extra command and characters
				spaces -= launchArgs[ i + 1 ].command.length() + 3;
			}

			commandMessage.append( ( size_t ) spaces, ' ' );
			commandMessage.append( launchArgs[ i ].description + "\n" );
			cout << commandMessage;
		}
	}
}
