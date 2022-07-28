#include "MovingHeadProjection.h"

MovingHeadProjection::MovingHeadProjection(IceConfiguration* ice)
{
    this->ice = ice;
}
MovingHeadProjection::~MovingHeadProjection()
{
    while (!outputs.empty()) {
        outputs.front().reset();
        outputs.pop();
    }
}

void MovingHeadProjection::clean()
{
    while (!outputs.empty()) {
        outputs.front().reset();
        outputs.pop();
    }
}


bool MovingHeadProjection::projectMovingHeads()
{
    double epsilon = 1e-6;
    // -- remove references (deletion of pointers done by kinetThread after treatment)
    while (!outputs.empty()) {
        outputs.front().reset();
        outputs.pop();
    }

    if (ice == nullptr) {
        Poco::Logger::get("MovingHeadProjection").error("Ice configuration is nullptr !", __FILE__, __LINE__);
        return false;
    }

    for (int i = 0; i < ice->getNbMovingHeads(); i++) {

        IceConfiguration::MovingheadConf* confMH = ice->getMovingHead(i);
        std::shared_ptr<Template::MovingHeadsOutputs> mhOutput(templateExtract->getMovingHeadOutputs(confMH->name));

        if (mhOutput == nullptr) {
            Poco::Logger::get("MovingHeadProjection").error("No corresponding outputs in template for movingHead : " + confMH->name, __FILE__, __LINE__);
            return false;
        }

        // -- position must be greater or equal to 0 and inferior or equal to 1
        if (mhOutput->ratioPositionX < -epsilon || mhOutput->ratioPositionY < -epsilon) {
            //Poco::Logger::get("MovingHeadProjection").debug( "Angles not correct : skipped!", __FILE__, __LINE__);
            std::shared_ptr<OutputMovingHeadAngle> output = std::make_shared<OutputMovingHeadAngle>(OutputMovingHeadAngle());
            output->name = confMH->name;
            Color black;
            black.setRGB(0, 0, 0);
            output->color = black;
            output->pan = 0;
            output->tilt = 0;
            outputs.push(output);
            mhOutput.reset();
            continue;
        }

        // -- we determine absolute position of pointage required
        // -- Z is gradually raising with y
        double pointerRequiredX = (confMH->maxX - confMH->minX) * mhOutput->ratioPositionX + confMH->minX;
        double pointerRequiredY = (confMH->maxY - confMH->minY) * mhOutput->ratioPositionY + confMH->minY;
        double pointerRequiredZ = (confMH->maxZ - confMH->minZ) * mhOutput->ratioPositionY + confMH->minZ;

        //Poco::Logger::get("MovingHeadProjection").debug("Moving head must point to (X,Y,Z) : (" + 
        //    std::to_string(pointerRequiredX) + "," + std::to_string(pointerRequiredY) + ","  +std::to_string(pointerRequiredZ) + ")", __FILE__, __LINE__);

        // -- we determine vector from movingHead to pointed position
        double vectorViewX = pointerRequiredX - confMH->offsetX;
        double vectorViewY = pointerRequiredY - confMH->offsetY;
        double vectorViewZ = pointerRequiredZ - confMH->offsetZ;

        // -- normalize vector 
        double length = sqrt(vectorViewX*vectorViewX+vectorViewY*vectorViewY+vectorViewZ*vectorViewZ);
        vectorViewX /= length;
        vectorViewY /= length;
        vectorViewZ /= length;

        double pan = 0.0;
        if (confMH->left) {
            pan = std::atan(-vectorViewY/vectorViewX);
        }
        else {
            pan = std::atan(vectorViewY/vectorViewX);
        }
        double tilt = -std::acos(-vectorViewZ);

        // - To validate ! 
        if (!confMH->left) {
            pan *= -1;
        }

        // -- transform to degree
        pan *= 180.0 / M_PI;
        tilt *= 180.0 / M_PI;

        //Poco::Logger::get("MovingHeadProjection").debug("Pan and tilt in degree :" + std::to_string(pan) + ", " + std::to_string(tilt), __FILE__, __LINE__);
        
        std::shared_ptr<OutputMovingHeadAngle> output = std::make_shared<OutputMovingHeadAngle>(OutputMovingHeadAngle());
        output->name = confMH->name;
        output->color = mhOutput->color;
        output->pan = pan;
        output->tilt = tilt;
        outputs.push(output);
        mhOutput.reset();
        output.reset();
    }

    return true;
}

MovingHeadProjection::OutputMovingHeadAngle* MovingHeadProjection::projectPanTiltFromRatio(IceConfiguration* ice, std::string deviceId, double ratioX, double ratioY)
{
    double epsilon = 1e-6;

    if (ice == nullptr) {
        Poco::Logger::get("MovingHeadProjection").error("Ice configuration is nullptr !", __FILE__, __LINE__);
        return nullptr;
    }

    IceConfiguration::MovingheadConf* confMH = ice->getMovingHead(deviceId);

    if (confMH == nullptr) {
        Poco::Logger::get("MovingHeadProjection").error("Can't find device in Ice configuration !", __FILE__, __LINE__);
        return nullptr;
    }

    if (ratioX < -epsilon || ratioY < -epsilon) {
        Poco::Logger::get("MovingHeadProjection").error( "Angles not correct : skipped!", __FILE__, __LINE__);
        return nullptr;
    }

    OutputMovingHeadAngle* output = new OutputMovingHeadAngle();

    // -- we determine absolute position of pointage required
    // -- Z is gradually raising with y
    double pointerRequiredX = (confMH->maxX - confMH->minX) * ratioX + confMH->minX;
    double pointerRequiredY = (confMH->maxY - confMH->minY) * ratioY + confMH->minY;
    double pointerRequiredZ = (confMH->maxZ - confMH->minZ) * ratioY + confMH->minZ;

    //Poco::Logger::get("MovingHeadProjection").debug("Moving head must point to (X,Y,Z) : (" + 
    //    std::to_string(pointerRequiredX) + "," + std::to_string(pointerRequiredY) + ","  +std::to_string(pointerRequiredZ) + ")", __FILE__, __LINE__);

    // -- we determine vector from movingHead to pointed position
    double vectorViewX = pointerRequiredX - confMH->offsetX;
    double vectorViewY = pointerRequiredY - confMH->offsetY;
    double vectorViewZ = pointerRequiredZ - confMH->offsetZ;

    // -- normalize vector 
    double length = sqrt(vectorViewX*vectorViewX+vectorViewY*vectorViewY+vectorViewZ*vectorViewZ);
    vectorViewX /= length;
    vectorViewY /= length;
    vectorViewZ /= length;

    double pan = 0.0;
    if (confMH->left) {
        pan = std::atan(-vectorViewY/vectorViewX);
    }
    else {
        pan = std::atan(vectorViewY/vectorViewX);
    }
    double tilt = -std::acos(-vectorViewZ);

    // - To validate ! 
    if (!confMH->left) {
        pan *= -1;
    }

    // -- transform to degree
    pan *= 180.0 / M_PI;
    tilt *= 180.0 / M_PI;

    //Poco::Logger::get("MovingHeadProjection").debug("Pan and tilt in degree :" + std::to_string(pan) + ", " + std::to_string(tilt), __FILE__, __LINE__);

    output->pan = pan;
    output->tilt = tilt;

    return output;
}

std::shared_ptr<MovingHeadProjection::OutputMovingHeadAngle> MovingHeadProjection::getOutput()
{
    std::shared_ptr<OutputMovingHeadAngle> ret = nullptr;
    if (!outputs.empty()) {
        ret = outputs.front();
        outputs.pop();
    }
    return ret;
}