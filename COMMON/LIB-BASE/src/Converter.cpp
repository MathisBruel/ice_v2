#include "Converter.h"

static const unsigned char charToBase64[256] =
{
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255, 62,255,255,255, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61,255,255,255,  0,255,255,
    255,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,255,255,255,255,255,
    255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
};

static const char base64ToChar[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

uint32_t Converter::sizeDecode = 0;

std::string Converter::ipToString(uint32_t ip)
{
    std::string ipString;
    ipString += std::to_string((ip & 0xff000000) >> 24);
    ipString += ".";
    ipString += std::to_string((ip & 0xff0000) >> 16);
    ipString += ".";
    ipString += std::to_string((ip & 0xff00) >> 8);
    ipString += ".";
    ipString += std::to_string(ip & 0xff);

    return ipString;
}

std::string Converter::macToString(uint8_t* mac)
{
    std::string macString = "";
    std::stringstream stream;

    for (int i = 0; i < 6; i++) 
    {
        stream << std::hex << std::setw(2) << std::setfill('0') << (int)(mac[5-i]);
        if (i < 5) stream << ':';
    }

    return stream.str();
}

std::string Converter::serialToString(uint32_t serial)
{
    std::stringstream stream;
    stream << std::hex << std::setw(8) << std::setfill('0') << serial;
    return stream.str();
}

uint8_t* Converter::decodeBase64(uint8_t* buffer, int length)
{
    int index = 0;
    int offset = 0;
    ByteArray result(length*3);
    while (index < length)
    {
        uint8_t base64_0 {};
        uint8_t base64_1 {};
        uint8_t base64_2 {};
        uint8_t base64_3 {};
        bool endCharFound {};

        base64_0 = charToBase64[size_t(buffer[index])];
        index++;

        base64_1 = charToBase64[size_t(buffer[index])];

        result.setUInt8((base64_0 & 0x3F) << 2 | (base64_1 & 0x30) >> 4, offset++);
        index++;

        if ((index < length) & (buffer[index] == '='))
        {
            endCharFound = true;
        }
        if ((index < length) & !endCharFound)
        {
            base64_2 = charToBase64[size_t(buffer[index])];
            result.setUInt8((base64_1 & 0x0F) << 4 | (base64_2 & 0x3C) >> 2, offset++);
        }
        index++;

        if ((index < length) & (buffer[index] == '='))
        {
            endCharFound = true;
        }
        if ((index < length) & !endCharFound)
        {
            base64_3 = charToBase64[size_t(buffer[index])];
            result.setUInt8((base64_2 & 0x03) << 6 | (base64_3 & 0x3F) >> 0, offset++);
        }
        index++;
    }

    sizeDecode = result.getSize();
    return result.getData();
}

std::string Converter::decodeBase64(std::string encoded)
{
    int index = 0;
    int offset = 0;
    int length = encoded.length();
    ByteArray result(length*3);
    while (index < length)
    {
        uint8_t base64_0 {};
        uint8_t base64_1 {};
        uint8_t base64_2 {};
        uint8_t base64_3 {};
        bool endCharFound {};

        base64_0 = charToBase64[size_t(encoded[index])];
        index++;

        base64_1 = charToBase64[size_t(encoded[index])];

        result.setUInt8((base64_0 & 0x3F) << 2 | (base64_1 & 0x30) >> 4, offset++);
        index++;

        if ((index < length) & (encoded[index] == '='))
        {
            endCharFound = true;
        }
        if ((index < length) & !endCharFound)
        {
            base64_2 = charToBase64[size_t(encoded[index])];
            result.setUInt8((base64_1 & 0x0F) << 4 | (base64_2 & 0x3C) >> 2, offset++);
        }
        index++;

        if ((index < length) & (encoded[index] == '='))
        {
            endCharFound = true;
        }
        if ((index < length) & !endCharFound)
        {
            base64_3 = charToBase64[size_t(encoded[index])];
            result.setUInt8((base64_2 & 0x03) << 6 | (base64_3 & 0x3F) >> 0, offset++);
        }
        index++;
    }

    return std::string((char*)result.getData(), offset);
}

std::string Converter::encodeBase64(std::string decoded)
{
    std::string result;
    int index = 0;
    int size = decoded.length();
    ByteArray data(size);
    data.set((uint8_t*)decoded.c_str(), 0, size);
    while (index < size - 2)
    {
        result += base64ToChar[(data.getData()[index + 0] & 0xFC) >> 2];
        result += base64ToChar[(data.getData()[index + 0] & 0x03) << 4 | (data.getData()[index + 1] & 0xF0) >> 4];
        result += base64ToChar[(data.getData()[index + 1] & 0x0F) << 2 | (data.getData()[index + 2] & 0xC0) >> 6];
        result += base64ToChar[(data.getData()[index + 2] & 0x3F) >> 0];
        index += 3;
    }
    if ((size % 3) == 1)
    {
        result += base64ToChar[(data.getData()[index + 0] & 0xFC) >> 2];
        result += base64ToChar[(data.getData()[index + 0] & 0x03) << 4];
        result += "==";
    }
    if ((size % 3) == 2)
    {
        result += base64ToChar[(data.getData()[index + 0] & 0xFC) >> 2];
        result += base64ToChar[(data.getData()[index + 0] & 0x03) << 4 | (data.getData()[index + 1] & 0xF0) >> 4];
        result += base64ToChar[(data.getData()[index + 1] & 0x0F) << 2];
        result += '=';
    }
    return result;
}
std::string Converter::calculateSha1Buffer(unsigned char* buffer, long size)
{
    Poco::SHA1Engine engine;
    engine.update(buffer, size);

    // finish HMAC computation and obtain digest
    const Poco::DigestEngine::Digest& digest = engine.digest();
    
    // convert to a string of hexadecimal numbers
    return Poco::DigestEngine::digestToHex(digest);
}

std::string Converter::calculateSha1OfFile(std::string pathFile)
{
    Poco::SHA1Engine engine;

    if (pathFile.empty()) {
        return "";
    }

    std::ifstream infile(pathFile, std::ios_base::in | std::ios_base::binary);

    //get length of file
    infile.seekg(0, std::ios::end);
    uint32_t length = infile.tellg();
    infile.seekg(0, std::ios::beg);

    //read file
    char bufferTemp[64];

    for (int i = 0; i < length; i+= 64) {
        if (i+63 <= (length-1)) {
            infile.read(bufferTemp, 64);
            std::string message(bufferTemp, 64);
            engine.update(message);
        }
        else {
            int size = length - i;
            infile.read(bufferTemp, size);
            std::string message(bufferTemp, size);
            engine.update(message);
        }
    }

    // finish HMAC computation and obtain digest
    const Poco::DigestEngine::Digest& digest = engine.digest();
    
    // convert to a string of hexadecimal numbers
    return Poco::DigestEngine::digestToHex(digest);
}