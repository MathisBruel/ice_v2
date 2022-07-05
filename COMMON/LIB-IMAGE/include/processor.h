#include <stdlib.h>
#include <iostream>
#include <climits>
#include <float.h>
#include <map>
#include <fstream>

#include "image.h"
#define M_PI 3.14159265358979323846

#pragma once

class Processor
{

public:

    struct Point {
        int x;
        int y;
    };

    struct Seam {
        std::vector<Point*> points;
        double energy;
    };

    // -- local typedef for contour definition
    struct ContourPoint 
    {
        int x;
        int y;
        int direction; // Clockwise starting from REAR, (REAR-LEFT, LEFT, FRONT-LEFT, FRONT, FRONT-RIGHT, RIGHT, REAR-RIGHT)
    };
    struct Contour {std::vector<ContourPoint*> points;};
    std::map<int, int*> pointsTaken;

    Processor(Image* img);
    Processor(int width, int height, double **datas);
    ~Processor();

    void transformToGray(Image* img);
    
    void gaussianBlur(double sigma, int radius);
    void gaussianBlurRGB(double sigma, int radius);
    void medianBlur(int radius);

    void sharpen();
    void sharpen2(double sharpFactor, int radius);
    void threshold(int limit);
    
    void erode(int radius);
    void dilate(int radius);

    void calculateRoberts();
    void calculatePrewitt();
    void calculateSobel();
    void calculateSobelRGB();
    void calculateKirsch();

    void contourTracing();
    void simplifyContour(int nbPoints);

    Image* seamCarvingRemove(Image* refImg);
    Image* seamCarvingRemoveHigh(Image* refImg);
    Image* seamCarvingAdd(Image* refImg);
    Image* seamCarvingAddToLeft(Image* refImg, int newWidth);
    Image* seamCarvingAddToRight(Image* refImg, int newWidth);

    void seamCarvingDetect(bool saveEnergy = false);
    void seamCarvingDetectRGB(bool saveEnergy = false);
    void seamCarvingTracing(Image* refImg);
    void removeSeam(Seam* seam);

    Image* computeImage();

private:

    // -- for gaussian blur
    static double sigmaGauss(double x, double y, double sigma);

    // -- for contour detection
    bool isStartPixel(int x, int y, int direction);
    void detectContour(Contour* contour, int startX, int startY, int startDir);

    // -- function to get right pixel for orientation given
    double getRear(int x, int y, int direction);
    double getRearLeft(int x, int y, int direction);
    double getLeft(int x, int y, int direction);
    double getFrontLeft(int x, int y, int direction);
    double getFront(int x, int y, int direction);

    // -- function to create new contour point
    ContourPoint* createContourPointLeft(int x, int y, int startDir, int newDir);
    ContourPoint* createContourPointRearLeft(int x, int y, int startDir, int newDir);
    ContourPoint* createContourPointFrontLeft(int x, int y, int startDir, int newDir);
    ContourPoint* createContourPointFront(int x, int y, int startDir, int newDir);
    ContourPoint* createContourPointCenter(int x, int y, int startDir, int newDir);

    // -- list of contours detected
    std::vector<Contour*> contours;
    std::vector<Contour*> contourSimplified;

    // -- polyline simplification
    void polyLineSimplification(Contour* contour, int nbPoints);
    void douglasPeucker(Contour* contour, double* weights, int startIdx, int endIdx);
    double getDistancePolyline(Contour* contour, int startIdx, int endIdx, int currentIdx);

    // -- seam carving results
    std::vector<Seam*> seams;

    // -- reference image
    int width;
    int height;
    double** datas;
    double** datasR;
    double** datasG;
    double** datasB;
};