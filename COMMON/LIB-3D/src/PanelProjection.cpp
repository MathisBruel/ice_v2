#include "PanelProjection.h"

PanelProjection::PanelProjection(IceConfiguration* conf, KinetConfiguration* kinetConf, Template* templateExtract)
{
    this->templateExtract = templateExtract;
    confPanels = conf;
    this->kinetConf = kinetConf;
    output = nullptr;
}
PanelProjection::~PanelProjection() 
{
    output.reset();
}

bool PanelProjection::projectPanel(std::string panelName, ProjectionType type, PanelProjection::MixImageType mix)
{
    // -- we see large
    int positionMaxZ = 0;
    int positionMinZ = 0x00ffffff;
    int positionMaxY = 0;

    IceConfiguration::PanelConf *panel = confPanels->getPanel(panelName);
    KinetConfiguration::PanelConf *kinet = kinetConf->getPanel(panelName);

    // -- get min and max Z for panels to get referential in template image
    for (int i = 0; i < confPanels->getNbPanels(); i++) {
        if (confPanels->getPanel(i)->side == panel->side) {
            if (confPanels->getPanel(i)->offsetZ+confPanels->getPanel(i)->height > positionMaxZ) {
                positionMaxZ = confPanels->getPanel(i)->offsetZ+confPanels->getPanel(i)->height;
            }
            if (confPanels->getPanel(i)->offsetZ < positionMinZ) {
                positionMinZ = confPanels->getPanel(i)->offsetZ;
            }

            if (confPanels->getPanel(i)->offsetY+confPanels->getPanel(i)->width > positionMaxY) {
                positionMaxY = confPanels->getPanel(i)->offsetY+confPanels->getPanel(i)->width;
            }
        }
    }

    if (panel->side == IceConfiguration::LEFT) {

        std::shared_ptr<Image> foreRef = templateExtract->getImageForeLeft(panelName);

        if (kinet == nullptr || foreRef == nullptr) {
            Poco::Logger::get("PanelProjection").error("Can not find kinet configuration of panel or image front not found for " + panelName, __FILE__, __LINE__);
            foreRef.reset();
            return false;
        }

        Image* frontPanelImg = getForeImageProjection(panel, kinet, foreRef);
        if (frontPanelImg == nullptr) {
            Poco::Logger::get("PanelProjection").error("Error when projecting front images !", __FILE__, __LINE__);
            foreRef.reset();
            return false;
        }

        Image* backPanelImg = nullptr;
        switch (type) {
            case NO_FIT:
            backPanelImg = getBackImageLeftNoFit(panel, kinet, templateExtract->getImageBackLeft());
            break;

            case FIT_X:
            backPanelImg = getBackImageLeftFitX(panel, kinet, templateExtract->getImageBackLeft(), positionMaxY);
            break;

            case FIT_Y:
            backPanelImg = getBackImageLeftFitY(panel, kinet, templateExtract->getImageBackLeft(), positionMinZ, positionMaxZ);
            break;

            case FIT_X_PROP_Y:
            backPanelImg = getBackImageLeftFitX_Prop(panel, kinet, templateExtract->getImageBackLeft(), positionMaxY);
            break;

            case FIT_Y_PROP_X:
            backPanelImg = getBackImageLeftFitY_Prop(panel, kinet, templateExtract->getImageBackLeft(), positionMinZ, positionMaxZ);
            break;

            case FIT_X_FIT_Y:
            backPanelImg = getBackImageLeftFitXY(panel, kinet, templateExtract->getImageBackLeft(), positionMinZ, positionMaxZ, positionMaxY);
            break;
        }

        // -- we merge back and fore final images 
        if (backPanelImg == nullptr) {
            Poco::Logger::get("PanelProjection").error("Error when projecting back images ! ", __FILE__, __LINE__);
            delete frontPanelImg;
            foreRef.reset();
            return false;
        }

        Image* merged = nullptr;
        switch (mix) {
            
            case BLEND:
            merged = backPanelImg->blendWithForeImage(frontPanelImg);
            break;

            case MIX:
            merged = backPanelImg->mixWithForeImage(frontPanelImg);
            break;
        }

        std::shared_ptr<Image> mergedPtr = std::shared_ptr<Image>(merged);
        // -- set final struture (deletion of pointer is done in thread kinet after treatment)
        output = std::make_shared<OutputPanelImage>(OutputPanelImage(panelName, mergedPtr));
        mergedPtr.reset();
        foreRef.reset();
        delete frontPanelImg;
        delete backPanelImg;
    }
    
    else if (panel->side == IceConfiguration::RIGHT) {
      
        std::shared_ptr<Image> foreRef = templateExtract->getImageForeRight(panelName);
        
        if (kinet == nullptr || foreRef == nullptr) {
            Poco::Logger::get("PanelProjection").error("Can not find kinet configuration of panel or image front not found for " + panelName, __FILE__, __LINE__);
            foreRef.reset();
            return false;
        }
        
        Image* frontPanelImg = getForeImageProjection(panel, kinet, foreRef);
        if (frontPanelImg == nullptr) {
            Poco::Logger::get("PanelProjection").error("Error when projecting fore images ! ", __FILE__, __LINE__);
            foreRef.reset();
            return false;
        }

        Image* backPanelImg = nullptr;
        switch (type) {
            case NO_FIT:
            backPanelImg = getBackImageRightNoFit(panel, kinet, templateExtract->getImageBackRight());
            break;

            case FIT_X:
            backPanelImg = getBackImageRightFitX(panel, kinet, templateExtract->getImageBackRight(), positionMaxY);
            break;

            case FIT_Y:
            backPanelImg = getBackImageRightFitY(panel, kinet, templateExtract->getImageBackRight(), positionMinZ, positionMaxZ);
            break;

            case FIT_X_PROP_Y:
            backPanelImg = getBackImageRightFitX_Prop(panel, kinet, templateExtract->getImageBackRight(), positionMaxY);
            break;

            case FIT_Y_PROP_X:
            backPanelImg = getBackImageRightFitY_Prop(panel, kinet, templateExtract->getImageBackRight(), positionMinZ, positionMaxZ);
            break;

            case FIT_X_FIT_Y:
            backPanelImg = getBackImageRightFitXY(panel, kinet, templateExtract->getImageBackRight(), positionMinZ, positionMaxZ, positionMaxY);
            break;
        }

        // -- we merge back and fore final images
        if (backPanelImg == nullptr) {
            Poco::Logger::get("PanelProjection").error("Error when projecting back images ! ", __FILE__, __LINE__);
            delete frontPanelImg;
            foreRef.reset();
            return false;
        }

        Image* merged = nullptr;
        switch (mix) {
            
            case BLEND:
            merged = backPanelImg->blendWithForeImage(frontPanelImg);
            break;

            case MIX:
            merged = backPanelImg->mixWithForeImage(frontPanelImg);
            break;
        }

        // -- set final struture (deletion of pointer is done in thread kinet after treatment)
        std::shared_ptr<Image> mergedPtr = std::shared_ptr<Image>(merged);
        output = std::make_shared<OutputPanelImage>(OutputPanelImage(panelName, mergedPtr));
        delete frontPanelImg;
        delete backPanelImg;
        mergedPtr.reset();
        foreRef.reset();
    }
    
    else {
        Poco::Logger::get("PanelProjection").error("Unknown side : " + std::to_string(panel->side), __FILE__, __LINE__);
    }
    return true;
}

