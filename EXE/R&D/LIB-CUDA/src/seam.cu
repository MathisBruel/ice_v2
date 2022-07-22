#include <seam.cuh>

__global__ void cutFrame(int width, int height, int sizeBuffer, unsigned char* dataRGB, unsigned char* left, unsigned char* right)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int idx = index; idx < width*height*3*sizeBuffer; idx+=stride) {

        int comp = idx%3;
        int idxTemp = (idx-comp)/3;
        int i = idxTemp%width;
        int tmp = (idxTemp-i)/width;
        int j = tmp%height;
        int frame = (tmp - j)/height;

        int tempWidth = width/2;

        if (i < tempWidth) {
            left[((frame*height+j)*tempWidth+i)*3+comp] = dataRGB[idx];
        }
        else {
            right[(((frame*height+j)*tempWidth)+i-tempWidth)*3+comp] = dataRGB[idx];
        }
    }
}
__global__ void copyImage(int width, int height, int sizeBuffer, unsigned char* dataInLeft, unsigned char* dataInRight, unsigned char* dataOut) 
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int idx = index; idx < width*height*3*sizeBuffer; idx+=stride) {
        
        unsigned char comp = idx%3;
        int idxTemp = (idx-comp)/3;
        short i = idxTemp%width;
        int tmp = (idxTemp-i)/width;
        int j = tmp%height;
        int frame = (tmp - j)/height;
        int tempWidth = width/2;

        if (i < tempWidth) {
            dataOut[idx] = dataInLeft[(((frame*height+j)*tempWidth)+i)*3+comp];
        }
        else {
            dataOut[idx] = dataInRight[(((frame*height+j)*tempWidth)+i-tempWidth)*3+comp];
        }
    }
}

