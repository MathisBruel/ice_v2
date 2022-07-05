#include "seamProcessor.h"

struct CompareSeam
{
    bool operator() (SeamProcessor::Seam* s1, SeamProcessor::Seam* s2) const
    {
        return (s1->energy < s2->energy);
    }
};

SeamProcessor::SeamProcessor(Image* img)
{
    this->img = img;
    this->width = img->getWidth();
    this->height = img->getHeight();
    for (int j = 0; j < height; j++) {

        std::vector<float> line;
        for (int i = 0; i < width; i++) {
            Color* color = img->getValueAt(i, j);
            line.push_back(color->getRed()*0.2989f + color->getGreen()*0.587f + color->getBlue()*0.114f);
            delete color;
        }

        datas.push_back(line);
    }
}
    
SeamProcessor::~SeamProcessor() {}

double SeamProcessor::sigmaGauss(double x, double y, double sigma)
{
    double value = 1.0 / (2*M_PI*sigma*sigma) * (std::exp(-(x*x+y*y)/(2*sigma*sigma)));
    return value;
}
void SeamProcessor::gaussianBlur(double sigma, int radius)
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

    std::vector<std::vector<float>> newDatas;
    for (int j = 0; j < height; j++) {

        std::vector<float> line;
        for (int i = 0; i < width; i++) {

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
            float value = 0.0;
            for (int x = -localRadius; x <= localRadius; x++) {
                for (int y = -localRadius; y <= localRadius; y++)  {
                    value += datas.at(j+y).at(i+x) * core[x+radius][y+radius]/factor;
                }
            }

            if (value < 0.0) {value = 0.0;}
            else if (value > 1.0) {value = 1.0;}
            line.push_back(value);
        }
        newDatas.push_back(line);
    }
    datas = newDatas;
}
void SeamProcessor::calculateSobel()
{
    std::vector<std::vector<float>> newDatas;
    for (int j = 0; j < height; j++) {

        std::vector<float> line;
        for (int i = 0; i < width; i++) {

            float color1 = (i == 0 || j == 0) ? 0 : datas.at(j-1).at(i-1);
            float color2 = (j == 0) ? 0 : datas.at(j-1).at(i);
            float color3 = (i == width-1 || j == 0) ? 0 :  datas.at(j-1).at(i+1);
            float color4 = (i == 0) ? 0 : datas.at(j).at(i-1);
            float color5 = (i == width-1) ? 0 : datas.at(j).at(i+1);
            float color6 = (i == 0 || j == height-1) ? 0 : datas.at(j+1).at(i-1);
            float color7 = (j == height-1) ? 0 : datas.at(j+1).at(i);
            float color8 = (i == width-1 || j == height-1) ? 0 : datas.at(j+1).at(i+1);

            float dx = color3 + 2*color5 + color8 - color1 - 2*color4 - color6;
            float dy = color6 + 2*color7 + color8 - color1 - 2*color2 - color3;
            float value = dx*dx + dy*dy;
            line.push_back(value);
        }
        newDatas.push_back(line);
    }

    datas = newDatas;
}

