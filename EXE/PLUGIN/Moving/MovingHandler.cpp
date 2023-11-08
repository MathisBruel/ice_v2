#include "MovingHandler.h"

int MovingHandler::offsetXLeft = 1;
int MovingHandler::offsetXRight = 69;
int MovingHandler::sizeCube = 64;
int MovingHandler::offsetYFront = 1;
int MovingHandler::offsetYRear = 88;
int MovingHandler::sizeYColor = 20;

MovingHandler::MovingHandler()
{
    this->type = FRONT_LEFT;
    nbFrames = -1;
    frameToRender = -1;
    needSave = false;
    scaleFactor = 1.0;
    translateX = 0.0;
    translateY = 0.0;
    rotationAngle = 0.0;
    centerX = 0.5;
    centerY = 0.5;
    visible = true;
    resizeTimeline = 1.0;
    translateTimeline = 0;

    int num1 = std::rand() % 99;
    int num2 = std::rand() % 99;
    int num3 = std::rand() % 99;
    int num4 = std::rand() % 99;

    uuid = std::to_string(num1) + "-" + std::to_string(num2) + "-" + std::to_string(num3) + "-" + std::to_string(num4);
}
    
MovingHandler::~MovingHandler()
{
    for (int i = 0; i < pos.size(); i++) {
        delete pos.at(i);
    }
    for (int i = 0; i < color.size(); i++) {
        delete color.at(i);
    }
    for (int i = 0; i < posInterpolate.size(); i++) {
        delete posInterpolate.at(i);
    }
    for (int i = 0; i < colorInterpolate.size(); i++) {
        delete colorInterpolate.at(i);
    }
}

void MovingHandler::reloadPositions(MovingHandler * ref)
{
    // -- clean old position values
    for (int i = 0; i < pos.size(); i++) {
        delete pos.at(i);
    }
    pos.clear();
    for (int i = 0; i < posInterpolate.size(); i++) {
        delete posInterpolate.at(i);
    }
    posInterpolate.clear();

    // -- copy new values
    for (int i = 0; i < ref->pos.size(); i++) {
        Position* copyPos = new Position();
        std::memcpy(copyPos, ref->pos.at(i), sizeof(Position));
        pos.push_back(copyPos);
    }
    for (int i = 0; i < ref->posInterpolate.size(); i++) {
        Interpolation* copyInter = new Interpolation();
        std::memcpy(copyInter, ref->posInterpolate.at(i), sizeof(Interpolation));
        posInterpolate.push_back(copyInter);
    }

    // -- resize new position to fit
    double ratioTempo = (double)nbFrames / (double)ref->nbFrames;
    for (int i = 0; i < pos.size(); i++) {
        int newFrame = (pos.at(i)->frame * ratioTempo) + 0.5;
                
        // -- handle minimum delta of one frame
        if (i > 0 && pos.at(i-1)->frame == newFrame) {
            newFrame++;
        }
        pos.at(i)->frame = newFrame;
    }

    needSave = true;
}
void MovingHandler::reloadColors(MovingHandler * ref)
{
    // -- clean old colors values
    for (int i = 0; i < color.size(); i++) {
        delete color.at(i);
    }
    color.clear();
    for (int i = 0; i < colorInterpolate.size(); i++) {
        delete colorInterpolate.at(i);
    }
    colorInterpolate.clear();

    // -- copy new values
    for (int i = 0; i < ref->color.size(); i++) {
        Color* copyColor = new Color();
        std::memcpy(copyColor, ref->color.at(i), sizeof(Color));
        color.push_back(copyColor);
    }
    for (int i = 0; i < ref->colorInterpolate.size(); i++) {
        Interpolation* copyInter = new Interpolation();
        std::memcpy(copyInter, ref->colorInterpolate.at(i), sizeof(Interpolation));
        colorInterpolate.push_back(copyInter);
    }

    // -- resize new colors to fit
    double ratioTempo = (double)nbFrames / (double)ref->nbFrames;
    for (int i = 0; i < color.size(); i++) {
        int newFrame = (color.at(i)->frame * ratioTempo) + 0.5;
                
        // -- handle minimum delta of one frame
        if (i > 0 && color.at(i-1)->frame == newFrame) {
            newFrame++;
        }
        color.at(i)->frame = newFrame;
    }

    needSave = true;
}

void MovingHandler::changetype(MovingHeadType typeTemp)
{
    this->type = typeTemp;
    needSave = true;
}
void MovingHandler::setNbFrames(long nbFramesTemp) 
{
    // -- init no points at all
    if (nbFrames == -1 || nbFrames == 0) {

        // -- set nb frames
        nbFrames = nbFramesTemp;
        needSave = true;
    }

    // -- resizing
    else if (nbFramesTemp != nbFrames) {
        
        bool lastAlreadyCreated = false;

        std::vector<Position *>::iterator it;
        std::vector<Interpolation *>::iterator itInter = posInterpolate.begin();
        // -- resize position
        for (it = pos.begin(); it != pos.end(); it++) {
            Position* temp = (*it);

            // -- nothing to do
            if (temp->frame < nbFramesTemp) {}

            // -- no need to create another point
            // -- cut can start after this
            else if (temp->frame == nbFramesTemp-1) {
                lastAlreadyCreated = true;
            }

            // -- start cut
            else {
                if (!lastAlreadyCreated) {

                    // -- create new position
                    Position* newPos = getPositionAtTime(nbFramesTemp-1);
                    newPos->frame = nbFramesTemp-1;
                    it = pos.insert(it, newPos);
                    it++;

                    // -- create new interpolation
                    Interpolation* newInter = new Interpolation();
                    newInter->type = 1;
                    itInter = posInterpolate.insert(itInter, newInter);
                    itInter++;

                    lastAlreadyCreated = true;
                }

                delete (*itInter);
                itInter = posInterpolate.erase(itInter);
                itInter--;
                delete (*it);
                it = pos.erase(it);
                it--;
                if (it == pos.end()) {break;}
            }

            itInter++;
        }

        bool lastColorAlreadyCreated = false;

        std::vector<Color *>::iterator itColor;
        std::vector<Interpolation *>::iterator itInterColor = colorInterpolate.begin();
        // -- resize position
        for (itColor = color.begin(); itColor != color.end(); itColor++) {
            Color* temp = (*itColor);

            // -- nothing to do
            if (temp->frame < nbFramesTemp) {}

            // -- no need to create another point
            // -- cut can start after this
            else if (temp->frame == nbFramesTemp-1) {
                lastColorAlreadyCreated = true;
            }

            // -- start cut
            else {
                if (!lastColorAlreadyCreated) {

                    // -- create new color
                    Color* newColor = getColorAtTime(nbFramesTemp-1);
                    newColor->frame = nbFramesTemp-1;
                    itColor = color.insert(itColor, newColor);
                    itColor++;

                    // -- create new interpolation
                    Interpolation* newInter = new Interpolation();
                    newInter->type = 1;
                    itInterColor = colorInterpolate.insert(itInterColor, newInter);
                    itInterColor++;

                    lastColorAlreadyCreated = true;
                }

                delete (*itInterColor);
                itInterColor = colorInterpolate.erase(itInterColor);
                itInterColor--;
                delete (*itColor);
                itColor = color.erase(itColor);
                itColor--;
                if (itColor == color.end()) {break;}
            }

            itInterColor++;
        }
    
        nbFrames = nbFramesTemp;
        recalculateAllInterpolation();
        needSave = true;
    }

    else {needSave = false;}
}

bool MovingHandler::isPositionSetAtTime(long currentFrame)
{
    if (currentFrame < 0 || currentFrame >= nbFrames){
        return false;
    }

    for (int i = 0; i < pos.size(); i++) {
        if (pos.at(i)->frame == currentFrame) {
            return true;
        }
    }

    return false;
}
bool MovingHandler::isColorSetAtTime(long currentFrame)
{
    if (currentFrame < 0 || currentFrame >= nbFrames){
        return false;
    }

    for (int i = 0; i < color.size(); i++) {
        if (color.at(i)->frame == currentFrame) {
            return true;
        }
    }

    return false;
}
Position* MovingHandler::getPositionAtTime(long currentFrame)
{
    if (currentFrame < 0 || currentFrame >= nbFrames){
        return nullptr;
    }
    else if (pos.size() == 0) {
        return nullptr;
    }
    else if (currentFrame < pos.at(0)->frame) {
        return nullptr;
    }
    else if (currentFrame > pos.at(pos.size()-1)->frame) {
        return nullptr;
    }

    for (int i = 0; i < pos.size(); i++) {

        Position* current = pos.at(i);

        // -- frame is specified : no interpolation
        if (current->frame == currentFrame) {
            Position* posTemp = new Position();
            posTemp->ratioX = current->ratioX;
            posTemp->ratioY = current->ratioY;

            // -- apply temporary transformation not applied
            if (scaleFactor != 1.0) {
                Position* posScale = scalePoint(posTemp);
                delete posTemp;
                posTemp = posScale;
            }
            if (rotationAngle != 0.0) {
                Position* posRotate = rotatePoint(posTemp);
                delete posTemp;
                posTemp = posRotate;
            }
            if (translateX != 0.0 || translateY != 0.0) {
                Position* posTranslate = translatePoint(posTemp);
                delete posTemp;
                posTemp = posTranslate;
            }

            return posTemp;
        }

        // -- nothing to do for now
        else if (current->frame < currentFrame) {}

        // -- linear interpolation at this time : TODO rest
        // -- implies current->frame > currentFrame
        else {

            if (i > 0) {

                Position* posTemp = new Position();

                if (posInterpolate.at(i-1)->type < 2) {
                    // -- LINEAR INTERPOLATION to START
                    Position* last = pos.at(i-1);
                    double ratioTime = (double)(currentFrame - last->frame) / (double)(current->frame - last->frame);
                    posTemp->ratioX = last->ratioX + (current->ratioX - last->ratioX)*ratioTime;
                    posTemp->ratioY = last->ratioY + (current->ratioY - last->ratioY)*ratioTime;
                }
                else {
                    Interpolation* inter = posInterpolate.at(i-1);
                    if (!inter->valid) {
                        delete posTemp;
                        posTemp = nullptr;
                    }
                    
                    // -- CURVE
                    else if (inter->type == 2) {
                        // -- we calculate relative point in vector
                        double xTemp = inter->startX + inter->vectorX*(currentFrame-pos.at(i-1)->frame);
                        double yTemp = inter->startY + inter->vectorY*(currentFrame-pos.at(i-1)->frame);

                        if (inter->alongX) {
                            posTemp->ratioX = xTemp;
                            posTemp->ratioY = inter->a*xTemp*xTemp+inter->b*xTemp+inter->c;
                        }
                        else {
                            posTemp->ratioX = inter->a*yTemp*yTemp+inter->b*yTemp+inter->c;
                            posTemp->ratioY = yTemp;
                        }
                    }

                    // -- ARC
                    else if (inter->type == 3) {
                        // -- we calculate relative point in vector
                        double angle = (inter->startAngle + inter->stepAngle*(currentFrame-pos.at(i-1)->frame))*PI/180.0;
                        posTemp->ratioX = inter->centerX + inter->radius * std::cos(angle);
                        posTemp->ratioY = inter->centerY - inter->radius * std::sin(angle);
                    }
                }
            
                // -- apply temporary transformation not applied
                if (scaleFactor != 1.0) {
                    Position* posScale = scalePoint(posTemp);
                    delete posTemp;
                    posTemp = posScale;
                }
                if (rotationAngle != 0.0) {
                    Position* posRotate = rotatePoint(posTemp);
                    delete posTemp;
                    posTemp = posRotate;
                }
                if (translateX != 0.0 || translateY != 0.0) {
                    Position* posTranslate = translatePoint(posTemp);
                    delete posTemp;
                    posTemp = posTranslate;
                }

                return posTemp;
            }
        }
    }

    return nullptr;
}
Color* MovingHandler::getColorAtTime(long currentFrame)
{
    if (currentFrame < 0 || currentFrame >= nbFrames){
        return nullptr;
    }
    else if (color.size() == 0) {
        return nullptr;
    }
    else if (currentFrame < color.at(0)->frame) {
        return nullptr;
    }
    else if (currentFrame > color.at(color.size()-1)->frame) {
        return nullptr;
    }

    for (int i = 0; i < color.size(); i++) {

        Color* current = color.at(i);

        // -- frame is specified : no interpolation
        if (current->frame == currentFrame) {
            Color* colorTemp = new Color();
            colorTemp->red = current->red;
            colorTemp->green = current->green;
            colorTemp->blue = current->blue;
            colorTemp->alpha = current->alpha;
            return colorTemp;
        }

        // -- nothing to do for now
        else if (current->frame < currentFrame) {}

        // -- linear interpolation at this time : TODO rest
        // -- implies current->frame > currentFrame
        else {
            if (i > 0) {
                Color* colorTemp = new Color();
                Color* last = color.at(i-1);

                if (colorInterpolate.at(i-1)->type < 2) {
                    double ratioTime = (double)(currentFrame - last->frame) / (double)(current->frame - last->frame);
                    colorTemp->red = last->red + (current->red - last->red)*ratioTime;
                    colorTemp->green = last->green + (current->green - last->green)*ratioTime;
                    colorTemp->blue = last->blue + (current->blue - last->blue)*ratioTime;
                    colorTemp->alpha = last->alpha + (current->alpha - last->alpha)*ratioTime;
                    return colorTemp;
                }
                else {
                    Interpolation* inter = colorInterpolate.at(i-1);

                    // -- FAST
                    if (inter->type == 2) {
                        
                        double ratioTime = (double)(currentFrame - last->frame) / (double)(current->frame - last->frame);
                        double factor = (1.0-exp(-ratioTime/(inter->extra/100.0)) - inter->offset)*inter->ratio;
                        colorTemp->red = last->red + (current->red - last->red)*factor;
                        colorTemp->green = last->green + (current->green - last->green)*factor;
                        colorTemp->blue = last->blue + (current->blue - last->blue)*factor;
                        colorTemp->alpha = last->alpha + (current->alpha - last->alpha)*factor;
                        return colorTemp;
                    }

                    // -- SLOW
                    else if (inter->type == 3) {
                        // -- we calculate relative point in vector
                        double ratioTime = 1.0 - ((double)(currentFrame - last->frame) / (double)(current->frame - last->frame));
                        double factor = (exp(-ratioTime/(inter->extra/100.0)) - inter->offset)*inter->ratio;
                        colorTemp->red = last->red + (current->red - last->red)*factor;
                        colorTemp->green = last->green + (current->green - last->green)*factor;
                        colorTemp->blue = last->blue + (current->blue - last->blue)*factor;
                        colorTemp->alpha = last->alpha + (current->alpha - last->alpha)*factor;
                        return colorTemp;
                    }
                }
            }
        }
    }

    return nullptr;
}
Interpolation* MovingHandler::getPosInterpolationAtTime(long currentFrame)
{
    if (currentFrame < 0 || currentFrame >= nbFrames){
        return nullptr;
    }
    else if (pos.size() == 0) {
        return nullptr;
    }
    else if (currentFrame < pos.at(0)->frame) {
        return nullptr;
    }
    else if (currentFrame > pos.at(pos.size()-1)->frame) {
        return nullptr;
    }

    for (int i = 0; i < pos.size(); i++) {

        Position* current = pos.at(i);

        // -- frame is specified : no interpolation
        if (current->frame == currentFrame) {
            return posInterpolate.at(i);
        }

        // -- nothing to do for now
        else if (current->frame < currentFrame) {}

        // -- linear interpolation at this time : TODO rest
        // -- implies current->frame > currentFrame
        else {
            if (i > 0) {
                return posInterpolate.at(i-1);
            }
        }
    }

    return nullptr;
}
Interpolation* MovingHandler::getColorInterpolationAtTime(long currentFrame)
{
    if (currentFrame < 0 || currentFrame >= nbFrames){
        return nullptr;
    }
    else if (color.size() == 0) {
        return nullptr;
    }
    else if (currentFrame < color.at(0)->frame) {
        return nullptr;
    }
    else if (currentFrame > color.at(color.size()-1)->frame) {
        return nullptr;
    }

    for (int i = 0; i < color.size(); i++) {

        Color* current = color.at(i);

        // -- frame is specified : no interpolation
        if (current->frame == currentFrame) {
            return colorInterpolate.at(i);
        }

        // -- nothing to do for now
        else if (current->frame < currentFrame) {}

        // -- linear interpolation at this time : TODO rest
        // -- implies current->frame > currentFrame
        else {
            if (i > 0) {
                return colorInterpolate.at(i-1);
            }
        }
    }

    return nullptr;
}