Image* PanelProjection::getForeImageProjection(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> foreRef)
{
    // -- determine square to extract data from
    // -- width is always full width in source image 
    double pixelSrcOffsetX = 0;
    double paddingX = 0;
    double paddingY = 0;
    double pixelSrcWidth = foreRef->getWidth();

    // -- ratio to pass from dimension in mm to pixels
    double ratioMmToPixel = (double)foreRef->getWidth() / (double)panel->width;

    // -- offset Y depends of the ratio on width : center of the panel is center of the image
    double pixelSrcOffsetY = (double)(foreRef->getHeight())/2.0 - (double)((panel->height)/2.0)*ratioMmToPixel;
    double pixelSrcHeight = (double)(panel->height) * ratioMmToPixel;

    // -- we determine ratio between image extracted and resolution of the panel
    double ratioPanelResolutionX = (double)(panel->width) / (double)(kinet->sizePixel) / pixelSrcWidth;
    double ratioPanelResolutionY = (double)(panel->height) / (double)(kinet->sizePixel) / pixelSrcHeight;

    // -- check offsetY is not out of bounds
    if (pixelSrcOffsetY < 0) {
        pixelSrcHeight += pixelSrcOffsetY; // height is reduced
        paddingY = -pixelSrcOffsetY*ratioPanelResolutionY; // -- determine zone to paint black in final image
        pixelSrcOffsetY = 0;
    }
    if (pixelSrcOffsetY + pixelSrcHeight > foreRef->getHeight()) {
        pixelSrcHeight = (double)(foreRef->getHeight()) - pixelSrcOffsetY;
    }

    // -- extract from source image
    Image* extractedFore = foreRef->extractData((int)(pixelSrcOffsetX+0.5), (int)(pixelSrcOffsetY+0.5), (int)(pixelSrcWidth+0.5), (int)(pixelSrcHeight+0.5));

    // -- resize to fill resolution of the panel
    int panelPixelWidth = pixelSrcWidth*ratioPanelResolutionX + 0.5;
    int panelPixelHeight = pixelSrcHeight*ratioPanelResolutionY + 0.5;

    extractedFore->resizeBell(panelPixelWidth, panelPixelHeight);

    // -- fill black image resized to match size of panel
    // -- always full size of resized image and full width height in destination
    // -- only padding may vary depending if cropping has been done in Y (never on X) 
    Image* img = extractedFore->extractDataWithFillBlack(0, 0, extractedFore->getWidth(), extractedFore->getHeight(),
                                                            (int)(paddingX+0.5), (int)(paddingY+0.5), panel->width / kinet->sizePixel, panel->height / kinet->sizePixel);
    delete extractedFore;
    foreRef.reset();
    return img;
}

Image* PanelProjection::getBackImageLeftNoFit(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> backRef)
{
    // -- we start over the back image (just we do not take into account the offset between screen and first panel)
    IceConfiguration::PanelConf * L1 = confPanels->getPanel("L1");
    if (L1 == nullptr) {
        Poco::Logger::get("PanelProjection").error("Panel L1 is not defined : we can't know the left offset for starting back image ! ", __FILE__, __LINE__);
        return nullptr;
    }
    int offsetYNotCountingBack = L1->offsetY;

    // -- margin to take data from in template
    int marginLeftRight = 4;
    // -- ratio to transform size in mm to pixel (based on height of screen without margin pixels in template)
    double ratioMmToPixelBack = (double)(backRef->getHeight()) / (double)(confPanels->getScreen()->height);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioMmToPixelBack : " + std::to_string(ratioMmToPixelBack), __FILE__, __LINE__);

    // -- we determine the size of the attended pixels square in image
    double pixelSrcOffsetY =  (double)(confPanels->getScreen()->height+confPanels->getScreen()->offsetZ - (panel->offsetZ+panel->height)) * ratioMmToPixelBack;
    double pixelSrcHeight = (double)(panel->height) * ratioMmToPixelBack;
    double pixelSrcWidth = (double)(panel->width) * ratioMmToPixelBack;
    double pixelSrcOffsetX = (double)(backRef->getWidth()) - ((double)(panel->offsetY-offsetYNotCountingBack) * ratioMmToPixelBack) - pixelSrcWidth - marginLeftRight;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetY : " + std::to_string(pixelSrcOffsetY), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcHeight : " + std::to_string(pixelSrcHeight), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcWidth : " + std::to_string(pixelSrcWidth), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetX : " + std::to_string(pixelSrcOffsetX), __FILE__, __LINE__);

    // -- padding is used to place the final resized image in image for panel (offsets) 
    double paddingX = 0;
    double paddingY = 0;

    // -- we determine ratio between size pixels obtained by dimension and resolution of panel
    double ratioPanelResolutionX = (double)(panel->width) / (double)(kinet->sizePixel) / pixelSrcWidth;
    double ratioPanelResolutionY = (double)(panel->height) / (double)(kinet->sizePixel) / pixelSrcHeight;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioPanelResolutionX : " + std::to_string(ratioPanelResolutionX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioPanelResolutionY : " + std::to_string(ratioPanelResolutionY), __FILE__, __LINE__);

    // -- correction of position if out of image
    if (pixelSrcOffsetY < 0) {
        pixelSrcHeight += pixelSrcOffsetY;
        paddingY = -pixelSrcOffsetY*ratioPanelResolutionY;
        pixelSrcOffsetY = 0;
    }
    if (pixelSrcOffsetY + pixelSrcHeight > backRef->getHeight()) {
        pixelSrcHeight = (double)(backRef->getHeight()) - pixelSrcOffsetY;
    }
    if (pixelSrcOffsetX < 0) {
        paddingX = -pixelSrcOffsetX*ratioPanelResolutionX;
        pixelSrcWidth += pixelSrcOffsetX;
        pixelSrcOffsetX = 0;
    }

    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetY : " + std::to_string(pixelSrcOffsetY), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcHeight : " + std::to_string(pixelSrcHeight), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcWidth : " + std::to_string(pixelSrcWidth), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetX : " + std::to_string(pixelSrcOffsetX), __FILE__, __LINE__);

    // -- we extract image from dimension versus screen
    Image* extractedBack = backRef->extractData((int)(pixelSrcOffsetX+0.5), (int)(pixelSrcOffsetY+0.5), (int)(pixelSrcWidth+0.5), (int)(pixelSrcHeight+0.5));
    
    // -- we resize image to fill resolution of panel
    int panelPixelWidth = pixelSrcWidth*ratioPanelResolutionX + 0.5;
    int panelPixelHeight = pixelSrcHeight*ratioPanelResolutionY + 0.5;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " panelPixelWidth : " + std::to_string(panelPixelWidth), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " panelPixelHeight : " + std::to_string(panelPixelHeight), __FILE__, __LINE__);

    if (!extractedBack->resizeBell(panelPixelWidth, panelPixelHeight)) {
        Poco::Logger::get("PanelProjection").error("Hermite resize failed ! ", __FILE__, __LINE__);
        return nullptr;
    }

    //Poco::Logger::get("PanelProjection").debug(panel->name + " paddingX : " + std::to_string(paddingX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " paddingY : " + std::to_string(paddingY), __FILE__, __LINE__);

    Image* img = extractedBack->extractDataWithFillBlack(0, 0, extractedBack->getWidth(), extractedBack->getHeight(),
                                                            (int)(paddingX+0.5), (int)(paddingY+0.5), panel->width / kinet->sizePixel, panel->height / kinet->sizePixel);
    
    delete extractedBack;
    backRef.reset();
    return img;                                                 
}
Image* PanelProjection::getBackImageRightNoFit(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> backRef)
{
    // -- we start over the back image (just we do not take into account the offset between screen and first panel)
    IceConfiguration::PanelConf * R1 = confPanels->getPanel("R1");
    if (R1 == nullptr) {
        Poco::Logger::get("PanelProjection").error("Panel R1 is not defined : we can't know the left offset for starting back image ! ", __FILE__, __LINE__);
        return nullptr;
    }
    int offsetYNotCountingBack = R1->offsetY;

    // -- margin to take data from in template
    int marginLeftRight = 4;
    // -- ratio to transform size in mm to pixel (based on height of screen without margin pixels in template)
    double ratioMmToPixelBack = (double)(backRef->getHeight()) / (double)(confPanels->getScreen()->height);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioMmToPixelBack : " + std::to_string(ratioMmToPixelBack), __FILE__, __LINE__);

    // -- we determine the size of the attended pixels square in image
    double pixelSrcOffsetY =  (double)(confPanels->getScreen()->height+confPanels->getScreen()->offsetZ - (panel->offsetZ+panel->height)) * ratioMmToPixelBack;
    double pixelSrcHeight = (double)(panel->height) * ratioMmToPixelBack;
    double pixelSrcOffsetX = (double)(panel->offsetY-offsetYNotCountingBack) * ratioMmToPixelBack + marginLeftRight;
    double pixelSrcWidth = (double)(panel->width) * ratioMmToPixelBack;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetY : " + std::to_string(pixelSrcOffsetY), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcHeight : " + std::to_string(pixelSrcHeight), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetX : " + std::to_string(pixelSrcOffsetX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcWidth : " + std::to_string(pixelSrcWidth), __FILE__, __LINE__);

    // -- padding is used to place the final resized image in image for panel (offsets) 
    double paddingX = 0;
    double paddingY = 0;

    // -- we determine ratio between size pixels obtained by dimension and resolution of panel
    double ratioPanelResolutionX = (double)(panel->width) / (double)(kinet->sizePixel) / pixelSrcWidth;
    double ratioPanelResolutionY = (double)(panel->height) / (double)(kinet->sizePixel) / pixelSrcHeight;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioPanelResolutionX : " + std::to_string(ratioPanelResolutionX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioPanelResolutionY : " + std::to_string(ratioPanelResolutionY), __FILE__, __LINE__);

    // -- correction of position if out of image
    if (pixelSrcOffsetY < 0) {
        pixelSrcHeight += pixelSrcOffsetY;
        paddingY = -pixelSrcOffsetY*ratioPanelResolutionY;
        pixelSrcOffsetY = 0;
    }
    if (pixelSrcOffsetY + pixelSrcHeight > backRef->getHeight()) {
        pixelSrcHeight = (double)(backRef->getHeight()) - pixelSrcOffsetY;
    }
    if (pixelSrcOffsetX + pixelSrcWidth > backRef->getWidth() - marginLeftRight) {
        pixelSrcWidth = (double)(backRef->getWidth() - marginLeftRight) - pixelSrcOffsetX;
    }

    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetY : " + std::to_string(pixelSrcOffsetY), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcHeight : " + std::to_string(pixelSrcHeight), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcWidth : " + std::to_string(pixelSrcWidth), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetX : " + std::to_string(pixelSrcOffsetX), __FILE__, __LINE__);

     // -- we extract image from dimension versus screen
    Image* extractedBack = backRef->extractData((int)(pixelSrcOffsetX+0.5), (int)(pixelSrcOffsetY+0.5), (int)(pixelSrcWidth+0.5), (int)(pixelSrcHeight+0.5));
    
    // -- we resize image to fill resolution of panel
    int panelPixelWidth = pixelSrcWidth*ratioPanelResolutionX + 0.5;
    int panelPixelHeight = pixelSrcHeight*ratioPanelResolutionY + 0.5;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " panelPixelWidth : " + std::to_string(panelPixelWidth), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " panelPixelHeight : " + std::to_string(panelPixelHeight), __FILE__, __LINE__);

    extractedBack->resizeBell(panelPixelWidth, panelPixelHeight);

    //Poco::Logger::get("PanelProjection").debug(panel->name + " paddingX : " + std::to_string(paddingX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " paddingY : " + std::to_string(paddingY), __FILE__, __LINE__);

    Image* img = extractedBack->extractDataWithFillBlack(0, 0, extractedBack->getWidth(), extractedBack->getHeight(),
                                                            (int)(paddingX+0.5), (int)(paddingY+0.5), panel->width / kinet->sizePixel, panel->height / kinet->sizePixel);
    delete extractedBack;
    backRef.reset();
    return img;
}