void SeamProcessor::calculateEnergy()
{
    energy.clear();

    // -- calculate all temporaries energies
    for (int j = 0; j < height; j++) {

        std::vector<float> energyLine;
        for (int i = 0; i < width; i++) {

            float value = 0.0;

            if (j == 0) {
                value = datas.at(j).at(i);
            }
            else {

                if (i == 0) {
                    float middle = energy.at(j-1).at(i);
                    float right = energy.at(j-1).at(i+1);
                    
                    if (middle <= right) {
                        value = datas.at(j).at(i) + middle;
                    }
                    else {
                        value = datas.at(j).at(i) + right;
                    }
                }

                else if (i == width - 1) {

                    float left = energy.at(j-1).at(i-1);
                    float middle = energy.at(j-1).at(i);

                    if (middle <= left) {
                        value = datas.at(j).at(i) + middle;
                    }
                    else {
                        value = datas.at(j).at(i) + left;
                    }
                }

                else {

                    float left = energy.at(j-1).at(i-1);
                    float middle = energy.at(j-1).at(i);
                    float right = energy.at(j-1).at(i+1);

                    // -- choose middle in priority
                    if (middle <= left && middle <= right) {
                        value = datas.at(j).at(i) + middle;
                    }
                    // -- choose left in second priority
                    else if (left < middle && left <= right) {
                        value = datas.at(j).at(i) + left;
                    }
                    else {
                        value = datas.at(j).at(i) + right;
                    }
                }
            }
            energyLine.push_back(value); 
        }
        energy.push_back(energyLine);
    }
}
SeamProcessor::Seam* SeamProcessor::getLowerSeam(int leftBound, int rightBound)
{
    // -- find lowest energy at end of column
    float energyMin = energy.at(height-1).at(leftBound);
    int idx = leftBound;
    for (int i = leftBound+1; i <= rightBound; i++) {

        float energyTemp = energy.at(height-1).at(i);
        if (energyTemp < energyMin) {
            energyMin = energyTemp;
            idx = i;
        }
    }


    Seam* seam = new Seam();
    seam->energy = energyMin;
    seam->posX.push_back(idx);
    int current = idx;

    for (int j = height-2; j >= 0; j--) {

        if (current == 0) {
            float middle = energy.at(j).at(current);
            float right = energy.at(j).at(current+1);

            int value = 0;
            // -- choose middle in priority
            if (middle <= right) {
                value = current;
            }
            else {
                value = current+1;
            }

            current = value;
            seam->posX.push_back(value);
        }
        else if (current == width -1) {

            float left = energy.at(j).at(current-1);
            float middle = energy.at(j).at(current);

            int value = 0;
            // -- choose middle in priority
            if (middle <= left) {
                value = current;
            }
            else {
                value = current-1;
            }

            current = value;
            seam->posX.push_back(value);

        }
        else {

            float left = energy.at(j).at(current-1);
            float middle = energy.at(j).at(current);
            float right = energy.at(j).at(current+1);

           int value = 0;
            // -- choose middle in priority
            if (middle <= left && middle <= right) {
                value = current;
            }
            // -- choose left in second priority
            else if (left < middle && left <= right) {
                value = current-1;
            }
            else {
                value = current+1;
            }

            current = value;
            seam->posX.push_back(value);
        }
    }

    std::reverse(seam->posX.begin(), seam->posX.end());
    return seam;
}
void SeamProcessor::removeFromEnergy(Seam* seam)
{
    // -- starting from last line
    for (int i = 0; i < seam->posX.size(); i++) {
        datas.at(i).erase(std::next(datas.at(i).begin(), seam->posX.at(i)));
    }
    width--;
}

