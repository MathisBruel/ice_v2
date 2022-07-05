#pragma once
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <iostream>
#include <math.h>
#include <stdio.h>

#define SEAMLIBRARY_API __declspec(dllexport)

extern "C" SEAMLIBRARY_API void addResizeVideoFirst(int widthSrc, int heightSrc, int widthDst, unsigned char* dataIn, unsigned char* dataOut, int* seamsOutLeft, int* seamsOutRight);
extern "C" SEAMLIBRARY_API void addResizeVideoOther(int widthSrc, int heightSrc, int widthDst, unsigned char* dataIn, unsigned char* dataOut, int* seamsInLeft, int* seamsInRight);

extern "C" SEAMLIBRARY_API void addResizeVideoMotionFirst(int widthSrc, int heightSrc, int widthDst, unsigned char* dataIn, unsigned char* dataOut, int* seamsOutLeft, int* seamsOutRight);
extern "C" SEAMLIBRARY_API void addResizeVideoMotionOther(int widthSrc, int heightSrc, int widthDst, unsigned char* dataIn, unsigned char* dataOut, int* seamsInOutLeft, int* seamsInOutRight);

extern "C" SEAMLIBRARY_API void addResizeVideoDiffFirst(int widthSrc, int heightSrc, int widthDst, unsigned char* dataIn, unsigned char* dataOut, float* sobelOutLeft, float* sobelOutRight);
extern "C" SEAMLIBRARY_API void addResizeVideoDiffOther(int widthSrc, int heightSrc, int widthDst, unsigned char* dataIn, unsigned char* dataOut, float* sobelInOutLeft, float* sobelInOutRight);