Image* PanelProjection::getBackImageLeftFitY_Prop(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> backRef, int positionMinZ, int positionMaxZ)
{
    // -- we start over the back image (just we do not take into account the offset between screen and first panel)
    IceConfiguration::PanelConf * L1 = confPanels->getPanel("L1");
    if (L1 == nullptr) {
        Poco::Logger::get("PanelProjection").error("Panel L1 is not defined : we can't know the left offset for starting back image ! ", __FILE__, __LINE__);
        return nullptr;
    }
    int offsetYNotCountingBack = L1->offsetY;

    int marginLeftRight = 4;

    // -- center of screen is center of image source
    int mireZ = confPanels->getScreen()->offsetZ+confPanels->getScreen()->height/2;
    // -- we determine size in mm of the image (maximazed size in panel based of mire center)
    int deltaMax = std::max(mireZ - positionMinZ, positionMaxZ - mireZ);
        // -- ratio to transform size in mm to pixel (based on height)
    double ratioMmToPixelBack = (double)(backRef->getHeight()) / (deltaMax*2.0);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioMmToPixelBack : " + std::to_string(ratioMmToPixelBack), __FILE__, __LINE__);

    // -- we determine the size of the attended pixels square in image
    double pixelSrcOffsetY =  (double)(mireZ+deltaMax - (panel->offsetZ+panel->height)) * ratioMmToPixelBack;
    double pixelSrcHeight = (double)(panel->height) * ratioMmToPixelBack;
    double pixelSrcWidth = (double)(panel->width) * ratioMmToPixelBack;
    double pixelSrcOffsetX = (double)(backRef->getWidth()) - (double)(panel->offsetY-offsetYNotCountingBack) * ratioMmToPixelBack - pixelSrcWidth - marginLeftRight;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetY : " + std::to_string(pixelSrcOffsetY), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcHeight : " + std::to_string(pixelSrcHeight), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetX : " + std::to_string(pixelSrcOffsetX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcWidth : " + std::to_string(pixelSrcWidth), __FILE__, __LINE__);

    // -- padding is used to place the final resized image in image for panel (offsets) 
    double paddingX = 0;
    double paddingY = 0;

    // -- we determine ratio between size pixels obtained by dimension and resolution of panel
    double ratioPanelResolutionX = (double)(panel->width) / (double)(kinet->sizePixel) / pixelSrcWidth;
    double ratioPanelResolutionY = (double)(panel->height) / (double)(kinet->sizePixel) / pixelSrcHeight;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioPanelResolutionX : " + std::to_string(ratioPanelResolutionX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioPanelResolutionY : " + std::to_string(ratioPanelResolutionY), __FILE__, __LINE__);

    if (pixelSrcOffsetX < 0) {
        paddingX = -pixelSrcOffsetX*ratioPanelResolutionX;
        pixelSrcWidth += pixelSrcOffsetX;
        pixelSrcOffsetX = 0;
    }

    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetY : " + std::to_string(pixelSrcOffsetY), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcHeight : " + std::to_string(pixelSrcHeight), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetX : " + std::to_string(pixelSrcOffsetX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcWidth : " + std::to_string(pixelSrcWidth), __FILE__, __LINE__);

    // -- we extract image from dimension versus screen
    Image* extractedBack = backRef->extractData((int)(pixelSrcOffsetX+0.5), (int)(pixelSrcOffsetY+0.5), (int)(pixelSrcWidth+0.5), (int)(pixelSrcHeight+0.5));
    
    // -- we resize image to fill resolution of panel
    int panelPixelWidth = pixelSrcWidth*ratioPanelResolutionX + 0.5;
    int panelPixelHeight = pixelSrcHeight*ratioPanelResolutionY + 0.5;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " panelPixelWidth : " + std::to_string(panelPixelWidth), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " panelPixelHeight : " + std::to_string(panelPixelHeight), __FILE__, __LINE__);

    extractedBack->resizeBell(panelPixelWidth, panelPixelHeight);

    //Poco::Logger::get("PanelProjection").debug(panel->name + " paddingX : " + std::to_string(paddingX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " paddingY : " + std::to_string(paddingY), __FILE__, __LINE__);

    Image* img = extractedBack->extractDataWithFillBlack(0, 0, extractedBack->getWidth(), extractedBack->getHeight(),
                                                            (int)(paddingX+0.5), (int)(paddingY+0.5), panel->width / kinet->sizePixel, panel->height / kinet->sizePixel);
    delete extractedBack;
    backRef.reset();
    return img;
}
Image* PanelProjection::getBackImageRightFitY_Prop(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> backRef, int positionMinZ, int positionMaxZ)
{
    // -- we start over the back image (just we do not take into account the offset between screen and first panel)
    IceConfiguration::PanelConf * R1 = confPanels->getPanel("R1");
    if (R1 == nullptr) {
        Poco::Logger::get("PanelProjection").error("Panel R1 is not defined : we can't know the left offset for starting back image ! ", __FILE__, __LINE__);
        return nullptr;
    }
    int offsetYNotCountingBack = R1->offsetY;

    int marginLeftRight = 4;

    // -- center of screen is center of image source
    int mireZ = confPanels->getScreen()->offsetZ+confPanels->getScreen()->height/2;
    // -- we determine size in mm of the image (maximazed size in panel based of mire center)
    int deltaMax = std::max(mireZ - positionMinZ, positionMaxZ - mireZ);
    // -- ratio to transform size in mm to pixel (based on height)
    double ratioMmToPixelBack = (double)(backRef->getHeight() / (deltaMax*2.0));
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioMmToPixelBack : " + std::to_string(ratioMmToPixelBack), __FILE__, __LINE__);

    // -- we determine the size of the attended pixels square in image
    double pixelSrcOffsetY =  (double)(mireZ+deltaMax - (panel->offsetZ+panel->height)) * ratioMmToPixelBack;
    double pixelSrcHeight = (double)(panel->height) * ratioMmToPixelBack;
    double pixelSrcOffsetX = (double)(panel->offsetY-offsetYNotCountingBack) * ratioMmToPixelBack + marginLeftRight;
    double pixelSrcWidth = (double)(panel->width) * ratioMmToPixelBack;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetY : " + std::to_string(pixelSrcOffsetY), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcHeight : " + std::to_string(pixelSrcHeight), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetX : " + std::to_string(pixelSrcOffsetX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcWidth : " + std::to_string(pixelSrcWidth), __FILE__, __LINE__);

    // -- padding is used to place the final resized image in image for panel (offsets) 
    double paddingX = 0;
    double paddingY = 0;

    // -- we determine ratio between size pixels obtained by dimension and resolution of panel
    double ratioPanelResolutionX = (double)(panel->width) / (double)(kinet->sizePixel) / pixelSrcWidth;
    double ratioPanelResolutionY = (double)(panel->height) / (double)(kinet->sizePixel) / pixelSrcHeight;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioPanelResolutionX : " + std::to_string(ratioPanelResolutionX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioPanelResolutionY : " + std::to_string(ratioPanelResolutionY), __FILE__, __LINE__);

    if (pixelSrcOffsetX + pixelSrcWidth > backRef->getWidth() - marginLeftRight) {
        pixelSrcWidth = (double)(backRef->getWidth() - marginLeftRight) - pixelSrcOffsetX;
    }

    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetY : " + std::to_string(pixelSrcOffsetY), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcHeight : " + std::to_string(pixelSrcHeight), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetX : " + std::to_string(pixelSrcOffsetX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcWidth : " + std::to_string(pixelSrcWidth), __FILE__, __LINE__);

    // -- we extract image from dimension versus screen
    Image* extractedBack = backRef->extractData((int)(pixelSrcOffsetX+0.5), (int)(pixelSrcOffsetY+0.5), (int)(pixelSrcWidth+0.5), (int)(pixelSrcHeight+0.5));
    
    // -- we resize image to fill resolution of panel
    int panelPixelWidth = (double)pixelSrcWidth*ratioPanelResolutionX + 0.5;
    int panelPixelHeight = (double)pixelSrcHeight*ratioPanelResolutionY + 0.5;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " panelPixelWidth : " + std::to_string(panelPixelWidth), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " panelPixelHeight : " + std::to_string(panelPixelHeight), __FILE__, __LINE__);

    extractedBack->resizeBell(panelPixelWidth, panelPixelHeight);

    //Poco::Logger::get("PanelProjection").debug(panel->name + " paddingX : " + std::to_string(paddingX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " paddingY : " + std::to_string(paddingY), __FILE__, __LINE__);

    Image* img = extractedBack->extractDataWithFillBlack(0, 0, extractedBack->getWidth(), extractedBack->getHeight(),
                                                            (int)(paddingX+0.5), (int)(paddingY+0.5), panel->width / kinet->sizePixel, panel->height / kinet->sizePixel);
    delete extractedBack;
    backRef.reset();
    return img;
}