void SeamProcessor::getSeamsLeft(int* seams, int sizeWindow, int newWidth)
{
    calculateSobel();

    std::vector<Seam*> seamsToAdd;
    int nbDeletion = newWidth - width;
    int window = sizeWindow-1;
    // -- find all removal backward seams on deletion
    for (int d = 0; d < nbDeletion; d++) {
        calculateEnergy();

        // -- get lower seam to delete
        Seam* seam = getLowerSeam(0, window);
        seamsToAdd.push_back(seam);
        removeFromEnergy(seam);
        window--;
    }

    // -- replace seam to reel point in image reference
    for (int s = 0 ; s < seamsToAdd.size(); s++) {

        Seam* ref = seamsToAdd.at(s);
        for (int ss = s+1 ; ss < seamsToAdd.size(); ss++) {
            Seam* move = seamsToAdd.at(ss);

            for (int j = 0; j < height; j++) {
                if (move->posX.at(j) >= ref->posX.at(j)) {
                    move->posX.at(j) += 2;
                }
            }
        }

        for (int j = 0; j < height; j++) {
            seams[s*height+j] = ref->posX.at(j);
        }
    }
}
void SeamProcessor::getSeamsRight(int* seams, int sizeWindow, int newWidth)
{
    calculateSobel();

    std::vector<Seam*> seamsToAdd;
    int window = width-sizeWindow;
    int nbDeletion = newWidth - width;
    // -- find all removal backward seams on deletion
    for (int d = 0; d < nbDeletion; d++) {
        calculateEnergy();

        // -- get lower seam to delete
        Seam* seam = getLowerSeam(window, width-1);
        seamsToAdd.push_back(seam);
        removeFromEnergy(seam);
    }

    // -- replace seam to reel point in image reference
    for (int s = 0 ; s < seamsToAdd.size(); s++) {
        
        Seam* ref = seamsToAdd.at(s);
        for (int ss = s+1 ; ss < seamsToAdd.size(); ss++) {
            Seam* move = seamsToAdd.at(ss);

            for (int j = 0; j < height; j++) {
                if (move->posX.at(j) >= ref->posX.at(j)) {
                    move->posX.at(j) += 2;
                }
            }
        }

        for (int j = 0; j < height; j++) {
            seams[s*height+j] = ref->posX.at(j);
        }
    }
}

