#include <seam.cuh>

__global__ void cutFrame(int width, int height, unsigned char* dataRGB, unsigned char* left, unsigned char* right)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int idx = index; idx < width*height*3; idx+=stride) {

        int comp = idx%3;
        int idxTemp = (idx-comp)/3;
        int i = idxTemp%width;
        int j = (idxTemp-i)/width;
        int tempWidth = width/2;

        if (i < tempWidth) {
            left[(j*tempWidth+i)*3+comp] = dataRGB[idx];
        }
        else {
            right[((j*tempWidth)+i-tempWidth)*3+comp] = dataRGB[idx];
        }
    }
}

__global__ void grayscale(int width, int height, unsigned char* dataRGB, float* dataOutGray)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int i = index; i < width*height; i+=stride) {
        dataOutGray[i] = (float)(dataRGB[i*3])*0.2989f + (float)(dataRGB[i*3+1])*0.587f + (float)(dataRGB[i*3+2])*0.114f;
        dataOutGray[i] /= 255;
    }
}

__global__ void sobel(int width, int height, float* dataInGray, float* dataOutSobel, float* dataOutSobel2)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int idx = index; idx < height*width; idx+=stride) {

        int i = idx%width;
        int j = (idx - i)/width;
        float color1 = (i == 0 || j == 0) ? 0 : dataInGray[idx - width - 1];
        float color2 = (j == 0) ? 0 : dataInGray[idx - width];
        float color3 = (i == width-1 || j == 0) ? 0 :  dataInGray[idx - width + 1];
        float color4 = (i == 0) ? 0 : dataInGray[idx - 1];
        float color5 = (i == width-1) ? 0 : dataInGray[idx + 1];
        float color6 = (i == 0 || j == height-1) ? 0 : dataInGray[idx + width - 1];
        float color7 = (j == height-1) ? 0 : dataInGray[idx + width];
        float color8 = (i == width-1 || j == height-1) ? 0 : dataInGray[idx + width + 1];

        float dx = color3 + 2*color5 + color8 - color1 - 2*color4 - color6;
        float dy = color6 + 2*color7 + color8 - color1 - 2*color2 - color3;
        dataOutSobel[idx] = dx*dx + dy*dy;
        dataOutSobel2[idx] = dataOutSobel[idx];
    }
}
__global__ void sobelSimple(int width, int height, float* dataInGray, float* dataOutSobel)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int idx = index; idx < height*width; idx+=stride) {

        int i = idx%width;
        int j = (idx - i)/width;
        float color1 = (i == 0 || j == 0) ? 0 : dataInGray[idx - width - 1];
        float color2 = (j == 0) ? 0 : dataInGray[idx - width];
        float color3 = (i == width-1 || j == 0) ? 0 :  dataInGray[idx - width + 1];
        float color4 = (i == 0) ? 0 : dataInGray[idx - 1];
        float color5 = (i == width-1) ? 0 : dataInGray[idx + 1];
        float color6 = (i == 0 || j == height-1) ? 0 : dataInGray[idx + width - 1];
        float color7 = (j == height-1) ? 0 : dataInGray[idx + width];
        float color8 = (i == width-1 || j == height-1) ? 0 : dataInGray[idx + width + 1];

        float dx = color3 + 2*color5 + color8 - color1 - 2*color4 - color6;
        float dy = color6 + 2*color7 + color8 - color1 - 2*color2 - color3;
        dataOutSobel[idx] = dx*dx + dy*dy;
    }
}
__global__ void sobelComplex(int width, int height, float* dataInGray, float* dataInSobel, float* dataOutSobel)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int idx = index; idx < height*width; idx+=stride) {

        int i = idx%width;
        int j = (idx - i)/width;
        float color1 = (i == 0 || j == 0) ? 0 : dataInGray[idx - width - 1];
        float color2 = (j == 0) ? 0 : dataInGray[idx - width];
        float color3 = (i == width-1 || j == 0) ? 0 :  dataInGray[idx - width + 1];
        float color4 = (i == 0) ? 0 : dataInGray[idx - 1];
        float color5 = (i == width-1) ? 0 : dataInGray[idx + 1];
        float color6 = (i == 0 || j == height-1) ? 0 : dataInGray[idx + width - 1];
        float color7 = (j == height-1) ? 0 : dataInGray[idx + width];
        float color8 = (i == width-1 || j == height-1) ? 0 : dataInGray[idx + width + 1];

        float dx = color3 + 2*color5 + color8 - color1 - 2*color4 - color6;
        float dy = color6 + 2*color7 + color8 - color1 - 2*color2 - color3;
        float energyNew = (dx*dx + dy*dy);
        dataOutSobel[idx] = dataInSobel[idx] + (energyNew - dataInSobel[idx])*0.3;
        //dataInSobel[idx] = energyNew;
    }
}

__global__ void removeSeamFromSobel(int width, int height, float *dataSobel, float *dataSobelNew, int* seam)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int idx = index; idx < width*height; idx+=stride) {

        int i = idx%width;
        int j = (idx-i)/width;
        int idx2 = j*(width+1)+i;

        if (i < seam[j]) {dataSobelNew[idx] = dataSobel[idx2];}
        else {dataSobelNew[idx] = dataSobel[idx2+1];}
    }
}

__global__ void addSeamToImage(int width, int height, unsigned char* dataIn, unsigned char* dataOut, int* seam)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int idx = index; idx < width*height*3; idx+=stride) {

        int comp = idx%3;
        int idxTemp = (idx-comp)/3;
        int i = idxTemp%width;
        int j = (idxTemp-i)/width;
        int idx2 = (j*(width-1)+i)*3+comp;

        // -- before seam
        if (i <= seam[j]) {dataOut[idx] = dataIn[idx2];}
        // -- after seam
        else if (i > seam[j] + 1) {dataOut[idx] = dataIn[idx2-3];}
        // -- duplicate seam
        else {
            int value = dataIn[idx2-3];
            if (seam[j]+1 < width-1) {
                value += dataIn[idx2];
            }
            value /= 2;
            dataOut[idx] = (unsigned char)value;
        }
    }
}

__global__ void copyImage(int width, int height, unsigned char* dataInLeft, unsigned char* dataInRight, unsigned char* dataOut) 
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int idx = index; idx < width*height*3; idx+=stride) {
        
        unsigned char comp = idx%3;
        int idxTemp = (idx-comp)/3;
        short i = idxTemp%width;
        short j = (idxTemp-i)/width;
        int tempWidth = width/2;

        if (i < tempWidth) {
            dataOut[idx] = dataInLeft[((j*tempWidth)+i)*3+comp];
        }
        else {
            dataOut[idx] = dataInRight[((j*tempWidth)+i-tempWidth)*3+comp];
        }
    }
}

__global__ void computeIndexMap(int width, int height, float* sobelImage, int* indexes)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int idx = index; idx < width*(height-1); idx+=stride) {

        int i = idx%width;
        int tempIdx = idx+width;
        if (i == 0) {
            float middle = sobelImage[tempIdx];
            float right = sobelImage[tempIdx+1];

            if (middle <= right) {indexes[idx] = tempIdx;}
            else {indexes[idx] = tempIdx+1;}
        }
        else if (i == width-1) {
            float left = sobelImage[tempIdx-1];
            float middle = sobelImage[tempIdx];

            if (middle <= left) {indexes[idx] = tempIdx;}
            else {indexes[idx] = tempIdx-1;}
        }
        else {
            float left = sobelImage[tempIdx-1];
            float middle = sobelImage[tempIdx];
            float right = sobelImage[tempIdx+1];

            if (middle <= left && middle <= right) {indexes[idx] = tempIdx;}
            else if (left <= right) {indexes[idx] = tempIdx-1;}
            else {indexes[idx] = tempIdx+1;}
        }
    }
}

__global__ void computeIndexMap2(int width, int height, float* sobelImage, int* indexes)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int idx = index; idx < width*(height-1); idx+=stride) {

        int i = idx%width;
        int tempIdx = idx;
        if (i == 0) {
            float middle = sobelImage[tempIdx];
            float right = sobelImage[tempIdx+1];

            if (middle <= right) {indexes[idx] = tempIdx;}
            else {indexes[idx] = tempIdx+1;}
        }
        else if (i == width-1) {
            float left = sobelImage[tempIdx-1];
            float middle = sobelImage[tempIdx];

            if (middle <= left) {indexes[idx] = tempIdx;}
            else {indexes[idx] = tempIdx-1;}
        }
        else {
            float left = sobelImage[tempIdx-1];
            float middle = sobelImage[tempIdx];
            float right = sobelImage[tempIdx+1];

            if (middle <= left && middle <= right) {indexes[idx] = tempIdx;}
            else if (left <= right) {indexes[idx] = tempIdx-1;}
            else {indexes[idx] = tempIdx+1;}
        }
    }
}

__global__ void repercuteSeam(int nbSeamsImpact, int* seams, int height)
{
    for (int i = 0; i < nbSeamsImpact; i++) {
        for (int j = i+1; j < nbSeamsImpact; j++) {

            int index = blockIdx.x * blockDim.x + threadIdx.x;
            int stride = blockDim.x * gridDim.x;
            for (int idx = index; idx < height; idx+=stride) {
        
                if (seams[j*height+idx] >= seams[i*height+idx]) {
                    seams[j*height+idx] += 2;
                }
            }
        }
    }
}

__global__ void calculateAllSeams(int width, int height, int* indexes, float* sobel, int* seamsTemporaries, float* energy, int leftBound, int rightBound)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    int max = rightBound - leftBound + 1;
    for (int idx = index; idx < max; idx+=stride) {

        energy[idx] = sobel[leftBound+idx];
        seamsTemporaries[idx*height] = leftBound+idx;
        int indexCurrent = leftBound+idx;
        for (int j = 1; j < height; j++) {
            indexCurrent = indexes[indexCurrent];
            energy[idx] += sobel[indexCurrent];
            seamsTemporaries[idx*height+j] = indexCurrent%width;
        }
    }
}

__global__ void calculateAllSeams2(int width, int height, int* indexes, float* sobel, int* seamsTemporaries, float* energy, int leftBound, int rightBound)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    int max = rightBound - leftBound + 1;
    for (int idx = index; idx < max; idx+=stride) {

        energy[idx] = sobel[(height-1)*width+leftBound+idx];
        seamsTemporaries[idx*height + height-1] = leftBound+idx;
        int indexCurrent = (height-2)*width+leftBound+idx;
        for (int j = height-2; j >= 0; j--) {
            indexCurrent = indexes[indexCurrent];
            energy[idx] += sobel[indexCurrent];
            seamsTemporaries[idx*height+j] = indexCurrent%width;
        }
    }
}

