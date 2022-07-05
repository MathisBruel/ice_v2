#include "ByteArray.h"

ByteArray::ByteArray() {}

ByteArray::ByteArray(uint32_t size)
{
    data = new uint8_t[size];
    this->size = size;
}
ByteArray::ByteArray(uint8_t* data, uint32_t size)
{
    this->data = new uint8_t[size];
    for (int i = 0; i < size; i++) {
        this->data[i] = data[i];
    }
    this->size = size;
}

ByteArray::~ByteArray() 
{
    delete[] data;
}

uint8_t* ByteArray::getCopyData()
{
    uint8_t* toReturn = new uint8_t[size];
    for (int i = 0; i < size; i++) {
        toReturn[i] = data[i];
    }
    return toReturn;
}

void ByteArray::resize(uint32_t newSize)
{
    uint8_t* newData = new uint8_t[newSize];
    if (newSize > size) {
        for (int i = 0; i < size; i++) {
            newData[i] = data[i];
        }
    }
    else {
        for (int i = 0; i < newSize; i++) {
            newData[i] = data[i];
        }
    }

    delete[] data;
    data = newData;
    size = newSize;
}

uint8_t* ByteArray::get(uint32_t offset, uint32_t length)
{
    if (offset < 0 || offset >= size || (offset+length-1) >= size) {

        Poco::Logger::get("ByteArray").error("Index out of range !", __FILE__, __LINE__);
        std::string details = "Offset : " + std::to_string(offset) + ", length : " + std::to_string(length) + ", size : " + std::to_string(size);
        //Poco::Logger::get("ByteArray").debug(details, __FILE__, __LINE__);
        return nullptr;
    }

    uint8_t* temp = new uint8_t[length];
    for (int i = 0; i < length; i++) {
        temp[i] = data[offset+i];
    }
    return temp;
}
uint8_t ByteArray::getUInt8(uint32_t offset)
{
    if (offset < 0 || offset >= size) {
        Poco::Logger::get("ByteArray").error("Index out of range !", __FILE__, __LINE__);
        std::string details = "Offset : " + std::to_string(offset) + ", size : " + std::to_string(size);
        //Poco::Logger::get("ByteArray").debug(details, __FILE__, __LINE__);
        return -1;
    }

    return data[offset];
}
uint16_t ByteArray::getUInt16(uint32_t offset, bool littleEndian)
{
    uint16_t value = 0;

    if (!littleEndian) {
        value += ((uint16_t)getUInt8(offset)) << 8;
        value += getUInt8(offset+1);
    } else {
        value += getUInt8(offset);
        value += ((uint16_t)getUInt8(offset+1)) << 8;
    }

    return value;
}
uint32_t ByteArray::getUInt32(uint32_t offset, bool littleEndian)
{
    uint32_t value = 0;
    if (!littleEndian) {
        value += ((uint32_t)getUInt16(offset, littleEndian)) << 16;
        value += getUInt16(offset+2, littleEndian);
    } else {
        value += getUInt16(offset, littleEndian);
        value += ((uint32_t)getUInt16(offset+2, littleEndian)) << 16;
    }

    return value;
}
uint64_t ByteArray::getUInt64(uint32_t offset, bool littleEndian)
{
    uint64_t value = 0;
    if (!littleEndian) {
        value += ((uint64_t)getUInt32(offset, littleEndian)) << 32;
        value += getUInt32(offset+4, littleEndian);
    } else {
        value += getUInt32(offset, littleEndian);
        value += ((uint64_t)getUInt32(offset+4, littleEndian)) << 32;
    }

    return value;
}

void ByteArray::set(uint8_t* value, uint32_t offset, uint32_t length)
{
    if (offset < 0 || offset >= size || (offset+length) > size) {
        Poco::Logger::get("ByteArray").error("Index out of range !", __FILE__, __LINE__);
        std::string details = "Offset : " + std::to_string(offset) + ", length : " + std::to_string(length) + ", size : " + std::to_string(size);
        //Poco::Logger::get("ByteArray").debug(details, __FILE__, __LINE__);
        return;
    }

    for (int i = 0; i < length; i++) {
        data[offset+i] = value[i];
    }
}
void ByteArray::setUInt8(uint8_t value, uint32_t offset)
{
    if (offset < 0 || offset >= size) {
        Poco::Logger::get("ByteArray").error("Index out of range !", __FILE__, __LINE__);
        std::string details = "Offset : " + std::to_string(offset) + ", size : " + std::to_string(size);
        //Poco::Logger::get("ByteArray").debug(details, __FILE__, __LINE__);
        return;
    }

    data[offset] = value;
}
void ByteArray::setUInt16(uint16_t value, uint32_t offset, bool littleEndian)
{
    if (!littleEndian) {
        setUInt8(value >> 8, offset);
        setUInt8(value & 0xff, offset+1);
    } else {
        setUInt8(value & 0xff, offset);
        setUInt8(value >> 8, offset+1);
    }
}
void ByteArray::setUInt32(uint32_t value, uint32_t offset, bool littleEndian)
{
    if (!littleEndian) {
        setUInt16((value & 0xffff0000) >> 16, offset, littleEndian);
        setUInt16((value & 0xffff), offset+2, littleEndian);
    } else {
        setUInt16((value & 0xffff), offset, littleEndian);
        setUInt16((value & 0xffff0000) >> 16, offset+2, littleEndian);
    }
}
void ByteArray::setUInt64(uint64_t value, uint32_t offset, bool littleEndian)
{
    if (!littleEndian) {

        setUInt32((value & 0xffffffff00000000) >> 32, offset, littleEndian);
        setUInt32((value & 0xffffffff), offset+4, littleEndian);
    } else {
        setUInt32((value & 0xffffffff), offset, littleEndian);
        setUInt32((value & 0xffffffff00000000) >> 32, offset+4, littleEndian);
    }
}

std::string ByteArray::toString()
{
    std::stringstream str;
    str << "size : " << size << "\n" << std::hex;
    for (int i = 0; i < size; i++) {
        str << std::setw(2) << std::setfill('0') << (int)data[i] << " ";
    }
    return str.str();
}
