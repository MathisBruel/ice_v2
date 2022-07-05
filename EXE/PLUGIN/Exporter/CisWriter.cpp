#include "cisWriter.h"

CisWriter::CisWriter()
{
    offset = nullptr;
    imagesBuffer = nullptr;
    lastFrameSaved = -1;
}

CisWriter::~CisWriter()
{
    if (offset != nullptr) {
        delete[] offset;
    }

    if (imagesBuffer != nullptr) {
        delete[] imagesBuffer;
    }
}

void CisWriter::setImageFormat(uint32_t widthIn, uint32_t heightIn, uint32_t bppIn)
{
    this->width = widthIn;
    this->height = heightIn;
    this->bpp = bppIn;

    // -- hack : 24 FPS
    this->frameRate = 0;
}

void CisWriter::setNbImages(int nbImagesIn)
{
    this->nbImages = nbImagesIn;

    offset = new uint64_t[nbImages];
    imagesBuffer = new char*[nbImages];
    for (int i = 0; i < nbImages; i++) {
        imagesBuffer[i] = nullptr;
    }
}

bool CisWriter::startSave(std::string fileName)
{
    file = fopen(fileName.c_str(), "wb");
    if (file == nullptr) {
        return false;
    }
    return true;
}

void CisWriter::addImageAsync(int frame, char* bufferImg)
{
    imagesBuffer[frame] = bufferImg;

    /*png_image* image = new png_image();
    image->version = PNG_IMAGE_VERSION;
    image->width = width;
    image->height = height;
    image->format = PNG_FORMAT_BGRA;
    image->opaque = nullptr;
    image->flags = 0;
    image->colormap_entries = 0;

    std::string fileStr = "C:\\ICE\\" + std::to_string(frame) + ".png";
    png_image_write_to_file(image, fileStr.c_str(), 0, bufferImg, PNG_IMAGE_ROW_STRIDE(*image), 0);
    delete image;*/
}

bool CisWriter::saveFrameAsync(int frame)
{
    offset[frame] = ftell(file);

    png_image* image = new png_image();
    image->version = PNG_IMAGE_VERSION;
    image->width = width;
    image->height = height;
    image->format = PNG_FORMAT_BGRA;
    image->opaque = nullptr;
    image->flags = 0;
    image->colormap_entries = 0;
    int returnValue = png_image_write_to_stdio(image, file, 0, imagesBuffer[frame], PNG_IMAGE_ROW_STRIDE(*image), 0);
    if (returnValue != 0 && returnValue != 1) {
        delete image;
        return false;
    }
    delete image;    
    return true;
}

void CisWriter::trySave()
{
    if (imagesBuffer[lastFrameSaved+1] != nullptr) {
        saveFrameAsync(lastFrameSaved+1);
        lastFrameSaved++;
        delete[] imagesBuffer[lastFrameSaved];
        imagesBuffer[lastFrameSaved] = nullptr;
    }
}

float CisWriter::getProgress()
{
    float progress = (float)(lastFrameSaved+1) / (float)(nbImages);
    return progress;
}

bool CisWriter::isFinished()
{
    if (lastFrameSaved >= nbImages-1) {
        return true;
    }
    return false;
}

bool CisWriter::endSave(std::string type) 
{
    uint64_t positionHeader = ftell(file);

    uint32_t length = 24;
    char buffer[24];
    for (int i = 0; i < 4; i++) {
        buffer[3-i] = (char)((length >> (i*8)) & 0xff);
        buffer[7-i] = (char)((nbImages >> (i*8)) & 0xff);
        buffer[11-i] = (char)((frameRate >> (i*8)) & 0xff);
        buffer[15-i] = (char)((width >> (i*8)) & 0xff);
        buffer[19-i] = (char)((height >> (i*8)) & 0xff);
        buffer[23-i] = (char)((bpp >> (i*8)) & 0xff);
    }
    fwrite(buffer, length, 1, file);

    for (int i = 0; i < nbImages; i++) {
        char bufferOffset[8];
        for (int j = 0; j < 8; j++) {
            bufferOffset[7-j] = (char)((offset[i] >> (j*8)) & 0xff);
        }
        fwrite(bufferOffset, sizeof(bufferOffset), 1, file);
    }

    uint64_t offsetHeader = ftell(file) + 16 - positionHeader;
    char bufferOffsetHeader[8];
    for (int i = 0; i < 8; i++) {
        bufferOffsetHeader[7-i] = (char)((offsetHeader >> (i*8)) & 0xff);
    }
    fwrite(bufferOffsetHeader, sizeof(bufferOffsetHeader), 1, file);
    fwrite((char*)type.c_str(), sizeof(type.c_str()), 1, file);

    fclose(file);

    delete[] offset;
    return true;
}