__global__ void keepFirstSeam(int height, int* seamsTemporaries, float* energy, int* seam, int sizeWindow, float* energyMax)
{
    int idx = 0;
    float energyMin = energy[0];
    for (int i = 1; i < sizeWindow; i++) {
        if (energy[i] < energyMin) {
            energyMin = energy[i];
            idx = i;
        }
    }

    energyMax[0] += energy[idx];

    for (int i = 0; i < height; i++) {
        seam[i] = seamsTemporaries[idx*height+i];
    }
}
__global__ void keepFirstSeamRight(int height, int* seamsTemporaries, float* energy, int* seam, int sizeWindow, float* energyMax)
{
    int idx = 0;
    float energyMin = energy[0];
    for (int i = 1; i < sizeWindow; i++) {
        if (energy[i] <= energyMin) {
            energyMin = energy[i];
            idx = i;
        }
    }

    energyMax[0] += energy[idx];

    for (int i = 0; i < height; i++) {
        seam[i] = seamsTemporaries[idx*height+i];
    }
}

__global__ void calculateDistanceToOldSeam(int height, int* seamPrevious, int* seamsTemporaries, float* energy, int* distance, int sizeWindow)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int idx = index; idx < sizeWindow; idx+=stride) {

        distance[idx] = 0;
        for (int j = 0; j < height; j++) {
            distance[idx] += abs(seamPrevious[j] - seamsTemporaries[idx*height+j]);
        }
        distance[idx] *= energy[idx];
    }
}

__global__ void keepFirstMotionSeamLeft(int height, int* seamsTemporaries, float* energy, int* seam, int sizeWindow)
{
    int idx = 0;
    float energyMin = energy[0];
    for (int i = 1; i < sizeWindow; i++) {
        if (energy[i] < energyMin) {
            energyMin = energy[i];
            idx = i;
        }
    }

    memcpy(seam, &seamsTemporaries[idx*height], sizeof(int)*height);
}
__global__ void keepFirstMotionSeamRight(int height, int* seamsTemporaries, float* energy, int* seam, int sizeWindow)
{
    int idx = 0;
    float energyMin = energy[0];
    for (int i = 1; i < sizeWindow; i++) {
        if (energy[i] <= energyMin) {
            energyMin = energy[i];
            idx = i;
        }
    }

    memcpy(seam, &seamsTemporaries[idx*height], sizeof(int)*height);
}

__global__ void keepCloserSeam(int height, int* seamsTemporaries, int* distance, int* seam, int sizeWindow)
{
    int idx = 0;
    int minDistance = distance[0];
    for (int i = 1; i < sizeWindow; i++) {
        if (distance[i] < minDistance) {
            minDistance = distance[i];
            idx = i;
        }
    }

    memcpy(seam, &seamsTemporaries[idx*height], sizeof(int)*height);
}

