#include "labCalib.h"

LabCalib::LabCalib()
{
    nbPoints = 21;
    factorA = 0;
    biasA = 0;
    factorB = 0;
    biasB = 0;
    dataSrcL = new double[nbPoints];
    dataDstL = new double[nbPoints];

    for (int i = 0; i < nbPoints; i++) {
        dataSrcL[i] = i*100.0/20.0;
        dataDstL[i] = i*100.0/20.0;
    }
}
    
LabCalib::~LabCalib()
{
    if (dataSrcL != nullptr) {
        delete[] dataSrcL;
    }
    if (dataDstL != nullptr) {
        delete[] dataDstL;
    }
}

bool LabCalib::open(std::string filename)
{
    Poco::File cubeFile(filename);

    // -- check file exists
    if (!cubeFile.exists()) {
        Poco::Logger::get("LabCalib").error("LAB file " + filename + " does not exists.", __FILE__, __LINE__);
        return false;
    }

    if (dataSrcL != nullptr) {
        delete[] dataSrcL;
    }
    if (dataDstL != nullptr) {
        delete[] dataDstL;
    }

    // -- read text file line by line 
    std::ifstream ifs;
    ifs.open(filename, std::ifstream::in);
    std::string colorText;

    // -- parse all color lines
    while(getline(ifs, colorText)) {

        if (colorText.find("NB") != std::string::npos) {
            int posSep = colorText.find(" ");
            nbPoints = std::stoi(colorText.substr(posSep+1));
            dataSrcL = new double[nbPoints];
            dataDstL = new double[nbPoints];
        }

        if (colorText.find("L") != std::string::npos) {
            
            for (int i = 0; i < nbPoints; i++) {
                std::string value;
                getline(ifs, value);
                int posSep = colorText.find(" ");
                dataSrcL[i] = std::stod(value.substr(0, posSep));
                dataDstL[i] = std::stod(value.substr(posSep+1));
            }
        }

        if (colorText.find("a") != std::string::npos) {
            std::string value;
            getline(ifs, value);
            int posSep = value.find(" ");
            factorA = std::stod(value.substr(0, posSep));
            biasA = std::stod(value.substr(posSep+1));
        }

        if (colorText.find("b") != std::string::npos) {
            std::string value;
            getline(ifs, value);
            int posSep = value.find(" ");
            factorB = std::stod(value.substr(0, posSep));
            biasB = std::stod(value.substr(posSep+1));
        }
    }

    ifs.close();

    return true;
}

void LabCalib::save(std::string filename)
{
    // -- write text file line by line 
    std::ofstream ofs;
    ofs.open(filename, std::ofstream::out);

    std::stringstream stream;
    stream << std::fixed << std::setprecision(6) << "L" << std::endl;
    for (int i = 0; i < nbPoints; i++) {stream << dataSrcL[i] << " " << dataDstL[i] << std::endl;}

    stream << std::endl << "a" << std::endl;
    stream << factorA << " " << biasA << std::endl;

    stream << std::endl << "b" << std::endl;
    stream << factorB << " " << biasB << std::endl;

    ofs << stream.str();

    // Close the file
    ofs.close();
}

void LabCalib::applyCorrection(Image* img)
{
    // -- for all pixels
    for (int i = 0; i < img->getWidth(); i++) {
        for (int j = 0; j < img->getHeight(); j++) {
            
            // -- get color and interpolate
            Color* ref = img->getValueAt(i, j);
            Color* cor = getBestInterpolation(ref);
            // -- replace by corrected values
            img->setValueAt(i, j, *cor);
            delete cor;
            delete ref;
        }
    }
}

Color* LabCalib::getBestInterpolation(Color* src)
{
    src->convertRGBToXYZ();
    src->convertXYZToLAB();

    // -- calculate L
    double LFinal = 0;
    for (int i = 0; i < nbPoints; i++) {
        if (dataSrcL[i] == src->getL()) {
            LFinal = dataDstL[i];
            break;
        }
        else if (dataSrcL[i] > src->getL()) {

            double factorL = (src->getL() - dataSrcL[i-1]) / (dataSrcL[i] - dataSrcL[i-1]);
            LFinal = dataDstL[i-1] + (dataDstL[i] - dataDstL[i-1])*factorL;
            break;
        }
    }

    // -- calculate A & B
    double AFinal = src->getA() * factorA + biasA;
    double BFinal = src->getB() * factorB + biasB;

    // -- set corrected color
    Color* corrected = new Color();
    corrected->setLAB(LFinal, AFinal, BFinal);
    corrected->convertLABToXYZ();
    corrected->convertXYZToRGB();
    corrected->correctRGBLimits();

    Poco::Logger::get("LabCalib").debug("Lab conversion " + src->toLABString() + " -> " + corrected->toLABString(), __FILE__, __LINE__);
    Poco::Logger::get("LabCalib").debug("Lab conversion RGB " + src->toString() + " -> " + corrected->toString(), __FILE__, __LINE__);

    return corrected;
}