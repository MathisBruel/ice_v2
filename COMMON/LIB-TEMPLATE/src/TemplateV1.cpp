#include "TemplateV1.h"

TemplateV1::TemplateV1()
    : Template::Template("V1")
{

    // -- definition of panels
    // -- marge of 4 pixels up, down, right
    panelDefinitionBackLeft.name = "Back-left";
    panelDefinitionBackLeft.offsetX = 0;
    panelDefinitionBackLeft.offsetY = 438; 
    panelDefinitionBackLeft.width = 356;
    panelDefinitionBackLeft.height = 128;

    // -- marge of 4 pixels up, down, left
    panelDefinitionBackLeft.name = "Back-right";
    panelDefinitionBackRight.offsetX = 364; 
    panelDefinitionBackRight.offsetY = 438; 
    panelDefinitionBackRight.width = 356;
    panelDefinitionBackRight.height = 128;

    panelDefinitionForeLeft = new Template::SquareImageDefinition[10];
    for (int i = 0; i < 10; i++) {
        panelDefinitionForeLeft[i].name = "L" + std::to_string(i+1);
        panelDefinitionForeLeft[i].offsetX = 332-36*i;
        panelDefinitionForeLeft[i].offsetY = 358;
        panelDefinitionForeLeft[i].width = 20;
        panelDefinitionForeLeft[i].height = 84;
    }

    panelDefinitionForeRight = new Template::SquareImageDefinition[10];
    for (int i = 0; i < 10; i++) {
        panelDefinitionForeRight[i].name = "R" + std::to_string(i+1);
        panelDefinitionForeRight[i].offsetX = 368+36*i;
        panelDefinitionForeRight[i].offsetY = 358;
        panelDefinitionForeRight[i].width = 20;
        panelDefinitionForeRight[i].height = 84;
    }

    // -- definition of movingHeads
    int nbMovingHeads = 4;
    movingHeadsDefinitionPan = new Template::PointImageDefinition[nbMovingHeads];
    movingHeadsDefinitionTilt = new Template::PointImageDefinition[nbMovingHeads];
    movingHeadsDefinitionColor = new Template::PointImageDefinition[nbMovingHeads];
    movingHeadsDefinitionPan[0].name = "MH-LF";
    movingHeadsDefinitionPan[0].offsetX = 33;
    movingHeadsDefinitionPan[0].offsetY = 0;
    movingHeadsDefinitionTilt[0].name = "MH-LF";
    movingHeadsDefinitionTilt[0].offsetX = 34;
    movingHeadsDefinitionTilt[0].offsetY = 0;
    movingHeadsDefinitionColor[0].name = "MH-LF";
    movingHeadsDefinitionColor[0].offsetX = 0;
    movingHeadsDefinitionColor[0].offsetY = 67;

    movingHeadsDefinitionPan[1].name = "MH-LR";
    movingHeadsDefinitionPan[1].offsetX = 39;
    movingHeadsDefinitionPan[1].offsetY = 0;
    movingHeadsDefinitionTilt[1].name = "MH-LR";
    movingHeadsDefinitionTilt[1].offsetX = 40;
    movingHeadsDefinitionTilt[1].offsetY = 0;
    movingHeadsDefinitionColor[1].name = "MH-LR";
    movingHeadsDefinitionColor[1].offsetX = 0;
    movingHeadsDefinitionColor[1].offsetY = 135;

    movingHeadsDefinitionPan[2].name = "MH-RF";
    movingHeadsDefinitionPan[2].offsetX = 36;
    movingHeadsDefinitionPan[2].offsetY = 0;
    movingHeadsDefinitionTilt[2].name = "MH-RF";
    movingHeadsDefinitionTilt[2].offsetX = 37;
    movingHeadsDefinitionTilt[2].offsetY = 0;
    movingHeadsDefinitionColor[2].name = "MH-RF";
    movingHeadsDefinitionColor[2].offsetX = 68;
    movingHeadsDefinitionColor[2].offsetY = 67;

    movingHeadsDefinitionPan[3].name = "MH-RR";
    movingHeadsDefinitionPan[3].offsetX = 42;
    movingHeadsDefinitionPan[3].offsetY = 0;
    movingHeadsDefinitionTilt[3].name = "MH-RR";
    movingHeadsDefinitionTilt[3].offsetX = 43;
    movingHeadsDefinitionTilt[3].offsetY = 0;
    movingHeadsDefinitionColor[3].name = "MH-RR";
    movingHeadsDefinitionColor[3].offsetX = 68;
    movingHeadsDefinitionColor[3].offsetY = 135;

    // -- definition of pars
    int nbPAR = 8;
    parsDefinition = new Template::PointImageDefinition[nbPAR];
    for (int i = 0; i < 4; i++) {
        parsDefinition[i].name = "PAR-L" + std::to_string(i+1);
        parsDefinition[i].offsetX = 30+i*60;
        parsDefinition[i].offsetY = 320;

        parsDefinition[7-i].name = "PAR-R" + std::to_string(i+1);;
        parsDefinition[7-i].offsetX = 690-i*60;
        parsDefinition[7-i].offsetY = 320;
    }

    // -- definition of backlight
    int nbBackLight = 4;
    backlightsDefinition = new Template::PointImageDefinition[nbBackLight];
    for (int i = 0; i < 4; i++) {
        backlightsDefinition[i].name = "BL-" + std::to_string(i+1);
        backlightsDefinition[i].offsetX = 270+i*60;
        backlightsDefinition[i].offsetY = 320;
    }
}

