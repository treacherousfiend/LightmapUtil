# LightmapUtil
 A simple command line utility to tell you if your lightmaps are too high res (and maybe more soon?)


 I'm not entirely sure the building requirements, but I use the C++20 Standard and VS v143 Platform Toolset

 ## To-do
 - Tell the user if they have lightmaps which are too low res
 - Progress bar? (not really needed due to how fast the program is)
 - Make the code less garbage
    - Get a better approximation for the center of a face, right now its based off of only 3 vertices
 - Support compressed TF2 maps
 - Maybe give the user a suggestion for what the lightmap res should be?
 ## Licensing
 This repository is unlicensed however,
 Almost all of the BSP reading related code (in bsplib.h and the other code files) is either taken directly from the [Source 2013 SDK](https://github.com/ValveSoftware/source-sdk-2013/) or heavily based on it.
 I am not a lawyer this is the best I can do for a licensing section.