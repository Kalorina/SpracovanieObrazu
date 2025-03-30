#include "ImageProcessing.h"

ImageProcessing::ImageProcessing() {

}

ImageProcessing::~ImageProcessing() {

	delete[] m_pImgLocalData;
}

QVector<QVector<float>> ImageProcessing::convertTo2Dvector(QImage img)
{
	QVector<QVector<float>> pixelValues(img.width(), QVector<float>(img.height(), 0.0));

	// original image values
	for (int x = 0; x < img.width(); x++) {
		for (int y = 0; y < img.height(); y++) {
			pixelValues[x][y] = qGray(img.pixel(x, y));
		}
	}

	pixelValues_org = pixelValues;
	return pixelValues;
}

QVector<QVector<float>> ImageProcessing::convertTo2DvectorNorm(QImage img)
{
	QVector<QVector<float>> pixelValues(img.width(), QVector<float>(img.height(), 0.0));

	// original image values
	for (int x = 0; x < img.width(); x++) {
		for (int y = 0; y < img.height(); y++) {
			pixelValues[x][y] = qBound(0, qGray(img.pixel(x, y)) / 255, 1);
		}
	}

	pixelValues_org = pixelValues;
	return pixelValues;
}

QImage ImageProcessing::convertToQImage(QVector<QVector<float>> pixelValues, int width, int height)
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

QImage ImageProcessing::convertToQImageMirrored(QVector<QVector<float>> pixelValues, int width, int height, int padding)
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

void ImageProcessing::printImgData(QVector<QVector<float>> imgData, int min, int max)
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

