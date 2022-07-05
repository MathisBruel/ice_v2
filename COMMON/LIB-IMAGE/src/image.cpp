#include "image.h"

Image::Image() 
{
	data = nullptr;
}

Image::Image(std::string filename)
{
	this->filename = filename;
	data = nullptr;
}

Image::Image(Image* imgToCopy)
{
	this->width = imgToCopy->width;
	this->height = imgToCopy->height;
	this->format = imgToCopy->format;
	computeNbChannels();
	this->bufferSize = width*height*nbChannels;

	data = new unsigned char[this->bufferSize];
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int index = (i*width+j)*nbChannels;
			data[index] = imgToCopy->data[index];
			data[index+1] = imgToCopy->data[index+1];
			data[index+2] = imgToCopy->data[index+2];
		}
	}
}

Image::Image(int width, int height, uint32_t format, unsigned char* data)
{
	this->width = width;
	this->height = height;
	this->format = format;
	this->data = data;
	computeNbChannels();
	this->bufferSize = width*height*nbChannels;
}

Image::Image(int width, int height, uint32_t format, uint8_t red, uint8_t green, uint8_t blue) {
	this->width = width;
	this->height = height;
	this->format = format;
	computeNbChannels();
	this->bufferSize = width*height*nbChannels;
	
	this->data = new unsigned char[this->bufferSize];
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int index = (i*width+j)*3;
			data[index] = red;
			data[index+1] = green;
			data[index+2] = blue;
		}
	}
}

Image::~Image()
{
	try {
		if (data != nullptr) {
			delete[] data;
		}
		data = nullptr;
	}
	catch (std::exception &e) {
		Poco::Logger::get("Image").warning("Image already deleted !", __FILE__, __LINE__);
	}
}

void Image::computeNbChannels()
{
	// -- others format not handled
	switch (format) {
		case PNG_FORMAT_RGB :
		nbChannels = 3;
		break;

		case PNG_FORMAT_RGBA :
		nbChannels = 4;
		break;

		default :
		Poco::Logger::get("Image").error("Format image unknow or unhandled", __FILE__, __LINE__);
		nbChannels = 0;
		break;
	}
}

Image* Image::convertToRGB()
{
	// -- no need to be converted
	if (nbChannels == 3) {
		return this;
	}

	// -- assume format is already RGBA
	unsigned char* newData = new unsigned char[width*height*3];
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			newData[((i*width)+j)*3] = data[((i*width)+j)*4];
			newData[((i*width)+j)*3+1] = data[((i*width)+j)*4+1];
			newData[((i*width)+j)*3+2] = data[((i*width)+j)*4+2];
		}
	}

	Image* img = new Image(width, height, PNG_FORMAT_RGB, newData);
	return img;
}

bool Image::openImageFromStream(std::string filename, std::istream* stream)
{
	this->filename = filename;

	// -- check signature
	uint8_t* signature = new uint8_t[8];
	stream->read((char*)signature, 8);
	if (!stream->good()) {
		Poco::Logger::get("Image").error("Can't read signature from file " + filename, __FILE__, __LINE__);
		return false;
	}
	if (png_sig_cmp(signature, 0, 8) != 0) {
		Poco::Logger::get("Image").error("Signature of file is incorrect " + filename, __FILE__, __LINE__);
		return false;
	}


	// -- create PNG read struct (equivalent to file handle)
	// -- nullptr : we don't need actually to get all errors and warnings at this point 
	png_structp pngFd = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (pngFd == nullptr) {
		Poco::Logger::get("Image").error("Can't allocate read png structure", __FILE__, __LINE__);
		return false;
	}

	// -- create PNG info struct (informations of image)
	png_infop info = png_create_info_struct(pngFd);
	if (info == nullptr) {
		png_destroy_read_struct(&pngFd, nullptr, nullptr);
		Poco::Logger::get("Image").error("Can't allocate info png structure", __FILE__, __LINE__);
		return false;
	}

	// -- handling errors when reading by jumping to this following section
	if (setjmp(png_jmpbuf(pngFd))) {
		png_destroy_read_struct(&pngFd, &info, nullptr);
		Poco::Logger::get("Image").error("Error while reading image", __FILE__, __LINE__);
		return false;
	}

	// -- we override the reading function by ours
	png_set_read_fn(pngFd,(png_voidp)stream, userReadData);

	// -- Skip the signature bytes we've already read
    png_set_sig_bytes(pngFd, 8);

    // -- we read info
    png_read_info(pngFd, info);

	// -- update the informations
	width =  png_get_image_width(pngFd, info);
    height = png_get_image_height(pngFd, info);
    nbChannels = png_get_channels(pngFd, info);
	bufferSize = width*height*nbChannels;
	if (nbChannels == 4) {
		this->format = PNG_FORMAT_RGBA;
	} else if (nbChannels == 3) {
		this->format = PNG_FORMAT_RGB;
	}

	// -- skip if not in the right format
	if (nbChannels < 3) {
		png_destroy_read_struct(&pngFd, &info, nullptr);
		Poco::Logger::get("Image").error("We only handle RGB or RGBA image file !", __FILE__, __LINE__);
		return false;
	}

    // -- Array of row pointers. One for every row.
    uint8_t** rows = new uint8_t*[height];

    // -- Allocate a buffer with enough space.
    data = new unsigned char[bufferSize];
    // -- This is the length in bytes, of one row.
    unsigned int stride = width * nbChannels;

    for (int i = 0; i < height; i++) {
		//uint32_t q = (height - i - 1) * stride;
        uint32_t q = i * stride;
        rows[i] = (png_bytep)data + q;
    }

	// -- actual reading
    png_read_image(pngFd, rows);

    //And don't forget to clean up the read and info structs !
	delete[] (png_bytep)rows;
    png_destroy_read_struct(&pngFd, &info,(png_infopp)0);
	
	return true;
}

bool Image::openImage(std::string filename, uint32_t format)
{
	this->filename = filename;
	return openImage(format);
}

