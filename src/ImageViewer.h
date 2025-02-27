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

	QImage* img_original;
	uchar* imgData_original;

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
	void on_actionOriginal_triggeted();
	void on_actionInvert_triggered();
	void on_actionMirror_triggered();
	void on_actionFSHS_triggered(); // Full Scale Histogram Strech
	void on_actionEH_triggered(); // Equalization of Histogram 
};
