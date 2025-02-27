#include "ImageProcessing.h"

ImageProcessing::ImageProcessing() {

}

ImageProcessing::~ImageProcessing() {

	delete[] m_pImgLocalData;
}

bool ImageProcessing::pixelsMirror(uchar* originalImgData, const int bytesPerLine, const int imgWidth, const int imgHeight, const int padding)
{
	// check if there is already some image data stored
	if (m_pImgLocalData != nullptr)
	{
		delete[] m_pImgLocalData; // if there is, delete old data
		m_pImgLocalData = nullptr;
	}

	// compute new size
	m_imgWidth = imgWidth + 2 * padding;
	m_imgHeight = imgHeight + 2 * padding;
	size_t size = (size_t)m_imgWidth * m_imgHeight;

	// m_pImgLocalData = new double[size] {}; // allocate memory
	m_pImgLocalData = (double*)calloc(size, sizeof(double)); // allocate memory
	if (m_pImgLocalData == nullptr) // check, if allocation was successful
		return false;

	int indexNew = 0, indexOld = 0; double tempD = 0.0;	int temp = 0;

	// copy old (smaller) image into image with padding N
	for (size_t i = 0; i < imgWidth; i++) {
		for (size_t j = 0; j < imgHeight; j++){

			indexNew = (i + padding) * imgWidth + (j + padding);
			indexOld = i * bytesPerLine + j; // bytesPerLine -> bytes in row
			// converting pixel values from 0-255 to 0.0-1.0 range
			tempD = static_cast<double>(originalImgData[indexOld]);
			tempD = tempD / 255.0;
			m_pImgLocalData[indexNew] = tempD;
		}
	}

	// mirror over Upper and Lower edges
	temp = 1; // for correct reflection 

	/*
		pading = 3
		A B C -> C B A A B C C B A

		Original image (3x3):
		1 2 3
		4 5 6
		7 8 9

		Padding=1 and  using temp (5x5):
		5 4 5 6 5
		2 1 2 3 2
		5 4 5 6 5
		8 7 8 9 8
		5 4 5 6 5

		Without temp:
		2 1 2 3 2
		2 1 2 3 2
		5 4 5 6 5
		8 7 8 9 8
		8 7 8 9 8
	*/

	for (int i = 0; i < padding; i++)
	{
		for (int j = padding; j < m_imgWidth - padding; j++)
		{
			// upper egde
			indexOld = (i + padding) * m_imgWidth + j;
			indexNew = (i + padding - temp) * m_imgWidth + j;
			m_pImgLocalData[indexNew] = m_pImgLocalData[indexOld];

			// lower edge
			indexOld = (m_imgHeight - i - padding - 1) * m_imgWidth + j;
			indexNew = (m_imgHeight - i - padding + temp - 1) * m_imgWidth + j;
			m_pImgLocalData[indexNew] = m_pImgLocalData[indexOld];
		}
		temp += 2;
	}

	// mirror over Left and Right edges
	for (int i = 0; i < m_imgHeight; i++)
	{
		temp = 1;
		for (int j = 0; j < padding; j++)
		{
			// left edge
			indexOld = i * m_imgWidth + (j + padding);
			indexNew = i * m_imgWidth + (j + padding - temp);
			m_pImgLocalData[indexNew] = m_pImgLocalData[indexOld];

			// right edge
			indexOld = i * m_imgWidth + (m_imgWidth - padding - 1 - j);
			indexNew = i * m_imgWidth + (m_imgWidth - padding - 1 - j + temp);
			m_pImgLocalData[indexNew] = m_pImgLocalData[indexOld];

			temp += 2;
		}
	}

	return true;
}

