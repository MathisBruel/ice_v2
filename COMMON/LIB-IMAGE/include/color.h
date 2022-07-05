#include <cstdlib>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <string>

#pragma once

#define DELTA (6.0/29.0)
#define DELTA2 ((6.0*6.0)/(29.0*29.0))
#define DELTA3 ((6.0*6.0*6.0)/(29.0*29.0*29.0))
#define XN1 (0.950489)
#define ZN1 (1.088840)
#define XN2 (0.964212)
#define ZN2 (0.825188)

class Color
{

public:
    Color();
    ~Color();

    void setRGB(double red, double green, double blue);
    void setHSL(double hue, double sat, double lum);
    void setLAB(double L, double A, double B);
    void setRGBA(double red, double green, double blue, double alpha);
    void setGA(double grey, double alpha);
    void setWhiteIntensity(double white, double intensity);

    void correctRGBLimits();
    void convertRGBToHSL();
    void convertHSLToRGB();

    void convertRGBToXYZ();
    void convertXYZToLAB();
    void convertXYZToRGB();
    void convertLABToXYZ();

    // -- setters & getters
    double getRed() {return red;}
    double getGreen() {return green;}
    double getBlue() {return blue;}
    double getAlpha() {return alpha;}
    double getGrey() {return grey;}
    double getHue() {return hue;}
    double getSat() {return sat;}
    double getLum() {return lum;}
    double getWhite() {return white;}
    double getIntensity() {return intensity;}
    double getL() {return L;}
    double getA() {return a;}
    double getB() {return b;}
    double getX() {return X;}
    double getY() {return Y;}
    double getZ() {return Z;}

    bool operator<(const Color& rhs) const;
    bool operator==(const Color& rhs) const;
    std::string toString() const;
    std::string toHSLString() const;
    std::string toLABString() const;

    Color* getThis() {return this;}

private: 

    long double red;
    long double green;
    long double blue;
    long double alpha;
    long double grey;
    long double white;
    long double intensity;

    // -- format HSL
    double hue;
    double sat;
    double lum;

    // -- format CIE XYZ
    double X;
    double Y;
    double Z;

    // -- format CIE L*a*b*
    double L;
    double a;
    double b;
};