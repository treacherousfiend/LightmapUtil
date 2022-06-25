# LightmapUtil

[![Packaging status](https://repology.org/badge/vertical-allrepos/lightmaputil.svg)](https://repology.org/project/lightmaputil/versions)

A simple command line utility to tell you if your lightmaps are too high resolution (and maybe more soon?)

## Usage

The program can be run with `LightmapUtil.exe filename.bsp`. This generates `filename-LightmapUtilLog.txt` that provides you with details about lightmapping on that map file.

Note: map files currently must be uncompressed. If they are compressed, the `bspzip` utility included in source engine games can be used to decompress maps: `bspzip -repack filename.bsp`

### Build Requirements

* C++20

This is developed with VS v143 Platform Toolset. Other configurations may work, but are not intentionally supported.

## For Developers

### Issues

Currently MSVC does not build due to an issue I can't figure out with operator overloading.

### To-do

- Tell the user if they have lightmaps which are too low res
- Progress bar? (not really needed due to how fast the program is)
- Make the code less garbage
- Support compressed TF2 maps
- Maybe give the user a suggestion for what the lightmap res should be?

### Licensing

This repository is unlicensed -- almost all of the BSP reading related code (in bsplib.h and the other code files) is either taken directly from the [Source 2013 SDK](https://github.com/ValveSoftware/source-sdk-2013/) or heavily based on it.

I am not a lawyer this is the best I can do for a licensing section.
