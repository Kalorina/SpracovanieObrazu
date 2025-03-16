#include "ImageProcessing.h"

ImageProcessing::ImageProcessing() {

}

ImageProcessing::~ImageProcessing() {

	delete[] m_pImgLocalData;
}

QImage ImageProcessing::pixelsMirror(QImage img, int padding)
{
	if (img.format() != QImage::Format_Grayscale8) {return QImage();}

	m_imgWidth = img.width();
	m_imgHeight = img.height();
	int bytesPerLine = img.bytesPerLine();
	const uchar* originalImgData = img.bits();  // Get raw image data

	// Compute new size with padding
	int imgWidthMirrored = img.width() + 2 * padding;
	int imgHeightMirrored = img.height() + 2 * padding;
	size_t size = static_cast<size_t>(imgWidthMirrored) * imgHeightMirrored;

	// Check if memory needs to be cleared
	if (m_pImgLocalData != nullptr)
	{
		delete[] m_pImgLocalData;
		m_pImgLocalData = nullptr;
	}

	// Allocate memory for padded image
	m_pImgLocalData = (double*)calloc(size, sizeof(double));
	if (m_pImgLocalData == nullptr)
		return QImage();

	int indexNew = 0, indexOld = 0;
	double tempD = 0.0;

	// Copy the original image into the center of the padded image
	for (int i = 0; i < m_imgHeight; i++) {
		for (int j = 0; j < m_imgWidth; j++) {
			indexNew = (i + padding) * imgWidthMirrored + (j + padding);
			indexOld = i * bytesPerLine + j;
			tempD = static_cast<double>(originalImgData[indexOld]) / 255.0;  // Normalize to 0.0 - 1.0
			m_pImgLocalData[indexNew] = tempD;
		}
	}

	// Mirror over Upper and Lower edges
	for (int i = 0; i < padding; i++) {
		for (int j = 0; j < imgWidthMirrored; j++) {
			// Upper edge
			indexOld = (padding + i) * imgWidthMirrored + j;
			indexNew = (padding - i - 1) * imgWidthMirrored + j;
			m_pImgLocalData[indexNew] = m_pImgLocalData[indexOld];

			// Lower edge (mirror upward)
			// Lower edge (mirror upward correctly)
			indexOld = (imgHeightMirrored - padding - 1 - i) * imgWidthMirrored + j; // Reflecting row
			indexNew = (imgHeightMirrored - padding + i) * imgWidthMirrored + j; // Proper mirrored position
			m_pImgLocalData[indexNew] = m_pImgLocalData[indexOld];
		}
	}


	// Mirror over Left and Right edges
	for (int i = 0; i < imgHeightMirrored; i++) {
		for (int j = 0; j < padding; j++) {
			// Left edge
			indexOld = i * imgWidthMirrored + (j + padding); // Original pixel position 
			indexNew = i * imgWidthMirrored + (padding - j - 1); // Mirrored position
			m_pImgLocalData[indexNew] = m_pImgLocalData[indexOld];

			// Right edge
			indexOld = i * imgWidthMirrored + (imgWidthMirrored - padding - 1 - j); // Original pixel position
			indexNew = i * imgWidthMirrored + (imgWidthMirrored - padding + j); // Mirrored position
			m_pImgLocalData[indexNew] = m_pImgLocalData[indexOld];
		}
	}

	// Convert to QImage
	QImage img_mirrored(imgWidthMirrored, imgHeightMirrored, QImage::Format_Grayscale8);
	uchar* imageData = img_mirrored.bits(); // pointer for data

	for (int i = 0; i < imgHeightMirrored; i++) {
		for (int j = 0; j < imgWidthMirrored; j++) {
			int index = i * imgWidthMirrored + j;
			int pixelValue = static_cast<int>(m_pImgLocalData[index] * 255.0);  // Scale to 0-255
			pixelValue = qBound(0, pixelValue, 255);  // Ensure within valid range

			imageData[i * img_mirrored.bytesPerLine() + j] = static_cast<uchar>(pixelValue);
		}
	}

	// Testing Mirroring
	exportToPGM(img_mirrored, "Mirrored_test.pgm");
	return img_mirrored;
}

