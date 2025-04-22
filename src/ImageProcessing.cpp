#include "ImageProcessing.h"

ImageProcessing::ImageProcessing() {

}

ImageProcessing::~ImageProcessing() {

	delete[] m_pImgLocalData;
}

QVector<QVector<double>> ImageProcessing::convertTo2Dvector(QImage img)
{
	QVector<QVector<double>> pixelValues(img.width(), QVector<double>(img.height(), 0.0));

	// original image values
	for (int x = 0; x < img.width(); x++) {
		for (int y = 0; y < img.height(); y++) {
			pixelValues[x][y] = qGray(img.pixel(x, y));
		}
	}

	pixelValues_org = pixelValues;
	return pixelValues;
}

QVector<QVector<double>> ImageProcessing::convertTo2DvectorNorm(QImage img)
{
	QVector<QVector<double>> pixelValues(img.width(), QVector<double>(img.height(), 0.0));

	// original image values
	for (int x = 0; x < img.width(); x++) {
		for (int y = 0; y < img.height(); y++) {
			pixelValues[x][y] = qBound(0, qGray(img.pixel(x, y)) / 255, 1);
		}
	}

	pixelValues_org = pixelValues;
	return pixelValues;
}

QImage ImageProcessing::convertToQImage(QVector<QVector<double>> pixelValues, int width, int height)
{
	QImage Img(width,height, QImage::Format_Grayscale8);

	// the pixel values back to an image
	for (int x = 0; x < width; x++) {
		for (int y = 0; y <height; y++) {
			// qBound(min, value, max) -> value in <min, max>
			int newVal = qBound(0, (int)pixelValues[x][y], 255);
			Img.setPixel(x, y, qRgb(newVal, newVal, newVal));
		}
	}
	return Img;
}

QImage ImageProcessing::convertToQImageMirrored(QVector<QVector<double>> pixelValues, int width, int height, int padding)
{
	// Umirrored Dimentions for QImage
	QImage Img(width - 2 * padding, height - 2 * padding, QImage::Format_Grayscale8);

	for (int x = padding; x < width - padding; x++) {
		for (int y = padding; y < height - padding; y++) {
			// qBound(min, value, max) -> value in <min, max>
			int newVal = qBound(0, (int)pixelValues[x][y], 255);
			Img.setPixel(x - padding, y - padding, qRgb(newVal, newVal, newVal));
		}
	}
	return Img;
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

	return img.copy(padding, padding, originalWidth, originalHeight);
}

void ImageProcessing::printImgData(QVector<QVector<double>> imgData, int min, int max)
{
	for (int x = min; x < max; x++)
	{
		QString line;
		for (int y = min; y < max; y++)
		{
			line += QString::number(imgData[x][y], 'f', 2) + " ";
		}
		qDebug() << line;
	}
}

void ImageProcessing::printImgData(QVector<QVector<double>> imgData)
{
	for (int y = 0; y < imgData[0].size(); y++) {
		QString row;
		for (int x = 0; x < imgData.size(); x++) {
			row += QString::number(imgData[x][y], 'f', 2) + " ";
		}
		qDebug() << row;
	}
}

QImage ImageProcessing::Convolution(QImage img, int padding)
{
	qDebug() << "Convolution started";
	QImage m_img = pixelsMirror(img.copy(), padding);

	QVector<QVector<double>> pixelValues = convertTo2Dvector(m_img);
	QVector<QVector<double>> outputImgData(img.width(), QVector<double>(img.height(), 0.0f));

	// Convolution
	int maskRadius = convolution_mask.size() / 2;

	for (int y = 0; y < img.height(); ++y) {
		for (int x = 0; x < img.width(); ++x) {
			double newValue = 0.0f;

			for (int ky = -maskRadius; ky <= maskRadius; ++ky) {
				for (int kx = -maskRadius; kx <= maskRadius; ++kx) {
					// Compute coordinates in the **mirrored** image
					int pixelX = x + kx + padding;
					int pixelY = y + ky + padding;

					// Ensure safe access within bounds
					if (pixelX >= 0 && pixelX < m_img.width() && pixelY >= 0 && pixelY < m_img.height()) {
						newValue += pixelValues[pixelX][pixelY] * convolution_mask[ky + maskRadius][kx + maskRadius];
					}
				}
			}
			outputImgData[x][y] = qBound(0.0f, newValue, 255.0f);
		}
	}

	return convertToQImage(outputImgData, img.width(), img.height());
	qDebug() << "Convolution done";
}

void ImageProcessing::EdgeDetectorSobelKernels(QImage img, double K)
{
	qDebug() << "Edge Detector -> gradients: Sobel kernels (filtered with IS-LHEq)";
	// Convert to grayscale if img is RGB
	img = img.convertToFormat(QImage::Format_Grayscale8);
	int padding = 1;
	QImage m_img = pixelsMirror(img, padding);
	QVector<QVector<double>> pixelValues = convertTo2Dvector(m_img); // RGB <0,255>
	// QVector<QVector<float>> pixelValues = convertTo2DvectorNorm(m_img); // Normalized <0,1>
	//printImgData(pixelValues, m_img.width() / 2 - 10, m_img.width() / 2 );

	//pixelValues = schemeExplicitDouble(pixelValues, 1, 0.2);
	pixelValues = schemeImplicitDouble(pixelValues, 1, 0.5);
	//printImgData(pixelValues, m_img.width() / 2 - 10, m_img.width() / 2 );

	QVector<QVector<double>> outputImgData(img.width(), QVector<double>(img.height(), 0.0f));

	//float K = 1.0f; //Norm <0,10000>
	//float K = 2.5f; //RGB <0,5>

	// Sobel filter
	for (int x = 1; x < m_img.width() - 1; x++) {
		for (int y = 1; y < m_img.height() - 1; y++) {
			double grad_x = 0, grad_y = 0;

			// convolution -> Sobel Kernel 4 directions 
			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {
					double pixelVal = pixelValues[x + i][y + j];
					grad_x += Gx[i + 1][j + 1] * pixelVal;
					grad_y += Gy[i + 1][j + 1] * pixelVal;
				}
			}

			double magE = std::abs(grad_x);
			double magS = std::abs(grad_y);

			// Compute mean gradient 
			double magnitude = (magE + magS) / 2;
			//float g_value = 1 / (1 + K * std::pow(magnitude, 2));
			outputImgData[x - 1][y - 1] = qBound(0.0f, magnitude, 255.0f);
		}
	}
	//printImgData(outputImgData, m_img.width() / 2 - 10, m_img.width() / 2);
	//printImgData(outputImgData);
	qDebug() << "Edge Detector Done";
	exportQImageToPGM(convertToQImage(outputImgData,img.width(),img.height()), "edgeDetector_gradient_fromImg.pgm");
}