bool Image::openImage(uint32_t format)
{
	// -- inti path to check extension
	Poco::Path pathFile(filename);
	Poco::File file(filename);

	if (!file.exists()) {
		Poco::Logger::get("Image").error(filename + " does not exists : can't be opened !", __FILE__, __LINE__);
		return false;
	}
	
	// -- TIFF
	if (pathFile.getExtension() == "tiff" || pathFile.getExtension() == "tif") {

		TIFF* tiff = TIFFOpen(filename.c_str(), "r");
		if (tiff == nullptr) {
			Poco::Logger::get("Image").error("Error when opening tiff file " + filename, __FILE__, __LINE__);
			return false;
		}

		// -- get headers infos
		TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height);
		TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &nbChannels); 

		// -- set format
		if (nbChannels == 4) {
			this->format = PNG_FORMAT_RGBA;
		} else if (nbChannels == 3) {
			this->format = PNG_FORMAT_RGB;
		}

		// -- allocate buffer data
		bufferSize = width*height*nbChannels;
		uint32_t* data32 = (uint32_t*)malloc(width*height * sizeof(uint32_t));

		// -- read dats
		if (TIFFReadRGBAImage(tiff, width, height, data32, 0)) {

			// -- conver to uint8_t
			data = convertFormatToGlobal(data32);
			std::free(data32);
		} 
		else {
			Poco::Logger::get("Image").error("Error when reading tiff image " + filename, __FILE__, __LINE__);
			TIFFClose(tiff);
			return false;
		}

		TIFFClose(tiff);
		return true;
	}

	// -- exit if not png
	else if (pathFile.getExtension() == "png") {

		png_image* image = new png_image();
		image->version = PNG_IMAGE_VERSION;
		image->opaque = nullptr;
		image->flags = 0;
		image->colormap_entries = 0;

		if (!png_image_begin_read_from_file(image, filename.c_str()))
		{
			Poco::Logger::get("Image").error("Error at begin read image " + filename, __FILE__, __LINE__);
			delete image;
			return false;
		}

		image->format = format;
		this->format = format;
		width = image->width;
		height = image->height;
		computeNbChannels();

		bufferSize = PNG_IMAGE_SIZE(*image);
		data = new unsigned char[bufferSize];

		if (!png_image_finish_read(image, 0, data, 0, 0))
		{
			Poco::Logger::get("Image").error("Error at reading image " + filename, __FILE__, __LINE__);
			delete image;
			return false;
		}

		delete image;
		return true;
	}

	Poco::Logger::get("Image").error("Image file with extension " + pathFile.getExtension() + " not handled", __FILE__, __LINE__);
	return false;
}

bool Image::openImageFromBuffer(unsigned char* buffer, int size)
{
	// -- TIFF
	// -- TODO

	png_image* image = new png_image();
	image->version = PNG_IMAGE_VERSION;
	image->opaque = nullptr;
	image->flags = 0;
	image->colormap_entries = 0;

	if (!png_image_begin_read_from_memory(image, buffer, size))
	{
		Poco::Logger::get("Image").error("Error at begin read image from memory", __FILE__, __LINE__);
		delete image;
		return false;
	}

	image->format = format;
	this->format = format;
	width = image->width;
	height = image->height;
	computeNbChannels();

	bufferSize = PNG_IMAGE_SIZE(*image);
	data = new unsigned char[bufferSize];

	if (!png_image_finish_read(image, 0, data, 0, 0))
	{
		Poco::Logger::get("Image").error("Error at reading image " + filename, __FILE__, __LINE__);
		delete image;
		return false;
	}

	delete image;
	return true;
}

unsigned char* Image::convertFormatToGlobal(uint32_t* data32)
{
	unsigned char* buffer = new unsigned char[width*height*nbChannels];

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {

			uint32_t value = data32[((height-1-i)*width+j)];
			int alpha = value >> 24;
			int red = (value & 0xFF0000) >> 16;
			int green = (value & 0xFF00) >> 8;
			int blue = (value & 0xFF);
			
			buffer[(i*width+j)*nbChannels] = red;
			buffer[(i*width+j)*nbChannels+1] = green;
			buffer[(i*width+j)*nbChannels+2] = blue;

			if (format == PNG_FORMAT_RGBA) {
				buffer[(i*width+j)*nbChannels+3] = alpha;
			}
		}
	}
	return buffer;
}

unsigned char* Image::convertToLine(int y)
{
	unsigned char* line = new unsigned char[width * nbChannels];

	for (int i = 0; i < width; i++) {
		for (int c = 0; c < nbChannels; c++) {
			
			int index = (y*width+i)*nbChannels+c;
			line[i*nbChannels+c] = data[index];
		}
	}

	return line;
}

void Image::userReadData(png_structp pngPtr, png_bytep data, png_size_t length) {
    //Here we get our IO pointer back from the read struct.
    //This is the parameter we passed to the png_set_read_fn() function.
    //Our std::istream pointer.
    png_voidp stream = png_get_io_ptr(pngPtr);
    //Cast the pointer to std::istream* and read 'length' bytes into 'data'
    ((std::istream*)stream)->read((char*)data, length);
}

bool Image::resizeBilinear(int newWidth, int newHeight)
{
	if (width == newWidth && height == newHeight) {
		return true;
	}

	// assure image source is correct
	if (data != nullptr) {

		// -- define new size 
		uint32_t pngImageSize = newWidth*newHeight*nbChannels;
		unsigned char* newData = new unsigned char[pngImageSize];

		// calculate ratio between old and new image
		double x_ratio = ((double)(width-1))/newWidth;
    	double y_ratio = ((double)(height-1))/newHeight;

		int offset = 0;

		// compute for all new pixels
		for (int i=0 ; i < newHeight ; i++) {
			for (int j=0 ; j < newWidth ; j++) {

				// value of index in older image
				int x = (int)(x_ratio * j);
				int y = (int)(y_ratio * i);
				
				// value normalize corresponding in core convolution
				double x_diff = (x_ratio * j) - x;
				double y_diff = (y_ratio * i) - y;

				// compute for all channels
				for (int channel = 0; channel < nbChannels; channel++) {

					// get all pixel reference to calculate new pixel value
					unsigned char valueFinal = (unsigned char)(
						getValueAt(x, y, channel)*(1-x_diff)*(1-y_diff) + 
						getValueAt(x+1, y, channel)*x_diff*(1-y_diff) + 
						getValueAt(x, y+1, channel)*(1-x_diff)*y_diff + 
						getValueAt(x+1, y+1, channel)*x_diff*y_diff
					);

					newData[offset++] = valueFinal;
				}
			}
		}

		// attribute new values
		width = newWidth;
		height = newHeight;
		delete[] data;
		bufferSize = pngImageSize;
		data = new unsigned char[pngImageSize];
		std::memcpy(data, newData, bufferSize);
		delete[] newData;
		return true;
	} else {
		return false;
	}
}

