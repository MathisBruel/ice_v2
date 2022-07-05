#pragma once
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <iostream>
#include <math.h>
#include <stdio.h>

#define SEAMLIBRARY_API __declspec(dllexport)

extern "C" SEAMLIBRARY_API void seamResizeVideo(int widthSrc, int heightSrc, int widthDst, int nbImages, unsigned char* dataIn, unsigned char* dataOut);