void MovingHandler::applyTimelineResize()
{
    // -- calculate ratio
    double ratioTempo = resizeTimeline;

    // -- resize position
    for (int i = 0; i < pos.size(); i++) {
        int newFrame = (pos.at(i)->frame * ratioTempo) + 0.5;
        
        // -- handle minimum delta of one frame
        if (i > 0 && pos.at(i-1)->frame == newFrame) {
            newFrame++;
        }
        pos.at(i)->frame = newFrame;
    }

    // -- resize color
    for (int i = 0; i < color.size(); i++) {
        int newFrame = (color.at(i)->frame * ratioTempo) + 0.5;
        
        // -- handle minimum delta of one frame
        if (i > 0 && color.at(i-1)->frame == newFrame) {
            newFrame++;
        }
        color.at(i)->frame = newFrame;
    }

    setNbFrames(nbFrames*ratioTempo);
    resizeTimeline = 1.0;
}
void MovingHandler::applyTimelineTranslation()
{
    std::vector<Position *>::iterator it;
    // -- resize position
    for (it = pos.begin(); it != pos.end(); it++) {
        Position* temp = (*it);
        
        if (temp->frame+translateTimeline < 0) {
            Position * posTemp = getPositionAtTime(-temp->frame+translateTimeline);
            temp->frame = 0;
            temp->ratioX = posTemp->ratioX;
            temp->ratioY = posTemp->ratioY;
            delete posTemp;
        }
        else if (temp->frame+translateTimeline >= nbFrames) {
            Position * posTemp = getPositionAtTime(nbFrames-translateTimeline-1);
            temp->frame = nbFrames-1;
            temp->ratioX = posTemp->ratioX;
            temp->ratioY = posTemp->ratioY;
            delete posTemp;
        }
        else {
            temp->frame += translateTimeline;
        }
    }

    std::vector<Color *>::iterator itC;
    // -- resize position
    for (itC = color.begin(); itC != color.end(); itC++) {
        Color* temp = (*itC);
        
        if (temp->frame+translateTimeline < 0) {
            Color * posTemp = getColorAtTime(-temp->frame+translateTimeline);
            temp->frame = 0;
            temp->alpha = posTemp->alpha;
            temp->red = posTemp->red;
            temp->green = posTemp->green;
            temp->blue = posTemp->blue;
            delete posTemp;
        }
        else if (temp->frame+translateTimeline >= nbFrames) {
            Color * posTemp = getColorAtTime(nbFrames-translateTimeline-1);
            temp->frame = nbFrames-1;
            temp->alpha = posTemp->alpha;
            temp->red = posTemp->red;
            temp->green = posTemp->green;
            temp->blue = posTemp->blue;
            delete posTemp;
        }
        else {
            temp->frame += translateTimeline;
        }
    }
    translateTimeline = 0;
    needSave = true;
}
void MovingHandler::reverseTimeline()
{
    if ((pos.size() == 0 && color.size() == 0) || nbFrames == 0 || nbFrames == -1) {
        return;
    }

    std::vector<Position*> newVectorPos;
    std::vector<Interpolation*> newVectorPosInter;
    for (int i = pos.size()-1; i >= 0; i--) {
        pos.at(i)->frame = nbFrames - pos.at(i)->frame - 1;
        newVectorPos.push_back(pos.at(i));

        // -- last one stay last one
        if (i < pos.size()-1) {
            newVectorPosInter.push_back(posInterpolate.at(i));

            // -- inverse extra data to keep same shape
            posInterpolate.at(i)->extra *= -1;
            posInterpolate.at(i)->extra2 *= -1;
        }
    }
    newVectorPosInter.push_back(posInterpolate.at(pos.size()-1));
    pos = newVectorPos;
    posInterpolate = newVectorPosInter;

    std::vector<Color*> newVectorColor;
    std::vector<Interpolation*> newVectorColorInter;
    for (int i = color.size()-1; i >= 0; i--) {
        color.at(i)->frame = nbFrames - color.at(i)->frame - 1;
        newVectorColor.push_back(color.at(i));
        // -- last one stay last one
        if (i < color.size()-1) {
            newVectorColorInter.push_back(colorInterpolate.at(i));

            // -- inverse logic on interpolation
            if (colorInterpolate.at(i)->type == 2) colorInterpolate.at(i)->type = 3;
            else if (colorInterpolate.at(i)->type == 3) colorInterpolate.at(i)->type = 2;
        }
    }
    newVectorColorInter.push_back(colorInterpolate.at(color.size()-1));
    color = newVectorColor;
    colorInterpolate = newVectorColorInter;

    recalculateAllInterpolation();
    needSave = true;
}

void MovingHandler::createPosition(double x, double y)
{
    if (!isFrameValid()) {
        return;
    }
    else if (frameToRender < 0 || frameToRender >= nbFrames){
        return;
    }

    Position* newPos = new Position();
    newPos->frame = frameToRender;
    newPos->ratioX = x / 100.0;
    newPos->ratioY = y / 100.0;
    Interpolation* newInter = new Interpolation();
    newInter->type = 0;
    newInter->extra = 0.0;
    newInter->extra2 = 0.0;

    std::vector<Position*>::iterator it;
    std::vector<Interpolation*>::iterator interIt;
    bool added = false;
    if (pos.size() > 0) {
        interIt = posInterpolate.begin();
        for (it = pos.begin(); it != pos.end(); it++) {
            if (newPos->frame < (*it)->frame) {
                pos.insert(it, newPos);
                posInterpolate.insert(interIt, newInter);
                added = true;
                break;
            }
            else if (newPos->frame == (*it)->frame) {
                (*it)->ratioX = newPos->ratioX;
                (*it)->ratioY = newPos->ratioY;
                delete newPos;
                delete newInter;
                added = true;
                break;
            }
            else {
                // -- nothing to do
            }

            interIt++;
        }
    }

    if (!added) {
        pos.push_back(newPos);
        posInterpolate.push_back(newInter);
    }

    recalculateAllInterpolation();

    needSave = true;
}
void MovingHandler::removePosition()
{
    std::vector<Position*>::iterator it;
    std::vector<Interpolation*>::iterator interIt;

    interIt = posInterpolate.begin();
    for (it = pos.begin(); it != pos.end(); it++) {
        if ((*it)->frame == frameToRender) {
            pos.erase(it);
            posInterpolate.erase(interIt);
            break;
        }

        interIt++;
    }

    recalculateAllInterpolation();
    needSave = true;
}
void MovingHandler::changePosInterpolate(int typeNew, double extra, double extra2)
{
    if (!isFrameValid()) {
        return;
    }
    else if (frameToRender < 0 || frameToRender >= nbFrames){
        return;
    }

    for (int i = 0; i < pos.size(); i++) {

        if (frameToRender == pos.at(i)->frame) {
            posInterpolate.at(i)->type = typeNew;
            posInterpolate.at(i)->extra = extra;
            posInterpolate.at(i)->extra2 = extra2;

            // -- recalculate curve or arc
            if (type == 2) {recalculateCurve(i);}
            else if (type == 3) {recalculateArc(i);}
            break;
        }

        else if (frameToRender < pos.at(i)->frame) {

			if (i > 0) {
				posInterpolate.at(i-1)->type = typeNew;
				posInterpolate.at(i-1)->extra = extra;
				posInterpolate.at(i-1)->extra2 = extra2;

				// -- recalculate curve or arc
				if (type == 2) { recalculateCurve(i-1); }
				else if (type == 3) { recalculateArc(i-1); }
			}
            break;
        }
    }

    needSave = true;
}
void MovingHandler::createColor(int red, int green, int blue, int alpha)
{
    if (!isFrameValid()) {
        return;
    }
    else if (frameToRender < 0 || frameToRender >= nbFrames){
        return;
    }

    Color* newColor = new Color();
    newColor->frame = frameToRender;
    newColor->red = red;
    newColor->green = green;
    newColor->blue = blue;
    newColor->alpha = alpha;
    Interpolation* newInter = new Interpolation();
    newInter->type = 0;
    newInter->extra = 50.0;

    std::vector<Color*>::iterator it;
    std::vector<Interpolation*>::iterator interIt;
    bool added = false;
    if (color.size() > 0) {

        interIt = colorInterpolate.begin();
        for (it = color.begin(); it != color.end(); it++) {
            if (newColor->frame < (*it)->frame) {
                color.insert(it, newColor);
                colorInterpolate.insert(interIt, newInter);
                added = true;
                break;
            }
            else if (newColor->frame == (*it)->frame) {
                (*it)->red = newColor->red;
                (*it)->green = newColor->green;
                (*it)->blue = newColor->blue;
                (*it)->alpha = newColor->alpha;
                delete newColor;
                delete newInter;
                added = true;
                break;
            }
            else {
                // -- nothing to do
            }

            interIt++;
        }
    }

    if (!added) {
        color.push_back(newColor);
        colorInterpolate.push_back(newInter);
    }

    recalculateAllInterpolation();
    needSave = true;
}
void MovingHandler::removeColor()
{
    std::vector<Color*>::iterator it;
    std::vector<Interpolation*>::iterator interIt;

    interIt = colorInterpolate.begin();
    for (it = color.begin(); it != color.end(); it++) {
        if ((*it)->frame == frameToRender) {
            color.erase(it);
            colorInterpolate.erase(interIt);
            break;
        }

        interIt++;
    }

    recalculateAllInterpolation();
    needSave = true;
}   
void MovingHandler::changeColorInterpolate(int typeNew, double extra)
{
    if (!isFrameValid()) {
        return;
    }
    else if (frameToRender < 0 || frameToRender >= nbFrames){
        return;
    }

    for (int i = 0; i < color.size(); i++) {

        if (frameToRender == color.at(i)->frame) {
            colorInterpolate.at(i)->type = typeNew;
            colorInterpolate.at(i)->extra = extra;
            if (type >= 2) {recalculateColorInter(i);}
            break;
        }

        else if (frameToRender < color.at(i)->frame) {

			if (i > 0) {
				colorInterpolate.at(i-1)->type = typeNew;
				colorInterpolate.at(i-1)->extra = extra;
				if (type >= 2) { recalculateColorInter(i-1); }
			}
            break;
        }
    }

    needSave = true;
}