void ImageProcessing::printImgData(QVector<QVector<float>> imgData)
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

	QVector<QVector<float>> pixelValues = convertTo2Dvector(m_img);
	QVector<QVector<float>> outputImgData(img.width(), QVector<float>(img.height(), 0.0f));

	// Convolution
	int maskRadius = convolution_mask.size() / 2;

	for (int y = 0; y < img.height(); ++y) {
		for (int x = 0; x < img.width(); ++x) {
			float newValue = 0.0f;

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

void ImageProcessing::EdgeDetectorSobelKernels(QImage img, float K)
{
	qDebug() << "Edge Detector -> gradients: Sobel kernels (filtered with IS-LHEq)";
	// Convert to grayscale if img is RGB
	img = img.convertToFormat(QImage::Format_Grayscale8);
	int padding = 1;
	QImage m_img = pixelsMirror(img, padding);
	QVector<QVector<float>> pixelValues = convertTo2Dvector(m_img); // RGB <0,255>
	// QVector<QVector<float>> pixelValues = convertTo2DvectorNorm(m_img); // Normalized <0,1>
	//printImgData(pixelValues, m_img.width() / 2 - 10, m_img.width() / 2 );

	//pixelValues = schemeExplicitFloat(pixelValues, 1, 0.2);
	pixelValues = schemeImplicitFloat(pixelValues, 1, 0.5, 1.25);
	//printImgData(pixelValues, m_img.width() / 2 - 10, m_img.width() / 2 );

	QVector<QVector<float>> outputImgData(img.width(), QVector<float>(img.height(), 0.0f));

	//float K = 1.0f; //Norm <0,10000>
	//float K = 2.5f; //RGB <0,5>

	// Sobel filter
	for (int x = 1; x < m_img.width() - 1; x++) {
		for (int y = 1; y < m_img.height() - 1; y++) {
			float grad_x = 0, grad_y = 0;

			// convolution -> Sobel Kernel 4 directions 
			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {
					double pixelVal = pixelValues[x + i][y + j];
					grad_x += Gx[i + 1][j + 1] * pixelVal;
					grad_y += Gy[i + 1][j + 1] * pixelVal;
				}
			}

			float magE = std::abs(grad_x);
			float magS = std::abs(grad_y);

			// Compute mean gradient 
			float magnitude = (magE + magS) / 2;
			//float g_value = 1 / (1 + K * std::pow(magnitude, 2));
			outputImgData[x - 1][y - 1] = qBound(0.0f, magnitude, 255.0f);
		}
	}
	//printImgData(outputImgData, m_img.width() / 2 - 10, m_img.width() / 2);
	//printImgData(outputImgData);
	qDebug() << "Edge Detector Done";
	exportQImageToPGM(convertToQImage(outputImgData,img.width(),img.height()), "edgeDetector_gradient_fromImg.pgm");
}

QImage ImageProcessing::EdgeDetectorImgSobelKernels(QImage img, float K)
{
	qDebug() << "Edge Detector -> gradients: Sobel Kernels (filtered with IS-LHEq)";
	// Convert to grayscale if img is RGB
	img = img.convertToFormat(QImage::Format_Grayscale8);
	int padding = 1;
	QImage m_img = pixelsMirror(img, padding);
	QVector<QVector<float>> pixelValues = convertTo2Dvector(m_img); // RGB <0,255>
	// QVector<QVector<float>> pixelValues = convertTo2DvectorNorm(m_img); // Normalized <0,1>
	//printImgData(pixelValues, m_img.width() / 2 - 10, m_img.width() / 2 );

	//pixelValues = schemeExplicitFloat(pixelValues, 1, 0.2);
	pixelValues = schemeImplicitFloat(pixelValues, 1, 0.5, 1.25);
	//printImgData(pixelValues, m_img.width() / 2 - 10, m_img.width() / 2 );

	QVector<QVector<float>> outputImgData(img.width(), QVector<float>(img.height(), 0.0f));

	//float K = 1.0f; //Norm <0,10000>
	//float K = 2.5f; //RGB <0,5>

	// Sobel filter
	for (int x = 1; x < m_img.width() - 1; x++) {
		for (int y = 1; y < m_img.height() - 1; y++) {
			float grad_x = 0, grad_y = 0;

			// convolution -> Sobel Kernel x and y directions 
			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {
					double pixelVal = pixelValues[x + i][y + j];
					grad_x += Gx[i + 1][j + 1] * pixelVal;
					grad_y += Gy[i + 1][j + 1] * pixelVal;
				}
			}

			float magE = std::abs(grad_x);
			float magS = std::abs(grad_y);

			// Compute mean gradient 
			float magnitude = (magE + magS) / 2;
			float g_value = 1 / (1 + K * std::pow(magnitude, 2));
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

QImage ImageProcessing::EdgeDetectorImgDirectEdges(QImage img, float K)
{
	qDebug() << "Edge Detector -> gradients: direct Edge (filtered with IS-LHEq)";
	// Convert to grayscale if img is RGB
	img = img.convertToFormat(QImage::Format_Grayscale8);
	int padding = 1;
	QImage m_img = pixelsMirror(img, padding);
	QVector<QVector<float>> pixelValues = convertTo2Dvector(m_img); // RGB <0,255>
	// QVector<QVector<float>> pixelValues = convertTo2DvectorNorm(m_img); // Normalized <0,1>
	//printImgData(pixelValues, m_img.width() / 2 - 10, m_img.width() / 2 );

	//pixelValues = schemeExplicitFloat(pixelValues, 1, 0.2);
	pixelValues = schemeImplicitFloat(pixelValues, 1, 0.5, 1.25);
	//printImgData(pixelValues, m_img.width() / 2 - 10, m_img.width() / 2 );

	QVector<QVector<float>> outputImgData(img.width(), QVector<float>(img.height(), 0.0f));

	//float K = 1.0f; //Norm <0,10000>
	//float K = 0.005f; //RGB <0,5>
	float h = 1.0f;

	// Sobel filter
	for (int x = 1; x < m_img.width() - 1; x++) {
		for (int y = 1; y < m_img.height() - 1; y++) {
			float gradX = 0, gradY = 0;
			float g_E = 0, g_N = 0, g_W = 0, g_S = 0;
			float normSq_qE = 0, normSq_qN = 0, normSq_qW = 0, normSq_qS = 0;

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
			float magnitude = (normSq_qE + normSq_qN + normSq_qW + normSq_qS) / 4;
			float g_value = diffCoefFunction(K, magnitude);
			outputImgData[x - 1][y - 1] = qBound(0.0f, g_value * 255.0, 255.0f);
		}
	}
	//printImgData(outputImgData, m_img.width() / 2 - 10, m_img.width() / 2);
	//printImgData(outputImgData);
	qDebug() << "Edge Detector 2 Done";
	exportQImageToPGM(convertToQImage(outputImgData, img.width(), img.height()), "edgeDetector_DirectEdge.pgm");
	QImage img_edgeD = convertToQImage(outputImgData, img.width(), img.height());
	return img_edgeD;
}

QVector<float> ImageProcessing::EdgeDetectorGradient3x3(QVector<QVector<float>> imgData, int x, int y)
{
	// imgData -> 3x3 matrix in 2D vector

	if (imgData.size() != 3 || imgData[0].size() != 3) {
		return QVector<float>(4, 0.0f);
	}

	QVector<float> gradients(4, 0.0f); // 4 directions E, N, W, S
	
	float gradX = 0, gradY = 0;
	double h = 0.0;

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
	gradX = (imgData[x + 1][y] - imgData[x][y]) / h;
	gradY = (imgData[x + 1][y - 1] + imgData[x][y - 1] - imgData[x][y + 1] - imgData[x + 1][y + 1]) / (4.0 * h);

	float normSq_qE = gradX * gradX + gradY * gradY;
	//g_E = diffCoefFunction(K, normSq);

	//----- NORTH edge -----//
	// y -> (N - p) / h
	// x -> (W + NW - E - NE) / 4h 
	gradX = (imgData[x - 1][y] + imgData[x - 1][y - 1] - imgData[x + 1][y] - imgData[x + 1][y - 1]) / (4.0 * h);
	gradY = (imgData[x][y - 1] - imgData[x][y]) / h;

	float normSq_qN = gradX * gradX + gradY * gradY;
	//g_N = diffCoefFunction(K, normSq);

	//----- WEST edge -----//
	// x -> (p - W) / h
	// y -> (S + SW - N - NW) / 4h
	gradX = (imgData[x - 1][y] - imgData[x][y]) / h;
	gradY = (imgData[x][y + 1] + imgData[x - 1][y + 1] - imgData[x][y - 1] - imgData[x - 1][y - 1]) / (4.0 * h);

	float normSq_qW = gradX * gradX + gradY * gradY;
	//g_W = diffCoefFunction(K, normSq);

	//----- SOUTH edge -----//
	// y -> (S - p) / h
	// x -> (E + SE - W - SW) / 4h
	gradX = (imgData[x + 1][y] + imgData[x + 1][y + 1] - imgData[x - 1][y] - imgData[x - 1][y + 1]) / (4.0 * h);
	gradY = (imgData[x][y + 1] - imgData[x][y]) / h;

	float normSq_qS = gradX * gradX + gradY * gradY;
	//g_S = diffCoefFunction(K, normSq);

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
	QVector<QVector<float>> pixelValues = convertTo2Dvector(m_img);
	QVector<QVector<float>> newPixelValues = pixelValues;

	for (int step = 0; step < stepCount; step++)
	{
		for (int x = 1; x < m_img.width() - 1; x++) {
			for (int y = 1; y < m_img.height() - 1; y++) {
				float u_p_n = pixelValues[x][y];

				// Mirrored neighbors
				float u_q1 = pixelValues[x + 1][y];
				float u_q2 = pixelValues[x - 1][y];
				float u_q3 = pixelValues[x][y + 1];
				float u_q4 = pixelValues[x][y - 1];

				newPixelValues[x][y] = (1 - 4 * timeStep) * u_p_n 
					+ timeStep * (u_q1 + u_q2 + u_q3 + u_q4);
			}
		}
		// all new pixel values updated /w mirrored pixels 
		pixelValues = newPixelValues;

		// boundary is updated so that the flux on boundary stays the same 
		// padding = 1;
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

		float currentMean = computeImageMeanIntesity(pixelValues, m_img.width(), m_img.height());
		qDebug() << "Intensity Mean:" << currentMean;

		// currentImg = width x height -> without mirroring
		// QImage currentImg(img.width(), img.height(), QImage::Format_Grayscale8);
		// the pixel values back to an image
		QImage currentImg = convertToQImageMirrored(pixelValues, m_img.width(), m_img.height(), padding);
		images.append(currentImg);
	}
	return images;
}

QVector<QVector<float>> ImageProcessing::schemeExplicitFloat(QVector<QVector<float>> imgData, int stepCount, double timeStep)
{
	int width = imgData.size();
	if (width == 0) return QVector<QVector<float>>();
	int height = imgData[0].size();

	QVector<QVector<float>> outputImgData(width, QVector<float>(height, 0.0));

	for (int step = 0; step < stepCount; step++)
	{
		// pixels values 2D vecotr = width+2 x height+2 -> with mirroring padding:1
		QVector<QVector<float>> newPixelValues = imgData;

		for (int x = 1; x < width - 1; x++) {
			for (int y = 1; y < height - 1; y++) {
				float u_p_n = imgData[x][y];

				// Mirrored neighbors
				float u_q1 = imgData[x + 1][y];
				float u_q2 = imgData[x - 1][y];
				float u_q3 = imgData[x][y + 1];
				float u_q4 = imgData[x][y - 1];

				newPixelValues[x][y] = (1 - 4 * timeStep) * u_p_n + timeStep * (u_q1 + u_q2 + u_q3 + u_q4);
			}
		}
		// all new pixel values updated /w mirrored pixels 
		outputImgData = newPixelValues;

		// boundary is updated so that the flux on boundary stays the same 
		// padding = 1;
		// edges
		for (int x = 1; x < width - 1; x++) {
			outputImgData[x][0] = outputImgData[x][1];						// Top boundary
			outputImgData[x][height - 1] = outputImgData[x][height - 2];	// Bottom boundary
		}
		for (int y = 1; y < height - 1; y++) {
			outputImgData[0][y] = outputImgData[1][y];						// Left boundary
			outputImgData[width - 1][y] = outputImgData[width - 2][y];		// Right boundary
		}

		// corners 
		outputImgData[0][0] = outputImgData[1][1];									// Top-left
		outputImgData[width - 1][0] = outputImgData[width - 2][1];					// Top-right
		outputImgData[0][height - 1] = outputImgData[1][height - 2];				// Bottom-left
		outputImgData[width - 1][height - 1] = outputImgData[width - 2][height - 2];// Bottom-right

		//float currentMean = computeImageMeanIntesity(outputImgData, width, height);
		//qDebug() << " ES Intensity Mean:" << currentMean;
	}
	return outputImgData;
}

QVector<QImage> ImageProcessing::schemeImplicit(QImage img, int stepCount, double timeStep, double omega)
{
	QVector<QImage> images;

	if (img.isNull() || stepCount <= 0 || timeStep <= 0) {
		return images;
	}

	int width = img.width();
	int height = img.height();
	int padding = 1;

	QImage m_img = pixelsMirror(img.convertToFormat(QImage::Format_Grayscale8), padding);
	QVector<QVector<float>> pixelValues = convertTo2Dvector(m_img);
	QVector<float> b(width * height, 0.0);	// RHS vector (b) 

	// Initialize b with original values
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			b[y * width + x] = pixelValues[x + padding][y + padding];
		}
	}

	// SOR parameters
	// A * phi = b
	// Aii = 1 + 4 * timeStep
	// Aij = -timeStep
	//double omega = 1.25;
	const int MAX_ITER = 1000;
	const double TOL = 1.0E-6;
	double Aii = 1.0 + 4 * timeStep;
	double Aij = -timeStep;

	qDebug() << "Linear Heat Eq Implicit scheme SOR method MaxIter" << MAX_ITER << " omega:" << omega << "tolerance:" << TOL;

	for (int t = 0; t < stepCount; t++)
	{
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
					float neighborSum =
						pixelValues[x - 1][y] +   // left (West)
						pixelValues[x + 1][y] +   // right (East)
						pixelValues[x][y - 1] +   // top (North)
						pixelValues[x][y + 1];    // bottom (South)

					// Sum of neighbors
					double sigmaSOR = Aij * neighborSum;
					float originalVal = pixelValues[x][y];
					float newVal = (1.0 - omega) * originalVal + (omega / Aii) * (b[(y - padding) * width + (x - padding)] - sigmaSOR);

					rezid += (newVal - originalVal) * (newVal - originalVal);
					pixelValues[x][y] = newVal;
				}
			}

			// Stopping condition - convergence
			rezid = sqrt(rezid / (width * height));
			//qDebug() << "rezid:" << rezid;
			if (rezid < TOL) {
				qDebug() << "break; iter:" << iter << "rezid:" << rezid;
				break;
			}
				
		} while (iter < MAX_ITER);

		// mirroring: boundary conditions -> zero flux
		// edges
		for (int x = 1; x < m_img.width() - 1; x++) {
			pixelValues[x][0] = pixelValues[x][1];									// Top boundary
			pixelValues[x][m_img.height() - 1] = pixelValues[x][m_img.height() - 2];// Bottom boundary
		}
		for (int y = 1; y < m_img.height() - 1; y++) {
			pixelValues[0][y] = pixelValues[1][y];									// Left boundary
			pixelValues[m_img.width() - 1][y] = pixelValues[m_img.width() - 2][y];	// Right boundary
		}
		// corners 
		pixelValues[0][0] = pixelValues[1][1];																	// Top-left
		pixelValues[m_img.width() - 1][0] = pixelValues[m_img.width() - 2][1];									// Top-right
		pixelValues[0][m_img.height() - 1] = pixelValues[1][m_img.height() - 2];								// Bottom-left
		pixelValues[m_img.width() - 1][m_img.height() - 1] = pixelValues[m_img.width() - 2][m_img.height() - 2];// Bottom-right

		// Intensity Mean
		float img_mean = computeImageMeanIntesity(pixelValues, m_img.width(), m_img.height());
		qDebug() << "Intensity Mean:" << img_mean;

		// Convert updated new pixel values back to QImage
		//QImage currentImg = convertToQImageMirrored(pixelValues, m_img.width(), m_img.height(), 1);
		images.append(convertToQImageMirrored(pixelValues, m_img.width(), m_img.height(), 1));
	}

	return images;
}

