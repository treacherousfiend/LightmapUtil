cmake_minimum_required(VERSION 3.10)

# set the project name
project(LightmapUtil)

# C++ Standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(lightmaputil src/LightmapUtil.cpp)

# Install Location
install (TARGETS lightmaputil DESTINATION bin)
