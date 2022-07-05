#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "Poco/Path.h"
#include "Poco/File.h"
#include "Poco/Logger.h"
#include "image.h"
#include "color.h"

#pragma once

class LUT 
{

public :
    
    LUT();
    ~LUT();

    bool open(std::string filename);
    void save(std::string filename);

    double* getRed() {return red;}
    double* getGreen() {return green;}
    double* getBlue() {return blue;}
    double* getRedOnGreen() {return redOnGreen;}
    double* getBlueOnGreen() {return blueOnGreen;}
    double* getRedOnBlue() {return redOnBlue;}
    double* getGreenOnBlue() {return greenOnBlue;}
    double* getGreenOnRed() {return greenOnRed;}
    double* getBlueOnRed() {return blueOnRed;}

    double* getRedFinalPoly() {return redFinalPoly;}
    double* getGreenFinalPoly() {return greenFinalPoly;}
    double* getBlueFinalPoly() {return blueFinalPoly;}
    double* getRedOnBlueFinalPoly() {return redOnBlueFinalPoly;}
    double* getBlueOnRedFinalPoly() {return blueOnRedFinalPoly;}
    double* getGreenOnBlueFinalPoly() {return greenOnBlueFinalPoly;}
    double* getBlueOnGreenFinalPoly() {return blueOnGreenFinalPoly;}
    double* getRedOnGreenFinalPoly() {return redOnGreenFinalPoly;}
    double* getGreenOnRedFinalPoly() {return greenOnRedFinalPoly;}

    void recalculateAll();

    void precomputeAll();

    Color* getBestInterpolation(Color* src);
    void applyCorrection(Image* img);

private :

    struct __attribute__((__packed__)) ColorTemp
    {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    };

    void recalculatePolynome(double* src, double* poly);
    void setPolyFinal(double* poly, double* final);

    double* red;
    double* redPoly;
    double* redFinalPoly;

    double* green;
    double* greenPoly;
    double* greenFinalPoly;

    double* blue;
    double* bluePoly;
    double* blueFinalPoly;

    double* redOnGreen;
    double* redOnGreenPoly;
    double* redOnGreenFinalPoly;
    double* blueOnGreen;
    double* blueOnGreenPoly;
    double* blueOnGreenFinalPoly;

    double* redOnBlue;
    double* redOnBluePoly;
    double* redOnBlueFinalPoly;
    double* greenOnBlue;
    double* greenOnBluePoly;
    double* greenOnBlueFinalPoly;

    double* greenOnRed;
    double* greenOnRedPoly;
    double* greenOnRedFinalPoly;
    double* blueOnRed;
    double* blueOnRedPoly;
    double* blueOnRedFinalPoly;

    ColorTemp* precomputed;
};