#include "device/Par.h"

PAR::PAR(std::string name, std::string id, uint32_t serial, uint16_t dmxOffset, std::string formatBuffer, TypePAR category, uint8_t defaultZoom, int lumens)
    : DmxDevice(name, id, serial, dmxOffset, formatBuffer)
{
    type = DmxType::PAR;
    this->category = category;
    this->defaultZoom = defaultZoom;
    this->lumens = lumens;
}

void PAR::createPayload(uint16_t red, uint16_t green, uint16_t blue, uint16_t white, uint8_t intensity, uint8_t zoom, bool lutApplied)
{
    if (lutApplied) {
        red = std::min((int)((double)(red * 0.72) + 0.5), 255);
        blue = std::min((int)((double)(blue * 0.5) + 0.5), 255);
    }

    // -- retroColor correction : determine reel color was really wanted
    if (lutApplied && refColorCorrection != nullptr) {
        Color* src = new Color();
        src->setRGB(red/255.0, green/255.0, blue/255.0);
        Color* ref = refColorCorrection->applyInverseCorrection(src);
        red = ref->getRed()*255.0+0.5;
        green = ref->getGreen()*255.0+0.5;
        blue = ref->getBlue()*255.0+0.5;
        intensity = 240;
        delete ref;
        delete src;
        lutApplied = false;
    }

    if (colorCorrection != nullptr && !lutApplied) {
        Color src;
        src.setRGBA(red/65535.0, green/65535.0, blue/65535.0, intensity/255.0);
        Color* dst = colorCorrection->applyCorrection(&src);
        red = dst->getRed()*65535.0+0.5;
        green = dst->getGreen()*65535.0+0.5;
        blue = dst->getBlue()*65535.0+0.5;
        white = dst->getWhite()*65535.0+0.5;
        delete dst;
    }

    for (int i = 0; i < bufferTemplate.length(); i++) {
        if (bufferTemplate.substr(i, 1) == "I") {
            payload[i] = intensity;
        }
        else if (bufferTemplate.substr(i, 1) == "Z") {
            payload[i] = zoom;
        }
        else if (bufferTemplate.substr(i, 1) == "R") {
            payload[i] = (uint8_t)(red >> 8);
        }
        else if (bufferTemplate.substr(i, 1) == "r") {
            payload[i] = (uint8_t)(red & 0xff);
        }
        else if (bufferTemplate.substr(i, 1) == "G") {
            payload[i] = (uint8_t)(green >> 8);
        }
        else if (bufferTemplate.substr(i, 1) == "g") {
            payload[i] = (uint8_t)(green & 0xff);
        }
        else if (bufferTemplate.substr(i, 1) == "B") {
            payload[i] = (uint8_t)(blue >> 8);
        }
        else if (bufferTemplate.substr(i, 1) == "b") {
            payload[i] = (uint8_t)(blue & 0xff);
        }
        else if (bufferTemplate.substr(i, 1) == "W") {
            payload[i] = (uint8_t)(white >> 8);
        }
        else if (bufferTemplate.substr(i, 1) == "w") {
            payload[i] = (uint8_t)(white & 0xff);
        }
    }
}

void PAR::createPayload(uint16_t red, uint16_t green, uint16_t blue, uint16_t white, uint8_t intensity, bool lutApplied)
{
    if (lutApplied) {
        red = std::min((int)((double)(red * 0.72) + 0.5), 255);
        blue = std::min((int)((double)(blue * 0.5) + 0.5), 255);
    }

    // -- retroColor correction : determine reel color was really wanted
    if (lutApplied && refColorCorrection != nullptr) {
        Color* src = new Color();
        src->setRGB(red/255.0, green/255.0, blue/255.0);
        Color* ref = refColorCorrection->applyInverseCorrection(src);
        red = ref->getRed()*255.0+0.5;
        green = ref->getGreen()*255.0+0.5;
        blue = ref->getBlue()*255.0+0.5;
        intensity = 240;
        delete ref;
        delete src;
        lutApplied = false;
    }

    if (colorCorrection != nullptr && !lutApplied) {
        Color src;
        src.setRGBA(red/65535.0, green/65535.0, blue/65535.0, intensity/255.0);
        Color* dst = colorCorrection->applyCorrection(&src);
        red = dst->getRed()*65535.0+0.5;
        green = dst->getGreen()*65535.0+0.5;
        blue = dst->getBlue()*65535.0+0.5;
        white = dst->getWhite()*65535.0+0.5;
        delete dst;
    }

    for (int i = 0; i < bufferTemplate.length(); i++) {
        if (bufferTemplate.substr(i, 1) == "I") {
            payload[i] = intensity;
        }
        else if (bufferTemplate.substr(i, 1) == "Z") {
            payload[i] = defaultZoom;
        }
        else if (bufferTemplate.substr(i, 1) == "R") {
            payload[i] = (uint8_t)(red >> 8);
        }
        else if (bufferTemplate.substr(i, 1) == "r") {
            payload[i] = (uint8_t)(red & 0xff);
        }
        else if (bufferTemplate.substr(i, 1) == "G") {
            payload[i] = (uint8_t)(green >> 8);
        }
        else if (bufferTemplate.substr(i, 1) == "g") {
            payload[i] = (uint8_t)(green & 0xff);
        }
        else if (bufferTemplate.substr(i, 1) == "B") {
            payload[i] = (uint8_t)(blue >> 8);
        }
        else if (bufferTemplate.substr(i, 1) == "b") {
            payload[i] = (uint8_t)(blue & 0xff);
        }
        else if (bufferTemplate.substr(i, 1) == "W") {
            payload[i] = (uint8_t)(white >> 8);
        }
        else if (bufferTemplate.substr(i, 1) == "w") {
            payload[i] = (uint8_t)(white & 0xff);
        }
    }
}