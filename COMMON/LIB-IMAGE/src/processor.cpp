#include "processor.h"

Processor::Processor(Image* img)
{
    this->width = img->getWidth();
    this->height = img->getHeight();
    transformToGray(img);
}
Processor::Processor(int width, int height, double **datas)
{
    this->width = width;
    this->height = height;

    this->datas = new double*[width];
    this->datasR = new double*[width];
    this->datasG = new double*[width];
    this->datasB = new double*[width];
    for (int i = 0; i < width; i++) {
        this->datas[i] = new double[height];
        this->datasR[i] = new double[height];
        this->datasG[i] = new double[height];
        this->datasB[i] = new double[height];
    }
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            this->datas[i][j] = datas[i][j];
            this->datasR[i][j] = datasR[i][j];
            this->datasG[i][j] = datasG[i][j];
            this->datasB[i][j] = datasB[i][j];
        }
    }
}
Processor::~Processor() 
{
    for (int i = 0; i < width; i++) {
        delete[] datas[i];
        delete[] datasR[i];
        delete[] datasG[i];
        delete[] datasB[i];
    }
    delete[] datas;
    delete[] datasR;
    delete[] datasG;
    delete[] datasB;
}

struct CompareSeam
{
    bool operator() (Processor::Seam* s1, Processor::Seam* s2) const
    {
        return (s1->energy < s2->energy);
    }
};

void Processor::transformToGray(Image* img) {
    datas = new double*[width];
    datasR = new double*[width];
    datasG = new double*[width];
    datasB = new double*[width];
    for (int i = 0; i < width; i++) {
        datas[i] = new double[height];
        datasR[i] = new double[height];
        datasG[i] = new double[height];
        datasB[i] = new double[height];
    }
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            Color* colorSrc = img->getValueAt(i, j);
            datas[i][j] = colorSrc->getRed()*0.2989 + colorSrc->getGreen()*0.587 + colorSrc->getBlue()*0.114;
            datasR[i][j] = colorSrc->getRed();
            datasG[i][j] = colorSrc->getGreen();
            datasB[i][j] = colorSrc->getBlue();
            delete colorSrc;
        }
    }
}

Image* Processor::computeImage()
{
    Image* img = new Image(width, height, PNG_FORMAT_RGB, 0, 0, 0);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            Color temp;
            temp.setRGB(datas[i][j], datas[i][j], datas[i][j]);
            img->setValueAt(i, j, temp);
        }
    }
    return img;
}

double Processor::sigmaGauss(double x, double y, double sigma)
{
    double value = 1.0 / (2*M_PI*sigma*sigma) * (std::exp(-(x*x+y*y)/(2*sigma*sigma)));
    return value;
}
void Processor::gaussianBlur(double sigma, int radius)
{
    // -- calculate CORE
    double** core = new double*[radius*2+1];
    for (int x = 0; x < radius*2+1; x++) {
        core[x] = new double[radius*2+1];
    }
    for (int x = -radius; x <= radius; x++) {
        for (int y = -radius; y <= radius; y++) {
            double xD = std::fabs((double)x/(double)radius);
            double yD = std::fabs((double)y/(double)radius);
            core[x+radius][y+radius] = sigmaGauss(xD, yD, sigma);
        }
    }
    double factor = 0.0;
    for (int x = 0; x < radius*2+1; x++) {
        for (int y = 0; y < radius*2+1; y++) {
            factor += core[x][y];
        }
    }

    double** newDatas = new double*[width];
    for (int i = 0; i < width; i++) {
        newDatas[i] = new double[height];
        for (int j = 0; j < height; j++) {
            newDatas[i][j] = 0.0;
        }
    }
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            int localRadius = radius;

            // -- determine radius taken into account (for borders overflow)
            if (i-localRadius < 0) {
                localRadius = i;
            }
            else if (i+localRadius >= width) {
                localRadius = width - i - 1;
            }
            if (j-localRadius < 0) {
                localRadius = j;
            }
            else if (j+localRadius >= height) {
                localRadius = height - j - 1;
            }

            // -- fill array with all values
            double value = 0.0;
            for (int x = -localRadius; x <= localRadius; x++) {
                for (int y = -localRadius; y <= localRadius; y++)  {
                    value += datas[i+x][j+y] * core[x+radius][y+radius]/factor;
                }
            }

            if (value < 0.0) {value = 0.0;}
            else if (value > 1.0) {value = 1.0;}       
            newDatas[i][j] = value;
        }
    }
    for (int i = 0; i < width; i++) {
        delete[] datas[i];
    }
    delete[] datas;
    datas = newDatas;
}
void Processor::gaussianBlurRGB(double sigma, int radius)
{
    // -- calculate CORE
    double** core = new double*[radius*2+1];
    for (int x = 0; x < radius*2+1; x++) {
        core[x] = new double[radius*2+1];
    }
    for (int x = -radius; x <= radius; x++) {
        for (int y = -radius; y <= radius; y++) {
            double xD = std::fabs((double)x/(double)radius);
            double yD = std::fabs((double)y/(double)radius);
            core[x+radius][y+radius] = sigmaGauss(xD, yD, sigma);
        }
    }
    double factor = 0.0;
    for (int x = 0; x < radius*2+1; x++) {
        for (int y = 0; y < radius*2+1; y++) {
            factor += core[x][y];
        }
    }

    double** newDatasR = new double*[width];
    double** newDatasG = new double*[width];
    double** newDatasB = new double*[width];
    for (int i = 0; i < width; i++) {
        newDatasR[i] = new double[height];
        newDatasG[i] = new double[height];
        newDatasB[i] = new double[height];
        for (int j = 0; j < height; j++) {
            newDatasR[i][j] = 0.0;
            newDatasG[i][j] = 0.0;
            newDatasB[i][j] = 0.0;
        }
    }
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            int localRadius = radius;

            // -- determine radius taken into account (for borders overflow)
            if (i-localRadius < 0) {
                localRadius = i;
            }
            else if (i+localRadius >= width) {
                localRadius = width - i - 1;
            }
            if (j-localRadius < 0) {
                localRadius = j;
            }
            else if (j+localRadius >= height) {
                localRadius = height - j - 1;
            }

            // -- fill array with all values
            double valueR = 0.0;
            double valueG = 0.0;
            double valueB = 0.0;
            for (int x = -localRadius; x <= localRadius; x++) {
                for (int y = -localRadius; y <= localRadius; y++)  {
                    valueR += datasR[i+x][j+y] * core[x+radius][y+radius]/factor;
                    valueG += datasG[i+x][j+y] * core[x+radius][y+radius]/factor;
                    valueB += datasB[i+x][j+y] * core[x+radius][y+radius]/factor;
                }
            }

            if (valueR < 0.0) {valueR = 0.0;}
            else if (valueR > 1.0) {valueR = 1.0;}   
            if (valueG < 0.0) {valueG = 0.0;}
            else if (valueG > 1.0) {valueG = 1.0;}  
            if (valueB < 0.0) {valueB = 0.0;}
            else if (valueB > 1.0) {valueB = 1.0;}      
            newDatasR[i][j] = valueR;
            newDatasG[i][j] = valueG;
            newDatasB[i][j] = valueB;
        }
    }
    for (int i = 0; i < width; i++) {
        delete[] datasR[i];
        delete[] datasG[i];
        delete[] datasB[i];
    }
    delete[] datasR;
    delete[] datasG;
    delete[] datasB;
    datasR = newDatasR;
    datasG = newDatasG;
    datasB = newDatasB;
}
void Processor::medianBlur(int radius)
{
    double** newDatas = new double*[width];
    for (int i = 0; i < width; i++) {
        newDatas[i] = new double[height];
        for (int j = 0; j < height; j++) {
            newDatas[i][j] = 0.0;
        }
    }
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            int localRadius = radius;

            // -- determine radius taken into account (for borders overflow)
            if (i-localRadius < 0) {
                localRadius = i;
            }
            else if (i+localRadius >= width) {
                localRadius = width - i - 1;
            }
            if (j-localRadius < 0) {
                localRadius = j;
            }
            else if (j+localRadius >= height) {
                localRadius = height - j - 1;
            }

            // -- fill array with all values
            std::vector<double> values;
            for (int x = i-localRadius; x <= i+localRadius; x++) {
                for (int y = j-localRadius; y <= j+localRadius; y++) {
                    values.push_back(datas[x][y]);
                }
            }

            // -- sorting
            std::sort(values.begin(), values.end());

            // -- determine index median
            int index = values.size() / 2;
            double value = values.at(index);
            newDatas[i][j] = value;
        }
    }
    for (int i = 0; i < width; i++) {
        delete[] datas[i];
    }
    delete[] datas;
    datas = newDatas;
}

void Processor::sharpen()
{
    double** newDatas = new double*[width];
    for (int i = 0; i < width; i++) {
        newDatas[i] = new double[height];
        for (int j = 0; j < height; j++) {
            newDatas[i][j] = 0.0;
        }
    }
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            double color1 = (i == 0 || j == 0) ? 0 : datas[i-1][j-1];
            double color2 = (j == 0) ? 0 : datas[i][j-1];
            double color3 = (i == width-1 || j == 0) ? 0 :  datas[i+1][j-1];
            double color4 = (i == 0) ? 0 : datas[i-1][j];
            double color5 = (i == width-1) ? 0 : datas[i+1][j];
            double color6 = (i == 0 || j == height-1) ? 0 : datas[i-1][j+1];
            double color7 = (j == height-1) ? 0 : datas[i][j+1];
            double color8 = (i == width-1 || j == height-1) ? 0 : datas[i+1][j+1];
            double color9 = datas[i][j];

            double value = 9*color9 - color4 - color5 - color1 - color2 - color3 - color6 - color7 - color8;

            if (value < 0.0) {value = 0.0;}
            else if (value > 1.0) {value = 1.0;}

            newDatas[i][j] = value;
        }
    }
    for (int i = 0; i < width; i++) {
        delete[] datas[i];
    }
    delete[] datas;

    datas = newDatas;
}
void Processor::sharpen2(double sharpFactor, int radius)
{
    Processor procCoarse(width, height, datas);
    procCoarse.medianBlur(radius);
    double** newDatas = new double*[width];
    for (int i = 0; i < width; i++) {
        newDatas[i] = new double[height];
        for (int j = 0; j < height; j++) {
            newDatas[i][j] = 0.0;
        }
    }
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            double colorSrc = datas[i][j];
            double colorCoarse = procCoarse.datas[i][j];

            double value = colorSrc + (colorSrc - colorCoarse) * sharpFactor;
            if (value < 0.0) {value = 0.0;}
            else if (value > 1.0) {value = 1.0;}

            newDatas[i][j] = value;
        }
    }

    for (int i = 0; i < width; i++) {
        delete[] datas[i];
    }
    delete[] datas;

    datas = newDatas;
}

