#include "MainWindow.h"

#include "sleep.h"

#include "ColorClusterFinder.h"
#include "ShootingAngleFinder.h"

#include <QPainter>
#include <QPixmap>
#include <QLabel>
#include <QDesktopWidget>
#include <QRgb>
#include <QPoint>
#include <QRect>
#include <QRegion>
#include <QPalette>

#include <iostream>
#include <vector>
using namespace std;

// used for "click-through"
#if _WIN32
    #include <windows.h>
#else
	#include <QX11Info>
	#include <X11/Xlib.h>
	#include <X11/extensions/shape.h>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow),
	m_child()
{
	ui->setupUi(this);

	// delete the content
	QRegion maskedRegion(0, 0, width(), 1 );
    setMask(maskedRegion);

	
	setWindowFlags(Qt::WindowStaysOnTopHint);

	m_child.show();
}

void MainWindow::resizeEvent(QResizeEvent* e)
{
	m_child.resize(size());
	moveEvent(NULL);
}

void MainWindow::moveEvent(QMoveEvent* e)
{
	QRect g = frameGeometry();
	m_child.move( g.x(), g.y() );
}

void MainWindow::closeEvent(QCloseEvent* e)
{
	QApplication::quit();
}

MainWindow::~MainWindow()
{
    delete ui;
}
