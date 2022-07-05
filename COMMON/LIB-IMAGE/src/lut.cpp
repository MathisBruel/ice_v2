#include "lut.h"

LUT::LUT()
{
    // -- initialization of all structure
    red = new double[16];
    green = new double[16];
    blue = new double[16];
    redOnGreen = new double[16];
    blueOnGreen = new double[16];
    redOnBlue = new double[16];
    greenOnBlue = new double[16];
    greenOnRed = new double[16];
    blueOnRed = new double[16];

    redPoly = new double[14*3];
    redFinalPoly = new double[256];
    greenPoly = new double[14*3];
    greenFinalPoly = new double[256];
    bluePoly = new double[14*3];
    blueFinalPoly = new double[256];

    redOnGreenPoly = new double[14*3];
    redOnGreenFinalPoly = new double[256];
    greenOnRedPoly = new double[14*3];
    greenOnRedFinalPoly = new double[256];

    redOnBluePoly = new double[14*3];
    redOnBlueFinalPoly = new double[256];
    blueOnRedPoly = new double[14*3];
    blueOnRedFinalPoly = new double[256];

    blueOnGreenPoly = new double[14*3];
    blueOnGreenFinalPoly = new double[256];
    greenOnBluePoly = new double[14*3];
    greenOnBlueFinalPoly = new double[256];

    for (int i = 0; i < 16; i++) {
        red[i] = (double)i*17.0/255.0;
        green[i] = (double)i*17.0/255.0;
        blue[i] = (double)i*17.0/255.0;
        redOnGreen[i] = 0.0;
        blueOnGreen[i] = 0.0;
        redOnBlue[i] = 0.0;
        greenOnBlue[i] = 0.0;
        greenOnRed[i] = 0.0;
        blueOnRed[i] = 0.0;
    }

    recalculateAll();

    precomputed = nullptr;
}
    
LUT::~LUT()
{
    if (precomputed != nullptr) {
        delete[] precomputed;
    }
}

void LUT::recalculateAll()
{
    // -- recalculate all polynomial factor (passing through all points 3 by 3)
    recalculatePolynome(red, redPoly);
    recalculatePolynome(green, greenPoly);
    recalculatePolynome(blue, bluePoly);
    recalculatePolynome(blueOnRed, blueOnRedPoly);
    recalculatePolynome(redOnBlue, redOnBluePoly);
    recalculatePolynome(blueOnGreen, blueOnGreenPoly);
    recalculatePolynome(greenOnBlue, greenOnBluePoly);
    recalculatePolynome(redOnGreen, redOnGreenPoly);
    recalculatePolynome(greenOnRed, greenOnRedPoly);

    // -- using polynopmial factor set all corresponding values (0 to 255 included) 
    setPolyFinal(redPoly, redFinalPoly);
    setPolyFinal(greenPoly, greenFinalPoly);
    setPolyFinal(bluePoly, blueFinalPoly);
    setPolyFinal(blueOnRedPoly, blueOnRedFinalPoly);
    setPolyFinal(redOnBluePoly, redOnBlueFinalPoly);
    setPolyFinal(blueOnGreenPoly, blueOnGreenFinalPoly);
    setPolyFinal(greenOnBluePoly, greenOnBlueFinalPoly);
    setPolyFinal(redOnGreenPoly, redOnGreenFinalPoly);
    setPolyFinal(greenOnRedPoly, greenOnRedFinalPoly);
}

void LUT::recalculatePolynome(double* src, double* poly)
{
    for (int index = 0; index < 14; index++) {
        poly[index*3] = (src[index]-src[index+1]) + (src[index+2] - src[index])/2;
        poly[index*3+1] = 2*(src[index+1]-src[index]) - (src[index+2] - src[index])/2;
        poly[index*3+2] = src[index];
    }
}

