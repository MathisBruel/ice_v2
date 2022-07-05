#include "Template.h"

Template::Template(std::string version)
{
    this->version = version;
}

Template::~Template()
{
    cleanDatas();
}

void Template::setImageTemplate(std::shared_ptr<Image> refImage)
{
    this->refImage = refImage;
}

void Template::cleanDatas() 
{
    // -- free accessor to ref image
    refImage.reset();

    // -- free accessor to back images
    imgPanelBackLeft.reset();
    imgPanelBackRight.reset();

    // -- clear vector and map object to automatically free datas when no more used
    std::map<std::string, std::shared_ptr<Image>>::iterator itPan;
    for (itPan = imgPanelForeLeft.begin(); itPan != imgPanelForeLeft.end(); itPan++) {
        itPan->second.reset();
    }
    imgPanelForeLeft.clear();
    for (itPan = imgPanelForeRight.begin(); itPan != imgPanelForeRight.end(); itPan++) {
        itPan->second.reset();
    }
    imgPanelForeRight.clear();
    std::map<std::string, std::shared_ptr<Template::MovingHeadsOutputs>>::iterator itMH;
    for (itMH = movingHeads.begin(); itMH != movingHeads.end(); itMH++) {
        itMH->second.reset();
    }
    movingHeads.clear();
    for (std::shared_ptr<Template::ParOutputs> par : pars) {
        par.reset();
    }
    pars.clear();
    for (std::shared_ptr<Template::BacklightOutputs> backlight : backlights) {
        backlight.reset();
    }
    backlights.clear();
}

std::shared_ptr<Image> Template::getImageForeLeft(std::string name)
{
    std::shared_ptr<Image> ret = nullptr;
    std::map<std::string, std::shared_ptr<Image>>::iterator it = imgPanelForeLeft.find(name);
    if (it != imgPanelForeLeft.end()) {
        ret = it->second;
    }
    return ret;
}
std::shared_ptr<Image> Template::getImageForeRight(std::string name)
{
    std::shared_ptr<Image> ret = nullptr;
    std::map<std::string, std::shared_ptr<Image>>::iterator it = imgPanelForeRight.find(name);
    if (it != imgPanelForeRight.end()) {
        ret = it->second;
    }
    return ret;
}

// -- get moving heads
std::shared_ptr<Template::MovingHeadsOutputs> Template::getMovingHeadOutputs(std::string name)
{
    std::shared_ptr<Template::MovingHeadsOutputs> ret = nullptr;
    std::map<std::string, std::shared_ptr<Template::MovingHeadsOutputs>>::iterator it = movingHeads.find(name);
    if (it != movingHeads.end()) {
        ret = it->second;
    }
    return ret;
}