bool Image::resizeBell(int newWidth, int newHeight)
{
	// -- value to be considered as 0 if lower than
	double Epsilon = 1e-12;
	double radiusKernel = 2.0;

	if (width == newWidth && height == newHeight) {
		return true;
	}

	// assure image source is correct
	if (data != nullptr) {

		double* pp = new double[nbChannels];

		// -- create first temporary image by resizing only by width
		Image* tempImg = new Image();
		tempImg->width = newWidth;
		tempImg->height = height;
		tempImg->format = format;
		tempImg->computeNbChannels();

		// -- allocate structure of pixel values
		tempImg->bufferSize = newWidth*height*nbChannels;
		tempImg->data = new unsigned char[tempImg->bufferSize];

		// -- temporary structure for computing new pixels value
		PixelCalculationList* computation = new PixelCalculationList[newWidth];

		// calculate ratio between old and new image
		double xScale = ((double)newWidth / (double)width);
		double yScale = ((double)newHeight / (double)height);

		// -- down-sizing along width
		if (xScale < 1.0) {

			// -- determine how many near pixels to be taken
			// -- depends on radius of kernel and scale 
			double sizeCheck = (radiusKernel / xScale);
			for (int i = 0; i < newWidth; i++)
                {
					// -- initialize structure
                    computation[i].nbPixel = 0;
                    computation[i].pixelList = new PixelCalculation[(int)floor(2.0 * sizeCheck + 1.0)];
                    computation[i].weightSum = 0.0;

					// -- determine center of pixel equivalent in source image
                    double center = ((i + 0.5) / xScale);
					// -- determine boundaries for use pixels calculation
                    int left = (int)(center - sizeCheck);
                    int right = (int)(center + sizeCheck);

                    for (int j = left; j <= right; j++)
                    {
						// -- calculate weight of pixel compared to center (equivalent of abcisse 0 in kernel)
						// -- function of scaling
                        double weight = getKernelBell((center - j - 0.5) * xScale);

						// -- exit condition to optimize processing
                        if (std::fabs(weight) < Epsilon || (j < 0) || (j >= width)) {continue;}

						// -- fill structure to calculate further pixel value with all entries in pixel list
                        computation[i].pixelList[computation[i].nbPixel].pixel = j;
                        computation[i].pixelList[computation[i].nbPixel].weight = weight;
                        computation[i].weightSum += weight;
                        computation[i].nbPixel++;
                    }
                }


		// -- up-sizing along width
		} else {
			for (int i = 0; i < newWidth; i++)
                {
                    // -- initialize structure
                    computation[i].nbPixel = 0;
                    computation[i].pixelList = new PixelCalculation[(int)floor(2.0 * radiusKernel + 1.0)];
                    computation[i].weightSum = 0.0;

					// -- determine center of pixel equivalent in source image
                    double center = ((i + 0.5) / xScale);
					// -- determine boundaries for use pixels calculation
                    int left = (int)(center - radiusKernel);
                    int right = (int)ceil(center + radiusKernel);

                    for (int j = left; j <= right; j++)
                    {
						// -- calculate weight of pixel compared to center (equivalent of abcisse 0 in kernel)
                        double weight = getKernelBell(center - j - 0.5);

						// -- exit condition to optimize processing
                        if (std::fabs(weight) < Epsilon || (j < 0) || (j >= width)) {continue;}

                        // -- fill structure to calculate further pixel value with all entries in pixel list
                        computation[i].pixelList[computation[i].nbPixel].pixel = j;
                        computation[i].pixelList[computation[i].nbPixel].weight = weight;
                        computation[i].weightSum += weight;
                        computation[i].nbPixel++;
                    }
                }
		}

		// -- computation of real temporary pixel new values with width-only resizing
		for (int k = 0; k < height; k++)
		{
			for (int i = 0; i < newWidth; i++)
			{
				// -- compute all values of contributor pixels and sum all of them
				for (int channel = 0; channel < nbChannels; channel++) {
					pp[channel] = 0;
				}

				for (int j = 0; j < computation[i].nbPixel; j++)
				{
					int offset = computation[i].pixelList[j].pixel;
					double weight = computation[i].pixelList[j].weight;

					if (std::fabs(weight) < Epsilon) {continue;}

					for (int channel = 0; channel < nbChannels; channel++) {
						pp[channel] += (getValueAt(offset, k, channel) * weight);
					}
				}

				// -- set final value by average all sum by the sum of all weight
				for (int channel = 0; channel < nbChannels; channel++) {
					tempImg->setValueAt(i, k, channel, (pp[channel] / computation[i].weightSum) + 0.5);
				}
			}
		}

		// -- create final image by resizing by height basing of temporary image (only width)
		Image* newImage = new Image();
		newImage->width = newWidth;
		newImage->height = newHeight;
		newImage->format = format;
		newImage->computeNbChannels();

		// -- allocate structure of pixel values
		newImage->bufferSize = newWidth*newHeight*nbChannels;
		newImage->data = new unsigned char[newImage->bufferSize];

		// -- temporary structure for computing new pixels value
		computation = new PixelCalculationList[newHeight];

		// -- down-sizing along height
		if (yScale < 1.0)
		{
			// -- determine how many near pixels to be taken
			// -- depends on radius of kernel and scale 
			double sizeCheck = (radiusKernel / yScale);

			for (int i = 0; i < newHeight; i++)
			{
				// -- initialize structure
				computation[i].nbPixel = 0;
				computation[i].pixelList = new PixelCalculation[(int)floor(2.0 * sizeCheck + 1.0)];
				computation[i].weightSum = 0.0;

				// -- determine center of pixel equivalent in source image
				double center = ((i + 0.5) / yScale);
				// -- determine boundaries for use pixels calculation
				int left = (int)(center - sizeCheck);
				int right = (int)(center + sizeCheck);

				for (int j = left; j <= right; j++)
				{
					// -- calculate weight of pixel compared to center (equivalent of abcisse 0 in kernel)
					// -- function of scaling
					double weight = getKernelBell((center - j - 0.5) * yScale);

					// -- exit condition to optimize processing
					if (std::fabs(weight) < Epsilon || (j < 0) || (j >= height)) {continue;}

					// -- fill structure to calculate further pixel value with all entries in pixel list
					computation[i].pixelList[computation[i].nbPixel].pixel = j;
					computation[i].pixelList[computation[i].nbPixel].weight = weight;
					computation[i].weightSum += weight;
					computation[i].nbPixel++;
				}
			}

		// -- up-sizing along height
		} else {
			for (int i = 0; i < newHeight; i++)
			{
				// -- initialize structure
				computation[i].nbPixel = 0;
				computation[i].pixelList = new PixelCalculation[(int)floor(2.0 * 2.0 + 1.0)];
				computation[i].weightSum = 0.0;

				// -- determine center of pixel equivalent in source image
				double center = ((i + 0.5) / yScale);
				// -- determine boundaries for use pixels calculation
				int left = (int)(center - radiusKernel);
				int right = (int)(center + radiusKernel);

				for (int j = left; j <= right; j++)
				{
					// -- calculate weight of pixel compared to center (equivalent of abcisse 0 in kernel)
					double weight = getKernelBell(center - j - 0.5);

					// -- exit condition to optimize processing
					if (std::fabs(weight) < Epsilon || (j < 0) || (j >= height)) {continue;}

					// -- fill structure to calculate further pixel value with all entries in pixel list
					computation[i].pixelList[computation[i].nbPixel].pixel = j;
					computation[i].pixelList[computation[i].nbPixel].weight = weight;
					computation[i].weightSum += weight;
					computation[i].nbPixel++;
				}
			}
		}

		// -- computation of real pixel new values with height-only resizing (basing on already width resizing)
		for (int k = 0; k < newWidth; k++)
		{
			for (int i = 0; i < newHeight; i++)
			{
				// -- compute all values of contributor pixels and sum all of them
				for (int channel = 0; channel < nbChannels; channel++) {
					pp[channel] = 0;
				}

				for (int j = 0; j < computation[i].nbPixel; j++)
				{
					int offset = computation[i].pixelList[j].pixel;
					double weight = computation[i].pixelList[j].weight;
					if (std::fabs(weight) < Epsilon)
					{
						continue;
					}

					for (int channel = 0; channel < nbChannels; channel++) {
						pp[channel] += (tempImg->getValueAt(k, offset, channel) * weight);
					}
				}

				// -- set final value by average all sum by the sum of all weight
				for (int channel = 0; channel < nbChannels; channel++) {
					newImage->setValueAt(k, i, channel, (pp[channel] / computation[i].weightSum) + 0.5);
				}
			}
		}

		// attribute new values
		width = newImage->width;
		height = newImage->height;
		delete tempImg;
		delete[] data;
		data = new unsigned char[newImage->bufferSize];
		std::memcpy(data, newImage->data, newImage->bufferSize*sizeof(unsigned char));
		bufferSize = newImage->bufferSize;
		delete newImage;
		return true;
	} 
	else {
		return false;
	}
}

