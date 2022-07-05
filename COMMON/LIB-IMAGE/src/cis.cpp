#include "cis.h"

Cis::Cis()
{
    inStream = nullptr;
    outStream = nullptr;
    footer = nullptr;
    header = nullptr;
    frameOffsets = nullptr;
}
Cis::~Cis()
{
    if (inStream != nullptr) {
        if (inStream->is_open()) {
            inStream->close();
        }
        delete inStream;
    }

    if (outStream != nullptr) {
        if (outStream->is_open()) {
            outStream->close();
        }
        delete outStream;
    }

    if (footer != nullptr) {
        delete[] footer->magic;
        delete footer;
    }

    if (header != nullptr) {
        delete header;
    }

    if (frameOffsets != nullptr) {
        delete[] frameOffsets;
    }
}

bool Cis::open(std::string filename)
{
    this->fileName = filename;

    // -- check if file exists
    Poco::File file(filename);
    if (!file.exists()) {
        Poco::Logger::get("Cis").error("Cis file does not exists : " + fileName, __FILE__, __LINE__);
        return false;
    }

    // -- open stream on file and check if opened
    inStream = new std::ifstream(filename, std::ios_base::binary);

    if (!inStream->is_open()) {
        Poco::Logger::get("Cis").error("Cis file can't be opened : " + fileName, __FILE__, __LINE__);
        return false;
    }

    if (!checkFooter()) {
        return false;
    }

    if (!checkHeader()) {
        return false;
    }

    return true;
}

bool Cis::checkFooter() {

    // -- place the cursor at the end to read footer
    inStream->seekg(-SizeFooter, std::ios_base::end);
    if (inStream->fail()) {
        Poco::Logger::get("Cis").error("Error while seeking at the end of cis file : " + fileName, __FILE__, __LINE__);
        return false;
    }

    // -- read footer
    //
    ByteArray array(SizeFooter);
    inStream->read((char*)array.getData(), SizeFooter);

    // -- check if fail at reading
    if (inStream->fail() || inStream->gcount() != SizeFooter) {
        Poco::Logger::get("Cis").error("Error while reading footer of cis file : " + fileName, __FILE__, __LINE__);
        return false;
    }

    // -- fill structure of footer
    footer = new CisFooter();
    footer->headerOffset = array.getUInt64(0, false);
    footer->magic = array.get(sizeof(uint64_t), 8);
    inStream->seekg(0, std::ios::end);
    contentSize = inStream->tellg();
    contentSize -= footer->headerOffset;

    // -- check footer value
    std::string magicString((char*)footer->magic, 8);
    if (magicString != CisMagicStringV1 && magicString != CisMagicStringV2) {
        Poco::Logger::get("Cis").error("Cis file does not contain a valid cis magic string : " + fileName + "-" + magicString, __FILE__, __LINE__);
        return false;
    }
    return true;
}

bool Cis::checkHeader() {

    // -- place the cursor at the begin to read header
    inStream->seekg(-footer->headerOffset, std::ios_base::end);
    if (inStream->fail()) {
        Poco::Logger::get("Cis").error("Error while seeking at the start of cis file : " + fileName, __FILE__, __LINE__);
        return false;
    }

    uint64_t headerData = footer->headerOffset - SizeFooter;
    if (headerData < sizeof(CisHeader)) {
        Poco::Logger::get("Cis").error("Size of header data is not enough : " + fileName, __FILE__, __LINE__);
        return false;
    }

    ByteArray array(headerData);
    inStream->read((char*)array.getData(), headerData);
    if (inStream->fail() || inStream->gcount() != headerData) {
        Poco::Logger::get("Cis").error("Error while reading header of cis file : " + fileName, __FILE__, __LINE__);
        return false;
    }

    // -- parse byte array data to fill header
    header = new CisHeader();
    int offset = 0;
    header->length = array.getUInt32(offset, false);
    offset += sizeof(uint32_t);
    header->frameCount = array.getUInt32(offset, false);
    offset += sizeof(uint32_t);
    header->frameRate = (TimeCodeType)array.getUInt32(offset, false);
    offset += sizeof(uint32_t);
    header->frameWidth = array.getUInt32(offset, false);
    offset += sizeof(uint32_t);
    header->frameHeight = array.getUInt32(offset, false);
    offset += sizeof(uint32_t);
    header->bpp = array.getUInt32(offset, false);
    offset += sizeof(uint32_t);

    std::string magic((char*)footer->magic, 8);
    if (magic == CisMagicStringV1) {
        header->lutApplied = true;
        header->version = 1;
    }
    else if (magic == CisMagicStringV2) {
        header->lutApplied = false;
        header->version = 2;
    }

    // -- deduct all frameOffsets
    frameOffsets = new uint64_t[header->frameCount];
    for (int i = 0; i < header->frameCount; i++) {
        frameOffsets[i] = array.getUInt64(header->length+8*i, false);
    }
    uint32_t offsetDataSize = headerData - header->length;

    if (offsetDataSize % sizeof(uint64_t) != 0 || offsetDataSize / sizeof(uint64_t) != header->frameCount) {
        Poco::Logger::get("Cis").error("The index is inconsistent/corrupted in " + fileName, __FILE__, __LINE__);
        return false;
    }

    return true;
}

