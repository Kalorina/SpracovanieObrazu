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

	// Returns pointer to memory where the image data are stored.
	double* getImgData() { return m_pImgLocalData; }

	// Get width of the locally stored image.
	uint getImgWidth() { return m_imgWidth; }

	// Get height of the locally stored image.
	uint getImgHeight() { return m_imgHeight; }

	// #################### Functions ###################

	bool pixelsMirror(uchar* originalImgData, const int bytesPerLine, const int imgWidth, const int imgHeight, const int padding);
	bool pixelsMirror(double* originalImgData, const int bytesPerLine, const int imgWidth, const int imgHeight, const int padding);
	uchar* pixelsUnmirror(int padding);

	// #################### Export ###################

	bool exportToPGM(std::string fileName, uint imgWidth, uint imgHeight, int maxValue, uchar* imgData);
	bool exportToPGM(std::string fileName, uint imgWidth, uint imgHeight, int maxValue, double* imgData, bool scaleData);

private: 
	// #################### Variables ###################

	double* m_pImgLocalData = nullptr;
	uint m_imgWidth = 0;
	uint m_imgHeight = 0;

};