bool Image::resizeBicubic(int newWidth, int newHeight)
{
	// -- value to be considered as 0 if lower than
	double Epsilon = 1e-12;
	double radiusKernel = 2.0;

	if (width == newWidth && height == newHeight) {
		return true;
	}

	// assure image source is correct
	if (data != nullptr) {

		double* pp = new double[nbChannels];

		// -- create first temporary image by resizing only by width
		Image* tempImg = new Image();
		tempImg->width = newWidth;
		tempImg->height = height;
		tempImg->format = format;
		tempImg->computeNbChannels();

		// -- allocate structure of pixel values
		tempImg->bufferSize = newWidth*height*nbChannels;
		tempImg->data = new unsigned char[tempImg->bufferSize];

		// -- temporary structure for computing new pixels value
		PixelCalculationList* computation = new PixelCalculationList[newWidth];

		// calculate ratio between old and new image
		double xScale = ((double)newWidth / width);
		double yScale = ((double)newHeight / height);

		// -- down-sizing along width
		if (xScale < 1.0) {

			// -- determine how many near pixels to be taken
			// -- depends on radius of kernel and scale 
			double sizeCheck = (radiusKernel / xScale);
			for (int i = 0; i < newWidth; i++)
                {
					// -- initialize structure
                    computation[i].nbPixel = 0;
                    computation[i].pixelList = new PixelCalculation[(int)floor(2.0 * sizeCheck + 1.0)];
                    computation[i].weightSum = 0.0;

					// -- determine center of pixel equivalent in source image
                    double center = ((i + 0.5) / xScale);
					// -- determine boundaries for use pixels calculation
                    int left = (int)(center - sizeCheck);
                    int right = (int)(center + sizeCheck);

                    for (int j = left; j <= right; j++)
                    {
						// -- calculate weight of pixel compared to center (equivalent of abcisse 0 in kernel)
						// -- function of scaling
                        double weight = getKernelBicubic((center - j - 0.5) * xScale);

						// -- exit condition to optimize processing
                        if (std::fabs(weight) < Epsilon || (j < 0) || (j >= width)) {continue;}

						// -- fill structure to calculate further pixel value with all entries in pixel list
                        computation[i].pixelList[computation[i].nbPixel].pixel = j;
                        computation[i].pixelList[computation[i].nbPixel].weight = weight;
                        computation[i].weightSum += weight;
                        computation[i].nbPixel++;
                    }
                }


		// -- up-sizing along width
		} else {
			for (int i = 0; i < newWidth; i++)
                {
                    // -- initialize structure
                    computation[i].nbPixel = 0;
                    computation[i].pixelList = new PixelCalculation[(int)floor(2.0 * radiusKernel + 1.0)];
                    computation[i].weightSum = 0.0;

					// -- determine center of pixel equivalent in source image
                    double center = ((i + 0.5) / xScale);
					// -- determine boundaries for use pixels calculation
                    int left = (int)(center - radiusKernel);
                    int right = (int)ceil(center + radiusKernel);

                    for (int j = left; j <= right; j++)
                    {
						// -- calculate weight of pixel compared to center (equivalent of abcisse 0 in kernel)
                        double weight = getKernelBicubic(center - j - 0.5);

						// -- exit condition to optimize processing
                        if (std::fabs(weight) < Epsilon || (j < 0) || (j >= width)) {continue;}

                        // -- fill structure to calculate further pixel value with all entries in pixel list
                        computation[i].pixelList[computation[i].nbPixel].pixel = j;
                        computation[i].pixelList[computation[i].nbPixel].weight = weight;
                        computation[i].weightSum += weight;
                        computation[i].nbPixel++;
                    }
                }
		}

		// -- computation of real temporary pixel new values with width-only resizing
		for (int k = 0; k < height; k++)
		{
			for (int i = 0; i < newWidth; i++)
			{
				// -- compute all values of contributor pixels and sum all of them
				for (int channel = 0; channel < nbChannels; channel++) {
					pp[channel] = 0;
				}

				for (int j = 0; j < computation[i].nbPixel; j++)
				{
					int offset = computation[i].pixelList[j].pixel;
					double weight = computation[i].pixelList[j].weight;

					if (std::fabs(weight) < Epsilon) {continue;}

					for (int channel = 0; channel < nbChannels; channel++) {
						pp[channel] += (getValueAt(offset, k, channel) * weight);
					}
				}

				// -- set final value by average all sum by the sum of all weight
				for (int channel = 0; channel < nbChannels; channel++) {
					tempImg->setValueAt(i, k, channel, pp[channel] / computation[i].weightSum);
				}
			}
		}

		// -- create final image by resizing by height basing of temporary image (only width)
		Image* newImage = new Image();
		newImage->width = newWidth;
		newImage->height = newHeight;
		newImage->format = format;
		newImage->computeNbChannels();

		// -- allocate structure of pixel values
		newImage->bufferSize = newWidth*newHeight*nbChannels;
		newImage->data = new unsigned char[newImage->bufferSize];

		// -- temporary structure for computing new pixels value
		computation = new PixelCalculationList[newHeight];

		// -- down-sizing along height
		if (yScale < 1.0)
		{
			// -- determine how many near pixels to be taken
			// -- depends on radius of kernel and scale 
			double sizeCheck = (radiusKernel / yScale);

			for (int i = 0; i < newHeight; i++)
			{
				// -- initialize structure
				computation[i].nbPixel = 0;
				computation[i].pixelList = new PixelCalculation[(int)floor(2.0 * sizeCheck + 1.0)];
				computation[i].weightSum = 0.0;

				// -- determine center of pixel equivalent in source image
				double center = ((i + 0.5) / yScale);
				// -- determine boundaries for use pixels calculation
				int left = (int)(center - sizeCheck);
				int right = (int)(center + sizeCheck);

				for (int j = left; j <= right; j++)
				{
					// -- calculate weight of pixel compared to center (equivalent of abcisse 0 in kernel)
					// -- function of scaling
					double weight = getKernelBicubic((center - j - 0.5) * yScale);

					// -- exit condition to optimize processing
					if (std::fabs(weight) < Epsilon || (j < 0) || (j >= height)) {continue;}

					// -- fill structure to calculate further pixel value with all entries in pixel list
					computation[i].pixelList[computation[i].nbPixel].pixel = j;
					computation[i].pixelList[computation[i].nbPixel].weight = weight;
					computation[i].weightSum += weight;
					computation[i].nbPixel++;
				}
			}

		// -- up-sizing along height
		} else {
			for (int i = 0; i < newHeight; i++)
			{
				// -- initialize structure
				computation[i].nbPixel = 0;
				computation[i].pixelList = new PixelCalculation[(int)floor(2.0 * 2.0 + 1.0)];
				computation[i].weightSum = 0.0;

				// -- determine center of pixel equivalent in source image
				double center = ((i + 0.5) / yScale);
				// -- determine boundaries for use pixels calculation
				int left = (int)(center - radiusKernel);
				int right = (int)(center + radiusKernel);

				for (int j = left; j <= right; j++)
				{
					// -- calculate weight of pixel compared to center (equivalent of abcisse 0 in kernel)
					double weight = getKernelBicubic(center - j - 0.5);

					// -- exit condition to optimize processing
					if (std::fabs(weight) < Epsilon || (j < 0) || (j >= height)) {continue;}

					// -- fill structure to calculate further pixel value with all entries in pixel list
					computation[i].pixelList[computation[i].nbPixel].pixel = j;
					computation[i].pixelList[computation[i].nbPixel].weight = weight;
					computation[i].weightSum += weight;
					computation[i].nbPixel++;
				}
			}
		}

		// -- computation of real pixel new values with height-only resizing (basing on already width resizing)
		for (int k = 0; k < newWidth; k++)
		{
			for (int i = 0; i < newHeight; i++)
			{
				// -- compute all values of contributor pixels and sum all of them
				for (int channel = 0; channel < nbChannels; channel++) {
					pp[channel] = 0;
				}

				for (int j = 0; j < computation[i].nbPixel; j++)
				{
					int offset = computation[i].pixelList[j].pixel;
					double weight = computation[i].pixelList[j].weight;
					if (std::fabs(weight) < Epsilon)
					{
						continue;
					}

					for (int channel = 0; channel < nbChannels; channel++) {
						pp[channel] += (tempImg->getValueAt(k, offset, channel) * weight);
					}
				}

				// -- set final value by average all sum by the sum of all weight
				for (int channel = 0; channel < nbChannels; channel++) {
					newImage->setValueAt(k, i, channel, pp[channel] / computation[i].weightSum);
				}
			}
		}

		// attribute new values
		width = newImage->width;
		height = newImage->height;
		delete tempImg;
		delete[] data;
		data = new unsigned char[newImage->bufferSize];
		std::memcpy(data, newImage->data, newImage->bufferSize*sizeof(unsigned char));
		bufferSize = newImage->bufferSize;
		delete newImage;
		return true;
	} 
	else {
		return false;
	}
}