QImage ImageProcessing::pixelsUnmirror(QImage img, int padding)
{
	int mirroredWidth = img.width();
	int mirroredHeight = img.height();

	int originalWidth = mirroredWidth - 2 * padding;
	int originalHeight = mirroredHeight - 2 * padding;

	if (originalWidth <= 0 || originalHeight <= 0) {
		qDebug() << "Error: Invalid crop size.";
		return img; 
	}

	// Crop in the center
	int xOffset = (mirroredWidth - originalWidth) / 2;
	int yOffset = (mirroredHeight - originalHeight) / 2;

	QImage resultImg = img.copy(padding, padding, originalWidth, originalHeight);
	exportToPGM(resultImg, "Unmirrored_test.pgm");

	return resultImg;
}

QImage ImageProcessing::Convolution(QImage img, int padding)
{
	QImage mirroredImg = img.copy();

	// Mirroring 
	mirroredImg = pixelsMirror(mirroredImg, padding);

	// Convolution
	
	int mirroredWidth = mirroredImg.width();
	int mirroredHeight = mirroredImg.height();

	// output image with original size
	QImage outputImg(img.size(), QImage::Format_Grayscale8);

	int maskRadius = convolution_mask.size() / 2;

	for (int y = 0; y < img.height(); ++y) { 
		for (int x = 0; x < img.width(); ++x) {
			int newValue = 0;

			for (int ky = -maskRadius; ky <= maskRadius; ++ky) {
				for (int kx = -maskRadius; kx <= maskRadius; ++kx) {
					// Compute coordinates in the **mirrored** image
					int pixelX = x + kx + padding;
					int pixelY = y + ky + padding;

					int grayValue = qGray(mirroredImg.pixel(pixelX, pixelY));

					newValue += grayValue * convolution_mask[ky + maskRadius][kx + maskRadius];
				}
			}

			// qBound(min, value, max) -> ensures range <max,min>
			newValue = qBound(0, newValue, 255);

			outputImg.setPixelColor(x, y, QColor(newValue, newValue, newValue));
		}
	}
	exportToPGM(outputImg, "Unmirrored_test.pgm");
	return outputImg;
}

QVector<QImage> ImageProcessing::schemeExplicit(QImage img, int stepCount, double timeStep)
{
	qDebug() << "Linear Heat Eq Explicit scheme";

	QVector<QImage> images;

	// Convert to grayscale if img is RGB
	QImage currentImg = img.convertToFormat(QImage::Format_Grayscale8);

	for (int step = 0; step < stepCount; step++) {
		QImage nextImg = pixelsMirror(currentImg, 1);

		for (int y = 1; y < img.height() - 1; y++) {
			for (int x = 1; x < img.width() - 1; x++) {
				int u_p_n = qGray(currentImg.pixel(x, y));

				int u_q1 = qGray(currentImg.pixel(x + 1, y)); // Right neighbor
				int u_q2 = qGray(currentImg.pixel(x - 1, y)); // Left neighbor
				int u_q3 = qGray(currentImg.pixel(x, y + 1)); // Bottom neighbor
				int u_q4 = qGray(currentImg.pixel(x, y - 1)); // Top neighbor

				// newVal = u_p_n+1
				int newVal = (1 - 4 * timeStep) * u_p_n + timeStep * (u_q1 + u_q2 + u_q3 + u_q4);
				// qBound(min, value, max) -> value in <min, max>
				newVal = qBound(0, newVal, 255);

				nextImg.setPixel(x, y, qRgb(newVal, newVal, newVal));
			}
		}

		qDebug() << "Intensity Mean:" << computeImageMeanIntesity(nextImg);;

		images.append(nextImg.copy(1, 1, img.width(), img.height()));
		currentImg = nextImg;
	}

	return images;
}