void MovingHandler::applyScaling()
{
    if (scaleFactor != 1.0) {
        for (int i = 0; i < pos.size(); i++) {
            Position* currentPos = pos.at(i);
            Position* newPos = scalePoint(currentPos);
            currentPos->ratioX = newPos->ratioX;
            currentPos->ratioY = newPos->ratioY;
            delete newPos;
        }

        scaleFactor = 1.0;
        needSave = true;
    }
}
void MovingHandler::applyTranslation()
{
    if (translateX != 0.0 || translateY != 0.0) {
        for (int i = 0; i < pos.size(); i++) {
            Position* currentPos = pos.at(i);
            Position* newPos = translatePoint(currentPos);
            currentPos->ratioX = newPos->ratioX;
            currentPos->ratioY = newPos->ratioY;
            delete newPos;
        }

        translateX = 0.0;
        translateY = 0.0;
        needSave = true;
    }
}
void MovingHandler::applyRotation()
{
    if (rotationAngle != 0.0) {
        for (int i = 0; i < pos.size(); i++) {
            Position* currentPos = pos.at(i);
            Position* newPos = rotatePoint(currentPos);
            currentPos->ratioX = newPos->ratioX;
            currentPos->ratioY = newPos->ratioY;
            delete newPos;
        }

        rotationAngle = 0.0;
        centerX = 0.5;
        centerY = 0.5;
        needSave = true;
    }
}
Position* MovingHandler::scalePoint(Position* inPoint)
{
    Position* posTemp = new Position();

    // -- define vector
    double vectorX = inPoint->ratioX - 0.5;
    double vectorY = inPoint->ratioY - 0.5;

    // -- normalize vector
    // -- scale vector
    vectorX *= scaleFactor;
    vectorY *= scaleFactor;

    // -- final position
    posTemp->ratioX = 0.5 + vectorX;
    posTemp->ratioY = 0.5 + vectorY;

    return posTemp;
}
Position* MovingHandler::translatePoint(Position* inPoint)
{
    Position* posTemp = new Position();
    posTemp->ratioX = inPoint->ratioX + translateX;
    posTemp->ratioY = inPoint->ratioY + translateY;
    return posTemp;
}
Position* MovingHandler::rotatePoint(Position* inPoint)
{
    Position* posTemp = new Position();
    double sin = std::sin(rotationAngle*PI/180.0);
    double cos = std::cos(rotationAngle*PI/180.0);

    // translate point back to origin
    double tempX = inPoint->ratioX - centerX;
    double tempY = inPoint->ratioY - centerY;

    // rotate point
    posTemp->ratioX = tempX * cos - tempY * sin;
    posTemp->ratioY = tempX * sin + tempY * cos;

    // translate point back
    posTemp->ratioX += centerX;
    posTemp->ratioY += centerY;

    return posTemp;
}

void MovingHandler::recalculateCurve(int idx)
{
    // -- nothing to do here
    if (idx == pos.size()-1) {
        return;
    }

    Interpolation* inter = posInterpolate.at(idx);

    // -- get start/end points
    Position* start = pos.at(idx);
    Position* end = pos.at(idx+1);
    long duration = end->frame - start->frame;
    inter->startX = start->ratioX;
    inter->startY = start->ratioY;

    // -- calculate middle : TODO : change to set anywhere here
    double xMid = (end->ratioX + start->ratioX) / 2.0;
    double yMid = (end->ratioY + start->ratioY) / 2.0;

    double vectorXTemp = end->ratioX - start->ratioX;
    double vectorYTemp = end->ratioY - start->ratioY;

    // -- calculate midOffsetted
    xMid += vectorXTemp*inter->extra2/200.0;
    yMid += vectorYTemp*inter->extra2/200.0;

    // -- calculate normal vector
    double normalX = -vectorYTemp;
    double normalY = vectorXTemp;

    // -- calculate length && transform
    double lenRatio = (inter->extra/100.0) / (std::sqrt(normalX*normalX+normalY*normalY));
    normalX *= lenRatio;
    normalY *= lenRatio;
    inter->tempX = xMid + normalX;
    inter->tempY = yMid + normalY;

    // -- steps for each frame
    inter->vectorX = vectorXTemp / (double)duration;
    inter->vectorY = vectorYTemp / (double)duration;

    double x1 = start->ratioX;
    double y1 = start->ratioY;
    double x2 = inter->tempX;
    double y2 = inter->tempY;
    double x3 = end->ratioX;
    double y3 = end->ratioY;

    inter->valid = true;
    inter->alongX = true;

    // -- calculate over Y since no difference along X
    if (x1 == x2 || x1 == x3 || x2 == x3) {

        // -- no calculation are possible
        if (y1 == y2 || y1 == y3 || y2 == y3) {
            inter->valid = false;
            return;
        }
        
        inter->alongX = false;
        inter->a = ((x1-x2)*(y2-y3) - (x2-x3)*(y1-y2)) / 
                ((y1*y1-y2*y2)*(y2-y3) - (y2*y2-y3*y3)*(y1-y2));

        inter->b = ((x1-x2) - inter->a*(y1*y1-y2*y2)) / 
                    (y1-y2);

        inter->c = x1 - inter->a*y1*y1 - inter->b*y1;
    }
    else {
        // -- calculate curve parameters
        inter->a = ((y1-y2)*(x2-x3) - (y2-y3)*(x1-x2)) / 
                ((x1*x1-x2*x2)*(x2-x3) - (x2*x2-x3*x3)*(x1-x2));

        inter->b = ((y1-y2) - inter->a*(x1*x1-x2*x2)) / 
                    (x1-x2);

        inter->c = y1 - inter->a*x1*x1 - inter->b*x1;
    }
}
void MovingHandler::recalculateArc(int idx)
{
    // -- nothing to do here
    if (idx == pos.size()-1) {
        return;
    }

    Interpolation* inter = posInterpolate.at(idx);

    if (inter->extra == 0) {
        inter->valid = false;
        return;
    }

    // -- get start/end points
    Position* start = pos.at(idx);
    Position* end = pos.at(idx+1);
    long duration = end->frame - start->frame;

    // -- calculate middle : TODO : change to set anywhere here
    double xMid = (end->ratioX + start->ratioX) / 2.0;
    double yMid = (end->ratioY + start->ratioY) / 2.0;

    double vectorXTemp = end->ratioX - start->ratioX;
    double vectorYTemp = end->ratioY - start->ratioY;

    // -- calculate normal vector
    double normalX = -vectorYTemp;
    double normalY = vectorXTemp;

    // -- calculate length && transform
    double lenRatio = (inter->extra/100.0) / (std::sqrt(normalX*normalX+normalY*normalY));
    normalX *= lenRatio;
    normalY *= lenRatio;
    inter->tempX = xMid + normalX;
    inter->tempY = yMid + normalY;

    double x1 = start->ratioX;
    double y1 = start->ratioY;
    double x2 = inter->tempX;
    double y2 = inter->tempY;
    double x3 = end->ratioX;
    double y3 = end->ratioY;

    inter->valid = true;

    // -- calculate circle
    double f = ((x1*x1-x3*x3)*(x1-x2) + (y1*y1-y3*y3)*(x1-x2) + (x2*x2-x1*x1)*(x1-x3) + (y2*y2-y1*y1)*(x1-x3)) / 
                (2*((y3-y1)*(x1-x2) - (y2-y1)*(x1-x3)));

    double g = ((x1*x1-x3*x3)*(y1-y2) + (y1*y1-y3*y3)*(y1-y2) + (x2*x2-x1*x1)*(y1-y3) + (y2*y2-y1*y1)*(y1-y3)) / 
                (2*((x3-x1)*(y1-y2) - (x2-x1)*(y1-y3)));

    double c = -(x1*x1) - (y1*y1) - 2*g*x1 - 2*f*y1;

    // -- center and radius calculation
    inter->centerX = -g;
    inter->centerY = -f;
    inter->radius = std::sqrt(inter->centerX*inter->centerX + inter->centerY*inter->centerY - c);

    // -- angle of the arc
    inter->startAngle = -std::atan2(y1-inter->centerY, x1-inter->centerX)*180.0/PI;
    inter->endAngle = -std::atan2(y3-inter->centerY, x3-inter->centerX)*180.0/PI;
    inter->midAngle = -std::atan2(y2-inter->centerY, x2-inter->centerX)*180.0/PI;

    if (inter->midAngle < inter->endAngle && inter->startAngle > inter->midAngle) 
    {
		if (std::fabs(inter->endAngle - inter->midAngle) > std::fabs(inter->startAngle - inter->midAngle)) {inter->endAngle -= 360;}
		else {inter->startAngle -= 360;}
    }
    else if (inter->midAngle > inter->startAngle && inter->endAngle < inter->midAngle) {
        if (std::fabs(inter->endAngle - inter->midAngle) > std::fabs(inter->startAngle - inter->midAngle)) {inter->endAngle += 360;}
        else {inter->startAngle += 360;}
    }

    // -- steps for each frame
    inter->stepAngle = (inter->endAngle - inter->startAngle) / (double)duration;
}
void MovingHandler::recalculateColorInter(int idx)
{
    // -- nothing to do here
    if (idx == color.size()-1) {
        return;
    }

    Interpolation* inter = colorInterpolate.at(idx);

    // -- FAST RISE
    if (inter->type == 2) {
        inter->offset = 0.0;
        inter->ratio = 1.0/(1.0 - std::exp(-1/(inter->extra/100.0)));
    }
    // -- SLOW RISE
    else if (inter->type == 3) {
        inter->offset = std::exp(-1.0/(inter->extra/100.0));
        inter->ratio = 1.0/(1.0-inter->offset);
    }
}
void MovingHandler::recalculateAllInterpolation()
{
    for (int i = 0; i < posInterpolate.size(); i++) {
        if (posInterpolate.at(i)->type == 2) {recalculateCurve(i);}
        else if (posInterpolate.at(i)->type == 3) {recalculateArc(i);}
    }

    for (int i = 0; i < colorInterpolate.size(); i++) {
        if (colorInterpolate.at(i)->type > 1) {recalculateColorInter(i);}
    }
}

