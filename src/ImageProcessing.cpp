#include "ImageProcessing.h"

ImageProcessing::ImageProcessing() {

}

ImageProcessing::~ImageProcessing() {

	delete[] m_pImgLocalData;
}

/*
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
	//

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
*/

QImage ImageProcessing::pixelsMirror(QImage img, int padding)
{
	if (img.format() != QImage::Format_Grayscale8) {
		qDebug() << "Error: Image must be in Format_Grayscale8.";
		return img;
	}

	int imgWidth = img.width();
	int imgHeight = img.height();
	int bytesPerLine = img.bytesPerLine();
	const uchar* originalImgData = img.bits();  // Get raw image data

	// Check if memory needs to be cleared
	if (m_pImgLocalData != nullptr)
	{
		delete[] m_pImgLocalData;
		m_pImgLocalData = nullptr;
	}

	// Compute new size with padding
	m_imgWidth = imgWidth + 2 * padding;
	m_imgHeight = imgHeight + 2 * padding;
	size_t size = static_cast<size_t>(m_imgWidth) * m_imgHeight;

	// Allocate memory for padded image
	m_pImgLocalData = (double*)calloc(size, sizeof(double));
	if (m_pImgLocalData == nullptr)
		return img;

	int indexNew = 0, indexOld = 0;
	double tempD = 0.0;
	int temp = 0;

	// Copy the original image into the center of the padded image
	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			indexNew = (i + padding) * m_imgWidth + (j + padding);
			indexOld = i * bytesPerLine + j;
			tempD = static_cast<double>(originalImgData[indexOld]) / 255.0;  // Normalize to 0.0 - 1.0
			m_pImgLocalData[indexNew] = tempD;
		}
	}

	// Mirror over Upper and Lower edges
	temp = 1;
	for (int i = 0; i < padding; i++)
	{
		for (int j = padding; j < m_imgWidth - padding; j++)
		{
			// Upper edge
			indexOld = (i + padding) * m_imgWidth + j;
			indexNew = (i + padding - temp) * m_imgWidth + j;
			m_pImgLocalData[indexNew] = m_pImgLocalData[indexOld];

			// Lower edge
			indexOld = (m_imgHeight - i - padding - 1) * m_imgWidth + j;
			indexNew = (m_imgHeight - i - padding + temp - 1) * m_imgWidth + j;
			m_pImgLocalData[indexNew] = m_pImgLocalData[indexOld];
		}
		temp += 2;
	}

	// Mirror over Left and Right edges
	for (int i = 0; i < m_imgHeight; i++)
	{
		temp = 1;
		for (int j = 0; j < padding; j++)
		{
			// Left edge
			indexOld = i * m_imgWidth + (j + padding);
			indexNew = i * m_imgWidth + (j + padding - temp);
			m_pImgLocalData[indexNew] = m_pImgLocalData[indexOld];

			// Right edge
			indexOld = i * m_imgWidth + (m_imgWidth - padding - 1 - j);
			indexNew = i * m_imgWidth + (m_imgWidth - padding - 1 - j + temp);
			m_pImgLocalData[indexNew] = m_pImgLocalData[indexOld];

			temp += 2;
		}
	}

	// Convert to QImage
	QImage img_mirrored(m_imgWidth, m_imgHeight, QImage::Format_Grayscale8);
	uchar* imageData = img_mirrored.bits(); // pointer for data

	for (int i = 0; i < m_imgHeight; i++) {
		for (int j = 0; j < m_imgWidth; j++) {
			int index = i * m_imgWidth + j;
			int pixelValue = static_cast<int>(m_pImgLocalData[index] * 255.0);  // Scale to 0-255
			//pixelValue = qBound(0, pixelValue, 255);  // Ensure within valid range

			imageData[i * img_mirrored.bytesPerLine() + j] = static_cast<uchar>(pixelValue);
		}
	}

	exportToPGM(img, "test.pgm");
	return img_mirrored;
}

