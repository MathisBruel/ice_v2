#include "device/LuminousTextile.h"

LuminousTextile::LuminousTextile(std::string id, std::string name, std::string macAddress, std::string ip, uint16_t nbPixels)
    : Device(id, name, macAddress, ip, nbPorts)
{
    type = LUMINOUS_TEXTIL;

    switch (nbPixels) {

        case 1440 :
        width = 20;
        height = nbPixels/width;
        nbPorts = 9;
        heightPort = new int[nbPorts];
        payloadsSize = new uint32_t[nbPorts];
        payloads = new uint8_t*[nbPorts];
        for (int i = 0; i < nbPorts; i++) {
            heightPort[i] = 8;
            payloadsSize[i] = 20*8*3;
        }
        break;

        case 1320:
        width = 20;
        height = nbPixels/width;
        nbPorts = 9;
        heightPort = new int[nbPorts];
        payloadsSize = new uint32_t[nbPorts];
        payloads = new uint8_t*[nbPorts];
        for (int i = 0; i < nbPorts-1; i++) {
            heightPort[i] = 8;
            payloadsSize[i] = 20*8*3;
        }
        heightPort[8] = 2;
        payloadsSize[8] = 20*2*3;
        break;

        case 1200:
        width = 20;
        height = nbPixels/width;
        nbPorts = 8;
        heightPort = new int[nbPorts];
        payloadsSize = new uint32_t[nbPorts];
        payloads = new uint8_t*[nbPorts];
        for (int i = 0; i < nbPorts-1; i++) {
            heightPort[i] = 8;
            payloadsSize[i] = 20*8*3;
        }
        heightPort[7] = 4;
        payloadsSize[7] = 20*4*3;
        break;

        case 1080:
        width = 20;
        height = nbPixels/width;
        nbPorts = 7;
        heightPort = new int[nbPorts];
        payloadsSize = new uint32_t[nbPorts];
        payloads = new uint8_t*[nbPorts];
        for (int i = 0; i < nbPorts-1; i++) {
            heightPort[i] = 8;
            payloadsSize[i] = 20*8*3;
        }
        heightPort[6] = 6;
        payloadsSize[6] = 20*6*3;
        break;


        case 960:
        width = 20;
        height = nbPixels/width;
        nbPorts = 6;
        heightPort = new int[nbPorts];
        payloadsSize = new uint32_t[nbPorts];
        payloads = new uint8_t*[nbPorts];
        for (int i = 0; i < nbPorts; i++) {
            heightPort[i] = 8;
            payloadsSize[i] = 20*8*3;
        }
        break;
        
        case 840:
        width = 20;
        height = nbPixels/width;
        nbPorts = 6;
        heightPort = new int[nbPorts];
        payloadsSize = new uint32_t[nbPorts];
        payloads = new uint8_t*[nbPorts];
        for (int i = 0; i < nbPorts-1; i++) {
            heightPort[i] = 8;
            payloadsSize[i] = 20*8*3;
        }
        heightPort[5] = 2;
        payloadsSize[5] = 20*2*3;
        break;

        case 720:
        width = 20;
        height = nbPixels/width;
        nbPorts = 5;
        heightPort = new int[nbPorts];
        payloadsSize = new uint32_t[nbPorts];
        payloads = new uint8_t*[nbPorts];
        for (int i = 0; i < nbPorts-1; i++) {
            heightPort[i] = 8;
            payloadsSize[i] = 20*8*3;
        }
        heightPort[4] = 4;
        payloadsSize[4] = 20*4*3;
        break;

        case 240:
        width = 20;
        height = nbPixels/width;
        nbPorts = 2;
        heightPort = new int[nbPorts];
        payloadsSize = new uint32_t[nbPorts];
        payloads = new uint8_t*[nbPorts];
        heightPort[0] = 8;
        payloadsSize[0] = 20*8*3;
        heightPort[1] = 4;
        payloadsSize[1] = 20*4*3;
        break;
    }
}

LuminousTextile::~LuminousTextile() {

    if (heightPort != nullptr) {
        delete[] heightPort;
    }
    img.reset();
}

void LuminousTextile::createPayload(uint8_t red, uint8_t green, uint8_t blue, bool lutApplied)
{
    unsigned char* data = new unsigned char[width*height*3];
    img = std::make_shared<Image>(Image(width, height, PNG_FORMAT_RGB, data));

    if (colorCorrection != nullptr && !lutApplied) {
        Poco::Stopwatch watch;
        watch.start();
        colorCorrection->applyCorrection(img.get());
        //Poco::Logger::get("LuminousTextile").debug("Elapsed time of correction : " + std::to_string(watch.elapsed()) + "µs !", __FILE__, __LINE__);
    }

    /// -- fill datas payloads 
    int offsetY = 0;
    for (int i = 0; i < nbPorts; i++) {

        int index = offsetY*width*3;
        payloads[i] = &img->getData()[index];
        offsetY += heightPort[i];
    }
}

void LuminousTextile::createPayload(std::shared_ptr<Image> img, bool lutApplied)
{
    // -- check datas
    if (img == nullptr) {
        Poco::Logger::get("LuminousTextile").error("Error image is nullptr !", __FILE__, __LINE__);
        return;
    }

    this->img = img;

    // -- we check size is exactly correct
    if (img->getWidth() != width ||
        img->getHeight() != height ||
        img->getNbChannels() != 3) {
        Poco::Logger::get("LuminousTextile").error("Error image is not at the right size !", __FILE__, __LINE__);
        Poco::Logger::get("LuminousTextile").error(std::to_string(img->getWidth()) + " : " + std::to_string(width), __FILE__, __LINE__);
        Poco::Logger::get("LuminousTextile").error(std::to_string(img->getHeight()) + " : " + std::to_string(height), __FILE__, __LINE__);
        Poco::Logger::get("LuminousTextile").error(std::to_string(img->getNbChannels()) + " : 3", __FILE__, __LINE__);
        return;
    }

    if (colorCorrection != nullptr && !lutApplied) {
        Poco::Stopwatch watch;
        watch.start();
        colorCorrection->applyCorrection(img.get());
        //Poco::Logger::get("LuminousTextile").debug("Elapsed time of correction : " + std::to_string(watch.elapsed()) + "µs !", __FILE__, __LINE__);
    }

    // -- fill datas payloads 
    int offsetY = 0;
    for (int i = 0; i < nbPorts; i++) {

        int index = offsetY*width*3;

        payloads[i] = &img->getData()[index];
        offsetY += heightPort[i];
    }
}