void Processor::threshold(int limit)
{
    // -- limits
    double limitHigh = ((double)limit/255.0);
    double limitLow = limitHigh/2.0;

    // -- temp values
    double** tempValues = new double*[width];
    for (int i = 0; i < width ; i++) {
        tempValues[i] = new double[height];
    }

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            double value = 0.5;
            if (datas[i][j] > limitHigh) {value = 1.0;}
            else if (datas[i][j] < limitLow) {value = 0.0;}
            tempValues[i][j] = value;
        }
    }

    double** newDatas = new double*[width];
    for (int i = 0; i < width; i++) {
        newDatas[i] = new double[height];
        for (int j = 0; j < height; j++) {
            newDatas[i][j] = 0.0;
        }
    }
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            double value;
            int minX = i-1;
            int minY = j-1;
            int maxX = i+1;
            int maxY = j+1;

            if (tempValues[i][j] == 0.5) {

                if (minX < 0) {minX = 0;}
                else if (maxX >= width) {maxX = width-1;}
                if (minY < 0) {minY = 0;}
                else if (maxY >= height) {maxY = height-1;}

                // -- if one is 1 => pixel is 1 and we stop
                bool isOne = false;
                for (int x = minX; x <= maxX; x++) {
                    for (int y = minY; y <= maxY; y++) {

                        if (tempValues[x][y] == 1.0) {
                            isOne = true;
                            break;
                        }
                    }
                }

                if (isOne) {value = 1.0;}
                else {value = 0.0;}
            }
            else {
                value = tempValues[i][j];
            }

            newDatas[i][j] = value;
        }
    }
    for (int i = 0; i < width; i++) {
        delete[] datas[i];
    }
    delete[] datas;

    datas = newDatas;
}

void Processor::erode(int radius)
{
    double** newDatas = new double*[width];
    for (int i = 0; i < width; i++) {
        newDatas[i] = new double[height];
        for (int j = 0; j < height; j++) {
            newDatas[i][j] = 0.0;
        }
    }
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            // -- define boundaries to calculate
            int minX = i-radius;
            int maxX = i+radius;
            int minY = j-radius;
            int maxY = j+radius;

            if (minX < 0) {minX = 0;}
            if (maxX >= width) {maxX = width-1;}
            if (minY < 0) {minY = 0;}
            if (maxY >= height) {maxY = height-1;}

            // -- if one is not 1 => pixel is 0 and wee stop
            bool isOne = true;
            for (int x = minX; x <= maxX; x++) {
                for (int y = minY; y <= maxY; y++) {
                    if (datas[x][y] < 0.5) {isOne = false;}

                    // -- out condition
                    if (!isOne) {break;}
                }
            }

            double value = isOne ? 1.0 : 0.0;
            newDatas[i][j] = value;
        }
    }
    for (int i = 0; i < width; i++) {
        delete[] datas[i];
    }
    delete[] datas;

    datas = newDatas;
}
void Processor::dilate(int radius)
{
    double** newDatas = new double*[width];
    for (int i = 0; i < width; i++) {
        newDatas[i] = new double[height];
        for (int j = 0; j < height; j++) {
            newDatas[i][j] = 0.0;
        }
    }
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            // -- define boundaries to calculate
            int minX = i-radius;
            int maxX = i+radius;
            int minY = j-radius;
            int maxY = j+radius;

            if (minX < 0) {minX = 0;}
            if (maxX >= width) {maxX = width-1;}
            if (minY < 0) {minY = 0;}
            if (maxY >= height) {maxY = height-1;}

            // -- if one is 1 => pixel is 1 and we stop
            bool isOne = false;
            for (int x = minX; x <= maxX; x++) {
                for (int y = minY; y <= maxY; y++) {
                    if (datas[x][y] > 0.5) {isOne = true;}

                    // -- out condition
                    if (isOne) {break;}
                }
            }

            double value = isOne ? 1.0 : 0.0;
            newDatas[i][j] = value;
        }
    }
    for (int i = 0; i < width; i++) {
        delete[] datas[i];
    }
    delete[] datas;

    datas = newDatas;
}

void Processor::calculateRoberts()
{
    double** newDatas = new double*[width];
    for (int i = 0; i < width; i++) {
        newDatas[i] = new double[height];
        for (int j = 0; j < height; j++) {
            newDatas[i][j] = 0.0;
        }
    }
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            double color1 = datas[i][j];
            double color2 = (i == width-1 || j == height-1) ? 0 : datas[i+1][j+1];
            double color3 = (i == width-1) ? 0 : datas[i+1][j];
            double color4 = (j == height-1) ? 0 : datas[i][j+1];

            double dx = color2 - color1;
            double dy = color4 - color3;

            double value = (std::sqrt(dx*dx + dy*dy)/std::sqrt(2));
            newDatas[i][j] = value;
        }
    }
    for (int i = 0; i < width; i++) {
        delete[] datas[i];
    }
    delete[] datas;
    datas = newDatas;
}
void Processor::calculatePrewitt()
{
    double** newDatas = new double*[width];
    for (int i = 0; i < width; i++) {
        newDatas[i] = new double[height];
        for (int j = 0; j < height; j++) {
            newDatas[i][j] = 0.0;
        }
    }
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            double color1 = (i == 0 || j == 0) ? 0 : datas[i-1][j-1];
            double color2 = (j == 0) ? 0 : datas[i][j-1];
            double color3 = (i == width-1 || j == 0) ? 0 :  datas[i+1][j-1];
            double color4 = (i == 0) ? 0 : datas[i-1][j];
            double color5 = (i == width-1) ? 0 : datas[i+1][j];
            double color6 = (i == 0 || j == height-1) ? 0 : datas[i-1][j+1];
            double color7 = (j == height-1) ? 0 : datas[i][j+1];
            double color8 = (i == width-1 || j == height-1) ? 0 : datas[i+1][j+1];

            double dx = color3 + color5 + color8 - color1 - color4 - color6;
            double dy = color6 + color7 + color8 - color1 - color2 - color3;

            double value = (std::sqrt(dx*dx + dy*dy)/std::sqrt(8));
            newDatas[i][j] = value;
        }
    }
    for (int i = 0; i < width; i++) {
        delete[] datas[i];
    }
    delete[] datas;
    datas = newDatas;
}
void Processor::calculateSobel()
{
    double** newDatas = new double*[width];
    for (int i = 0; i < width; i++) {
        newDatas[i] = new double[height];
        for (int j = 0; j < height; j++) {
            newDatas[i][j] = 0.0;
        }
    }

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            double color1 = (i == 0 || j == 0) ? 0 : datas[i-1][j-1];
            double color2 = (j == 0) ? 0 : datas[i][j-1];
            double color3 = (i == width-1 || j == 0) ? 0 :  datas[i+1][j-1];
            double color4 = (i == 0) ? 0 : datas[i-1][j];
            double color5 = (i == width-1) ? 0 : datas[i+1][j];
            double color6 = (i == 0 || j == height-1) ? 0 : datas[i-1][j+1];
            double color7 = (j == height-1) ? 0 : datas[i][j+1];
            double color8 = (i == width-1 || j == height-1) ? 0 : datas[i+1][j+1];

            double dx = color3 + 2*color5 + color8 - color1 - 2*color4 - color6;
            double dy = color6 + 2*color7 + color8 - color1 - 2*color2 - color3;

            double value = (std::sqrt(dx*dx + dy*dy)/std::sqrt(18));
            newDatas[i][j] = value;
        }
    }

    for (int i = 0; i < width; i++) {
        delete[] datas[i];
    }
    delete[] datas;

    datas = newDatas;
}
void Processor::calculateKirsch()
{
    double** newDatas = new double*[width];
    for (int i = 0; i < width; i++) {
        newDatas[i] = new double[height];
        for (int j = 0; j < height; j++) {
            newDatas[i][j] = 0.0;
        }
    }
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            double color1 = (i == 0 || j == 0) ? 0 : datas[i-1][j-1];
            double color2 = (j == 0) ? 0 : datas[i][j-1];
            double color3 = (i == width-1 || j == 0) ? 0 :  datas[i+1][j-1];
            double color4 = (i == 0) ? 0 : datas[i-1][j];
            double color5 = (i == width-1) ? 0 : datas[i+1][j];
            double color6 = (i == 0 || j == height-1) ? 0 : datas[i-1][j+1];
            double color7 = (j == height-1) ? 0 : datas[i][j+1];
            double color8 = (i == width-1 || j == height-1) ? 0 : datas[i+1][j+1];

            double d1 = 5*color1 + 5*color2 + 5*color3
            - 3*color4 - 3*color5 - 3*color6 
            - 3*color7 - 3*color8;
            double d2 = 5*color1 + 5*color2 - 3*color3
            + 5*color4 - 3*color5 - 3*color6 
            - 3*color7 - 3*color8;
            double d3 = 5*color1 - 3*color2 - 3*color3
            + 5*color4 - 3*color5 + 5*color6 
            - 3*color7 - 3*color8;
            double d4 = -3*color1 - 3*color2 - 3*color3
            + 5*color4 - 3*color5 + 5*color6 
            + 5*color7 - 3*color8;
            double d5 = -3*color1 - 3*color2 - 3*color3
            - 3*color4 - 3*color5 + 5*color6 
            + 5*color7 + 5*color8;
            double d6 = -3*color1 - 3*color2 - 3*color3
            - 3*color4 + 5*color5 - 3*color6 
            + 5*color7 + 5*color8;
            double d7 = -3*color1 - 3*color2 + 5*color3
            - 3*color4 + 5*color5 - 3*color6 
            - 3*color7 + 5*color8;
            double d8 = -3*color1 + 5*color2 + 5*color3
            - 3*color4 + 5*color5 - 3*color6 
            - 3*color7 - 3*color8;

            double value = (std::sqrt(d1*d1 + d2*d2 + d3*d3 + d4*d4 + d5*d5 + d6*d6 + d7*d7 + d8*d8)/15);
            newDatas[i][j] = value;
        }
    }
    for (int i = 0; i < width; i++) {
        delete[] datas[i];
    }
    delete[] datas;
    datas = newDatas;
}
void Processor::calculateSobelRGB()
{
    double** newDatasR = new double*[width];
    double** newDatasG = new double*[width];
    double** newDatasB = new double*[width];
    for (int i = 0; i < width; i++) {
        newDatasR[i] = new double[height];
        newDatasG[i] = new double[height];
        newDatasB[i] = new double[height];
        for (int j = 0; j < height; j++) {
            newDatasR[i][j] = 0.0;
            newDatasG[i][j] = 0.0;
            newDatasB[i][j] = 0.0;
        }
    }

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            double color1 = (i == 0 || j == 0) ? 0 : datasR[i-1][j-1];
            double color2 = (j == 0) ? 0 : datasR[i][j-1];
            double color3 = (i == width-1 || j == 0) ? 0 :  datasR[i+1][j-1];
            double color4 = (i == 0) ? 0 : datasR[i-1][j];
            double color5 = (i == width-1) ? 0 : datasR[i+1][j];
            double color6 = (i == 0 || j == height-1) ? 0 : datasR[i-1][j+1];
            double color7 = (j == height-1) ? 0 : datasR[i][j+1];
            double color8 = (i == width-1 || j == height-1) ? 0 : datasR[i+1][j+1];

            double dx = color3 + 2*color5 + color8 - color1 - 2*color4 - color6;
            double dy = color6 + 2*color7 + color8 - color1 - 2*color2 - color3;

            double value = (std::sqrt(dx*dx + dy*dy)/std::sqrt(18));
            newDatasR[i][j] = value;
        }
    }
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            double color1 = (i == 0 || j == 0) ? 0 : datasB[i-1][j-1];
            double color2 = (j == 0) ? 0 : datasB[i][j-1];
            double color3 = (i == width-1 || j == 0) ? 0 :  datasB[i+1][j-1];
            double color4 = (i == 0) ? 0 : datasB[i-1][j];
            double color5 = (i == width-1) ? 0 : datasB[i+1][j];
            double color6 = (i == 0 || j == height-1) ? 0 : datasB[i-1][j+1];
            double color7 = (j == height-1) ? 0 : datasB[i][j+1];
            double color8 = (i == width-1 || j == height-1) ? 0 : datasB[i+1][j+1];

            double dx = color3 + 2*color5 + color8 - color1 - 2*color4 - color6;
            double dy = color6 + 2*color7 + color8 - color1 - 2*color2 - color3;

            double value = (std::sqrt(dx*dx + dy*dy)/std::sqrt(18));
            newDatasB[i][j] = value;
        }
    }
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            double color1 = (i == 0 || j == 0) ? 0 : datasG[i-1][j-1];
            double color2 = (j == 0) ? 0 : datasG[i][j-1];
            double color3 = (i == width-1 || j == 0) ? 0 :  datasG[i+1][j-1];
            double color4 = (i == 0) ? 0 : datasG[i-1][j];
            double color5 = (i == width-1) ? 0 : datasG[i+1][j];
            double color6 = (i == 0 || j == height-1) ? 0 : datasG[i-1][j+1];
            double color7 = (j == height-1) ? 0 : datasG[i][j+1];
            double color8 = (i == width-1 || j == height-1) ? 0 : datasG[i+1][j+1];

            double dx = color3 + 2*color5 + color8 - color1 - 2*color4 - color6;
            double dy = color6 + 2*color7 + color8 - color1 - 2*color2 - color3;

            double value = (std::sqrt(dx*dx + dy*dy)/std::sqrt(18));
            newDatasG[i][j] = value;
        }
    }

    for (int i = 0; i < width; i++) {
        delete[] datasR[i];
        delete[] datasG[i];
        delete[] datasB[i];
    }
    delete[] datasR;
    delete[] datasG;
    delete[] datasB;

    datasR = newDatasR;
    datasG = newDatasG;
    datasB = newDatasB;
}