QImage ImageProcessing::FSHS(QImage img)
{
	if (img.isNull()) return img;

	// copy of the image
	QImage resultImg = img;

	// if image is grayscale
	bool isGrayscale = img.isGrayscale();
	if (!isGrayscale) return img;

	// minimum and maximum intensity values
	int minIntensity = 255;
	int maxIntensity = 0;

	for (int y = 0; y < img.height(); ++y) {
		for (int x = 0; x < img.width(); ++x) {
			QColor pixel = img.pixelColor(x, y);
			int gray = qGray(pixel.red(), pixel.green(), pixel.blue());

			minIntensity = std::min(minIntensity, gray);
			maxIntensity = std::max(maxIntensity, gray);
		}
	}

	// stretch to each pixel
	for (int y = 0; y < img.height(); ++y) {
		for (int x = 0; x < resultImg.width(); ++x) {
			QColor oldColor = img.pixelColor(x, y);
			int gray = qGray(oldColor.red(), oldColor.green(), oldColor.blue());

			// stretch
			int newGray = 0;
			if (maxIntensity > minIntensity) {
				newGray = static_cast<int>(255.0 * (gray - minIntensity) / (maxIntensity - minIntensity));
			}

			// range <0,255> -> range <0,1> CHANGE
			newGray = std::min(255, std::max(0, newGray));

			resultImg.setPixelColor(x, y, QColor(newGray, newGray, newGray));
		}
	}

	return resultImg;
}

QImage ImageProcessing::EH(QImage img)
{
	if (img.isNull()) return img;

	// copy of the image
	QImage resultImg = img;

	// if image is grayscale
	bool isGrayscale = img.isGrayscale();
	if (!isGrayscale) return img;

	// Get image dimensions
	int width = img.width();
	int height = img.height();
	int totalPixels = width * height;

	// Create histogram array (for grayscale intensity)
	std::vector<int> histogram(256, 0);

	// histogram
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			QColor pixel = img.pixelColor(x, y);
			int gray = qGray(pixel.red(), pixel.green(), pixel.blue());
			histogram[gray]++;
		}
	}

	// cumulative distribution function (CDF)
	std::vector<int> cdf(256, 0);

	// Initialize first element of CDF
	cdf[0] = histogram[0];

	// CDF for all other values
	for (int i = 1; i < 256; ++i) {
		cdf[i] = cdf[i - 1] + histogram[i];
	}
	// mapping lookup table
	std::vector<int> mapping(256, 0);

	// minimum non-zero CDF value
	int cdfMin = 0;
	for (int i = 0; i < 256; ++i) {
		if (cdf[i] > 0) {
			cdfMin = cdf[i];
			break;
		}
	}

	// Create mapping lookup table
	for (int i = 0; i < 256; ++i) {
		// equalized value using the CDF
		if (cdf[i] == 0)
			mapping[i] = 0;
		else
			mapping[i] = std::round(((float)(cdf[i] - cdfMin) / (float)(totalPixels - cdfMin)) * 255.0f);
	}

	// equalization to the image
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			QColor oldColor = img.pixelColor(x, y);
			int gray = qGray(oldColor.red(), oldColor.green(), oldColor.blue());
			int newGray = mapping[gray];

			// in range <0,255> CHANGE to range <0,1>
			newGray = std::min(255, std::max(0, newGray));

			QColor newColor(newGray, newGray, newGray);
			resultImg.setPixelColor(x, y, newColor);
		}
	}

	return resultImg;
}

bool ImageProcessing::exportToPGM(const QImage& image, const QString& filename)
{
	// Ensure the directory exists; if not, create it
	QString directory = "C:/Users/karol/Documents/UNI_SCHOOL/STU/Inzinier/Semester_8/SpracovanieObrazu/ImageViewerSO/data";
	QDir dir(directory);
	if (!dir.exists()) {
		dir.mkpath(directory);
	}

	// Construct the full file path
	QString filePath = dir.filePath(filename);

	// Save the image in PGM format
	return image.save(filePath, "PGM");
}