QVector<QVector<float>> ImageProcessing::schemeImplicitFloat(QVector<QVector<float>> imgData, int stepCount, double timeStep, double omega)
{
	int padding = 1;

	int width_mirrored = imgData.size();
	if (width_mirrored == 0) return QVector<QVector<float>>();
	int width_org = imgData.size() - 2 * padding;
	int height_org = imgData[0].size() - 2 * padding;
	int height_mirrored = imgData[0].size();

	QVector<float> b(width_org * height_org, 0.0);	// RHS vector (b) 

	// Initialize b with original values
	for (int y = 0; y < height_org; y++) {
		for (int x = 0; x < width_org; x++) {
			b[y * width_org + x] = imgData[x + padding][y + padding];
		}
	}

	// SOR parameters
	// A * phi = b
	// Aii = 1 + 4 * timeStep
	// Aij = -timeStep
	//double omega = 1.25;
	const int MAX_ITER = 1000;
	const double TOL = 1.0E-6;
	double Aii = 1.0 + 4 * timeStep;
	double Aij = -timeStep;

	//qDebug() << "Linear Heat Eq Implicit scheme SOR method";

	for (int t = 0; t < stepCount; t++)
	{
		double rezid = 0.0;
		int iter = 0;

		do
		{
			iter++;
			rezid = 0.0;

			for (int x = padding; x < width_org + padding; x++)
			{
				for (int y = padding; y < height_org + padding; y++)
				{
					float neighborSum =
						imgData[x - 1][y] +   // left (West)
						imgData[x + 1][y] +   // right (East)
						imgData[x][y - 1] +   // top (North)
						imgData[x][y + 1];    // bottom (South)

					// Sum of neighbors
					double sigmaSOR = Aij * neighborSum;
					float originalVal = imgData[x][y];
					float newVal = (1.0 - omega) * originalVal + (omega / Aii) * (b[(y - padding) * width_org + (x - padding)] - sigmaSOR);

					rezid += (newVal - originalVal) * (newVal - originalVal);
					imgData[x][y] = newVal;
				}
			}

			// Stopping condition - convergence
			rezid = sqrt(rezid / (width_org * height_org));
			//qDebug() << "rezid:" << rezid;
			if (rezid < TOL) {
				//qDebug() << "break; rezid:" << rezid;
				break;
			}

		} while (iter < MAX_ITER);

		// mirroring: boundary conditions -> zero flux
		// edges
		for (int x = 1; x < width_mirrored - 1; x++) {
			imgData[x][0] = imgData[x][1];										// Top boundary
			imgData[x][height_mirrored - 1] = imgData[x][height_mirrored - 2];	// Bottom boundary
		}
		for (int y = 1; y < height_mirrored - 1; y++) {
			imgData[0][y] = imgData[1][y];										// Left boundary
			imgData[width_mirrored - 1][y] = imgData[width_mirrored - 2][y];	// Right boundary
		}
		// corners 
		imgData[0][0] = imgData[1][1];																		// Top-left
		imgData[width_mirrored - 1][0] = imgData[width_mirrored - 2][1];									// Top-right
		imgData[0][height_mirrored - 1] = imgData[1][height_mirrored - 2];									// Bottom-left
		imgData[width_mirrored - 1][height_mirrored - 1] = imgData[width_mirrored - 2][height_mirrored - 2];// Bottom-right

		// Intensity Mean
		//float img_mean = computeImageMeanIntesity(imgData, m_img.width(), m_img.height());
		//qDebug() << "Intensity Mean:" << img_mean;
	}

	return imgData;
}