QImage ImageProcessing::EdgeDetectorImgSobelKernels(QImage img, double K)
{
	qDebug() << "Edge Detector -> gradients: Sobel Kernels (filtered with IS-LHEq)";
	// Convert to grayscale if img is RGB
	img = img.convertToFormat(QImage::Format_Grayscale8);
	int padding = 1;
	QImage m_img = pixelsMirror(img, padding);
	QVector<QVector<double>> pixelValues = convertTo2Dvector(m_img); // RGB <0,255>
	// QVector<QVector<float>> pixelValues = convertTo2DvectorNorm(m_img); // Normalized <0,1>
	//printImgData(pixelValues, m_img.width() / 2 - 10, m_img.width() / 2 );

	//pixelValues = schemeExplicitDouble(pixelValues, 1, 0.2);
	pixelValues = schemeImplicitDouble(pixelValues, 1, 0.5);
	//printImgData(pixelValues, m_img.width() / 2 - 10, m_img.width() / 2 );

	QVector<QVector<double>> outputImgData(img.width(), QVector<double>(img.height(), 0.0f));

	//float K = 1.0f; //Norm <0,10000>
	//float K = 2.5f; //RGB <0,5>

	// Sobel filter
	for (int x = 1; x < m_img.width() - 1; x++) {
		for (int y = 1; y < m_img.height() - 1; y++) {
			double grad_x = 0, grad_y = 0;

			// convolution -> Sobel Kernel x and y directions 
			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {
					double pixelVal = pixelValues[x + i][y + j];
					grad_x += Gx[i + 1][j + 1] * pixelVal;
					grad_y += Gy[i + 1][j + 1] * pixelVal;
				}
			}

			double magE = std::abs(grad_x);
			double magS = std::abs(grad_y);

			// Compute mean gradient 
			double magnitude = (magE + magS) / 2;
			double g_value = 1 / (1 + K * std::pow(magnitude, 2));
			outputImgData[x - 1][y - 1] = qBound(0.0f, g_value * 255.0, 255.0f);
		}
	}
	//printImgData(outputImgData, m_img.width() / 2 - 10, m_img.width() / 2);
	//printImgData(outputImgData);
	qDebug() << "Edge Detector Done";
	exportQImageToPGM(convertToQImage(outputImgData, img.width(), img.height()), "edgeDetector_SobelKernels.pgm");
	QImage img_edgeD = convertToQImage(outputImgData, img.width(), img.height());
	return img_edgeD;
}

QImage ImageProcessing::EdgeDetectorImgDirectEdges(QImage img, double K)
{
	qDebug() << "Edge Detector -> gradients: direct Edge (filtered with IS-LHEq)";
	// Convert to grayscale if img is RGB
	img = img.convertToFormat(QImage::Format_Grayscale8);
	int padding = 1;
	QImage m_img = pixelsMirror(img, padding);
	QVector<QVector<double>> pixelValues = convertTo2Dvector(m_img); // RGB <0,255>
	// QVector<QVector<float>> pixelValues = convertTo2DvectorNorm(m_img); // Normalized <0,1>
	//printImgData(pixelValues, m_img.width() / 2 - 10, m_img.width() / 2 );

	//pixelValues = schemeExplicitDouble(pixelValues, 1, 0.2);
	pixelValues = schemeImplicitDouble(pixelValues, 1, 0.5);
	//printImgData(pixelValues, m_img.width() / 2 - 10, m_img.width() / 2 );

	QVector<QVector<double>> outputImgData(img.width(), QVector<double>(img.height(), 0.0f));

	//float K = 1.0f; //Norm <0,10000>
	//float K = 0.005f; //RGB <0,5>
	double h = 1.0f;

	// Sobel filter
	for (int x = 1; x < m_img.width() - 1; x++) {
		for (int y = 1; y < m_img.height() - 1; y++) {
			double gradX = 0, gradY = 0;
			double g_E = 0, g_N = 0, g_W = 0, g_S = 0;
			double normSq_qE = 0, normSq_qN = 0, normSq_qW = 0, normSq_qS = 0;

			// E = (x + 1, y)
			// W = (x - 1, y)
			// N = (x, y - 1)
			// S = (x, y + 1)
			// NE = (x + 1, y - 1)
			// SE = (x + 1, y + 1)
			// SW = (x - 1, y + 1)
			// NW = (x - 1, y - 1)
			
			//----- EAST edge -----//
			// x -> (E - p) / h
			// y -> (NE + N - S - SE) / 4h
			gradX = (pixelValues[x + 1][y] - pixelValues[x][y]) / h;
			gradY = (pixelValues[x + 1][y - 1] + pixelValues[x][y - 1] - pixelValues[x][y + 1] - pixelValues[x + 1][y + 1]) / (4.0 * h);

			normSq_qE = gradX * gradX + gradY * gradY;
			//g_E = diffCoefFunction(K, normSq);

			//----- NORTH edge -----//
			// y -> (N - p) / h
			// x -> (W + NW - E - NE) / 4h 
			gradX = (pixelValues[x-1][y] + pixelValues[x-1][y-1] - pixelValues[x+1][y] - pixelValues[x+1][y-1]) / (4.0 * h);
			gradY = (pixelValues[x][y - 1] - pixelValues[x][y]) / h;

			normSq_qN = gradX * gradX + gradY * gradY;
			//g_N = diffCoefFunction(K, normSq);

			//----- WEST edge -----//
			// x -> (p - W) / h
			// y -> (S + SW - N - NW) / 4h
			gradX = (pixelValues[x - 1][y] - pixelValues[x][y]) / h;
			gradY = (pixelValues[x][y + 1] + pixelValues[x - 1][y + 1] - pixelValues[x][y - 1] - pixelValues[x - 1][y - 1]) / (4.0 * h);

			normSq_qW = gradX * gradX + gradY * gradY;
			//g_W = diffCoefFunction(K, normSq);

			//----- SOUTH edge -----//
			// y -> (S - p) / h
			// x -> (E + SE - W - SW) / 4h
			gradX = (pixelValues[x + 1][y] + pixelValues[x + 1][y + 1] - pixelValues[x - 1][y] - pixelValues[x - 1][y + 1]) / (4.0 * h);
			gradY = (pixelValues[x][y + 1] - pixelValues[x][y]) / h;

			normSq_qS = gradX * gradX + gradY * gradY;
			//g_S = diffCoefFunction(K, normSq);

			// Compute mean gradient 
			double magnitude = (normSq_qE + normSq_qN + normSq_qW + normSq_qS) / 4;
			double g_value = diffCoefFunction(K, magnitude);
			outputImgData[x - 1][y - 1] = qBound(0.0f, g_value * 255.0, 255.0f);
		}
	}
	//printImgData(outputImgData, m_img.width() / 2 - 10, m_img.width() / 2);
	//printImgData(outputImgData);
	qDebug() << "Edge Detector Done";
	exportQImageToPGM(convertToQImage(outputImgData, img.width(), img.height()), "edgeDetector_DirectEdge.pgm");
	QImage img_edgeD = convertToQImage(outputImgData, img.width(), img.height());
	return img_edgeD;
}

