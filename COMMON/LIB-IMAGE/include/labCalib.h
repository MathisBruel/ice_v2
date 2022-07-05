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

class LabCalib 
{

public :
    
    LabCalib();
    ~LabCalib();

    bool open(std::string filename);
    void save(std::string filename);

    Color* getBestInterpolation(Color* src);
    void applyCorrection(Image* img);

    void setFactorA(double factorA) {this->factorA = factorA;}
    void setBiasA(double biasA) {this->biasA = biasA;}
    void setFactorB(double factorB) {this->factorB = factorB;}
    void setBiasB(double biasB) {this->biasB = biasB;}

    int getNbPoints() {return nbPoints;}
    double* getDataSrcL() {return dataSrcL;}
    double* getDataDstL() {return dataDstL;}
    double getFactorA() {return factorA;}
    double getFactorB() {return factorB;}
    double getBiasA() {return biasA;}
    double getBiasB() {return biasB;}

private :

    double* dataSrcL;
    double* dataDstL;

    double factorA;
    double factorB;
    double biasA;
    double biasB;

    int nbPoints;
};