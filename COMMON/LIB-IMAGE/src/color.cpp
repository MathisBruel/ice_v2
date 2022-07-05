#include "color.h"

Color::Color() {
    red = -1.0;
    green = -1.0;
    blue = -1.0;
    alpha = -1.0;
    grey = -1.0;
    hue = -1.0;
    sat = -1.0;
    lum = -1.0;
}

Color::~Color() {}

void Color::correctRGBLimits()
{
    if (red < 0.0) {
        red = 0.0;
    }
    else if (red > 1.0) {
        red = 1.0;
    }

    if (green < 0.0) {
        green = 0.0;
    }
    else if (green > 1.0) {
        green = 1.0;
    }

    if (blue < 0.0) {
        blue = 0.0;
    }
    else if (blue > 1.0) {
        blue = 1.0;
    }
}

void Color::setRGB(double red, double green, double blue)
{
    this->red = red;
    this->green = green;
    this->blue = blue;
}
void Color::setHSL(double hue, double sat, double lum)
{
    this->hue = hue;
    this->sat = sat;
    this->lum = lum;
}
void Color::setLAB(double L, double A, double B)
{
    this->L = L;
    this->a = A;
    this->b = B;
}
void Color::setRGBA(double red, double green, double blue, double alpha)
{
    setRGB(red, green, blue);
    this->alpha = alpha;
}
void Color::setGA(double grey, double alpha)
{
    this->grey = grey;
    this->alpha = alpha;
}

void Color::setWhiteIntensity(double white, double intensity)
{
    this->white = white;
    this->intensity = intensity;
}

void Color::convertRGBToHSL()
{
    double cmax = std::fmax(std::fmax(red, green), blue);
    double cmin = std::fmin(std::fmin(red, green), blue);
    double delta = (cmax - cmin);
    lum = (cmax + cmin)/2;

    if (delta != 0.0) {

        if (lum <= 0.5) {
            sat = delta / (cmax + cmin);
        }
        else {
            sat = delta/(2-cmax-cmin);
        }

        if (cmax == red) {
            hue = (green - blue)/delta;
        }
        else if (cmax == green) {
            hue = ((blue - red) / delta) + 2;
        }
        else if (cmax == blue) {
            hue = ((red - green) / delta) + 4;
        }
        if (hue < 0) {hue += 6;}
        else if (hue >= 6) {hue -= 6;}
    }
    else {
        hue = 0.0;
        sat = 0.0;
    }

    if (hue > 6) {hue -= 6;}
    else if (hue < 0) {hue += 6;}
}
void Color::convertHSLToRGB() 
{
    double C = (1 - std::fabs(2*lum - 1)) * sat;
    double X = C * (1 - std::fabs(std::fmod(hue, 2) - 1));
    double m = lum - C/2;

    if (hue >= 0 && hue < 1) {
        red = C + m;
        green = X + m;
        blue = m;
    }
    else if (hue < 2) {
        red = X + m;
        green = C + m;
        blue = m;
    }
    else if (hue < 3) {
        red = m;
        green = C + m;
        blue = X + m;
    }
    else if (hue < 4) {
        red = m;
        green = X + m;
        blue = C + m;
    }
    else if (hue < 5) {
        blue = C + m;
        red = X + m;
        green = m;
    }
    else {
        blue = X + m;
        red = C + m;
        green = m;
    }
}

void Color::convertRGBToXYZ()
{
    double Xp = (red <= 0.04045) ? red/12.92 : std::pow((red+0.055)/1.055, 2.4);
    double Yp = (green <= 0.04045) ? green/12.92 : std::pow((green+0.055)/1.055, 2.4);
    double Zp = (blue <= 0.04045) ? blue/12.92 : std::pow((blue+0.055)/1.055, 2.4);

    X = 0.4124564*Xp + 0.3575761*Yp + 0.1804375*Zp;
    Y = 0.2126729*Xp + 0.7151522*Yp + 0.072175*Zp;
    Z = 0.0193339*Xp + 0.119192*Yp + 0.9503041*Zp;
}
void Color::convertXYZToLAB()
{
    double XXn = X/XN1;
    double ZZn = Z/ZN1;

    double fXXn = (XXn > DELTA3) ? std::pow(XXn, 1.0/3.0) : ((XXn/(3*DELTA2))+4.0/29.0);
    double fYYn = (Y > DELTA3) ? std::pow(Y, 1.0/3.0) : ((Y/(3*DELTA2))+4.0/29.0);
    double fZZn = (ZZn > DELTA3) ? std::pow(ZZn, 1.0/3.0) : ((ZZn/(3*DELTA2))+4.0/29.0);

    L = 116.0*fYYn - 16;
    a = 500 * (fXXn - fYYn);
    b = 200 * (fYYn - fZZn);
}

void Color::convertXYZToRGB()
{
    double Xp = 3.2404542*X - 1.5371385*Y - 0.4985314*Z;
    double Yp = -0.9692660*X + 1.8760108*Y + 0.0415560*Z;
    double Zp = 0.0556434*X - 0.2040259*Y + 1.0572252*Z;

    red = (Xp <= 0.0031308) ? Xp*12.92 : 1.055*std::pow(Xp, 1/2.4)-0.055;
    green = (Yp <= 0.0031308) ? Yp*12.92 : 1.055*std::pow(Yp, 1/2.4)-0.055;
    blue = (Zp <= 0.0031308) ? Zp*12.92 : 1.055*std::pow(Zp, 1/2.4)-0.055;
}
void Color::convertLABToXYZ()
{
    double tX = (L+16.0)/116.0 + a/500.0;
    double tY = (L+16.0)/116.0;
    double tZ = (L+16.0)/116.0 - b/200.0;

    double ftX = (tX > DELTA) ? std::pow(tX, 3) : 3*DELTA2*(tX-(4.0/29.0));
    double ftY = (tY > DELTA) ? std::pow(tY, 3) : 3*DELTA2*(tY-(4.0/29.0));
    double ftZ = (tZ > DELTA) ? std::pow(tZ, 3) : 3*DELTA2*(tZ-(4.0/29.0));

    X = XN1 * ftX;
    Y = ftY;
    Z = ZN1 * ftZ;
}

std::string Color::toString() const
{
    return "(" + std::to_string(red*255.0) + " | " + std::to_string(green*255.0) + " | " + std::to_string(blue*255.0) + ")";
}
std::string Color::toHSLString() const
{
    return "(" + std::to_string(hue*60.0) + " | " + std::to_string(sat*100.0) + " | " + std::to_string(lum*100.0) + ")";
}

std::string Color::toLABString() const
{
    return "(" + std::to_string(L) + " | " + std::to_string(a) + " | " + std::to_string(b) + ")";
}

bool Color::operator<(const Color& rhs) const
{
    if (blue < rhs.blue) {
        return true;
    } else if (blue == rhs.blue) {
        if (green < rhs.green) {
            return true;
        } else if (green == rhs.green) {
            if (red < rhs.red) {
                return true;
            }
        }
    }
}
bool Color::operator==(const Color& rhs) const
{
    return (red == rhs.red && green == rhs.green && blue == rhs.blue);
}