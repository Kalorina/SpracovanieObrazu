#include "ImageViewer.h"

ImageViewer::ImageViewer(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::ImageViewerClass)
{
	ui->setupUi(this);
	vW = new ViewerWidget(QSize(500, 500));
	ui->scrollArea->setWidget(vW);

	ui->scrollArea->setBackgroundRole(QPalette::Dark);
	ui->scrollArea->setWidgetResizable(true);
	ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	ui->stepCountspinBox->setValue(10);
	ui->timeStepdoubleSpinBox->setValue(0.2);

	vW->setObjectName("ViewerWidget");
}

//ImageViewer Events
void ImageViewer::closeEvent(QCloseEvent* event)
{
	if (QMessageBox::Yes == QMessageBox::question(this, "Close Confirmation", "Are you sure you want to exit?", QMessageBox::Yes | QMessageBox::No))
	{
		event->accept();
	}
	else {
		event->ignore();
	}
}

//Image functions
bool ImageViewer::openImage(QString filename)
{
	QImage loadedImg(filename);
	if (!loadedImg.isNull()) {
		img_original = loadedImg; // saving original data
		return vW->setImage(loadedImg); // obr zostane iba ako kopia vo ViewerWidget
	}

	return false;
}
bool ImageViewer::saveImage(QString filename)
{
	QFileInfo fi(filename);
	QString extension = fi.completeSuffix();

	QImage* img = vW->getImage();
	return img->save(filename, extension.toStdString().c_str());
}

bool ImageViewer::invertColors()
{
	if (vW->isEmpty()) {
		return false;
	}

	uchar* data = vW->getData(); //kopia smernika na data obr

	//PGM Grayscale -> 1 bite = 8 bytov, RGB viac bitov - trojice, stvorice

	int row = vW->getImage()->bytesPerLine(); //bytes in row a aky farbeny model
	int depth = vW->getImage()->depth();

	for (int i = 0; i < vW->getImgHeight(); i++)
	{
		for (int j = 0; j < vW->getImgWidth(); j++)
		{
			//Grayscale -> na cviceni hlavne 1 kanal obr (povinne)
			if (depth == 8) {
				// function of viewerImage -> controla + nastavenie hodnoty
				vW->setPixel(j, i, static_cast<uchar>(255 - data[i * row + j]));
			}
			//RGBA
			else {
				uchar r = static_cast<uchar>(255 - data[i * row + j * 4]);
				uchar g = static_cast<uchar>(255 - data[i * row + j * 4 + 1]);
				uchar b = static_cast<uchar>(255 - data[i * row + j * 4 + 2]);
				vW->setPixel(j, i, r, g, b);
			}
		}
	}
	vW->update(); // prekreslenie ViewerImage
	return true;
}

// Slots
// Extra
void ImageViewer::on_actionOpen_triggered()
{
	QString folder = settings.value("folder_img_load_path", "").toString();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm .*xbm .* xpm);;All files (*)";
	QString fileName = QFileDialog::getOpenFileName(this, "Load image", folder, fileFilter);
	if (fileName.isEmpty()) { return; }

	QFileInfo fi(fileName);
	settings.setValue("folder_img_load_path", fi.absoluteDir().absolutePath());

	if (!openImage(fileName)) {
		msgBox.setText("Unable to open image.");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
}
void ImageViewer::on_actionSave_as_triggered()
{
	QString folder = settings.value("folder_img_save_path", "").toString();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm .*xbm .* xpm);;All files (*)";
	QString fileName = QFileDialog::getSaveFileName(this, "Save image", folder, fileFilter);
	if (!fileName.isEmpty()) {
		QFileInfo fi(fileName);
		settings.setValue("folder_img_save_path", fi.absoluteDir().absolutePath());

		if (!saveImage(fileName)) {
			msgBox.setText("Unable to save image.");
			msgBox.setIcon(QMessageBox::Warning);
		}
		else {
			msgBox.setText(QString("File %1 saved.").arg(fileName));
			msgBox.setIcon(QMessageBox::Information);
		}
		msgBox.exec();
	}
}
void ImageViewer::on_actionExit_triggered()
{
	this->close();
}

// Image Functions 
void ImageViewer::on_actionOriginal_triggered()
{
	if (vW->isEmpty()) {
		false;
	}
	vW->setImage(img_original);
	vW->update();
}
void ImageViewer::on_actionInvert_triggered()
{
	invertColors();
}
void ImageViewer::on_actionFSHS_triggered()
{
	if (vW->isEmpty()) {
		false;
	}

	ImageProcessing IPmodul;
	QImage new_img;
	new_img = IPmodul.FSHS(*vW->getImage());
	vW->setImage(new_img);
	vW->update();
}
void ImageViewer::on_actionEH_triggered()
{
	if (vW->isEmpty()) {
		false;
	}

	ImageProcessing IPmodul;
	QImage new_img;
	new_img = IPmodul.EH(*vW->getImage());
	vW->setImage(new_img);
	vW->update();
}
void ImageViewer::on_actionConvolution_triggered()
{
	if (vW->isEmpty()) {
		false;
	}

	ImageProcessing IPmodul;
	QImage new_img;
	new_img = IPmodul.Convolution(*vW->getImage(), 2);
	vW->setImage(new_img);
	vW->update();
}
void ImageViewer::on_actionLinearHeatEq_Scheme_triggered()
{
	if (vW->isEmpty()) false;

	stepCount = ui->stepCountspinBox->value();  
	timeStep = ui->timeStepdoubleSpinBox->value();

	qDebug() << "Step Count:" << stepCount;
	qDebug() << "Time Step:" << timeStep;

	ImageProcessing IPmodul;
	if (timeStep > 0.2) {
		QVector<QImage> new_imgs = IPmodul.schemeImplicit(*vW->getImage(), stepCount, timeStep);
		if (!new_imgs.isEmpty()) { 
			qDebug() << "Showing last solution of T=" << stepCount;
			vW->setImage(new_imgs.last());
			vW->update();
		}
	}
	else {
		QVector<QImage> new_imgs = IPmodul.schemeExplicit(*vW->getImage(), stepCount, timeStep);
		if (!new_imgs.isEmpty()) {
			qDebug() << "Showing last solution of T=" << stepCount;
			vW->setImage(new_imgs.last());
			vW->update();
		}
	}
}
