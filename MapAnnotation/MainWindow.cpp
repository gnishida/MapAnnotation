#include "MainWindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);
	setCentralWidget(&canvas);

	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(onOpen()));
	connect(ui.actionOpenXML, SIGNAL(triggered()), this, SLOT(onOpenXML()));
	connect(ui.actionOpenOSM, SIGNAL(triggered()), this, SLOT(onOpenOSM()));
	connect(ui.actionSaveXML, SIGNAL(triggered()), this, SLOT(onSaveXML()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
}

MainWindow::~MainWindow() {
}

void MainWindow::keyPressEvent(QKeyEvent* e) {
	canvas.keyPressEvent(e);
}

void MainWindow::keyReleaseEvent(QKeyEvent* e) {
	canvas.keyReleaseEvent(e);
}

void MainWindow::onOpen() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open image file..."), "", tr("Image files (*.png)"));
	if (filename.isEmpty()) return;

	canvas.loadImage(filename);
	canvas.update();
}

void MainWindow::onOpenXML() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open XML file..."), "", tr("XML files (*.xml)"));
	if (filename.isEmpty()) return;

	canvas.loadXML(filename);
	canvas.update();
}

void MainWindow::onOpenOSM() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open OSM file..."), "", tr("OSM files (*.osm)"));
	if (filename.isEmpty()) return;

	canvas.loadOSM(filename);
	canvas.update();
}

void MainWindow::onSaveXML() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save XML file..."), "", tr("XML files (*.xml)"));
	if (filename.isEmpty()) return;

	canvas.saveXML(filename);
}