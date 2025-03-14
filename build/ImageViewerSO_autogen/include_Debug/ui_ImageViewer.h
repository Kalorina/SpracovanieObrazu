/********************************************************************************
** Form generated from reading UI file 'ImageViewer.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IMAGEVIEWER_H
#define UI_IMAGEVIEWER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ImageViewerClass
{
public:
    QAction *actionOpen;
    QAction *actionSave_as;
    QAction *actionClear;
    QAction *actionExit;
    QAction *actionResize;
    QAction *actionInvert;
    QAction *actionMirror;
    QAction *actionFSHS;
    QAction *actionEH;
    QAction *actionOriginal;
    QAction *actionConvolution;
    QAction *actionExplicite_Scheme;
    QAction *actionExplicit_Scheme;
    QAction *actionLinearHeatEq_Scheme;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QDoubleSpinBox *timeStepdoubleSpinBox;
    QSpinBox *stepCountspinBox;
    QLabel *label_2;
    QLabel *label;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuImage;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ImageViewerClass)
    {
        if (ImageViewerClass->objectName().isEmpty())
            ImageViewerClass->setObjectName("ImageViewerClass");
        ImageViewerClass->resize(1019, 646);
        actionOpen = new QAction(ImageViewerClass);
        actionOpen->setObjectName("actionOpen");
        actionSave_as = new QAction(ImageViewerClass);
        actionSave_as->setObjectName("actionSave_as");
        actionClear = new QAction(ImageViewerClass);
        actionClear->setObjectName("actionClear");
        actionExit = new QAction(ImageViewerClass);
        actionExit->setObjectName("actionExit");
        actionResize = new QAction(ImageViewerClass);
        actionResize->setObjectName("actionResize");
        actionInvert = new QAction(ImageViewerClass);
        actionInvert->setObjectName("actionInvert");
        actionMirror = new QAction(ImageViewerClass);
        actionMirror->setObjectName("actionMirror");
        actionFSHS = new QAction(ImageViewerClass);
        actionFSHS->setObjectName("actionFSHS");
        actionEH = new QAction(ImageViewerClass);
        actionEH->setObjectName("actionEH");
        actionOriginal = new QAction(ImageViewerClass);
        actionOriginal->setObjectName("actionOriginal");
        actionConvolution = new QAction(ImageViewerClass);
        actionConvolution->setObjectName("actionConvolution");
        actionExplicite_Scheme = new QAction(ImageViewerClass);
        actionExplicite_Scheme->setObjectName("actionExplicite_Scheme");
        actionExplicit_Scheme = new QAction(ImageViewerClass);
        actionExplicit_Scheme->setObjectName("actionExplicit_Scheme");
        actionLinearHeatEq_Scheme = new QAction(ImageViewerClass);
        actionLinearHeatEq_Scheme->setObjectName("actionLinearHeatEq_Scheme");
        centralWidget = new QWidget(ImageViewerClass);
        centralWidget->setObjectName("centralWidget");
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName("horizontalLayout");
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName("groupBox");
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName("gridLayout");
        timeStepdoubleSpinBox = new QDoubleSpinBox(groupBox);
        timeStepdoubleSpinBox->setObjectName("timeStepdoubleSpinBox");

        gridLayout->addWidget(timeStepdoubleSpinBox, 1, 2, 1, 1);

        stepCountspinBox = new QSpinBox(groupBox);
        stepCountspinBox->setObjectName("stepCountspinBox");

        gridLayout->addWidget(stepCountspinBox, 0, 2, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        label = new QLabel(groupBox);
        label->setObjectName("label");

        gridLayout->addWidget(label, 0, 0, 1, 1);


        horizontalLayout->addWidget(groupBox);

        scrollArea = new QScrollArea(centralWidget);
        scrollArea->setObjectName("scrollArea");
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 796, 560));
        scrollArea->setWidget(scrollAreaWidgetContents);

        horizontalLayout->addWidget(scrollArea);

        ImageViewerClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(ImageViewerClass);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 1019, 33));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName("menuFile");
        menuImage = new QMenu(menuBar);
        menuImage->setObjectName("menuImage");
        ImageViewerClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ImageViewerClass);
        mainToolBar->setObjectName("mainToolBar");
        ImageViewerClass->addToolBar(Qt::ToolBarArea::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(ImageViewerClass);
        statusBar->setObjectName("statusBar");
        ImageViewerClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuImage->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSave_as);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuImage->addAction(actionOriginal);
        menuImage->addAction(actionInvert);
        menuImage->addAction(actionFSHS);
        menuImage->addAction(actionEH);
        menuImage->addAction(actionConvolution);
        menuImage->addAction(actionLinearHeatEq_Scheme);

        retranslateUi(ImageViewerClass);

        QMetaObject::connectSlotsByName(ImageViewerClass);
    } // setupUi

    void retranslateUi(QMainWindow *ImageViewerClass)
    {
        ImageViewerClass->setWindowTitle(QCoreApplication::translate("ImageViewerClass", "ImageViewer", nullptr));
        actionOpen->setText(QCoreApplication::translate("ImageViewerClass", "Open", nullptr));
#if QT_CONFIG(shortcut)
        actionOpen->setShortcut(QCoreApplication::translate("ImageViewerClass", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSave_as->setText(QCoreApplication::translate("ImageViewerClass", "Save as", nullptr));
#if QT_CONFIG(shortcut)
        actionSave_as->setShortcut(QCoreApplication::translate("ImageViewerClass", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        actionClear->setText(QCoreApplication::translate("ImageViewerClass", "Clear", nullptr));
        actionExit->setText(QCoreApplication::translate("ImageViewerClass", "Exit", nullptr));
#if QT_CONFIG(shortcut)
        actionExit->setShortcut(QCoreApplication::translate("ImageViewerClass", "Alt+F4", nullptr));
#endif // QT_CONFIG(shortcut)
        actionResize->setText(QCoreApplication::translate("ImageViewerClass", "Resize", nullptr));
        actionInvert->setText(QCoreApplication::translate("ImageViewerClass", "Invert", nullptr));
        actionMirror->setText(QCoreApplication::translate("ImageViewerClass", "Convolution", nullptr));
        actionFSHS->setText(QCoreApplication::translate("ImageViewerClass", "FSHS", nullptr));
        actionEH->setText(QCoreApplication::translate("ImageViewerClass", "EH", nullptr));
        actionOriginal->setText(QCoreApplication::translate("ImageViewerClass", "Original", nullptr));
        actionConvolution->setText(QCoreApplication::translate("ImageViewerClass", "Convolution", nullptr));
        actionExplicite_Scheme->setText(QCoreApplication::translate("ImageViewerClass", "Explicite Scheme", nullptr));
        actionExplicit_Scheme->setText(QCoreApplication::translate("ImageViewerClass", "Explicit Scheme", nullptr));
        actionLinearHeatEq_Scheme->setText(QCoreApplication::translate("ImageViewerClass", "LinearHeatEq Scheme", nullptr));
        groupBox->setTitle(QCoreApplication::translate("ImageViewerClass", "Controls", nullptr));
        label_2->setText(QCoreApplication::translate("ImageViewerClass", "Time Step Size", nullptr));
        label->setText(QCoreApplication::translate("ImageViewerClass", "Time Step Count", nullptr));
        menuFile->setTitle(QCoreApplication::translate("ImageViewerClass", "File", nullptr));
        menuImage->setTitle(QCoreApplication::translate("ImageViewerClass", "Image", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ImageViewerClass: public Ui_ImageViewerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMAGEVIEWER_H
