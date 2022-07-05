#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <AE_Effect.h>
#include <AE_Macros.h>
#include <AE_EffectUI.h>
#include <AE_EffectCB.h>
#include <math.h>
#include <cstdlib>

# define PI 3.14159265358979323846  /* pi */

#pragma once

// -- global
#define ID_CHANGETYPE 1
#define ID_UPDATE 2
#define ID_VISIBLE 3

// -- POS
#define ID_GROUP_POS 4
#define ID_ACTIVEPOS 5
#define ID_POSITION 6
#define ID_INTERPOS_TYPE 7
#define ID_INTERPOS_EXTRA 8
#define ID_INTERPOS_EXTRA2 9
#define ID_END_GROUP_POS 10

// -- COLOR
#define ID_GROUP_COLOR 11
#define ID_ACTIVECOLOR 12
#define ID_COLOR 13
#define ID_COLOR_INTENSITY 14
#define ID_INTERCOLOR_TYPE 15
#define ID_INTERCOLOR_EXTRA 16
#define ID_END_GROUP_COLOR 17

// -- TIMELINE
#define ID_GROUP_TIMELINE 18
#define ID_RESIZE_TIMELINE 19
#define ID_APPLY_RESIZE_TIMELINE 20
#define ID_TRANSLATE_TIMELINE 21
#define ID_APPLY_TRANSLATE_TIMELINE 22
#define ID_REVERSE_TIMELINE 23
#define ID_MODE_LOOP 24
#define ID_APPLY_LOOP 25
#define ID_END_GROUP_TIMELINE 26

// -- TRANSFORMATION
#define ID_GROUP_TRANSFORM 27
#define ID_FLIP_POS_H 28
#define ID_FLIP_POS_V 29
#define ID_SCALE_POS 30
#define ID_APPLY_SCALE_POS 31
#define ID_TRANSLATE_POS 32
#define ID_APPLY_TRANSLATE_POS 33
#define ID_ROTATION_ANGLE 34
#define ID_ROTATION_CENTER 35
#define ID_APPLY_ROTATION 36
#define ID_END_GROUP_TRANSFORM 37

// -- PRESETS
#define ID_GROUP_PRESET 38
#define ID_LIST_PRESET_POS 39
#define ID_LOAD_PRESET_POS 40
#define ID_SAVE_PRESET_POS 41
#define ID_LIST_PRESET_COLOR 42
#define ID_LOAD_PRESET_COLOR 43
#define ID_SAVE_PRESET_COLOR 44
#define ID_END_GROUP_PRESET 45

struct Position
{
    double ratioX;
    double ratioY;
    long frame;
};

struct Interpolation
{
    // -- 1 : LINEAR, 2 : CURVE, 3 : ARC
    int type;
    double extra;
    double extra2;

    // -- for curve computation
    // -- start point
    double startX;
    double startY;

    // -- vector unitary for each frame
    double vectorX;
    double vectorY;

    // -- result of computation
    double a, b, c;
    bool alongX;

    // -- for curve computation
    // -- center of the circle
    double centerX;
    double centerY;

    // -- step for each frame
    double stepAngle;

    // -- radius
    double radius;

    // -- angles
    double startAngle;
    double midAngle;
    double endAngle;

    // -- common to CURVE and ARC
    // -- middle point
    double tempX;
    double tempY;
    bool valid;

    // -- for COLOR interpolation
    double offset;
    double ratio;
};

struct Color
{
    int red;
    int green;
    int blue;
    int alpha;
    long frame;
};

enum MovingHeadType {
    FRONT_LEFT = 1,
    FRONT_RIGHT,
    REAR_LEFT,
    REAR_RIGHT
};

class MovingHandler
{

public:

    static int offsetXLeft;
    static int offsetXRight;
    static int sizeCube;
    static int offsetYFront;
    static int offsetYRear;
    static int sizeYColor;
    
    MovingHandler();
    ~MovingHandler();

    void reloadPositions(MovingHandler * ref);
    void reloadColors(MovingHandler * ref);

    // -- paramters
    static void createParams(PF_InData *in_data, std::string presetsPosStr, std::string presetsColorStr, int nbPos, int nbColor);
    static void createPositionGroup(PF_InData *in_data);
    static void createColorGroup(PF_InData *in_data);
    static void createTimelineGroup(PF_InData *in_data);
    static void createTransformGroup(PF_InData *in_data);
    static void createScaleGroup(PF_InData *in_data);
    static void createTranslateGroup(PF_InData *in_data);
    static void createRotateGroup(PF_InData *in_data);
    static void createPresetsGroup(PF_InData *in_data, std::string presetsPosStr, std::string presetsColorStr, int nbPos, int nbColor);
    static void createLoopGroup(PF_InData *in_data);

