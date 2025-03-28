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

void ImageProcessing::EdgeDetector(QImage img)
{
	qDebug() << "Edge Detector -> Gradient of pixels ( filtered data IS-LHEq)";
	// Convert to grayscale if img is RGB
	img = img.convertToFormat(QImage::Format_Grayscale8);
	int padding = 1;
	QImage m_img = pixelsMirror(img, padding);
	QVector<QVector<float>> pixelValues = convertTo2Dvector(m_img); // RGB <0,255>
	// QVector<QVector<float>> pixelValues = convertTo2DvectorNorm(m_img); // Normalized <0,1>
	//printImgData(pixelValues, m_img.width() / 2 - 10, m_img.width() / 2 );

	//pixelValues = schemeExplicitFloat(pixelValues, 1, 0.2);
	pixelValues = schemeImplicitFloat(pixelValues, 1, 0.5);
	//printImgData(pixelValues, m_img.width() / 2 - 10, m_img.width() / 2 );

	QVector<QVector<float>> outputImgData(img.width(), QVector<float>(img.height(), 0.0f));

	//float K = 1.0f; //Norm <0,10000>
	float K = 2.5f; //RGB <0,5>

	// Sobel filter
	for (int x = 1; x < m_img.width() - 1; x++) {
		for (int y = 1; y < m_img.height() - 1; y++) {
			float gradE = 0, gradS = 0, gradD1 = 0, gradD2 = 0;

			// convolution -> Sobel Kernel 4 directions 
			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {
					double pixelVal = pixelValues[x + i][y + j];
					gradE += Gx[i + 1][j + 1] * pixelVal;
					gradS += Gy[i + 1][j + 1] * pixelVal;
					gradD1 += Gd1[i + 1][j + 1] * pixelVal;
					gradD2 += Gd2[i + 1][j + 1] * pixelVal;
				}
			}
			float magE = std::abs(gradE);
			float magS = std::abs(gradS);
			float magD1 = std::abs(gradD1);
			float magD2 = std::abs(gradD2);

			// Compute mean gradient 
			float magnitude = (magE + magS + magD1 + magD2) / 4;
			// normalized magnitude <0,1>
			//double g_value = 1.0 / (1.0 + K * std::pow(magnitude, 2));
			outputImgData[x - 1][y - 1] = qBound(0.0f, (float)magnitude, 255.0f);
		}
	}
	//printImgData(outputImgData, m_img.width() / 2 - 10, m_img.width() / 2);
	//printImgData(outputImgData);
	qDebug() << "Edge Detector Done";
	//exportQImageToPGM(convertToQImage(outputImgData,img.width(),img.height()), "edgeDetector_gradient_fromImg.pgm");
	exportToPGM(outputImgData, "edgeDetector_gradient_fromData.pgm");
}