Image* PanelProjection::getBackImageRightFitX_Prop(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> backRef, int positionMaxY)
{
    // -- we start over the back image (just we do not take into account the offset between screen and first panel)
    IceConfiguration::PanelConf * R1 = confPanels->getPanel("R1");
    if (R1 == nullptr) {
        Poco::Logger::get("PanelProjection").error("Panel R1 is not defined : we can't know the left offset for starting back image ! ", __FILE__, __LINE__);
        return nullptr;
    }
    int offsetYNotCountingBack = R1->offsetY;

    // -- margin to take data from in template
    int marginLeftRight = 4;
    // -- ratio to transform size in mm to pixel (based on height of screen without margin pixels in template)
    double ratioMmToPixelBack = (double)(backRef->getWidth() - marginLeftRight) / (double)(positionMaxY-offsetYNotCountingBack);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioMmToPixelBack : " + std::to_string(ratioMmToPixelBack), __FILE__, __LINE__);

    // -- we determine the size of the attended pixels square in image
    double pixelSrcOffsetY =  (double)(confPanels->getScreen()->height+confPanels->getScreen()->offsetZ - (panel->offsetZ+panel->height)) * ratioMmToPixelBack;
    double pixelSrcHeight = (double)(panel->height) * ratioMmToPixelBack;
    double pixelSrcOffsetX = (double)(panel->offsetY-offsetYNotCountingBack) * ratioMmToPixelBack + marginLeftRight;
    double pixelSrcWidth = (double)(panel->width) * ratioMmToPixelBack;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetY : " + std::to_string(pixelSrcOffsetY), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcHeight : " + std::to_string(pixelSrcHeight), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetX : " + std::to_string(pixelSrcOffsetX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcWidth : " + std::to_string(pixelSrcWidth), __FILE__, __LINE__);

    // -- padding is used to place the final resized image in image for panel (offsets) 
    double paddingX = 0;
    double paddingY = 0;

    // -- we determine ratio between size pixels obtained by dimension and resolution of panel
    double ratioPanelResolutionX = (double)(panel->width) / (double)(kinet->sizePixel) / pixelSrcWidth;
    double ratioPanelResolutionY = (double)(panel->height) / (double)(kinet->sizePixel) / pixelSrcHeight;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioPanelResolutionX : " + std::to_string(ratioPanelResolutionX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioPanelResolutionY : " + std::to_string(ratioPanelResolutionY), __FILE__, __LINE__);

    // -- correction of position if out of image
    if (pixelSrcOffsetY < 0) {
        pixelSrcHeight += pixelSrcOffsetY;
        paddingY = -pixelSrcOffsetY*ratioPanelResolutionY;
        pixelSrcOffsetY = 0;
    }
    if (pixelSrcOffsetY + pixelSrcHeight > backRef->getHeight() - marginLeftRight) {
        pixelSrcHeight = (double)(backRef->getHeight() - marginLeftRight) - pixelSrcOffsetY;
    }

    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetY : " + std::to_string(pixelSrcOffsetY), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcHeight : " + std::to_string(pixelSrcHeight), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetX : " + std::to_string(pixelSrcOffsetX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcWidth : " + std::to_string(pixelSrcWidth), __FILE__, __LINE__);

    // -- we extract image from dimension versus screen
    Image* extractedBack = backRef->extractData((int)(pixelSrcOffsetX+0.5), (int)(pixelSrcOffsetY+0.5), (int)(pixelSrcWidth+0.5), (int)(pixelSrcHeight+0.5));
    
    // -- we resize image to fill resolution of panel
    int panelPixelWidth = pixelSrcWidth*ratioPanelResolutionX + 0.5;
    int panelPixelHeight = pixelSrcHeight*ratioPanelResolutionY + 0.5;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " panelPixelWidth : " + std::to_string(panelPixelWidth), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " panelPixelHeight : " + std::to_string(panelPixelHeight), __FILE__, __LINE__);

    extractedBack->resizeBell(panelPixelWidth, panelPixelHeight);

    //Poco::Logger::get("PanelProjection").debug(panel->name + " paddingX : " + std::to_string(paddingX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " paddingY : " + std::to_string(paddingY), __FILE__, __LINE__);

    Image* img = extractedBack->extractDataWithFillBlack(0, 0, extractedBack->getWidth(), extractedBack->getHeight(),
                                                            (int)(paddingX+0.5), (int)(paddingY+0.5), panel->width / kinet->sizePixel, panel->height / kinet->sizePixel);
    delete extractedBack;
    backRef.reset();
    return img;
}
Image* PanelProjection::getBackImageLeftFitX_Prop(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> backRef, int positionMaxY)
{
    // -- we start over the back image (just we do not take into account the offset between screen and first panel)
    IceConfiguration::PanelConf * L1 = confPanels->getPanel("L1");
    if (L1 == nullptr) {
        Poco::Logger::get("PanelProjection").error("Panel L1 is not defined : we can't know the left offset for starting back image ! ", __FILE__, __LINE__);
        return nullptr;
    }
    int offsetYNotCountingBack = L1->offsetY;

    // -- margin to take data from in template
    int marginLeftRight = 4;
    // -- ratio to transform size in mm to pixel (based on height of screen without margin pixels in template)
    double ratioMmToPixelBack = (double)(backRef->getWidth() - marginLeftRight) / (double)(positionMaxY-offsetYNotCountingBack);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioMmToPixelBack : " + std::to_string(ratioMmToPixelBack), __FILE__, __LINE__);

    // -- we determine the size of the attended pixels square in image
    double pixelSrcOffsetY =  (double)(confPanels->getScreen()->height+confPanels->getScreen()->offsetZ - (panel->offsetZ+panel->height)) * ratioMmToPixelBack + marginLeftRight;
    double pixelSrcHeight = (double)(panel->height) * ratioMmToPixelBack;
    double pixelSrcWidth = (double)(panel->width) * ratioMmToPixelBack;
    double pixelSrcOffsetX = (double)(backRef->getWidth()) - (double)(panel->offsetY-offsetYNotCountingBack) * ratioMmToPixelBack - pixelSrcWidth - marginLeftRight;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetY : " + std::to_string(pixelSrcOffsetY), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcHeight : " + std::to_string(pixelSrcHeight), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetX : " + std::to_string(pixelSrcOffsetX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcWidth : " + std::to_string(pixelSrcWidth), __FILE__, __LINE__);

    // -- padding is used to place the final resized image in image for panel (offsets) 
    double paddingX = 0;
    double paddingY = 0;

    // -- we determine ratio between size pixels obtained by dimension and resolution of panel
    double ratioPanelResolutionX = (double)(panel->width) / (double)(kinet->sizePixel) / pixelSrcWidth;
    double ratioPanelResolutionY = (double)(panel->height) / (double)(kinet->sizePixel) / pixelSrcHeight;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioPanelResolutionX : " + std::to_string(ratioPanelResolutionX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioPanelResolutionY : " + std::to_string(ratioPanelResolutionY), __FILE__, __LINE__);

    // -- correction of position if out of image
    if (pixelSrcOffsetY < 0) {
        pixelSrcHeight += pixelSrcOffsetY;
        paddingY = -pixelSrcOffsetY*ratioPanelResolutionY;
        pixelSrcOffsetY = 0;
    }
    if (pixelSrcOffsetY + pixelSrcHeight > backRef->getHeight()) {
        pixelSrcHeight = (double)(backRef->getHeight()) - pixelSrcOffsetY;
    }

    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetY : " + std::to_string(pixelSrcOffsetY), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcHeight : " + std::to_string(pixelSrcHeight), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetX : " + std::to_string(pixelSrcOffsetX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcWidth : " + std::to_string(pixelSrcWidth), __FILE__, __LINE__);

    // -- we extract image from dimension versus screen
    Image* extractedBack = backRef->extractData((int)(pixelSrcOffsetX+0.5), (int)(pixelSrcOffsetY+0.5), (int)(pixelSrcWidth+0.5), (int)(pixelSrcHeight+0.5));
    
    // -- we resize image to fill resolution of panel
    int panelPixelWidth = pixelSrcWidth*ratioPanelResolutionX + 0.5;
    int panelPixelHeight = pixelSrcHeight*ratioPanelResolutionY + 0.5;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " panelPixelWidth : " + std::to_string(panelPixelWidth), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " panelPixelHeight : " + std::to_string(panelPixelHeight), __FILE__, __LINE__);

    extractedBack->resizeBell(panelPixelWidth, panelPixelHeight);

    //Poco::Logger::get("PanelProjection").debug(panel->name + " paddingX : " + std::to_string(paddingX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " paddingY : " + std::to_string(paddingY), __FILE__, __LINE__);

    Image* img = extractedBack->extractDataWithFillBlack(0, 0, extractedBack->getWidth(), extractedBack->getHeight(),
                                                            (int)(paddingX+0.5), (int)(paddingY+0.5), panel->width / kinet->sizePixel, panel->height / kinet->sizePixel);
    delete extractedBack;
    backRef.reset();
    return img;
}

