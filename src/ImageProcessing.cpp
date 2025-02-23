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