__global__ void grayscale(int width, int height, int sizeBuffer, unsigned char* dataRGB, float* dataOutGray)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int i = index; i < width*height*sizeBuffer; i+=stride) {
        dataOutGray[i] = (float)(dataRGB[i*3])*0.2989f + (float)(dataRGB[i*3+1])*0.587f + (float)(dataRGB[i*3+2])*0.114f;
        dataOutGray[i] /= 255;
    }
}
__global__ void sobel(int width, int height, int sizeBuffer, float* dataInGray, float* dataOutSobel)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int idx = index; idx < height*width*sizeBuffer; idx+=stride) {

        int i = idx%width;
        int j = (idx - i)/width;
        j %= height;

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
        dataOutSobel[idx] = sqrt(dx*dx + dy*dy);
    }
}
__global__ void mergeSobel(int width, int height, int frame, int size, float* dataInSobel, float* dataOutSobel)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int idx = index; idx < height*width; idx+=stride) {
        if (frame == 0) {
            dataOutSobel[idx] += dataInSobel[frame*width*height+idx]/size;
        }
        else {
            dataOutSobel[idx] += (fabs(dataInSobel[frame*width*height+idx] - dataInSobel[(frame-1)*width*height+idx])*0.7 + dataInSobel[frame*width*height+idx]*0.3)/size;
        }
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
__global__ void addSeamToImage(int width, int height, int sizeBuffer, unsigned char* dataIn, unsigned char* dataOut, int* seam)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int idx = index; idx < width*height*3*sizeBuffer; idx+=stride) {

        int comp = idx%3;
        int idxTemp = (idx-comp)/3;
        int i = idxTemp%width;
        int tmp = (idxTemp-i)/width;
        int j = tmp%height;
        int frame = (tmp - j)/height;
        int idx2 = ((frame*height+j)*(width-1)+i)*3+comp;

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

__global__ void keepFirstSeam(int height, int* seamsTemporaries, float* energy, int* seam, int sizeWindow)
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
__global__ void keepFirstSeamRight(int height, int* seamsTemporaries, float* energy, int* seam, int sizeWindow)
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

void seamResizeVideo(int width, int height, int widthDst, int sizeBuffer, unsigned char** dataIn, unsigned char** dataOut)
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

    // -- mean sobel for all frames
    float* sobelAllLeft;
    float* sobelAllRight;
    cudaMalloc((void **)&sobelAllLeft, widthHalf*height*sizeof(float));
    cudaMalloc((void **)&sobelAllRight, widthHalf*height*sizeof(float));
    cudaMemset(sobelAllLeft, 0, widthHalf*height*sizeof(float));
    cudaMemset(sobelAllRight, 0, widthHalf*height*sizeof(float));
    float* sobelAll2Left;
    float* sobelAll2Right;
    cudaMalloc((void **)&sobelAll2Left, widthHalf*height*sizeof(float));
    cudaMalloc((void **)&sobelAll2Right, widthHalf*height*sizeof(float));

    // -- max 120 frames at the time
    int nbFramesTreated = 0;
    while (nbFramesTreated < sizeBuffer) {

        int nbToTreat = 120;
        if (sizeBuffer - nbFramesTreated < 120) {
            nbToTreat = sizeBuffer - nbFramesTreated;
        }

        // -- copy datas to GPU
        unsigned char* dataRGB;
        cudaMalloc(&dataRGB, width*height*3*nbToTreat*sizeof(unsigned char));
        for (int i = 0; i < nbToTreat; i++) {
            cudaMemcpy(&dataRGB[i*width*height*3], dataIn[nbFramesTreated+i], width*height*3*sizeof(unsigned char), cudaMemcpyHostToDevice);
        }

        // -- separate left and right
        unsigned char* dataRGBLeft;
        unsigned char* dataRGBRight;
        int numBlocksSplit = ((width*height*3*nbToTreat) + blockSize - 1) / blockSize;
        cudaMalloc(&dataRGBLeft, widthHalf*height*3*nbToTreat*sizeof(unsigned char));
        cudaMalloc(&dataRGBRight, widthHalf*height*3*nbToTreat*sizeof(unsigned char));
        cutFrame<<<numBlocksSplit, blockSize>>>(width, height, nbToTreat, dataRGB, dataRGBLeft, dataRGBRight);
        cudaDeviceSynchronize();

        // -- calculate grayscale image
        float* grayImageLeft;
        float* grayImageRight;
        int numBlocksFilter = ((widthHalf*height*nbToTreat) + blockSize - 1) / blockSize;
        cudaMalloc((void **)&grayImageLeft, widthHalf*height*nbToTreat*sizeof(float));
        cudaMalloc((void **)&grayImageRight, widthHalf*height*nbToTreat*sizeof(float));
        grayscale<<<numBlocksFilter, blockSize, 0, streamLeft>>>(widthHalf, height, nbToTreat, dataRGBLeft, grayImageLeft);
        grayscale<<<numBlocksFilter, blockSize, 0, streamRight>>>(widthHalf, height, nbToTreat, dataRGBRight, grayImageRight);
        cudaDeviceSynchronize();

        // -- calculate sobel
        float* sobelImageLeft;
        float* sobelImageRight;
        cudaMalloc((void **)&sobelImageLeft, widthHalf*height*nbToTreat*sizeof(float));
        cudaMalloc((void **)&sobelImageRight, widthHalf*height*nbToTreat*sizeof(float));
        // -- energy definition
        sobel<<<numBlocksFilter, blockSize, 0, streamLeft>>>(widthHalf, height, nbToTreat, grayImageLeft, sobelImageLeft);
        sobel<<<numBlocksFilter, blockSize, 0, streamRight>>>(widthHalf, height, nbToTreat, grayImageRight, sobelImageRight);
        cudaDeviceSynchronize();

        // -- global energy definition
        int numBlocksMerge = ((widthHalf*height) + blockSize - 1) / blockSize;
        for (int i = 0; i < nbToTreat; i++) {
            mergeSobel<<<numBlocksMerge, blockSize, 0, streamLeft>>>(widthHalf, height, i, sizeBuffer, sobelImageLeft, sobelAllLeft);
            mergeSobel<<<numBlocksMerge, blockSize, 0, streamRight>>>(widthHalf, height, i, sizeBuffer, sobelImageRight, sobelAllRight);
            cudaDeviceSynchronize();
        }

        // Free temporary sobel memory
        cudaFree(dataRGB);
        cudaFree(dataRGBLeft);
        cudaFree(dataRGBRight);
        cudaFree(grayImageLeft);
        cudaFree(grayImageRight);
        cudaFree(sobelImageLeft);
        cudaFree(sobelImageRight);

        nbFramesTreated += nbToTreat;
    }

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
            sobelInLeft = sobelAllLeft;
            sobelOutLeft = sobelAll2Left;
            sobelInRight = sobelAllRight;
            sobelOutRight = sobelAll2Right;
        }
        else {
            sobelInLeft = sobelAll2Left;
            sobelOutLeft = sobelAllLeft;
            sobelInRight = sobelAll2Right;
            sobelOutRight = sobelAllRight;
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

        keepFirstSeam<<<1,1, 0, streamLeft>>>(height, seamsAllLeft, energySeamsLeft, &seamsLeft[i*height], rightBoundLeft - leftBoundLeft + 1);
        keepFirstSeamRight<<<1,1, 0, streamRight>>>(height, seamsAllRight, energySeamsRight, &seamsRight[i*height], rightBoundRight - leftBoundRight + 1);
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

    // Free temporary seam memory
    cudaFree(sobelAllLeft);
    cudaFree(sobelAllRight);
    cudaFree(sobelAll2Left);
    cudaFree(sobelAll2Right);
    cudaFree(indexesLeft);
    cudaFree(indexesRight);
    cudaFree(seamsAllLeft);
    cudaFree(seamsAllRight);
    cudaFree(energySeamsLeft);
    cudaFree(energySeamsRight);

    nbFramesTreated = 0;
    while (nbFramesTreated < sizeBuffer) {

        int nbToTreat = 120;
        if (sizeBuffer - nbFramesTreated < 120) {
            nbToTreat = sizeBuffer - nbFramesTreated;
        }

        // -- copy datas to GPU
        unsigned char* dataRGB;
        cudaMalloc(&dataRGB, width*height*3*nbToTreat*sizeof(unsigned char));
        for (int i = 0; i < nbToTreat; i++) {
            cudaMemcpy(&dataRGB[i*width*height*3], dataIn[nbFramesTreated+i], width*height*3*sizeof(unsigned char), cudaMemcpyHostToDevice);
        }

        // -- separate left and right
        unsigned char* dataRGBLeft;
        unsigned char* dataRGBRight;
        int numBlocksSplit = ((width*height*3*nbToTreat) + blockSize - 1) / blockSize;
        cudaMalloc(&dataRGBLeft, widthHalf*height*3*nbToTreat*sizeof(unsigned char));
        cudaMalloc(&dataRGBRight, widthHalf*height*3*nbToTreat*sizeof(unsigned char));
        cutFrame<<<numBlocksSplit, blockSize>>>(width, height, nbToTreat, dataRGB, dataRGBLeft, dataRGBRight);
        cudaDeviceSynchronize();

        // -- add new seams in order
        unsigned char* dataOutDeviceLeft;
        unsigned char* dataOutDevice2Left;
        unsigned char* dataOutDeviceRight;
        unsigned char* dataOutDevice2Right;
        cudaMalloc(&dataOutDeviceLeft, widthDst/2*height*3*nbToTreat*sizeof(unsigned char));
        cudaMalloc(&dataOutDeviceRight, widthDst/2*height*3*nbToTreat*sizeof(unsigned char));
        cudaMalloc(&dataOutDevice2Left, widthDst/2*height*3*nbToTreat*sizeof(unsigned char));
        cudaMalloc(&dataOutDevice2Right, widthDst/2*height*3*nbToTreat*sizeof(unsigned char));
        cudaMemcpy(dataOutDeviceLeft, dataRGBLeft, widthHalf*height*3*nbToTreat*sizeof(unsigned char), cudaMemcpyDeviceToDevice);
        cudaMemcpy(dataOutDeviceRight, dataRGBRight, widthHalf*height*3*nbToTreat*sizeof(unsigned char), cudaMemcpyDeviceToDevice);

        widthTemp = widthHalf+1;
        for (int i = 0; i < nbAdd; i++) {
            int numBlocksAdd = ((widthTemp*height*3*nbToTreat) + blockSize - 1) / blockSize;
            if (i%2 == 0) {
                addSeamToImage<<<numBlocksAdd, blockSize, 0, streamLeft>>>(widthTemp, height, nbToTreat, dataOutDeviceLeft, dataOutDevice2Left, &seamsLeft[i*height]);
                addSeamToImage<<<numBlocksAdd, blockSize, 0, streamRight>>>(widthTemp, height, nbToTreat, dataOutDeviceRight, dataOutDevice2Right, &seamsRight[i*height]);
            }
            else {
                addSeamToImage<<<numBlocksAdd, blockSize, 0, streamLeft>>>(widthTemp, height, nbToTreat, dataOutDevice2Left, dataOutDeviceLeft, &seamsLeft[i*height]);
                addSeamToImage<<<numBlocksAdd, blockSize, 0, streamRight>>>(widthTemp, height, nbToTreat, dataOutDevice2Right, dataOutDeviceRight, &seamsRight[i*height]);
            }
            cudaDeviceSynchronize();
            widthTemp++;
        }

        unsigned char* dataOutDevice;
        cudaMalloc((void **)&dataOutDevice, widthDst*height*3*nbToTreat*sizeof(unsigned char));
        int numBlocksCopy = ((widthDst*height*3*nbToTreat) + blockSize - 1) / blockSize;
        if (nbAdd%2 == 0) {copyImage<<<numBlocksCopy,blockSize>>>(widthDst, height, nbToTreat, dataOutDeviceLeft, dataOutDeviceRight, dataOutDevice);}
        else {copyImage<<<numBlocksCopy,blockSize>>>(widthDst, height, nbToTreat, dataOutDevice2Left, dataOutDevice2Right, dataOutDevice);}
        cudaDeviceSynchronize();

        // -- copy final image
        for (int i = 0; i < nbToTreat; i++) {
            cudaMemcpy(dataOut[nbFramesTreated+i], &dataOutDevice[i*widthDst*height*3], widthDst*height*3*sizeof(unsigned char), cudaMemcpyDeviceToHost);
        }

        // Free final images
        cudaFree(dataRGB);
        cudaFree(dataRGBLeft);
        cudaFree(dataRGBRight);
        cudaFree(dataOutDevice);
        cudaFree(dataOutDeviceLeft);
        cudaFree(dataOutDeviceRight);
        cudaFree(dataOutDevice2Left);
        cudaFree(dataOutDevice2Right);

        nbFramesTreated += nbToTreat;
    }

    // Free global memory
    cudaFree(seamsLeft);
    cudaFree(seamsRight);
    cudaStreamDestroy(streamLeft);
    cudaStreamDestroy(streamRight);
}