QVector<double> ImageProcessing::EdgeDetectorGradient3x3(QVector<QVector<double>> imgData, int x, int y)
{
	// imgData -> 3x3 matrix in 2D vector

	QVector<double> gradients; // 4 directions E, N, W, S

	if (imgData.size() != 3 || imgData[0].size() != 3) {
		return QVector<double>(4, 0.0f);
	}
	double h = 1.0;
	
	// E = (x + 1, y)
	// W = (x - 1, y)
	// N = (x, y - 1)
	// S = (x, y + 1)
	// NE = (x + 1, y - 1)
	// SE = (x + 1, y + 1)
	// SW = (x - 1, y + 1)
	// NW = (x - 1, y - 1)
	double gradX = 0, gradY = 0;
	double normSq_qE = 0, normSq_qN = 0, normSq_qW = 0, normSq_qS = 0;

	//----- EAST edge -----//
	// x -> (E - p) / h
	// y -> (NE + N - S - SE) / 4h
	gradX = (imgData[x + 1][y] - imgData[x][y]) / h;
	gradY = (imgData[x + 1][y - 1] + imgData[x][y - 1] - imgData[x][y + 1] - imgData[x + 1][y + 1]) / (4.0 * h);

	normSq_qE = gradX * gradX + gradY * gradY;

	//----- NORTH edge -----//
	// y -> (N - p) / h
	// x -> (W + NW - E - NE) / 4h 
	gradX = (imgData[x - 1][y] + imgData[x - 1][y - 1] - imgData[x + 1][y] - imgData[x + 1][y - 1]) / (4.0 * h);
	gradY = (imgData[x][y - 1] - imgData[x][y]) / h;

	normSq_qN = gradX * gradX + gradY * gradY;

	//----- WEST edge -----//
	// x -> (p - W) / h
	// y -> (S + SW - N - NW) / 4h
	gradX = (imgData[x - 1][y] - imgData[x][y]) / h;
	gradY = (imgData[x][y + 1] + imgData[x - 1][y + 1] - imgData[x][y - 1] - imgData[x - 1][y - 1]) / (4.0 * h);

	normSq_qW = gradX * gradX + gradY * gradY;

	//----- SOUTH edge -----//
	// y -> (S - p) / h
	// x -> (E + SE - W - SW) / 4h
	gradX = (imgData[x + 1][y] + imgData[x + 1][y + 1] - imgData[x - 1][y] - imgData[x - 1][y + 1]) / (4.0 * h);
	gradY = (imgData[x][y + 1] - imgData[x][y]) / h;

	normSq_qS = gradX * gradX + gradY * gradY; 

	gradients.append(normSq_qE);
	gradients.append(normSq_qN);
	gradients.append(normSq_qW);
	gradients.append(normSq_qS);

	//qDebug() << "Edge Detector -> Direct gradients of pixel for Perona-Malikova Done";
	return gradients;
}

QVector<QImage> ImageProcessing::schemeExplicit(QImage img, int stepCount, double timeStep)
{
	qDebug() << "Linear Heat Eq Explicit scheme";
	// Convert to grayscale if img is RGB
	img = img.convertToFormat(QImage::Format_Grayscale8);
	int padding = 1;
	QImage m_img = pixelsMirror(img, padding);

	QVector<QImage> images;
	// pixels values 2D vecotr = width+2 x height+2 -> with mirroring padding:1
	QVector<QVector<double>> pixelValues = convertTo2Dvector(m_img);

	for (int step = 0; step < stepCount; step++)
	{

		QVector<QVector<double>> newPixelValues = pixelValues;

		for (int x = 1; x < m_img.width() - 1; x++) {
			for (int y = 1; y < m_img.height() - 1; y++) {

				double u_p_n = pixelValues[x][y];
				// Mirrored neighbors
				double u_q1 = pixelValues[x + 1][y];
				double u_q2 = pixelValues[x - 1][y];
				double u_q3 = pixelValues[x][y + 1];
				double u_q4 = pixelValues[x][y - 1];

				newPixelValues[x][y] = (1 - 4 * timeStep) * u_p_n 
					+ timeStep * (u_q1 + u_q2 + u_q3 + u_q4);
			}
		}

		// boundary is updated so that the flux on boundary stays the same 
		// padding = 1;
		// edges
		for (int x = 1; x < m_img.width() - 1; x++) {
			newPixelValues[x][0] = newPixelValues[x][1];									// Top boundary
			newPixelValues[x][m_img.height() - 1] = newPixelValues[x][m_img.height() - 2];	// Bottom boundary
		}
		for (int y = 1; y < m_img.height() - 1; y++) {
			newPixelValues[0][y] = newPixelValues[1][y];									// Left boundary
			newPixelValues[m_img.width() - 1][y] = newPixelValues[m_img.width() - 2][y];		// Right boundary
		}

		// corners 
		newPixelValues[0][0] = newPixelValues[1][1];																	// Top-left
		newPixelValues[m_img.width() - 1][0] = newPixelValues[m_img.width() - 2][1];									// Top-right
		newPixelValues[0][m_img.height() - 1] = newPixelValues[1][m_img.height() - 2];								// Bottom-left
		newPixelValues[m_img.width() - 1][m_img.height() - 1] = newPixelValues[m_img.width() - 2][m_img.height() - 2];// Bottom-right

		pixelValues = newPixelValues;

		qDebug() << "Intensity Mean:" << computeImageMeanIntesity(pixelValues, m_img.width(), m_img.height());

		// currentImg = width x height -> without mirroring
		// QImage currentImg(img.width(), img.height(), QImage::Format_Grayscale8);
		// the pixel values back to an image
		QImage currentImg = convertToQImageMirrored(pixelValues, m_img.width(), m_img.height(), padding);
		images.append(currentImg);
	}
	return images;
}