void LUT::setPolyFinal(double* poly, double* final)
{
    for (int i = 0; i < 256; i++) {
        double x = 2.0*(i % 34)/34.0;
        int indexMin = (i - i % 17)/17;
        indexMin -= indexMin%2;
        if (indexMin == 14) {
            indexMin--;
            x += 1.0;
        }

        final[i] = poly[indexMin*3]*x*x+poly[indexMin*3+1]*x+poly[indexMin*3+2];
    }

    for (int i = 18; i < 238; i++) {

        int i2 = i-17;
        double x = 2.0*(i2 % 34)/34.0;
        int indexMin = (i2 - i2 % 17)/17;
        indexMin -= indexMin%2;
        indexMin += 1;

        final[i] += poly[indexMin*3]*x*x+poly[indexMin*3+1]*x+poly[indexMin*3+2];
        final[i] /= 2;
    }
}

void LUT::precomputeAll() {
    
    precomputed = new ColorTemp[16777216];
    for (int red = 0; red < 256; red++) {
        for (int green = 0; green < 256; green++) {
            for (int blue = 0; blue < 256; blue++) {
                
                int idx = (red << 16) + (green << 8) + blue;
                Color src;
                src.setRGB((double)(red)/255.0, (double)(green)/255.0, (double)(blue)/255.0);
                Color* dst = getBestInterpolation(&src);
                precomputed[idx].red = (uint8_t)(dst->getRed()*255.0+0.5);
                precomputed[idx].green = (uint8_t)(dst->getGreen()*255.0+0.5);
                precomputed[idx].blue = (uint8_t)(dst->getBlue()*255.0+0.5);
                delete dst;
            }
        }
    }
}

bool LUT::open(std::string filename)
{
    Poco::File cubeFile(filename);

    // -- check file exists
    if (!cubeFile.exists()) {
        Poco::Logger::get("LUT").error("LUT file " + filename + " does not exists.", __FILE__, __LINE__);
        return false;
    }

    // -- read text file line by line 
    std::ifstream ifs;
    ifs.open(filename, std::ifstream::in);
    std::string colorText;

    // -- parse all color lines
    while(getline(ifs, colorText)) {

        if (colorText.find("RED") != std::string::npos) {
            for (int i = 0; i < 16; i++) {
                std::string value;
                getline(ifs, value);
                red[i] = std::stod(value);
            }
        }

        else if (colorText.find("GREEN") != std::string::npos) {
            for (int i = 0; i < 16; i++) {
                std::string value;
                getline(ifs, value);
                green[i] = std::stod(value);
            }
        }

        else if (colorText.find("BLUE") != std::string::npos) {
            for (int i = 0; i < 16; i++) {
                std::string value;
                getline(ifs, value);
                blue[i] = std::stod(value);
            }
        }

        else if (colorText.find("RGMap") != std::string::npos) {
            for (int i = 0; i < 16; i++) {
                std::string value;
                getline(ifs, value);
                redOnGreen[i] = std::stod(value);
            }
        }

        else if (colorText.find("BGMap") != std::string::npos) {
            for (int i = 0; i < 16; i++) {
                std::string value;
                getline(ifs, value);
                blueOnGreen[i] = std::stod(value);
            }
        }

        else if (colorText.find("RBMap") != std::string::npos) {
            for (int i = 0; i < 16; i++) {
                std::string value;
                getline(ifs, value);
                redOnBlue[i] = std::stod(value);
            }
        }

        else if (colorText.find("GBMap") != std::string::npos) {
            for (int i = 0; i < 16; i++) {
                std::string value;
                getline(ifs, value);
                greenOnBlue[i] = std::stod(value);
            }
        }

        else if (colorText.find("GRMap") != std::string::npos) {
            for (int i = 0; i < 16; i++) {
                std::string value;
                getline(ifs, value);
                greenOnRed[i] = std::stod(value);
            }
        }

        else if (colorText.find("BRMap") != std::string::npos) {
            for (int i = 0; i < 16; i++) {
                std::string value;
                getline(ifs, value);
                blueOnRed[i] = std::stod(value);
            }
        }
    }

    ifs.close();
    recalculateAll();

    return true;
}