void MovingHandler::createCanvas(PF_InData *in_data, PF_LayerDef *output)
{
    if (output != nullptr) {
        
        PF_FILL(NULL, NULL, output);

        if (visible) {

            PF_Pixel c_white = {0xff, 0xff, 0xff, 0xff};
            PF_Pixel c_grey = {0xff, 0x80, 0x80, 0x80};

            // -- left, top, right, bottom;
            if (type == FRONT_LEFT) {
                PF_Rect mhPos = {offsetXLeft, offsetYFront, offsetXLeft + sizeCube, offsetYFront + sizeCube};
                PF_Rect mhCol = {offsetXLeft, offsetYFront + sizeCube, offsetXLeft + sizeCube, offsetYFront + sizeCube + sizeYColor};
                PF_FILL(&c_white, &mhPos, output);
                PF_FILL(&c_grey, &mhCol, output);
            }
            else if (type == FRONT_RIGHT) {
                PF_Rect mhPos = {offsetXRight, offsetYFront, offsetXRight + sizeCube, offsetYFront + sizeCube};
                PF_Rect mhCol = {offsetXRight, offsetYFront + sizeCube, offsetXRight + sizeCube, offsetYFront + sizeCube + sizeYColor};
                PF_FILL(&c_white, &mhPos, output);
                PF_FILL(&c_grey, &mhCol, output);
            }
            else if (type == REAR_LEFT) {
                PF_Rect mhPos = {offsetXLeft, offsetYRear, offsetXLeft + sizeCube, offsetYRear + sizeCube};
                PF_Rect mhCol = {offsetXLeft, offsetYRear + sizeCube, offsetXLeft + sizeCube, offsetYRear + sizeCube + sizeYColor};
                PF_FILL(&c_white, &mhPos, output);
                PF_FILL(&c_grey, &mhCol, output);
            }
            else if (type == REAR_RIGHT) {
                PF_Rect mhPos = {offsetXRight, offsetYRear, offsetXRight + sizeCube, offsetYRear + sizeCube};
                PF_Rect mhCol = {offsetXRight, offsetYRear + sizeCube, offsetXRight + sizeCube, offsetYRear + sizeCube + sizeYColor};
                PF_FILL(&c_white, &mhPos, output);
                PF_FILL(&c_grey, &mhCol, output);
            }
        }   
    }
}
void MovingHandler::createPositionPoints(PF_InData *in_data, PF_LayerDef *output, int frame)
{
    // Get screen buffer address is used all over the place later
	PF_Pixel8 *pixels;
	PF_GET_PIXEL_DATA8(output, NULL, &pixels); // Get pointer to pixel data

    PF_Pixel c_black = {0xff, 0x00, 0x00, 0x00};
    PF_Pixel c_red = {0xff, 0xff, 0x00, 0x00};
    PF_Pixel c_blue = {0xff, 0x00, 0x00, 0xff};

    int offsetX = 0;
    int offsetY = 0;

    if (type == FRONT_LEFT) {
        offsetX = offsetXLeft;
        offsetY = offsetYFront;
    }
    else if (type == FRONT_RIGHT) {
        offsetX = offsetXRight;
        offsetY = offsetYFront;
    }
    else if (type == REAR_LEFT) {
        offsetX = offsetXLeft;
        offsetY = offsetYRear;
    }
    else if (type == REAR_RIGHT) {
        offsetX = offsetXRight;
        offsetY = offsetYRear;
    }

    // -- draw center of rotation
    int posCenterX = offsetX + (sizeCube*centerX);
    int posCenterY = offsetY + (sizeCube*centerY);
    int line_off = output->rowbytes / sizeof(PF_Pixel8);
    int idxCenter = posCenterY*line_off + posCenterX;
    pixels[idxCenter] = c_blue;
    pixels[idxCenter-line_off-1] = c_blue;
    pixels[idxCenter-line_off+1] = c_blue;
    pixels[idxCenter+line_off-1] = c_blue;
    pixels[idxCenter+line_off+1] = c_blue;

    for (int i = 0; i < pos.size(); i++) {

        // -- stop drawing points not already reach
        if ((pos.at(i)->frame*resizeTimeline)+translateTimeline > frame) {continue;}
        else if ((pos.at(i)->frame*resizeTimeline)+translateTimeline < 0) {continue;}

        Position* posTemp = new Position();
        posTemp->ratioX = pos.at(i)->ratioX;
        posTemp->ratioY = pos.at(i)->ratioY;

        // -- apply temporary transformation not applied
        if (scaleFactor != 1.0) {
            Position* posScale = scalePoint(posTemp);
            delete posTemp;
            posTemp = posScale;
        }
        if (rotationAngle != 0.0) {
            Position* posRotate = rotatePoint(posTemp);
            delete posTemp;
            posTemp = posRotate;
        }
        if (translateX != 0.0 || translateY != 0.0) {
            Position* posTranslate = translatePoint(posTemp);
            delete posTemp;
            posTemp = posTranslate;
        }

        if (posTemp->ratioX < 0) {posTemp->ratioX = 0;}
        else if (posTemp->ratioX > 1) {posTemp->ratioX = 1;}
        if (posTemp->ratioY < 0) {posTemp->ratioY = 0;}
        else if (posTemp->ratioY > 1) {posTemp->ratioY = 1;}

        int posX = offsetX + (sizeCube*posTemp->ratioX);
        int posY = offsetY + (sizeCube*posTemp->ratioY);

        delete posTemp;

        int idxBuffer = posY*line_off + posX;
        pixels[idxBuffer-line_off] = c_black;
        pixels[idxBuffer-1] = c_black;
        if ((pos.at(i)->frame*resizeTimeline)+translateTimeline == frame) {pixels[idxBuffer] = c_red;}
        else {pixels[idxBuffer] = c_black;}
        pixels[idxBuffer] = c_black;
        pixels[idxBuffer+1] = c_black;
        pixels[idxBuffer+line_off] = c_black;
    }
}
void MovingHandler::createPositionLine(PF_InData *in_data, PF_LayerDef *output, int frame)
{
    // Get screen buffer address is used all over the place later
	PF_Pixel8 *pixels;
	PF_GET_PIXEL_DATA8(output, NULL, &pixels); // Get pointer to pixel data

    PF_Pixel c_grey = {0xff, 0x80, 0x80, 0x80};
    PF_Pixel c_red = {0xff, 0xff, 0x00, 0x00};

    int offsetX = 0;
    int offsetY = 0;

    if (type == FRONT_LEFT) {
        offsetX = offsetXLeft;
        offsetY = offsetYFront;
    }
    else if (type == FRONT_RIGHT) {
        offsetX = offsetXRight;
        offsetY = offsetYFront;
    }
    else if (type == REAR_LEFT) {
        offsetX = offsetXLeft;
        offsetY = offsetYRear;
    }
    else if (type == REAR_RIGHT) {
        offsetX = offsetXRight;
        offsetY = offsetYRear;
    }

    // -- we draw every elements
    for (int i = 0; i <= frame; i++) {

        // -- we calculate corresponding frame
        Position* posTemp = getPositionAtTime((i-translateTimeline)/resizeTimeline);

        // -- actual draw
        if (posTemp != nullptr) {

            if (posTemp->ratioX < 0) {posTemp->ratioX = 0;}
            else if (posTemp->ratioX > 1) {posTemp->ratioX = 1;}
            if (posTemp->ratioY < 0) {posTemp->ratioY = 0;}
            else if (posTemp->ratioY > 1) {posTemp->ratioY = 1;}

            int posX = offsetX + (sizeCube*posTemp->ratioX);
            int posY = offsetY + (sizeCube*posTemp->ratioY);

            int line_off = output->rowbytes / sizeof(PF_Pixel8);
            int idxBuffer = posY*line_off + posX;
            

           if ((i-translateTimeline)/resizeTimeline == frame) {
                pixels[idxBuffer] = c_red;
            }
            else {
                pixels[idxBuffer] = c_grey;
            }

            delete posTemp;
        }
    }
}
void MovingHandler::createColorsRect(PF_InData *in_data, PF_LayerDef *output, int frame)
{
    // Get screen buffer address is used all over the place later
	PF_Pixel8 *pixels;
	PF_GET_PIXEL_DATA8(output, NULL, &pixels); // Get pointer to pixel data

    PF_Pixel c_white = {0xff, 0xff, 0xff, 0xff};
    PF_Pixel c_black = {0xff, 0x00, 0x00, 0x00};

    int offsetX = 0;
    int offsetY = 0;
    int sizeY = 16;

    if (type == FRONT_LEFT) {
        offsetX = offsetXLeft;
        offsetY = offsetYFront + sizeCube + 2;
    }
    else if (type == FRONT_RIGHT) {
        offsetX = offsetXRight;
        offsetY = offsetYFront + sizeCube + 2;
    }
    else if (type == REAR_LEFT) {
        offsetX = offsetXLeft;
        offsetY = offsetYRear + sizeCube + 2;
    }
    else if (type == REAR_RIGHT) {
        offsetX = offsetXRight;
        offsetY = offsetYRear + sizeCube + 2;
    }

    for (int i = 0; i < nbFrames; i++) {

        // -- we calculate corresponding frame
        Color* colorTemp = getColorAtTime((i-translateTimeline)/resizeTimeline);

        // -- actual draw
        if (colorTemp != nullptr) {
            int posX = offsetX + (i*sizeCube/nbFrames);
            PF_Pixel color_pixel = {colorTemp->alpha, colorTemp->red, colorTemp->green, colorTemp->blue};
            PF_Rect mhCol = {posX, offsetY, posX+1, offsetY+sizeY};
            PF_FILL(&color_pixel, &mhCol, output);

            delete colorTemp;
        }
    }

    // -- draw color created
    int line_off = output->rowbytes / sizeof(PF_Pixel8);
    for (int i = 0; i < color.size(); i++) {
        int posXColor = offsetX + ((color.at(i)->frame*resizeTimeline)+translateTimeline)*sizeCube/nbFrames;
        int idxBuffer = (offsetY+sizeY)*line_off + posXColor;
        pixels[idxBuffer] = c_white;
        pixels[idxBuffer+line_off] = c_white;
        pixels[idxBuffer+line_off-1] = c_white;
        pixels[idxBuffer+line_off+1] = c_white;
        pixels[idxBuffer+line_off*2] = c_white;
        pixels[idxBuffer+line_off*2-2] = c_white;
        pixels[idxBuffer+line_off*2-1] = c_white;
        pixels[idxBuffer+line_off*2+1] = c_white;
        pixels[idxBuffer+line_off*2+2] = c_white;
        pixels[idxBuffer-line_off] = c_white;
    }

    // -- show current cursor
    int posXCurrent = offsetX + frame*sizeCube/nbFrames;
    int idxBuffer = offsetY*line_off + posXCurrent;
    
    // -- draw array
    pixels[idxBuffer] = c_white;
    pixels[idxBuffer-line_off] = c_white;
    pixels[idxBuffer-line_off-1] = c_white;
    pixels[idxBuffer-line_off+1] = c_white;
    pixels[idxBuffer-line_off*2] = c_white;
    pixels[idxBuffer-line_off*2-2] = c_white;
    pixels[idxBuffer-line_off*2-1] = c_white;
    pixels[idxBuffer-line_off*2+1] = c_white;
    pixels[idxBuffer-line_off*2+2] = c_white;
    pixels[idxBuffer+line_off] = c_white;
}
void MovingHandler::createFinalRender(PF_InData *in_data, PF_LayerDef *output, int frame)
{
    // Get screen buffer address is used all over the place later
	PF_Pixel8 *pixels;
	PF_GET_PIXEL_DATA8(output, NULL, &pixels); // Get pointer to pixel data

    int offsetXPan = 0;
    int offsetYPan = 0;
    int offsetXTilt = 0;
    int offsetYTilt = 0;
    int offsetXColor = 0;
    int offsetYColor = 0;

    if (type == FRONT_LEFT) {
        offsetXPan = 33;
        offsetYPan = 0;
        offsetXTilt = 34;
        offsetYTilt = 0;
        offsetXColor = 0;
        offsetYColor = 67;
    }
    else if (type == FRONT_RIGHT) {
        offsetXPan = 36;
        offsetYPan = 0;
        offsetXTilt = 37;
        offsetYTilt = 0;
        offsetXColor = 68;
        offsetYColor = 67;
    }
    else if (type == REAR_LEFT) {
        offsetXPan = 39;
        offsetYPan = 0;
        offsetXTilt = 40;
        offsetYTilt = 0;
        offsetXColor = 0;
        offsetYColor = 135;
    }
    else if (type == REAR_RIGHT) {
        offsetXPan = 42;
        offsetYPan = 0;
        offsetXTilt = 43;
        offsetYTilt = 0;
        offsetXColor = 68;
        offsetYColor = 135;
    }

    int line_off = output->rowbytes / sizeof(PF_Pixel8);
    Position* posTemp = getPositionAtTime((frame-translateTimeline)/resizeTimeline);
    Color* colorTemp = getColorAtTime((frame-translateTimeline)/resizeTimeline);

    // -- set color
    if (colorTemp != nullptr) {
        PF_Pixel colorPixel;
        colorPixel.alpha = 0xff;
        colorPixel.red = colorTemp->red;
        colorPixel.green = colorTemp->green;
        colorPixel.blue = colorTemp->blue;
        colorPixel.alpha = colorTemp->alpha;
        int idxBuffer = offsetYColor*line_off + offsetXColor;
        pixels[idxBuffer] = colorPixel;

        delete colorTemp;
    }

    // -- set pan/tilt
    if (posTemp != nullptr) {

        double dX = (0.5 - posTemp->ratioX);
        double dY = (0.5 - posTemp->ratioY);
        double len = PF_SQRT(dX*dX + dY*dY);

        // get the pan angle
        double pan;
        if (len > 0.001) { // Avoid division by zero.
            pan = PF_ASIN(dX/len) / PF_RAD_PER_DEGREE;
        }
        else {
            // The head points (almost) straight to the floor: any pan should do.
            pan = -180.0; // 0.0; The old plugin uses -180.0 degrees but 0.0 should do too.
        }
        // Correct angle for the quadrant
        if ( (posTemp->ratioX <= 0.5) && (posTemp->ratioY >  0.5) ) { // left-bottom
            pan = 180.0 - pan;
        }
        if ((posTemp->ratioX > 0.5) && (posTemp->ratioY >  0.5)) { // right-bottom
            pan = -180.0 - pan;
        }

        // Tilt should be between:
        // 0 (points to 0.5, 0.5) (floor center)
        // +45 (any centerpoint at the wall)
        // The tilt is never negative!

        // Length vector from ceiling to the floor (pythagoras)
        double len_z = PF_SQRT((len * len) + (0.5*0.5));
        // Get the tilt vector
        double tilt = PF_ASIN(len/len_z) / PF_RAD_PER_DEGREE;

        // -- format data
        PF_Pixel panPixel;
        pan += 180.0;
        int panInt = (pan*65535.0/360.0) + 0.5;
        panPixel.alpha = 0xff;
        panPixel.red = 0x00;
        panPixel.green = (panInt >> 8) & 0xff;
        panPixel.blue = panInt & 0xff;
        int idxBuffer = offsetYPan*line_off + offsetXPan;
        pixels[idxBuffer] = panPixel;

        PF_Pixel tiltPixel;
        tilt += 90.0;
        int tiltInt = (tilt*65535.0/180.0) + 0.5;
        tiltPixel.alpha = 0xff;
        tiltPixel.red = 0x00;
        tiltPixel.green = (tiltInt >> 8) & 0xff;
        tiltPixel.blue = tiltInt & 0xff;
        idxBuffer = offsetYTilt*line_off + offsetXTilt;
        pixels[idxBuffer] = tiltPixel;

        delete posTemp;
    }
}

void MovingHandler::flipH()
{
    if (pos.size() == 0) {
        return;
    }

    for (int i = 0; i < pos.size(); i++) {
        pos.at(i)->ratioY = 1 - pos.at(i)->ratioY;
        posInterpolate.at(i)->extra *= -1;
    }
    recalculateAllInterpolation();
    needSave = true;
}
void MovingHandler::flipV()
{
    if (pos.size() == 0) {
        return;
    }

    for (int i = 0; i < pos.size(); i++) {
        pos.at(i)->ratioX = 1 - pos.at(i)->ratioX;
        posInterpolate.at(i)->extra *= -1;
    }
    recalculateAllInterpolation();
    needSave = true;
}

