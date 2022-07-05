#include <cstdlib>
#include <iostream>
#include <vector>

#pragma once 
#include "Poco/DOM/NodeList.h"
#include "Poco/DOM/Node.h"
#include "cplFile.h"

class CplInfos
{

public:

    CplInfos();
    ~CplInfos();

    void setCplId(std::string cplId) {this->cplId = cplId;}
    void setCplTitle(std::string cplTitle) {this->cplTitle = cplTitle;}
    void setcontentVersionId(std::string contentVersionId) {this->contentVersionId = contentVersionId;}
    void setCplType(CplFile::CplType type) {this->type = type;}
    void addProperties(CplFile::CplProperty property) {properties.push_back(property);}
    void setPlayable(bool playable) {this->playable = playable;}
    void set3D(bool is3D) {this->is3D = is3D;}
    void setSpeedRate(double speedRate) {this->speedRate = speedRate;}
    void setCplDuration(uint32_t cplDuration) {this->cplDuration = cplDuration;}
    void setContentFile(std::string contentFile) {this->contentFile = contentFile;}
    void setPictureWidth(int pictureWidth) {this->pictureWidth = pictureWidth;}

    std::string getCplId() {return cplId;}
    std::string getCplTitle() {return cplTitle;}
    std::string getcontentVersionId() {return contentVersionId;}
    CplFile::CplType getCplType() {return type;}
    std::vector<CplFile::CplProperty> getProperties() {return properties;}
    bool isPlayable() {return playable;}
    bool has3D() {return is3D;}
    double getSpeedRate() {return speedRate;}
    uint32_t getCplDuration() {return cplDuration;}
    std::string getContentFile() {return contentFile;}
    int getPictureWidth() {return pictureWidth;}

    std::string toString();
 
    // -- TODO to XML for API

private:

    std::string cplId;
    std::string cplTitle;
    std::string contentVersionId;

    CplFile::CplType type;
    std::vector<CplFile::CplProperty> properties;
    bool playable;
    bool is3D;

    int pictureWidth;
    
    double speedRate;
    uint32_t cplDuration;

    std::string contentFile;
};