Image* PanelProjection::getBackImageRightFitX(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> backRef, int positionMaxY)
{
    // -- we start over the back image (just we do not take into account the offset between screen and first panel)
    IceConfiguration::PanelConf * R1 = confPanels->getPanel("R1");
    if (R1 == nullptr) {
        Poco::Logger::get("PanelProjection").error("Panel R1 is not defined : we can't know the left offset for starting back image ! ", __FILE__, __LINE__);
        return nullptr;
    }
    int offsetYNotCountingBack = R1->offsetY;

    // -- margin to take data from in template
    int marginLeftRight = 4;
    // -- ratio to transform size in mm to pixel (based on height of screen without margin pixels in template)
    double ratioMmToPixelBackX = (double)(backRef->getWidth() - marginLeftRight) /  (double)(positionMaxY-offsetYNotCountingBack);
    double ratioMmToPixelBackY = (double)(backRef->getHeight()) / (double)(confPanels->getScreen()->height);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioMmToPixelBackX : " + std::to_string(ratioMmToPixelBackX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioMmToPixelBackY : " + std::to_string(ratioMmToPixelBackY), __FILE__, __LINE__);

    // -- we determine the size of the attended pixels square in image
    double pixelSrcOffsetY =  (double)(confPanels->getScreen()->height+confPanels->getScreen()->offsetZ - (panel->offsetZ+panel->height)) * ratioMmToPixelBackY;
    double pixelSrcHeight = (double)(panel->height) * ratioMmToPixelBackY;
    double pixelSrcOffsetX = (double)(panel->offsetY-offsetYNotCountingBack) * ratioMmToPixelBackX + marginLeftRight;
    double pixelSrcWidth = (double)(panel->width) * ratioMmToPixelBackX;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetY : " + std::to_string(pixelSrcOffsetY), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcHeight : " + std::to_string(pixelSrcHeight), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetX : " + std::to_string(pixelSrcOffsetX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcWidth : " + std::to_string(pixelSrcWidth), __FILE__, __LINE__);


    // -- padding is used to place the final resized image in image for panel (offsets) 
    double paddingX = 0;
    double paddingY = 0;

    // -- we determine ratio between size pixels obtained by dimension and resolution of panel
    double ratioPanelResolutionX = (double)(panel->width) / (double)(kinet->sizePixel) / pixelSrcWidth;
    double ratioPanelResolutionY = (double)(panel->height) / (double)(kinet->sizePixel) / pixelSrcHeight;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioPanelResolutionX : " + std::to_string(ratioPanelResolutionX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioPanelResolutionY : " + std::to_string(ratioPanelResolutionY), __FILE__, __LINE__);

    // -- correction of position if out of image
    if (pixelSrcOffsetY < 0) {
        pixelSrcHeight += pixelSrcOffsetY;
        paddingY = -pixelSrcOffsetY*ratioPanelResolutionY;
        pixelSrcOffsetY = 0;
    }
    if (pixelSrcOffsetY + pixelSrcHeight > backRef->getHeight() - marginLeftRight) {
        pixelSrcHeight = (double)(backRef->getHeight() - marginLeftRight) - pixelSrcOffsetY;
    }

    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetY : " + std::to_string(pixelSrcOffsetY), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcHeight : " + std::to_string(pixelSrcHeight), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetX : " + std::to_string(pixelSrcOffsetX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcWidth : " + std::to_string(pixelSrcWidth), __FILE__, __LINE__);

    // -- we extract image from dimension versus screen
    Image* extractedBack = backRef->extractData((int)(pixelSrcOffsetX+0.5), (int)(pixelSrcOffsetY+0.5), (int)(pixelSrcWidth+0.5), (int)(pixelSrcHeight+0.5));
    
    // -- we resize image to fill resolution of panel
    int panelPixelWidth = pixelSrcWidth*ratioPanelResolutionX + 0.5;
    int panelPixelHeight = pixelSrcHeight*ratioPanelResolutionY + 0.5;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " panelPixelWidth : " + std::to_string(panelPixelWidth), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " panelPixelHeight : " + std::to_string(panelPixelHeight), __FILE__, __LINE__);

    extractedBack->resizeBell(panelPixelWidth, panelPixelHeight);

    //Poco::Logger::get("PanelProjection").debug(panel->name + " paddingX : " + std::to_string(paddingX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " paddingY : " + std::to_string(paddingY), __FILE__, __LINE__);

    Image* img = extractedBack->extractDataWithFillBlack(0, 0, extractedBack->getWidth(), extractedBack->getHeight(),
                                                            (int)(paddingX+0.5), (int)(paddingY+0.5), panel->width / kinet->sizePixel, panel->height / kinet->sizePixel);
    delete extractedBack;
    backRef.reset();
    return img;
}
Image* PanelProjection::getBackImageLeftFitX(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> backRef, int positionMaxY)
{
    // -- we start over the back image (just we do not take into account the offset between screen and first panel)
    IceConfiguration::PanelConf * L1 = confPanels->getPanel("L1");
    if (L1 == nullptr) {
        Poco::Logger::get("PanelProjection").error("Panel L1 is not defined : we can't know the left offset for starting back image ! ", __FILE__, __LINE__);
        return nullptr;
    }
    int offsetYNotCountingBack = L1->offsetY;

    // -- margin to take data from in template
    int marginLeftRight = 4;
    // -- ratio to transform size in mm to pixel (based on height of screen without margin pixels in template)
    double ratioMmToPixelBackX = (double)(backRef->getWidth() - marginLeftRight) / (double)(positionMaxY-offsetYNotCountingBack);
    double ratioMmToPixelBackY = (double)(backRef->getHeight()) / (double)(confPanels->getScreen()->height);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioMmToPixelBackX : " + std::to_string(ratioMmToPixelBackX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioMmToPixelBackY : " + std::to_string(ratioMmToPixelBackY), __FILE__, __LINE__);

    // -- we determine the size of the attended pixels square in image
    double pixelSrcOffsetY =  (double)(confPanels->getScreen()->height+confPanels->getScreen()->offsetZ - (panel->offsetZ+panel->height)) * ratioMmToPixelBackY;
    double pixelSrcHeight = (double)(panel->height) * ratioMmToPixelBackY;
    double pixelSrcWidth = (double)(panel->width) * ratioMmToPixelBackX;
    double pixelSrcOffsetX = (double)(backRef->getWidth()) - (double)(panel->offsetY-offsetYNotCountingBack) * ratioMmToPixelBackX - pixelSrcWidth - marginLeftRight;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetY : " + std::to_string(pixelSrcOffsetY), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcHeight : " + std::to_string(pixelSrcHeight), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetX : " + std::to_string(pixelSrcOffsetX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcWidth : " + std::to_string(pixelSrcWidth), __FILE__, __LINE__);

    // -- padding is used to place the final resized image in image for panel (offsets) 
    double paddingX = 0;
    double paddingY = 0;

    // -- we determine ratio between size pixels obtained by dimension and resolution of panel
    double ratioPanelResolutionX = (double)(panel->width) / (double)(kinet->sizePixel) / pixelSrcWidth;
    double ratioPanelResolutionY = (double)(panel->height) / (double)(kinet->sizePixel) / pixelSrcHeight;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioPanelResolutionX : " + std::to_string(ratioPanelResolutionX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioPanelResolutionY : " + std::to_string(ratioPanelResolutionY), __FILE__, __LINE__);

    // -- correction of position if out of image
    if (pixelSrcOffsetY < 0) {
        pixelSrcHeight += pixelSrcOffsetY;
        paddingY = -pixelSrcOffsetY*ratioPanelResolutionY;
        pixelSrcOffsetY = 0;
    }
    if (pixelSrcOffsetY + pixelSrcHeight > backRef->getHeight()) {
        pixelSrcHeight = (double)(backRef->getHeight()) - pixelSrcOffsetY;
    }

    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetY : " + std::to_string(pixelSrcOffsetY), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcHeight : " + std::to_string(pixelSrcHeight), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetX : " + std::to_string(pixelSrcOffsetX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcWidth : " + std::to_string(pixelSrcWidth), __FILE__, __LINE__);

    // -- we extract image from dimension versus screen
    Image* extractedBack = backRef->extractData((int)(pixelSrcOffsetX+0.5), (int)(pixelSrcOffsetY+0.5), (int)(pixelSrcWidth+0.5), (int)(pixelSrcHeight+0.5));
    
    // -- we resize image to fill resolution of panel
    int panelPixelWidth = pixelSrcWidth*ratioPanelResolutionX + 0.5;
    int panelPixelHeight = pixelSrcHeight*ratioPanelResolutionY + 0.5;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " panelPixelWidth : " + std::to_string(panelPixelWidth), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " panelPixelHeight : " + std::to_string(panelPixelHeight), __FILE__, __LINE__);

    extractedBack->resizeBell(panelPixelWidth, panelPixelHeight);

    //Poco::Logger::get("PanelProjection").debug(panel->name + " paddingX : " + std::to_string(paddingX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " paddingY : " + std::to_string(paddingY), __FILE__, __LINE__);

    Image* img =  extractedBack->extractDataWithFillBlack(0, 0, extractedBack->getWidth(), extractedBack->getHeight(),
                                                            (int)(paddingX+0.5), (int)(paddingY+0.5), panel->width / kinet->sizePixel, panel->height / kinet->sizePixel);
    delete extractedBack;
    backRef.reset();
    return img;
}

