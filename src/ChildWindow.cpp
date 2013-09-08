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

#include <cmath>
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
	//m_debugLabel = new QLabel();
	//m_debugLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	//m_debugLabel->show();

	m_angle = 0;

	// setup timer
	QObject::connect( &m_timer, SIGNAL(timeout()), this, SLOT(updateOverlay()) );
	m_timer.start( 1000 );
}


void ChildWindow::updateOverlay()
{
	QRect g = geometry(); // client area geometry
	QPixmap pic = QPixmap::grabWindow( QApplication::desktop()->winId(),
					g.x(), g.y(), g.width(), g.height() );
	
	
	QColor colorPlayer( 4, 153, 4 ); // player-tank-green
	QColor colorEnemy( 203, 0, 0 ); // enemy-tank-red

	m_debugPoints.clear();
	m_enemyTargetInfos.clear();

	vector<QPoint> my_position = ColorClusterFinder::findCluster( &pic, colorPlayer.rgb() );

	if (my_position.size() > 1 || my_position.empty())
	{
		cout << "player position not distinct" << endl;
	}
	else
	{
		m_playerPosition = my_position[0];
		m_debugPoints.push_back( my_position[0] );

		bool ok;
		float angle = ShootingAngleFinder::findAngle( &pic, my_position[0], ok );

		//vector<QPoint> tmp = ShootingAngleFinder::findAnglePoints( &pic, my_position[0] );
		//m_debugPoints.insert( m_debugPoints.end(), tmp.begin(), tmp.end() );

		if (ok)
		{
			vector <QPoint> enemies = ColorClusterFinder::findCluster( &pic, colorEnemy.rgb(), m_playerPosition );
			
			m_debugPoints.insert( m_debugPoints.end(), enemies.begin(), enemies.end() );

			m_angle = (int)(angle + 0.5);

			cout << "angle: " << m_angle << endl;

			for (int i = 0; i < enemies.size(); i++)
			{
				int dx = enemies[i].x() - my_position[0].x();
				int dy = enemies[i].y() - my_position[0].y();

				int miss = -1;
				int power = ShootingAngleFinder::calculatePower( dx, dy, m_angle, miss );

				cout << "for enemy " << i << " : " << power << ", with miss: " << miss << endl;

				TargetInfo t;
				t.pos = enemies[i];
				t.velocity = power;
				t.miss = miss;

				m_enemyTargetInfos.push_back( t );
			}
		}
		else
		{
			cout << "couldn't find shooting angle" << endl;
		}
	}



	repaint(); // force repaint
}

void ChildWindow::paintEvent(QPaintEvent* e)
{
	QPainter p(this);
	p.setPen( QColor( 255, 255, 0 ) );

	//cout << "to draw: " << m_debugPoints.size() << endl;

	for (int i = 0; i < m_debugPoints.size(); i++)
	{
		//cout << "Cluster at: " << m_debugPoints[i].x() << ","
		//		<< m_debugPoints[i].y() << endl;
		p.drawPoint( m_debugPoints[i] );
	}

	// draw found angle at players position
	QPoint pPos = m_playerPosition - QPoint( 20, -20 );
	p.drawText( pPos, QString( "Angle: " ) + QString::number( m_angle ) );

	// draw target infos!
	for (int i = 0; i < m_enemyTargetInfos.size(); i++)
	{
		TargetInfo t = m_enemyTargetInfos[i];

		QPoint drawPos = t.pos - QPoint( 20, -30 );

		if (drawPos.x() < 10) drawPos.rx() = 10;
		if (drawPos.x() > width() - 60) drawPos.rx() = width() - 60;

		if (drawPos.y() > height() - 30) drawPos.ry() = t.pos.y() - 30;


		p.drawText( drawPos, QString( "Vel.: " ) + QString::number( t.velocity ) );
		drawPos.ry() += 10;

		p.drawText( drawPos, QString( "Miss: " ) + QString::number( t.miss ) );

	}

	p.end();
}

ChildWindow::~ChildWindow()
{
    delete ui;
}
