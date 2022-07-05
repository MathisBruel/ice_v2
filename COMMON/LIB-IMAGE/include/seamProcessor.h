#include <stdlib.h>
#include <iostream>
#include <climits>
#include <float.h>
#include <map>
#include <fstream>

#include "image.h"
#define M_PI 3.14159265358979323846

#pragma once

class SeamProcessor
{

public:

    struct Seam {
        std::vector<int> posX;
        double energy;
    };

    SeamProcessor(Image* img);
    ~SeamProcessor();

    void gaussianBlur(double sigma, int radius);
    void calculateSobel();

    void calculateEnergy();
    Seam* getLowerSeam(int leftBound, int rightBound);
    void removeFromEnergy(Seam* seam);

    Image* seamCarvingRemove(int newWidth, bool blur = false);
    Image* seamCarvingAdd(int newWidth, bool blur = false);
    Image* seamCarvingAddToLeft(int sizeWindow, int newWidth, bool blur = false);
    Image* seamCarvingAddToRight(int sizeWindow, int newWidth, bool blur = false);

    void getSeamsLeft(int* seams, int sizeWindow, int newWidth);
    void getSeamsRight(int* seams, int sizeWindow, int newWidth);

private:

    // -- for gaussian blur
    static double sigmaGauss(double x, double y, double sigma);

    // -- reference image
    Image* img;

    int width;
    int height;
    std::vector<std::vector<float>> energy;
    std::vector<std::vector<float>> datas;
};