void Processor::contourTracing()
{
    // -- clean between execution
    pointsTaken.clear();
    for (int i = 0; i < contours.size(); i++) {
        delete contours.at(i);
    }
    contours.clear();

    // -- border of image not taken into account
    for (int i = 1; i < width-2; i++) {
        for (int j = 1; j < height-2; j++) {

            int valueMap = j*width + i;

            // -- points is already own by another contour
            if (pointsTaken.find(valueMap) != pointsTaken.end()) {continue;}

            // -- check start condition for taking this pixel
            // -- taking all possible orientation
            bool startPixel = false;
            int directionRetained = -1;
            for (int dir = 0; dir <= 6; dir+=2) {
                if (isStartPixel(i, j, dir)) {
                    startPixel = true;
                    directionRetained = dir;
                    break;
                }
            }

            if (startPixel) {
                Contour* contour = new Contour();
                detectContour(contour, i, j, directionRetained);
                contours.push_back(contour);
            }
        }
    }

    // -- draw contours
    double** newDatas = new double*[width];
    for (int i = 0; i < width; i++) {
        newDatas[i] = new double[height];
        for (int j = 0; j < height; j++) {
            newDatas[i][j] = 0.0;
        }
    }
    for (int i = 0; i < contours.size(); i++) {
        
        // -- random color
        double red = (std::rand() % 100) + 150;
        red /= 255.0;

        Contour* contour = contours.at(i);
        for (int j = 0; j < contour->points.size(); j++) {
            newDatas[contour->points.at(j)->x][contour->points.at(j)->y] = red;
        }
    }
    for (int i = 0; i < width; i++) {
        delete[] datas[i];
    }
    delete[] datas;
    datas = newDatas;
}
bool Processor::isStartPixel(int x, int y, int direction)
{
    // -- check if start pixel is black; otherwise not taken
    double colorCenter = datas[x][y];
    if (colorCenter != 1.0) {return false;}

    // -- check if rear is white; otherwise not taken
    double colorRear = getRear(x, y, direction);
    if (colorRear != 0.0) {return false;}

    // -- check if not left-rear inner-outer corner; otherwise not taken
    double colorLeft = getLeft(x, y, direction);
    double colorRearLeft = getRearLeft(x, y, direction);
    if (colorLeft == 0.0 && colorRearLeft == 1.0) {
        return false;
    }

    return true;
}
void Processor::detectContour(Contour* contour, int startX, int startY, int startDir)
{
    // -- init tracer
    int tracerX = startX;
    int tracerY = startY;
    int direction = startDir;

    // -- add first point of contour
    ContourPoint* pointRef = createContourPointCenter(startX, startY, startDir, startDir);
    pointRef->direction = startDir;
    contour->points.push_back(pointRef);
    pointsTaken.insert_or_assign(tracerY*width+tracerX, nullptr);

    // -- loop
    do {
        // -- state 0
        double left = getLeft(tracerX, tracerY, direction);
        
        int stage1X = -1;
        int stage1Y = -1;
        int stage1Direction = -1;

        // -- state 1
        if (left == 1.0) {
            ContourPoint* point = createContourPointLeft(tracerX, tracerY, direction, 2);
            stage1X = point->x;
            stage1Y = point->y;
            stage1Direction = point->direction;
            contour->points.push_back(point);
            //Poco::Logger::get("Processor").debug("State 1 : New point at " + std::to_string(tracerX) + "," + std::to_string(tracerY) + " ; direction : " + std::to_string(direction), __FILE__, __LINE__);
            pointsTaken.insert_or_assign(tracerY*width+tracerX, nullptr);
        }

        // -- state 2 : NOTHING TO DO
        else {}

        // -- state 1 / 2
        double rearLeft = getRearLeft(tracerX, tracerY, direction);

        // -- state 3
        if (rearLeft == 1.0) {
            ContourPoint* point = createContourPointRearLeft(tracerX, tracerY, direction, 0);
            stage1X = point->x;
            stage1Y = point->y;
            stage1Direction = point->direction;
            contour->points.push_back(point);
            //Poco::Logger::get("Processor").debug("State 3 : New point at " + std::to_string(tracerX) + "," + std::to_string(tracerY) + " ; direction : " + std::to_string(direction), __FILE__, __LINE__);
            pointsTaken.insert_or_assign(tracerY*width+tracerX, nullptr);
        }

        // -- state 4 / 5 : NOTHING TO DO
        else {}

        // -- END STAGE 1
        if (stage1X != -1) {
            tracerX = stage1X;
            tracerY = stage1Y;
            direction = stage1Direction;
        }

        // -- START STAGE 2
        int stage2X = -1;
        int stage2Y = -1;
        int stage2Direction = -1;

        // -- state 5
        double front = getFront(tracerX, tracerY, direction);

        // -- state 6
        if (front == 1.0) {
            double frontLeft = getFrontLeft(tracerX, tracerY, direction);

            // -- state 8
            if (frontLeft == 1.0) {
                ContourPoint* point = createContourPointFront(tracerX, tracerY, direction, 2);
                contour->points.push_back(point);
                //Poco::Logger::get("Processor").debug("State 8 : New point at " + std::to_string(tracerX) + "," + std::to_string(tracerY) + " ; direction : " + std::to_string(direction), __FILE__, __LINE__);
                pointsTaken.insert_or_assign(tracerY*width+tracerX, nullptr);

                ContourPoint* point2 = createContourPointFrontLeft(tracerX, tracerY, direction, 4);
                stage2X = point2->x;
                stage2Y = point2->y;
                stage2Direction = point2->direction;
                contour->points.push_back(point2);
                //Poco::Logger::get("Processor").debug("State 8 : New point at " + std::to_string(tracerX) + "," + std::to_string(tracerY) + " ; direction : " + std::to_string(direction), __FILE__, __LINE__);
                pointsTaken.insert_or_assign(tracerY*width+tracerX, nullptr);
            }

            // -- state 7
            else {
                ContourPoint* point = createContourPointFront(tracerX, tracerY, direction, 6);
                stage2X = point->x;
                stage2Y = point->y;
                stage2Direction = point->direction;
                contour->points.push_back(point);
                //Poco::Logger::get("Processor").debug("State 7 : New point at " + std::to_string(tracerX) + "," + std::to_string(tracerY) + " ; direction : " + std::to_string(direction), __FILE__, __LINE__);
                pointsTaken.insert_or_assign(tracerY*width+tracerX, nullptr);
            }
        }

        // -- state 9
        else {
            double frontLeft = getFrontLeft(tracerX, tracerY, direction);

            // -- state 10
            if (frontLeft == 1.0) {
                ContourPoint* point = createContourPointFrontLeft(tracerX, tracerY, direction, 4);
                stage2X = point->x;
                stage2Y = point->y;
                stage2Direction = point->direction;
                contour->points.push_back(point);
                //Poco::Logger::get("Processor").debug("State 10 : New point at " + std::to_string(tracerX) + "," + std::to_string(tracerY) + " ; direction : " + std::to_string(direction), __FILE__, __LINE__);
                pointsTaken.insert_or_assign(tracerY*width+tracerX, nullptr);
            }

            // -- state 11
            else {
                ContourPoint* point = createContourPointCenter(tracerX, tracerY, direction, 0);
                stage2X = point->x;
                stage2Y = point->y;
                stage2Direction = point->direction;
                contour->points.push_back(point);
                //Poco::Logger::get("Processor").debug("State 11 : New point at " + std::to_string(tracerX) + "," + std::to_string(tracerY) + " ; direction : " + std::to_string(direction), __FILE__, __LINE__);
                pointsTaken.insert_or_assign(tracerY*width+tracerX, nullptr);
            }
        }

        // -- END STAGE 2
        if (stage2X != -1) {
            tracerX = stage2X;
            tracerY = stage2Y;
            direction = stage2Direction;
        }

    } while (tracerX != startX || tracerY != startY || direction != startDir);
}

