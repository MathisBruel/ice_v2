#include <cstdlib>
#include <iostream>

#pragma once

#include "image.h"
#include "Poco/Logger.h"

class Template
{

public :

    struct SquareImageDefinition {
        std::string name;
        uint16_t offsetX;
        uint16_t offsetY;
        uint16_t width;
        uint16_t height;
    };

    struct PointImageDefinition {
        std::string name;
        uint16_t offsetX;
        uint16_t offsetY;
    };

    struct MovingHeadsOutputs {
        std::string name;
        Color color;
        double ratioPositionX;
        double ratioPositionY;
    };

    struct ParOutputs {
        std::string name;
        Color color;
    };

    struct BacklightOutputs {
        std::string name;
        Color color;
    };

    virtual ~Template();
    void setImageTemplate(std::shared_ptr<Image> refImage);
    virtual void extractBackLeftPanelsImage() {}
    virtual void extractBackRightPanelsImage() {}
    virtual void extractForeLeftPanelsImage() {}
    virtual void extractForeRightPanelsImage() {}
    virtual void extractMovingHeadsDatas() {}
    virtual void extractPARDatas() {}
    virtual void extractBacklightDatas() {}
    void cleanDatas();

    // -- generic
    std::string getVersion() {return version;}

    // -- get Panels
    std::shared_ptr<Image> getImageBackLeft() {return imgPanelBackLeft;}
    std::shared_ptr<Image> getImageBackRight() {return imgPanelBackRight;}

    std::shared_ptr<Image>getImageForeLeft(std::string name);
    std::shared_ptr<Image> getImageForeRight(std::string name);

    // -- get moving heads
    std::shared_ptr<MovingHeadsOutputs> getMovingHeadOutputs(std::string name);

    // -- get par
    int getNbPars() {return pars.size();}
    std::shared_ptr<ParOutputs> getParOutputs(int index) {return pars.at(index);}

    // -- get backlight
    int getNbBacklights() {return backlights.size();}
    std::shared_ptr<BacklightOutputs> getBacklightOutputs(int index) {return backlights.at(index);}

protected:

    Template(std::string version);

    // -- one implementation for each version
    std::string version;
    std::shared_ptr<Image> refImage;

    // -- for left panels : complete images
    std::shared_ptr<Image> imgPanelBackLeft;
    std::map<std::string, std::shared_ptr<Image>> imgPanelForeLeft;

    // -- for right panels : complete images
    std::shared_ptr<Image> imgPanelBackRight;
    std::map<std::string, std::shared_ptr<Image>> imgPanelForeRight;

    // -- for movingHeads illimited number
    std::map<std::string, std::shared_ptr<MovingHeadsOutputs>> movingHeads;

    // -- for PAR illimited number
    std::vector<std::shared_ptr<ParOutputs>> pars;

    // -- for BACKLIGHT illimited number
    std::vector<std::shared_ptr<BacklightOutputs>> backlights;
};