QVector<QVector<double>> ImageProcessing::schemeExplicitDouble(QVector<QVector<double>> imgData, int stepCount, double timeStep)
{
	int width = imgData.size();
	if (width == 0) return QVector<QVector<double>>();
	int height = imgData[0].size();

	QVector<QVector<double>> currentImgData = imgData;

	for (int step = 0; step < stepCount; step++)
	{
		// pixels values 2D vecotr = width+2 x height+2 -> with mirroring padding:1
		QVector<QVector<double>> newPixelValues = currentImgData;

		for (int x = 1; x < width - 1; x++) {
			for (int y = 1; y < height - 1; y++) {

				double u_p_n = currentImgData[x][y];
				// Mirrored neighbors
				double u_q1 = currentImgData[x + 1][y];
				double u_q2 = currentImgData[x - 1][y];
				double u_q3 = currentImgData[x][y + 1];
				double u_q4 = currentImgData[x][y - 1];

				newPixelValues[x][y] = (1 - 4 * timeStep) * u_p_n + timeStep * (u_q1 + u_q2 + u_q3 + u_q4);
			}
		}

		// boundary is updated so that the flux on boundary stays the same 
		// padding = 1;
		// edges
		for (int x = 1; x < width - 1; x++) {
			newPixelValues[x][0] = newPixelValues[x][1];						// Top boundary
			newPixelValues[x][height - 1] = newPixelValues[x][height - 2];	// Bottom boundary
		}
		for (int y = 1; y < height - 1; y++) {
			newPixelValues[0][y] = newPixelValues[1][y];						// Left boundary
			newPixelValues[width - 1][y] = newPixelValues[width - 2][y];		// Right boundary
		}

		// corners 
		newPixelValues[0][0] = newPixelValues[1][1];									// Top-left
		newPixelValues[width - 1][0] = newPixelValues[width - 2][1];					// Top-right
		newPixelValues[0][height - 1] = newPixelValues[1][height - 2];				// Bottom-left
		newPixelValues[width - 1][height - 1] = newPixelValues[width - 2][height - 2];// Bottom-right
		
		currentImgData = newPixelValues;
		qDebug() << "Intensity Mean:" << computeImageMeanIntesity(currentImgData, width, height);
	}
	return currentImgData;
}

QVector<QImage> ImageProcessing::schemeImplicit(QImage img, int stepCount, double timeStep)
{
	QVector<QImage> images;

	if (img.isNull() || stepCount <= 0 || timeStep <= 0) {
		return images;
	}

	int width = img.width();
	int height = img.height();
	int padding = 1;

	QImage m_img = pixelsMirror(img.convertToFormat(QImage::Format_Grayscale8), padding);
	QVector<QVector<double>> phi = convertTo2Dvector(m_img);
	QVector<double> b(width * height, 0.0);	// RHS vector (b) 
	QVector<QVector<double>> phi_new = phi;

	// SOR parameters
	// A * phi = b phi -> pixel values
	// Aii = 1 + 4 * timeStep
	// Aij = -timeStep
	double omega = 1.25;
	const int MAX_ITER = 1000;
	const double TOL = 1.0E-7;
	double Aii = 1.0 + 4 * timeStep;
	double Aij = -timeStep;

	qDebug() << "Linear Heat Eq Implicit scheme SOR method MaxIter" << MAX_ITER << "tolerance:" << TOL;

	for (int t = 0; t < stepCount; t++)
	{
		// b with pixel values
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				b[y * width + x] = phi[x + padding][y + padding];
			}
		}

		double rezid = 0.0;
		int iter = 0;

		do
		{
			iter++;
			rezid = 0.0;

			for (int x = 1; x < m_img.width() - 1; x++)
			{
				for (int y = 1; y < m_img.height() - 1; y++)
				{
					double neighborSum =
						(x > 1 ? phi_new[x - 1][y] : phi[x - 1][y]) + // left (West) 
						phi[x + 1][y] +   // right (East)
						(y > 1 ? phi_new[x][y - 1] : phi[x][y - 1]) + // top (North)
						phi[x][y + 1];    // bottom (South)

					// Sum of neighbors
					double sigmaSOR = Aij * neighborSum;
					double originalVal = phi[x][y];
					double newVal = (1.0 - omega) * originalVal + (omega / Aii) * (b[(y - padding) * width + (x - padding)] - sigmaSOR);
					phi_new[x][y] = newVal;

					rezid += (newVal - originalVal) * (newVal - originalVal);
				}
			}

			phi = phi_new;
			// mirroring: boundary conditions -> zero flux
			// edges
			for (int x = 1; x < m_img.width() - 1; x++) {
				phi[x][0] = phi[x][1];									// Top boundary
				phi[x][m_img.height() - 1] = phi[x][m_img.height() - 2];// Bottom boundary
			}
			for (int y = 1; y < m_img.height() - 1; y++) {
				phi[0][y] = phi[1][y];									// Left boundary
				phi[m_img.width() - 1][y] = phi[m_img.width() - 2][y];	// Right boundary
			}
			// corners 
			phi[0][0] = phi[1][1];																	// Top-left
			phi[m_img.width() - 1][0] = phi[m_img.width() - 2][1];									// Top-right
			phi[0][m_img.height() - 1] = phi[1][m_img.height() - 2];								// Bottom-left
			phi[m_img.width() - 1][m_img.height() - 1] = phi[m_img.width() - 2][m_img.height() - 2];// Bottom-right

			// Stopping condition - convergence
			rezid = sqrt(rezid / (width * height));
			//qDebug() << "rezid:" << rezid;

			if (rezid < TOL) {
				qDebug() << "break; iter:" << iter << "rezid:" << rezid;
				break;
			}
				
		} while (iter < MAX_ITER);

		// Intensity Mean
		double img_mean = computeImageMeanIntesity(phi, m_img.width(), m_img.height());
		qDebug() << "Intensity Mean:" << img_mean;

		// Convert updated new pixel values back to QImage
		//QImage currentImg = convertToQImageMirrored(pixelValues, m_img.width(), m_img.height(), 1);
		images.append(convertToQImageMirrored(phi, m_img.width(), m_img.height(), 1));
	}

	return images;
}

