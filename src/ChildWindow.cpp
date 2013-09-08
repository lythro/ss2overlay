#include "ChildWindow.h"
#include "ui_childwindow.h"

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

ChildWindow::ChildWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::ChildWindow)
{

	setAttribute(Qt::WA_TranslucentBackground, true ); // important: before! clickthrough!

	setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
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


	ui->setupUi(this);

	// debug
	m_debugLabel = new QLabel();
	m_debugLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	m_debugLabel->show();

	// setup timer
	QObject::connect( &m_timer, SIGNAL(timeout()), this, SLOT(updateOverlay()) );
	m_timer.start( 1000 );
}


void ChildWindow::updateOverlay()
{
	QRect g = geometry(); // client area geometry
	QPixmap pic = QPixmap::grabWindow( QApplication::desktop()->winId(),
					g.x(), g.y(), g.width(), g.height() );

	m_debugLabel->setPixmap( pic );
	
	QColor colorPlayer( 4, 153, 4 ); // player-tank-green
	QColor colorEnemy( 203, 0, 0 ); // enemy-tank-red

	m_debugPoints = ColorClusterFinder::findCluster( &pic, colorPlayer.rgb() );

	vector<QPoint> tmp;
	if (m_debugPoints.size() > 0)
	{
		tmp = ShootingAngleFinder::findAnglePoints( &pic, m_debugPoints[0] );

		m_debugPoints.insert( m_debugPoints.end(), tmp.begin(), tmp.end() );
	} else 
		cout << "no origin!" << endl;

	tmp = ColorClusterFinder::findCluster( &pic, colorEnemy.rgb() );
	m_debugPoints.insert( m_debugPoints.end(), tmp.begin(), tmp.end() );


	repaint(); // force repaint
}

void ChildWindow::paintEvent(QPaintEvent* e)
{
	QPainter p(this);

	// TODO draw _everything_ in this method.
	p.setPen( QColor( 255, 255, 0 ) );

	cout << "to draw: " << m_debugPoints.size() << endl;

	for (int i = 0; i < m_debugPoints.size(); i++)
	{
		//cout << "Cluster at: " << m_debugPoints[i].x() << ","
		//		<< m_debugPoints[i].y() << endl;
		p.drawPoint( m_debugPoints[i] );
	}
	p.end();
}

ChildWindow::~ChildWindow()
{
    delete ui;
}