    // -- GLOBAL
    void changetype(MovingHeadType typeTemp);
    void setVisible(bool isVisible) {this->visible = isVisible;}
    void setNbFrames(long nbFramesTemp);

    // -- timeline transformations
    void setResize(double resizeValue) {resizeTimeline = resizeValue; needSave = true;}
    void applyTimelineResize();
    void setTimelineTranslation(double translateValue) {translateTimeline = (int)translateValue; needSave = true;}
    void applyTimelineTranslation();
    void reverseTimeline();
    
    // -- modify positions
    void createPosition(double x, double y);
    void removePosition();
    void changePosInterpolate(int type, double extra, double extra2 = -100.0);

    // -- modify colors
    void createColor(int red, int green, int blue, int alpha);
    void removeColor();
    void changeColorInterpolate(int type, double extra);

    // -- modify positions forms
    void changeScaleFactor(double factor) {scaleFactor = factor; needSave = true;}
    void applyScaling();
    void changeTranslationVector(double depX, double depY) {translateX = depX; translateY = depY; needSave = true;}
    void applyTranslation();
    void changeRotationAngle(double angle) {rotationAngle = angle; needSave = true;}
    void changeCenterRotation(double x, double y) {centerX = x; centerY = y; needSave = true;}
    void applyRotation();
    void flipH();
    void flipV();

    // -- loop
    void setLoopMode(bool mode) {modeLoop = mode; needSave = true;}
    void loop();

    // -- draw functions
    void createCanvas(PF_InData *in_data, PF_LayerDef *output);
    void createPositionPoints(PF_InData *in_data, PF_LayerDef *output, int frame);
    void createPositionLine(PF_InData *in_data, PF_LayerDef *output, int frame);
    void createColorsRect(PF_InData *in_data, PF_LayerDef *output, int frame);
    void createFinalRender(PF_InData *in_data, PF_LayerDef *output, int frame);

    // -- coherence for modification
    void setLastRenderFrame(long last) {frameToRender = last;}
    long getLastRendererFrame() {return frameToRender;}
    double getScaleFactor() {return scaleFactor;}
    double getTranslationX() {return translateX;}
    double getTranslationY() {return translateY;}
    double getRotationAngle() {return rotationAngle;}
    double getCenterRotationX() {return centerX;}
    double getCenterRotationY() {return centerY;}

    // -- getters
    bool isPositionSetAtTime(long currentFrame);
    bool isColorSetAtTime(long currentFrame);
    Position* getPositionAtTime(long currentFrame);
    Color* getColorAtTime(long currentFrame);
    Interpolation* getPosInterpolationAtTime(long currentFrame);
    Interpolation* getColorInterpolationAtTime(long currentFrame);
    bool isFrameValid() {return frameToRender != -1;}
    double getResizeFactor() {return resizeTimeline;}
    double getTimelineTranslation() {return (int)translateTimeline;}
    bool isModeLoopReversed() {return modeLoop;}
    std::string getUuid() {return uuid;}
    bool isVisible() {return visible;}
    long getNbFrames() {return nbFrames;}

    // -- save methods
    void flat(unsigned char* data);
    void unflat(unsigned char* data);
    void setSave() {needSave = true;}
    bool isSaveNeeded() {return needSave;}

    // -- hard saving
    std::string getContentString(bool isColor);
    bool loadFromString(std::string content); 

private:

    // -- interpolation global calculation 
    void recalculateCurve(int idx);
    void recalculateArc(int idx);
    void recalculateColorInter(int idx);
    void recalculateAllInterpolation();

    Position* scalePoint(Position* inPoint);
    Position* translatePoint(Position* inPoint);
    Position* rotatePoint(Position* inPoint);

    // -- uuid
    std::string uuid;

    // -- type of movinghead
    MovingHeadType type;

    // -- size of the timeline
    long nbFrames;
    // -- current frame to render or to modify
    long frameToRender;

    // -- positions
    std::vector<Position*> pos;
    // -- colors
    std::vector<Color*> color;

    // -- interpolation between position
    std::vector<Interpolation*> posInterpolate;
    // -- interpolation between colors
    std::vector<Interpolation*> colorInterpolate;

    // -- mode loop : is reversed
    bool modeLoop;
    // -- temporary timeline value
    double resizeTimeline;
    int translateTimeline;

    // -- flag to set save is required : force to be flattened
    bool needSave;

    // -- transformation
    double scaleFactor;
    double translateX;
    double translateY;
    double rotationAngle;
    double centerX;
    double centerY;

    bool visible;
};