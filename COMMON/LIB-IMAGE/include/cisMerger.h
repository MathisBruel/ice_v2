#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>

#pragma once
#include "cis.h"
#include "image.h"
#include "Poco/Logger.h"

class CisMerger
{

public:

    CisMerger();
    ~CisMerger();

    void setImageFormat(uint32_t width, uint32_t height, uint32_t bpp, TimeCodeType frameRate);
    void addCis(Cis* cis) {cisList.push_back(cis);}

    bool save(std::string fileName, std::string type);

private:

    std::string type;
    TimeCodeType frameRate;
    uint32_t width;
    uint32_t height;
    uint32_t bpp;

    std::vector<Cis*> cisList;

    // -- for savings
    FILE* file;
};