bool ImageProcessing::pixelsMirror(double* originalImgData, const int bytesPerLine, const int imgWidth, const int imgHeight, const int padding)
{
	int indexNew = 0, indexOld = 0;
	int temp = 0;

	// check if there is already some image data stored
	if (m_pImgLocalData != nullptr)
	{
		delete[] m_pImgLocalData; // if there is, delete old data
		m_pImgLocalData = nullptr;
	}

	// compute new size
	m_imgWidth = imgWidth + 2 * padding;
	m_imgHeight = imgHeight + 2 * padding;
	size_t size = (size_t)m_imgWidth * m_imgHeight;

	// m_pImgLocalData = new double[size] {0.0}; // allocate memory
	m_pImgLocalData = (double*)calloc(size, sizeof(double)); // allocate memory

	if (m_pImgLocalData == nullptr) // check, if allocation was successful
		return false;

	// copy old image data
	for (int i = 0; i < imgHeight; i++)
	{
		for (int j = 0; j < imgWidth; j++)
		{
			indexNew = (i + padding) * m_imgWidth + (j + padding);
			indexOld = i * bytesPerLine + j;

			m_pImgLocalData[indexNew] = originalImgData[indexOld];
		}
	}

	// mirror over Upper and Lower edges
	temp = 1;
	for (int i = 0; i < padding; i++)
	{
		for (int j = padding; j < m_imgWidth - padding; j++)
		{
			// upper egde
			indexOld = (i + padding) * m_imgWidth + j;
			indexNew = (i + padding - temp) * m_imgWidth + j;
			m_pImgLocalData[indexNew] = m_pImgLocalData[indexOld];

			// lower edge
			indexOld = (m_imgHeight - i - padding - 1) * m_imgWidth + j;
			indexNew = (m_imgHeight - i - padding + temp - 1) * m_imgWidth + j;
			m_pImgLocalData[indexNew] = m_pImgLocalData[indexOld];
		}
		temp += 2;
	}

	// mirror over Left and Right edges
	for (int i = 0; i < m_imgHeight; i++)
	{
		temp = 1;
		for (int j = 0; j < padding; j++)
		{
			// left edge
			indexOld = i * m_imgWidth + (j + padding);
			indexNew = i * m_imgWidth + (j + padding - temp);
			m_pImgLocalData[indexNew] = m_pImgLocalData[indexOld];

			// right edge
			indexOld = i * m_imgWidth + (m_imgWidth - padding - 1 - j);
			indexNew = i * m_imgWidth + (m_imgWidth - padding - 1 - j + temp);
			m_pImgLocalData[indexNew] = m_pImgLocalData[indexOld];

			temp += 2;
		}
	}

	return true;
}

uchar* ImageProcessing::pixelsUnmirror(int padding)
{
	if (m_pImgLocalData == nullptr) // check, if there is some image to be cropped
		return nullptr;

	// calculate new size
	uint newWidth = m_imgWidth - 2 * padding;
	uint newHeight = m_imgHeight - 2 * padding;
	size_t size = (size_t)newWidth * newHeight;

	// uchar* pImgData = new uchar[size]{ 0 };
	uchar* pImgData = (uchar*)calloc(size, sizeof(uchar)); // allocate new memory

	if (pImgData == nullptr) // check, if allocation successful
		return nullptr;

	// copy only the requested data based on the "padding"
	int indexOld = 0, indexNew = 0;
	int iNew = 0, jNew = 0;
	for (int i = padding; i < m_imgHeight - padding; i++)
	{
		jNew = 0;
		for (int j = padding; j < m_imgWidth - padding; j++)
		{
			indexOld = i * m_imgWidth + j;
			indexNew = iNew * newWidth + jNew;
			pImgData[indexNew] = static_cast<uchar>(m_pImgLocalData[indexOld] * 255.0 + 0.5);

			jNew++;
		}
		iNew++;
	}

	return pImgData;
}

QImage* ImageProcessing::FSHS(QImage* img)
{
	if (img == nullptr) return img;

	// copy of the image
	QImage* resultImg = new QImage(*img);

	// if image is grayscale
	bool isGrayscale = img->isGrayscale();
	// For RGB images
	std::vector<int> minValues(3, 255);  // R, G, B minimums
	std::vector<int> maxValues(3, 0);    // R, G, B maximums

	// For grayscale
	if (isGrayscale) {
		int minGray = std::min(std::min(minValues[0], minValues[1]), minValues[2]);
		int maxGray = std::max(std::max(maxValues[0], maxValues[1]), maxValues[2]);

		minValues[0] = minValues[1] = minValues[2] = minGray;
		maxValues[0] = maxValues[1] = maxValues[2] = maxGray;
	}
	else {
		// minimum and maximum for each channel
		for (int j = 0; j < img->height(); ++j) {
			for (int i = 0; i < img->width(); ++i) {
				QColor pixelColor = img->pixelColor(i, j);

				minValues[0] = std::min(minValues[0], pixelColor.red());
				minValues[1] = std::min(minValues[1], pixelColor.green());
				minValues[2] = std::min(minValues[2], pixelColor.blue());

				maxValues[0] = std::max(maxValues[0], pixelColor.red());
				maxValues[1] = std::max(maxValues[1], pixelColor.green());
				maxValues[2] = std::max(maxValues[2], pixelColor.blue());
			}
		}
	}

	// stretch to each pixel
	for (int j = 0; j < img->height(); ++j) {
		for (int i = 0; i < img->width(); ++i) {
			QColor oldColor = img->pixelColor(i, j);
			QColor newColor;

			int newRed = 0, newGreen = 0, newBlue = 0;

			// division by zero 
			if (maxValues[0] > minValues[0]) {
				newRed = static_cast<int>(255.0 * (oldColor.red() - minValues[0]) / (maxValues[0] - minValues[0]));
			}

			if (maxValues[1] > minValues[1]) {
				newGreen = static_cast<int>(255.0 * (oldColor.green() - minValues[1]) / (maxValues[1] - minValues[1]));
			}

			if (maxValues[2] > minValues[2]) {
				newBlue = static_cast<int>(255.0 * (oldColor.blue() - minValues[2]) / (maxValues[2] - minValues[2]));
			}

			// check if values are <0,255>
			newRed = std::min(255, std::max(0, newRed));
			newGreen = std::min(255, std::max(0, newGreen));
			newBlue = std::min(255, std::max(0, newBlue));

			newColor.setRgb(newRed, newGreen, newBlue);
			resultImg->setPixelColor(i, j, newColor);
		}
	}

	return resultImg;
}