void MovingHandler::loop()
{
    // -- nothing to do
    if (pos.size() < 2 && color.size() < 2) {
        return;
    }

    // -- calculate size of elements to be copied
    // -- max and min of pos
    int min = 0;
    
    int max = MAX(pos.at(pos.size()-1)->frame, color.at(color.size()-1)->frame);

    // -- check loop only if not in reversed
    if (!modeLoop) {

        // -- check positions
        if (pos.at(0)->frame == min && pos.at(pos.size()-1)->frame == max) {
            if (pos.at(0)->ratioX != pos.at(pos.size()-1)->ratioX || 
                pos.at(0)->ratioY != pos.at(pos.size()-1)->ratioY) {
                return;
            }
        }

        // -- check color
        if (color.at(0)->frame == min && color.at(color.size()-1)->frame == max) {
            if (color.at(0)->red != color.at(color.size()-1)->red || 
                color.at(0)->green != color.at(color.size()-1)->green ||
                color.at(0)->blue != color.at(color.size()-1)->blue ||
                color.at(0)->alpha != color.at(color.size()-1)->alpha) {
                return;
            }
        }
    }

    // -- copy structure
    std::vector<Position *> newPos;
    std::vector<Color *> newColor;
    for (int i = 0; i < pos.size(); i++) {newPos.push_back(pos.at(i));}
    for (int i = 0; i < color.size(); i++) {newColor.push_back(color.at(i));}

    // -- calculate offset and nb loop
    int nbLoop = (nbFrames-max) / (max-min);
    int offsetLoop = max;

    // -- place at the last element of interpolation
    std::vector<Interpolation *>::iterator interIt = posInterpolate.end();
    interIt--;
    std::vector<Interpolation *>::iterator interItColor = colorInterpolate.end();
    interItColor--;

    bool loopIsReversed = true;
    for (int i = 0; i < nbLoop; i++) {
        if (modeLoop) {

            bool addStart = color.at(0)->frame != min;
            bool addEnd = color.at(color.size()-1)->frame != max;

            if (loopIsReversed) {

                // -- pos adding
                for (int i = pos.size()-2; i >= 0; i--) {
                    Position* posTemp = new Position();
                    posTemp->ratioX = pos.at(i)->ratioX;
                    posTemp->ratioY = pos.at(i)->ratioY;
                    posTemp->frame = offsetLoop + (max - pos.at(i)->frame-min);
                    newPos.push_back(posTemp);

                    Interpolation* newInter = new Interpolation();
                    newInter->type = posInterpolate.at(i)->type;
                    newInter->extra = -posInterpolate.at(i)->extra;
                    newInter->extra2 = -posInterpolate.at(i)->extra2;
                    interIt = posInterpolate.insert(interIt, newInter);
                    interIt++;
                }

                int startIndex = color.size()-2;

                if (addEnd && !addStart) {
                    Color* tempColor = new Color();
                    tempColor->red = 0;
                    tempColor->green = 0;
                    tempColor->blue = 0;
                    tempColor->alpha = 255;
                    tempColor->frame = offsetLoop - (max - color.at(color.size()-1)->frame) + 1;
                    newColor.push_back(tempColor);

                    Interpolation* newInter = new Interpolation();
                    newInter->type = 0;
                    interItColor = colorInterpolate.insert(interItColor, newInter);
                    interItColor++;

                    Color* tempColor2 = new Color();
                    tempColor2->red = 0;
                    tempColor2->green = 0;
                    tempColor2->blue = 0;
                    tempColor2->alpha = 255;
                    tempColor2->frame = offsetLoop + (max - color.at(color.size()-1)->frame) - 1;
                    newColor.push_back(tempColor2);

                    Interpolation* newInter2 = new Interpolation();
                    newInter2->type = 0;
                    interItColor = colorInterpolate.insert(interItColor, newInter2);
                    interItColor++;

                    startIndex = color.size()-1;
                }

                // -- color adding
                for (int i = startIndex; i >= 0 ; i--) {
                    Color* colorTemp = new Color();
                    colorTemp->red = color.at(i)->red;
                    colorTemp->green = color.at(i)->green;
                    colorTemp->blue = color.at(i)->blue;
                    colorTemp->alpha = color.at(i)->alpha;
                    colorTemp->frame = offsetLoop + (max - color.at(i)->frame-min);
                    newColor.push_back(colorTemp);

                    Interpolation* newInter = new Interpolation();
                    newInter->type = colorInterpolate.at(i)->type;
                    newInter->extra = colorInterpolate.at(i)->extra;
                    interItColor = colorInterpolate.insert(interItColor, newInter);
                    interItColor++;
                }

                if (addEnd && addStart) {
                    Color* tempColor = new Color();
                    tempColor->red = 0;
                    tempColor->green = 0;
                    tempColor->blue = 0;
                    tempColor->alpha = 255;
                    tempColor->frame = offsetLoop - (color.at(0)->frame) + 1;
                    newColor.push_back(tempColor);

                    Interpolation* newInter = new Interpolation();
                    newInter->type = 0;
                    interItColor = colorInterpolate.insert(interItColor, newInter);
                    interItColor++;

                    Color* tempColor2 = new Color();
                    tempColor2->red = 0;
                    tempColor2->green = 0;
                    tempColor2->blue = 0;
                    tempColor2->alpha = 255;
                    tempColor2->frame = offsetLoop + (color.at(0)->frame) - 1;
                    newColor.push_back(tempColor2);

                    Interpolation* newInter2 = new Interpolation();
                    newInter2->type = 0;
                    interItColor = colorInterpolate.insert(interItColor, newInter2);
                    interItColor++;
                }

                offsetLoop += (max-min);
                loopIsReversed = false;
            }
            else {

                // -- position adding
                for (int i = 1; i < pos.size(); i++) {
                    Position* posTemp = new Position();
                    posTemp->ratioX = pos.at(i)->ratioX;
                    posTemp->ratioY = pos.at(i)->ratioY;
                    posTemp->frame = offsetLoop + (pos.at(i)->frame);
                    newPos.push_back(posTemp);

                    Interpolation* newInter = new Interpolation();
                    newInter->type = posInterpolate.at(i-1)->type;
                    newInter->extra = posInterpolate.at(i-1)->extra;
                    newInter->extra2 = posInterpolate.at(i-1)->extra2;
                    interIt = posInterpolate.insert(interIt, newInter);
                    interIt++;
                }

                int startIndex = 1;

                if (!addEnd && addStart) {
                    Color* tempColor = new Color();
                    tempColor->red = 0;
                    tempColor->green = 0;
                    tempColor->blue = 0;
                    tempColor->alpha = 255;
                    tempColor->frame = offsetLoop - (color.at(0)->frame) + 1;
                    newColor.push_back(tempColor);

                    Interpolation* newInter = new Interpolation();
                    newInter->type = 0;
                    interItColor = colorInterpolate.insert(interItColor, newInter);
                    interItColor++;

                    Color* tempColor2 = new Color();
                    tempColor2->red = 0;
                    tempColor2->green = 0;
                    tempColor2->blue = 0;
                    tempColor2->alpha = 255;
                    tempColor2->frame = offsetLoop + (color.at(0)->frame) - 1;
                    newColor.push_back(tempColor2);

                    Interpolation* newInter2 = new Interpolation();
                    newInter2->type = 0;
                    interItColor = colorInterpolate.insert(interItColor, newInter2);
                    interItColor++;

                    startIndex = 0;
                }

                // -- color adding
                for (int i = startIndex; i < color.size(); i++) {
                    Color* colorTemp = new Color();
                    colorTemp->red = color.at(i)->red;
                    colorTemp->green = color.at(i)->green;
                    colorTemp->blue = color.at(i)->blue;
                    colorTemp->alpha = color.at(i)->alpha;
                    colorTemp->frame = offsetLoop + (color.at(i)->frame - min);
                    newColor.push_back(colorTemp);

                    Interpolation* newInter = new Interpolation();
                    if (i > 0) {
                        newInter->type = colorInterpolate.at(i-1)->type;
                        newInter->extra = colorInterpolate.at(i-1)->extra;
                    }
                    else {
                        newInter->type = 1;
                    }
                    
                    interItColor = colorInterpolate.insert(interItColor, newInter);
                    interItColor++;
                }

                if (addEnd && addStart) {
                    Color* tempColor = new Color();
                    tempColor->red = 0;
                    tempColor->green = 0;
                    tempColor->blue = 0;
                    tempColor->alpha = 255;
                    tempColor->frame = offsetLoop - (max - color.at(color.size()-1)->frame) + 1;
                    newColor.push_back(tempColor);

                    Interpolation* newInter = new Interpolation();
                    newInter->type = 0;
                    interItColor = colorInterpolate.insert(interItColor, newInter);
                    interItColor++;

                    Color* tempColor2 = new Color();
                    tempColor2->red = 0;
                    tempColor2->green = 0;
                    tempColor2->blue = 0;
                    tempColor2->alpha = 255;
                    tempColor2->frame = offsetLoop + (max - color.at(color.size()-1)->frame) - 1;
                    newColor.push_back(tempColor2);

                    Interpolation* newInter2 = new Interpolation();
                    newInter2->type = 0;
                    interItColor = colorInterpolate.insert(interItColor, newInter2);
                    interItColor++;
                }

                offsetLoop += (max-min);
                loopIsReversed = true;
            }
        }
        else {

            // -- position adding
            for (int i = 1; i < pos.size(); i++) {
                Position* posTemp = new Position();
                posTemp->ratioX = pos.at(i)->ratioX;
                posTemp->ratioY = pos.at(i)->ratioY;
                posTemp->frame = offsetLoop + (pos.at(i)->frame);
                newPos.push_back(posTemp);

                Interpolation* newInter = new Interpolation();
                newInter->type = posInterpolate.at(i-1)->type;
                newInter->extra = posInterpolate.at(i-1)->extra;
                newInter->extra2 = posInterpolate.at(i-1)->extra2;
                interIt = posInterpolate.insert(interIt, newInter);
                interIt++;
            }

            bool needAdd = false;
            long addLow = 0;
            long addHigh = 0;

            // -- add black color to compensate start/end
            if (color.at(color.size()-1)->frame != max || color.at(0)->frame != min) {
                needAdd = true;

                if (color.at(color.size()-1)->frame != max) {
                    addLow = offsetLoop - (max - color.at(color.size()-1)->frame) + 1;
                }
                else {
                    addLow = offsetLoop + 1;
                }

                if (color.at(0)->frame != min) {
                    addHigh = offsetLoop + color.at(0)->frame - 1;
                }
                else {
                    addHigh = offsetLoop - 1;
                }

                Color* tempColor = new Color();
                tempColor->red = 0;
                tempColor->green = 0;
                tempColor->blue = 0;
                tempColor->alpha = 255;
                tempColor->frame = addLow;
                newColor.push_back(tempColor);

                Interpolation* newInter = new Interpolation();
                newInter->type = 0;
                interItColor = colorInterpolate.insert(interItColor, newInter);
                interItColor++;

                Color* tempColor2 = new Color();
                tempColor2->red = 0;
                tempColor2->green = 0;
                tempColor2->blue = 0;
                tempColor2->alpha = 255;
                tempColor2->frame = addHigh;
                newColor.push_back(tempColor2);

                Interpolation* newInter2 = new Interpolation();
                newInter2->type = 0;
                interItColor = colorInterpolate.insert(interItColor, newInter2);
                interItColor++;
            }

            int startIndex = (needAdd) ? 0 : 1;

            // -- color adding
            for (int i = startIndex; i < color.size(); i++) {
                Color* colorTemp = new Color();
                colorTemp->red = color.at(i)->red;
                colorTemp->green = color.at(i)->green;
                colorTemp->blue = color.at(i)->blue;
                colorTemp->alpha = color.at(i)->alpha;
                colorTemp->frame = offsetLoop + (color.at(i)->frame - min);
                newColor.push_back(colorTemp);

                Interpolation* newInter = new Interpolation();
				if (i > 0) {
					newInter->type = colorInterpolate.at(i-1)->type;
					newInter->extra = colorInterpolate.at(i-1)->extra;
				}
				else {
					newInter->type = 1;
				}
                
                interItColor = colorInterpolate.insert(interItColor, newInter);
                interItColor++;
            }

            offsetLoop += (max-min);
        }
    }

    pos = newPos;
    color = newColor;

    needSave = true;
}

void MovingHandler::createParams(PF_InData *in_data, std::string presetsPosStr, std::string presetsColorStr, int nbPos, int nbColor)
{
    // -- list of moving types
    PF_ParamDef	listOfTypeMovings;
	AEFX_CLR_STRUCT(listOfTypeMovings);
    listOfTypeMovings.param_type = PF_Param_POPUP;
    PF_STRCPY(listOfTypeMovings.name, "MovingHead type :");
    listOfTypeMovings.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    listOfTypeMovings.u.pd.value = 1;
    listOfTypeMovings.u.pd.dephault = 1;
    listOfTypeMovings.u.pd.num_choices = 4;
    A_char *popup = (A_char*)("FRONT_LEFT | FRONT_RIGHT | REAR_LEFT | REAR_RIGHT");
    listOfTypeMovings.u.pd.u.namesptr = popup;
    PF_ADD_PARAM(in_data, ID_CHANGETYPE, &listOfTypeMovings);

    PF_ParamDef	updateParams;
	AEFX_CLR_STRUCT(updateParams);
    updateParams.param_type = PF_Param_BUTTON;
    updateParams.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    A_char *updateStr = (A_char*)("Update");
    updateParams.u.button_d.u.namesptr = updateStr;
    PF_ADD_PARAM(in_data, ID_UPDATE, &updateParams);

    // -- visible UI
    PF_ParamDef	visibleUI;
    AEFX_CLR_STRUCT(visibleUI);
    visibleUI.param_type = PF_Param_CHECKBOX;
    visibleUI.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    visibleUI.u.bd.dephault = 1;
    visibleUI.u.bd.value = 1;
    A_char *visibleStr = (A_char*)("Visible");
    visibleUI.u.bd.u.nameptr = visibleStr;
    PF_ADD_PARAM(in_data, ID_VISIBLE, &visibleUI);

    createPositionGroup(in_data);
    createColorGroup(in_data);
    createTimelineGroup(in_data);
    createTransformGroup(in_data);
    createPresetsGroup(in_data, presetsPosStr, presetsColorStr, nbPos, nbColor);

    PF_CustomUIInfo customInfo;
    customInfo.events = PF_CustomEFlag_COMP | PF_CustomEFlag_LAYER | PF_CustomEFlag_PREVIEW | PF_CustomEFlag_EFFECT;
    customInfo.comp_ui_width = customInfo.comp_ui_height = 0;
    customInfo.comp_ui_alignment = PF_UIAlignment_NONE;
    PF_REGISTER_UI(in_data, &customInfo);
}

