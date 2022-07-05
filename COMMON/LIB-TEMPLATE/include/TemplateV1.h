#include "Template.h"
#define M_PI 3.14159265358979323846
#include <cmath>

#pragma once
#include "Poco/Logger.h"

class TemplateV1 : public Template
{

public:

    TemplateV1();
    virtual ~TemplateV1();
    void extractBackLeftPanelsImage();
    void extractBackRightPanelsImage();
    void extractForeLeftPanelsImage();
    void extractForeRightPanelsImage();
    void extractMovingHeadsDatas();
    void extractPARDatas();
    void extractBacklightDatas();

private :

    // -- for panels
    Template::SquareImageDefinition panelDefinitionBackLeft;
    Template::SquareImageDefinition* panelDefinitionForeLeft;

    Template::SquareImageDefinition panelDefinitionBackRight;
    Template::SquareImageDefinition* panelDefinitionForeRight;

    // -- for Backlight
    Template::PointImageDefinition* backlightsDefinition;

    // -- for PAR
    Template::PointImageDefinition* parsDefinition;

    // -- for Moving heads
    Template::PointImageDefinition* movingHeadsDefinitionPan;
    Template::PointImageDefinition* movingHeadsDefinitionTilt;
    Template::PointImageDefinition* movingHeadsDefinitionColor;
};