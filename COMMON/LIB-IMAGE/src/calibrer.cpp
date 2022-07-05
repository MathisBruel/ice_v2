#include "calibrer.h"

Calibrer::Calibrer()
{
    isFixedPoint = false;
}

Calibrer::~Calibrer() {}

bool Calibrer::open(std::string filename)
{
    Poco::File calibFile(filename);

    // -- check file exists
    if (!calibFile.exists()) {
        Poco::Logger::get("Calibrer").error("Calib file " + filename + " does not exists.", __FILE__, __LINE__);
        return false;
    }

    // -- read text file line by line 
    std::ifstream ifs;
    ifs.open(filename, std::ifstream::in);
    std::string colorText;

    getline(ifs, colorText);
    if (colorText.find("FIXED") != std::string::npos) {
        getline(ifs, colorText);
        int posSpace1 = colorText.find(" ", 0);
        int posSpace2 = colorText.find(" ", posSpace1+1);

        factorRed = std::stod(colorText.substr(0, posSpace1)) / 255.0;
        factorGreen = std::stod(colorText.substr(posSpace1+1, posSpace2-posSpace1)) / 255.0;
        factorBlue = std::stod(colorText.substr(posSpace2+1)) / 255.0;

        isFixedPoint = true;
    }
    
    // -- CALIB WHITE
    else {

        isFixedPoint = true;
        indexWhite = new double[2];
        redWhite = new double[2];
        greenWhite = new double[2];
        blueWhite = new double[2];

        int idx = 0;

        // -- parse all color lines
        while(getline(ifs, colorText)) {
            int posSpace1 = colorText.find(" ", 0);
            int posSpace2 = colorText.find(" ", posSpace1+1);
            int posSpace3 = colorText.find(" ", posSpace2+1);

            double white = std::stoi(colorText.substr(0, posSpace1));
            double red = std::stod(colorText.substr(posSpace1+1, posSpace2-posSpace1));
            double green = std::stod(colorText.substr(posSpace2+1, posSpace3-posSpace2));
            double blue = std::stod(colorText.substr(posSpace3+1));

            redWhite[idx] = red;
            greenWhite[idx] = green;
            blueWhite[idx] = blue;
            indexWhite[idx] = white;
            idx++;
        }

        recalculateAllPoly();
    }

    ifs.close();

    return true;
}

void Calibrer::recalculateAllPoly()
{
    // -- determine polynome for red
    double GRed = (indexWhite[0]*indexWhite[0] - (redWhite[0]/redWhite[1])*indexWhite[1]*indexWhite[1]) / ((redWhite[0]/redWhite[1])*indexWhite[1] - indexWhite[0]);
    polyRed = new double[2];
    polyRed[0] = redWhite[0] / (indexWhite[0]*indexWhite[0] + indexWhite[0]*GRed);
    polyRed[1] = polyRed[0] * GRed;

    // -- determine polynome for green
    double GGreen = (indexWhite[0]*indexWhite[0] - (greenWhite[0]/greenWhite[1])*indexWhite[1]*indexWhite[1]) / ((greenWhite[0]/greenWhite[1])*indexWhite[1] - indexWhite[0]);
    polyGreen = new double[2];
    polyGreen[0] = greenWhite[0] / (indexWhite[0]*indexWhite[0] + indexWhite[0]*GGreen);
    polyGreen[1] = polyGreen[0] * GGreen;

    // -- determine polynome for blue
    double GBlue = (indexWhite[0]*indexWhite[0] - (blueWhite[0]/blueWhite[1])*indexWhite[1]*indexWhite[1]) / ((blueWhite[0]/blueWhite[1])*indexWhite[1] - indexWhite[0]);
    polyBlue = new double[2];
    polyBlue[0] = blueWhite[0] / (indexWhite[0]*indexWhite[0] + indexWhite[0]*GBlue);
    polyBlue[1] = polyBlue[0] * GBlue;
}

