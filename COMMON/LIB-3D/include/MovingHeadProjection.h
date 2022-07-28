#include <cstdlib>
#include <iostream>
#include <queue>

#pragma once

#include "IceConfiguration.h"
#include "Template.h"
#include "image.h"

#define M_PI 3.14159265358979323846
#include <cmath>

#include "Poco/Logger.h"

class MovingHeadProjection
{

public:

    // -- structure to project Moving head
    struct OutputMovingHeadAngle {
        std::string name;
        Color color;
        double pan;
        double tilt;
    };

    MovingHeadProjection(IceConfiguration* ice);

    // -- set template to get back all extracted sub-images
    void setTemplateExtractor(Template* templateExtract) {this->templateExtract = templateExtract;}
    ~MovingHeadProjection();

    void clean();

    // -- used in real time when playing CIS
    bool projectMovingHeads();
    // -- used when a command to set MH are executed
    static OutputMovingHeadAngle* projectPanTiltFromRatio(IceConfiguration* ice, std::string deviceId, double ratioX, double ratioY);
    // -- get back all outputs datas from moving heads projections
    std::shared_ptr<OutputMovingHeadAngle> getOutput();

private:
    
    // -- give seat configuration and position of movingheads
    IceConfiguration* ice;

    // -- give proportional placement depending of the seat zone
    Template* templateExtract;

    std::queue<std::shared_ptr<OutputMovingHeadAngle>> outputs;
};