void MovingHandler::createPositionGroup(PF_InData *in_data)
{
    // -- group of positions
    PF_ParamDef	group;
    AEFX_CLR_STRUCT(group);
    group.param_type = PF_Param_GROUP_START;
    PF_STRCPY(group.name, "Position");
    PF_ADD_PARAM(in_data, ID_GROUP_POS, &group);

    // -- activate position
    PF_ParamDef	activePos;
    AEFX_CLR_STRUCT(activePos);
    activePos.param_type = PF_Param_CHECKBOX;
    activePos.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    activePos.u.bd.dephault = 0;
    activePos.u.bd.value = 0;
    A_char *activePosStr = (A_char*)("Active");
    activePos.u.bd.u.nameptr = activePosStr;
    PF_ADD_PARAM(in_data, ID_ACTIVEPOS, &activePos);

    // -- set position
    PF_ParamDef	ratioXY;
    AEFX_CLR_STRUCT(ratioXY);
    ratioXY.param_type = PF_Param_POINT;
    ratioXY.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    ratioXY.ui_flags = PF_PUI_DISABLED;
    PF_STRCPY(ratioXY.name, "Ratio X/Y (0 - 100):");
    ratioXY.u.td.x_dephault = 0;
    ratioXY.u.td.y_dephault = 0;
    ratioXY.u.td.x_value = 0;
    ratioXY.u.td.y_value = 0;
    ratioXY.u.td.restrict_bounds = 0;
    PF_ADD_PARAM(in_data, ID_POSITION, &ratioXY);

    PF_ParamDef	listOfTypeInterPos;
	AEFX_CLR_STRUCT(listOfTypeInterPos);
    listOfTypeInterPos.param_type = PF_Param_POPUP;
    PF_STRCPY(listOfTypeInterPos.name, "Type interpolation :");
    listOfTypeInterPos.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    listOfTypeInterPos.u.pd.value = 0;
    listOfTypeInterPos.u.pd.dephault = 0;
    listOfTypeInterPos.u.pd.num_choices = 4;
    A_char *typeInterStr = (A_char*)("LINEAR | CURVE | ARC");
    listOfTypeInterPos.u.pd.u.namesptr = typeInterStr;
    PF_ADD_PARAM(in_data, ID_INTERPOS_TYPE, &listOfTypeInterPos);

    PF_ParamDef	interPosExtra;
	AEFX_CLR_STRUCT(interPosExtra);
    interPosExtra.param_type = PF_Param_FLOAT_SLIDER;
    PF_STRCPY(interPosExtra.name, "Extra :");
    interPosExtra.ui_flags = PF_PUI_DISABLED;
    interPosExtra.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    interPosExtra.u.fs_d.dephault = 0;
    interPosExtra.u.fs_d.value = 0;
    interPosExtra.u.fs_d.valid_min = -50.0;
    interPosExtra.u.fs_d.slider_min = -50.0;
    interPosExtra.u.fs_d.valid_max = 50.0;
    interPosExtra.u.fs_d.slider_max = 50.0;
    interPosExtra.u.fs_d.precision = 2;
    PF_ADD_PARAM(in_data, ID_INTERPOS_EXTRA, &interPosExtra);

    PF_ParamDef	interPosExtra2;
	AEFX_CLR_STRUCT(interPosExtra2);
    interPosExtra2.param_type = PF_Param_FLOAT_SLIDER;
    PF_STRCPY(interPosExtra2.name, "Extra 2 :");
    interPosExtra2.ui_flags = PF_PUI_DISABLED;
    interPosExtra2.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    interPosExtra.u.fs_d.dephault = 0;
    interPosExtra2.u.fs_d.value = 0;
    interPosExtra2.u.fs_d.valid_min = -100.0;
    interPosExtra2.u.fs_d.slider_min = -100.0;
    interPosExtra2.u.fs_d.valid_max = 100.0;
    interPosExtra2.u.fs_d.slider_max = 100.0;
    interPosExtra2.u.fs_d.precision = 2;
    PF_ADD_PARAM(in_data, ID_INTERPOS_EXTRA2, &interPosExtra2);

    AEFX_CLR_STRUCT(group);
    group.param_type = PF_Param_GROUP_END;
    PF_ADD_PARAM(in_data, ID_END_GROUP_POS, &group);
}
void MovingHandler::createColorGroup(PF_InData *in_data) 
{
    // -- group of colors
    PF_ParamDef	group;
    AEFX_CLR_STRUCT(group);
    group.param_type = PF_Param_GROUP_START;
    PF_STRCPY(group.name, "Color");
    PF_ADD_PARAM(in_data, ID_GROUP_COLOR, &group);

    // -- activate color
    PF_ParamDef	activeColor;
    AEFX_CLR_STRUCT(activeColor);
    activeColor.param_type = PF_Param_CHECKBOX;
    activeColor.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    activeColor.u.bd.dephault = 0;
    activeColor.u.bd.value = 0;
    A_char *activeColorStr = (A_char*)("Active");
    activeColor.u.bd.u.nameptr = activeColorStr;
    PF_ADD_PARAM(in_data, ID_ACTIVECOLOR, &activeColor);

    // -- set color
    PF_ParamDef	colorDef;
    AEFX_CLR_STRUCT(colorDef);
    colorDef.param_type = PF_Param_COLOR;
    colorDef.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    colorDef.ui_flags = PF_PUI_DISABLED;
    PF_STRCPY(colorDef.name, "Color value :");
    colorDef.u.cd.dephault.red = 0x00;
    colorDef.u.cd.dephault.green = 0x00;
    colorDef.u.cd.dephault.blue = 0x00;
    colorDef.u.cd.value.red = 0x00;
    colorDef.u.cd.value.green = 0x00;
    colorDef.u.cd.value.blue = 0x00;
    PF_ADD_PARAM(in_data, ID_COLOR, &colorDef);

    // -- set color intensity
    PF_ParamDef	intensityDef;
    AEFX_CLR_STRUCT(intensityDef);
    intensityDef.param_type = PF_Param_FLOAT_SLIDER;
    intensityDef.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    intensityDef.ui_flags = PF_PUI_DISABLED;
    PF_STRCPY(intensityDef.name, "Intensity value :");
    intensityDef.u.fs_d.dephault = 255.0f;
    intensityDef.u.fs_d.slider_min = 0.0f;
    intensityDef.u.fs_d.valid_min = 0.0f;
    intensityDef.u.fs_d.slider_max = 255.0f;
    intensityDef.u.fs_d.valid_max = 255.0f;
    intensityDef.u.fs_d.precision = 0;
    PF_ADD_PARAM(in_data, ID_COLOR_INTENSITY, &intensityDef);

    PF_ParamDef	listOfTypeInterColor;
	AEFX_CLR_STRUCT(listOfTypeInterColor);
    listOfTypeInterColor.param_type = PF_Param_POPUP;
    PF_STRCPY(listOfTypeInterColor.name, "Type interpolation :");
    listOfTypeInterColor.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    listOfTypeInterColor.u.pd.value = 0;
    listOfTypeInterColor.u.pd.dephault = 0;
    listOfTypeInterColor.u.pd.num_choices = 4;
    A_char *typeInterColorStr = (A_char*)("LINEAR | FAST | SLOW");
    listOfTypeInterColor.u.pd.u.namesptr = typeInterColorStr;
    PF_ADD_PARAM(in_data, ID_INTERCOLOR_TYPE, &listOfTypeInterColor);

    PF_ParamDef	interColorExtra;
	AEFX_CLR_STRUCT(interColorExtra);
    interColorExtra.param_type = PF_Param_FLOAT_SLIDER;
    PF_STRCPY(interColorExtra.name, "Extra :");
    interColorExtra.ui_flags = PF_PUI_DISABLED;
    interColorExtra.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    interColorExtra.u.fs_d.dephault = 50.0;
    interColorExtra.u.fs_d.value = 50.0;
    interColorExtra.u.fs_d.valid_min = 0.0;
    interColorExtra.u.fs_d.slider_min = 0.0;
    interColorExtra.u.fs_d.valid_max = 100.0;
    interColorExtra.u.fs_d.slider_max = 100.0;
    interColorExtra.u.fs_d.precision = 2;
    PF_ADD_PARAM(in_data, ID_INTERCOLOR_EXTRA, &interColorExtra);

    AEFX_CLR_STRUCT(group);
    group.param_type = PF_Param_GROUP_END;
    PF_ADD_PARAM(in_data, ID_END_GROUP_COLOR, &group);
}
void MovingHandler::createTimelineGroup(PF_InData *in_data)
{
    // -- group timeline
    PF_ParamDef	group;
    AEFX_CLR_STRUCT(group);
    group.param_type = PF_Param_GROUP_START;
    PF_STRCPY(group.name, "Timeline");
    PF_ADD_PARAM(in_data, ID_GROUP_TIMELINE, &group);

    PF_ParamDef	resizeTimeline;
	AEFX_CLR_STRUCT(resizeTimeline);
    resizeTimeline.param_type = PF_Param_FLOAT_SLIDER;
    PF_STRCPY(resizeTimeline.name, "Resize factor :");
    resizeTimeline.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    resizeTimeline.u.fs_d.dephault = 1.0;
    resizeTimeline.u.fs_d.value = 1.0;
    resizeTimeline.u.fs_d.valid_min = 0.001;
    resizeTimeline.u.fs_d.slider_min = 0.001;
    resizeTimeline.u.fs_d.valid_max = 10.0;
    resizeTimeline.u.fs_d.slider_max = 10.0;
    resizeTimeline.u.fs_d.precision = 3;
    PF_ADD_PARAM(in_data, ID_RESIZE_TIMELINE, &resizeTimeline);

    PF_ParamDef	applyResizeParams;
	AEFX_CLR_STRUCT(applyResizeParams);
    applyResizeParams.param_type = PF_Param_BUTTON;
    applyResizeParams.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    A_char *applyResizeStr = (A_char*)("Apply resize");
    applyResizeParams.u.button_d.u.namesptr = applyResizeStr;
    PF_ADD_PARAM(in_data, ID_APPLY_RESIZE_TIMELINE, &applyResizeParams);

    PF_ParamDef	translateTimelineParam;
	AEFX_CLR_STRUCT(translateTimelineParam);
    translateTimelineParam.param_type = PF_Param_FLOAT_SLIDER;
    PF_STRCPY(translateTimelineParam.name, "Translate timeline :");
    translateTimelineParam.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    translateTimelineParam.u.fs_d.dephault = 0;
    translateTimelineParam.u.fs_d.value = 0;
    translateTimelineParam.u.fs_d.valid_min = -100.0;
    translateTimelineParam.u.fs_d.slider_min = -100.0;
    translateTimelineParam.u.fs_d.valid_max = 100.0;
    translateTimelineParam.u.fs_d.slider_max = 100.0;
    translateTimelineParam.u.fs_d.precision = 0;
    PF_ADD_PARAM(in_data, ID_TRANSLATE_TIMELINE, &translateTimelineParam);

    PF_ParamDef	applyTranslateTimelineParams;
	AEFX_CLR_STRUCT(applyTranslateTimelineParams);
    applyTranslateTimelineParams.param_type = PF_Param_BUTTON;
    applyTranslateTimelineParams.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    A_char *applyTranslateTimelineStr = (A_char*)("Apply timeline translation");
    applyTranslateTimelineParams.u.button_d.u.namesptr = applyTranslateTimelineStr;
    PF_ADD_PARAM(in_data, ID_APPLY_TRANSLATE_TIMELINE, &applyTranslateTimelineParams);

    PF_ParamDef	reverseTimelineParams;
	AEFX_CLR_STRUCT(reverseTimelineParams);
    reverseTimelineParams.param_type = PF_Param_BUTTON;
    reverseTimelineParams.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    A_char *reverseTimelineStr = (A_char*)("Reverse timeline");
    reverseTimelineParams.u.button_d.u.namesptr = reverseTimelineStr;
    PF_ADD_PARAM(in_data, ID_REVERSE_TIMELINE, &reverseTimelineParams);

    createLoopGroup(in_data);

    AEFX_CLR_STRUCT(group);
    group.param_type = PF_Param_GROUP_END;
    PF_ADD_PARAM(in_data, ID_END_GROUP_TIMELINE, &group);
}
void MovingHandler::createTransformGroup(PF_InData *in_data)
{
    // -- group Transform
    PF_ParamDef	group;
    AEFX_CLR_STRUCT(group);
    group.param_type = PF_Param_GROUP_START;
    PF_STRCPY(group.name, "Transform");
    PF_ADD_PARAM(in_data, ID_GROUP_TRANSFORM, &group);

    PF_ParamDef	reverseHParams;
	AEFX_CLR_STRUCT(reverseHParams);
    reverseHParams.param_type = PF_Param_BUTTON;
    reverseHParams.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    A_char *reverseHStr = (A_char*)("Flip position horizontally");
    reverseHParams.u.button_d.u.namesptr = reverseHStr;
    PF_ADD_PARAM(in_data, ID_FLIP_POS_H, &reverseHParams);

    PF_ParamDef	reverseVParams;
	AEFX_CLR_STRUCT(reverseVParams);
    reverseVParams.param_type = PF_Param_BUTTON;
    reverseVParams.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    A_char *reverseVStr = (A_char*)("Flip position vertically");
    reverseVParams.u.button_d.u.namesptr = reverseVStr;
    PF_ADD_PARAM(in_data, ID_FLIP_POS_V, &reverseVParams);

    createScaleGroup(in_data);
    createTranslateGroup(in_data);
    createRotateGroup(in_data);

    AEFX_CLR_STRUCT(group);
    group.param_type = PF_Param_GROUP_END;
    PF_ADD_PARAM(in_data, ID_END_GROUP_TRANSFORM, &group);
}
void MovingHandler::createScaleGroup(PF_InData *in_data)
{
    PF_ParamDef	scaleFactor;
	AEFX_CLR_STRUCT(scaleFactor);
    scaleFactor.param_type = PF_Param_FLOAT_SLIDER;
    PF_STRCPY(scaleFactor.name, "Scaling factor :");
    scaleFactor.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    scaleFactor.u.fs_d.dephault = 1.0;
    scaleFactor.u.fs_d.value = 1.0;
    scaleFactor.u.fs_d.valid_min = 0.0;
    scaleFactor.u.fs_d.slider_min = 0.0;
    scaleFactor.u.fs_d.valid_max = 20.0;
    scaleFactor.u.fs_d.slider_max = 20.0;
    scaleFactor.u.fs_d.precision = 2;
    PF_ADD_PARAM(in_data, ID_SCALE_POS, &scaleFactor);

    PF_ParamDef	applyParam;
	AEFX_CLR_STRUCT(applyParam);
    applyParam.param_type = PF_Param_BUTTON;
    applyParam.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    A_char *applyStr = (A_char*)("Apply scaling");
    applyParam.u.button_d.u.namesptr = applyStr;
    PF_ADD_PARAM(in_data, ID_APPLY_SCALE_POS, &applyParam);
}
void MovingHandler::createTranslateGroup(PF_InData *in_data)
{
    // -- set shift
    PF_ParamDef	translatePoint;
    AEFX_CLR_STRUCT(translatePoint);
    translatePoint.param_type = PF_Param_POINT;
    translatePoint.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    PF_STRCPY(translatePoint.name, "Translate (-100 -> 100):");
    translatePoint.u.td.x_dephault = 0;
    translatePoint.u.td.y_dephault = 0;
    translatePoint.u.td.x_value = 0;
    translatePoint.u.td.y_value = 0;
    translatePoint.u.td.restrict_bounds = 0;
    PF_ADD_PARAM(in_data, ID_TRANSLATE_POS, &translatePoint);

    PF_ParamDef	applyParam;
	AEFX_CLR_STRUCT(applyParam);
    applyParam.param_type = PF_Param_BUTTON;
    applyParam.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    A_char *applyStr = (A_char*)("Apply translation");
    applyParam.u.button_d.u.namesptr = applyStr;
    PF_ADD_PARAM(in_data, ID_APPLY_TRANSLATE_POS, &applyParam);
}
void MovingHandler::createRotateGroup(PF_InData *in_data)
{
    PF_ParamDef	rotateAngle;
	AEFX_CLR_STRUCT(rotateAngle);
    rotateAngle.param_type = PF_Param_ANGLE;
    PF_STRCPY(rotateAngle.name, "Angle of rotation :");
    rotateAngle.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    rotateAngle.u.ad.valid_min = 0;
    rotateAngle.u.ad.valid_max = 360*65536.0;
    rotateAngle.u.ad.dephault = 0;
    rotateAngle.u.ad.value = 0;
    PF_ADD_PARAM(in_data, ID_ROTATION_ANGLE, &rotateAngle);

    // -- set center of rotation
    PF_ParamDef	centerPoint;
    AEFX_CLR_STRUCT(centerPoint);
    centerPoint.param_type = PF_Param_POINT;
    centerPoint.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    PF_STRCPY(centerPoint.name, "Center of rotation (0 -> 100):");
    centerPoint.u.td.x_dephault = 50.0*65536.0;
    centerPoint.u.td.y_dephault = 50.0*65536.0;
    centerPoint.u.td.x_value = 50.0*65536.0;
    centerPoint.u.td.y_value = 50.0*65536.0;
    centerPoint.u.td.restrict_bounds = 0;
    PF_ADD_PARAM(in_data, ID_ROTATION_CENTER, &centerPoint);

    PF_ParamDef	applyParam;
	AEFX_CLR_STRUCT(applyParam);
    applyParam.param_type = PF_Param_BUTTON;
    applyParam.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    A_char *applyStr = (A_char*)("Apply rotation");
    applyParam.u.button_d.u.namesptr = applyStr;
    PF_ADD_PARAM(in_data, ID_APPLY_ROTATION, &applyParam);
}
void MovingHandler::createLoopGroup(PF_InData *in_data)
{
    // -- list of positions presets
    PF_ParamDef	loopModeParam;
	AEFX_CLR_STRUCT(loopModeParam);
    loopModeParam.param_type = PF_Param_POPUP;
    PF_STRCPY(loopModeParam.name, "Loop mode :");
    loopModeParam.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    loopModeParam.u.pd.value = 1;
    loopModeParam.u.pd.dephault = 1;
    loopModeParam.u.pd.num_choices = 2;
    A_char *loopModeStr = (A_char*)("NORMAL | REVERSE");
    loopModeParam.u.pd.u.namesptr = loopModeStr;
    PF_ADD_PARAM(in_data, -1, &loopModeParam);

    PF_ParamDef	loopParam;
	AEFX_CLR_STRUCT(loopParam);
    loopParam.param_type = PF_Param_BUTTON;
    loopParam.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    A_char *loopStr = (A_char*)("LOOP");
    loopParam.u.button_d.u.namesptr = loopStr;
    PF_ADD_PARAM(in_data, -1, &loopParam);
}
void MovingHandler::createPresetsGroup(PF_InData *in_data, std::string presetsPosStr, std::string presetsColorStr, int nbPos, int nbColor)
{
    // -- group Transform
    PF_ParamDef	group;
    AEFX_CLR_STRUCT(group);
    group.param_type = PF_Param_GROUP_START;
    PF_STRCPY(group.name, "Presets");
    PF_ADD_PARAM(in_data, ID_GROUP_PRESET, &group);

    // -- list of positions presets
    PF_ParamDef	listPresetsPosition;
	AEFX_CLR_STRUCT(listPresetsPosition);
    listPresetsPosition.param_type = PF_Param_POPUP;
    PF_STRCPY(listPresetsPosition.name, "Presets positions :");
    listPresetsPosition.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    listPresetsPosition.u.pd.value = 1;
    listPresetsPosition.u.pd.dephault = 1;
    listPresetsPosition.u.pd.num_choices = nbPos;
    A_char *presetStrP = (A_char*)(presetsPosStr.c_str());
    listPresetsPosition.u.pd.u.namesptr = presetStrP;
    PF_ADD_PARAM(in_data, ID_LIST_PRESET_POS, &listPresetsPosition);

    PF_ParamDef	loadPosParam;
	AEFX_CLR_STRUCT(loadPosParam);
    loadPosParam.param_type = PF_Param_BUTTON;
    loadPosParam.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    A_char *loadPosStr = (A_char*)("Load positions");
    loadPosParam.u.button_d.u.namesptr = loadPosStr;
    PF_ADD_PARAM(in_data, ID_LOAD_PRESET_POS, &loadPosParam);

    PF_ParamDef	savePosParam;
	AEFX_CLR_STRUCT(savePosParam);
    savePosParam.param_type = PF_Param_BUTTON;
    savePosParam.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    A_char *savePosStr = (A_char*)("Save positions");
    savePosParam.u.button_d.u.namesptr = savePosStr;
    PF_ADD_PARAM(in_data, ID_SAVE_PRESET_POS, &savePosParam);

    // -- list of colors presets
    PF_ParamDef	listPresetsColors;
	AEFX_CLR_STRUCT(listPresetsColors);
    listPresetsColors.param_type = PF_Param_POPUP;
    PF_STRCPY(listPresetsColors.name, "Presets colors :");
    listPresetsColors.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    listPresetsColors.u.pd.value = 1;
    listPresetsColors.u.pd.dephault = 1;
    listPresetsColors.u.pd.num_choices = nbColor;
    A_char *presetStrC = (A_char*)(presetsColorStr.c_str());
    listPresetsColors.u.pd.u.namesptr = presetStrC;
    PF_ADD_PARAM(in_data, ID_LIST_PRESET_COLOR, &listPresetsColors);

    PF_ParamDef	loadColorParam;
	AEFX_CLR_STRUCT(loadColorParam);
    loadColorParam.param_type = PF_Param_BUTTON;
    loadColorParam.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    A_char *loadColorStr = (A_char*)("Load colors");
    loadColorParam.u.button_d.u.namesptr = loadColorStr;
    PF_ADD_PARAM(in_data, ID_LOAD_PRESET_COLOR, &loadColorParam);

    PF_ParamDef	saveColorParam;
	AEFX_CLR_STRUCT(saveColorParam);
    saveColorParam.param_type = PF_Param_BUTTON;
    saveColorParam.flags = PF_ParamFlag_CANNOT_TIME_VARY | PF_ParamFlag_SUPERVISE;
    A_char *saveColorStr = (A_char*)("Save colors");
    saveColorParam.u.button_d.u.namesptr = saveColorStr;
    PF_ADD_PARAM(in_data, ID_SAVE_PRESET_COLOR, &saveColorParam);

    AEFX_CLR_STRUCT(group);
    group.param_type = PF_Param_GROUP_END;
    PF_ADD_PARAM(in_data, ID_END_GROUP_PRESET, &group);
}