TemplateV1::~TemplateV1() {

    if (panelDefinitionForeLeft != nullptr) {
        delete[] panelDefinitionForeLeft;
    }
    if (panelDefinitionForeRight != nullptr) {
        delete[] panelDefinitionForeRight;
    }
    if (backlightsDefinition != nullptr) {
        delete[] backlightsDefinition;
    }
    if (parsDefinition != nullptr) {
        delete[] parsDefinition;
    }
    if (movingHeadsDefinitionPan != nullptr) {
        delete[] movingHeadsDefinitionPan;
    }
    if (movingHeadsDefinitionTilt != nullptr) {
        delete[] movingHeadsDefinitionTilt;
    }
    if (movingHeadsDefinitionColor != nullptr) {
        delete[] movingHeadsDefinitionColor;
    }
}

void TemplateV1::extractBackLeftPanelsImage()
{
    if (refImage == nullptr) {
        Poco::Logger::get("TemplateV1").error("Error extraction in template : image is nullptr !", __FILE__, __LINE__);
        return;
    }

    imgPanelBackLeft = std::shared_ptr<Image>(refImage->extractData(panelDefinitionBackLeft.offsetX, panelDefinitionBackLeft.offsetY, 
                                            panelDefinitionBackLeft.width, panelDefinitionBackLeft.height));
}

void TemplateV1::extractBackRightPanelsImage()
{
    if (refImage == nullptr) {
        Poco::Logger::get("TemplateV1").error("Error extraction in template : image is nullptr !", __FILE__, __LINE__);
        return;
    }

    imgPanelBackRight = std::shared_ptr<Image>(refImage->extractData(panelDefinitionBackRight.offsetX, panelDefinitionBackRight.offsetY, 
                                            panelDefinitionBackRight.width, panelDefinitionBackRight.height));
}

void TemplateV1::extractForeLeftPanelsImage()
{
    if (refImage == nullptr) {
        Poco::Logger::get("TemplateV1").error("Error extraction in template : image is nullptr !", __FILE__, __LINE__);
        return;
    }

    std::map<std::string, std::shared_ptr<Image>>::iterator it;
    for (it = imgPanelForeLeft.begin(); it != imgPanelForeLeft.end(); it++) {
        it->second.reset();
    }
    imgPanelForeLeft.clear();

    for (int i = 0; i < 10; i++) {

        std::shared_ptr<Image> img = std::shared_ptr<Image>(refImage->extractData(panelDefinitionForeLeft[i].offsetX, panelDefinitionForeLeft[i].offsetY,
                panelDefinitionForeLeft[i].width, panelDefinitionForeLeft[i].height));

        imgPanelForeLeft.insert_or_assign(panelDefinitionForeLeft[i].name, img);
        img.reset();
    }
}

void TemplateV1::extractForeRightPanelsImage()
{
    if (refImage == nullptr) {
        Poco::Logger::get("TemplateV1").error("Error extraction in template : image is nullptr !", __FILE__, __LINE__);
        return;
    }

    std::map<std::string, std::shared_ptr<Image>>::iterator it;
    for (it = imgPanelForeRight.begin(); it != imgPanelForeRight.end(); it++) {
        it->second.reset();
    }
    imgPanelForeRight.clear();

    for (int i = 0; i < 10; i++) {

        std::shared_ptr<Image> img = std::shared_ptr<Image>(refImage->extractData(panelDefinitionForeRight[i].offsetX, panelDefinitionForeRight[i].offsetY,
                    panelDefinitionForeRight[i].width, panelDefinitionForeRight[i].height));

        imgPanelForeRight.insert_or_assign(panelDefinitionForeRight[i].name, img);
        img.reset();
    }
}