QImage* ImageProcessing::EH(QImage* img)
{
	if (img == nullptr) return img;
	// copy of the image
	QImage* resultImg = new QImage(*img);

	// if image is grayscale
	bool isGrayscale = img->isGrayscale();

	// Get image dimensions
	int width = img->width();
	int height = img->height();
	int totalPixels = width * height;

	// Create histogram arrays (for each channel)
	std::vector<int> histogramR(256, 0);
	std::vector<int> histogramG(256, 0);
	std::vector<int> histogramB(256, 0);

	// Compute histograms
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			QColor pixel = img->pixelColor(x, y);
			histogramR[pixel.red()]++;
			histogramG[pixel.green()]++;
			histogramB[pixel.blue()]++;
		}
	}

	// Calculate cumulative distribution function (CDF) for each channel
	std::vector<int> cdfR(256, 0);
	std::vector<int> cdfG(256, 0);
	std::vector<int> cdfB(256, 0);

	// Initialize first element of CDF
	cdfR[0] = histogramR[0];
	cdfG[0] = histogramG[0];
	cdfB[0] = histogramB[0];

	// Compute CDF for all other values
	for (int i = 1; i < 256; ++i) {
		cdfR[i] = cdfR[i - 1] + histogramR[i];
		cdfG[i] = cdfG[i - 1] + histogramG[i];
		cdfB[i] = cdfB[i - 1] + histogramB[i];
	}

	// If grayscale, use the same mapping for all channels
	std::vector<int> mappingR(256, 0);
	std::vector<int> mappingG(256, 0);
	std::vector<int> mappingB(256, 0);

	// Find the minimum non-zero CDF value for each channel
	int cdfMinR = 0, cdfMinG = 0, cdfMinB = 0;

	for (int i = 0; i < 256; ++i) {
		if (cdfR[i] > 0) {
			cdfMinR = cdfR[i];
			break;
		}
	}

	for (int i = 0; i < 256; ++i) {
		if (cdfG[i] > 0) {
			cdfMinG = cdfG[i];
			break;
		}
	}

	for (int i = 0; i < 256; ++i) {
		if (cdfB[i] > 0) {
			cdfMinB = cdfB[i];
			break;
		}
	}

	// Create mapping lookup table
	for (int i = 0; i < 256; ++i) {
		// Calculate equalized value using the CDF transformation
		if (cdfR[i] == 0)
			mappingR[i] = 0;
		else
			mappingR[i] = std::round(((float)(cdfR[i] - cdfMinR) / (float)(totalPixels - cdfMinR)) * 255.0f);

		if (cdfG[i] == 0)
			mappingG[i] = 0;
		else
			mappingG[i] = std::round(((float)(cdfG[i] - cdfMinG) / (float)(totalPixels - cdfMinG)) * 255.0f);

		if (cdfB[i] == 0)
			mappingB[i] = 0;
		else
			mappingB[i] = std::round(((float)(cdfB[i] - cdfMinB) / (float)(totalPixels - cdfMinB)) * 255.0f);
	}

	// If grayscale, use the same mapping for all channels
	if (isGrayscale) {
		std::vector<int> combinedMapping(256, 0);

		for (int i = 0; i < 256; ++i) {
			combinedMapping[i] = (mappingR[i] + mappingG[i] + mappingB[i]) / 3;
		}

		mappingR = mappingG = mappingB = combinedMapping;
	}

	// Apply equalization to the image
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			QColor oldColor = img->pixelColor(x, y);
			int newRed = mappingR[oldColor.red()];
			int newGreen = mappingG[oldColor.green()];
			int newBlue = mappingB[oldColor.blue()];

			// Ensure values are in valid range
			newRed = std::min(255, std::max(0, newRed));
			newGreen = std::min(255, std::max(0, newGreen));
			newBlue = std::min(255, std::max(0, newBlue));

			QColor newColor(newRed, newGreen, newBlue);
			resultImg->setPixelColor(x, y, newColor);
		}
	}

	return resultImg;
}