double Processor::getRear(int x, int y, int direction)
{
    // -- REAR
    if (direction == 0) {
        return datas[x][y-1];
    }
    // -- LEFT
    else if (direction == 2) {
        return datas[x+1][y];
    }
    // -- FRONT
    else if (direction == 4) {
        return datas[x][y+1];
    }
    // -- RIGHT
    else if (direction == 6) {
        return datas[x-1][y];
    }

    return 0;
}
double Processor::getRearLeft(int x, int y, int direction)
{
    // -- REAR
    if (direction == 0) {
        return datas[x+1][y-1];
    }
    // -- LEFT
    else if (direction == 2) {
        return datas[x+1][y+1];
    }
    // -- FRONT
    else if (direction == 4) {
        return datas[x-1][y+1];
    }
    // -- RIGHT
    else if (direction == 6) {
        return datas[x-1][y-1];
    }

    return 0;
}
double Processor::getLeft(int x, int y, int direction)
{
    // -- REAR
    if (direction == 0) {
        return datas[x+1][y];
    }
    // -- LEFT
    else if (direction == 2) {
        return datas[x][y+1];
    }
    // -- FRONT
    else if (direction == 4) {
        return datas[x-1][y];
    }
    // -- RIGHT
    else if (direction == 6) {
        return datas[x][y-1];
    }

    return 0;
}
double Processor::getFrontLeft(int x, int y, int direction)
{
    // -- REAR
    if (direction == 0) {
        return datas[x+1][y+1];
    }
    // -- LEFT
    else if (direction == 2) {
        return datas[x-1][y+1];
    }
    // -- FRONT
    else if (direction == 4) {
        return datas[x-1][y-1];
    }
    // -- RIGHT
    else if (direction == 6) {
        return datas[x+1][y-1];
    }

    return 0;
}
double Processor::getFront(int x, int y, int direction)
{
    // -- REAR
    if (direction == 0) {
        return datas[x][y+1];
    }
    // -- LEFT
    else if (direction == 2) {
        return datas[x-1][y];
    }
    // -- FRONT
    else if (direction == 4) {
        return datas[x][y-1];
    }
    // -- RIGHT
    else if (direction == 6) {
        return datas[x+1][y];
    }

    return 0;
}

Processor::ContourPoint* Processor::createContourPointLeft(int x, int y, int startDir, int newDir)
{
    ContourPoint* point = new ContourPoint();
    point->direction = newDir;

    // -- REAR
    if (startDir == 0) {
        point->x = x+1;
        point->y = y;
        point->direction = (newDir+4) % 8;
    }

    // -- LEFT
    else if (startDir == 2) {
        point->x = x;
        point->y = y+1;
        point->direction = newDir-2;
        if (point->direction < 0) {point->direction += 8;}
    }

    // -- FRONT
    else if (startDir == 4) {
        point->x = x-1;
        point->y = y;
        point->direction = newDir;
    }

    // -- RIGHT
    else if (startDir == 6) {
        point->x = x;
        point->y = y-1;
        point->direction = newDir+2;
        if (point->direction >= 8) {point->direction -= 8;}
    }

    return point;
}
Processor::ContourPoint* Processor::createContourPointRearLeft(int x, int y, int startDir, int newDir)
{
    ContourPoint* point = new ContourPoint();
    point->direction = newDir;

    // -- REAR
    if (startDir == 0) {
        point->x = x+1;
        point->y = y-1;
        point->direction = (newDir+4) % 8;
    }

    // -- LEFT
    else if (startDir == 2) {
        point->x = x+1;
        point->y = y+1;
        point->direction = newDir-2;
        if (point->direction < 0) {point->direction += 8;}
    }

    // -- FRONT
    else if (startDir == 4) {
        point->x = x-1;
        point->y = y+1;
        point->direction = newDir;
    }

    // -- RIGHT
    else if (startDir == 6) {
        point->x = x-1;
        point->y = y-1;
        point->direction = newDir+2;
        if (point->direction >= 8) {point->direction -= 8;}
    }

    return point;
}
Processor::ContourPoint* Processor::createContourPointFrontLeft(int x, int y, int startDir, int newDir)
{
    ContourPoint* point = new ContourPoint();
    point->direction = newDir;

    // -- REAR
    if (startDir == 0) {
        point->x = x+1;
        point->y = y+1;
        point->direction = (newDir+4) % 8;
    }

    // -- LEFT
    else if (startDir == 2) {
        point->x = x-1;
        point->y = y+1;
        point->direction = newDir-2;
        if (point->direction < 0) {point->direction += 8;}
    }

    // -- FRONT
    else if (startDir == 4) {
        point->x = x-1;
        point->y = y-1;
        point->direction = newDir;
    }

    // -- RIGHT
    else if (startDir == 6) {
        point->x = x+1;
        point->y = y-1;
        point->direction = newDir+2;
        if (point->direction >= 8) {point->direction -= 8;}
    }

    return point;
}
Processor::ContourPoint* Processor::createContourPointFront(int x, int y, int startDir, int newDir)
{
    ContourPoint* point = new ContourPoint();
    point->direction = newDir;

    // -- REAR
    if (startDir == 0) {
        point->x = x;
        point->y = y+1;
        point->direction = (newDir+4) % 8;
    }

    // -- LEFT
    else if (startDir == 2) {
        point->x = x-1;
        point->y = y;
        point->direction = newDir-2;
        if (point->direction < 0) {point->direction += 8;}
    }

    // -- FRONT
    else if (startDir == 4) {
        point->x = x;
        point->y = y-1;
        point->direction = newDir;
    }

    // -- RIGHT
    else if (startDir == 6) {
        point->x = x+1;
        point->y = y;
        point->direction = newDir+2;
        if (point->direction >= 8) {point->direction -= 8;}
    }

    return point;
}
Processor::ContourPoint* Processor::createContourPointCenter(int x, int y, int startDir, int newDir)
{
    ContourPoint* point = new ContourPoint();
    point->x = x;
    point->y = y;

    // -- REAR
    if (startDir == 0) {
        point->direction = (newDir+4) % 8;
    }

    // -- LEFT
    else if (startDir == 2) {
        point->direction = newDir-2;
        if (point->direction < 0) {point->direction += 8;}
    }

    // -- FRONT
    else if (startDir == 4) {
        point->direction = newDir;
    }

    // -- RIGHT
    else if (startDir == 6) {
        point->direction = newDir+2;
        if (point->direction >= 8) {point->direction -= 8;}
    }

    return point;
}

void Processor::simplifyContour(int nbPoints)
{
    // -- clean last execution
    for (int i = 0; i < contourSimplified.size(); i++) {
        delete contourSimplified.at(i);
    }
    contourSimplified.clear();

    // -- calculate new simplified contours
    for (int i = 0; i < contours.size(); i++) {
        polyLineSimplification(contours.at(i), nbPoints);
    }

    // -- draw in new image
    double** newDatas = new double*[width];
    for (int i = 0; i < width; i++) {
        newDatas[i] = new double[height];
        for (int j = 0; j < height; j++) {
            newDatas[i][j] = 0.0;
        }
    }
    for (int i = 0; i < contourSimplified.size(); i++) {
        
        // -- random color
        double red = (std::rand() % 100) + 150;
        red /= 255.0;

        Contour* contour = contourSimplified.at(i);
        for (int j = 0; j < contour->points.size(); j++) {
            newDatas[contour->points.at(j)->x][contour->points.at(j)->y] = red;
        }
    }
    for (int i = 0; i < width; i++) {
        delete[] datas[i];
    }
    delete[] datas;
    datas = newDatas;
}
void Processor::polyLineSimplification(Contour* contour, int nbPoints)
{
    Poco::Logger::get("Processor").debug("Treat contour", __FILE__, __LINE__);

    int lenContour = contour->points.size();
    double* weights = new double[lenContour];

    Poco::Logger::get("Processor").debug("Contour Size : " + std::to_string(lenContour), __FILE__, __LINE__);

    douglasPeucker(contour, weights, 0, lenContour-1);
    weights[0] = DBL_MAX;
    weights[lenContour-1] = DBL_MAX;

    // -- sort weights to determine max tolerence for keeping points
    std::vector<double> weightArray;
    Poco::Logger::get("Processor").debug("WEIGHTS", __FILE__, __LINE__);
    for (int i = 0; i < lenContour; i++) {
        Poco::Logger::get("Processor").debug(std::to_string(weights[i]), __FILE__, __LINE__);
        weightArray.push_back(weights[i]);
    }
    std::sort(weightArray.begin(), weightArray.end(), std::greater<double>());
    double maxTolerence = weightArray.at(nbPoints-1);
    Poco::Logger::get("Processor").debug("Max tolerence : " + std::to_string(maxTolerence), __FILE__, __LINE__);

    // -- create new simplified contour
    Contour* newContour = new Contour();
    Poco::Logger::get("Processor").debug("WEIGHTS", __FILE__, __LINE__);
    for (int i = 0; i < lenContour; i++) {
        Poco::Logger::get("Processor").debug(std::to_string(weights[i]), __FILE__, __LINE__);
        if (weights[i] >= maxTolerence) {
            Poco::Logger::get("Processor").debug("KEEP : " + std::to_string(contour->points.at(i)->x) + ", " + std::to_string(contour->points.at(i)->y), __FILE__, __LINE__);
            newContour->points.push_back(contour->points.at(i));
        }
    }
    contourSimplified.push_back(newContour);
}
void Processor::douglasPeucker(Contour* contour, double* weights, int startIdx, int endIdx)
{
    if (endIdx > startIdx + 1) {
        double maxDist = -1;
        int maxDistIdx = 0;
        for (int i = startIdx+1 ; i < endIdx; i++) {
            double dist = getDistancePolyline(contour, startIdx, endIdx, i);

            if (dist > maxDist) {
                maxDist = dist;
                maxDistIdx = i;
            }
        }

        weights[maxDistIdx] = maxDist;

        douglasPeucker(contour, weights, startIdx, maxDistIdx);
        douglasPeucker(contour, weights, maxDistIdx, endIdx);
    }
}
double Processor::getDistancePolyline(Contour* contour, int startIdx, int endIdx, int currentIdx)
{
    Processor::ContourPoint* p1 = contour->points.at(startIdx);
    Processor::ContourPoint* p2 = contour->points.at(endIdx);
    Processor::ContourPoint* pC = contour->points.at(currentIdx);

    double lengthSquared = (p1->x - p2->x)*(p1->x - p2->x) + (p1->y - p2->y)*(p1->y - p2->y);
    
    // -- calculate ratio
    double ratio = 0.0;
    if (lengthSquared == 0.0) {
        ratio = (p1->x - pC->x)*(p1->x - pC->x) + (p1->y - pC->y)*(p1->y - pC->y);
    }
    else {
        ratio = ((pC->x - p1->x)*(p2->x - p1->x) + (pC->y - p1->y)*(p2->y - p1->y)) / lengthSquared;
    }

    // -- calculate distance final
    if (ratio < 0.0) {
        return (p1->x - pC->x)*(p1->x - pC->x) + (p1->y - pC->y)*(p1->y - pC->y);
    }
    else if (ratio > 1.0) {
        return (p2->x - pC->x)*(p2->x - pC->x) + (p2->y - pC->y)*(p2->y - pC->y);
    }
    else {
        double xF = p1->x + ratio*(p2->x - p1->x);
        double yF = p1->y + ratio*(p2->y - p1->y);

        return (xF - pC->x)*(xF - pC->x) + (yF - pC->y)*(yF - pC->y);
    }
}

