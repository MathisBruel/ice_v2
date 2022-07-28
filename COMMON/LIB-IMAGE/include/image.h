#include "png.h"
#include "tiffio.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include "Poco/Path.h"
#include "Poco/File.h"

#pragma once
#include "color.h"
#include "Poco/Logger.h"

enum class FillPosition
{
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_RIGHT,
	CENTER
};

class Image
{

public:
	
	Image();
	Image(Image* img);
	Image(std::string filename);
	Image(int width, int height, uint32_t format, unsigned char* data);
	Image(int width, int height, uint32_t format, uint8_t red, uint8_t green, uint8_t blue);
	~Image();

	// -- loader of image
	bool openImageFromStream(std::string filename, std::istream* stream);
	bool openImage(uint32_t format = PNG_FORMAT_RGBA);
	bool openImageFromBuffer(unsigned char* buffer, int size);
	bool openImage(std::string filename, uint32_t format = PNG_FORMAT_RGBA);

	// -- resizing algorithm
	bool resizeBilinear(int newWidth, int newHeight);
	bool resizeHermite(int newWidth, int newHeight);
	bool resizeBell(int newWidth, int newHeight);
	bool resizeBicubic(int newWidth, int newHeight);

	// -- saver image
	bool saveImage();
	bool saveImage(std::string filename);

	// -- extracting of sub part of image
	Image* extractData(int offsetX, int offsetY, int newWidth, int newHeight);
	Image* extractDataWithFillBlack(int offsetSrcX, int offsetSrcY, int widthSrc, int heightSrc, int offsetDstX, int offsetDstY, int widthDst, int heightDst);

	// -- pixel accessors
	unsigned char getValueAt(int x, int y, int channel);
	void setValueAt(int x, int y, int channel, unsigned char value);
	Color* getValueAt(int x, int y);
	void setValueAt(int x, int y, Color color);
	void setValueAt(int x, int y, int widthRect, int heightRect, Color color);
	int getSizeBuffer() {return bufferSize;}

	// -- RGBA to RGB buffer
	Image* convertToRGB();

	inline void setFilename(std::string filename) {this->filename = filename;}
	inline std::string getFilename() {return filename;}
	inline uint8_t* getData() {return data;}
	
	int getNbChannels() {return nbChannels;}
	int getWidth() {return width;}
	int getHeight() {return height;}
	uint32_t getFormat() {return format;}

	// -- merging images
	Image* blendWithForeImage(Image* fore);
	Image* mixWithForeImage(Image* fore);

	
private:

	static double getKernelHermite(double point);
	static double getKernelBell(double point);
	static double getKernelBicubic(double point);
	void computeNbChannels();

	unsigned char* convertFormatToGlobal(uint32_t* data32);
	unsigned char* convertToLine(int y);
	static void userReadData(png_structp pngPtr, png_bytep data, png_size_t length);


	std::string filename;
	int width, height;
	uint32_t format;
	int nbChannels;

	unsigned char* data;
	int bufferSize;


	// -------------------------------------
	//	Temporary structures for resizing 
	// -------------------------------------

	struct PixelCalculation {
		int pixel; 						// -- index of pixel index (in width or height) that contribute to computation
		double weight;					// -- weight of that pixel
	};

	struct PixelCalculationList {
		int nbPixel;					// -- number of pixel that contributes to computation of new pixel
		PixelCalculation* pixelList; 	// -- list of pixel that contributes to computation (index + associated weight)
		double weightSum;				// -- sum of weight of all pixel that contributes
	};
};