Image* SeamProcessor::seamCarvingRemove(int newWidth, bool blur)
{
    if (blur) {
        gaussianBlur(0.8, 1);
    }

    // -- create temporary table of values
    std::vector<std::vector<double>> newDatasR;
    std::vector<std::vector<double>> newDatasG;
    std::vector<std::vector<double>> newDatasB;
    for (int j = 0; j < height; j++) {

        std::vector<double> lineR;
        std::vector<double> lineG;
        std::vector<double> lineB;
        for (int i = 0; i < width; i++) {
            Color* color = img->getValueAt(i, j);
            lineR.push_back(color->getRed());
            lineG.push_back(color->getGreen());
            lineB.push_back(color->getBlue());
            delete color;
        }
        newDatasR.push_back(lineR);
        newDatasG.push_back(lineG);
        newDatasB.push_back(lineB);
    }

    for (int s = 0; s < width - newWidth; s++) {
        // -- forward energy : recalculate contour and energy
        calculateSobel();
        calculateEnergy();

        // -- get lower seam to delete
        Seam* seam = getLowerSeam(0, width-1);

        for (int j = 0; j < height; j++) {
            newDatasR.at(j).erase(std::next(newDatasR.at(j).begin(), seam->posX.at(j)));
            newDatasG.at(j).erase(std::next(newDatasG.at(j).begin(), seam->posX.at(j)));
            newDatasB.at(j).erase(std::next(newDatasB.at(j).begin(), seam->posX.at(j)));
        }

        width--;
        delete seam;

        datas.clear();
        for (int j = 0; j < height; j++) {
            std::vector<float> line;
            for (int i = 0; i < newDatasR.at(j).size(); i++) {
                line.push_back(newDatasR.at(j).at(i)*0.2989f + newDatasG.at(j).at(i)*0.587f + newDatasB.at(j).at(i)*0.114f);
            }
            datas.push_back(line);
        }
    }

    // -- create final image
    Image* newImage = new Image(newWidth, height, PNG_FORMAT_RGB, 0, 0, 0);
    for (int i = 0; i < newImage->getWidth(); i++) {
        for (int j = 0; j < newImage->getHeight(); j++) {

            Color color;
            color.setRGB(newDatasR.at(j).at(i), newDatasG.at(j).at(i), newDatasB.at(j).at(i));
            newImage->setValueAt(i, j, color);
        }
    }
    return newImage;
}
Image* SeamProcessor::seamCarvingAdd(int newWidth, bool blur)
{
    if (blur) {
        gaussianBlur(0.8, 1);
    }

    // -- backward energy : sobel is calculated only once
    calculateSobel();

    std::vector<Seam*> seamsToAdd;
    int nbDeletion = newWidth - width;
    // -- find all removal backward seams on deletion
    for (int d = 0; d < nbDeletion; d++) {

        calculateEnergy();

        // -- get lower seam to delete
        Seam* seam = getLowerSeam(0, width-1);
        seamsToAdd.push_back(seam);
        removeFromEnergy(seam);
    }

    // -- replace seam to reel point in image reference
    for (int s = 0 ; s < seamsToAdd.size(); s++) {
        for (int ss = s+1 ; ss < seamsToAdd.size(); ss++) {
            Seam* ref = seamsToAdd.at(s);
            Seam* move = seamsToAdd.at(ss);

            for (int j = 0; j < height; j++) {
                if (move->posX.at(j) >= ref->posX.at(j)) {
                    move->posX.at(j) += 2;
                }
            }
        }
    }

    width = img->getWidth();

    // -- create temporary table of values
    std::vector<std::vector<double>> newDatasR;
    std::vector<std::vector<double>> newDatasG;
    std::vector<std::vector<double>> newDatasB;
    for (int j = 0; j < height; j++) {

        std::vector<double> lineR;
        std::vector<double> lineG;
        std::vector<double> lineB;
        for (int i = 0; i < width; i++) {
            Color* color = img->getValueAt(i, j);
            lineR.push_back(color->getRed());
            lineG.push_back(color->getGreen());
            lineB.push_back(color->getBlue());
            delete color;
        }
        newDatasR.push_back(lineR);
        newDatasG.push_back(lineG);
        newDatasB.push_back(lineB);
    }

    // -- add all seams to table
    for (int s = 0 ; s < seamsToAdd.size(); s++) {
        Seam* seamToAdd = seamsToAdd.at(s);
        
        for (int j = 0; j < height; j++) {

            // -- calculate new color from average
            int idxSeam = seamToAdd->posX.at(j);
            double red = newDatasR.at(j).at(idxSeam);
            double green = newDatasG.at(j).at(idxSeam);
            double blue = newDatasB.at(j).at(idxSeam);
            if (idxSeam+1 < newDatasR.at(j).size()) {
                red += newDatasR.at(j).at(idxSeam+1);
            }
            if (idxSeam+1 < newDatasG.at(j).size()) {
                green += newDatasG.at(j).at(idxSeam+1);
            }
            if (idxSeam+1 < newDatasB.at(j).size()) {
                blue += newDatasB.at(j).at(idxSeam+1);
            }
            red /= 2.0;
            green /= 2.0;
            blue /= 2.0;

            if (idxSeam == newDatasR.at(j).size() - 1) {
                newDatasR.at(j).push_back(red);
                newDatasG.at(j).push_back(green);
                newDatasB.at(j).push_back(blue);
            }
            else {
                newDatasR.at(j).insert(std::next(newDatasR.at(j).begin(), idxSeam+1), red);
                newDatasG.at(j).insert(std::next(newDatasG.at(j).begin(), idxSeam+1), green);
                newDatasB.at(j).insert(std::next(newDatasB.at(j).begin(), idxSeam+1), blue);
            }
        }
    }

    // -- create final image
    Image* newImage = new Image(newWidth, height, PNG_FORMAT_RGB, 0, 0, 0);
    for (int i = 0; i < newImage->getWidth(); i++) {
        for (int j = 0; j < newImage->getHeight(); j++) {

            Color color;
            color.setRGB(newDatasR.at(j).at(i), newDatasG.at(j).at(i), newDatasB.at(j).at(i));
            newImage->setValueAt(i, j, color);
        }
    }
    return newImage;
}
Image* SeamProcessor::seamCarvingAddToLeft(int sizeWindow, int newWidth, bool blur)
{
    if (blur) {
        gaussianBlur(0.8, 1);
    }
    // -- backward energy : sobel is calculated only once
    calculateSobel();

    std::vector<Seam*> seamsToAdd;

    int nbDeletion = newWidth - width;
    int window = sizeWindow-1;
    // -- find all removal backward seams on deletion
    for (int d = 0; d < nbDeletion; d++) {
        calculateEnergy();

        // -- get lower seam to delete
        Seam* seam = getLowerSeam(0, window);
        seamsToAdd.push_back(seam);
        removeFromEnergy(seam);
        window--;
    }

    // -- replace seam to reel point in image reference
    for (int s = 0 ; s < seamsToAdd.size(); s++) {

        /*std::string seamStr = "Seam " + std::to_string(s) + " : ";
        Seam* print = seamsToAdd.at(s);
        for (int j = 0; j < height; j++) {
            seamStr += " | " + std::to_string(print->posX.at(j));
        }
        Poco::Logger::get("SeamProcessor").debug(seamStr, __FILE__, __LINE__);*/

        for (int ss = s+1 ; ss < seamsToAdd.size(); ss++) {
            Seam* ref = seamsToAdd.at(s);
            Seam* move = seamsToAdd.at(ss);

            for (int j = 0; j < height; j++) {
                if (move->posX.at(j) >= ref->posX.at(j)) {
                    move->posX.at(j) += 2;
                }
            }
        }
    }

    width = img->getWidth();

    // -- create temporary table of values
    std::vector<std::vector<double>> newDatasR;
    std::vector<std::vector<double>> newDatasG;
    std::vector<std::vector<double>> newDatasB;
    for (int j = 0; j < height; j++) {

        std::vector<double> lineR;
        std::vector<double> lineG;
        std::vector<double> lineB;
        for (int i = 0; i < width; i++) {
            Color* color = img->getValueAt(i, j);
            lineR.push_back(color->getRed());
            lineG.push_back(color->getGreen());
            lineB.push_back(color->getBlue());
            delete color;
        }
        newDatasR.push_back(lineR);
        newDatasG.push_back(lineG);
        newDatasB.push_back(lineB);
    }

    // -- add all seams to table
    for (int s = 0 ; s < seamsToAdd.size(); s++) {
        Seam* seamToAdd = seamsToAdd.at(s);

        for (int j = 0; j < height; j++) {

            // -- calculate new color from average
            int idxSeam = seamToAdd->posX.at(j);
            double red = newDatasR.at(j).at(idxSeam);
            double green = newDatasG.at(j).at(idxSeam);
            double blue = newDatasB.at(j).at(idxSeam);
            if (idxSeam+1 < newDatasR.at(j).size()) {
                red += newDatasR.at(j).at(idxSeam+1);
            }
            if (idxSeam+1 < newDatasG.at(j).size()) {
                green += newDatasG.at(j).at(idxSeam+1);
            }
            if (idxSeam+1 < newDatasB.at(j).size()) {
                blue += newDatasB.at(j).at(idxSeam+1);
            }
            red /= 2.0;
            green /= 2.0;
            blue /= 2.0;

            if (idxSeam == newDatasG.at(j).size() - 1) {
                newDatasR.at(j).push_back(red);
                newDatasG.at(j).push_back(green);
                newDatasB.at(j).push_back(blue);
            }
            else {
                newDatasR.at(j).insert(std::next(newDatasR.at(j).begin(), idxSeam+1), red);
                newDatasG.at(j).insert(std::next(newDatasG.at(j).begin(), idxSeam+1), green);
                newDatasB.at(j).insert(std::next(newDatasB.at(j).begin(), idxSeam+1), blue);
            }
        }
    }

    // -- create final image
    Image* newImage = new Image(newWidth, height, PNG_FORMAT_RGB, 0, 0, 0);
    for (int i = 0; i < newImage->getWidth(); i++) {
        for (int j = 0; j < newImage->getHeight(); j++) {

            Color color;
            color.setRGB(newDatasR.at(j).at(i), newDatasG.at(j).at(i), newDatasB.at(j).at(i));
            newImage->setValueAt(i, j, color);
        }
    }
    return newImage;
}
Image* SeamProcessor::seamCarvingAddToRight(int sizeWindow, int newWidth, bool blur)
{
    if (blur) {
        gaussianBlur(0.8, 1);
    }
    // -- backward energy : sobel is calculated only once
    calculateSobel();

    std::vector<Seam*> seamsToAdd;
    int window = width-sizeWindow;
    int nbDeletion = newWidth - width;
    // -- find all removal backward seams on deletion
    for (int d = 0; d < nbDeletion; d++) {
        calculateEnergy();

        // -- get lower seam to delete
        Seam* seam = getLowerSeam(window, width-1);
        seamsToAdd.push_back(seam);
        removeFromEnergy(seam);
    }

    // -- replace seam to reel point in image reference
    for (int s = 0 ; s < seamsToAdd.size(); s++) {
        for (int ss = s+1 ; ss < seamsToAdd.size(); ss++) {
            Seam* ref = seamsToAdd.at(s);
            Seam* move = seamsToAdd.at(ss);

            for (int j = 0; j < height; j++) {
                if (move->posX.at(j) >= ref->posX.at(j)) {
                    move->posX.at(j) += 2;
                }
            }
        }
    }

    width = img->getWidth();

    // -- create temporary table of values
    std::vector<std::vector<double>> newDatasR;
    std::vector<std::vector<double>> newDatasG;
    std::vector<std::vector<double>> newDatasB;
    for (int j = 0; j < height; j++) {

        std::vector<double> lineR;
        std::vector<double> lineG;
        std::vector<double> lineB;
        for (int i = 0; i < width; i++) {
            Color* color = img->getValueAt(i, j);
            lineR.push_back(color->getRed());
            lineG.push_back(color->getGreen());
            lineB.push_back(color->getBlue());
            delete color;
        }
        newDatasR.push_back(lineR);
        newDatasG.push_back(lineG);
        newDatasB.push_back(lineB);
    }

    // -- add all seams to table
    for (int s = 0 ; s < seamsToAdd.size(); s++) {
        Seam* seamToAdd = seamsToAdd.at(s);
        
        for (int j = 0; j < height; j++) {

            // -- calculate new color from average
            int idxSeam = seamToAdd->posX.at(j);
            double red = newDatasR.at(j).at(idxSeam);
            double green = newDatasG.at(j).at(idxSeam);
            double blue = newDatasB.at(j).at(idxSeam);
            if (idxSeam+1 < newDatasR.at(j).size()) {
                red += newDatasR.at(j).at(idxSeam+1);
            }
            if (idxSeam+1 < newDatasG.at(j).size()) {
                green += newDatasG.at(j).at(idxSeam+1);
            }
            if (idxSeam+1 < newDatasB.at(j).size()) {
                blue += newDatasB.at(j).at(idxSeam+1);
            }
            red /= 2.0;
            green /= 2.0;
            blue /= 2.0;

            if (idxSeam == newDatasG.at(j).size() - 1) {
                newDatasR.at(j).push_back(red);
                newDatasG.at(j).push_back(green);
                newDatasB.at(j).push_back(blue);
            }
            else {
                newDatasR.at(j).insert(std::next(newDatasR.at(j).begin(), idxSeam+1), red);
                newDatasG.at(j).insert(std::next(newDatasG.at(j).begin(), idxSeam+1), green);
                newDatasB.at(j).insert(std::next(newDatasB.at(j).begin(), idxSeam+1), blue);
            }
        }
    }

    // -- create final image
    Image* newImage = new Image(newWidth, height, PNG_FORMAT_RGB, 0, 0, 0);
    for (int i = 0; i < newImage->getWidth(); i++) {
        for (int j = 0; j < newImage->getHeight(); j++) {

            Color color;
            color.setRGB(newDatasR.at(j).at(i), newDatasG.at(j).at(i), newDatasB.at(j).at(i));
            newImage->setValueAt(i, j, color);
        }
    }
    return newImage;
}