Image* Processor::seamCarvingRemove(Image* refImg)
{
    calculateSobelRGB();
    seamCarvingDetectRGB();

    int idxRemove = 0;
    double seamMinimal = 500.0;

    // -- detect seam to remove
    for (int i = 0; i < seams.size(); i++) {
        if (seams.at(i)->energy < seamMinimal) {
            idxRemove = i;
            seamMinimal = seams.at(i)->energy;
        }
    }

    Seam* seamToRemove = seams.at(idxRemove);
    Image* newImage = new Image(refImg->getWidth() - 1, refImg->getHeight(), PNG_FORMAT_RGB, 0, 0, 0);
    for (int i = 0; i < refImg->getWidth(); i++) {
        for (int j = 0; j < refImg->getHeight(); j++) {

            // -- ignore
            if (i == seamToRemove->points.at(j)->x) {
                continue;
            }
            else if (i < seamToRemove->points.at(j)->x) {
                Color* color = refImg->getValueAt(i, j);
                newImage->setValueAt(i, j, *color);
                delete color;
            }
            else {
                Color* color = refImg->getValueAt(i, j);
                newImage->setValueAt(i-1, j, *color);
                delete color;
            }
        }
    }
    delete refImg;
    return newImage;
}
Image* Processor::seamCarvingAdd(Image* refImg)
{
    calculateSobelRGB();
    seamCarvingDetectRGB();

    int idxAdd = 0;
    double seamMinimal = 500.0;

    // -- detect seam to remove
    bool leftAdd = false;
    for (int i = 0; i < seams.size(); i++) {
        if (seams.at(i)->energy < seamMinimal) {
            idxAdd = i;
            seamMinimal = seams.at(i)->energy;
        }
    }

    if (idxAdd > 0) {
        double energyLeft = seams.at(idxAdd-1)->energy;
        double energyRight = seams.at(idxAdd+1)->energy;

        if (energyRight < energyLeft) {
            leftAdd = true;
        }
    }

    Seam* seamToAdd = seams.at(idxAdd);
    Image* newImage = new Image(refImg->getWidth() + 1, refImg->getHeight(), PNG_FORMAT_RGB, 0, 0, 0);
    for (int i = 0; i < refImg->getWidth(); i++) {
        for (int j = 0; j < refImg->getHeight(); j++) {

            if (!leftAdd) {
                if (i <= seamToAdd->points.at(j)->x) {
                    Color* color = refImg->getValueAt(i, j);
                    newImage->setValueAt(i, j, *color);
                    delete color;
                }
                else if (i > seamToAdd->points.at(j)->x) {
                    Color* color = refImg->getValueAt(i, j);
                    newImage->setValueAt(i+1, j, *color);
                    delete color;
                }
            }
            else {
                if (i < seamToAdd->points.at(j)->x) {
                    Color* color = refImg->getValueAt(i, j);
                    newImage->setValueAt(i, j, *color);
                    delete color;
                }
                else if (i >= seamToAdd->points.at(j)->x) {
                    Color* color = refImg->getValueAt(i, j);
                    newImage->setValueAt(i+1, j, *color);
                    delete color;
                }
            }
        }
    }

    if (!leftAdd) {
        for (int j = 0; j < newImage->getHeight(); j++) {

            int point = seamToAdd->points.at(j)->x;
            Color* colorLeft = newImage->getValueAt(point, j);
            Color* colorRight = newImage->getValueAt(point+2, j);
            if (colorLeft == nullptr) {
                colorLeft = new Color();
                colorLeft->setRGB(0, 0, 0);
            }
            if (colorRight == nullptr) {
                colorRight = new Color();
                colorRight->setRGB(0, 0, 0);
            }
            Color temp;
            temp.setRGB((colorLeft->getRed() + colorRight->getRed())/2.0, 
                        (colorLeft->getGreen() + colorRight->getGreen())/2.0, 
                        (colorLeft->getBlue() + colorRight->getBlue())/2.0);
            newImage->setValueAt(point+1, j, temp);
            delete colorLeft;
            delete colorRight;
        }
    }
    else {
        for (int j = 0; j < newImage->getHeight(); j++) {

            int point = seamToAdd->points.at(j)->x;
            Color* colorLeft = newImage->getValueAt(point-1, j);
            Color* colorRight = newImage->getValueAt(point+1, j);
            if (colorLeft == nullptr) {
                colorLeft = new Color();
                colorLeft->setRGB(0, 0, 0);
            }
            if (colorRight == nullptr) {
                colorRight = new Color();
                colorRight->setRGB(0, 0, 0);
            }
            Color temp;
            temp.setRGB((colorLeft->getRed() + colorRight->getRed())/2.0, 
                        (colorLeft->getGreen() + colorRight->getGreen())/2.0, 
                        (colorLeft->getBlue() + colorRight->getBlue())/2.0);
            newImage->setValueAt(point, j, temp);
            delete colorLeft;
            delete colorRight;
        }
    }

    delete refImg;
    return newImage;
}
Image* Processor::seamCarvingRemoveHigh(Image* refImg)
{
    calculateSobel();
    seamCarvingDetect();

    int idxRemove = 0;
    double seamMinimal = 0.0;

    // -- detect seam to remove
    for (int i = 0; i < seams.size(); i++) {
        if (seams.at(i)->energy > seamMinimal) {
            idxRemove = i;
            seamMinimal = seams.at(i)->energy;
        }
    }

    Seam* seamToRemove = seams.at(idxRemove);
    Image* newImage = new Image(refImg->getWidth() - 1, refImg->getHeight(), PNG_FORMAT_RGB, 0, 0, 0);
    for (int i = 0; i < refImg->getWidth(); i++) {
        for (int j = 0; j < refImg->getHeight(); j++) {

            // -- ignore
            if (i == seamToRemove->points.at(j)->x) {
                continue;
            }
            else if (i < seamToRemove->points.at(j)->x) {
                Color* color = refImg->getValueAt(i, j);
                newImage->setValueAt(i, j, *color);
                delete color;
            }
            else {
                Color* color = refImg->getValueAt(i, j);
                newImage->setValueAt(i-1, j, *color);
                delete color;
            }
        }
    }
    delete refImg;
    return newImage;
}