bool Image::resizeHermite(int newWidth, int newHeight)
{
	// -- value to be considered as 0 if lower than
	double Epsilon = 1e-12;
	double radiusKernel = 2.0;

	if (width == newWidth && height == newHeight) {
		return true;
	}

	// assure image source is correct
	if (data != nullptr) {

		// -- create first temporary image by resizing only by width
		double* pp = new double[nbChannels];
		Image* tempImg = new Image();
		tempImg->width = newWidth;
		tempImg->height = height;
		tempImg->format = format;
		tempImg->computeNbChannels();

		// -- allocate structure of pixel values
		tempImg->bufferSize = newWidth*height*nbChannels;
		tempImg->data = new unsigned char[tempImg->bufferSize];

		// -- temporary structure for computing new pixels value
		PixelCalculationList* computation = new PixelCalculationList[newWidth];

		// calculate ratio between old and new image
		double xScale = ((double)newWidth / width);
		double yScale = ((double)newHeight / height);

		// -- down-sizing along width
		if (xScale < 1.0) {

			// -- determine how many near pixels to be taken
			// -- depends on radius of kernel and scale 
			double sizeCheck = (radiusKernel / xScale);
			for (int i = 0; i < newWidth; i++)
                {
					// -- initialize structure
                    computation[i].nbPixel = 0;
                    computation[i].pixelList = new PixelCalculation[(int)floor(2.0 * sizeCheck + 1.0)];
                    computation[i].weightSum = 0.0;

					// -- determine center of pixel equivalent in source image
                    double center = ((i + 0.5) / xScale);
					// -- determine boundaries for use pixels calculation
                    int left = (int)(center - sizeCheck);
                    int right = (int)(center + sizeCheck);

                    for (int j = left; j <= right; j++)
                    {
						// -- calculate weight of pixel compared to center (equivalent of abcisse 0 in kernel)
						// -- function of scaling
                        double weight = getKernelHermite((center - j - 0.5) * xScale);

						// -- exit condition to optimize processing
                        if (abs(weight) < Epsilon || (j < 0) || (j >= width)) {continue;}

						// -- fill structure to calculate further pixel value with all entries in pixel list
                        computation[i].pixelList[computation[i].nbPixel].pixel = j;
                        computation[i].pixelList[computation[i].nbPixel].weight = weight;
                        computation[i].weightSum += weight;
                        computation[i].nbPixel++;
                    }
                }


		// -- up-sizing along width
		} else {
			for (int i = 0; i < newWidth; i++)
                {
                    // -- initialize structure
                    computation[i].nbPixel = 0;
                    computation[i].pixelList = new PixelCalculation[(int)floor(2.0 * radiusKernel + 1.0)];
                    computation[i].weightSum = 0.0;

					// -- determine center of pixel equivalent in source image
                    double center = ((i + 0.5) / xScale);
					// -- determine boundaries for use pixels calculation
                    int left = (int)(center - radiusKernel);
                    int right = (int)ceil(center + radiusKernel);

                    for (int j = left; j <= right; j++)
                    {
						// -- calculate weight of pixel compared to center (equivalent of abcisse 0 in kernel)
                        double weight = getKernelHermite(center - j - 0.5);

						// -- exit condition to optimize processing
                        if (abs(weight) < Epsilon || (j < 0) || (j >= width)) {continue;}

                        // -- fill structure to calculate further pixel value with all entries in pixel list
                        computation[i].pixelList[computation[i].nbPixel].pixel = j;
                        computation[i].pixelList[computation[i].nbPixel].weight = weight;
                        computation[i].weightSum += weight;
                        computation[i].nbPixel++;
                    }
                }
		}

		// -- computation of real temporary pixel new values with width-only resizing
		for (int k = 0; k < height; k++)
		{
			for (int i = 0; i < newWidth; i++)
			{
				// -- compute all values of contributor pixels and sum all of them
				for (int channel = 0; channel < nbChannels; channel++) {
					pp[channel] = 0;
				}

				for (int j = 0; j < computation[i].nbPixel; j++)
				{
					int offset = computation[i].pixelList[j].pixel;
					double weight = computation[i].pixelList[j].weight;

					if (abs(weight) < Epsilon) {continue;}

					for (int channel = 0; channel < nbChannels; channel++) {
						pp[channel] += (getValueAt(offset, k, channel) * weight);
					}
				}

				// -- set final value by average all sum by the sum of all weight
				for (int channel = 0; channel < nbChannels; channel++) {
					tempImg->setValueAt(i, k, channel, pp[channel] / computation[i].weightSum);
				}
			}
		}

		// -- create final image by resizing by height basing of temporary image (only width)
		Image* newImage = new Image();
		newImage->width = newWidth;
		newImage->height = newHeight;
		newImage->format = format;
		newImage->computeNbChannels();

		// -- allocate structure of pixel values
		newImage->bufferSize = newWidth*newHeight*nbChannels;
		newImage->data = new unsigned char[newImage->bufferSize];

		// -- temporary structure for computing new pixels value
		computation = new PixelCalculationList[newHeight];

		// -- down-sizing along height
		if (yScale < 1.0)
		{
			// -- determine how many near pixels to be taken
			// -- depends on radius of kernel and scale 
			double sizeCheck = (radiusKernel / yScale);

			for (int i = 0; i < newHeight; i++)
			{
				// -- initialize structure
				computation[i].nbPixel = 0;
				computation[i].pixelList = new PixelCalculation[(int)floor(2.0 * sizeCheck + 1.0)];
				computation[i].weightSum = 0.0;

				// -- determine center of pixel equivalent in source image
				double center = ((i + 0.5) / yScale);
				// -- determine boundaries for use pixels calculation
				int left = (int)(center - sizeCheck);
				int right = (int)(center + sizeCheck);

				for (int j = left; j <= right; j++)
				{
					// -- calculate weight of pixel compared to center (equivalent of abcisse 0 in kernel)
					// -- function of scaling
					double weight = getKernelHermite((center - j - 0.5) * yScale);

					// -- exit condition to optimize processing
					if (abs(weight) < Epsilon || (j < 0) || (j >= height)) {continue;}

					// -- fill structure to calculate further pixel value with all entries in pixel list
					computation[i].pixelList[computation[i].nbPixel].pixel = j;
					computation[i].pixelList[computation[i].nbPixel].weight = weight;
					computation[i].weightSum += weight;
					computation[i].nbPixel++;
				}
			}

		// -- up-sizing along height
		} else {
			for (int i = 0; i < newHeight; i++)
			{
				// -- initialize structure
				computation[i].nbPixel = 0;
				computation[i].pixelList = new PixelCalculation[(int)floor(2.0 * 2.0 + 1.0)];
				computation[i].weightSum = 0.0;

				// -- determine center of pixel equivalent in source image
				double center = ((i + 0.5) / yScale);
				// -- determine boundaries for use pixels calculation
				int left = (int)(center - radiusKernel);
				int right = (int)(center + radiusKernel);

				for (int j = left; j <= right; j++)
				{
					// -- calculate weight of pixel compared to center (equivalent of abcisse 0 in kernel)
					double weight = getKernelHermite(center - j - 0.5);

					// -- exit condition to optimize processing
					if (abs(weight) < Epsilon || (j < 0) || (j >= height)) {continue;}

					// -- fill structure to calculate further pixel value with all entries in pixel list
					computation[i].pixelList[computation[i].nbPixel].pixel = j;
					computation[i].pixelList[computation[i].nbPixel].weight = weight;
					computation[i].weightSum += weight;
					computation[i].nbPixel++;
				}
			}
		}

		// -- computation of real pixel new values with height-only resizing (basing on already width resizing)
		for (int k = 0; k < newWidth; k++)
		{
			for (int i = 0; i < newHeight; i++)
			{
				// -- compute all values of contributor pixels and sum all of them
				for (int channel = 0; channel < nbChannels; channel++) {
					pp[channel] = 0;
				}

				for (int j = 0; j < computation[i].nbPixel; j++)
				{
					int offset = computation[i].pixelList[j].pixel;
					double weight = computation[i].pixelList[j].weight;
					if (abs(weight) < Epsilon)
					{
						continue;
					}

					for (int channel = 0; channel < nbChannels; channel++) {
						pp[channel] += (tempImg->getValueAt(k, offset, channel) * weight);
					}
				}

				// -- set final value by average all sum by the sum of all weight
				for (int channel = 0; channel < nbChannels; channel++) {
					newImage->setValueAt(k, i, channel, pp[channel] / computation[i].weightSum);
				}
			}
		}

		// attribute new values
		width = newImage->width;
		height = newImage->height;
		delete tempImg;
		delete[] data;
		data = new unsigned char[newImage->bufferSize];
		std::memcpy(data, newImage->data, newImage->bufferSize*sizeof(unsigned char));
		bufferSize = newImage->bufferSize;
		delete newImage;
		return true;
	} 
	else {
		return false;
	}
}

