#include <iostream>
#include <cstdlib>
#include <fstream>
#include "portable_endian.h"

#pragma once

#include "image.h"
#include "ByteArray.h"
#include "Poco/Logger.h"

static const std::string CisMagicStringV1 = "CIS-V001";
static const std::string CisMagicStringV2 = "CIS-V002";
static const int SizeFooter = 16;

enum class TimeCodeType: int {
	Invalid = -1,
	Film_24fps = 0,
	EBU_25fps = 1,
	DF_29_97fps = 2,
	SMPTE_30fps = 3,
	Film_23_976fps = 4,
	Film_48fps = 5,
	PAL_50fps = 6,
	NTSC_59_94fps = 7,
	NTSC_60fps = 8,
	Film_72fps = 9,
	ITU_120fps = 10,
	Test_1fps = 11,
};

struct CisHeader
{
    uint32_t length;
    uint32_t frameCount;
    TimeCodeType frameRate;
    uint32_t frameWidth;
    uint32_t frameHeight;
    uint32_t bpp;
	bool lutApplied;
	int version;
};

struct CisFooter
{
    uint64_t headerOffset;
    uint8_t* magic;
};

class Cis
{
public:

	Cis();
	~Cis();

	bool open(std::string filename);
	
	// -- all header informations
	uint32_t getFrameWidth() {return header->frameWidth;}
	uint32_t getFrameHeight() {return header->frameHeight;}
	uint32_t getBitsPerPixel() { return header->bpp;}
    uint32_t getFrameCount() { return header->frameCount;}
    TimeCodeType getFrameRate() { return header->frameRate;}
	bool isLutApplied() {return header->lutApplied;}
	double getSpeedRate();
	int getVersion() {return header->version;}

	static uint32_t getTimeCodeTypeInt(TimeCodeType timeCodeType);

    Image* getRGBAImage(uint64_t frameIndex);

	// -- for hard copy
	uint64_t* getFrameOffset() {return frameOffsets;}
	uint64_t getHeaderOffset() {return footer->headerOffset;}
	std::string getFileName() {return fileName;}
	uint64_t getContentSize() {return contentSize;}

private:
	
	std::ifstream* inStream;
	std::ofstream* outStream;
    std::string fileName;
    CisHeader *header;
	CisFooter *footer;
    uint64_t *frameOffsets;
	uint64_t contentSize;

    bool checkFooter();
	bool checkHeader();
	bool moveToPNGFile(uint64_t frameIndex);
};