QVector<QImage> ImageProcessing::schemeExplicitFloat(QImage img, int stepCount, double timeStep)
{
	qDebug() << "Linear Heat Eq Explicit scheme with floating-point precision";

	// Convert to grayscale if img is RGB
	// QImage img = img.convertToFormat(QImage::Format_Grayscale8);

	QImage m_img = pixelsMirror(img, 1);

	QVector<QImage> images;
	QVector<QVector<float>> pixelValues(m_img.width(), QVector<float>(m_img.height(), 0.0));

	// original image values
	for (int x = 0; x < m_img.width(); x++) {
		for (int y = 0; y < m_img.height(); y++) {
			pixelValues[x][y] = qGray(m_img.pixel(x, y));
		}
	}

	for (int step = 0; step < stepCount; step++)
	{
		QImage currentImg(img.width(), img.height(), QImage::Format_Grayscale8);
		// currentImg = width x height from img original -> without mirroring 
		// pixels values 2D vecotr = width+2 x height+2 -> with mirroring padding:1
		QVector<QVector<float>> newPixelValues = pixelValues;

		for (int x = 1; x < m_img.width() - 1; x++) {
			for (int y = 1; y < m_img.height() - 1; y++) {
				float u_p_n = pixelValues[x][y];

				// Mirrored neighbors
				float u_q1 = pixelValues[x + 1][y];
				float u_q2 = pixelValues[x - 1][y];
				float u_q3 = pixelValues[x][y + 1];
				float u_q4 = pixelValues[x][y - 1];

				newPixelValues[x][y] = (1 - 4 * timeStep) * u_p_n + timeStep * (u_q1 + u_q2 + u_q3 + u_q4);
			}
		}
		// all new pixel values updated /w mirrored pixels 
		pixelValues = newPixelValues;

		// boundary is updated so that the flux on boundary stays the same 
		// edges
		for (int x = 1; x < m_img.width() - 1; x++) {
			pixelValues[x][0] = pixelValues[x][1];									// Top boundary
			pixelValues[x][m_img.height() - 1] = pixelValues[x][m_img.height() - 2];	// Bottom boundary
		}
		for (int y = 1; y < m_img.height() - 1; y++) {
			pixelValues[0][y] = pixelValues[1][y];									// Left boundary
			pixelValues[m_img.width() - 1][y] = pixelValues[m_img.width() - 2][y];		// Right boundary
		}

		// corners 
		pixelValues[0][0] = pixelValues[1][1];																	// Top-left
		pixelValues[m_img.width() - 1][0] = pixelValues[m_img.width() - 2][1];									// Top-right
		pixelValues[0][m_img.height() - 1] = pixelValues[1][m_img.height() - 2];								// Bottom-left
		pixelValues[m_img.width() - 1][m_img.height() - 1] = pixelValues[m_img.width() - 2][m_img.height() - 2];// Bottom-right

		// the pixel values back to an image
		for (int x = 1; x < m_img.width() - 1; x++) {
			for (int y = 1; y < m_img.height() - 1; y++) {
				// qBound(min, value, max) -> value in <min, max>
				int newVal = qBound(0, (int)pixelValues[x][y], 255);
				currentImg.setPixel(x - 1, y - 1, qRgb(newVal, newVal, newVal)); 
			}
		}

		float currentMean = computeImageMeanIntesity(currentImg);
		qDebug() << "Intensity Mean:" << currentMean;

		images.append(currentImg);
	}

	return images;
}

QVector<QImage> ImageProcessing::schemeImplicit(QImage img, int stepCount, double timeStep)
{
	qDebug() << "Linear Heat Eq Implicit scheme";
	return QVector<QImage>();
}

float ImageProcessing::computeImageMeanIntesity(QImage img)
{
	if (img.isNull()) return 0.0;

	qint64 sumIntensity = 0.0;
	for (int y = 0; y < img.height(); y++)
	{
		for (int x = 0; x < img.width(); x++)
		{
			sumIntensity += qGray(img.pixel(x, y)); // grayscale intensity
		}
	}

	return (float)sumIntensity / (img.width() * img.height());
}