Image* Image::blendWithForeImage(Image* fore)
{
	if (width != fore->width ||
		height != fore->height ||
		nbChannels != 4 ||
		fore->nbChannels != 4) {

		Poco::Logger::get("Image").error("Can't merge 2 images with different size or that has no alpha component !", __FILE__, __LINE__);
		return nullptr;
	}

	unsigned char* data = new unsigned char[width*height*3];

	// -- calculation fo reach pixel
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			std::shared_ptr<Color> colorFore(fore->getValueAt(j, i));
			std::shared_ptr<Color> colorBack(getValueAt(j, i));
			int indexPixel = (i*width+j)*3;
			
			// -- blended
			double rTmp = std::pow(colorFore->getRed(), 2) + (std::pow(colorBack->getRed(), 2)*0.5*(1.0f-colorFore->getAlpha())); 
			int red = std::min(sqrtf(rTmp), 1.0f)*255.0 + 0.5;
			double gTmp = std::pow(colorFore->getGreen(), 2) + (std::pow(colorBack->getGreen(), 2)*(1.0f-colorFore->getAlpha())); 
			int green = std::min(sqrtf(gTmp), 1.0f)*255.0 + 0.5;
			double bTmp = std::pow(colorFore->getBlue(), 2) + (std::pow(colorBack->getBlue(), 2)*0.1*(1.0f-colorFore->getAlpha()));
			int blue = std::min(sqrtf(bTmp), 1.0f)*255.0 + 0.5;

			data[indexPixel] = red;
			data[indexPixel+1] = green;
			data[indexPixel+2] = blue;
		}
	}

	Image* merged = new Image(width, height, PNG_FORMAT_RGB, data);
	return merged;
}