void LUT::save(std::string filename)
{
    // -- write text file line by line 
    std::ofstream ofs;
    ofs.open(filename, std::ofstream::out);

    std::stringstream stream;
    stream << std::fixed << std::setprecision(6) << "RED" << std::endl;
    for (int i = 0; i < 16; i++) {stream << red[i] << std::endl;}

    stream << std::endl << "GREEN" << std::endl;
    for (int i = 0; i < 16; i++) {stream << green[i] << std::endl;}

    stream << std::endl << "BLUE" << std::endl;
    for (int i = 0; i < 16; i++) {stream << blue[i] << std::endl;}

    stream << std::endl << "RGMap" << std::endl;
    for (int i = 0; i < 16; i++) {stream << redOnGreen[i] << std::endl;}
    stream << std::endl << "BGMap" << std::endl;
    for (int i = 0; i < 16; i++) {stream << blueOnGreen[i] << std::endl;}

    stream << std::endl << "RBMap" << std::endl;
    for (int i = 0; i < 16; i++) {stream << redOnBlue[i] << std::endl;}
    stream << std::endl << "GBMap" << std::endl;
    for (int i = 0; i < 16; i++) {stream << greenOnBlue[i] << std::endl;}

    stream << std::endl << "GRMap" << std::endl;
    for (int i = 0; i < 16; i++) {stream << greenOnRed[i] << std::endl;}
    stream << std::endl << "BRMap" << std::endl;
    for (int i = 0; i < 16; i++) {stream << blueOnRed[i] << std::endl;}

    ofs << stream.str();


    // Close the file
    ofs.close();
}

void LUT::applyCorrection(Image* img)
{
    // -- for all pixels
    for (int i = 0; i < img->getWidth(); i++) {
        for (int j = 0; j < img->getHeight(); j++) {
            
            Color* ref = img->getValueAt(i, j);

            if (precomputed == nullptr) {
                // -- get color and interpolate
                Color* dst = getBestInterpolation(ref);
                // -- replace by corrected values
                img->setValueAt(i, j, *dst);
                delete dst;
                
            }
            else {
                int red = (int)(ref->getRed()*255.0 + 0.5);
                int green = (int)(ref->getGreen()*255.0 + 0.5);
                int blue = (int)(ref->getBlue()*255.0 + 0.5);
                int idx = (red << 16) + (green << 8) + blue;

                // -- get color and interpolate
                Color dst;
                dst.setRGB((double)(precomputed[idx].red)/255.0, (double)(precomputed[idx].green)/255.0, (double)(precomputed[idx].blue)/255.0);
                // -- replace by corrected values
                img->setValueAt(i, j, dst);
            }

            delete ref;
        }
    }
}

Color* LUT::getBestInterpolation(Color* src)
{
    // -- calculate corresponding index in structure
    int redIndex = src->getRed()*255.0 + 0.5;
    int greenIndex = src->getGreen()*255.0 + 0.5;
    int blueIndex = src->getBlue()*255.0 + 0.5;

    // calculate red final
    double redFinal = redFinalPoly[redIndex];
    double factorRed = (1.0-redFinal)*(1.0-redFinal);
    double biasRed = greenOnRedFinalPoly[greenIndex]*factorRed + blueOnRedFinalPoly[blueIndex]*factorRed;

    // calculate green final
    double greenFinal = greenFinalPoly[greenIndex];
    double factorGreen = (1.0-greenFinal)*(1.0-greenFinal);
    double biasGreen = redOnGreenFinalPoly[redIndex]*factorGreen + blueOnGreenFinalPoly[blueIndex]*factorGreen;

    // calculate blue final
    double blueFinal = blueFinalPoly[blueIndex];
    double factorBlue = (1.0-blueFinal)*(1.0-blueFinal);
    double biasBlue = redOnBlueFinalPoly[redIndex]*factorBlue + greenOnBlueFinalPoly[greenIndex]*factorBlue;

    // -- set corrected color
    Color* corrected = new Color();
    corrected->setRGB(redFinal + biasRed, greenFinal + biasGreen, blueFinal + biasBlue);

    return corrected;
}