Color* Calibrer::applyInverseCorrection(Color* src)
{
    if (isFixedPoint) {
        Poco::Logger::get("Calibrer").debug("Apply reverse fixed point correction !", __FILE__, __LINE__);
        return applyInverseFixedPointCorrection(src);
    }
    else {
        Poco::Logger::get("Calibrer").debug("Apply reverse white balance correction !", __FILE__, __LINE__);
        return applyInverseWhiteBalanceCorrection(src);
    }
}
Color* Calibrer::applyInverseFixedPointCorrection(Color* src)
{
    Color* dst = new Color();
    double red = src->getRed()/factorRed;
    double green = src->getGreen()/factorGreen;
    double blue = src->getBlue()/factorBlue;
    dst->setRGB(red, green, blue);

    Poco::Logger::get("Calibrer").debug("Apply reverse fixed point correction : " + src->toString() + " -> " + dst->toString(), __FILE__, __LINE__);
    return dst;
}
Color* Calibrer::applyInverseWhiteBalanceCorrection(Color* src)
{
    Color* dst = new Color();
    double redRef = src->getRed()*255.0;
    double greenRef = src->getGreen()*255.0;
    double blueRef = src->getBlue()*255.0;

    Poco::Logger::get("Calibrer").debug("1 !", __FILE__, __LINE__);
    
    // -- calculate RED
    double deltaRed = polyRed[1]*polyRed[1] + 4.0*redRef*polyRed[0];
    double redFinal = (std::sqrt(deltaRed) - polyRed[1])/(2*polyRed[0]);

    Poco::Logger::get("Calibrer").debug("2 !", __FILE__, __LINE__);

    // -- calculate GREEN
    double deltaGreen = polyGreen[1]*polyGreen[1] + 4.0*greenRef*polyGreen[0];
    double greenFinal = (std::sqrt(deltaGreen) - polyGreen[1])/(2*polyGreen[0]);

    Poco::Logger::get("Calibrer").debug("3 !", __FILE__, __LINE__);

    // -- calculate BLUE
    double deltaBlue = polyBlue[1]*polyBlue[1] + 4.0*blueRef*polyBlue[0];
    double blueFinal = (std::sqrt(deltaBlue) - polyBlue[1])/(2*polyBlue[0]);

    dst->setRGB(redFinal/255.0, greenFinal/255.0, blueFinal/255.0);

    Poco::Logger::get("Calibrer").debug("Apply reverse white balance correction : " + src->toString() + " -> " + dst->toString(), __FILE__, __LINE__);
    return dst;
}

Color* Calibrer::applyCorrection(Color* src)
{
    if (isFixedPoint) {
        return applyFixedPointCorrection(src);
    }
    else {
        return applyWhiteBalanceCorrection(src);
    }
}
Color* Calibrer::applyFixedPointCorrection(Color* src)
{
    Color* dst = new Color();
    double red = src->getRed()*factorRed;
    double green = src->getGreen()*factorGreen;
    double blue = src->getBlue()*factorBlue;
    dst->setRGBA(red, green, blue, src->getAlpha());

    Poco::Logger::get("Calibrer").debug("Apply fixed point correction : " + src->toString() + " -> " + dst->toString(), __FILE__, __LINE__);
    return dst;
}
Color* Calibrer::applyWhiteBalanceCorrection(Color* src)
{
    Color* dst = new Color();
    int redRef = src->getRed()*255.0+0.5;
    int greenRef = src->getGreen()*255.0+0.5;
    int blueRef = src->getBlue()*255.0+0.5;

    double white = std::min(redRef, std::min(greenRef, blueRef));

    // -- 1st solution : set white 
    redRef -= white;
    greenRef -= white;
    blueRef -= white;

    // -- extrapolate following curve to get factors
    double redFinalFactor = redRef*redRef*polyRed[0] + redRef*polyRed[1];
    double greenFinalFactor = greenRef*greenRef*polyGreen[0] + greenRef*polyGreen[1];
    double blueFinalFactor = blueRef*blueRef*polyBlue[0] + blueRef*polyBlue[1];

    // -- apply correct RGB mixing
    redRef *= redFinalFactor;
    greenRef *= greenFinalFactor;
    blueRef *= blueFinalFactor;

    dst->setRGBA(redRef/255.0, greenRef/255.0, blueRef/255.0, src->getAlpha());
    dst->setWhiteIntensity(white/255.0, 0);

    Poco::Logger::get("Calibrer").debug("Apply white balance correction : " + src->toString() + " -> " + dst->toString(), __FILE__, __LINE__);
    return dst;
}