Image* Image::mixWithForeImage(Image* fore)
{
	if (width != fore->width ||
		height != fore->height ||
		nbChannels != 4 ||
		fore->nbChannels != 4) {

		Poco::Logger::get("Image").error("Can't merge 2 images with different size or that has no alpha component !", __FILE__, __LINE__);
		return nullptr;
	}

	unsigned char* data = new unsigned char[width*height*3];

	// -- calculation fo reach pixel
	for (int i = 0; i < height; i++) {
		int idxRow = i*width*3;
		for (int j = 0; j < width; j++) {
			std::shared_ptr<Color> colorFore(fore->getValueAt(j, i));
			std::shared_ptr<Color> colorBack(getValueAt(j, i));
			int indexPixel = idxRow+(j*3);
			
			// -- blended
			double red = colorBack->getRed() + colorFore->getAlpha() * 0.5*(colorFore->getRed() - colorBack->getRed());
			double green = colorBack->getGreen() + colorFore->getAlpha() * (colorFore->getGreen() - colorBack->getGreen());
			double blue = colorBack->getBlue() + colorFore->getAlpha() * 0.1*(colorFore->getBlue() - colorBack->getBlue());

			data[indexPixel] = std::max(std::min(red, 1.0), 0.0)*255.0 + 0.5;
			data[indexPixel+1] = std::max(std::min(green, 1.0), 0.0)*255.0 + 0.5;
			data[indexPixel+2] = std::max(std::min(blue, 1.0), 0.0)*255.0 + 0.5;
		}
	}

	Image* merged = new Image(width, height, PNG_FORMAT_RGB, data);
	return merged;
}

bool Image::saveImage(std::string filename)
{
	this->filename = filename;
	return saveImage();
}

bool Image::saveImage()
{
	// -- inti path to check extension
	Poco::Path pathFile(filename);
	
	// -- TIFF
	if (pathFile.getExtension() == "tiff") {

		TIFF *tiff = TIFFOpen(filename.c_str(), "w");
		TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, width); 
		TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, height);
		if (nbChannels == 3) {
			TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 8, 8, 8);
		} else if (nbChannels == 4) {
			TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 8, 8, 8, 8); 
			TIFFSetField(tiff, TIFFTAG_EXTRASAMPLES, 1);
		}
		TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, nbChannels); 
		TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, 1);   
		TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
		TIFFSetField(tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
		TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
		TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
		TIFFSetField(tiff, TIFFTAG_COMPRESSION, COMPRESSION_LZW);

		// -- save image height by height
		for (int i = 0; i < height; i++) {
			TIFFWriteScanline(tiff, convertToLine(i), i, 0);
		}
		TIFFClose(tiff);

		return true;
	} 
	else if (pathFile.getExtension() == "png") {

		// -- create first temporary image by resizing only by width
		png_image* image = new png_image();
		image->version = PNG_IMAGE_VERSION;
		image->width = width;
		image->height = height;
		image->format = format;
		image->opaque = nullptr;
		image->flags = 0;
		image->colormap_entries = 0;

		int returnValue = png_image_write_to_file(image, filename.c_str(), 0, data, PNG_IMAGE_ROW_STRIDE(*image), 0);
		if (returnValue != 0 && returnValue != 1) {
			Poco::Logger::get("Image").error("Error at saving image : " + filename + ". Message : " + image->message, __FILE__, __LINE__);
			delete image;
			return false;
		}
		delete image;
		return true;
	}

	Poco::Logger::get("Image").error("Image file with extension " + pathFile.getExtension() + " not handled", __FILE__, __LINE__);
	return false;
}

unsigned char Image::getValueAt(int x, int y, int channel)
{
	int index = (y*width+x)*nbChannels+channel;
	if (data != nullptr && index < bufferSize && index >= 0) {
		return data[index];
	} else {
		return 0;
	}
}

void Image::setValueAt(int x, int y, int channel, unsigned char value) {
	
	int index = (y*width+x)*nbChannels+channel;
	if (data != nullptr && index < bufferSize && index >= 0) {
		data[index] = value;
	}
}

Color* Image::getValueAt(int x, int y)
{
	Color* color = new Color();

	if (data == nullptr || 
		x < 0 || x >= width ||
		y < 0 || y >= height) {
		return nullptr;
	} 

	int index = (y*width+x)*nbChannels;

	switch (format) {
		case  PNG_FORMAT_RGB :
		color->setRGB(((double)data[index])/255.0, ((double)data[index+1])/255.0, ((double)data[index+2])/255.0);
		break;

		case PNG_FORMAT_RGBA :
		color->setRGBA(((double)data[index])/255.0, ((double)data[index+1])/255.0, ((double)data[index+2])/255.0,  ((double)data[index+3])/255.0);
		break;

		default :
		Poco::Logger::get("Image").error("Format image unknow or unhandled !", __FILE__, __LINE__);
		return nullptr;
		break;
	}

	return color;
}

