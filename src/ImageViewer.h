#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets>
#include "ui_ImageViewer.h"
#include "ViewerWidget.h"
#include "ImageProcessing.h"

class ImageViewer : public QMainWindow
{
	Q_OBJECT

public:
	ImageViewer(QWidget* parent = Q_NULLPTR);

private:
	Ui::ImageViewerClass* ui; 
	ViewerWidget* vW; 

	QSettings settings;
	QMessageBox msgBox;

	QImage img_original;

	// Linear Heat Eq

	//QSpinBox* stepCountInput = new QSpinBox(this);
	//QDoubleSpinBox* timeStepInput = new QDoubleSpinBox(this);

	// Variables Exlicit, Implicit, Semi-Implicit
	double timeStep = 0.5;
	int stepCount = 10;
	double omega = 0.5;
	double K = 2.5;

	QVector<QImage> images_ES; // linear heat eq -> explicit scheme
	QVector<QImage> images_IS; // linear heat eq -> implicit scheme
	QVector<QImage> images_SIS; // linear Diffusion -> semi-implicit scheme

	//ImageViewer Events
	void closeEvent(QCloseEvent* event);

	//Image functions
	bool openImage(QString filename);
	bool saveImage(QString filename);
	bool invertColors();

private slots:
	// Extra 
	void on_actionOpen_triggered(); /*slot zachytava signal z ViewerImage */
	void on_actionSave_as_triggered();
	void on_actionExit_triggered();
	
	// Image Functions
	void on_actionOriginal_triggered();
	void on_actionInvert_triggered();
	void on_actionFSHS_triggered(); // Full Scale Histogram Strech
	void on_actionEH_triggered(); // Equalization of Histogram 
	void on_actionConvolution_triggered(); // Convolution + Un/Mirroring
	void on_actionLinearHeatEq_Scheme_triggered(); // Explicit timeStep (tau) <0.3 else Implicit scheme
	void on_actionEdge_Detector_triggered(); //Edge Detecotor
	void on_actionSemi_Implicit_Scheme_Diffusion_triggered(); // Semi implicit Scheme Linear Diffusion

	void updateImageFromSpinBoxExplicitLH(int index);
	void updateImageFromSpinBoxImplicitLH(int index);
	void updateImageFromSpinBoxSemiImplicitDiffusion(int index);
};