QVector<QVector<double>> ImageProcessing::schemeImplicitDouble(QVector<QVector<double>> imgData, int stepCount, double timeStep)
{
	int padding = 1;

	int width_mirrored = imgData.size();
	if (width_mirrored == 0) return QVector<QVector<double>>();
	int width = imgData.size() - 2 * padding;
	int height = imgData[0].size() - 2 * padding;
	int height_mirrored = imgData[0].size();

	QVector<double> b(width * height, 0.0);	// RHS vector (b) 
	QVector<QVector<double>> phi = imgData;
	QVector<QVector<double>> phi_new = phi;

	// SOR parameters
	// A * phi = b
	// Aii = 1 + 4 * timeStep
	// Aij = -timeStep
	double omega = 1.25;
	const int MAX_ITER = 1000;
	const double TOL = 1.0E-7;
	double Aii = 1.0 + 4 * timeStep;
	double Aij = -timeStep;

	//qDebug() << "Linear Heat Eq Implicit scheme SOR method";

	for (int t = 0; t < stepCount; t++)
	{
		// b with pixel values
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				b[y * width + x] = phi[x + padding][y + padding];
			}
		}

		double rezid = 0.0;
		int iter = 0;

		do
		{
			iter++;
			rezid = 0.0;

			for (int x = 1; x < width_mirrored - 1; x++)
			{
				for (int y = 1; y < height_mirrored - 1; y++)
				{
					double neighborSum =
						(x > 1 ? phi_new[x - 1][y] : phi[x - 1][y]) + // left (West) 
						phi[x + 1][y] +   // right (East)
						(y > 1 ? phi_new[x][y - 1] : phi[x][y - 1]) + // top (North)
						phi[x][y + 1];    // bottom (South)

					// Sum of neighbors
					double sigmaSOR = Aij * neighborSum;
					double originalVal = phi[x][y];
					double newVal = (1.0 - omega) * originalVal + (omega / Aii) * (b[(y - padding) * width + (x - padding)] - sigmaSOR);
					phi_new[x][y] = newVal;

					rezid += (newVal - originalVal) * (newVal - originalVal);
				}
			}

			phi = phi_new;
			// mirroring: boundary conditions -> zero flux
			// edges
			for (int x = 1; x < width_mirrored - 1; x++) {
				phi[x][0] = phi[x][1];									// Top boundary
				phi[x][height_mirrored - 1] = phi[x][height_mirrored - 2];// Bottom boundary
			}
			for (int y = 1; y < height_mirrored - 1; y++) {
				phi[0][y] = phi[1][y];									// Left boundary
				phi[width_mirrored - 1][y] = phi[width_mirrored - 2][y];	// Right boundary
			}
			// corners 
			phi[0][0] = phi[1][1];																	// Top-left
			phi[width_mirrored - 1][0] = phi[width_mirrored - 2][1];									// Top-right
			phi[0][height_mirrored - 1] = phi[1][height_mirrored - 2];								// Bottom-left
			phi[width_mirrored - 1][height_mirrored - 1] = phi[width_mirrored - 2][height_mirrored - 2];// Bottom-right

			// Stopping condition - convergence
			rezid = sqrt(rezid / (width * height));
			//qDebug() << "rezid:" << rezid;

			if (rezid < TOL) {
				qDebug() << "break; iter:" << iter << "rezid:" << rezid;
				break;
			}

		} while (iter < MAX_ITER);

		// Intensity Mean
		//double img_mean = computeImageMeanIntesity(phi, m_img.width(), m_img.height());
		//qDebug() << "Intensity Mean:" << img_mean;

	}

	return phi;
}

