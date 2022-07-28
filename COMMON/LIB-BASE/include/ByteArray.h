#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>

#pragma once
#include "Poco/Logger.h"

class ByteArray
{

public:

    // -- object that handle a bytearray
    // little endian or big endian 
    // for 8 to 32 bits data sized

    ByteArray();
    ByteArray(uint32_t size);
    ByteArray(uint8_t* data, uint32_t size);
    ~ByteArray();

    uint32_t getSize() {return size;}
    uint8_t* get(uint32_t offset, uint32_t length);
    uint8_t getUInt8(uint32_t offset);
    uint16_t getUInt16(uint32_t offset, bool littleEndian);
    uint32_t getUInt32(uint32_t offset, bool littleEndian);
    uint64_t getUInt64(uint32_t offset, bool littleEndian);

    void resize(uint32_t newSize);
    void setSize(uint32_t size) {this->size = size;}
    void set(uint8_t* value, uint32_t offset, uint32_t length);
    void setUInt8(uint8_t value, uint32_t offset);
    void setUInt16(uint16_t value, uint32_t offset, bool littleEndian);
    void setUInt32(uint32_t value, uint32_t offset, bool littleEndian);
    void setUInt64(uint64_t value, uint32_t offset, bool littleEndian);

    uint8_t* getData() {return data;}
    uint8_t* getCopyData();

    std::string toString();

private:

    uint8_t* data;
    uint32_t size;
};