QVector<QImage> ImageProcessing::schemeSemi_Implicit(QImage img, int stepCount, double timeStep, double omega, double K)
{
	qDebug() << "Semi-Implicit Diffusion Perona-Malikova method";
	QVector<QImage> images;

	if (img.isNull() || stepCount <= 0 || timeStep <= 0 || omega <= 0 || K <=0) {
		return images;
	}
	// Convert to grayscale if img is RGB
	img = img.convertToFormat(QImage::Format_Grayscale8);
	int padding = 1;
	QImage m_img = pixelsMirror(img, padding);

	// Convert to 2D vector
	QVector<QVector<float>> pixelValues = convertTo2Dvector(m_img);

	QVector<float> b(img.width() * img.height(), 0.0);	// RHS vector (b) 
	// Initialize b with original values
	for (int y = 0; y < img.height(); y++) {
		for (int x = 0; x < img.width(); x++) {
			b[y * img.width() + x] = pixelValues[x + padding][y + padding];
		}
	}

	// system matrix coefficients
	double Aii = 0.0; // = 1 + tau * (g_N + g_S + g_E + g_W)
	double Aij_N = 0.0; // = - tau * g_N
	double Aij_S = 0.0; // = - tau * g_S
	double Aij_E = 0.0; // = - tau * g_E
	double Aij_W = 0.0; // = - tau * g_W
	double neighborSum = 0.0;
	double sigmaSOR = 0.0;
	const int MAX_ITER = 1000;
	const double TOL = 1.0E-6;

	// Perona-Malikova Diffusion
	for (int step = 0; step < stepCount; step++)
	{
		//Filter data with ES/IS-LHEq due to tau=timeStep
		int insideStepCount = 1;
		if (timeStep <= 0.25){ 
			pixelValues = schemeExplicitFloat(pixelValues, insideStepCount, timeStep);
		}
		else{ 
			pixelValues = schemeImplicitFloat(pixelValues, insideStepCount, timeStep, 1.25);
		}

		double rezid = 0.0;
		int iter = 0;

		do
		{
			iter++;
			rezid = 0.0;

			for (int x = 1; x < m_img.width() - 1; x++) {
				for (int y = 1; y < m_img.height() - 1; y++) {

					// Select 3x3 pixels
					QVector<QVector<float>> selectedPixels = pixelSelection3x3(pixelValues, x, y); // 3x3

					// Compute gradient of pixels inside 3x3 window, center pixel is at (1,1)
					QVector<float> gradients = EdgeDetectorGradient3x3(selectedPixels, 1, 1); // 3x3

					float u_p = pixelValues[x][y]; // actual center pixel value

					// neighbors
					float u_qE = pixelValues[x + 1][y];
					float u_qW = pixelValues[x - 1][y];
					float u_qN = pixelValues[x][y + 1];
					float u_qS = pixelValues[x][y - 1];

					/*
					gradients.append(normSq_qE);
					gradients.append(normSq_qN);
					gradients.append(normSq_qW);
					gradients.append(normSq_qS);
					*/

					float g_uE = 1 / (1 + K * gradients[0]);	// East direction
					float g_uN = 1 / (1 + K * gradients[1]);	// North direction
					float g_uW = 1 / (1 + K * gradients[2]);	// West direction
					float g_uS = 1 / (1 + K * gradients[3]);	// South direction
					
					//float newVal = u_p + timeStep * (
					//	g_uE * (u_qE - u_p) +		// East direction 
					//	g_uN * (u_qN - u_p) +		// North direction 
					//	g_uW * (u_qW - u_p) +		// West direction 
					//	g_uS * (u_qS - u_p)			// South direction 
					//	);

					// Perona-Malikova Diffusion 
					// Implicit
					Aii = 1 + timeStep * (g_uN + g_uS + g_uE + g_uW);
					Aij_N = -timeStep * g_uN;
					Aij_S = -timeStep * g_uS;
					Aij_E = -timeStep * g_uE;
					Aij_W = -timeStep * g_uW;

					float originalVal = pixelValues[x][y];
					sigmaSOR = Aij_E * u_qE + Aij_N * u_qN + Aij_W * u_qW + Aij_S * u_qS;
					float newVal = (1.0 - omega) * originalVal + (omega / Aii) * (b[(y - padding) * img.width() + (x - padding)] - sigmaSOR);
					
					rezid += (newVal - originalVal) * (newVal - originalVal);
					
					pixelValues[x][y] = newVal;
				}
			}

			// Stopping condition - convergence
			rezid = sqrt(rezid / (img.width() * img.height()));
			//qDebug() << "rezid:" << rezid;
			if (rezid < TOL) {
				qDebug() << "break; iter:" << iter << "rezid:" << rezid;
				break;
			}

		} while (iter < MAX_ITER);

		// mirroring: boundary conditions -> zero flux
		// edges
		for (int x = 1; x < m_img.width() - 1; x++) {
			pixelValues[x][0] = pixelValues[x][1];										// Top boundary
			pixelValues[x][m_img.height() - 1] = pixelValues[x][m_img.height() - 2];	// Bottom boundary
		}
		for (int y = 1; y < m_img.height() - 1; y++) {
			pixelValues[0][y] = pixelValues[1][y];										// Left boundary
			pixelValues[m_img.width() - 1][y] = pixelValues[m_img.width() - 2][y];		// Right boundary
		}

		// corners 
		pixelValues[0][0] = pixelValues[1][1];																	// Top-left
		pixelValues[m_img.width() - 1][0] = pixelValues[m_img.width() - 2][1];									// Top-right
		pixelValues[0][m_img.height() - 1] = pixelValues[1][m_img.height() - 2];								// Bottom-left
		pixelValues[m_img.width() - 1][m_img.height() - 1] = pixelValues[m_img.width() - 2][m_img.height() - 2];// Bottom-right

		// Intensity Mean	
		float img_mean = computeImageMeanIntesity(pixelValues, m_img.width(), m_img.height());
		qDebug() << "Intensity Mean:" << img_mean;

		// Convert updated new pixel values back to QImage
		QImage currentImg = convertToQImageMirrored(pixelValues, m_img.width(), m_img.height(), padding);
		images.append(currentImg);
	}

	return images;
}

QVector<QVector<float>> ImageProcessing::pixelSelection3x3(QVector<QVector<float>> imgData, int x, int y)
{
	if (imgData.isEmpty()) return QVector<QVector<float>>();
	QVector<QVector<float>> selectedPixels(3, QVector<float>(3, 0.0f));

	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			selectedPixels[i+1][i+1] = imgData[x + i][y + j];
		}
	}
	//qDebug() << selectedPixels;
	return selectedPixels;
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

float ImageProcessing::computeImageMeanIntesity(QVector<QVector<float>> pixelsValues, int width, int height)
{
	if (pixelsValues.isEmpty()) return 0.0;

	float sumIntensity = 0.0;
	for (int x = 0; x < width; x++){
		for (int y = 0; y < height; y++){
			sumIntensity += pixelsValues[x][y]; 
		}
	}
	return (float)sumIntensity / (width * height);
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