bool Cis::moveToPNGFile(uint64_t frameIndex) {

    if (header == nullptr || frameOffsets == nullptr) {
        Poco::Logger::get("Cis").error("File not opened or readable !", __FILE__, __LINE__);
        return false;
    }

    if (frameIndex >= header->frameCount) {
        Poco::Logger::get("Cis").error("Frame index is out of range !", __FILE__, __LINE__);
        return false;
    }
    inStream->seekg(frameOffsets[frameIndex]);
    if (inStream->fail()) {
        Poco::Logger::get("Cis").error("Error while seeking at frame index " + std::to_string(frameIndex) + " adress : " + std::to_string(frameOffsets[frameIndex]), __FILE__, __LINE__);
        return false;
    }

    return true;
}

uint32_t Cis::getTimeCodeTypeInt(TimeCodeType timeCodeType)
{
    switch (timeCodeType) {

        case TimeCodeType::Invalid:
        return -1;

	    case TimeCodeType::Film_24fps:
        return 0;

	    case TimeCodeType::EBU_25fps:
        return 1;

	    case TimeCodeType::DF_29_97fps:
        return 2;

	    case TimeCodeType::SMPTE_30fps:
        return 3;

	    case TimeCodeType::Film_23_976fps:
        return 4;

	    case TimeCodeType::Film_48fps:
        return 5;

	    case TimeCodeType::PAL_50fps:
        return 6;

	    case TimeCodeType::NTSC_59_94fps:
        return 7;

	    case TimeCodeType::NTSC_60fps:
        return 8;

	    case TimeCodeType::Film_72fps:
        return 9;

	    case TimeCodeType::ITU_120fps:
        return 10;

	    case TimeCodeType::Test_1fps:
        return 11;
    }
}

double Cis::getSpeedRate()
{
    switch (header->frameRate) {

        case TimeCodeType::Invalid:
        return -1;

	    case TimeCodeType::Film_24fps:
        return 24;

	    case TimeCodeType::EBU_25fps:
        return 25;

	    case TimeCodeType::DF_29_97fps:
        return 29.97;

	    case TimeCodeType::SMPTE_30fps:
        return 30;

	    case TimeCodeType::Film_23_976fps:
        return 23.976;

	    case TimeCodeType::Film_48fps:
        return 48;

	    case TimeCodeType::PAL_50fps:
        return 50;

	    case TimeCodeType::NTSC_59_94fps:
        return 59.94;

	    case TimeCodeType::NTSC_60fps:
        return 60;

	    case TimeCodeType::Film_72fps:
        return 72;

	    case TimeCodeType::ITU_120fps:
        return 120;

	    case TimeCodeType::Test_1fps:
        return 1;
    }
}

Image* Cis::getRGBAImage(uint64_t frameIndex)
{
    if (!moveToPNGFile(frameIndex)) {
        return nullptr;
    }

    Image* img = new Image();
    img->openImageFromStream(fileName, inStream);
    return img;
}