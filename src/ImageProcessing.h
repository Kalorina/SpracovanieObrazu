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
#include <cmath>

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

	QVector<QVector<double>> convertTo2Dvector(QImage img);
	QVector<QVector<double>> convertTo2DvectorNorm(QImage img);
	QImage convertToQImage(QVector<QVector<double>> pixelValues, int width, int height);
	QImage convertToQImageMirrored(QVector<QVector<double>> pixelValues, int width, int height, int padding);

	QImage pixelsMirror(QImage img, int padding);
	QImage pixelsUnmirror(QImage img, int padding);
	void printImgData(QVector<QVector<double>> imgData, int min, int max);
	void printImgData(QVector<QVector<double>> imgData);
	
	//Convolution
	QImage Convolution(QImage img, int padding);
	
	// Edge Detector
	void EdgeDetectorSobelKernels(QImage img, double K);		// export to PGM
	QImage EdgeDetectorImgSobelKernels(QImage img, double K); // with Sobel kernels masks 4 direction: x,y,xy,-xy
	QImage EdgeDetectorImgDirectEdges(QImage img, double K); // with each edge separately
	QVector<double> EdgeDetectorGradient3x3(QVector<QVector<double>> imgData, int x, int y); // Edge Detector for Semi-implicit scheme
	QVector<double> EdgeDetectorGradient3x3Epsylon(QVector<QVector<double>> imgData, int x, int y, double epsylon); // Edge Detector for Semi-implicit scheme

	double diffCoefFunction(double K, double normGradSquared) { return (1.0 / (1.0 + (K * normGradSquared))); }

	// Heat Equation 
	QVector<QImage> schemeExplicit(QImage img, int stepCount, double timeStep);
	QVector<QVector<double>> schemeExplicitDouble(QVector<QVector<double>> imgData, int stepCount, double timeStep);

	QVector<QImage> schemeImplicit(QImage img, int stepCount, double timeStep);
	QVector<QVector<double>> schemeImplicitDouble(QVector<QVector<double>> imgData, int stepCount, double timeStep);
	
	// Linear Diffusion
	QVector<QImage> schemeSemi_Implicit(QImage img, int stepCount, double timeStep, double omega, double sigma, double K);

	QVector<QImage> schemeMCF(QImage img, int stepCount, double timeStep, double omega, double sigma, double K);
	QVector<QImage> schemeGMCF(QImage img, int stepCount, double timeStep, double omega, double sigma, double K);

	// Eikonal Equation Distance
	QVector<QVector<double>> computeEdgePixels(QImage img);
	QImage computeEikonalDistance(QImage img, int maxIter);

	double computeImageMeanIntesity(QImage img);
	double computeImageMeanIntesity(QVector<QVector<double>> pixelsValues, int width, int height);

	// Histogram
	QVector<int> computeHistogram(QImage img);
	QImage FSHS(QImage img); // Full Scale Histogram Strech
	QImage EH(QImage img); // Equalization of Histogram 

	// #################### Export ###################

	void exportQImageToPGM(QImage img, QString filename);

private: 
	// #################### Variables ###################

	double* m_pImgLocalData = nullptr;
	uint m_imgWidth = 0;
	uint m_imgHeight = 0;

	QVector<QVector<double>> pixelValues_org;
	// Convolution mask
	QVector<QVector<double>> convolution_mask = {
		{0.000077693991227,   0.001813519368126,   0.005031312077870,   0.001813519368126,   0.000077693991227},
		{0.001813519368126,   0.042330847554975,   0.117439994473243,   0.042330847554975,   0.001813519368126},
		{0.005031312077870,   0.117439994473243,   0.325972452665734,   0.117439994473243,   0.005031312077870},
		{0.001813519368126,   0.042330847554975,   0.117439994473243,   0.042330847554975,   0.001813519368126},
		{0.000077693991227,   0.001813519368126,   0.005031312077870,   0.001813519368126,   0.000077693991227}
	};
	// Gradient masks - Sobel kernels
	QVector<QVector<int>> Gx = 
	{	{-1, 0, 1}, 
		{-2, 0, 2},
		{-1, 0, 1} };
	QVector<QVector<int>> Gy = 
	{	{1, 2, 1}, 
		{0, 0, 0}, 
		{-1, -2, -1} };
};