QVector<QImage> ImageProcessing::schemeSemi_Implicit(QImage img, int stepCount, double timeStep, double omega, double sigma, double K)
{
	QVector<QImage> images;

	if (img.isNull() || stepCount <= 0 || timeStep <= 0) {
		return images;
	}

	int width = img.width();
	int height = img.height();
	int padding = 1;

	QImage m_img = pixelsMirror(img.convertToFormat(QImage::Format_Grayscale8), padding);
	QVector<QVector<double>> phi = convertTo2Dvector(m_img);
	QVector<double> b(width * height, 0.0);	// RHS vector (b) 
	QVector<QVector<double>> phi_new = phi;

	QVector<QVector<double>> filtered_phi;
	QVector<QVector<double>> Aii(m_img.width(), QVector<double>(m_img.height(), 0.0f));
	QVector<QVector<double>> Aij_E(m_img.width(), QVector<double>(m_img.height(), 0.0f));
	QVector<QVector <double>> Aij_W(m_img.width(), QVector<double>(m_img.height(), 0.0f));
	QVector<QVector<double>> Aij_N(m_img.width(), QVector<double>(m_img.height(), 0.0f));
	QVector<QVector <double>> Aij_S(m_img.width(), QVector<double>(m_img.height(), 0.0f));
	QVector<QVector<double>> selectedPixels(3, QVector<double>(3, 0.0));
	QVector<double> gradients; // 4 directions E, N, W, S

	// SOR parameters
	const int MAX_ITER = 1000;
	const double TOL = 1.0E-7;
	/*double Aii = 1.0 + 4 * timeStep;
	double Aij = -timeStep;*/
	double g_uE = 0.0, g_uN = 0.0, g_uW = 0.0, g_uS = 0.0;
	double u_qE = 0.0, u_qN = 0.0, u_qW = 0.0, u_qS = 0.0;

	qDebug() << "Semi-Implicit Perona-Malikova method; MaxIter" << MAX_ITER << "tolerance:" << TOL;

	for (int step = 0; step < stepCount; step++)
	{
		// b with pixel values
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				b[y * width + x] = phi[x + padding][y + padding];
			}
		}

		qDebug() << "Time step : " << step;
		//Filter data with ES/IS-LHEq with tau=1 based on sigma 
		if (sigma <= 0.25) {
			qDebug() << "Data filtration with Explicit Scheme";
			filtered_phi = schemeExplicitDouble(phi, 1, timeStep);
		}
		else {
			qDebug() << "Data filtration with Implicit Scheme";
			filtered_phi = schemeImplicitDouble(phi, 1, timeStep);
		}
		qDebug() << "Data filtration done";

		double rezid = 0.0;
		int iter = 0;

		do
		{
			iter++;
			rezid = 0.0;

			for (int x = 1; x < m_img.width() - 1; x++)
			{
				for (int y = 1; y < m_img.height() - 1; y++)
				{
					// Select 3x3 pixels
					for (int i = -1; i <= 1; i++) {
						for (int j = -1; j <= 1; j++) {
							selectedPixels[i + 1][j + 1] = filtered_phi[x + i][y + j];
						}
					}
					// 4 directions E, N, W, S
					gradients = EdgeDetectorGradient3x3(selectedPixels, 1, 1);

					g_uE = 1.0 / (1.0 + (K * gradients[0])); // East
					g_uN = 1.0 / (1.0 + (K * gradients[1])); // North
					g_uW = 1.0 / (1.0 + (K * gradients[2])); // West
					g_uS = 1.0 / (1.0 + (K * gradients[3])); // South

					Aii[x][y] = 1 + timeStep * (g_uN + g_uS + g_uE + g_uW);
					Aij_E[x][y] = -timeStep * g_uE;
					Aij_W[x][y] = -timeStep * g_uW;
					Aij_N[x][y] = -timeStep * g_uN;
					Aij_S[x][y] = -timeStep * g_uS;

					// neighbors
					u_qE = phi[x + 1][y];								// right (East)
					u_qW = (x > 1 ? phi_new[x - 1][y] : phi[x - 1][y]); // left (West)
					u_qN = (y > 1 ? phi_new[x][y - 1] : phi[x][y - 1]); // top (North)
					u_qS = phi[x][y + 1];								// bottom (South)

					double sigmaSOR = Aij_E[x][y] * u_qE + Aij_N[x][y] * u_qN + Aij_W[x][y] * u_qW + Aij_S[x][y] * u_qS;
					double originalVal = phi[x][y];
					double newVal = (1.0 - omega) * originalVal + (omega / Aii[x][y]) * (b[(y - padding) * img.width() + (x - padding)] - sigmaSOR);
					phi_new[x][y] = newVal;

					rezid += (newVal - originalVal) * (newVal - originalVal);
				}
			}

			phi = phi_new;
			// mirroring: boundary conditions -> zero flux
			// edges
			for (int x = 1; x < m_img.width() - 1; x++) {
				phi[x][0] = phi[x][1];									// Top boundary
				phi[x][m_img.height() - 1] = phi[x][m_img.height() - 2];// Bottom boundary
			}
			for (int y = 1; y < m_img.height() - 1; y++) {
				phi[0][y] = phi[1][y];									// Left boundary
				phi[m_img.width() - 1][y] = phi[m_img.width() - 2][y];	// Right boundary
			}
			// corners 
			phi[0][0] = phi[1][1];																	// Top-left
			phi[m_img.width() - 1][0] = phi[m_img.width() - 2][1];									// Top-right
			phi[0][m_img.height() - 1] = phi[1][m_img.height() - 2];								// Bottom-left
			phi[m_img.width() - 1][m_img.height() - 1] = phi[m_img.width() - 2][m_img.height() - 2];// Bottom-right

			// Stopping condition - convergence
			rezid = sqrt(rezid / (width * height));
			//qDebug() << "rezid:" << rezid;

			if (rezid < TOL) {
				qDebug() << "break; iter:" << iter << "rezid:" << rezid;
				break;
			}

		} while (iter < MAX_ITER);

		// Intensity Mean
		double img_mean = computeImageMeanIntesity(phi, m_img.width(), m_img.height());
		qDebug() << "Intensity Mean:" << img_mean;

		// Convert updated new pixel values back to QImage
		//QImage currentImg = convertToQImageMirrored(pixelValues, m_img.width(), m_img.height(), 1);
		images.append(convertToQImageMirrored(phi, m_img.width(), m_img.height(), 1));
	}

	return images;
}