void Image::setValueAt(int x, int y, Color color)
{
	if (data == nullptr || 
		x < 0 || x >= width ||
		y < 0 || y >= height) {
		return;
	} 

	int index = (y*width+x)*nbChannels;

	switch (format) {
		case  PNG_FORMAT_RGB :
		data[index] = std::min((int)(color.getRed()*255.0 + 0.5), 255);
		data[index+1] = std::min((int)(color.getGreen()*255.0 + 0.5), 255);
		data[index+2] = std::min((int)(color.getBlue()*255.0 + 0.5), 255);
		break;

		case PNG_FORMAT_RGBA :
		data[index] = std::min((int)(color.getRed()*255.0 + 0.5), 255);
		data[index+1] = std::min((int)(color.getGreen()*255.0 + 0.5), 255);
		data[index+2] = std::min((int)(color.getBlue()*255.0 + 0.5), 255);
		data[index+3] = std::min((int)(color.getAlpha()*255.0 + 0.5), 255);
		break;

		default :
		Poco::Logger::get("Image").error("Format image unknow or unhandled !", __FILE__, __LINE__);
		return;
		break;
	}
}

void Image::setValueAt(int x, int y, int widthRect, int heightRect, Color color)
{
	if (data == nullptr || 
		x < 0 || x+widthRect-1 >= width ||
		y < 0 || y+heightRect-1 >= height) {
		return;
	} 

	for (int j = 0; j < heightRect; j++) {
		for (int i = 0; i < widthRect; i++) {
			int index = ((y+j)*width+x+i)*nbChannels;

			switch (format) {
				case  PNG_FORMAT_RGB :
				data[index] = std::min((int)(color.getRed()*255.0 + 0.5), 255);
				data[index+1] = std::min((int)(color.getGreen()*255.0 + 0.5), 255);
				data[index+2] = std::min((int)(color.getBlue()*255.0 + 0.5), 255);
				break;

				case PNG_FORMAT_RGBA :
				data[index] = std::min((int)(color.getRed()*255.0 + 0.5), 255);
				data[index+1] = std::min((int)(color.getGreen()*255.0 + 0.5), 255);
				data[index+2] = std::min((int)(color.getBlue()*255.0 + 0.5), 255);
				data[index+3] = std::min((int)(color.getAlpha()*255.0 + 0.5), 255);
				break;

				default :
				Poco::Logger::get("Image").error("Format image unknow or unhandled !", __FILE__, __LINE__);
				return;
				break;
			}
		}
	}
}

double Image::getKernelHermite(double point)	
{
	if (point < 0) point = -point;
   	if (point <= 1) return ((2*point - 3)*point*point + 1);
	return 0.0f;
}

double Image::getKernelBell(double point)
{
	if (point < 0) point = -point;
	if (point < 0.5) return (0.75-point*point);
	else if (point < 1.5) return (0.5*(point-1.5)*(point-1.5));
	return 0.0f;
}

double Image::getKernelBicubic(double point)
{
	if (point < 0) point = -point;
   	if (point <= 1) return ((1.5*point - 2.5)*point*point + 1);
	else if (point < 2) return ((-0.5*point*point + 2.5*point - 4)*point + 2);
	return 0.0f;
}

Image* Image::extractData(int offsetX, int offsetY, int newWidth, int newHeight)
{
	// -- we check extraction is possible
	if (offsetX < 0 || offsetX >= width ||
		offsetY < 0 || offsetY >= height ||
		offsetX + newWidth > width ||
		offsetY + newHeight > height) {

		Poco::Logger::get("Image").error("Can't extract !", __FILE__, __LINE__);
		Poco::Logger::get("Image").error("OffsetX : " + std::to_string(offsetX), __FILE__, __LINE__);
		Poco::Logger::get("Image").error("offsetY : " + std::to_string(offsetY), __FILE__, __LINE__);
		Poco::Logger::get("Image").error("newWidth : " + std::to_string(newWidth), __FILE__, __LINE__);
		Poco::Logger::get("Image").error("newHeight : " + std::to_string(newHeight), __FILE__, __LINE__);
		Poco::Logger::get("Image").error("width : " + std::to_string(width), __FILE__, __LINE__);
		Poco::Logger::get("Image").error("height : " + std::to_string(height), __FILE__, __LINE__);
		return nullptr;
	}

	// -- creation of new image
	Image* newLib = new Image();
	newLib->width = newWidth;
	newLib->height = newHeight;
	newLib->format = format;
	newLib->nbChannels = nbChannels;
	newLib->bufferSize = newWidth*newHeight*nbChannels;
	newLib->data = new unsigned char[newLib->bufferSize];

	// -- fill data buffer
	int offsetDst = 0;
	int offsetSrc = (offsetY*width+offsetX)*nbChannels;
	for (int y = 0 ; y < newHeight; y++) {
		std::memcpy(&newLib->data[offsetDst], &data[offsetSrc], nbChannels*newWidth);
		offsetDst += nbChannels*newWidth;
		offsetSrc += nbChannels*width;
	}

	return newLib;
}

Image* Image::extractDataWithFillBlack(int offsetSrcX, int offsetSrcY, int widthSrc, int heightSrc, int offsetDstX, int offsetDstY, int widthDst, int heightDst)
{
	// -- we check extraction is possible
	if (offsetSrcX < 0 || offsetSrcY < 0 ||
		offsetSrcX >= width || offsetSrcY >= height ||
		offsetSrcX + widthSrc > width ||
		offsetSrcY + heightSrc > height ||
		offsetDstX < 0 || offsetDstY < 0 ||
		offsetDstX >= widthDst || offsetDstY >= heightDst ||
		offsetDstX + widthSrc > widthDst ||
		offsetDstY + heightSrc > heightDst) {

		Poco::Logger::get("Image").error("Can't extract !", __FILE__, __LINE__);
		return nullptr;
	}

	// -- creation of new image
	Image* newLib = new Image();
	newLib->width = widthDst;
	newLib->height = heightDst;
	newLib->format = format;
	newLib->nbChannels = nbChannels;
	newLib->bufferSize = widthDst*heightDst*nbChannels;
	newLib->data = new unsigned char[newLib->bufferSize];

	std::memset(newLib->data, 0, newLib->bufferSize);

	// -- fill data buffer
	int offsetDst = (offsetDstY*widthDst+offsetDstX)*nbChannels;
	int offsetSrc = (offsetSrcY*width+offsetSrcX)*nbChannels;
	for (int y = 0 ; y < heightDst; y++) {
		std::memcpy(&newLib->data[offsetDst], &data[offsetSrc], nbChannels*widthDst);
		offsetDst += nbChannels*widthDst;
		offsetSrc += nbChannels*width;
	}

	return newLib;
}