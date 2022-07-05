#include <cstdlib>
#include <iostream>

#pragma once

#include "Template.h"
#include "IceConfiguration.h"
#include "KinetConfiguration.h"
#include "image.h"
#include "OutputPanelImage.h"

#include "Poco/Logger.h"
#include "Poco/Stopwatch.h"

class PanelProjection
{

public:

    enum ProjectionType {
        NO_FIT,         // -- only depending on height screen and proportional
        FIT_Y,          // -- fit all panels on Y, no try to fit on X 
        FIT_X,          // -- fit all panels on X, no try to fit on Y
        FIT_Y_PROP_X,   // -- fit all panels on Y, proportion of resize kept on X
        FIT_X_PROP_Y,   // -- fit all panels on Y, proportion of resize kept on X
        FIT_X_FIT_Y,    // -- fit all panels on X and Y, proportion of resize not kept
        UNKNOWN         // -- take projection of general ICe serveur CONTEXT
    };
    
    enum MixImageType {
        BLEND,
        MIX
    };

    PanelProjection(IceConfiguration* conf, KinetConfiguration* kinetConf, Template* templateExtract);
    ~PanelProjection();

    bool projectPanel(std::string panelName, ProjectionType type, MixImageType mix);
    
    std::shared_ptr<OutputPanelImage> getOutput() {return output;}

private : 

    Image* getForeImageProjection(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> foreRef);

    Image* getBackImageRightNoFit(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> backRef);
    Image* getBackImageLeftNoFit(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> backRef);

    Image* getBackImageRightFitX_Prop(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> backRef, int positionMaxY);
    Image* getBackImageLeftFitX_Prop(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> backRef, int positionMaxY);

    Image* getBackImageRightFitX(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> backRef, int positionMaxY);
    Image* getBackImageLeftFitX(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> backRef, int positionMaxY);

    Image* getBackImageRightFitY(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> backRef, int positionMinZ, int positionMaxZ);
    Image* getBackImageLeftFitY(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> backRef, int positionMinZ, int positionMaxZ);

    Image* getBackImageRightFitY_Prop(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> backRef, int positionMinZ, int positionMaxZ);
    Image* getBackImageLeftFitY_Prop(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> backRef, int positionMinZ, int positionMaxZ);

    Image* getBackImageLeftFitXY(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> backRef, int positionMinZ, int positionMaxZ, int positionMaxY);
    Image* getBackImageRightFitXY(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> backRef, int positionMinZ, int positionMaxZ, int positionMaxY);

    // -- where extract datas;
    Template* templateExtract;

    // -- configuration of all panels
    IceConfiguration* confPanels;
    KinetConfiguration* kinetConf;

    // -- results
    std::shared_ptr<OutputPanelImage> output;
};