#include <stdlib.h>
#include <iostream>
#include <fstream>

#include "Poco/Path.h"
#include "Poco/Logger.h"
#include "Poco/File.h"
#include "color.h"

#pragma once

class Calibrer 
{

public :
    
    Calibrer();
    ~Calibrer();

    bool open(std::string filename);
    Color* applyCorrection(Color* src);
    Color* applyInverseCorrection(Color* src);

private :

    Color* applyFixedPointCorrection(Color* src);
    Color* applyWhiteBalanceCorrection(Color* src);
    Color* applyInverseFixedPointCorrection(Color* src);
    Color* applyInverseWhiteBalanceCorrection(Color* src);

    void recalculateAllPoly();

    // -- is fixed factor or RGBW correction
    bool isFixedPoint;

    // -- for fixed point correction
    double factorRed;
    double factorGreen;
    double factorBlue;

    // -- for white balance correction
    double* indexWhite;
    double* redWhite;
    double* greenWhite;
    double* blueWhite;

    // -- polynome for calculation (size 2 : c = 0)
    double* polyRed;
    double* polyGreen;
    double* polyBlue;
};