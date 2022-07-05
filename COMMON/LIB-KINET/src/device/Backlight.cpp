#include "device/Backlight.h"

Backlight::Backlight(std::string name, std::string id, uint32_t serial, uint16_t dmxOffset, std::string formatBuffer)
    : DmxDevice(name, id, serial, dmxOffset, formatBuffer)
{
    type = BACKLIGHT;
}

void Backlight::createPayload(uint8_t red, uint8_t green, uint8_t blue, uint8_t amber, bool lutApplied)
{
    if (lutApplied) {
        red = std::min((int)((double)(red * 0.7615) + 0.5), 255);
        blue = std::min((int)((double)(blue * 0.2811) + 0.5), 255);
    }

    // -- retroColor correction : determine reel color was really wanted
    if (lutApplied && refColorCorrection != nullptr) {
        Color* src = new Color();
        src->setRGB(red/255.0, green/255.0, blue/255.0);
        Color* ref = refColorCorrection->applyInverseCorrection(src);
        red = ref->getRed()*255.0+0.5;
        green = ref->getGreen()*255.0+0.5;
        blue = ref->getBlue()*255.0+0.5;
        delete ref;
        delete src;
        lutApplied = false;
    }

    // -- correction to do : not applied in TEMPLATE
    if (colorCorrection != nullptr && !lutApplied) {
        Color src;
        src.setRGB(red/255.0, green/255.0, blue/255.0);
        Color* dst = colorCorrection->applyCorrection(&src);
        red = dst->getRed()*255.0+0.5;
        green = dst->getGreen()*255.0+0.5;
        blue = dst->getBlue()*255.0+0.5;
        delete dst;
    }


    for (int i = 0; i < bufferTemplate.length(); i++) {
        if (bufferTemplate.substr(i, 1) == "R") {
            payload[i] = red;
        }
        else if (bufferTemplate.substr(i, 1) == "G") {
            payload[i] = green;
        }
        else if (bufferTemplate.substr(i, 1) == "B") {
            payload[i] = blue;
        }
        else if (bufferTemplate.substr(i, 1) == "A") {
            payload[i] = amber;
        }
    }
}