Image* PanelProjection::getBackImageRightFitY(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> backRef, int positionMinZ, int positionMaxZ)
{
    // -- we start over the back image (just we do not take into account the offset between screen and first panel)
    IceConfiguration::PanelConf * R1 = confPanels->getPanel("R1");
    if (R1 == nullptr) {
        Poco::Logger::get("PanelProjection").error("Panel R1 is not defined : we can't know the left offset for starting back image ! ", __FILE__, __LINE__);
        return nullptr;
    }
    int offsetYNotCountingBack = R1->offsetY;

    // -- center of screen is center of image source
    int mireZ = confPanels->getScreen()->offsetZ+confPanels->getScreen()->height/2;
    // -- we determine size in mm of the image (maximazed size in panel based of mire center)
    int deltaMax = std::max(mireZ - positionMinZ, positionMaxZ - mireZ);

    // -- margin to take data from in template
    int marginLeftRight = 4;
    // -- ratio to transform size in mm to pixel (based on height of screen without margin pixels in template)
    double ratioMmToPixelBackX = (double)(backRef->getHeight()) / (double)(confPanels->getScreen()->height);
    double ratioMmToPixelBackY = (double)(backRef->getHeight()) / (double)(deltaMax*2.0);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioMmToPixelBackX : " + std::to_string(ratioMmToPixelBackX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioMmToPixelBackY : " + std::to_string(ratioMmToPixelBackY), __FILE__, __LINE__);

    // -- we determine the size of the attended pixels square in image
    double pixelSrcOffsetY =  (double)(mireZ+deltaMax - (panel->offsetZ+panel->height)) * ratioMmToPixelBackY;
    double pixelSrcHeight = (double)(panel->height) * ratioMmToPixelBackY;
    double pixelSrcOffsetX = (double)(panel->offsetY-offsetYNotCountingBack) * ratioMmToPixelBackX + marginLeftRight;
    double pixelSrcWidth = (double)(panel->width) * ratioMmToPixelBackX;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetY : " + std::to_string(pixelSrcOffsetY), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcHeight : " + std::to_string(pixelSrcHeight), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetX : " + std::to_string(pixelSrcOffsetX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcWidth : " + std::to_string(pixelSrcWidth), __FILE__, __LINE__);

    // -- we determine ratio between size pixels obtained by dimension and resolution of panel
    double ratioPanelResolutionX = (double)(panel->width) / (double)(kinet->sizePixel) / pixelSrcWidth;
    double ratioPanelResolutionY = (double)(panel->height) / (double)(kinet->sizePixel) / pixelSrcHeight;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioPanelResolutionX : " + std::to_string(ratioPanelResolutionX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioPanelResolutionY : " + std::to_string(ratioPanelResolutionY), __FILE__, __LINE__);

    if (pixelSrcOffsetX + pixelSrcWidth > backRef->getWidth() - marginLeftRight) {
        pixelSrcWidth = (double)(backRef->getWidth() - marginLeftRight) - pixelSrcOffsetX;
    }

    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetY : " + std::to_string(pixelSrcOffsetY), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcHeight : " + std::to_string(pixelSrcHeight), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetX : " + std::to_string(pixelSrcOffsetX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcWidth : " + std::to_string(pixelSrcWidth), __FILE__, __LINE__);

    // -- we extract image from dimension versus screen
    Image* extractedBack = backRef->extractData((int)(pixelSrcOffsetX+0.5), (int)(pixelSrcOffsetY+0.5), (int)(pixelSrcWidth+0.5), (int)(pixelSrcHeight+0.5));
    
    // -- we resize image to fill resolution of panel
    int panelPixelWidth = pixelSrcWidth*ratioPanelResolutionX + 0.5;
    int panelPixelHeight = pixelSrcHeight*ratioPanelResolutionY + 0.5;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " panelPixelWidth : " + std::to_string(panelPixelWidth), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " panelPixelHeight : " + std::to_string(panelPixelHeight), __FILE__, __LINE__);

    extractedBack->resizeBell(panelPixelWidth, panelPixelHeight);

    Image* img =  extractedBack->extractDataWithFillBlack(0, 0, extractedBack->getWidth(), extractedBack->getHeight(),
                                                            0, 0, panel->width / kinet->sizePixel, panel->height / kinet->sizePixel);
    delete extractedBack;
    backRef.reset();
    return img;
}
Image* PanelProjection::getBackImageLeftFitY(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> backRef, int positionMinZ, int positionMaxZ)
{
    // -- we start over the back image (just we do not take into account the offset between screen and first panel)
    IceConfiguration::PanelConf * L1 = confPanels->getPanel("L1");
    if (L1 == nullptr) {
        Poco::Logger::get("PanelProjection").error("Panel L1 is not defined : we can't know the left offset for starting back image ! ", __FILE__, __LINE__);
        return nullptr;
    }
    int offsetYNotCountingBack = L1->offsetY;

    // -- center of screen is center of image source
    int mireZ = confPanels->getScreen()->offsetZ+confPanels->getScreen()->height/2;
    // -- we determine size in mm of the image (maximazed size in panel based of mire center)
    int deltaMax = std::max(mireZ - positionMinZ, positionMaxZ - mireZ);

    // -- margin to take data from in template
    int marginLeftRight = 4;
    // -- ratio to transform size in mm to pixel (based on height of screen without margin pixels in template)
    double ratioMmToPixelBackX = (double)(backRef->getHeight()) / (double)(confPanels->getScreen()->height);
    double ratioMmToPixelBackY = (double)(backRef->getHeight() / (deltaMax*2.0));
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioMmToPixelBackX : " + std::to_string(ratioMmToPixelBackX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioMmToPixelBackY : " + std::to_string(ratioMmToPixelBackY), __FILE__, __LINE__);

    // -- we determine the size of the attended pixels square in image
    double pixelSrcOffsetY =  (double)(mireZ+deltaMax - (panel->offsetZ+panel->height)) * ratioMmToPixelBackY;
    double pixelSrcHeight = (double)(panel->height) * ratioMmToPixelBackY;
    double pixelSrcWidth = (double)(panel->width) * ratioMmToPixelBackX;
    double pixelSrcOffsetX = (double)(backRef->getWidth()) - (double)(panel->offsetY-offsetYNotCountingBack) * ratioMmToPixelBackX - pixelSrcWidth - marginLeftRight;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetY : " + std::to_string(pixelSrcOffsetY), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcHeight : " + std::to_string(pixelSrcHeight), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetX : " + std::to_string(pixelSrcOffsetX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcWidth : " + std::to_string(pixelSrcWidth), __FILE__, __LINE__);

    // -- padding is used to place the final resized image in image for panel (offsets) 
    double paddingX = 0;
    double paddingY = 0;

    // -- we determine ratio between size pixels obtained by dimension and resolution of panel
    double ratioPanelResolutionX = (double)(panel->width) / (double)(kinet->sizePixel) / pixelSrcWidth;
    double ratioPanelResolutionY = (double)(panel->height) / (double)(kinet->sizePixel) / pixelSrcHeight;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioPanelResolutionX : " + std::to_string(ratioPanelResolutionX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioPanelResolutionY : " + std::to_string(ratioPanelResolutionY), __FILE__, __LINE__);

    if (pixelSrcOffsetX < 0) {
        paddingX = -pixelSrcOffsetX*ratioPanelResolutionX;
        pixelSrcWidth += pixelSrcOffsetX;
        pixelSrcOffsetX = 0;
    }

    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetY : " + std::to_string(pixelSrcOffsetY), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcHeight : " + std::to_string(pixelSrcHeight), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetX : " + std::to_string(pixelSrcOffsetX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcWidth : " + std::to_string(pixelSrcWidth), __FILE__, __LINE__);

    // -- we extract image from dimension versus screen
    Image* extractedBack = backRef->extractData((int)(pixelSrcOffsetX+0.5), (int)(pixelSrcOffsetY+0.5), (int)(pixelSrcWidth+0.5), (int)(pixelSrcHeight+0.5));
    
    // -- we resize image to fill resolution of panel
    int panelPixelWidth = pixelSrcWidth*ratioPanelResolutionX + 0.5;
    int panelPixelHeight = pixelSrcHeight*ratioPanelResolutionY + 0.5;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " panelPixelWidth : " + std::to_string(panelPixelWidth), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " panelPixelHeight : " + std::to_string(panelPixelHeight), __FILE__, __LINE__);

    extractedBack->resizeBell(panelPixelWidth, panelPixelHeight);

    //Poco::Logger::get("PanelProjection").debug(panel->name + " paddingX : " + std::to_string(paddingX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " paddingY : " + std::to_string(paddingY), __FILE__, __LINE__);

    Image* img =  extractedBack->extractDataWithFillBlack(0, 0, extractedBack->getWidth(), extractedBack->getHeight(),
                                                            (int)(paddingX+0.5), (int)(paddingY+0.5), panel->width / kinet->sizePixel, panel->height / kinet->sizePixel);
    delete extractedBack;
    backRef.reset();
    return img;
}