QVector<int> ImageProcessing::computeHistogram(QImage img) {
	QVector<int> histogram(256, 0); // Initialize histogram with 256 bins (0-255)

	for (int y = 0; y < img.height(); ++y) {
		for (int x = 0; x < img.width(); ++x) {
			QColor pixel = img.pixelColor(x, y);
			int gray = qGray(pixel.red(), pixel.green(), pixel.blue());
			histogram[gray]++; 
		}
	}

	return histogram;
}

QImage ImageProcessing::FSHS(QImage img)
{
	if (img.isNull()) return img;

	// copy of the image
	QImage resultImg(img.width(), img.height(), img.format());

	// if image is grayscale
	bool isGrayscale = img.isGrayscale();
	if (!isGrayscale) return img;

	QVector<int> histogram = computeHistogram(img);

	// minimum and maximum intensity values
	int minIntensity = 255;
	int maxIntensity = 0;

	for (int i = 0; i < histogram.size(); ++i) {
		if (histogram[i] > 0) { // Only consider non-zero intensity values
			minIntensity = std::min(minIntensity, i);
			maxIntensity = std::max(maxIntensity, i);
		}
	}

	// stretch to each pixel
	for (int y = 0; y < img.height(); ++y) {
		for (int x = 0; x < img.width(); ++x) {
			QColor oldColor = img.pixelColor(x, y);
			int gray = qGray(oldColor.red(), oldColor.green(), oldColor.blue());

			// stretch
			int newGray = 0;
			if (maxIntensity > minIntensity) {
				newGray = static_cast<int>(255.0 * (gray - minIntensity) / (maxIntensity - minIntensity));
			}

			// qBound(min, value, max)
			newGray = qBound(0, newGray, 255);
			resultImg.setPixelColor(x, y, QColor(newGray, newGray, newGray));
		}
	}

	return resultImg;
}

QImage ImageProcessing::EH(QImage img)
{
	if (img.isNull()) return img;

	// copy of the image
	QImage resultImg(img.width(), img.height(), img.format());

	// if image is grayscale
	bool isGrayscale = img.isGrayscale();
	if (!isGrayscale) return img;

	int totalPixels = img.width() * img.height();

	QVector<int> histogram = computeHistogram(img);

	// Cumulative distribution function (CDF)
	QVector<int> cdf(256, 0);

	// Initialize first element of CDF
	cdf[0] = histogram[0];

	// CDF for all other values
	for (int i = 1; i < 256; ++i) {
		cdf[i] = cdf[i - 1] + histogram[i];
	}

	// minimum non-zero CDF value 
	int cdfMin = 0;
	for (int i = 0; i < 256; ++i) {
		if (cdf[i] > 0) {
			cdfMin = cdf[i];
			break;
		}
	}
	// mapping lookup table
	QVector<int> mapping(256, 0);

	for (int i = 0; i < 256; ++i) {
		// equalized value using the CDF
		if (cdf[i] == 0)
			mapping[i] = 0;
		else
			mapping[i] = std::round(((float)(cdf[i] - cdfMin) / (float)(totalPixels - cdfMin)) * 255.0f);
	}

	// equalization to the image
	for (int y = 0; y < img.height(); ++y) {
		for (int x = 0; x < img.width(); ++x) {
			QColor oldColor = img.pixelColor(x, y);
			int gray = qGray(oldColor.red(), oldColor.green(), oldColor.blue());
			int newGray = mapping[gray];

			// qBound(min, value, max)
			newGray = qBound(0, newGray, 255);

			resultImg.setPixelColor(x, y, QColor(newGray, newGray, newGray));
		}
	}

	return resultImg;
}

bool ImageProcessing::exportToPGM(QImage img, const QString& filename)
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
	return img.save(filePath, "PGM");
}