/*Image* Processor::seamCarvingAddToLeft(Image* refImg, int newWidth)
{
    std::vector<Seam*> seamsToAdd;

    // -- first get all deletion seams
    int window = 360 * refImg->getWidth() / 1024;
    Image* tempDelete = new Image(refImg);
    int nbToDelete = newWidth - width;
    std::cout << "Delete left " << nbToDelete << std::endl;
    for (int d = 0; d < nbToDelete; d++) {

        std::cout << "Try delete left " << d << ", window (" << 0 << ", " << window << ")" << std::endl;

        Processor tempDeleteProc(tempDelete);
        tempDeleteProc.calculateSobelRGB();
        tempDeleteProc.seamCarvingDetectRGB();

        int idxRemove = 0;
        double seamMinimal = 500.0;

        // -- detect seam to remove
        for (int j = 0; j < window; j++) {
            if (tempDeleteProc.seams.at(j)->energy <= seamMinimal) {
                idxRemove = j;
                seamMinimal = tempDeleteProc.seams.at(j)->energy;
            }
        }

        window--;

        // -- remove actually form temporary image
        Seam* seamToRemove = tempDeleteProc.seams.at(idxRemove);
        Image* newImage = new Image(tempDelete->getWidth() - 1, tempDelete->getHeight(), PNG_FORMAT_RGB, 0, 0, 0);
        for (int i = 0; i < tempDelete->getWidth(); i++) {
            for (int j = 0; j < tempDelete->getHeight(); j++) {

                // -- ignore
                if (i == seamToRemove->points.at(j)->x) {
                    continue;
                }
                else if (i < seamToRemove->points.at(j)->x) {
                    Color* color = tempDelete->getValueAt(i, j);
                    newImage->setValueAt(i, j, *color);
                    delete color;
                }
                else {
                    Color* color = tempDelete->getValueAt(i, j);
                    newImage->setValueAt(i-1, j, *color);
                    delete color;
                }
            }
        }

        Seam* toAdd = new Seam();
        for (Point* p : seamToRemove->points) {
            Point* nPoint = new Point();
            nPoint->x = p->x;
            nPoint->y = p->y;
            toAdd->points.push_back(nPoint);
        }
        seamsToAdd.push_back(toAdd);

        delete tempDelete;
        tempDelete = newImage;
    }

    // -- replace seam to reel point in image reference
    for (int s = 0 ; s < seamsToAdd.size(); s++) {
        for (int ss = s+1 ; ss < seamsToAdd.size(); ss++) {
            Seam* ref = seamsToAdd.at(s);
            Seam* move = seamsToAdd.at(ss);

            for (int j = 0; j < refImg->getHeight(); j++) {
                if (move->points.at(j)->x >= ref->points.at(j)->x) {
                    move->points.at(j)->x++;
                }
            }
        }
    }

    int size = refImg->getWidth() + 1;
    Image* addedImage = new Image(refImg);

    for (int s = 0 ; s < seamsToAdd.size(); s++) {

        Seam* seamToAdd = seamsToAdd.at(s);
        std::cout << "Try add left " << s << " at " << seamToAdd->points.at(0)->x << " - " << 
        seamToAdd->points.at(seamToAdd->points.size()-1)->x << std::endl;
        Image* newImage = new Image(size, addedImage->getHeight(), PNG_FORMAT_RGB, 0, 0, 0);
        for (int i = 0; i < addedImage->getWidth(); i++) {
            for (int j = 0; j < addedImage->getHeight(); j++) {

                Color* color = addedImage->getValueAt(i, j);
                if (i <= seamToAdd->points.at(j)->x) {
                    newImage->setValueAt(i, j, *color);
                }
                else if (i > seamToAdd->points.at(j)->x) {
                    newImage->setValueAt(i+1, j, *color);
                }
                delete color;
            }
        }
        size++;

        for (int j = 0; j < newImage->getHeight(); j++) {

            int point = seamToAdd->points.at(j)->x;
            Color* colorLeft = newImage->getValueAt(point, j);
            Color* colorRight = newImage->getValueAt(point+2, j);

            if (colorLeft == nullptr) {
                colorLeft = new Color();
                colorLeft->setRGB(0, 0, 0);
            }
            if (colorRight == nullptr) {
                colorRight = new Color();
                colorRight->setRGB(0, 0, 0);
            }
            Color temp;
            double red = (colorLeft->getRed() + colorRight->getRed()) / 2.0;
            double green = (colorLeft->getGreen() + colorRight->getGreen()) / 2.0;
            double blue = (colorLeft->getBlue() + colorRight->getBlue()) / 2.0;
                    
            temp.setRGB(red, green, blue);
            newImage->setValueAt(point+1, j, temp);
            delete colorLeft;
            delete colorRight;
        }

        for (int ss = s+1; ss < seamsToAdd.size(); ss++) {
            Seam* seamToMove = seamsToAdd.at(ss);
            for (int j = 0; j < newImage->getHeight(); j++) {
                if (seamToMove->points.at(j)->x >= seamToAdd->points.at(j)->x)
                {
                    seamToMove->points.at(j)->x++;
                }
            }
        }
    
        delete addedImage;
        addedImage = newImage;
    }

    return addedImage;
}*/
Image* Processor::seamCarvingAddToLeft(Image* refImg, int newWidth)
{
    std::vector<Seam*> seamsToAdd;

    // -- first get all deletion seams
    int window = 360 * refImg->getWidth() / 1024;
    Image* tempDelete = new Image(refImg);
    int nbToDelete = newWidth - width;
    std::cout << "Delete left " << nbToDelete << std::endl;
    Processor tempDeleteProc(tempDelete);
    tempDeleteProc.calculateSobelRGB();
    for (int d = 0; d < nbToDelete; d++) {

        std::cout << "Try delete left " << d << std::endl;
        tempDeleteProc.seamCarvingDetectRGB();

        int idxRemove = 0;
        double seamMinimal = 500.0;

        // -- detect seam to remove
        for (int j = 0; j < window; j++) {
            if (tempDeleteProc.seams.at(j)->energy <= seamMinimal) {
                idxRemove = j;
                seamMinimal = tempDeleteProc.seams.at(j)->energy;
            }
        }
        window--;

        // -- remove actually form temporary image
        Seam* seamToRemove = tempDeleteProc.seams.at(idxRemove);
        Seam* toAdd = new Seam();
        for (Point* p : seamToRemove->points) {
            Point* nPoint = new Point();
            nPoint->x = p->x;
            nPoint->y = p->y;
            toAdd->points.push_back(nPoint);
        }
        seamsToAdd.push_back(toAdd);
        tempDeleteProc.removeSeam(toAdd);
    }

    // -- replace seam to reel point in image reference
    for (int s = 0 ; s < seamsToAdd.size(); s++) {
        for (int ss = s+1 ; ss < seamsToAdd.size(); ss++) {
            Seam* ref = seamsToAdd.at(s);
            Seam* move = seamsToAdd.at(ss);

            for (int j = 0; j < refImg->getHeight(); j++) {
                if (move->points.at(j)->x >= ref->points.at(j)->x) {
                    move->points.at(j)->x++;
                }
            }
        }
    }

    int size = refImg->getWidth() + 1;
    Image* addedImage = new Image(refImg);

    for (int s = 0 ; s < seamsToAdd.size(); s++) {

        Seam* seamToAdd = seamsToAdd.at(s);
        std::cout << "Try add left " << s << " at " << seamToAdd->points.at(0)->x << " - " << 
        seamToAdd->points.at(seamToAdd->points.size()-1)->x << std::endl;
        Image* newImage = new Image(size, addedImage->getHeight(), PNG_FORMAT_RGB, 0, 0, 0);
        for (int i = 0; i < addedImage->getWidth(); i++) {
            for (int j = 0; j < addedImage->getHeight(); j++) {

                Color* color = addedImage->getValueAt(i, j);
                if (i <= seamToAdd->points.at(j)->x) {
                    newImage->setValueAt(i, j, *color);
                }
                else if (i > seamToAdd->points.at(j)->x) {
                    newImage->setValueAt(i+1, j, *color);
                }
                delete color;
            }
        }
        size++;

        for (int j = 0; j < newImage->getHeight(); j++) {

            int point = seamToAdd->points.at(j)->x;
            Color* colorLeft = newImage->getValueAt(point, j);
            Color* colorRight = newImage->getValueAt(point+2, j);

            if (colorLeft == nullptr) {
                colorLeft = new Color();
                colorLeft->setRGB(0, 0, 0);
            }
            if (colorRight == nullptr) {
                colorRight = new Color();
                colorRight->setRGB(0, 0, 0);
            }
            Color temp;
            double red = (colorLeft->getRed() + colorRight->getRed()) / 2.0;
            double green = (colorLeft->getGreen() + colorRight->getGreen()) / 2.0;
            double blue = (colorLeft->getBlue() + colorRight->getBlue()) / 2.0;
                    
            temp.setRGB(red, green, blue);
            newImage->setValueAt(point+1, j, temp);
            delete colorLeft;
            delete colorRight;
        }

        for (int ss = s+1; ss < seamsToAdd.size(); ss++) {
            Seam* seamToMove = seamsToAdd.at(ss);
            for (int j = 0; j < newImage->getHeight(); j++) {
                if (seamToMove->points.at(j)->x >= seamToAdd->points.at(j)->x)
                {
                    seamToMove->points.at(j)->x++;
                }
            }
        }
    
        delete addedImage;
        addedImage = newImage;
    }

    return addedImage;
}
/*Image* Processor::seamCarvingAddToRight(Image* refImg, int newWidth)
{
        std::vector<Seam*> seamsToAdd;

    // -- first get all deletion seams
    int window = refImg->getWidth() - (360 * refImg->getWidth() / 1024);
    Image* tempDelete = new Image(refImg);
    int nbToDelete = newWidth - width;
    std::cout << "Delete right " << nbToDelete << std::endl;
    for (int d = 0; d < nbToDelete; d++) {

        std::cout << "Try delete right " << d << std::endl;

        Processor tempDeleteProc(tempDelete);
        tempDeleteProc.calculateSobelRGB();
        tempDeleteProc.seamCarvingDetectRGB();

        int idxRemove = 0;
        double seamMinimal = 500.0;

        // -- detect seam to remove
        for (int j = window; j < tempDeleteProc.seams.size(); j++) {
            if (tempDeleteProc.seams.at(j)->energy <= seamMinimal) {
                idxRemove = j;
                seamMinimal = tempDeleteProc.seams.at(j)->energy;
            }
        }

        window--;

        // -- remove actually form temporary image
        Seam* seamToRemove = tempDeleteProc.seams.at(idxRemove);
        Image* newImage = new Image(tempDelete->getWidth() - 1, tempDelete->getHeight(), PNG_FORMAT_RGB, 0, 0, 0);
        for (int i = 0; i < tempDelete->getWidth(); i++) {
            for (int j = 0; j < tempDelete->getHeight(); j++) {

                // -- ignore
                if (i == seamToRemove->points.at(j)->x) {
                    continue;
                }
                else if (i < seamToRemove->points.at(j)->x) {
                    Color* color = tempDelete->getValueAt(i, j);
                    newImage->setValueAt(i, j, *color);
                    delete color;
                }
                else {
                    Color* color = tempDelete->getValueAt(i, j);
                    newImage->setValueAt(i-1, j, *color);
                    delete color;
                }
            }
        }

        Seam* toAdd = new Seam();
        for (Point* p : seamToRemove->points) {
            Point* nPoint = new Point();
            nPoint->x = p->x;
            nPoint->y = p->y;
            toAdd->points.push_back(nPoint);
        }
        seamsToAdd.push_back(toAdd);

        delete tempDelete;
        tempDelete = newImage;
    }

    // -- replace seam to reel point in image reference
    for (int s = 0 ; s < seamsToAdd.size(); s++) {
        for (int ss = s+1 ; ss < seamsToAdd.size(); ss++) {
            Seam* ref = seamsToAdd.at(s);
            Seam* move = seamsToAdd.at(ss);

            for (int j = 0; j < refImg->getHeight(); j++) {
                if (move->points.at(j)->x >= ref->points.at(j)->x) {
                    move->points.at(j)->x++;
                }
            }
        }
    }

    int size = refImg->getWidth() + 1;
    Image* addedImage = new Image(refImg);

    for (int s = 0 ; s < seamsToAdd.size(); s++) {

        Seam* seamToAdd = seamsToAdd.at(s);
        std::cout << "Try add right " << s << " at " << seamToAdd->points.at(0)->x << " - " << 
        seamToAdd->points.at(seamToAdd->points.size()-1)->x << std::endl;
        Image* newImage = new Image(size, addedImage->getHeight(), PNG_FORMAT_RGB, 0, 0, 0);
        for (int i = 0; i < addedImage->getWidth(); i++) {
            for (int j = 0; j < addedImage->getHeight(); j++) {

                Color* color = addedImage->getValueAt(i, j);
                if (i <= seamToAdd->points.at(j)->x) {
                    newImage->setValueAt(i, j, *color);
                }
                else if (i > seamToAdd->points.at(j)->x) {
                    newImage->setValueAt(i+1, j, *color);
                }
                delete color;
            }
        }
        size++;

        for (int j = 0; j < newImage->getHeight(); j++) {

            int point = seamToAdd->points.at(j)->x;
            Color* colorLeft = newImage->getValueAt(point, j);
            Color* colorRight = newImage->getValueAt(point+2, j);

            if (colorLeft == nullptr) {
                colorLeft = new Color();
                colorLeft->setRGB(0, 0, 0);
            }
            if (colorRight == nullptr) {
                colorRight = new Color();
                colorRight->setRGB(0, 0, 0);
            }
            Color temp;
            double red = (colorLeft->getRed() + colorRight->getRed()) / 2.0;
            double green = (colorLeft->getGreen() + colorRight->getGreen()) / 2.0;
            double blue = (colorLeft->getBlue() + colorRight->getBlue()) / 2.0;
                    
            temp.setRGB(red, green, blue);
            newImage->setValueAt(point+1, j, temp);
            delete colorLeft;
            delete colorRight;
        }

        for (int ss = s+1; ss < seamsToAdd.size(); ss++) {
            Seam* seamToMove = seamsToAdd.at(ss);
            for (int j = 0; j < newImage->getHeight(); j++) {
                if (seamToMove->points.at(j)->x >= seamToAdd->points.at(j)->x)
                {
                    seamToMove->points.at(j)->x++;
                }
            }
        }
    
        delete addedImage;
        addedImage = newImage;
    }

    return addedImage;
}*/
Image* Processor::seamCarvingAddToRight(Image* refImg, int newWidth)
{
    std::vector<Seam*> seamsToAdd;

    // -- first get all deletion seams
    int window = refImg->getWidth() - (360 * refImg->getWidth() / 1024);
    Image* tempDelete = new Image(refImg);
    int nbToDelete = newWidth - width;
    std::cout << "Delete right " << nbToDelete << std::endl;
    Processor tempDeleteProc(tempDelete);
    tempDeleteProc.calculateSobelRGB();
    for (int d = 0; d < nbToDelete; d++) {

        std::cout << "Try delete right " << d << std::endl;
        tempDeleteProc.seamCarvingDetectRGB();

        int idxRemove = 0;
        double seamMinimal = 500.0;

        // -- detect seam to remove
        for (int j = window; j < tempDeleteProc.seams.size(); j++) {
            if (tempDeleteProc.seams.at(j)->energy <= seamMinimal) {
                idxRemove = j;
                seamMinimal = tempDeleteProc.seams.at(j)->energy;
            }
        }

        window--;

        // -- remove actually form temporary image
        Seam* seamToRemove = tempDeleteProc.seams.at(idxRemove);
        Seam* toAdd = new Seam();
        for (Point* p : seamToRemove->points) {
            Point* nPoint = new Point();
            nPoint->x = p->x;
            nPoint->y = p->y;
            toAdd->points.push_back(nPoint);
        }
        seamsToAdd.push_back(toAdd);
        tempDeleteProc.removeSeam(toAdd);
    }

    // -- replace seam to reel point in image reference
    for (int s = 0 ; s < seamsToAdd.size(); s++) {
        for (int ss = s+1 ; ss < seamsToAdd.size(); ss++) {
            Seam* ref = seamsToAdd.at(s);
            Seam* move = seamsToAdd.at(ss);

            for (int j = 0; j < refImg->getHeight(); j++) {
                if (move->points.at(j)->x >= ref->points.at(j)->x) {
                    move->points.at(j)->x++;
                }
            }
        }
    }

    int size = refImg->getWidth() + 1;
    Image* addedImage = new Image(refImg);

    for (int s = 0 ; s < seamsToAdd.size(); s++) {

        Seam* seamToAdd = seamsToAdd.at(s);
        std::cout << "Try add right " << s << " at " << seamToAdd->points.at(0)->x << " - " << 
        seamToAdd->points.at(seamToAdd->points.size()-1)->x << std::endl;
        Image* newImage = new Image(size, addedImage->getHeight(), PNG_FORMAT_RGB, 0, 0, 0);
        for (int i = 0; i < addedImage->getWidth(); i++) {
            for (int j = 0; j < addedImage->getHeight(); j++) {

                Color* color = addedImage->getValueAt(i, j);
                if (i <= seamToAdd->points.at(j)->x) {
                    newImage->setValueAt(i, j, *color);
                }
                else if (i > seamToAdd->points.at(j)->x) {
                    newImage->setValueAt(i+1, j, *color);
                }
                delete color;
            }
        }
        size++;

        for (int j = 0; j < newImage->getHeight(); j++) {

            int point = seamToAdd->points.at(j)->x;
            Color* colorLeft = newImage->getValueAt(point, j);
            Color* colorRight = newImage->getValueAt(point+2, j);

            if (colorLeft == nullptr) {
                colorLeft = new Color();
                colorLeft->setRGB(0, 0, 0);
            }
            if (colorRight == nullptr) {
                colorRight = new Color();
                colorRight->setRGB(0, 0, 0);
            }
            Color temp;
            double red = (colorLeft->getRed() + colorRight->getRed()) / 2.0;
            double green = (colorLeft->getGreen() + colorRight->getGreen()) / 2.0;
            double blue = (colorLeft->getBlue() + colorRight->getBlue()) / 2.0;
                    
            temp.setRGB(red, green, blue);
            newImage->setValueAt(point+1, j, temp);
            delete colorLeft;
            delete colorRight;
        }

        for (int ss = s+1; ss < seamsToAdd.size(); ss++) {
            Seam* seamToMove = seamsToAdd.at(ss);
            for (int j = 0; j < newImage->getHeight(); j++) {
                if (seamToMove->points.at(j)->x >= seamToAdd->points.at(j)->x)
                {
                    seamToMove->points.at(j)->x++;
                }
            }
        }
    
        delete addedImage;
        addedImage = newImage;
    }

    return addedImage;
}

