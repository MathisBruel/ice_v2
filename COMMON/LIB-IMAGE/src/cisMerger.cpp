#include "cisMerger.h"

CisMerger::CisMerger() {}
CisMerger::~CisMerger() {}

void CisMerger::setImageFormat(uint32_t width, uint32_t height, uint32_t bpp, TimeCodeType frameRate)
{
    this->width = width;
    this->height = height;
    this->bpp = bpp;
    this->frameRate = frameRate;
}

bool CisMerger::save(std::string fileName, std::string type) 
{
    if (cisList.size() <= 0) {
        Poco::Logger::get("CisMerger").error("No cis given !", __FILE__, __LINE__);
        return false;
    }

    file = fopen(fileName.c_str(), "wb");
    if (file == nullptr) {
        Poco::Logger::get("CisMerger").error("Error when opening save file !", __FILE__, __LINE__);
        return false;
    }

    // -- determine number of total frame
    int nbFrames = 0;
    for (int i = 0; i < cisList.size(); i++) {
        nbFrames += cisList.at(i)->getFrameCount();
    }

    Poco::Logger::get("CisMerger").debug("Total Number of frames to save : " + std::to_string(nbFrames), __FILE__, __LINE__);

    // -- initialize offset structure & fill
    uint64_t* offset = new uint64_t[nbFrames];
    uint64_t globalOffset = 0;
    uint32_t globalIndex = 0;
    for (int i = 0; i < cisList.size(); i++) {
        uint64_t* frameOffset = cisList.at(i)->getFrameOffset();

        for (int j = 0; j < cisList.at(i)->getFrameCount(); j++) {
            offset[globalIndex+j] = be64toh(frameOffset[j]) + globalOffset;
        }
        globalIndex += cisList.at(i)->getFrameCount();
        globalOffset += cisList.at(i)->getContentSize();
    }

    // -- do the hard copy of images
    for (int i = 0; i < cisList.size(); i++) {

        // -- open cis
        FILE *cisFile;
        cisFile = fopen(cisList.at(i)->getFileName().c_str(), "rb");
        if (cisFile == nullptr) {
            Poco::Logger::get("CisMerger").error("Error when opening read file !", __FILE__, __LINE__);
            delete[] offset;
            return false;
        }

        // -- detect size of content
        uint64_t nbBytes = cisList.at(i)->getContentSize();
        char* buffer = new char[nbBytes];

        // -- read all datas
        fread(buffer, nbBytes, 1, cisFile);

        // -- write all datas
        fwrite(buffer, nbBytes, 1, file);

        // -- close file and cleaning
        delete[] buffer;
        fclose(cisFile);
    }

    // -- write header
    uint64_t positionHeader = ftell(file);

    // -- determine type of CIS
    uint32_t length = 24;

    // -- write header
    char buffer[24];
    for (int i = 0; i < 4; i++) {
        buffer[3-i] = (char)((length >> (i*8)) & 0xff);
        buffer[7-i] = (char)((nbFrames >> (i*8)) & 0xff);
        buffer[11-i] = (char)((Cis::getTimeCodeTypeInt(frameRate) >> (i*8)) & 0xff);
        buffer[15-i] = (char)((width >> (i*8)) & 0xff);
        buffer[19-i] = (char)((height >> (i*8)) & 0xff);
        buffer[23-i] = (char)((bpp >> (i*8)) & 0xff);
    }
    fwrite(buffer, length, 1, file);

    // -- write frames offset
    for (int i = 0; i < nbFrames; i++) {
        char bufferOffset[8];
        for (int j = 0; j < 8; j++) {
            bufferOffset[7-j] = (char)((offset[i] >> (j*8)) & 0xff);
        }
        fwrite(bufferOffset, sizeof(bufferOffset), 1, file);
    }

    // -- write footer
    uint64_t offsetHeader = ftell(file) + 16 - positionHeader;
    char bufferOffsetHeader[8];
    for (int i = 0; i < 8; i++) {
        bufferOffsetHeader[7-i] = (char)((offsetHeader >> (i*8)) & 0xff);
    }
    fwrite(bufferOffsetHeader, sizeof(bufferOffsetHeader), 1, file);
    fwrite((char*)type.c_str(), sizeof(type.c_str()), 1, file);

    // -- close file and cleaning
    fclose(file);
    delete[] offset;

    return true;
}