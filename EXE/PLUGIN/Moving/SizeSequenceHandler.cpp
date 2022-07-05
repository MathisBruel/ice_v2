#include "SizeSequenceHandler.h"

SizeSeqHandler::SizeSeqHandler()
{
    std::string filename = "C:/ICE-EFFECT/idSize.tmp";

    std::ifstream tempFile;
    tempFile.open(filename, std::ios::in);

    if (tempFile.good()) {

        std::string id;
        long size;
        while (tempFile >> id >> size)
        {
            idSize.insert_or_assign(id, size);
        }

        tempFile.close();
    }
}

SizeSeqHandler::~SizeSeqHandler() {}

void SizeSeqHandler::deleteTempFile()
{
    std::string filename = "C:/ICE-EFFECT/idSize.tmp";
    std::remove(filename.c_str());
}

void SizeSeqHandler::addId(std::string id, long size)
{
    idSize.insert_or_assign(id, size);
 
    std::string filename = "C:/ICE-EFFECT/idSize.tmp";
    std::ofstream tempFile;
    tempFile.open(filename, std::ios::out | std::ios::trunc);
    
    std::map<std::string, long>::iterator it;
    for (it = idSize.begin(); it != idSize.end(); it++) {
        tempFile << it->first << " " << it->second << "\n";
    }

    tempFile.close();
}
void SizeSeqHandler::removeId(std::string id)
{
    idSize.erase(id);

    std::string filename = "C:/ICE-EFFECT/idSize.tmp";
    std::ofstream tempFile;
    tempFile.open(filename, std::ios::out | std::ios::trunc);
    
    std::map<std::string, long>::iterator it;
    for (it = idSize.begin(); it != idSize.end(); it++) {
        tempFile << it->first << " " << it->second << "\n";
    }

    tempFile.close();
}
long SizeSeqHandler::getSizeForId(std::string id)
{
    long size = -1;
    std::map<std::string, long>::iterator it = idSize.find(id);
    if (it != idSize.end()) {
        size = it->second;
    }
    return size;
}