QVector<QImage> ImageProcessing::schemeMCF(QImage img, int stepCount, double timeStep, double omega, double sigma, double K)
{
	QVector<QImage> images;

	if (img.isNull() || stepCount <= 0 || timeStep <= 0) {
		return images;
	}

	int width = img.width();
	int height = img.height();
	int padding = 1;

	QImage m_img = pixelsMirror(img.convertToFormat(QImage::Format_Grayscale8), padding);
	QVector<QVector<double>> phi = convertTo2Dvector(m_img);
	QVector<double> b(width * height, 0.0);	// RHS vector (b) 
	QVector<QVector<double>> phi_new = phi;

	QVector<QVector<double>> filtered_phi;
	QVector<QVector<double>> Aii(m_img.width(), QVector<double>(m_img.height(), 0.0f));
	QVector<QVector<double>> Aij_E(m_img.width(), QVector<double>(m_img.height(), 0.0f));
	QVector<QVector <double>> Aij_W(m_img.width(), QVector<double>(m_img.height(), 0.0f));
	QVector<QVector<double>> Aij_N(m_img.width(), QVector<double>(m_img.height(), 0.0f));
	QVector<QVector <double>> Aij_S(m_img.width(), QVector<double>(m_img.height(), 0.0f));
	QVector<QVector<double>> selectedPixels(3, QVector<double>(3, 0.0));
	QVector<double> gradients; // 4 directions E, N, W, S

	// SOR parameters
	const int MAX_ITER = 1000;
	const double TOL = 1.0E-7;
	/*double Aii = 1.0 + 4 * timeStep;
	double Aij = -timeStep;*/
	double g_uE = 0.0, g_uN = 0.0, g_uW = 0.0, g_uS = 0.0;
	double u_qE = 0.0, u_qN = 0.0, u_qW = 0.0, u_qS = 0.0;

	qDebug() << "Semi-Implicit Perona-Malikova method; MaxIter" << MAX_ITER << "tolerance:" << TOL;

	for (int step = 0; step < stepCount; step++)
	{
		// b with pixel values
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				b[y * width + x] = phi[x + padding][y + padding];
			}
		}

		qDebug() << "Time step : " << step;
		//Filter data with ES/IS-LHEq with tau=1 based on sigma 
		if (sigma <= 0.25) {
			qDebug() << "Data filtration with Explicit Scheme";
			filtered_phi = schemeExplicitDouble(phi, 1, timeStep);
		}
		else {
			qDebug() << "Data filtration with Implicit Scheme";
			filtered_phi = schemeImplicitDouble(phi, 1, timeStep);
		}
		qDebug() << "Data filtration done";

		double rezid = 0.0;
		int iter = 0;

		do
		{
			iter++;
			rezid = 0.0;

			for (int x = 1; x < m_img.width() - 1; x++)
			{
				for (int y = 1; y < m_img.height() - 1; y++)
				{
					// Select 3x3 pixels
					for (int i = -1; i <= 1; i++) {
						for (int j = -1; j <= 1; j++) {
							selectedPixels[i + 1][j + 1] = filtered_phi[x + i][y + j];
						}
					}
					// 4 directions E, N, W, S
					gradients = EdgeDetectorGradient3x3(selectedPixels, 1, 1);

					g_uE = 1.0 / (1.0 + (K * gradients[0])); // East
					g_uN = 1.0 / (1.0 + (K * gradients[1])); // North
					g_uW = 1.0 / (1.0 + (K * gradients[2])); // West
					g_uS = 1.0 / (1.0 + (K * gradients[3])); // South

					Aii[x][y] = 1 + timeStep * (g_uN + g_uS + g_uE + g_uW);
					Aij_E[x][y] = -timeStep * g_uE;
					Aij_W[x][y] = -timeStep * g_uW;
					Aij_N[x][y] = -timeStep * g_uN;
					Aij_S[x][y] = -timeStep * g_uS;

					// neighbors
					u_qE = phi[x + 1][y];								// right (East)
					u_qW = (x > 1 ? phi_new[x - 1][y] : phi[x - 1][y]); // left (West)
					u_qN = (y > 1 ? phi_new[x][y - 1] : phi[x][y - 1]); // top (North)
					u_qS = phi[x][y + 1];								// bottom (South)

					double sigmaSOR = Aij_E[x][y] * u_qE + Aij_N[x][y] * u_qN + Aij_W[x][y] * u_qW + Aij_S[x][y] * u_qS;
					double originalVal = phi[x][y];
					double newVal = (1.0 - omega) * originalVal + (omega / Aii[x][y]) * (b[(y - padding) * img.width() + (x - padding)] - sigmaSOR);
					phi_new[x][y] = newVal;

					rezid += (newVal - originalVal) * (newVal - originalVal);
				}
			}

			phi = phi_new;
			// mirroring: boundary conditions -> zero flux
			// edges
			for (int x = 1; x < m_img.width() - 1; x++) {
				phi[x][0] = phi[x][1];									// Top boundary
				phi[x][m_img.height() - 1] = phi[x][m_img.height() - 2];// Bottom boundary
			}
			for (int y = 1; y < m_img.height() - 1; y++) {
				phi[0][y] = phi[1][y];									// Left boundary
				phi[m_img.width() - 1][y] = phi[m_img.width() - 2][y];	// Right boundary
			}
			// corners 
			phi[0][0] = phi[1][1];																	// Top-left
			phi[m_img.width() - 1][0] = phi[m_img.width() - 2][1];									// Top-right
			phi[0][m_img.height() - 1] = phi[1][m_img.height() - 2];								// Bottom-left
			phi[m_img.width() - 1][m_img.height() - 1] = phi[m_img.width() - 2][m_img.height() - 2];// Bottom-right

			// Stopping condition - convergence
			rezid = sqrt(rezid / (width * height));
			//qDebug() << "rezid:" << rezid;

			if (rezid < TOL) {
				qDebug() << "break; iter:" << iter << "rezid:" << rezid;
				break;
			}

		} while (iter < MAX_ITER);

		// Intensity Mean
		double img_mean = computeImageMeanIntesity(phi, m_img.width(), m_img.height());
		qDebug() << "Intensity Mean:" << img_mean;

		// Convert updated new pixel values back to QImage
		//QImage currentImg = convertToQImageMirrored(pixelValues, m_img.width(), m_img.height(), 1);
		images.append(convertToQImageMirrored(phi, m_img.width(), m_img.height(), 1));
	}

	return images;
}

