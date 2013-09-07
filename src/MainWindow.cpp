#include "MainWindow.h"
#include "ui_mainwindow.h"

#include "ColorClusterFinder.h"

#include <QPainter>
#include <QPixmap>
#include <QLabel>
#include <QDesktopWidget>
#include <QRgb>
#include <QPoint>
#include <QRect>

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
    QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	
	setWindowFlags(Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_TranslucentBackground ,true ); 

	// set window-click-through
#if _WIN32
	HWND hwnd = (HWND) winId();
	LONG styles = GetWindowLong(hwnd, GWL_EXSTYLE);
	SetWindowLong(hwnd, GWL_EXSTYLE, styles | WS_EX_TRANSPARENT);
#else
	QRegion region;
	XShapeCombineRegion( QX11Info::display(), winId(), 
							ShapeInput, 0, 0, region.handle(), ShapeSet );
#endif

	// debug
	m_debugLabel = new QLabel();
	m_debugLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	m_debugLabel->show();

	// setup timer
	QObject::connect( &m_timer, SIGNAL(timeout()), this, SLOT(updateOverlay()) );
	//m_timer.start( 5000 );
	m_timer.start( 1000 );
}


void MainWindow::updateOverlay()
{
	QRect g = geometry(); // client area geometry

	QPixmap pic = QPixmap::grabWindow( QApplication::desktop()->winId(),
					g.x(), g.y(), g.width(), g.height() );

	m_debugLabel->setPixmap( pic );


	QColor colorPlayer( 4, 153, 4 ); // player-tank-green
	QColor colorEnemy( 203, 0, 0 ); // enemy-tank-red

	m_debugPoints = ColorClusterFinder::findCluster( &pic, colorEnemy.rgb() );

	vector<QPoint> tmp = ColorClusterFinder::findCluster( &pic, colorPlayer.rgb() );

	m_debugPoints.insert( m_debugPoints.end(), tmp.begin(), tmp.end() );

	repaint(); // force repaint
}

void MainWindow::paintEvent(QPaintEvent* e)
{
	// TODO draw _everything_ in this method.
	QPainter p(this);
	p.setPen( QColor( 255, 255, 0 ) );

	for (int i = 0; i < m_debugPoints.size(); i++)
	{
		cout << "Cluster at: " << m_debugPoints[i].x() << ","
				<< m_debugPoints[i].y() << endl;
		p.drawPoint( m_debugPoints[i] );
	}
	p.end();
}

MainWindow::~MainWindow()
{
    delete ui;
}
