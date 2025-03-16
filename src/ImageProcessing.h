// Image Processing Class pre operatice s Img
// Flow: ImageViewer -> ImageProcessing -> ImageViewer -> ViewerWidget

// Mirror functio pre Img
/*
-> data do lokalnej premennej (kopia) a ako type double range 0 - 1->standart, da sa ale aj range 0 - 255

1. operacia na konvoluciu
-> 10x10 pix
->pre rohove oblasti mirror values->aby mal vsetky horndoty od smerov hore, dole, sprava, zlava
->zvacsime na 12x12, nastavi hodnoty a vyreze obr 10x10->toto sa bude robit VZDY dobre nastavit
->vola sa to zrkadlenie a odzrkadlenie
->zvacsenie->a symetricky zrkadlenie 4 smermi->nastavuju sa toky rohov aby boli nulove
->zacat zhora->vytvorit si obdlznik a mirroring
*/

#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets>
#include <string>
#include <stdio.h>
#include <stdlib.h>

//shortcut 
typedef unsigned char uchar;
typedef unsigned int uint;

class ImageProcessing {

public:
	// Constructor + Destructor
	ImageProcessing();
	~ImageProcessing();

	// #################### Get functions ###################

	double* getImgData() { return m_pImgLocalData; }
	uint getImgWidth() { return m_imgWidth; }
	uint getImgHeight() { return m_imgHeight; }

	// #################### Functions ###################

	//bool pixelsMirror(uchar* originalImgData, const int bytesPerLine, const int imgWidth, const int imgHeight, const int padding);
	//bool pixelsMirror(double* originalImgData, const int bytesPerLine, const int imgWidth, const int imgHeight, const int padding);
	//uchar* pixelsUnmirror(int padding);

	QImage pixelsMirror(QImage img, int padding);
	QImage pixelsUnmirror(QImage img, int padding);
	QImage Convolution(QImage img, int padding);

	// Heat Equation 

	QVector<QImage> schemeExplicit(QImage img, int stepCount, double timeStep);
	QVector<QImage> schemeExplicitFloat(QImage img, int stepCount, double timeStep);

	QVector<QImage> schemeImplicit(QImage img, int stepCount, double timeStep);
	float computeImageMeanIntesity(QImage img);

	QVector<int> computeHistogram(QImage img);
	QImage FSHS(QImage img); // Full Scale Histogram Strech
	QImage EH(QImage img); // Equalization of Histogram 

	// #################### Export ###################

	bool exportToPGM(QImage img, const QString& filename);

private: 
	// #################### Variables ###################

	double* m_pImgLocalData = nullptr;
	uint m_imgWidth = 0;
	uint m_imgHeight = 0;

	// Convolution mask
	QVector<QVector<double>> convolution_mask = {
		{0.000077693991227,   0.001813519368126,   0.005031312077870,   0.001813519368126,   0.000077693991227},
		{0.001813519368126,   0.042330847554975,   0.117439994473243,   0.042330847554975,   0.001813519368126},
		{0.005031312077870,   0.117439994473243,   0.325972452665734,   0.117439994473243,   0.005031312077870},
		{0.001813519368126,   0.042330847554975,   0.117439994473243,   0.042330847554975,   0.001813519368126},
		{0.000077693991227,   0.001813519368126,   0.005031312077870,   0.001813519368126,   0.000077693991227}
	};

};