void Processor::removeSeam(Seam* seam)
{
    double** newDatasR = new double*[width-1];
    double** newDatasG = new double*[width-1];
    double** newDatasB = new double*[width-1];
    for (int i = 0; i < width-1; i++) {
        newDatasR[i] = new double[height];
        newDatasG[i] = new double[height];
        newDatasB[i] = new double[height];
        for (int j = 0; j < height; j++) {
            newDatasR[i][j] = 0.0;
            newDatasG[i][j] = 0.0;
            newDatasB[i][j] = 0.0;
        }
    }

    for (int i = 0; i < width-1; i++) {
        for (int j = 0; j < height; j++) {
            if (i < seam->points.at(j)->x) {
                newDatasR[i][j] = datasR[i][j];
                newDatasG[i][j] = datasG[i][j];
                newDatasB[i][j] = datasB[i][j];
            }
            else if (i > seam->points.at(j)->x) {
                newDatasR[i][j] = datasR[i+1][j];
                newDatasG[i][j] = datasG[i+1][j];
                newDatasB[i][j] = datasB[i+1][j];
            }
        }
    }

    for (int i = 0; i < width; i++) {
        delete[] datasR[i];
        delete[] datasG[i];
        delete[] datasB[i];
    }
    delete[] datasR;
    delete[] datasG;
    delete[] datasB;

    datasR = newDatasR;
    datasG = newDatasG;
    datasB = newDatasB;
    width--;
}

