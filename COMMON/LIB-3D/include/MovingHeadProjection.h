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

    struct OutputMovingHeadAngle {
        std::string name;
        Color color;
        double pan;
        double tilt;
    };

    MovingHeadProjection(IceConfiguration* ice);
    void setTemplateExtractor(Template* templateExtract) {this->templateExtract = templateExtract;}
    ~MovingHeadProjection();

    void clean();

    bool projectMovingHeads();
    static OutputMovingHeadAngle* projectPanTiltFromRatio(IceConfiguration* ice, std::string deviceId, double ratioX, double ratioY);

    std::shared_ptr<OutputMovingHeadAngle> getOutput();

private:
    
    // -- give seat configuration and position of movingheads
    IceConfiguration* ice;

    // -- give proportional placement depending of the seat zone
    Template* templateExtract;

    std::queue<std::shared_ptr<OutputMovingHeadAngle>> outputs;
};