void MovingHandler::flat(unsigned char* data)
{
    int idx = 0;

    // -- 4+15+1+4+4+4+4+1+8+8+8+8+8+8+1+1 (87)
    // -- 8+8+8 by positions (24)
    // -- 4+8+8 by interpolations position (20) -> 44
    // -- 8+4+4+4+4 by colors (24)
    // -- 4+8 by interpolations colors (12) => 36

    // -- set uuid
    int sizeUuid = uuid.size();
    std::memcpy((void*)&data[idx], &sizeUuid, sizeof(sizeUuid));
    idx += sizeof(sizeUuid);
    const char* uuidC = uuid.c_str();
    std::memcpy((void*)&data[idx], uuidC, sizeUuid);
    idx += sizeUuid;

    // -- set type
	unsigned char typeChar = (unsigned char)type;
    data[idx] = typeChar;
    idx++;

    // -- set nbFrame
    std::memcpy((void*)&data[idx], &nbFrames, sizeof(nbFrames));
    idx += sizeof(nbFrames);

    // -- set current frame
    std::memcpy((void*)&data[idx], &frameToRender, sizeof(frameToRender));
    idx += sizeof(frameToRender);

    // -- set nbPos
    long sizePos = pos.size();
    std::memcpy((void*)&data[idx], &sizePos, sizeof(sizePos));
    idx += sizeof(sizePos);

    // -- set nbColor
    long sizeColor = color.size();
    std::memcpy((void*)&data[idx], &sizeColor, sizeof(sizeColor));
    idx += sizeof(sizeColor);

    // -- set Positions
    for (long i = 0; i < sizePos; i++) {
        Position* posTemp = pos.at(i);

        // -- set frame
        std::memcpy((void*)&data[idx], &posTemp->frame, sizeof(posTemp->frame));
        idx += sizeof(posTemp->frame);

        // -- set ratioX
        std::memcpy((void*)&data[idx], &posTemp->ratioX, sizeof(posTemp->ratioX));
        idx += sizeof(posTemp->ratioX);

        // -- set ratioY
        std::memcpy((void*)&data[idx], &posTemp->ratioY, sizeof(posTemp->ratioY));
        idx += sizeof(posTemp->ratioY);
    }

    // -- set Positions interpolations
    for (long i = 0; i < sizePos; i++) {
        Interpolation* interPosTemp = posInterpolate.at(i);

        // -- set type
        std::memcpy((void*)&data[idx], &interPosTemp->type, sizeof(interPosTemp->type));
        idx += sizeof(interPosTemp->type);

        // -- set extra
        std::memcpy((void*)&data[idx], &interPosTemp->extra, sizeof(interPosTemp->extra));
        idx += sizeof(interPosTemp->extra);

        // -- set extra2
        std::memcpy((void*)&data[idx], &interPosTemp->extra2, sizeof(interPosTemp->extra2));
        idx += sizeof(interPosTemp->extra2);
    }

    // -- set Colors
    for (long i = 0; i < sizeColor; i++) {
        Color* colorTemp = color.at(i);

        // -- set frame
        std::memcpy((void*)&data[idx], &colorTemp->frame, sizeof(colorTemp->frame));
        idx += sizeof(colorTemp->frame);

        // -- set red
        std::memcpy((void*)&data[idx], &colorTemp->red, sizeof(colorTemp->red));
        idx += sizeof(colorTemp->red);

        // -- set green
        std::memcpy((void*)&data[idx], &colorTemp->green, sizeof(colorTemp->green));
        idx += sizeof(colorTemp->green);

        // -- set blue
        std::memcpy((void*)&data[idx], &colorTemp->blue, sizeof(colorTemp->blue));
        idx += sizeof(colorTemp->blue);

        // -- set blue
        std::memcpy((void*)&data[idx], &colorTemp->alpha, sizeof(colorTemp->alpha));
        idx += sizeof(colorTemp->alpha);
    }

    // -- set Colors interpolations
    for (long i = 0; i < sizeColor; i++) {
        Interpolation* interColorTemp = colorInterpolate.at(i);

        // -- set type
        std::memcpy((void*)&data[idx], &interColorTemp->type, sizeof(interColorTemp->type));
        idx += sizeof(interColorTemp->type);

        // -- set extra
        std::memcpy((void*)&data[idx], &interColorTemp->extra, sizeof(interColorTemp->extra));
        idx += sizeof(interColorTemp->extra);
    }

    // -- set resizing
    std::memcpy((void*)&data[idx], &resizeTimeline, sizeof(resizeTimeline));
    idx += sizeof(resizeTimeline);

    std::memcpy((void*)&data[idx], &translateTimeline, sizeof(translateTimeline));
    idx += sizeof(translateTimeline);

    // -- set scale factor
    std::memcpy((void*)&data[idx], &scaleFactor, sizeof(scaleFactor));
    idx += sizeof(scaleFactor);

    // -- set vector translation X
    std::memcpy((void*)&data[idx], &translateX, sizeof(translateX));
    idx += sizeof(translateX);

    // -- set vector translation Y
    std::memcpy((void*)&data[idx], &translateY, sizeof(translateY));
    idx += sizeof(translateY);

    // -- set rotation angle
    std::memcpy((void*)&data[idx], &rotationAngle, sizeof(rotationAngle));
    idx += sizeof(rotationAngle);

    // -- set centerX rotation
    std::memcpy((void*)&data[idx], &centerX, sizeof(centerX));
    idx += sizeof(centerX);

    // -- set centerY rotation
    std::memcpy((void*)&data[idx], &centerY, sizeof(centerY));
    idx += sizeof(centerY);

    // -- set mode loop
    std::memcpy((void*)&data[idx], &modeLoop, sizeof(modeLoop));
    idx += sizeof(modeLoop);

    // -- set visible
    std::memcpy((void*)&data[idx], &visible, sizeof(visible));
    idx += sizeof(visible);
}
void MovingHandler::unflat(unsigned char* data)
{
    int idx = 0;

    // -- set uuid
    int sizeUuid;
    std::memcpy(&sizeUuid, (void*)&data[idx], sizeof(sizeUuid));
    idx += sizeof(sizeUuid);

    char* uuidC = new char[sizeUuid+1];
    std::memcpy(uuidC, (void*)&data[idx], sizeUuid);
    uuidC[sizeUuid] = '\0';
    uuid = std::string(uuidC);
    delete[] uuidC;
    idx += sizeUuid;

    // -- set type
    type = (MovingHeadType)data[idx];
    idx++;

    // -- set nbFrame
    std::memcpy(&nbFrames, (void*)&data[idx], sizeof(nbFrames));
    idx += sizeof(nbFrames);

    // -- set current frame
    std::memcpy(&frameToRender, (void*)&data[idx], sizeof(frameToRender));
    idx += sizeof(frameToRender);

    // -- set nbPos
    long sizePos = 0;
    std::memcpy(&sizePos, (void*)&data[idx], sizeof(sizePos));
    idx += sizeof(sizePos);

    // -- set nbColor
    long sizeColor = 0;
    std::memcpy(&sizeColor, (void*)&data[idx], sizeof(sizeColor));
    idx += sizeof(sizeColor);

    // -- set Positions
    for (long i = 0; i < pos.size(); i++) {
        delete pos.at(i);
    }
    pos.clear();
    for (long i = 0; i < sizePos; i++) {

        Position* posTemp = new Position();

        // -- set frame
        std::memcpy(&posTemp->frame, (void*)&data[idx], sizeof(posTemp->frame));
        idx += sizeof(posTemp->frame);

        // -- set ratioX
        std::memcpy(&posTemp->ratioX, (void*)&data[idx], sizeof(posTemp->ratioX));
        idx += sizeof(posTemp->ratioX);

        // -- set ratioY
        std::memcpy(&posTemp->ratioY, (void*)&data[idx], sizeof(posTemp->ratioY));
        idx += sizeof(posTemp->ratioY);

        pos.push_back(posTemp);
    }

    // -- set Positions interpolation
    for (long i = 0; i < posInterpolate.size(); i++) {
        delete posInterpolate.at(i);
    }
    posInterpolate.clear();
    for (long i = 0; i < sizePos; i++) {

        Interpolation* posInterTemp = new Interpolation();

        // -- set type
        std::memcpy(&posInterTemp->type, (void*)&data[idx], sizeof(posInterTemp->type));
        idx += sizeof(posInterTemp->type);

        // -- set extra
        std::memcpy(&posInterTemp->extra, (void*)&data[idx], sizeof(posInterTemp->extra));
        idx += sizeof(posInterTemp->extra);

        // -- set extra2
        std::memcpy(&posInterTemp->extra2, (void*)&data[idx], sizeof(posInterTemp->extra2));
        idx += sizeof(posInterTemp->extra2);

        posInterpolate.push_back(posInterTemp);

        // -- recalculate interpolation points datas
        if (posInterTemp->type == 2) {recalculateCurve(i);}
        else if (posInterTemp->type == 3) {recalculateArc(i);}
    }

    // -- set Colors
    for (long i = 0; i < color.size(); i++) {
        delete color.at(i);
    }
    color.clear();
    for (long i = 0; i < sizeColor; i++) {

        Color* colorTemp = new Color();

        // -- set frame
        std::memcpy(&colorTemp->frame, (void*)&data[idx], sizeof(colorTemp->frame));
        idx += sizeof(colorTemp->frame);

        // -- set red
        std::memcpy(&colorTemp->red, (void*)&data[idx], sizeof(colorTemp->red));
        idx += sizeof(colorTemp->red);

        // -- set green
        std::memcpy(&colorTemp->green, (void*)&data[idx], sizeof(colorTemp->green));
        idx += sizeof(colorTemp->green);

        // -- set blue
        std::memcpy(&colorTemp->blue, (void*)&data[idx], sizeof(colorTemp->blue));
        idx += sizeof(colorTemp->blue);

        // -- set alpha
        std::memcpy(&colorTemp->alpha, (void*)&data[idx], sizeof(colorTemp->alpha));
        idx += sizeof(colorTemp->alpha);

        color.push_back(colorTemp);
    }

    // -- set Colors interpolation
    for (long i = 0; i < colorInterpolate.size(); i++) {
        delete colorInterpolate.at(i);
    }
    colorInterpolate.clear();
    for (long i = 0; i < sizeColor; i++) {

        Interpolation* colorInterTemp = new Interpolation();

        // -- set type
        std::memcpy(&colorInterTemp->type, (void*)&data[idx], sizeof(colorInterTemp->type));
        idx += sizeof(colorInterTemp->type);

        // -- set extra
        std::memcpy(&colorInterTemp->extra, (void*)&data[idx], sizeof(colorInterTemp->extra));
        idx += sizeof(colorInterTemp->extra);

        colorInterpolate.push_back(colorInterTemp);
        if (colorInterTemp->type > 1) {recalculateColorInter(i);}
    }

    // -- set resizing
    std::memcpy(&resizeTimeline, (void*)&data[idx], sizeof(resizeTimeline));
    idx += sizeof(resizeTimeline);

    std::memcpy(&translateTimeline, (void*)&data[idx], sizeof(translateTimeline));
    idx += sizeof(translateTimeline);

    // -- set scale factor
    std::memcpy(&scaleFactor, (void*)&data[idx], sizeof(scaleFactor));
    idx += sizeof(scaleFactor);

    // -- set vector translation X
    std::memcpy(&translateX, (void*)&data[idx], sizeof(translateX));
    idx += sizeof(translateX);

    // -- set vector translation Y
    std::memcpy(&translateY, (void*)&data[idx], sizeof(translateY));
    idx += sizeof(translateY);

    // -- set rotation angle
    std::memcpy(&rotationAngle, (void*)&data[idx], sizeof(rotationAngle));
    idx += sizeof(rotationAngle);

    // -- set centerX rotation
    std::memcpy(&centerX, (void*)&data[idx], sizeof(centerX));
    idx += sizeof(centerX);

    // -- set centerY rotation
    std::memcpy(&centerY, (void*)&data[idx], sizeof(centerY));
    idx += sizeof(centerY);

    // -- set mode loop
    std::memcpy(&modeLoop, (void*)&data[idx], sizeof(modeLoop));
    idx += sizeof(modeLoop);

    // -- set visible
    std::memcpy(&visible, (void*)&data[idx], sizeof(visible));
    idx += sizeof(visible);
}
std::string MovingHandler::getContentString(bool isColor)
{
    std::string content = "";
    content += "FRAMES " + std::to_string(nbFrames) + "\n\n";

    if (!isColor) {
        content += "POSITIONS " + std::to_string(pos.size()) + "\n";
        for (int i = 0; i < pos.size(); i++) {
            content += "FRAME " + std::to_string(pos.at(i)->frame) + "\n";
            content += "X " + std::to_string(pos.at(i)->ratioX) + "\n";
            content += "Y " + std::to_string(pos.at(i)->ratioY) + "\n";
        }
        content += "\nINTERPOS " + std::to_string(posInterpolate.size()) + "\n";
        for (int i = 0; i < posInterpolate.size(); i++) {
            content += "TYPE " + std::to_string(posInterpolate.at(i)->type) + "\n";
            content += "X " + std::to_string(posInterpolate.at(i)->extra) + "\n";
            content += "X2 " + std::to_string(posInterpolate.at(i)->extra2) + "\n";
        }
    }
    else {
        content += "COLORS " + std::to_string(color.size()) + "\n";
        for (int i = 0; i < color.size(); i++) {
            content += "FRAME " + std::to_string(color.at(i)->frame) + "\n";
            content += "R " + std::to_string(color.at(i)->red) + "\n";
            content += "G " + std::to_string(color.at(i)->green) + "\n";
            content += "B " + std::to_string(color.at(i)->blue) + "\n";
            content += "A " + std::to_string(color.at(i)->alpha) + "\n";
        }

        content += "\nINTERCOLOR " + std::to_string(colorInterpolate.size()) + "\n";
        for (int i = 0; i < colorInterpolate.size(); i++) {
            content += "TYPE " + std::to_string(colorInterpolate.at(i)->type) + "\n";
            content += "X " + std::to_string(colorInterpolate.at(i)->extra) + "\n";
        }
    }

    return content;
}
bool MovingHandler::loadFromString(std::string content)
{
    std::stringstream ss(content);
    std::string line;

    try {
        while(std::getline(ss, line, '\n')) {
            int posSpace = line.find(" ", 0);
            std::string key = line.substr(0, posSpace);
            std::string value = line.substr(posSpace+1);

            if (key.find("FRAMES") != std::string::npos) {
                this->nbFrames = std::stoi(value);
            }

            else if (key.find("POSITIONS") != std::string::npos) {
                int nbPos = std::stoi(value);
                for (int i = 0; i < nbPos; i++) {

                    Position* newPos = new Position();

                    // -- frame
                    std::getline(ss, line, '\n');
                    posSpace = line.find(" ", 0);
                    value = line.substr(posSpace+1);
                    newPos->frame = std::stoi(value);

                    // -- X
                    std::getline(ss, line, '\n');
                    posSpace = line.find(" ", 0);
                    value = line.substr(posSpace+1);
                    newPos->ratioX = std::stod(value);

                    // -- Y
                    std::getline(ss, line, '\n');
                    posSpace = line.find(" ", 0);
                    value = line.substr(posSpace+1);
                    newPos->ratioY = std::stod(value);

                    pos.push_back(newPos);
                }
            }
            
            else if (key.find("COLORS") != std::string::npos) {
                int nbColor = std::stoi(value);
                for (int i = 0; i < nbColor; i++) {

                    Color* newColor = new Color();

                    // -- frame
                    std::getline(ss, line, '\n');
                    posSpace = line.find(" ", 0);
                    value = line.substr(posSpace+1);
                    newColor->frame = std::stoi(value);

                    // -- RED
                    std::getline(ss, line, '\n');
                    posSpace = line.find(" ", 0);
                    value = line.substr(posSpace+1);
                    newColor->red = std::stoi(value);

                    // -- GREEN
                    std::getline(ss, line, '\n');
                    posSpace = line.find(" ", 0);
                    value = line.substr(posSpace+1);
                    newColor->green = std::stoi(value);

                    // -- BLUE
                    std::getline(ss, line, '\n');
                    posSpace = line.find(" ", 0);
                    value = line.substr(posSpace+1);
                    newColor->blue = std::stoi(value);
                    
                    // -- ALPHA
                    std::getline(ss, line, '\n');
                    posSpace = line.find(" ", 0);
                    value = line.substr(posSpace+1);
                    newColor->alpha = std::stoi(value);

                    color.push_back(newColor);
                }
            }

            else if (key.find("INTERPOS") != std::string::npos) {
                int nbPosInter = std::stoi(value);
                for (int i = 0; i < nbPosInter; i++) {

                    Interpolation* newInter = new Interpolation();

                    // -- TYPE
                    std::getline(ss, line, '\n');
                    posSpace = line.find(" ", 0);
                    value = line.substr(posSpace+1);
                    newInter->type = std::stoi(value);

                    // -- EXTRA
                    std::getline(ss, line, '\n');
                    posSpace = line.find(" ", 0);
                    value = line.substr(posSpace+1);
                    newInter->extra = std::stod(value);

                    // -- EXTRA 2
                    std::getline(ss, line, '\n');
                    posSpace = line.find(" ", 0);
                    value = line.substr(posSpace+1);
                    newInter->extra2 = std::stod(value);

                    posInterpolate.push_back(newInter);
                    
                    if (newInter->type == 2) {recalculateCurve(i);}
                    else if (newInter->type == 2) {recalculateArc(i);}
                }
            }
        
            else if (key.find("INTERCOLOR") != std::string::npos) {
                int nbColorInter = std::stoi(value);
                for (int i = 0; i < nbColorInter; i++) {

                    Interpolation* newInter = new Interpolation();

                    // -- TYPE
                    std::getline(ss, line, '\n');
                    posSpace = line.find(" ", 0);
                    value = line.substr(posSpace+1);
                    newInter->type = std::stoi(value);

                    // -- EXTRA
                    std::getline(ss, line, '\n');
                    posSpace = line.find(" ", 0);
                    value = line.substr(posSpace+1);
                    newInter->extra = std::stod(value);

                    colorInterpolate.push_back(newInter);
                    
                    if (newInter->type > 1) {recalculateColorInter(i);}
                }
            }
        }
    }
    catch (std::exception &e) {
        return false;
    }

    return true;
}