void TemplateV1::extractMovingHeadsDatas()
{
    std::map<std::string, std::shared_ptr<Template::MovingHeadsOutputs>>::iterator itMH;
    for (itMH = movingHeads.begin(); itMH != movingHeads.end(); itMH++) {
        itMH->second.reset();
    }
    movingHeads.clear();

    for (int i = 0 ; i < 4; i++) {

        std::shared_ptr<MovingHeadsOutputs> movingHead = std::make_shared<MovingHeadsOutputs>(MovingHeadsOutputs());

        movingHead->name = movingHeadsDefinitionColor[i].name;
        Color* color = refImage->getValueAt(movingHeadsDefinitionColor[i].offsetX, movingHeadsDefinitionColor[i].offsetY);
        movingHead->color.setRGBA(color->getRed(), color->getGreen(), color->getBlue(), color->getAlpha());
        Color* panColor = refImage->getValueAt(movingHeadsDefinitionPan[i].offsetX, movingHeadsDefinitionPan[i].offsetY);
        Color* tiltColor = refImage->getValueAt(movingHeadsDefinitionTilt[i].offsetX, movingHeadsDefinitionTilt[i].offsetY);

        delete color;

        int panHigh = (int)(panColor->getGreen()*255.0 + 0.5);
        int panLow = (int)(panColor->getBlue()*255.0 + 0.5);
        int tiltHigh = (int)(tiltColor->getGreen()*255.0 + 0.5);
        int tiltLow = (int)(tiltColor->getBlue()*255.0 + 0.5);

        delete panColor;
        delete tiltColor;

        // -- pan angle is reconstructed
        double pan = ((panHigh << 8) + panLow);
        // -- transform assumed value goes from -180° to 180°
        pan *= (360.0 / 65535.0);
        pan -= 180.0;
        // -- to radians
        pan *= M_PI/180.0;
        
        // -- pan angle is reconstructed
        double tilt = ((tiltHigh << 8) + tiltLow);
        tilt *= (180.0 / 65535.0);
        tilt -= 90.0;
        // -- to radians
        tilt *= M_PI/180.0;
        
        // convert polar to XYZ
        double X,Y,Z;
        X = std::sin(tilt)*std::sin(-pan);
        Y = -std::sin(tilt)*std::cos(pan);
        Z = -std::cos(tilt);

        // -- replace at the MH parameter : assumed room is 1 of length
        // -- MH is placed at 0.5, 0.5, 0.5
        double Xmh = 0.5;
        double Ymh = 0.5;
        double Zmh = 0.5;

        // -- project pointer of MH to ground (Z = 0)
        double ratioOnGround = -Zmh / Z;
        X *= ratioOnGround;
        Y *= ratioOnGround;
        Z *= ratioOnGround;

        double pointerX = Xmh + X;
        double pointerY = Ymh + Y;
        double pointerZ = Zmh + Z;

        movingHead->ratioPositionX = pointerX;
        movingHead->ratioPositionY = pointerY;

        movingHeads.insert_or_assign(movingHead->name, movingHead);
        movingHead.reset();
    }
}
void TemplateV1::extractPARDatas()
{
    for (std::shared_ptr<Template::ParOutputs> par : pars) {
        par.reset();
    }
    pars.clear();

    for (int i = 0; i < 8; i++) {

        std::shared_ptr<ParOutputs> par = std::make_shared<ParOutputs>(ParOutputs());
        par->name = parsDefinition[i].name;
        Color* color = refImage->getValueAt(parsDefinition[i].offsetX, parsDefinition[i].offsetY);
        par->color.setRGBA(color->getRed(), color->getGreen(), color->getBlue(), color->getAlpha());;

        delete color;
        pars.push_back(par);
        par.reset();
    }
}
void TemplateV1::extractBacklightDatas()
{
    for (std::shared_ptr<Template::BacklightOutputs> backlight : backlights) {
        backlight.reset();
    }
    backlights.clear();

    for (int i = 0; i < 4; i++) {

        std::shared_ptr<BacklightOutputs> backlight = std::make_shared<BacklightOutputs>(BacklightOutputs());
        backlight->name = backlightsDefinition[i].name;

        Color* color = refImage->getValueAt(backlightsDefinition[i].offsetX, backlightsDefinition[i].offsetY);
        backlight->color.setRGBA(color->getRed(), color->getGreen(), color->getBlue(), color->getAlpha());

        delete color;

        backlights.push_back(backlight);
        backlight.reset();
    }
}