void Processor::seamCarvingDetect(bool saveEnergy)
{
    double max = 0;
    double** seamEnergy = new double*[width];
    for (int i = 0; i < width; i++) {
        seamEnergy[i] = new double[height];
        for (int j = 0; j < height; j++) {
            seamEnergy[i][j] = 0.0;
        }
    }

    // -- calculate all temporaries energies
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {

            if (j == 0) {
                seamEnergy[i][j] = datas[i][j];
            }
            else {

                if (i == 0) {
                    double middle = seamEnergy[i][j-1];
                    double right = seamEnergy[i+1][j-1];

                    if (middle <= right) {
                        seamEnergy[i][j] = datas[i][j] + middle;
                    }
                    else {
                        seamEnergy[i][j] = datas[i][j] + right;
                    }
                }

                else if (i == width - 1) {

                    double left = seamEnergy[i-1][j-1];
                    double middle = seamEnergy[i][j-1];

                    if (middle <= left) {
                        seamEnergy[i][j] = datas[i][j] + middle;
                    }
                    else {
                        seamEnergy[i][j] = datas[i][j] + left;
                    }
                }

                else {

                    double left = seamEnergy[i-1][j-1];
                    double middle = seamEnergy[i][j-1];
                    double right = seamEnergy[i+1][j-1];

                    // -- choose middle in priority
                    if (middle <= left && middle <= right) {
                        seamEnergy[i][j] = datas[i][j] + middle;
                    }
                    // -- choose left in second priority
                    else if (left < middle && left <= right) {
                        seamEnergy[i][j] = datas[i][j] + left;
                    }
                    else {
                        seamEnergy[i][j] = datas[i][j] + right;
                    }
                }

                if (j == height - 1) {
                    if (max < seamEnergy[i][j]) {
                        max = seamEnergy[i][j];
                    }
                }
            }
        }
    }

    if (saveEnergy) {
        Image* img = new Image(width, height, PNG_FORMAT_RGB, 0, 0, 0);
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                double value = seamEnergy[i][j] / max;
                Color temp;
                temp.setRGB(value, value, value);
                img->setValueAt(i, j, temp);
            }
        }
        img->saveImage("energy.png");
        delete img;
    }

    // -- removal
    for (Seam* seam : seams) {
        for (Point* p : seam->points) {
            delete p;
        }
        delete seam;
    }

    for (int i = 0; i < width; i++) {

        Seam* seam = new Seam();
        seam->energy = seamEnergy[i][height-1];
        Point* start = new Point();
        start->x = i;
        start->y = height-1;
        seam->points.push_back(start);
        seams.push_back(seam);

        Point* current = start;

        for (int j = height-2; j >= 0; j--) {

            if (current->x == 0) {
                double middle = seamEnergy[current->x][j];
                double right = seamEnergy[current->x+1][j];

                Point* p = new Point();
                p->y = j;

                // -- choose middle in priority
                if (middle <= right) {
                    p->x = current->x;
                }
                else {
                    p->x = current->x+1;
                }

                current = p;
                seam->points.push_back(p);
            }
            else if (current->x == width -1) {

                double left = seamEnergy[current->x-1][j];
                double middle = seamEnergy[current->x][j];

                Point* p = new Point();
                p->y = j;

                // -- choose middle in priority
                if (middle <= left) {
                    p->x = current->x;
                }
                else {
                    p->x = current->x-1;
                }

                current = p;
                seam->points.push_back(p);

            }
            else {

                double left = seamEnergy[current->x-1][j];
                double middle = seamEnergy[current->x][j];
                double right = seamEnergy[current->x+1][j];

                Point* p = new Point();
                p->y = j;

                // -- choose middle in priority
                if (middle <= left && middle <= right) {
                    p->x = current->x;
                }
                // -- choose left in second priority
                else if (left < middle && left <= right) {
                    p->x = current->x-1;
                }
                else {
                    p->x = current->x+1;
                }

                current = p;
                seam->points.push_back(p);
            }
        }
    }
}
void Processor::seamCarvingDetectRGB(bool saveEnergy)
{
    double max = 0;
    double** seamEnergy = new double*[width];
    for (int i = 0; i < width; i++) {
        seamEnergy[i] = new double[height];
        for (int j = 0; j < height; j++) {
            seamEnergy[i][j] = 0.0;
        }
    }

    // -- calculate all temporaries energies
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {

            if (j == 0) {
                seamEnergy[i][j] = datasR[i][j] + datasG[i][j] + datasB[i][j];
            }
            else {

                if (i == 0) {
                    double middle = seamEnergy[i][j-1];
                    double right = seamEnergy[i+1][j-1];

                    if (middle <= right) {
                        seamEnergy[i][j] = datasR[i][j] + datasG[i][j] + datasB[i][j] + middle;
                    }
                    else {
                        seamEnergy[i][j] = datasR[i][j] + datasG[i][j] + datasB[i][j] + right;
                    }
                }

                else if (i == width - 1) {

                    double left = seamEnergy[i-1][j-1];
                    double middle = seamEnergy[i][j-1];

                    if (middle <= left) {
                        seamEnergy[i][j] = datasR[i][j] + datasG[i][j] + datasB[i][j] + middle;
                    }
                    else {
                        seamEnergy[i][j] = datasR[i][j] + datasG[i][j] + datasB[i][j] + left;
                    }
                }

                else {

                    double left = seamEnergy[i-1][j-1];
                    double middle = seamEnergy[i][j-1];
                    double right = seamEnergy[i+1][j-1];

                    // -- choose middle in priority
                    if (middle <= left && middle <= right) {
                        seamEnergy[i][j] = datasR[i][j] + datasG[i][j] + datasB[i][j] + middle;
                    }
                    // -- choose left in second priority
                    else if (left < middle && left <= right) {
                        seamEnergy[i][j] = datasR[i][j] + datasG[i][j] + datasB[i][j] + left;
                    }
                    else {
                        seamEnergy[i][j] = datasR[i][j] + datasG[i][j] + datasB[i][j] + right;
                    }
                }

                if (j == height - 1) {
                    if (max < seamEnergy[i][j]) {
                        max = seamEnergy[i][j];
                    }
                }
            }
        }
    }

    if (saveEnergy) {
        Image* img = new Image(width, height, PNG_FORMAT_RGB, 0, 0, 0);
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                double value = seamEnergy[i][j] / max;
                Color temp;
                temp.setRGB(value, value, value);
                img->setValueAt(i, j, temp);
            }
        }
        img->saveImage("energy.png");
        delete img;
    }

    // -- removal
    for (Seam* seam : seams) {
        for (Point* p : seam->points) {
            delete p;
        }
        delete seam;
    }
    seams.clear();

    for (int i = 0; i < width; i++) {

        Seam* seam = new Seam();
        seam->energy = seamEnergy[i][height-1];
        Point* start = new Point();
        start->x = i;
        start->y = height-1;
        seam->points.push_back(start);
        seams.push_back(seam);

        Point* current = start;

        for (int j = height-2; j >= 0; j--) {

            if (current->x == 0) {
                double middle = seamEnergy[current->x][j];
                double right = seamEnergy[current->x+1][j];

                Point* p = new Point();
                p->y = j;

                // -- choose middle in priority
                if (middle <= right) {
                    p->x = current->x;
                }
                else {
                    p->x = current->x+1;
                }

                current = p;
                seam->points.push_back(p);
            }
            else if (current->x == width -1) {

                double left = seamEnergy[current->x-1][j];
                double middle = seamEnergy[current->x][j];

                Point* p = new Point();
                p->y = j;

                // -- choose middle in priority
                if (middle <= left) {
                    p->x = current->x;
                }
                else {
                    p->x = current->x-1;
                }

                current = p;
                seam->points.push_back(p);

            }
            else {

                double left = seamEnergy[current->x-1][j];
                double middle = seamEnergy[current->x][j];
                double right = seamEnergy[current->x+1][j];

                Point* p = new Point();
                p->y = j;

                // -- choose middle in priority
                if (middle <= left && middle <= right) {
                    p->x = current->x;
                }
                // -- choose left in second priority
                else if (left < middle && left <= right) {
                    p->x = current->x-1;
                }
                else {
                    p->x = current->x+1;
                }

                current = p;
                seam->points.push_back(p);
            }
        }
    }
}
void Processor::seamCarvingTracing(Image* refImg)
{
    std::map<int, int> energyFinalCount;
    std::map<int, double> energyFinal;
    std::map<int, double>::iterator it;
    std::map<int, int>::iterator it2;
    Image* imgGeneral = new Image(width, height, PNG_FORMAT_RGB, 0, 0, 0);
    for (int i = 0; i < width; i++) {

        int firstPoint = seams.at(i)->points.at(seams.at(i)->points.size()-1)->x;
        it = energyFinal.find(firstPoint);
        if (it != energyFinal.end()) {
            double value = it->second;
            int count = energyFinalCount.find(firstPoint)->second;
            value += seams.at(i)->energy;
            energyFinal.insert_or_assign(firstPoint, value);
            count++;
            energyFinalCount.insert_or_assign(firstPoint, count);
        }
        else {
            energyFinal.insert_or_assign(firstPoint, seams.at(i)->energy);
            energyFinalCount.insert_or_assign(firstPoint, 1);
        }

        int idxH = height-1;
        for (Point* p : seams.at(i)->points) {
            Color* refColor = refImg->getValueAt(p->x, p->y);
            imgGeneral->setValueAt(i, idxH, *refColor);
            delete refColor;
            idxH--;
        }
    }
    imgGeneral->saveImage("generalCarving.png");

    it2 = energyFinalCount.begin();
    for (it = energyFinal.begin(); it != energyFinal.end(); it++) {
        std::cout << "Point first : " << it->first << " : " << (it->second / it2->second) << ", count : " << it2->second << std::endl;
        it2++;
    }

    double i = 1.0;

    std::vector<Seam*> seamsLeft;
    std::vector<Seam*> seamsRight;
    std::vector<double> energyLeft;
    std::vector<double> energyRight;

    for (int i = 0; i < width/2; i++) {
        seamsLeft.push_back(seams.at(i));
        energyLeft.push_back(seams.at(i)->energy);
        seamsRight.push_back(seams.at(i+width/2));
        energyRight.push_back(seams.at(i+width/2)->energy);
    }

    // -- sort to draw
    std::sort(seams.begin(), seams.end(), CompareSeam());
    
    // -- sort to find energy max taken into account
    std::sort(energyLeft.begin(), energyLeft.end());
    std::sort(energyRight.begin(), energyRight.end());

    for (Seam* seam : seams) {

        for (Point* p : seam->points) {

            double toDraw = datas[p->x][p->y];
            datas[p->x][p->y] = i;
        }

        // -- end loop
        i -= 1.0/(seams.size()+1);
        if (i <= 0) {
            break;
        }
    }

    for (Seam* seam : seams) {
        std::cout << "Energy : " << seam->energy << " at column " << seam->points.at(0)->x << std::endl;
    }
    /*for (double energy : energyLeft) {
        std::cout << "Energy : " << energy << std::endl;
    }*/
    /*for (double energy : energyRight) {
        std::cout << "Energy : " << energy << std::endl;
    }*/

    // -- work on left
    double maxEnergyLeft = (energyLeft.at(energyLeft.size()-1) + energyLeft.at(0))/2.0;
    std::cout << "Max energy left : " << maxEnergyLeft << std::endl;
    int idx = 0;
    for (; idx < energyLeft.size(); idx++) {
        if (energyLeft.at(idx) > maxEnergyLeft) {
            break;
        }
    }
    std::cout << "Size generation : " << idx << std::endl;

    Image* imgLeft = new Image(idx, height, PNG_FORMAT_RGB, 0, 0, 0);
    int idxImage = 0;
    int idxSeam = 0;

    while (idxImage < idx) {

        if (seamsLeft.at(idxSeam)->energy <= maxEnergyLeft) {

            int idxH = height-1;
            for (Point* p : seamsLeft.at(idxSeam)->points) {
                Color* refColor = refImg->getValueAt(p->x, p->y);
                imgLeft->setValueAt(idxImage, idxH, *refColor);
                delete refColor;
                idxH--;
            }
            idxImage++;
        }
        idxSeam++;
    }
    imgLeft->saveImage("leftCarving.png");

    // -- work on right
    double maxEnergyRight = (energyRight.at(energyRight.size()-1) + energyRight.at(0))/2.0;
    std::cout << "Max energy right : " << maxEnergyRight << std::endl;
    idx = 0;
    for (; idx < energyRight.size(); idx++) {
        if (energyRight.at(idx) > maxEnergyRight) {
            break;
        }
    }
    std::cout << "Size generation : " << idx << std::endl;

    Image* imgRight = new Image(idx, height, PNG_FORMAT_RGB, 0, 0, 0);
    idxImage = 0;
    idxSeam = 0;

    while (idxImage < idx) {

        if (seamsRight.at(idxSeam)->energy <= maxEnergyRight) {

            int idxH = height-1;
            for (Point* p : seamsRight.at(idxSeam)->points) {
                Color* refColor = refImg->getValueAt(p->x, p->y);
                imgRight->setValueAt(idxImage, idxH, *refColor);
                delete refColor;
                idxH--;
            }
            idxImage++;
        }
        idxSeam++;
    }
    imgRight->saveImage("rightCarving.png");
}