QVector<QImage> ImageProcessing::schemeGMCF(QImage img, int stepCount, double timeStep, double omega, double sigma, double K)
{
	QVector<QImage> images;

	if (img.isNull() || stepCount <= 0 || timeStep <= 0) {
		return images;
	}

	int width = img.width();
	int height = img.height();
	int padding = 1;

	QImage m_img = pixelsMirror(img.convertToFormat(QImage::Format_Grayscale8), padding);
	QVector<QVector<double>> phi = convertTo2Dvector(m_img);
	QVector<double> b(width * height, 0.0);	// RHS vector (b) 
	QVector<QVector<double>> phi_new = phi;

	QVector<QVector<double>> filtered_phi;
	QVector<QVector<double>> Aii(m_img.width(), QVector<double>(m_img.height(), 0.0f));
	QVector<QVector<double>> Aij_E(m_img.width(), QVector<double>(m_img.height(), 0.0f));
	QVector<QVector <double>> Aij_W(m_img.width(), QVector<double>(m_img.height(), 0.0f));
	QVector<QVector<double>> Aij_N(m_img.width(), QVector<double>(m_img.height(), 0.0f));
	QVector<QVector <double>> Aij_S(m_img.width(), QVector<double>(m_img.height(), 0.0f));
	QVector<QVector<double>> selectedPixels(3, QVector<double>(3, 0.0));
	QVector<double> gradients; // 4 directions E, N, W, S

	// SOR parameters
	const int MAX_ITER = 1000;
	const double TOL = 1.0E-7;
	/*double Aii = 1.0 + 4 * timeStep;
	double Aij = -timeStep;*/
	double g_uE = 0.0, g_uN = 0.0, g_uW = 0.0, g_uS = 0.0;
	double u_qE = 0.0, u_qN = 0.0, u_qW = 0.0, u_qS = 0.0;

	qDebug() << "Semi-Implicit Perona-Malikova method; MaxIter" << MAX_ITER << "tolerance:" << TOL;

	for (int step = 0; step < stepCount; step++)
	{
		// b with pixel values
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				b[y * width + x] = phi[x + padding][y + padding];
			}
		}

		qDebug() << "Time step : " << step;
		//Filter data with ES/IS-LHEq with tau=1 based on sigma 
		if (sigma <= 0.25) {
			qDebug() << "Data filtration with Explicit Scheme";
			filtered_phi = schemeExplicitDouble(phi, 1, timeStep);
		}
		else {
			qDebug() << "Data filtration with Implicit Scheme";
			filtered_phi = schemeImplicitDouble(phi, 1, timeStep);
		}
		qDebug() << "Data filtration done";

		double rezid = 0.0;
		int iter = 0;

		do
		{
			iter++;
			rezid = 0.0;

			for (int x = 1; x < m_img.width() - 1; x++)
			{
				for (int y = 1; y < m_img.height() - 1; y++)
				{
					// Select 3x3 pixels
					for (int i = -1; i <= 1; i++) {
						for (int j = -1; j <= 1; j++) {
							selectedPixels[i + 1][j + 1] = filtered_phi[x + i][y + j];
						}
					}
					// 4 directions E, N, W, S
					gradients = EdgeDetectorGradient3x3(selectedPixels, 1, 1);

					g_uE = 1.0 / (1.0 + (K * gradients[0])); // East
					g_uN = 1.0 / (1.0 + (K * gradients[1])); // North
					g_uW = 1.0 / (1.0 + (K * gradients[2])); // West
					g_uS = 1.0 / (1.0 + (K * gradients[3])); // South

					Aii[x][y] = 1 + timeStep * (g_uN + g_uS + g_uE + g_uW);
					Aij_E[x][y] = -timeStep * g_uE;
					Aij_W[x][y] = -timeStep * g_uW;
					Aij_N[x][y] = -timeStep * g_uN;
					Aij_S[x][y] = -timeStep * g_uS;

					// neighbors
					u_qE = phi[x + 1][y];								// right (East)
					u_qW = (x > 1 ? phi_new[x - 1][y] : phi[x - 1][y]); // left (West)
					u_qN = (y > 1 ? phi_new[x][y - 1] : phi[x][y - 1]); // top (North)
					u_qS = phi[x][y + 1];								// bottom (South)

					double sigmaSOR = Aij_E[x][y] * u_qE + Aij_N[x][y] * u_qN + Aij_W[x][y] * u_qW + Aij_S[x][y] * u_qS;
					double originalVal = phi[x][y];
					double newVal = (1.0 - omega) * originalVal + (omega / Aii[x][y]) * (b[(y - padding) * img.width() + (x - padding)] - sigmaSOR);
					phi_new[x][y] = newVal;

					rezid += (newVal - originalVal) * (newVal - originalVal);
				}
			}

			phi = phi_new;
			// mirroring: boundary conditions -> zero flux
			// edges
			for (int x = 1; x < m_img.width() - 1; x++) {
				phi[x][0] = phi[x][1];									// Top boundary
				phi[x][m_img.height() - 1] = phi[x][m_img.height() - 2];// Bottom boundary
			}
			for (int y = 1; y < m_img.height() - 1; y++) {
				phi[0][y] = phi[1][y];									// Left boundary
				phi[m_img.width() - 1][y] = phi[m_img.width() - 2][y];	// Right boundary
			}
			// corners 
			phi[0][0] = phi[1][1];																	// Top-left
			phi[m_img.width() - 1][0] = phi[m_img.width() - 2][1];									// Top-right
			phi[0][m_img.height() - 1] = phi[1][m_img.height() - 2];								// Bottom-left
			phi[m_img.width() - 1][m_img.height() - 1] = phi[m_img.width() - 2][m_img.height() - 2];// Bottom-right

			// Stopping condition - convergence
			rezid = sqrt(rezid / (width * height));
			//qDebug() << "rezid:" << rezid;

			if (rezid < TOL) {
				qDebug() << "break; iter:" << iter << "rezid:" << rezid;
				break;
			}

		} while (iter < MAX_ITER);

		// Intensity Mean
		double img_mean = computeImageMeanIntesity(phi, m_img.width(), m_img.height());
		qDebug() << "Intensity Mean:" << img_mean;

		// Convert updated new pixel values back to QImage
		//QImage currentImg = convertToQImageMirrored(pixelValues, m_img.width(), m_img.height(), 1);
		images.append(convertToQImageMirrored(phi, m_img.width(), m_img.height(), 1));
	}

	return images;
}

double ImageProcessing::computeImageMeanIntesity(QImage img)
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

	return (double)sumIntensity / (img.width() * img.height());
}

double ImageProcessing::computeImageMeanIntesity(QVector<QVector<double>> pixelsValues, int width, int height)
{
	if (pixelsValues.isEmpty()) return 0.0;

	double sumIntensity = 0.0;
	for (int x = 0; x < width; x++){
		for (int y = 0; y < height; y++){
			sumIntensity += pixelsValues[x][y]; 
		}
	}
	return (double)sumIntensity / (width * height);
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

void ImageProcessing::exportQImageToPGM(QImage img, QString filename)
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
	img.save(filePath, "PGM");
}