QImage ImageProcessing::EdgeDetectorImg(QImage img)
{
	qDebug() << "Edge Detector -> Gradient of pixels ( filtered data IS-LHEq)";
	// Convert to grayscale if img is RGB
	img = img.convertToFormat(QImage::Format_Grayscale8);
	int padding = 1;
	QImage m_img = pixelsMirror(img, padding);
	QVector<QVector<float>> pixelValues = convertTo2Dvector(m_img); // RGB <0,255>
	// QVector<QVector<float>> pixelValues = convertTo2DvectorNorm(m_img); // Normalized <0,1>
	//printImgData(pixelValues, m_img.width() / 2 - 10, m_img.width() / 2 );

	//pixelValues = schemeExplicitFloat(pixelValues, 1, 0.2);
	pixelValues = schemeImplicitFloat(pixelValues, 1, 0.5);
	//printImgData(pixelValues, m_img.width() / 2 - 10, m_img.width() / 2 );

	QVector<QVector<float>> outputImgData(img.width(), QVector<float>(img.height(), 0.0f));

	//float K = 1.0f; //Norm <0,10000>
	float K = 2.5f; //RGB <0,5>

	// Sobel filter
	for (int x = 1; x < m_img.width() - 1; x++) {
		for (int y = 1; y < m_img.height() - 1; y++) {
			float gradE = 0, gradS = 0, gradD1 = 0, gradD2 = 0;

			// convolution -> Sobel Kernel 4 directions 
			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {
					double pixelVal = pixelValues[x + i][y + j];
					gradE += Gx[i + 1][j + 1] * pixelVal;
					gradS += Gy[i + 1][j + 1] * pixelVal;
					gradD1 += Gd1[i + 1][j + 1] * pixelVal;
					gradD2 += Gd2[i + 1][j + 1] * pixelVal;
				}
			}
			float magE = std::abs(gradE);
			float magS = std::abs(gradS);
			float magD1 = std::abs(gradD1);
			float magD2 = std::abs(gradD2);

			// Compute mean gradient 
			float magnitude = (magE + magS + magD1 + magD2) / 4;
			// normalized magnitude <0,1>
			//double g_value = 1.0 / (1.0 + K * std::pow(magnitude, 2));
			outputImgData[x - 1][y - 1] = qBound(0.0f, (float)magnitude, 255.0f);
		}
	}
	//printImgData(outputImgData, m_img.width() / 2 - 10, m_img.width() / 2);
	//printImgData(outputImgData);
	qDebug() << "Edge Detector Done";
	//exportQImageToPGM(convertToQImage(outputImgData,img.width(),img.height()), "edgeDetector_gradient_fromImg.pgm");
	QImage img_edgeD = convertToQImage(outputImgData,img.width(), img.height());
	exportToPGM(outputImgData, "edgeDetector_gradient_fromData.pgm");
	return img_edgeD;
}

QVector<QImage> ImageProcessing::schemeExplicitFloat(QImage img, int stepCount, double timeStep)
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
	double omega = 1.25;
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

			for (int x = padding; x < width + padding; x++)
			{
				for (int y = padding; y < height + padding; y++)
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
				//qDebug() << "break; rezid:" << rezid;
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
		QImage currentImg = convertToQImageMirrored(pixelValues, m_img.width(), m_img.height(), 1);
		images.append(currentImg);
	}

	return images;
}

QVector<QVector<float>> ImageProcessing::schemeImplicitFloat(QVector<QVector<float>> imgData, int stepCount, double timeStep)
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
	double omega = 1.25;
	const int MAX_ITER = 1000;
	const double TOL = 1.0E-6;
	double Aii = 1.0 + 4 * timeStep;
	double Aij = -timeStep;

	qDebug() << "Linear Heat Eq Implicit scheme SOR method";

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

void ImageProcessing::exportToPGM(QVector<QVector<float>> imageData, QString filename) {

	QString directory = "C:/Users/karol/Documents/UNI_SCHOOL/STU/Inzinier/Semester_8/SpracovanieObrazu/ImageViewerSO/data";
	QDir dir(directory);
	if (!dir.exists()) {
		dir.mkpath(directory);
	}

	// Construct the full file path
	QString filePath = dir.filePath(filename);

	int width = imageData.size();
	if (width == 0) return;
	int height = imageData[0].size();

	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly)) {
		qDebug() << "Failed to open file for writing:" << filename;
		return;
	}

	QTextStream out(&file);

	// PGM header (P2 format for ASCII)
	out << "P2\n" << width << " " << height << "\n255\n";

	// Find min & max values to normalize properly
	float minVal = std::numeric_limits<float>::max();
	float maxVal = std::numeric_limits<float>::lowest();

	for (const auto& row : imageData) {
		for (float val : row) {
			minVal = qMin(minVal, val);
			maxVal = qMax(maxVal, val);
		}
	}

	// Avoid division by zero
	float range = maxVal - minVal;
	if (range == 0) range = 1;

	// Write pixel values (normalized to 0-255)
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			// int pixelValue = static_cast<int>(255 * (imageData[x][y] - minVal) / range);
			out << imageData[x][y] << " ";
		}
		out << "\n"; // New line after each row
	}

	file.close();
}