void addResizeVideoFirst(int width, int height, int widthDst, unsigned char* dataIn, unsigned char* dataOut, int* seamsOutLeft, int* seamsOutRight)
{   
    int blockSize = 1024;
    int widthHalf = width/2;
    int nbAdd = (widthDst-width)/2;
    int searchWindow = 460*widthHalf/1024;
    int leftBoundLeft = 0;
    int rightBoundLeft = searchWindow-1;
    int leftBoundRight = widthHalf - searchWindow;
    int rightBoundRight = widthHalf-1;

    // -- create streams
    cudaStream_t streamLeft, streamRight;
    cudaStreamCreate(&streamLeft);
    cudaStreamCreate(&streamRight);
    cudaStream_t streamLeft2, streamRight2;
    cudaStreamCreate(&streamLeft2);
    cudaStreamCreate(&streamRight2);

    // -- copy datas to GPU
    unsigned char* dataRGB;
    cudaMalloc(&dataRGB, width*height*3*sizeof(unsigned char));
    cudaMemcpy(dataRGB, dataIn, width*height*3*sizeof(unsigned char), cudaMemcpyHostToDevice);

    // -- separate left and right
    unsigned char* dataRGBLeft;
    unsigned char* dataRGBRight;
    int numBlocksSplit = ((width*height*3) + blockSize - 1) / blockSize;
    cudaMalloc(&dataRGBLeft, widthHalf*height*3*sizeof(unsigned char));
    cudaMalloc(&dataRGBRight, widthHalf*height*3*sizeof(unsigned char));
    cutFrame<<<numBlocksSplit, blockSize>>>(width, height, dataRGB, dataRGBLeft, dataRGBRight);
    cudaDeviceSynchronize();

    // -- calculate grayscale image
    float* grayImageLeft;
    float* grayImageRight;
    int numBlocksFilter = ((widthHalf*height) + blockSize - 1) / blockSize;
    cudaMalloc((void **)&grayImageLeft, widthHalf*height*sizeof(float));
    cudaMalloc((void **)&grayImageRight, widthHalf*height*sizeof(float));
    grayscale<<<numBlocksFilter, blockSize, 0, streamLeft>>>(widthHalf, height, dataRGBLeft, grayImageLeft);
    grayscale<<<numBlocksFilter, blockSize, 0, streamRight>>>(widthHalf, height, dataRGBRight, grayImageRight);
    cudaDeviceSynchronize();

    // -- calculate sobel
    float* sobelImageLeftUp;
    float* sobelImageRightUp;
    float* sobelImageLeftDown;
    float* sobelImageRightDown;
    cudaMalloc((void **)&sobelImageLeftUp, widthHalf*height*sizeof(float));
    cudaMalloc((void **)&sobelImageRightUp, widthHalf*height*sizeof(float));
    cudaMalloc((void **)&sobelImageLeftDown, widthHalf*height*sizeof(float));
    cudaMalloc((void **)&sobelImageRightDown, widthHalf*height*sizeof(float));
    float* sobelImage2LeftUp;
    float* sobelImage2RightUp;
    float* sobelImage2LeftDown;
    float* sobelImage2RightDown;
    cudaMalloc((void **)&sobelImage2LeftUp, widthHalf*height*sizeof(float));
    cudaMalloc((void **)&sobelImage2RightUp, widthHalf*height*sizeof(float));
    cudaMalloc((void **)&sobelImage2LeftDown, widthHalf*height*sizeof(float));
    cudaMalloc((void **)&sobelImage2RightDown, widthHalf*height*sizeof(float));
    // -- energy definition
    sobel<<<numBlocksFilter, blockSize, 0, streamLeft>>>(widthHalf, height, grayImageLeft, sobelImageLeftUp, sobelImageLeftDown);
    sobel<<<numBlocksFilter, blockSize, 0, streamRight>>>(widthHalf, height, grayImageRight, sobelImageRightUp, sobelImageRightDown);
    cudaDeviceSynchronize();

    // -- indexes map definition
    int* indexesLeftUp;
    int* indexesRightUp;
    cudaMalloc((void**)&indexesLeftUp, height*widthHalf*sizeof(int));
    cudaMalloc((void**)&indexesRightUp, height*widthHalf*sizeof(int));
    int* indexesLeftDown;
    int* indexesRightDown;
    cudaMalloc((void**)&indexesLeftDown, height*widthHalf*sizeof(int));
    cudaMalloc((void**)&indexesRightDown, height*widthHalf*sizeof(int));
    // -- seams definitions
    int* seamsLeftUp;
    int* seamsRightUp;
    cudaMalloc((void**)&seamsLeftUp, height*nbAdd*sizeof(int));
    cudaMalloc((void**)&seamsRightUp, height*nbAdd*sizeof(int));
    int* seamsLeftDown;
    int* seamsRightDown;
    cudaMalloc((void**)&seamsLeftDown, height*nbAdd*sizeof(int));
    cudaMalloc((void**)&seamsRightDown, height*nbAdd*sizeof(int));

    // -- for other calculation of seams
    int* seamsAllLeftUp;
    int* seamsAllRightUp;
    cudaMalloc((void**)&seamsAllLeftUp, height*searchWindow*sizeof(int));
    cudaMalloc((void**)&seamsAllRightUp, height*searchWindow*sizeof(int));
    int* seamsAllLeftDown;
    int* seamsAllRightDown;
    cudaMalloc((void**)&seamsAllLeftDown, height*searchWindow*sizeof(int));
    cudaMalloc((void**)&seamsAllRightDown, height*searchWindow*sizeof(int));
    float* energySeamsLeftUp;
    float* energySeamsRightUp;
    cudaMalloc((void**)&energySeamsLeftUp, searchWindow*sizeof(float));
    cudaMalloc((void**)&energySeamsRightUp, searchWindow*sizeof(float));
    float* energySeamsLeftDown;
    float* energySeamsRightDown;
    cudaMalloc((void**)&energySeamsLeftDown, searchWindow*sizeof(float));
    cudaMalloc((void**)&energySeamsRightDown, searchWindow*sizeof(float));
    float* energySumUpLeft;
    float* energySumUpRight;
    float* energySumDownLeft;
    float* energySumDownRight;
    cudaMalloc((void**)&energySumUpLeft, sizeof(float));
    cudaMalloc((void**)&energySumUpRight, sizeof(float));
    cudaMalloc((void**)&energySumDownLeft, sizeof(float));
    cudaMalloc((void**)&energySumDownRight, sizeof(float));
    cudaMemset(energySumUpLeft, 0, sizeof(float));
    cudaMemset(energySumUpRight, 0, sizeof(float));
    cudaMemset(energySumDownLeft, 0, sizeof(float));
    cudaMemset(energySumDownRight, 0, sizeof(float));

    // -- start simulation removing
    int widthTemp = widthHalf;
    for (int i = 0; i < nbAdd; i++) {

        float* sobelInLeftUp;
        float* sobelOutLeftUp;
        float* sobelInRightUp;
        float* sobelOutRightUp;
        float* sobelInLeftDown;
        float* sobelOutLeftDown;
        float* sobelInRightDown;
        float* sobelOutRightDown;

        // -- alternate between
        if (i%2 == 0) {
            sobelInLeftUp = sobelImageLeftUp;
            sobelOutLeftUp = sobelImage2LeftUp;
            sobelInRightUp = sobelImageRightUp;
            sobelOutRightUp = sobelImage2RightUp;
            sobelInLeftDown = sobelImageLeftDown;
            sobelOutLeftDown = sobelImage2LeftDown;
            sobelInRightDown = sobelImageRightDown;
            sobelOutRightDown = sobelImage2RightDown;
        }
        else {
            sobelInLeftUp = sobelImage2LeftUp;
            sobelOutLeftUp = sobelImageLeftUp;
            sobelInRightUp = sobelImage2RightUp;
            sobelOutRightUp = sobelImageRightUp;
            sobelInLeftDown = sobelImage2LeftDown;
            sobelOutLeftDown = sobelImageLeftDown;
            sobelInRightDown = sobelImage2RightDown;
            sobelOutRightDown = sobelImageRightDown;
        }

        // -- index map calculation
        cudaMemset(indexesLeftUp, 0, height*widthTemp*sizeof(int));
        cudaMemset(indexesRightUp, 0, height*widthTemp*sizeof(int));
        cudaMemset(indexesLeftDown, 0, height*widthTemp*sizeof(int));
        cudaMemset(indexesRightDown, 0, height*widthTemp*sizeof(int));
        int numBlocksEnergy = ((widthTemp*(height-1)) + blockSize - 1) / blockSize;
        computeIndexMap<<<numBlocksEnergy, blockSize, 0, streamLeft>>>(widthTemp, height, sobelInLeftUp, indexesLeftUp);
        computeIndexMap<<<numBlocksEnergy, blockSize, 0, streamRight>>>(widthTemp, height, sobelInRightUp, indexesRightUp);
        computeIndexMap2<<<numBlocksEnergy, blockSize, 0, streamLeft2>>>(widthTemp, height, sobelInLeftDown, indexesLeftDown);
        computeIndexMap2<<<numBlocksEnergy, blockSize, 0, streamRight2>>>(widthTemp, height, sobelInRightDown, indexesRightDown);
        cudaDeviceSynchronize();

        int numBlocksSeams = ((rightBoundLeft - leftBoundLeft + 1) + blockSize - 1) / blockSize;
        calculateAllSeams<<<numBlocksSeams, blockSize, 0, streamLeft>>>(widthTemp, height, indexesLeftUp, sobelInLeftUp, seamsAllLeftUp, energySeamsLeftUp, leftBoundLeft, rightBoundLeft);
        calculateAllSeams<<<numBlocksSeams, blockSize, 0, streamRight>>>(widthTemp, height, indexesRightUp, sobelInRightUp, seamsAllRightUp, energySeamsRightUp, leftBoundRight, rightBoundRight);
        calculateAllSeams2<<<numBlocksSeams, blockSize, 0, streamLeft2>>>(widthTemp, height, indexesLeftDown, sobelInLeftDown, seamsAllLeftDown, energySeamsLeftDown, leftBoundLeft, rightBoundLeft);
        calculateAllSeams2<<<numBlocksSeams, blockSize, 0, streamRight2>>>(widthTemp, height, indexesRightDown, sobelInRightDown, seamsAllRightDown, energySeamsRightDown, leftBoundRight, rightBoundRight);
        cudaDeviceSynchronize();

        keepFirstSeam<<<1,1, 0, streamLeft>>>(height, seamsAllLeftUp, energySeamsLeftUp, &seamsLeftUp[i*height], rightBoundLeft - leftBoundLeft + 1, energySumUpLeft);
        keepFirstSeamRight<<<1,1, 0, streamRight>>>(height, seamsAllRightUp, energySeamsRightUp, &seamsRightUp[i*height], rightBoundRight - leftBoundRight + 1, energySumUpRight);
        keepFirstSeam<<<1,1, 0, streamLeft2>>>(height, seamsAllLeftDown, energySeamsLeftDown, &seamsLeftDown[i*height], rightBoundLeft - leftBoundLeft + 1, energySumDownLeft);
        keepFirstSeamRight<<<1,1, 0, streamRight2>>>(height, seamsAllRightDown, energySeamsRightDown, &seamsRightDown[i*height], rightBoundRight - leftBoundRight + 1, energySumDownRight);
        cudaDeviceSynchronize();

        // -- compute new sobel
        widthTemp--;
        rightBoundLeft--;
        rightBoundRight--;

        // -- remove seam from sobel
        int numBlocksRemove = ((widthTemp*height) + blockSize - 1) / blockSize;
        removeSeamFromSobel<<<numBlocksRemove, blockSize, 0, streamLeft>>>(widthTemp, height, sobelInLeftUp, sobelOutLeftUp, &seamsLeftUp[i*height]);
        removeSeamFromSobel<<<numBlocksRemove, blockSize, 0, streamRight>>>(widthTemp, height, sobelInRightUp, sobelOutRightUp, &seamsRightUp[i*height]);
        removeSeamFromSobel<<<numBlocksRemove, blockSize, 0, streamLeft2>>>(widthTemp, height, sobelInLeftDown, sobelOutLeftDown, &seamsLeftDown[i*height]);
        removeSeamFromSobel<<<numBlocksRemove, blockSize, 0, streamRight2>>>(widthTemp, height, sobelInRightDown, sobelOutRightDown, &seamsRightDown[i*height]);
        cudaDeviceSynchronize();
    }

    float energyUpLeft, energyUpRight, energyDownLeft, energyDownRight;
    cudaMemcpy(&energyUpLeft, energySumUpLeft, sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(&energyUpRight, energySumUpRight, sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(&energyDownLeft, energySumDownLeft, sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(&energyDownRight, energySumDownRight, sizeof(float), cudaMemcpyDeviceToHost);

    int* seamsLeft = energyUpLeft < energyDownLeft ? seamsLeftUp : seamsLeftDown;
    int* seamsRight = energyUpRight < energyDownRight ? seamsRightUp : seamsRightDown;

    cudaMemcpy(seamsOutLeft, seamsLeft, height*nbAdd*sizeof(int), cudaMemcpyDeviceToHost);
    cudaMemcpy(seamsOutRight, seamsRight, height*nbAdd*sizeof(int), cudaMemcpyDeviceToHost);

    // -- repercute offset of previous seam to all next seams
    int numBlocksReper = (height + blockSize - 1) / blockSize;
    repercuteSeam<<<numBlocksReper, blockSize, 0, streamLeft>>>(nbAdd, seamsLeft, height);
    repercuteSeam<<<numBlocksReper, blockSize, 0, streamRight>>>(nbAdd, seamsRight, height);
    cudaDeviceSynchronize();

    // Free memory
    cudaFree(grayImageLeft);
    cudaFree(grayImageRight);
    cudaFree(sobelImageLeftUp);
    cudaFree(sobelImageRightUp);
    cudaFree(sobelImage2LeftUp);
    cudaFree(sobelImage2RightUp);
    cudaFree(sobelImageLeftDown);
    cudaFree(sobelImageRightDown);
    cudaFree(sobelImage2LeftDown);
    cudaFree(sobelImage2RightDown);
    cudaFree(indexesLeftUp);
    cudaFree(indexesRightUp);
    cudaFree(seamsAllLeftUp);
    cudaFree(seamsAllRightUp);
    cudaFree(energySeamsLeftUp);
    cudaFree(energySeamsRightUp);
    cudaFree(indexesLeftDown);
    cudaFree(indexesRightDown);
    cudaFree(seamsAllLeftDown);
    cudaFree(seamsAllRightDown);
    cudaFree(energySeamsLeftDown);
    cudaFree(energySeamsRightDown);
    cudaFree(energySumUpLeft);
    cudaFree(energySumDownLeft);
    cudaFree(energySumUpRight);
    cudaFree(energySumDownRight);

    // -- add new seams in order
    unsigned char* dataOutDeviceLeft;
    unsigned char* dataOutDevice2Left;
    unsigned char* dataOutDeviceRight;
    unsigned char* dataOutDevice2Right;
    cudaMalloc(&dataOutDeviceLeft, widthDst/2*height*3*sizeof(unsigned char));
    cudaMalloc(&dataOutDeviceRight, widthDst/2*height*3*sizeof(unsigned char));
    cudaMalloc(&dataOutDevice2Left, widthDst/2*height*3*sizeof(unsigned char));
    cudaMalloc(&dataOutDevice2Right, widthDst/2*height*3*sizeof(unsigned char));
    cudaMemcpy(dataOutDeviceLeft, dataRGBLeft, widthHalf*height*3*sizeof(unsigned char), cudaMemcpyDeviceToDevice);
    cudaMemcpy(dataOutDeviceRight, dataRGBRight, widthHalf*height*3*sizeof(unsigned char), cudaMemcpyDeviceToDevice);

    widthTemp = widthHalf+1;
    for (int i = 0; i < nbAdd; i++) {
        int numBlocksAdd = ((widthTemp*height*3) + blockSize - 1) / blockSize;
        if (i%2 == 0) {
            addSeamToImage<<<numBlocksAdd, blockSize, 0, streamLeft>>>(widthTemp, height, dataOutDeviceLeft, dataOutDevice2Left, &seamsLeft[i*height]);
            addSeamToImage<<<numBlocksAdd, blockSize, 0, streamRight>>>(widthTemp, height, dataOutDeviceRight, dataOutDevice2Right, &seamsRight[i*height]);
        }
        else {
            addSeamToImage<<<numBlocksAdd, blockSize, 0, streamLeft>>>(widthTemp, height, dataOutDevice2Left, dataOutDeviceLeft, &seamsLeft[i*height]);
            addSeamToImage<<<numBlocksAdd, blockSize, 0, streamRight>>>(widthTemp, height, dataOutDevice2Right, dataOutDeviceRight, &seamsRight[i*height]);
        }
        cudaDeviceSynchronize();
        widthTemp++;
    }

    unsigned char* dataOutDevice;
    cudaMalloc((void **)&dataOutDevice, widthDst*height*3*sizeof(unsigned char));
    int numBlocksCopy = ((widthDst*height*3) + blockSize - 1) / blockSize;
    if (nbAdd%2 == 0) {copyImage<<<numBlocksCopy,blockSize>>>(widthDst, height, dataOutDeviceLeft, dataOutDeviceRight, dataOutDevice);}
    else {copyImage<<<numBlocksCopy,blockSize>>>(widthDst, height, dataOutDevice2Left, dataOutDevice2Right, dataOutDevice);}
    cudaDeviceSynchronize();

    // -- copy final image
    cudaMemcpy(dataOut, dataOutDevice, widthDst*height*3*sizeof(unsigned char), cudaMemcpyDeviceToHost);

    cudaFree(dataRGB);
    cudaFree(dataRGBLeft);
    cudaFree(dataRGBRight);
    cudaFree(dataOutDevice);
    cudaFree(dataOutDeviceLeft);
    cudaFree(dataOutDeviceRight);
    cudaFree(dataOutDevice2Left);
    cudaFree(dataOutDevice2Right);
    cudaFree(seamsLeftUp);
    cudaFree(seamsLeftDown);
    cudaFree(seamsRightUp);
    cudaFree(seamsRightDown);
    cudaStreamDestroy(streamLeft);
    cudaStreamDestroy(streamRight);
    cudaStreamDestroy(streamLeft2);
    cudaStreamDestroy(streamRight2);
}
void addResizeVideoOther(int width, int height, int widthDst, unsigned char* dataIn, unsigned char* dataOut, int* seamsInLeft, int* seamsInRight)
{   
    int blockSize = 1024;
    int widthHalf = width/2;
    int nbAdd = (widthDst-width)/2;

    // -- create streams
    cudaStream_t streamLeft, streamRight;
    cudaStreamCreate(&streamLeft);
    cudaStreamCreate(&streamRight);

    // -- copy datas to GPU
    unsigned char* dataRGB;
    cudaMalloc(&dataRGB, width*height*3*sizeof(unsigned char));
    cudaMemcpy(dataRGB, dataIn, width*height*3*sizeof(unsigned char), cudaMemcpyHostToDevice);

    // -- separate left and right
    unsigned char* dataRGBLeft;
    unsigned char* dataRGBRight;
    int numBlocksSplit = ((width*height*3) + blockSize - 1) / blockSize;
    cudaMalloc(&dataRGBLeft, widthHalf*height*3*sizeof(unsigned char));
    cudaMalloc(&dataRGBRight, widthHalf*height*3*sizeof(unsigned char));
    cutFrame<<<numBlocksSplit, blockSize>>>(width, height, dataRGB, dataRGBLeft, dataRGBRight);
    cudaDeviceSynchronize();
    
    // -- seams definitions
    int* seamsLeft;
    int* seamsRight;
    cudaMalloc((void**)&seamsLeft, height*nbAdd*sizeof(int));
    cudaMalloc((void**)&seamsRight, height*nbAdd*sizeof(int));
    cudaMemcpy(seamsLeft, seamsInLeft, height*nbAdd*sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(seamsRight, seamsInRight, height*nbAdd*sizeof(int), cudaMemcpyHostToDevice);

    // -- repercute offset of previous seam to all next seams
    int numBlocksReper = (height + blockSize - 1) / blockSize;
    repercuteSeam<<<numBlocksReper, blockSize, 0, streamLeft>>>(nbAdd, seamsLeft, height);
    repercuteSeam<<<numBlocksReper, blockSize, 0, streamRight>>>(nbAdd, seamsRight, height);
    cudaDeviceSynchronize();

    // -- add new seams in order
    unsigned char* dataOutDeviceLeft;
    unsigned char* dataOutDevice2Left;
    unsigned char* dataOutDeviceRight;
    unsigned char* dataOutDevice2Right;
    cudaMalloc(&dataOutDeviceLeft, widthDst/2*height*3*sizeof(unsigned char));
    cudaMalloc(&dataOutDeviceRight, widthDst/2*height*3*sizeof(unsigned char));
    cudaMalloc(&dataOutDevice2Left, widthDst/2*height*3*sizeof(unsigned char));
    cudaMalloc(&dataOutDevice2Right, widthDst/2*height*3*sizeof(unsigned char));
    cudaMemcpy(dataOutDeviceLeft, dataRGBLeft, widthHalf*height*3*sizeof(unsigned char), cudaMemcpyDeviceToDevice);
    cudaMemcpy(dataOutDeviceRight, dataRGBRight, widthHalf*height*3*sizeof(unsigned char), cudaMemcpyDeviceToDevice);

    int widthTemp = widthHalf+1;
    for (int i = 0; i < nbAdd; i++) {
        int numBlocksAdd = ((widthTemp*height*3) + blockSize - 1) / blockSize;
        if (i%2 == 0) {
            addSeamToImage<<<numBlocksAdd, blockSize, 0, streamLeft>>>(widthTemp, height, dataOutDeviceLeft, dataOutDevice2Left, &seamsLeft[i*height]);
            addSeamToImage<<<numBlocksAdd, blockSize, 0, streamRight>>>(widthTemp, height, dataOutDeviceRight, dataOutDevice2Right, &seamsRight[i*height]);
        }
        else {
            addSeamToImage<<<numBlocksAdd, blockSize, 0, streamLeft>>>(widthTemp, height, dataOutDevice2Left, dataOutDeviceLeft, &seamsLeft[i*height]);
            addSeamToImage<<<numBlocksAdd, blockSize, 0, streamRight>>>(widthTemp, height, dataOutDevice2Right, dataOutDeviceRight, &seamsRight[i*height]);
        }
        cudaDeviceSynchronize();
        widthTemp++;
    }

    unsigned char* dataOutDevice;
    cudaMalloc((void **)&dataOutDevice, widthDst*height*3*sizeof(unsigned char));
    int numBlocksCopy = ((widthDst*height*3) + blockSize - 1) / blockSize;
    if (nbAdd%2 == 0) {copyImage<<<numBlocksCopy,blockSize>>>(widthDst, height, dataOutDeviceLeft, dataOutDeviceRight, dataOutDevice);}
    else {copyImage<<<numBlocksCopy,blockSize>>>(widthDst, height, dataOutDevice2Left, dataOutDevice2Right, dataOutDevice);}
    cudaDeviceSynchronize();

    // -- copy final image
    cudaMemcpy(dataOut, dataOutDevice, widthDst*height*3*sizeof(unsigned char), cudaMemcpyDeviceToHost);

    cudaFree(dataRGB);
    cudaFree(dataRGBLeft);
    cudaFree(dataRGBRight);
    cudaFree(dataOutDevice);
    cudaFree(dataOutDeviceLeft);
    cudaFree(dataOutDeviceRight);
    cudaFree(dataOutDevice2Left);
    cudaFree(dataOutDevice2Right);
    cudaFree(seamsLeft);
    cudaFree(seamsRight);
    cudaStreamDestroy(streamLeft);
    cudaStreamDestroy(streamRight);
}

void addResizeVideoMotionFirst(int width, int height, int widthDst, unsigned char* dataIn, unsigned char* dataOut, int* seamsOutLeft, int* seamsOutRight)
{   
    int blockSize = 1024;
    int widthHalf = width/2;
    int nbAdd = (widthDst-width)/2;
    int searchWindow = 460*widthHalf/1024;
    int leftBoundLeft = 0;
    int rightBoundLeft = searchWindow-1;
    int leftBoundRight = widthHalf - searchWindow;
    int rightBoundRight = widthHalf-1;

    // -- create streams
    cudaStream_t streamLeft, streamRight;
    cudaStreamCreate(&streamLeft);
    cudaStreamCreate(&streamRight);

    // -- copy datas to GPU
    unsigned char* dataRGB;
    cudaMalloc(&dataRGB, width*height*3*sizeof(unsigned char));
    cudaMemcpy(dataRGB, dataIn, width*height*3*sizeof(unsigned char), cudaMemcpyHostToDevice);

    // -- separate left and right
    unsigned char* dataRGBLeft;
    unsigned char* dataRGBRight;
    int numBlocksSplit = ((width*height*3) + blockSize - 1) / blockSize;
    cudaMalloc(&dataRGBLeft, widthHalf*height*3*sizeof(unsigned char));
    cudaMalloc(&dataRGBRight, widthHalf*height*3*sizeof(unsigned char));
    cutFrame<<<numBlocksSplit, blockSize>>>(width, height, dataRGB, dataRGBLeft, dataRGBRight);
    cudaDeviceSynchronize();

    // -- calculate grayscale image
    float* grayImageLeft;
    float* grayImageRight;
    int numBlocksFilter = ((widthHalf*height) + blockSize - 1) / blockSize;
    cudaMalloc((void **)&grayImageLeft, widthHalf*height*sizeof(float));
    cudaMalloc((void **)&grayImageRight, widthHalf*height*sizeof(float));
    grayscale<<<numBlocksFilter, blockSize, 0, streamLeft>>>(widthHalf, height, dataRGBLeft, grayImageLeft);
    grayscale<<<numBlocksFilter, blockSize, 0, streamRight>>>(widthHalf, height, dataRGBRight, grayImageRight);
    cudaDeviceSynchronize();

    // -- calculate sobel
    float* sobelImageLeft;
    float* sobelImageRight;
    cudaMalloc((void **)&sobelImageLeft, widthHalf*height*sizeof(float));
    cudaMalloc((void **)&sobelImageRight, widthHalf*height*sizeof(float));
    float* sobelImage2Left;
    float* sobelImage2Right;
    cudaMalloc((void **)&sobelImage2Left, widthHalf*height*sizeof(float));
    cudaMalloc((void **)&sobelImage2Right, widthHalf*height*sizeof(float));
    // -- energy definition
    sobelSimple<<<numBlocksFilter, blockSize, 0, streamLeft>>>(widthHalf, height, grayImageLeft, sobelImageLeft);
    sobelSimple<<<numBlocksFilter, blockSize, 0, streamRight>>>(widthHalf, height, grayImageRight, sobelImageRight);
    cudaDeviceSynchronize();

    // -- indexes map definition
    int* indexesLeft;
    int* indexesRight;
    cudaMalloc((void**)&indexesLeft, height*widthHalf*sizeof(int));
    cudaMalloc((void**)&indexesRight, height*widthHalf*sizeof(int));
    // -- seams definitions
    int* seamsLeft;
    int* seamsRight;
    cudaMalloc((void**)&seamsLeft, height*nbAdd*sizeof(int));
    cudaMalloc((void**)&seamsRight, height*nbAdd*sizeof(int));

    // -- for other calculation of seams
    int* seamsAllLeft;
    int* seamsAllRight;
    cudaMalloc((void**)&seamsAllLeft, height*searchWindow*sizeof(int));
    cudaMalloc((void**)&seamsAllRight, height*searchWindow*sizeof(int));
    float* energySeamsLeft;
    float* energySeamsRight;
    cudaMalloc((void**)&energySeamsLeft, searchWindow*sizeof(float));
    cudaMalloc((void**)&energySeamsRight, searchWindow*sizeof(float));

    // -- start simulation removing
    int widthTemp = widthHalf;
    for (int i = 0; i < nbAdd; i++) {

        float* sobelInLeft;
        float* sobelOutLeft;
        float* sobelInRight;
        float* sobelOutRight;

        // -- alternate between
        if (i%2 == 0) {
            sobelInLeft = sobelImageLeft;
            sobelOutLeft = sobelImage2Left;
            sobelInRight = sobelImageRight;
            sobelOutRight = sobelImage2Right;
        }
        else {
            sobelInLeft = sobelImage2Left;
            sobelOutLeft = sobelImageLeft;
            sobelInRight = sobelImage2Right;
            sobelOutRight = sobelImageRight;
        }

        // -- index map calculation
        cudaMemset(indexesLeft, 0, height*widthTemp*sizeof(int));
        cudaMemset(indexesRight, 0, height*widthTemp*sizeof(int));
        int numBlocksEnergy = ((widthTemp*(height-1)) + blockSize - 1) / blockSize;
        computeIndexMap<<<numBlocksEnergy, blockSize, 0, streamLeft>>>(widthTemp, height, sobelInLeft, indexesLeft);
        computeIndexMap<<<numBlocksEnergy, blockSize, 0, streamRight>>>(widthTemp, height, sobelInRight, indexesRight);
        cudaDeviceSynchronize();

        int numBlocksSeams = ((rightBoundLeft - leftBoundLeft + 1) + blockSize - 1) / blockSize;
        calculateAllSeams<<<numBlocksSeams, blockSize, 0, streamLeft>>>(widthTemp, height, indexesLeft, sobelInLeft, seamsAllLeft, energySeamsLeft, leftBoundLeft, rightBoundLeft);
        calculateAllSeams<<<numBlocksSeams, blockSize, 0, streamRight>>>(widthTemp, height, indexesRight, sobelInRight, seamsAllRight, energySeamsRight, leftBoundRight, rightBoundRight);
        cudaDeviceSynchronize();

        keepFirstMotionSeamLeft<<<1,1, 0, streamLeft>>>(height, seamsAllLeft, energySeamsLeft, &seamsLeft[i*height], rightBoundLeft - leftBoundLeft + 1);
        keepFirstMotionSeamRight<<<1,1, 0, streamRight>>>(height, seamsAllRight, energySeamsRight, &seamsRight[i*height], rightBoundRight - leftBoundRight + 1);
        cudaDeviceSynchronize();

        // -- compute new sobel
        widthTemp--;
        rightBoundLeft--;
        rightBoundRight--;

        // -- remove seam from sobel
        int numBlocksRemove = ((widthTemp*height) + blockSize - 1) / blockSize;
        removeSeamFromSobel<<<numBlocksRemove, blockSize, 0, streamLeft>>>(widthTemp, height, sobelInLeft, sobelOutLeft, &seamsLeft[i*height]);
        removeSeamFromSobel<<<numBlocksRemove, blockSize, 0, streamRight>>>(widthTemp, height, sobelInRight, sobelOutRight, &seamsRight[i*height]);
        cudaDeviceSynchronize();
    }

    cudaMemcpy(seamsOutLeft, seamsLeft, height*nbAdd*sizeof(int), cudaMemcpyDeviceToHost);
    cudaMemcpy(seamsOutRight, seamsRight, height*nbAdd*sizeof(int), cudaMemcpyDeviceToHost);

    // -- repercute offset of previous seam to all next seams
    int numBlocksReper = (height + blockSize - 1) / blockSize;
    repercuteSeam<<<numBlocksReper, blockSize, 0, streamLeft>>>(nbAdd, seamsLeft, height);
    repercuteSeam<<<numBlocksReper, blockSize, 0, streamRight>>>(nbAdd, seamsRight, height);
    cudaDeviceSynchronize();

    // Free memory
    cudaFree(grayImageLeft);
    cudaFree(grayImageRight);

    cudaFree(sobelImageLeft);
    cudaFree(sobelImageRight);
    cudaFree(sobelImage2Left);
    cudaFree(sobelImage2Right);

    cudaFree(indexesLeft);
    cudaFree(indexesRight);
    cudaFree(seamsAllLeft);
    cudaFree(seamsAllRight);
    cudaFree(energySeamsLeft);
    cudaFree(energySeamsRight);

    // -- add new seams in order
    unsigned char* dataOutDeviceLeft;
    unsigned char* dataOutDevice2Left;
    unsigned char* dataOutDeviceRight;
    unsigned char* dataOutDevice2Right;
    cudaMalloc(&dataOutDeviceLeft, widthDst/2*height*3*sizeof(unsigned char));
    cudaMalloc(&dataOutDeviceRight, widthDst/2*height*3*sizeof(unsigned char));
    cudaMalloc(&dataOutDevice2Left, widthDst/2*height*3*sizeof(unsigned char));
    cudaMalloc(&dataOutDevice2Right, widthDst/2*height*3*sizeof(unsigned char));
    cudaMemcpy(dataOutDeviceLeft, dataRGBLeft, widthHalf*height*3*sizeof(unsigned char), cudaMemcpyDeviceToDevice);
    cudaMemcpy(dataOutDeviceRight, dataRGBRight, widthHalf*height*3*sizeof(unsigned char), cudaMemcpyDeviceToDevice);

    widthTemp = widthHalf+1;
    for (int i = 0; i < nbAdd; i++) {
        int numBlocksAdd = ((widthTemp*height*3) + blockSize - 1) / blockSize;
        if (i%2 == 0) {
            addSeamToImage<<<numBlocksAdd, blockSize, 0, streamLeft>>>(widthTemp, height, dataOutDeviceLeft, dataOutDevice2Left, &seamsLeft[i*height]);
            addSeamToImage<<<numBlocksAdd, blockSize, 0, streamRight>>>(widthTemp, height, dataOutDeviceRight, dataOutDevice2Right, &seamsRight[i*height]);
        }
        else {
            addSeamToImage<<<numBlocksAdd, blockSize, 0, streamLeft>>>(widthTemp, height, dataOutDevice2Left, dataOutDeviceLeft, &seamsLeft[i*height]);
            addSeamToImage<<<numBlocksAdd, blockSize, 0, streamRight>>>(widthTemp, height, dataOutDevice2Right, dataOutDeviceRight, &seamsRight[i*height]);
        }
        cudaDeviceSynchronize();
        widthTemp++;
    }

    unsigned char* dataOutDevice;
    cudaMalloc((void **)&dataOutDevice, widthDst*height*3*sizeof(unsigned char));
    int numBlocksCopy = ((widthDst*height*3) + blockSize - 1) / blockSize;
    if (nbAdd%2 == 0) {copyImage<<<numBlocksCopy,blockSize>>>(widthDst, height, dataOutDeviceLeft, dataOutDeviceRight, dataOutDevice);}
    else {copyImage<<<numBlocksCopy,blockSize>>>(widthDst, height, dataOutDevice2Left, dataOutDevice2Right, dataOutDevice);}
    cudaDeviceSynchronize();

    // -- copy final image
    cudaMemcpy(dataOut, dataOutDevice, widthDst*height*3*sizeof(unsigned char), cudaMemcpyDeviceToHost);

    cudaFree(dataRGB);
    cudaFree(dataRGBLeft);
    cudaFree(dataRGBRight);
    cudaFree(dataOutDevice);
    cudaFree(dataOutDeviceLeft);
    cudaFree(dataOutDeviceRight);
    cudaFree(dataOutDevice2Left);
    cudaFree(dataOutDevice2Right);
    cudaFree(seamsLeft);
    cudaFree(seamsRight);
    cudaStreamDestroy(streamLeft);
    cudaStreamDestroy(streamRight);
}
void addResizeVideoMotionOther(int width, int height, int widthDst, unsigned char* dataIn, unsigned char* dataOut, int* seamsInOutLeft, int* seamsInOutRight)
{   
    int blockSize = 1024;
    int widthHalf = width/2;
    int nbAdd = (widthDst-width)/2;
    int searchWindow = 460*widthHalf/1024;
    int leftBoundLeft = 0;
    int rightBoundLeft = searchWindow-1;
    int leftBoundRight = widthHalf - searchWindow;
    int rightBoundRight = widthHalf-1;

    // -- create streams
    cudaStream_t streamLeft, streamRight;
    cudaStreamCreate(&streamLeft);
    cudaStreamCreate(&streamRight);

    // -- copy datas to GPU
    unsigned char* dataRGB;
    cudaMalloc(&dataRGB, width*height*3*sizeof(unsigned char));
    cudaMemcpy(dataRGB, dataIn, width*height*3*sizeof(unsigned char), cudaMemcpyHostToDevice);

    // -- separate left and right
    unsigned char* dataRGBLeft;
    unsigned char* dataRGBRight;
    int numBlocksSplit = ((width*height*3) + blockSize - 1) / blockSize;
    cudaMalloc(&dataRGBLeft, widthHalf*height*3*sizeof(unsigned char));
    cudaMalloc(&dataRGBRight, widthHalf*height*3*sizeof(unsigned char));
    cutFrame<<<numBlocksSplit, blockSize>>>(width, height, dataRGB, dataRGBLeft, dataRGBRight);
    cudaDeviceSynchronize();

    // -- calculate grayscale image
    float* grayImageLeft;
    float* grayImageRight;
    int numBlocksFilter = ((widthHalf*height) + blockSize - 1) / blockSize;
    cudaMalloc((void **)&grayImageLeft, widthHalf*height*sizeof(float));
    cudaMalloc((void **)&grayImageRight, widthHalf*height*sizeof(float));
    grayscale<<<numBlocksFilter, blockSize, 0, streamLeft>>>(widthHalf, height, dataRGBLeft, grayImageLeft);
    grayscale<<<numBlocksFilter, blockSize, 0, streamRight>>>(widthHalf, height, dataRGBRight, grayImageRight);
    cudaDeviceSynchronize();

    // -- calculate sobel
    float* sobelImageLeft;
    float* sobelImageRight;
    cudaMalloc((void **)&sobelImageLeft, widthHalf*height*sizeof(float));
    cudaMalloc((void **)&sobelImageRight, widthHalf*height*sizeof(float));
    float* sobelImage2Left;
    float* sobelImage2Right;
    cudaMalloc((void **)&sobelImage2Left, widthHalf*height*sizeof(float));
    cudaMalloc((void **)&sobelImage2Right, widthHalf*height*sizeof(float));
    // -- energy definition
    sobelSimple<<<numBlocksFilter, blockSize, 0, streamLeft>>>(widthHalf, height, grayImageLeft, sobelImageLeft);
    sobelSimple<<<numBlocksFilter, blockSize, 0, streamRight>>>(widthHalf, height, grayImageRight, sobelImageRight);
    cudaDeviceSynchronize();

    // -- indexes map definition
    int* indexesLeft;
    int* indexesRight;
    cudaMalloc((void**)&indexesLeft, height*widthHalf*sizeof(int));
    cudaMalloc((void**)&indexesRight, height*widthHalf*sizeof(int));
    // -- seams definitions
    int* seamsLeft;
    int* seamsRight;
    cudaMalloc((void**)&seamsLeft, height*nbAdd*sizeof(int));
    cudaMalloc((void**)&seamsRight, height*nbAdd*sizeof(int));

    // -- for other calculation of seams
    int* seamsAllLeft;
    int* seamsAllRight;
    cudaMalloc((void**)&seamsAllLeft, height*searchWindow*sizeof(int));
    cudaMalloc((void**)&seamsAllRight, height*searchWindow*sizeof(int));
    float* energySeamsLeft;
    float* energySeamsRight;
    cudaMalloc((void**)&energySeamsLeft, searchWindow*sizeof(float));
    cudaMalloc((void**)&energySeamsRight, searchWindow*sizeof(float));

    // -- seams definitions
    int* seamsLeftPrevious;
    int* seamsRightPrevious;
    cudaMalloc((void**)&seamsLeftPrevious, height*nbAdd*sizeof(int));
    cudaMalloc((void**)&seamsRightPrevious, height*nbAdd*sizeof(int));
    cudaMemcpy(seamsLeftPrevious, seamsInOutLeft, height*nbAdd*sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(seamsRightPrevious, seamsInOutRight, height*nbAdd*sizeof(int), cudaMemcpyHostToDevice);

    // -- distance to keep better seam
    int* distanceLeft;
    int* distanceRight;
    cudaMalloc((void**)&distanceLeft, searchWindow*sizeof(int));
    cudaMalloc((void**)&distanceRight, searchWindow*sizeof(int));

    // -- start simulation removing
    int widthTemp = widthHalf;
    for (int i = 0; i < nbAdd; i++) {

        float* sobelInLeft;
        float* sobelOutLeft;
        float* sobelInRight;
        float* sobelOutRight;

        // -- alternate between
        if (i%2 == 0) {
            sobelInLeft = sobelImageLeft;
            sobelOutLeft = sobelImage2Left;
            sobelInRight = sobelImageRight;
            sobelOutRight = sobelImage2Right;
        }
        else {
            sobelInLeft = sobelImage2Left;
            sobelOutLeft = sobelImageLeft;
            sobelInRight = sobelImage2Right;
            sobelOutRight = sobelImageRight;
        }

        // -- index map calculation
        cudaMemset(indexesLeft, 0, height*widthTemp*sizeof(int));
        cudaMemset(indexesRight, 0, height*widthTemp*sizeof(int));
        int numBlocksEnergy = ((widthTemp*(height-1)) + blockSize - 1) / blockSize;
        computeIndexMap<<<numBlocksEnergy, blockSize, 0, streamLeft>>>(widthTemp, height, sobelInLeft, indexesLeft);
        computeIndexMap<<<numBlocksEnergy, blockSize, 0, streamRight>>>(widthTemp, height, sobelInRight, indexesRight);
        cudaDeviceSynchronize();

        int sizeWindow = rightBoundLeft - leftBoundLeft + 1;
        int numBlocksSeams = (sizeWindow + blockSize - 1) / blockSize;
        calculateAllSeams<<<numBlocksSeams, blockSize, 0, streamLeft>>>(widthTemp, height, indexesLeft, sobelInLeft, seamsAllLeft, energySeamsLeft, leftBoundLeft, rightBoundLeft);
        calculateAllSeams<<<numBlocksSeams, blockSize, 0, streamRight>>>(widthTemp, height, indexesRight, sobelInRight, seamsAllRight, energySeamsRight, leftBoundRight, rightBoundRight);
        cudaDeviceSynchronize();

        calculateDistanceToOldSeam<<<numBlocksSeams, blockSize, 0, streamLeft>>>(height, &seamsLeftPrevious[i*height], seamsAllLeft, energySeamsLeft, distanceLeft, sizeWindow);
        calculateDistanceToOldSeam<<<numBlocksSeams, blockSize, 0, streamRight>>>(height, &seamsRightPrevious[i*height], seamsAllRight, energySeamsRight, distanceRight, sizeWindow);
        cudaDeviceSynchronize();

        keepCloserSeam<<<1,1, 0, streamLeft>>>(height, seamsAllLeft, distanceLeft, &seamsLeft[i*height], sizeWindow);
        keepCloserSeam<<<1,1, 0, streamRight>>>(height, seamsAllRight, distanceRight, &seamsRight[i*height], sizeWindow);
        cudaDeviceSynchronize();

        // -- compute new sobel
        widthTemp--;
        rightBoundLeft--;
        rightBoundRight--;

        // -- remove seam from sobel
        int numBlocksRemove = ((widthTemp*height) + blockSize - 1) / blockSize;
        removeSeamFromSobel<<<numBlocksRemove, blockSize, 0, streamLeft>>>(widthTemp, height, sobelInLeft, sobelOutLeft, &seamsLeft[i*height]);
        removeSeamFromSobel<<<numBlocksRemove, blockSize, 0, streamRight>>>(widthTemp, height, sobelInRight, sobelOutRight, &seamsRight[i*height]);
        cudaDeviceSynchronize();
    }

    cudaMemcpy(seamsInOutLeft, seamsLeft, height*nbAdd*sizeof(int), cudaMemcpyDeviceToHost);
    cudaMemcpy(seamsInOutRight, seamsRight, height*nbAdd*sizeof(int), cudaMemcpyDeviceToHost);

    // -- repercute offset of previous seam to all next seams
    int numBlocksReper = (height + blockSize - 1) / blockSize;
    repercuteSeam<<<numBlocksReper, blockSize, 0, streamLeft>>>(nbAdd, seamsLeft, height);
    repercuteSeam<<<numBlocksReper, blockSize, 0, streamRight>>>(nbAdd, seamsRight, height);
    cudaDeviceSynchronize();

    // Free memory
    cudaFree(grayImageLeft);
    cudaFree(grayImageRight);

    cudaFree(sobelImageLeft);
    cudaFree(sobelImageRight);
    cudaFree(sobelImage2Left);
    cudaFree(sobelImage2Right);

    cudaFree(indexesLeft);
    cudaFree(indexesRight);
    cudaFree(seamsAllLeft);
    cudaFree(seamsAllRight);
    cudaFree(energySeamsLeft);
    cudaFree(energySeamsRight);
    cudaFree(distanceLeft);
    cudaFree(distanceRight);

    // -- add new seams in order
    unsigned char* dataOutDeviceLeft;
    unsigned char* dataOutDevice2Left;
    unsigned char* dataOutDeviceRight;
    unsigned char* dataOutDevice2Right;
    cudaMalloc(&dataOutDeviceLeft, widthDst/2*height*3*sizeof(unsigned char));
    cudaMalloc(&dataOutDeviceRight, widthDst/2*height*3*sizeof(unsigned char));
    cudaMalloc(&dataOutDevice2Left, widthDst/2*height*3*sizeof(unsigned char));
    cudaMalloc(&dataOutDevice2Right, widthDst/2*height*3*sizeof(unsigned char));
    cudaMemcpy(dataOutDeviceLeft, dataRGBLeft, widthHalf*height*3*sizeof(unsigned char), cudaMemcpyDeviceToDevice);
    cudaMemcpy(dataOutDeviceRight, dataRGBRight, widthHalf*height*3*sizeof(unsigned char), cudaMemcpyDeviceToDevice);

    widthTemp = widthHalf+1;
    for (int i = 0; i < nbAdd; i++) {
        int numBlocksAdd = ((widthTemp*height*3) + blockSize - 1) / blockSize;
        if (i%2 == 0) {
            addSeamToImage<<<numBlocksAdd, blockSize, 0, streamLeft>>>(widthTemp, height, dataOutDeviceLeft, dataOutDevice2Left, &seamsLeft[i*height]);
            addSeamToImage<<<numBlocksAdd, blockSize, 0, streamRight>>>(widthTemp, height, dataOutDeviceRight, dataOutDevice2Right, &seamsRight[i*height]);
        }
        else {
            addSeamToImage<<<numBlocksAdd, blockSize, 0, streamLeft>>>(widthTemp, height, dataOutDevice2Left, dataOutDeviceLeft, &seamsLeft[i*height]);
            addSeamToImage<<<numBlocksAdd, blockSize, 0, streamRight>>>(widthTemp, height, dataOutDevice2Right, dataOutDeviceRight, &seamsRight[i*height]);
        }
        cudaDeviceSynchronize();
        widthTemp++;
    }

    unsigned char* dataOutDevice;
    cudaMalloc((void **)&dataOutDevice, widthDst*height*3*sizeof(unsigned char));
    int numBlocksCopy = ((widthDst*height*3) + blockSize - 1) / blockSize;
    if (nbAdd%2 == 0) {copyImage<<<numBlocksCopy,blockSize>>>(widthDst, height, dataOutDeviceLeft, dataOutDeviceRight, dataOutDevice);}
    else {copyImage<<<numBlocksCopy,blockSize>>>(widthDst, height, dataOutDevice2Left, dataOutDevice2Right, dataOutDevice);}
    cudaDeviceSynchronize();

    // -- copy final image
    cudaMemcpy(dataOut, dataOutDevice, widthDst*height*3*sizeof(unsigned char), cudaMemcpyDeviceToHost);

    cudaFree(dataRGB);
    cudaFree(dataRGBLeft);
    cudaFree(dataRGBRight);
    cudaFree(dataOutDevice);
    cudaFree(dataOutDeviceLeft);
    cudaFree(dataOutDeviceRight);
    cudaFree(dataOutDevice2Left);
    cudaFree(dataOutDevice2Right);
    cudaFree(seamsLeft);
    cudaFree(seamsRight);
    cudaStreamDestroy(streamLeft);
    cudaStreamDestroy(streamRight);
}

void addResizeVideoDiffFirst(int width, int height, int widthDst, unsigned char* dataIn, unsigned char* dataOut, float* sobelOutLeft, float* sobelOutRight)
{   
    int blockSize = 1024;
    int widthHalf = width/2;
    int nbAdd = (widthDst-width)/2;
    int searchWindow = 460*widthHalf/1024;
    int leftBoundLeft = 0;
    int rightBoundLeft = searchWindow-1;
    int leftBoundRight = widthHalf - searchWindow;
    int rightBoundRight = widthHalf-1;

    // -- create streams
    cudaStream_t streamLeft, streamRight;
    cudaStreamCreate(&streamLeft);
    cudaStreamCreate(&streamRight);

    // -- copy datas to GPU
    unsigned char* dataRGB;
    cudaMalloc(&dataRGB, width*height*3*sizeof(unsigned char));
    cudaMemcpy(dataRGB, dataIn, width*height*3*sizeof(unsigned char), cudaMemcpyHostToDevice);

    // -- separate left and right
    unsigned char* dataRGBLeft;
    unsigned char* dataRGBRight;
    int numBlocksSplit = ((width*height*3) + blockSize - 1) / blockSize;
    cudaMalloc(&dataRGBLeft, widthHalf*height*3*sizeof(unsigned char));
    cudaMalloc(&dataRGBRight, widthHalf*height*3*sizeof(unsigned char));
    cutFrame<<<numBlocksSplit, blockSize>>>(width, height, dataRGB, dataRGBLeft, dataRGBRight);
    cudaDeviceSynchronize();

    // -- calculate grayscale image
    float* grayImageLeft;
    float* grayImageRight;
    int numBlocksFilter = ((widthHalf*height) + blockSize - 1) / blockSize;
    cudaMalloc((void **)&grayImageLeft, widthHalf*height*sizeof(float));
    cudaMalloc((void **)&grayImageRight, widthHalf*height*sizeof(float));
    grayscale<<<numBlocksFilter, blockSize, 0, streamLeft>>>(widthHalf, height, dataRGBLeft, grayImageLeft);
    grayscale<<<numBlocksFilter, blockSize, 0, streamRight>>>(widthHalf, height, dataRGBRight, grayImageRight);
    cudaDeviceSynchronize();

    // -- calculate sobel
    float* sobelImageLeft;
    float* sobelImageRight;
    cudaMalloc((void **)&sobelImageLeft, widthHalf*height*sizeof(float));
    cudaMalloc((void **)&sobelImageRight, widthHalf*height*sizeof(float));
    float* sobelImage2Left;
    float* sobelImage2Right;
    cudaMalloc((void **)&sobelImage2Left, widthHalf*height*sizeof(float));
    cudaMalloc((void **)&sobelImage2Right, widthHalf*height*sizeof(float));
    // -- energy definition
    sobelSimple<<<numBlocksFilter, blockSize, 0, streamLeft>>>(widthHalf, height, grayImageLeft, sobelImageLeft);
    sobelSimple<<<numBlocksFilter, blockSize, 0, streamRight>>>(widthHalf, height, grayImageRight, sobelImageRight);
    cudaDeviceSynchronize();

    // -- copy out sobel
    cudaMemcpy(sobelOutLeft, sobelImageLeft, widthHalf*height*sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(sobelOutRight, sobelImageRight, widthHalf*height*sizeof(float), cudaMemcpyDeviceToHost);

    // -- indexes map definition
    int* indexesLeft;
    int* indexesRight;
    cudaMalloc((void**)&indexesLeft, height*widthHalf*sizeof(int));
    cudaMalloc((void**)&indexesRight, height*widthHalf*sizeof(int));
    // -- seams definitions
    int* seamsLeft;
    int* seamsRight;
    cudaMalloc((void**)&seamsLeft, height*nbAdd*sizeof(int));
    cudaMalloc((void**)&seamsRight, height*nbAdd*sizeof(int));

    // -- for other calculation of seams
    int* seamsAllLeft;
    int* seamsAllRight;
    cudaMalloc((void**)&seamsAllLeft, height*searchWindow*sizeof(int));
    cudaMalloc((void**)&seamsAllRight, height*searchWindow*sizeof(int));
    float* energySeamsLeft;
    float* energySeamsRight;
    cudaMalloc((void**)&energySeamsLeft, searchWindow*sizeof(float));
    cudaMalloc((void**)&energySeamsRight, searchWindow*sizeof(float));

    // -- start simulation removing
    int widthTemp = widthHalf;
    for (int i = 0; i < nbAdd; i++) {

        float* sobelInLeft;
        float* sobelOutLeft;
        float* sobelInRight;
        float* sobelOutRight;

        // -- alternate between
        if (i%2 == 0) {
            sobelInLeft = sobelImageLeft;
            sobelOutLeft = sobelImage2Left;
            sobelInRight = sobelImageRight;
            sobelOutRight = sobelImage2Right;
        }
        else {
            sobelInLeft = sobelImage2Left;
            sobelOutLeft = sobelImageLeft;
            sobelInRight = sobelImage2Right;
            sobelOutRight = sobelImageRight;
        }

        // -- index map calculation
        cudaMemset(indexesLeft, 0, height*widthTemp*sizeof(int));
        cudaMemset(indexesRight, 0, height*widthTemp*sizeof(int));
        int numBlocksEnergy = ((widthTemp*(height-1)) + blockSize - 1) / blockSize;
        computeIndexMap<<<numBlocksEnergy, blockSize, 0, streamLeft>>>(widthTemp, height, sobelInLeft, indexesLeft);
        computeIndexMap<<<numBlocksEnergy, blockSize, 0, streamRight>>>(widthTemp, height, sobelInRight, indexesRight);
        cudaDeviceSynchronize();

        int numBlocksSeams = ((rightBoundLeft - leftBoundLeft + 1) + blockSize - 1) / blockSize;
        calculateAllSeams<<<numBlocksSeams, blockSize, 0, streamLeft>>>(widthTemp, height, indexesLeft, sobelInLeft, seamsAllLeft, energySeamsLeft, leftBoundLeft, rightBoundLeft);
        calculateAllSeams<<<numBlocksSeams, blockSize, 0, streamRight>>>(widthTemp, height, indexesRight, sobelInRight, seamsAllRight, energySeamsRight, leftBoundRight, rightBoundRight);
        cudaDeviceSynchronize();

        keepFirstMotionSeamLeft<<<1,1, 0, streamLeft>>>(height, seamsAllLeft, energySeamsLeft, &seamsLeft[i*height], rightBoundLeft - leftBoundLeft + 1);
        keepFirstMotionSeamRight<<<1,1, 0, streamRight>>>(height, seamsAllRight, energySeamsRight, &seamsRight[i*height], rightBoundRight - leftBoundRight + 1);
        cudaDeviceSynchronize();

        // -- compute new sobel
        widthTemp--;
        rightBoundLeft--;
        rightBoundRight--;

        // -- remove seam from sobel
        int numBlocksRemove = ((widthTemp*height) + blockSize - 1) / blockSize;
        removeSeamFromSobel<<<numBlocksRemove, blockSize, 0, streamLeft>>>(widthTemp, height, sobelInLeft, sobelOutLeft, &seamsLeft[i*height]);
        removeSeamFromSobel<<<numBlocksRemove, blockSize, 0, streamRight>>>(widthTemp, height, sobelInRight, sobelOutRight, &seamsRight[i*height]);
        cudaDeviceSynchronize();
    }

    // -- repercute offset of previous seam to all next seams
    int numBlocksReper = (height + blockSize - 1) / blockSize;
    repercuteSeam<<<numBlocksReper, blockSize, 0, streamLeft>>>(nbAdd, seamsLeft, height);
    repercuteSeam<<<numBlocksReper, blockSize, 0, streamRight>>>(nbAdd, seamsRight, height);
    cudaDeviceSynchronize();

    // Free memory
    cudaFree(grayImageLeft);
    cudaFree(grayImageRight);

    cudaFree(sobelImageLeft);
    cudaFree(sobelImageRight);
    cudaFree(sobelImage2Left);
    cudaFree(sobelImage2Right);

    cudaFree(indexesLeft);
    cudaFree(indexesRight);
    cudaFree(seamsAllLeft);
    cudaFree(seamsAllRight);
    cudaFree(energySeamsLeft);
    cudaFree(energySeamsRight);

    // -- add new seams in order
    unsigned char* dataOutDeviceLeft;
    unsigned char* dataOutDevice2Left;
    unsigned char* dataOutDeviceRight;
    unsigned char* dataOutDevice2Right;
    cudaMalloc(&dataOutDeviceLeft, widthDst/2*height*3*sizeof(unsigned char));
    cudaMalloc(&dataOutDeviceRight, widthDst/2*height*3*sizeof(unsigned char));
    cudaMalloc(&dataOutDevice2Left, widthDst/2*height*3*sizeof(unsigned char));
    cudaMalloc(&dataOutDevice2Right, widthDst/2*height*3*sizeof(unsigned char));
    cudaMemcpy(dataOutDeviceLeft, dataRGBLeft, widthHalf*height*3*sizeof(unsigned char), cudaMemcpyDeviceToDevice);
    cudaMemcpy(dataOutDeviceRight, dataRGBRight, widthHalf*height*3*sizeof(unsigned char), cudaMemcpyDeviceToDevice);

    widthTemp = widthHalf+1;
    for (int i = 0; i < nbAdd; i++) {
        int numBlocksAdd = ((widthTemp*height*3) + blockSize - 1) / blockSize;
        if (i%2 == 0) {
            addSeamToImage<<<numBlocksAdd, blockSize, 0, streamLeft>>>(widthTemp, height, dataOutDeviceLeft, dataOutDevice2Left, &seamsLeft[i*height]);
            addSeamToImage<<<numBlocksAdd, blockSize, 0, streamRight>>>(widthTemp, height, dataOutDeviceRight, dataOutDevice2Right, &seamsRight[i*height]);
        }
        else {
            addSeamToImage<<<numBlocksAdd, blockSize, 0, streamLeft>>>(widthTemp, height, dataOutDevice2Left, dataOutDeviceLeft, &seamsLeft[i*height]);
            addSeamToImage<<<numBlocksAdd, blockSize, 0, streamRight>>>(widthTemp, height, dataOutDevice2Right, dataOutDeviceRight, &seamsRight[i*height]);
        }
        cudaDeviceSynchronize();
        widthTemp++;
    }

    unsigned char* dataOutDevice;
    cudaMalloc((void **)&dataOutDevice, widthDst*height*3*sizeof(unsigned char));
    int numBlocksCopy = ((widthDst*height*3) + blockSize - 1) / blockSize;
    if (nbAdd%2 == 0) {copyImage<<<numBlocksCopy,blockSize>>>(widthDst, height, dataOutDeviceLeft, dataOutDeviceRight, dataOutDevice);}
    else {copyImage<<<numBlocksCopy,blockSize>>>(widthDst, height, dataOutDevice2Left, dataOutDevice2Right, dataOutDevice);}
    cudaDeviceSynchronize();

    // -- copy final image
    cudaMemcpy(dataOut, dataOutDevice, widthDst*height*3*sizeof(unsigned char), cudaMemcpyDeviceToHost);

    cudaFree(dataRGB);
    cudaFree(dataRGBLeft);
    cudaFree(dataRGBRight);
    cudaFree(dataOutDevice);
    cudaFree(dataOutDeviceLeft);
    cudaFree(dataOutDeviceRight);
    cudaFree(dataOutDevice2Left);
    cudaFree(dataOutDevice2Right);
    cudaFree(seamsLeft);
    cudaFree(seamsRight);
    cudaStreamDestroy(streamLeft);
    cudaStreamDestroy(streamRight);
}
void addResizeVideoDiffOther(int width, int height, int widthDst, unsigned char* dataIn, unsigned char* dataOut, float* sobelInOutLeft, float* sobelInOutRight)
{   
    int blockSize = 1024;
    int widthHalf = width/2;
    int nbAdd = (widthDst-width)/2;
    int searchWindow = 460*widthHalf/1024;
    int leftBoundLeft = 0;
    int rightBoundLeft = searchWindow-1;
    int leftBoundRight = widthHalf - searchWindow;
    int rightBoundRight = widthHalf-1;

    // -- create streams
    cudaStream_t streamLeft, streamRight;
    cudaStreamCreate(&streamLeft);
    cudaStreamCreate(&streamRight);

    // -- copy datas to GPU
    unsigned char* dataRGB;
    cudaMalloc(&dataRGB, width*height*3*sizeof(unsigned char));
    cudaMemcpy(dataRGB, dataIn, width*height*3*sizeof(unsigned char), cudaMemcpyHostToDevice);

    // -- separate left and right
    unsigned char* dataRGBLeft;
    unsigned char* dataRGBRight;
    int numBlocksSplit = ((width*height*3) + blockSize - 1) / blockSize;
    cudaMalloc(&dataRGBLeft, widthHalf*height*3*sizeof(unsigned char));
    cudaMalloc(&dataRGBRight, widthHalf*height*3*sizeof(unsigned char));
    cutFrame<<<numBlocksSplit, blockSize>>>(width, height, dataRGB, dataRGBLeft, dataRGBRight);
    cudaDeviceSynchronize();

    // -- calculate grayscale image
    float* grayImageLeft;
    float* grayImageRight;
    int numBlocksFilter = ((widthHalf*height) + blockSize - 1) / blockSize;
    cudaMalloc((void **)&grayImageLeft, widthHalf*height*sizeof(float));
    cudaMalloc((void **)&grayImageRight, widthHalf*height*sizeof(float));
    grayscale<<<numBlocksFilter, blockSize, 0, streamLeft>>>(widthHalf, height, dataRGBLeft, grayImageLeft);
    grayscale<<<numBlocksFilter, blockSize, 0, streamRight>>>(widthHalf, height, dataRGBRight, grayImageRight);
    cudaDeviceSynchronize();

    // -- calculate sobel
    float* sobelImageLeft;
    float* sobelImageRight;
    cudaMalloc((void **)&sobelImageLeft, widthHalf*height*sizeof(float));
    cudaMalloc((void **)&sobelImageRight, widthHalf*height*sizeof(float));
    float* sobelImage2Left;
    float* sobelImage2Right;
    cudaMalloc((void **)&sobelImage2Left, widthHalf*height*sizeof(float));
    cudaMalloc((void **)&sobelImage2Right, widthHalf*height*sizeof(float));
    cudaMemcpy(sobelImage2Left, sobelInOutLeft, widthHalf*height*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(sobelImage2Right, sobelInOutRight, widthHalf*height*sizeof(float), cudaMemcpyHostToDevice);

    // -- energy definition
    sobelComplex<<<numBlocksFilter, blockSize, 0, streamLeft>>>(widthHalf, height, grayImageLeft, sobelImage2Left, sobelImageLeft);
    sobelComplex<<<numBlocksFilter, blockSize, 0, streamRight>>>(widthHalf, height, grayImageRight, sobelImage2Right, sobelImageRight);
    cudaDeviceSynchronize();

    // -- copy out sobel
    cudaMemcpy(sobelInOutLeft, sobelImageLeft, widthHalf*height*sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(sobelInOutRight, sobelImageRight, widthHalf*height*sizeof(float), cudaMemcpyDeviceToHost);
    //cudaMemcpy(sobelInOutLeft, sobelImage2Left, widthHalf*height*sizeof(float), cudaMemcpyDeviceToHost);
    //cudaMemcpy(sobelInOutRight, sobelImage2Left, widthHalf*height*sizeof(float), cudaMemcpyDeviceToHost);

    // -- indexes map definition
    int* indexesLeft;
    int* indexesRight;
    cudaMalloc((void**)&indexesLeft, height*widthHalf*sizeof(int));
    cudaMalloc((void**)&indexesRight, height*widthHalf*sizeof(int));
    // -- seams definitions
    int* seamsLeft;
    int* seamsRight;
    cudaMalloc((void**)&seamsLeft, height*nbAdd*sizeof(int));
    cudaMalloc((void**)&seamsRight, height*nbAdd*sizeof(int));

    // -- for other calculation of seams
    int* seamsAllLeft;
    int* seamsAllRight;
    cudaMalloc((void**)&seamsAllLeft, height*searchWindow*sizeof(int));
    cudaMalloc((void**)&seamsAllRight, height*searchWindow*sizeof(int));
    float* energySeamsLeft;
    float* energySeamsRight;
    cudaMalloc((void**)&energySeamsLeft, searchWindow*sizeof(float));
    cudaMalloc((void**)&energySeamsRight, searchWindow*sizeof(float));

    // -- start simulation removing
    int widthTemp = widthHalf;
    for (int i = 0; i < nbAdd; i++) {

        float* sobelInLeft;
        float* sobelOutLeft;
        float* sobelInRight;
        float* sobelOutRight;

        // -- alternate between
        if (i%2 == 0) {
            sobelInLeft = sobelImageLeft;
            sobelOutLeft = sobelImage2Left;
            sobelInRight = sobelImageRight;
            sobelOutRight = sobelImage2Right;
        }
        else {
            sobelInLeft = sobelImage2Left;
            sobelOutLeft = sobelImageLeft;
            sobelInRight = sobelImage2Right;
            sobelOutRight = sobelImageRight;
        }

        // -- index map calculation
        cudaMemset(indexesLeft, 0, height*widthTemp*sizeof(int));
        cudaMemset(indexesRight, 0, height*widthTemp*sizeof(int));
        int numBlocksEnergy = ((widthTemp*(height-1)) + blockSize - 1) / blockSize;
        computeIndexMap<<<numBlocksEnergy, blockSize, 0, streamLeft>>>(widthTemp, height, sobelInLeft, indexesLeft);
        computeIndexMap<<<numBlocksEnergy, blockSize, 0, streamRight>>>(widthTemp, height, sobelInRight, indexesRight);
        cudaDeviceSynchronize();

        int numBlocksSeams = ((rightBoundLeft - leftBoundLeft + 1) + blockSize - 1) / blockSize;
        calculateAllSeams<<<numBlocksSeams, blockSize, 0, streamLeft>>>(widthTemp, height, indexesLeft, sobelInLeft, seamsAllLeft, energySeamsLeft, leftBoundLeft, rightBoundLeft);
        calculateAllSeams<<<numBlocksSeams, blockSize, 0, streamRight>>>(widthTemp, height, indexesRight, sobelInRight, seamsAllRight, energySeamsRight, leftBoundRight, rightBoundRight);
        cudaDeviceSynchronize();

        keepFirstMotionSeamLeft<<<1,1, 0, streamLeft>>>(height, seamsAllLeft, energySeamsLeft, &seamsLeft[i*height], rightBoundLeft - leftBoundLeft + 1);
        keepFirstMotionSeamRight<<<1,1, 0, streamRight>>>(height, seamsAllRight, energySeamsRight, &seamsRight[i*height], rightBoundRight - leftBoundRight + 1);
        cudaDeviceSynchronize();

        // -- compute new sobel
        widthTemp--;
        rightBoundLeft--;
        rightBoundRight--;

        // -- remove seam from sobel
        int numBlocksRemove = ((widthTemp*height) + blockSize - 1) / blockSize;
        removeSeamFromSobel<<<numBlocksRemove, blockSize, 0, streamLeft>>>(widthTemp, height, sobelInLeft, sobelOutLeft, &seamsLeft[i*height]);
        removeSeamFromSobel<<<numBlocksRemove, blockSize, 0, streamRight>>>(widthTemp, height, sobelInRight, sobelOutRight, &seamsRight[i*height]);
        cudaDeviceSynchronize();
    }

    // -- repercute offset of previous seam to all next seams
    int numBlocksReper = (height + blockSize - 1) / blockSize;
    repercuteSeam<<<numBlocksReper, blockSize, 0, streamLeft>>>(nbAdd, seamsLeft, height);
    repercuteSeam<<<numBlocksReper, blockSize, 0, streamRight>>>(nbAdd, seamsRight, height);
    cudaDeviceSynchronize();

    // Free memory
    cudaFree(grayImageLeft);
    cudaFree(grayImageRight);

    cudaFree(sobelImageLeft);
    cudaFree(sobelImageRight);
    cudaFree(sobelImage2Left);
    cudaFree(sobelImage2Right);

    cudaFree(indexesLeft);
    cudaFree(indexesRight);
    cudaFree(seamsAllLeft);
    cudaFree(seamsAllRight);
    cudaFree(energySeamsLeft);
    cudaFree(energySeamsRight);

    // -- add new seams in order
    unsigned char* dataOutDeviceLeft;
    unsigned char* dataOutDevice2Left;
    unsigned char* dataOutDeviceRight;
    unsigned char* dataOutDevice2Right;
    cudaMalloc(&dataOutDeviceLeft, widthDst/2*height*3*sizeof(unsigned char));
    cudaMalloc(&dataOutDeviceRight, widthDst/2*height*3*sizeof(unsigned char));
    cudaMalloc(&dataOutDevice2Left, widthDst/2*height*3*sizeof(unsigned char));
    cudaMalloc(&dataOutDevice2Right, widthDst/2*height*3*sizeof(unsigned char));
    cudaMemcpy(dataOutDeviceLeft, dataRGBLeft, widthHalf*height*3*sizeof(unsigned char), cudaMemcpyDeviceToDevice);
    cudaMemcpy(dataOutDeviceRight, dataRGBRight, widthHalf*height*3*sizeof(unsigned char), cudaMemcpyDeviceToDevice);

    widthTemp = widthHalf+1;
    for (int i = 0; i < nbAdd; i++) {
        int numBlocksAdd = ((widthTemp*height*3) + blockSize - 1) / blockSize;
        if (i%2 == 0) {
            addSeamToImage<<<numBlocksAdd, blockSize, 0, streamLeft>>>(widthTemp, height, dataOutDeviceLeft, dataOutDevice2Left, &seamsLeft[i*height]);
            addSeamToImage<<<numBlocksAdd, blockSize, 0, streamRight>>>(widthTemp, height, dataOutDeviceRight, dataOutDevice2Right, &seamsRight[i*height]);
        }
        else {
            addSeamToImage<<<numBlocksAdd, blockSize, 0, streamLeft>>>(widthTemp, height, dataOutDevice2Left, dataOutDeviceLeft, &seamsLeft[i*height]);
            addSeamToImage<<<numBlocksAdd, blockSize, 0, streamRight>>>(widthTemp, height, dataOutDevice2Right, dataOutDeviceRight, &seamsRight[i*height]);
        }
        cudaDeviceSynchronize();
        widthTemp++;
    }

    unsigned char* dataOutDevice;
    cudaMalloc((void **)&dataOutDevice, widthDst*height*3*sizeof(unsigned char));
    int numBlocksCopy = ((widthDst*height*3) + blockSize - 1) / blockSize;
    if (nbAdd%2 == 0) {copyImage<<<numBlocksCopy,blockSize>>>(widthDst, height, dataOutDeviceLeft, dataOutDeviceRight, dataOutDevice);}
    else {copyImage<<<numBlocksCopy,blockSize>>>(widthDst, height, dataOutDevice2Left, dataOutDevice2Right, dataOutDevice);}
    cudaDeviceSynchronize();

    // -- copy final image
    cudaMemcpy(dataOut, dataOutDevice, widthDst*height*3*sizeof(unsigned char), cudaMemcpyDeviceToHost);

    cudaFree(dataRGB);
    cudaFree(dataRGBLeft);
    cudaFree(dataRGBRight);
    cudaFree(dataOutDevice);
    cudaFree(dataOutDeviceLeft);
    cudaFree(dataOutDeviceRight);
    cudaFree(dataOutDevice2Left);
    cudaFree(dataOutDevice2Right);
    cudaFree(seamsLeft);
    cudaFree(seamsRight);
    cudaStreamDestroy(streamLeft);
    cudaStreamDestroy(streamRight);
}