#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#pragma once
#include "ByteArray.h"
#include "Poco/HMACEngine.h"
#include "Poco/SHA1Engine.h"
#include "Poco/DigestEngine.h"
#include "Poco/Logger.h"

class Converter
{

public:

    static std::string ipToString(uint32_t ip);
    static std::string macToString(uint8_t* mac);
    static std::string serialToString(uint32_t serial);

    static std::string decodeBase64(std::string encoded);
    static uint8_t* decodeBase64(uint8_t* buffer, int length);
    static std::string encodeBase64(std::string decoded);

    static std::string calculateSha1OfFile(std::string pathFile);
    static std::string calculateSha1Buffer(unsigned char* buffer, long size);

    static uint32_t sizeDecode;
};