Image* PanelProjection::getBackImageLeftFitXY(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> backRef, int positionMinZ, int positionMaxZ, int positionMaxY)
{
    // -- we start over the back image (just we do not take into account the offset between screen and first panel)
    IceConfiguration::PanelConf * L1 = confPanels->getPanel("L1");
    if (L1 == nullptr) {
        Poco::Logger::get("PanelProjection").error("Panel L1 is not defined : we can't know the left offset for starting back image ! ", __FILE__, __LINE__);
        return nullptr;
    }
    int offsetYNotCountingBack = L1->offsetY;

    // -- center of screen is center of image source
    int mireZ = confPanels->getScreen()->offsetZ+confPanels->getScreen()->height/2;
    // -- we determine size in mm of the image (maximazed size in panel based of mire center)
    int deltaMax = std::max(mireZ - positionMinZ, positionMaxZ - mireZ);

    // -- margin to take data from in template
    int marginLeftRight = 4;
    // -- ratio to transform size in mm to pixel (based on height of screen without margin pixels in template)
    double ratioMmToPixelBackX = (double)(backRef->getWidth() - marginLeftRight) / (double)(positionMaxY-offsetYNotCountingBack);
    double ratioMmToPixelBackY = (double)(backRef->getHeight()) / (deltaMax*2.0);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioMmToPixelBackX : " + std::to_string(ratioMmToPixelBackX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioMmToPixelBackY : " + std::to_string(ratioMmToPixelBackY), __FILE__, __LINE__);

    // -- we determine the size of the attended pixels square in image
    double pixelSrcOffsetY =  (double)(mireZ+deltaMax - (panel->offsetZ+panel->height)) * ratioMmToPixelBackY;
    double pixelSrcHeight = (double)(panel->height) * ratioMmToPixelBackY;
    double pixelSrcWidth = (double)(panel->width) * ratioMmToPixelBackX;
    double pixelSrcOffsetX = (double)(backRef->getWidth()) - (double)(panel->offsetY-offsetYNotCountingBack) * ratioMmToPixelBackX - pixelSrcWidth - marginLeftRight;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetY : " + std::to_string(pixelSrcOffsetY), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcHeight : " + std::to_string(pixelSrcHeight), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetX : " + std::to_string(pixelSrcOffsetX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcWidth : " + std::to_string(pixelSrcWidth), __FILE__, __LINE__);

    // -- padding is used to place the final resized image in image for panel (offsets) 
    double paddingX = 0;
    double paddingY = 0;

    // -- we determine ratio between size pixels obtained by dimension and resolution of panel
    double ratioPanelResolutionX = (double)(panel->width) / (double)(kinet->sizePixel) / pixelSrcWidth;
    double ratioPanelResolutionY = (double)(panel->height) / (double)(kinet->sizePixel) / pixelSrcHeight;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioPanelResolutionX : " + std::to_string(ratioPanelResolutionX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioPanelResolutionY : " + std::to_string(ratioPanelResolutionY), __FILE__, __LINE__);

    // -- correction of position if out of image
    if (pixelSrcOffsetY < 0) {
        pixelSrcHeight += pixelSrcOffsetY;
        paddingY = -pixelSrcOffsetY*ratioPanelResolutionY;
        pixelSrcOffsetY = 0;
    }
    if (pixelSrcOffsetY + pixelSrcHeight > backRef->getHeight()) {
        pixelSrcHeight = (double)(backRef->getHeight()) - pixelSrcOffsetY;
    }
    if (pixelSrcOffsetX < 0) {
        paddingX = -pixelSrcOffsetX*ratioPanelResolutionX;
        pixelSrcWidth += pixelSrcOffsetX;
        pixelSrcOffsetX = 0;
    }

    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetY : " + std::to_string(pixelSrcOffsetY), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcHeight : " + std::to_string(pixelSrcHeight), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetX : " + std::to_string(pixelSrcOffsetX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcWidth : " + std::to_string(pixelSrcWidth), __FILE__, __LINE__);

    // -- we extract image from dimension versus screen
    Image* extractedBack = backRef->extractData((int)(pixelSrcOffsetX+0.5), (int)(pixelSrcOffsetY+0.5), (int)(pixelSrcWidth+0.5), (int)(pixelSrcHeight+0.5));
    
    // -- we resize image to fill resolution of panel
    int panelPixelWidth = pixelSrcWidth*ratioPanelResolutionX + 0.5;
    int panelPixelHeight = pixelSrcHeight*ratioPanelResolutionY + 0.5;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " panelPixelWidth : " + std::to_string(panelPixelWidth), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " panelPixelHeight : " + std::to_string(panelPixelHeight), __FILE__, __LINE__);

    extractedBack->resizeBell(panelPixelWidth, panelPixelHeight);

    //Poco::Logger::get("PanelProjection").debug(panel->name + " paddingX : " + std::to_string(paddingX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " paddingY : " + std::to_string(paddingY), __FILE__, __LINE__);

    Image* img =  extractedBack->extractDataWithFillBlack(0, 0, extractedBack->getWidth(), extractedBack->getHeight(),
                                                            (int)(paddingX+0.5), (int)(paddingY+0.5), panel->width / kinet->sizePixel, panel->height / kinet->sizePixel);
    delete extractedBack;
    backRef.reset();
    return img;
}
Image* PanelProjection::getBackImageRightFitXY(IceConfiguration::PanelConf *panel, KinetConfiguration::PanelConf *kinet, std::shared_ptr<Image> backRef, int positionMinZ, int positionMaxZ, int positionMaxY)
{
    // -- we start over the back image (just we do not take into account the offset between screen and first panel)
    IceConfiguration::PanelConf * R1 = confPanels->getPanel("R1");
    if (R1 == nullptr) {
        Poco::Logger::get("PanelProjection").error("Panel R1 is not defined : we can't know the left offset for starting back image ! ", __FILE__, __LINE__);
        return nullptr;
    }
    int offsetYNotCountingBack = R1->offsetY;

    // -- center of screen is center of image source
    int mireZ = confPanels->getScreen()->offsetZ+confPanels->getScreen()->height/2;
    // -- we determine size in mm of the image (maximazed size in panel based of mire center)
    int deltaMax = std::max(mireZ - positionMinZ, positionMaxZ - mireZ);

    // -- margin to take data from in template
    int marginLeftRight = 4;
    // -- ratio to transform size in mm to pixel (based on height of screen without margin pixels in template)
    double ratioMmToPixelBackX = (double)(backRef->getWidth() - marginLeftRight) / (double)(positionMaxY-offsetYNotCountingBack);
    double ratioMmToPixelBackY = (double)(backRef->getHeight()) / (deltaMax*2.0);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioMmToPixelBackX : " + std::to_string(ratioMmToPixelBackX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioMmToPixelBackY : " + std::to_string(ratioMmToPixelBackY), __FILE__, __LINE__);

    // -- we determine the size of the attended pixels square in image
    double pixelSrcOffsetY =  (double)(mireZ+deltaMax - (panel->offsetZ+panel->height)) * ratioMmToPixelBackY;
    double pixelSrcHeight = (double)(panel->height) * ratioMmToPixelBackY;
    double pixelSrcOffsetX = (double)(panel->offsetY-offsetYNotCountingBack) * ratioMmToPixelBackX + marginLeftRight;
    double pixelSrcWidth = (double)(panel->width) * ratioMmToPixelBackX;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetY : " + std::to_string(pixelSrcOffsetY), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcHeight : " + std::to_string(pixelSrcHeight), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetX : " + std::to_string(pixelSrcOffsetX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcWidth : " + std::to_string(pixelSrcWidth), __FILE__, __LINE__);

    double paddingX = 0;
    double paddingY = 0;

    // -- we determine ratio between size pixels obtained by dimension and resolution of panel
    double ratioPanelResolutionX = (double)(panel->width) / (double)(kinet->sizePixel) / pixelSrcWidth;
    double ratioPanelResolutionY = (double)(panel->height) / (double)(kinet->sizePixel) / pixelSrcHeight;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioPanelResolutionX : " + std::to_string(ratioPanelResolutionX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " ratioPanelResolutionY : " + std::to_string(ratioPanelResolutionY), __FILE__, __LINE__);

    // -- correction of position if out of image
    if (pixelSrcOffsetY < 0) {
        pixelSrcHeight += pixelSrcOffsetY;
        paddingY = -pixelSrcOffsetY*ratioPanelResolutionY;
        pixelSrcOffsetY = 0;
    }
    if (pixelSrcOffsetY + pixelSrcHeight > backRef->getHeight()) {
        pixelSrcHeight = (double)(backRef->getHeight()) - pixelSrcOffsetY;
    }
    if (pixelSrcOffsetX + pixelSrcWidth > backRef->getWidth() - marginLeftRight) {
        pixelSrcWidth = (double)(backRef->getWidth() - marginLeftRight) - pixelSrcOffsetX;
    }
    if (pixelSrcOffsetX < 0) {
        paddingX = -pixelSrcOffsetX*ratioPanelResolutionX;
        pixelSrcWidth += pixelSrcOffsetX;
        pixelSrcOffsetX = 0;
    }

    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetY : " + std::to_string(pixelSrcOffsetY), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcHeight : " + std::to_string(pixelSrcHeight), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcOffsetX : " + std::to_string(pixelSrcOffsetX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " pixelSrcWidth : " + std::to_string(pixelSrcWidth), __FILE__, __LINE__);

    // -- we extract image from dimension versus screen
    Image* extractedBack = backRef->extractData((int)(pixelSrcOffsetX+0.5), (int)(pixelSrcOffsetY+0.5), (int)(pixelSrcWidth+0.5), (int)(pixelSrcHeight+0.5));
    
    // -- we resize image to fill resolution of panel
    int panelPixelWidth = pixelSrcWidth*ratioPanelResolutionX + 0.5;
    int panelPixelHeight = pixelSrcHeight*ratioPanelResolutionY + 0.5;
    //Poco::Logger::get("PanelProjection").debug(panel->name + " panelPixelWidth : " + std::to_string(panelPixelWidth), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " panelPixelHeight : " + std::to_string(panelPixelHeight), __FILE__, __LINE__);

    extractedBack->resizeBell(panelPixelWidth, panelPixelHeight);

    //Poco::Logger::get("PanelProjection").debug(panel->name + " paddingX : " + std::to_string(paddingX), __FILE__, __LINE__);
    //Poco::Logger::get("PanelProjection").debug(panel->name + " paddingY : " + std::to_string(paddingY), __FILE__, __LINE__);

    Image* img = extractedBack->extractDataWithFillBlack(0, 0, extractedBack->getWidth(), extractedBack->getHeight(),
                                                            (int)(paddingX+0.5), (int)(paddingY+0.5), panel->width / kinet->sizePixel, panel->height / kinet->sizePixel);
    delete extractedBack;
    backRef.reset();
    return img;
}