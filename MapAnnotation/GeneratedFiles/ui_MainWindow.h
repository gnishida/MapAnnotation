/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindowClass
{
public:
    QAction *actionExit;
    QAction *actionOpenImage;
    QAction *actionOpenXML;
    QAction *actionSaveXML;
    QAction *actionOpenOSM;
    QAction *actionSaveImage;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindowClass)
    {
        if (MainWindowClass->objectName().isEmpty())
            MainWindowClass->setObjectName(QStringLiteral("MainWindowClass"));
        MainWindowClass->resize(835, 744);
        actionExit = new QAction(MainWindowClass);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionOpenImage = new QAction(MainWindowClass);
        actionOpenImage->setObjectName(QStringLiteral("actionOpenImage"));
        actionOpenXML = new QAction(MainWindowClass);
        actionOpenXML->setObjectName(QStringLiteral("actionOpenXML"));
        actionSaveXML = new QAction(MainWindowClass);
        actionSaveXML->setObjectName(QStringLiteral("actionSaveXML"));
        actionOpenOSM = new QAction(MainWindowClass);
        actionOpenOSM->setObjectName(QStringLiteral("actionOpenOSM"));
        actionSaveImage = new QAction(MainWindowClass);
        actionSaveImage->setObjectName(QStringLiteral("actionSaveImage"));
        centralWidget = new QWidget(MainWindowClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        MainWindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindowClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 835, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        MainWindowClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindowClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindowClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindowClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionOpenXML);
        menuFile->addAction(actionSaveXML);
        menuFile->addSeparator();
        menuFile->addAction(actionOpenImage);
        menuFile->addAction(actionSaveImage);
        menuFile->addAction(actionOpenOSM);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);

        retranslateUi(MainWindowClass);

        QMetaObject::connectSlotsByName(MainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindowClass)
    {
        MainWindowClass->setWindowTitle(QApplication::translate("MainWindowClass", "Map Annotation", Q_NULLPTR));
        actionExit->setText(QApplication::translate("MainWindowClass", "Exit", Q_NULLPTR));
        actionOpenImage->setText(QApplication::translate("MainWindowClass", "Open Image", Q_NULLPTR));
        actionOpenXML->setText(QApplication::translate("MainWindowClass", "Open XML", Q_NULLPTR));
        actionOpenXML->setShortcut(QApplication::translate("MainWindowClass", "Ctrl+O", Q_NULLPTR));
        actionSaveXML->setText(QApplication::translate("MainWindowClass", "Save XML", Q_NULLPTR));
        actionSaveXML->setShortcut(QApplication::translate("MainWindowClass", "Ctrl+S", Q_NULLPTR));
        actionOpenOSM->setText(QApplication::translate("MainWindowClass", "Open OSM", Q_NULLPTR));
        actionSaveImage->setText(QApplication::translate("MainWindowClass", "Save Image", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("MainWindowClass", "File", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindowClass: public Ui_MainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
